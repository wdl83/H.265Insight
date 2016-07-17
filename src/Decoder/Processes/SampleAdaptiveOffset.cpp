#include <Decoder/Processes/SampleAdaptiveOffset.h>
#include <Decoder/Processes/CtbMask.h>
#include <Syntax/SequenceParameterSet.h>
#include <Syntax/PictureParameterSet.h>
#include <Syntax/SliceSegmentHeader.h>
#include <Syntax/CodingTreeUnit.h>
#include <Syntax/CodingQuadTree.h>
#include <Syntax/SAO.h>
#include <Syntax/CodingUnit.h>
#include <Structure/Picture.h>
#include <Structure/PelBuffer.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
namespace {
/*----------------------------------------------------------------------------*/
enum class FilterType
{
    Undefined = 0, NotApplied, Band, Edge0, Edge90, Edge135, Edge45
};

inline
const std::string &getName(FilterType type)
{
    static const std::string name[] =
    {
        "?", "NA", "BAND", "E0", "E90", "E135", "E45"
    };

    return name[int(type)];
}

inline
bool isBandFilter(FilterType type)
{
    return FilterType::Band == type;
}

inline
bool isEdgeFilter(FilterType type)
{
    return
        FilterType::Edge0 == type
        || FilterType::Edge90 == type
        || FilterType::Edge135 == type
        || FilterType::Edge45 == type;
}

inline
SaoEdgeOffsetClass toSaoEdgeOffsetClass(FilterType type)
{
    runtime_assert(isEdgeFilter(type));
    return static_cast<SaoEdgeOffsetClass>(int(type) - int(FilterType::Edge0));
}

inline
FilterType toFilterType(SaoEdgeOffsetClass eoClass)
{
    return static_cast<FilterType>(int(FilterType::Edge0) + int(eoClass));
}
/*----------------------------------------------------------------------------*/
static const std::array<PelCoord, 4> saoEoClassA =
{
    {
        {-1_pel, 0_pel}, // 0
        {0_pel, -1_pel}, // 90
        {-1_pel, -1_pel}, // 135
        {1_pel, -1_pel} // 45
    }
};

static const std::array<PelCoord, 4> saoEoClassB =
{
    {
        {1_pel, 0_pel}, // 0
        {0_pel, 1_pel}, // 90
        {1_pel, 1_pel}, // 135
        {-1_pel, 1_pel} // 45
    }
};
/*----------------------------------------------------------------------------*/
typedef Syntax::SAO::SaoOffsetVal::IndexToOffset IndexToOffset;

inline
void applyEdgeOffset(
        int bitDepth,
        SaoEdgeOffsetClass eoClass,
        const IndexToOffset &indexToOffset,
        PelCoord at,
        const Structure::PelBuffer &src,
        Structure::PelBuffer &dst)
{
    const auto curr = src[at];
    const auto adjA = src[at + saoEoClassA[int(eoClass)]];
    const auto adjB = src[at + saoEoClassB[int(eoClass)]];

    int edgeIndex = 2 + sign(curr - adjA) + sign(curr - adjB);

    edgeIndex =
        2 == edgeIndex
        ? 0
        : (
                1 == edgeIndex || 0 == edgeIndex
                ? edgeIndex + 1
                : edgeIndex);

    const auto offset = indexToOffset[edgeIndex];

    dst[at] = clip3(0, (1 << bitDepth ) - 1, curr + offset);
}

inline
void applyBandOffset(
        int bitDepth,
        int bandPosition,
        const IndexToOffset &indexToOffset,
        PelCoord at,
        const Structure::PelBuffer &src,
        Structure::PelBuffer &dst)
{
    std::array<int, 32> bandTable =
    {
        {
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
        }
    };

    for(auto k = 0; k < 4; ++k)
    {
        bandTable[(k + bandPosition) & 31] = k + 1;
    }

    const auto curr = src[at];
    const auto bandShift = bitDepth - 5;
    const auto bandIdx = bandTable[curr >> bandShift];

    dst[at] = clip3(0, (1 << bitDepth) - 1, curr + indexToOffset[bandIdx]);
}
/*----------------------------------------------------------------------------*/
typedef std::function<bool (PelCoord)> InSameTilePredicate;
typedef std::function<bool (PelCoord)> InSameSlicePredicate;
typedef std::function<MinTb (PelCoord)> ToMinTbAddrInZs;
typedef std::function<bool (PelCoord)> IsFilterAcrossSlicesEnabledPredicate;

/* see SaoEdgeOffsetClass definition for A/C/B explenation
 *
 * requires: C - to be valid & available
 * returns: A & B availability status */
bool isAvailable(
        /* picture level */
        PelRect pictureBdry,
        bool filterAcrossTilesEnabled,
        /* tile level */
        InSameTilePredicate inSameTile,
        /* slice level */
        InSameSlicePredicate inSameSlice,
        IsFilterAcrossSlicesEnabledPredicate isFilterAcrossSlicesEnabled,
        // current Slice/CTU flag
        bool filterAcrossSlicesEnabled,
        /* MinTb level */
        MinTb addrInZs,
        ToMinTbAddrInZs toMinTbAddrInZs,
        PelCoord coordC, PelCoord coordA, PelCoord coordB)
{
    /* 04/2013, 8.7.3.2 "Coding tree block modification process"
     * when: SaoTypeIdx[cIdx][rx][ry] is equal to 2 (edge offset) */
    bdryCheck(pictureBdry.encloses(coordC));

    if(!pictureBdry.encloses(coordA) || !pictureBdry.encloses(coordB))
    {
        return false;
    }

    if(
            !inSameSlice(coordA)
            && (
                !filterAcrossSlicesEnabled
                && toMinTbAddrInZs(coordA) < toMinTbAddrInZs(coordC)
                ||
                !isFilterAcrossSlicesEnabled(coordA)
                && toMinTbAddrInZs(coordC) < toMinTbAddrInZs(coordA)))
    {
        return false;
    }

    if(
            !inSameSlice(coordB)
            && (
                !filterAcrossSlicesEnabled && toMinTbAddrInZs(coordB) < addrInZs
                ||
                !isFilterAcrossSlicesEnabled(coordB) && addrInZs < toMinTbAddrInZs(coordB)))
    {
        return false;
    }

    if(!filterAcrossTilesEnabled && !inSameTile(coordA))
    {
        return false;
    }

    if(!filterAcrossTilesEnabled && !inSameTile(coordB))
    {
        return false;
    }

    return true;
}
/*----------------------------------------------------------------------------*/
typedef CtbMask Mask;
typedef std::function<bool (PelCoord)> MaskPredicate;

inline
void set(Mask &mask, PelCoord base, Log2 hSize, Log2 vSize)
{
    const auto hSide = toInt(hSize);
    const auto vSide = toInt(vSize);
    const auto xBase = toUnderlying(base.x());
    const auto yBase = toUnderlying(base.y());

    for(auto y = 0; y < vSide; ++y)
    {
        for(auto x = 0; x < hSide; ++x)
        {
            mask[{xBase + x, yBase + y}] = true;
        }
    }
}

inline
void vSet(Mask &mask, PelCoord base, Pel side, MaskPredicate predicate)
{
    const auto xBase = toUnderlying(base.x());
    const auto yBase = toUnderlying(base.y());

    for(auto y = 0; y < toUnderlying(side); ++y)
    {
        mask[{xBase, yBase + y}] = predicate({base.x(), base.y() + Pel{y}});
    }
}

inline
void hSet(Mask &mask, PelCoord base, Pel side, MaskPredicate predicate)
{
    const auto xBase = toUnderlying(base.x());
    const auto yBase = toUnderlying(base.y());

    for(auto x = 0; x < toUnderlying(side); ++x)
    {
        mask[{xBase + x, yBase}] = predicate({base.x() + Pel{x}, base.y()});
    }
}
/*----------------------------------------------------------------------------*/
void applyFilter(
        Ptr<Structure::Picture> picture,
        const Syntax::CodingTreeUnit &ctu,
        const Syntax::SAO &sao,
        Plane plane,
        FilterType filterType,
        const Mask &mask)
{
    using namespace Syntax;

    typedef SequenceParameterSet SPS;
    typedef CodingTreeUnit CTU;
    typedef CodingUnit CU;

    const auto chromaFormatIdc = picture->chromaFormatIdc;
    const auto bitDepth = picture->bitDepth(plane);
    const auto ctuCoord = ctu.get<CTU::Coord>()->inUnits();
    const auto ctuSize = picture->ctbSizeY;
    const auto hSide = std::min(toPel(ctuSize), picture->widthInLumaSamples - ctuCoord.x());
    const auto vSide = std::min(toPel(ctuSize), picture->heightInLumaSamples - ctuCoord.y());
    const auto coordInPlane = scale(ctuCoord, plane, chromaFormatIdc);
    const auto hSideInPlane = hScale(hSide, plane, chromaFormatIdc);
    const auto vSideInPlane = vScale(vSide, plane, chromaFormatIdc);
    const auto &src = picture->pelBuffer(PelLayerId::Deblocked, plane);
    auto &dst = picture->pelBuffer(PelLayerId::Decoded, plane);
    const auto indexToOffset = (*sao.get<SAO::SaoOffsetVal>())[plane];

    if(isEdgeFilter(filterType))
    {
        const auto eoClass = toSaoEdgeOffsetClass(filterType);

        for(auto y = 0_pel; y < vSideInPlane; ++y)
        {
            for(auto x = 0_pel; x < hSideInPlane; ++x)
            {
                if(!mask[{toUnderlying(x), toUnderlying(y)}])
                {
                    applyEdgeOffset(
                            bitDepth,
                            eoClass,
                            indexToOffset,
                            {coordInPlane.x() + x, coordInPlane.y() + y},
                            src, dst);
                }
            }
        }
    }
    else if(isBandFilter(filterType))
    {
        const auto bandPosition = (*sao.get<Syntax::SAO::SaoBandPosition>())[plane];

        for(auto y = 0_pel; y < vSideInPlane; ++y)
        {
            for(auto x = 0_pel; x < hSideInPlane; ++x)
            {
                if(!mask[{toUnderlying(x), toUnderlying(y)}])
                {
                    applyBandOffset(
                            bitDepth,
                            bandPosition,
                            indexToOffset,
                            {coordInPlane.x() + x, coordInPlane.y() + y},
                            src, dst);
                }
            }
        }
    }
}
/*----------------------------------------------------------------------------*/
Mask deriveMask(
        Ptr<const Structure::Picture> picture,
        const Syntax::CodingTreeUnit &ctu,
        Component component,
        FilterType filterType,
        MaskPredicate isNotAvailable)
{
    using namespace Syntax;

    typedef SequenceParameterSet SPS;
    typedef CodingTreeUnit CTU;
    typedef CodingUnit CU;

    const auto sps = picture->sps;
    const auto chromaFormatIdc = picture->chromaFormatIdc;
    const bool pcmLoopFilterDisabledFlag(*sps->get<SPS::PcmLoopFilterDisabledFlag>());
    const auto ctuCoord = ctu.get<CTU::Coord>()->inUnits();
    const auto ctuSize = picture->ctbSizeY;

    const auto hSide =
        hScale(
                std::min(toPel(ctuSize), picture->widthInLumaSamples - ctuCoord.x()),
                component,
                chromaFormatIdc);

    const auto vSide =
        vScale(
                std::min(toPel(ctuSize), picture->heightInLumaSamples - ctuCoord.y()),
                component,
                chromaFormatIdc);

    Mask mask;

    runtime_assert(isEdgeFilter(filterType) || isBandFilter(filterType));

    if(isEdgeFilter(filterType))
    {
        /* mask CTU boundaries (left/right/top/bottom)
         * TODO: optimize */
        vSet(mask, {0_pel, 0_pel}, vSide, isNotAvailable);
        vSet(mask, {hSide - 1_pel, 0_pel}, vSide, isNotAvailable);
        hSet(mask, {1_pel, 0_pel}, hSide - 1_pel, isNotAvailable);
        hSet(mask, {1_pel, vSide - 1_pel}, hSide - 1_pel, isNotAvailable);
    }

    const auto cqtList = ctu.getCodingQuadTreeListInZ();

    for(auto cqt : cqtList)
    {
        const auto cu = cqt->getCodingUnit();

        if(
                cu
                && (
                    cu->isCuTransQuantBypass()
                    || pcmLoopFilterDisabledFlag && cu->isPCM()))
        {
            const auto cuCoord = cu->get<CU::Coord>()->inUnits();
            const auto cuSize = cu->get<CU::Size>()->inUnits();

            set(
                    mask,
                    scale(cuCoord - ctuCoord, component, chromaFormatIdc),
                    hScale(cuSize, component, chromaFormatIdc),
                    vScale(cuSize, component, chromaFormatIdc));
        }
    }

    return mask;
}
/*----------------------------------------------------------------------------*/
FilterType deriveFilterType(const Syntax::SAO &sao, Component component)
{
    using namespace Syntax;

    const auto saoCoord = sao.get<SAO::Coord>()->inUnits();
    const auto saoType = (*sao.get<SAO::SaoTypeIdx>())[component];
    FilterType filterType = FilterType::Undefined;

    if(SaoType::EdgeOffset == saoType)
    {
        const auto eoClass = (*sao.get<Syntax::SAO::SaoEoClass>())[component];

        filterType = toFilterType(eoClass);
    }
    else if(SaoType::BandOffset == saoType)
    {
        filterType = FilterType::Band;
    }
    else if(SaoType::NotApplied == saoType)
    {
        filterType = FilterType::NotApplied;
    }

    runtime_assert(FilterType::Undefined != filterType);

    const auto toStr =
        [saoCoord, filterType](std::ostream &oss)
        {
            oss << saoCoord << ' ' << getName(filterType) << '\n';
        };

    const LogId logs[] =
    {
        LogId::SaoCtbMaskLuma,
        LogId::SaoCtbMaskChroma
    };

    log(logs[int(component)], toStr);

    return filterType;
}
/*----------------------------------------------------------------------------*/
void copy(
        Structure::Picture &picture,
        Plane plane, PelCoord base, Pel hSide, Pel vSide)
{
    const auto &src = picture.pelBuffer(PelLayerId::Deblocked, plane);
    auto &dst = picture.pelBuffer(PelLayerId::Decoded, plane);

    const auto hEnd =
        clip(makeRange(0_pel, picture.widthInPels(plane)), base.x() + hSide);
    const auto vEnd =
        clip(makeRange(0_pel, picture.heightInPels(plane)), base.y() + vSide);

    for(auto y = base.y(); y < vEnd; ++y)
    {
        for(auto x = base.x(); x < hEnd; ++x)
        {
            dst[{x, y}] = src[{x, y}];
        }
    }
}
/*----------------------------------------------------------------------------*/
} /* namespace */
/*----------------------------------------------------------------------------*/
void SampleAdaptiveOffset::exec(
        State &, Ptr<Structure::Picture> picture,
        const Syntax::CodingTreeUnit &ctu)
{
    using namespace Syntax;

    typedef SequenceParameterSet SPS;
    typedef PictureParameterSet PPS;
    typedef SliceSegmentHeader SSH;
    typedef CodingTreeUnit CTU;

    const auto chromaFormatIdc = picture->chromaFormatIdc;
    const auto ctuCoord = ctu.get<CTU::Coord>()->inUnits();
    const auto ctuSize = picture->ctbSizeY;
    const auto addrInZs = picture->toAddrInScanZ(ctuCoord);
    const auto pps = picture->pps;
    const auto slice = picture->slice(ctuCoord);
    const auto sh = slice->header();

    for(auto plane : EnumRange<Plane>())
    {
        copy(
                *picture,
                plane,
                scale(ctuCoord, plane, chromaFormatIdc),
                hScale(toPel(ctuSize), plane, chromaFormatIdc),
                vScale(toPel(ctuSize), plane, chromaFormatIdc));
    }

    const bool filterAcrossTilesEnabled(*pps->get<PPS::LoopFilterAcrossTilesEnabledFlag>());
    const bool filterAcrossSlicesEnabled(*sh->get<SSH::SliceLoopFilterAcrossSlicesEnabledFlag>());

    const auto inSameTile =
        [picture, ctuCoord](PelCoord adj)
        {
            return picture->tileId(ctuCoord) == picture->tileId(adj);
        };

    const auto inSameSlice =
        [picture, slice](PelCoord adj)
        {
            const auto addrInRs = slice->addr().inRs;
            const auto adjAddrInRs = picture->slice(adj)->addr().inRs;
            return addrInRs == adjAddrInRs;
        };

    const auto isFilterAcrossSlicesEnabled =
        [picture](PelCoord at)
        {
            const auto ssh = picture->slice(at)->header();
            return bool(*ssh->get<SSH::SliceLoopFilterAcrossSlicesEnabledFlag>());
        };

    const auto toMinTbAddrInZs =
        [picture](PelCoord coord)
        {
            return picture->toAddrInScanZ(coord);
        };

    if(*sh->get<SliceSegmentHeader::SliceSaoLumaFlag>())
    {
        const auto sao = ctu.getSubtree<SAO>();
        const auto filterType = deriveFilterType(*sao, Component::Luma);

        if(isEdgeFilter(filterType) || isBandFilter(filterType))
        {
            const auto pictureBdry = picture->boundaries();
            const auto isNotAvailable =
                [
                ctuCoord,
                pictureBdry,
                inSameTile,
                inSameSlice,
                isFilterAcrossSlicesEnabled,
                addrInZs,
                toMinTbAddrInZs,
                filterAcrossTilesEnabled,
                filterAcrossSlicesEnabled,
                filterType](PelCoord offset)
                {
                    const auto eoClass = toSaoEdgeOffsetClass(filterType);
                    const auto offsetC = offset;
                    const auto offsetA = offset + saoEoClassA[int(eoClass)];
                    const auto offsetB = offset + saoEoClassB[int(eoClass)];

                    return
                        !isAvailable(
                                pictureBdry,
                                filterAcrossTilesEnabled,
                                inSameTile,
                                inSameSlice,
                                isFilterAcrossSlicesEnabled,
                                filterAcrossSlicesEnabled,
                                addrInZs,
                                toMinTbAddrInZs,
                                ctuCoord + offsetC,
                                ctuCoord + offsetA,
                                ctuCoord + offsetB);
                };

            const auto mask = deriveMask(picture, ctu, Component::Luma, filterType, isNotAvailable);

            const auto toStr =
                [ctuSize, &mask](std::ostream &oss)
                {
                    mask.toStr(oss, ctuSize, ctuSize);
                };

            log(LogId::SaoCtbMaskLuma, toStr);
            applyFilter(picture, ctu, *sao, Plane::Y, filterType, mask);
        }
    }

    if(*sh->get<SliceSegmentHeader::SliceSaoChromaFlag>())
    {
        const auto sao = ctu.getSubtree<SAO>();
        const auto filterType = deriveFilterType(*sao, Component::Chroma);

        if(isEdgeFilter(filterType) || isBandFilter(filterType))
        {
            const auto pictureBdry = picture->boundaries();
            const auto isNotAvailable =
                [
                ctuCoord,
                pictureBdry,
                chromaFormatIdc,
                inSameTile,
                inSameSlice,
                isFilterAcrossSlicesEnabled,
                addrInZs,
                toMinTbAddrInZs,
                filterAcrossTilesEnabled,
                filterAcrossSlicesEnabled,
                filterType](PelCoord offset)
                {
                    const auto eoClass = toSaoEdgeOffsetClass(filterType);
                    const auto offsetC = offset;
                    const auto offsetA = offset + saoEoClassA[int(eoClass)];
                    const auto offsetB = offset + saoEoClassB[int(eoClass)];

                    return
                        !isAvailable(
                                pictureBdry,
                                filterAcrossTilesEnabled,
                                inSameTile,
                                inSameSlice,
                                isFilterAcrossSlicesEnabled,
                                filterAcrossSlicesEnabled,
                                addrInZs,
                                toMinTbAddrInZs,
                                ctuCoord + invScale(offsetC, Component::Chroma, chromaFormatIdc),
                                ctuCoord + invScale(offsetA, Component::Chroma, chromaFormatIdc),
                                ctuCoord + invScale(offsetB, Component::Chroma, chromaFormatIdc));
               };
            const auto mask = deriveMask(picture, ctu, Component::Chroma, filterType, isNotAvailable);

            const auto toStr =
                [chromaFormatIdc, ctuSize, &mask](std::ostream &oss)
                {
                    mask.toStr(
                            oss,
                            hScale(ctuSize, Component::Chroma, chromaFormatIdc),
                            vScale(ctuSize, Component::Chroma, chromaFormatIdc));
                };

            log(LogId::SaoCtbMaskChroma, toStr);

            applyFilter(picture, ctu, *sao, Plane::Cb, filterType, mask);
            applyFilter(picture, ctu, *sao, Plane::Cr, filterType, mask);
        }
    }

    const auto toStr =
        [picture, chromaFormatIdc, ctuCoord, ctuSize](std::ostream &oss, Plane plane, PelLayerId layerId)
        {
            const auto picWidth = picture->widthInPels(toComponent(plane));
            const auto picHeight = picture->heightInPels(toComponent(plane));
            const auto coord = scale(ctuCoord, plane, chromaFormatIdc);
            const auto hSide = hScale(toPel(ctuSize), plane, chromaFormatIdc);
            const auto vSide = vScale(toPel(ctuSize), plane, chromaFormatIdc);
            const auto w = std::min(hSide, picWidth - coord.x());
            const auto h = std::min(vSide, picHeight - coord.y());
            const auto &src = picture->pelBuffer(layerId, plane);

            oss << ctuCoord << '\n';
            src.toStr(oss, {coord, w, h});
        };

    for(auto plane : EnumRange<Plane>())
    {

        if(!isPresent(plane, chromaFormatIdc))
        {
            continue;
        }

        const LogId deblocked[] =
        {
            LogId::DeblockedY, LogId::DeblockedCb, LogId::DeblockedCr,
        };

        const LogId decoded[] =
        {
            LogId::DecodedY, LogId::DecodedCb, LogId::DecodedCr,
        };

        log(
                deblocked[int(plane)],
                    [toStr, plane](std::ostream &oss)
                    {
                        return toStr(oss, plane, PelLayerId::Deblocked);
                    });

        log(
                decoded[int(plane)],
                    [toStr, plane](std::ostream &oss)
                    {
                        return toStr(oss, plane, PelLayerId::Decoded);
                    });
    }
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */
