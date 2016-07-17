#include <Structure/Picture.h>
#include <Syntax/VideoParameterSet.h>
#include <Syntax/SequenceParameterSet.h>
#include <Syntax/PictureParameterSet.h>
#include <Syntax/SliceSegmentHeader.h>
#include <Syntax/CodingTreeUnit.h>
#include <Syntax/CodingQuadTree.h>
#include <Syntax/SAO.h>
#include <Syntax/CodingUnit.h>
#include <Syntax/PredictionUnit.h>
#include <Syntax/TransformTree.h>
#include <Syntax/TransformUnit.h>
#include <MD5Hasher.h>
/* STDC++ */
#include <fstream>

namespace HEVC { namespace Structure {
/*----------------------------------------------------------------------------*/
/* Picture */
/*----------------------------------------------------------------------------*/
Picture::Picture(
        Ptr<const VPS> vps,
        Ptr<const SPS> sps,
        Ptr<const PPS> pps,
        NalUnitType nalUnitType,
        bool noRaslOutputFlag,
        int64_t decodingNo_):
    PictureProperties(vps, sps, pps, nalUnitType, noRaslOutputFlag),
    decodingNo{decodingNo_},
    outputNo{-1},
    reference{RefPicType::ShortTerm, PicOrderCntVal{}}
{
    m_tile.reserve(getNumTiles());

    for(int y = 0, id = 0; y < getNumTileRows(); ++y)
    {
        for(int x = 0; x < getNumTileColumns(); ++x)
        {
            m_tile.emplaceBack(id, tileAddrBdryInTs(id));
            ++id;
        }
    }

    m_ctu.reserve(toUnderlying(sizeInCtbsY));

    for(auto &pelLayer : m_pelLayers)
    {
        for(const auto plane : EnumRange<Plane>())
        {
            const auto width = hScale(widthInLumaSamples, plane, chromaFormatIdc);
            const auto height = vScale(heightInLumaSamples, plane, chromaFormatIdc);

            if(isPresent(plane, chromaFormatIdc))
            {
                pelLayer[int(plane)].resize(width, height);
            }
        }
    }

    m_rps.construct();
}
/*----------------------------------------------------------------------------*/
void Picture::add(CtbAddr addr, Ptr<SSH> ssh, Ptr<SSD> ssd)
{
    using namespace Syntax;

    typedef SliceSegmentHeader SSH;

    m_slice.back()->add(addr, ssh, ssd);
}
/*----------------------------------------------------------------------------*/
void Picture::store(Syntax::CABAD::RestorePoint point)
{
    log(LogId::ArithmeticDecoderState, "Picture::store(", getName(point), ")\n");

    m_cabadState[int(point)] = m_activeCabadState;
}
/*----------------------------------------------------------------------------*/
void Picture::restore(
        Syntax::CABAD::RestorePoint point, StreamAccessLayer &streamAccessLayer)
{
    log(LogId::ArithmeticDecoderState, "Picture::restore(", getName(point), ")\n");

    m_activeCabadState = *m_cabadState[int(point)];
    m_activeCabadState.arithmeticDecoder.init(streamAccessLayer);
}
/*----------------------------------------------------------------------------*/
Ptr<Syntax::SAO> Picture::getSAO(PelCoord at) const
{
    return getCodingTreeUnit(at)->getSAO();
}
/*----------------------------------------------------------------------------*/
Ptr<Syntax::CodingQuadTree> Picture::getCodingQuadTree(PelCoord at) const
{
    return getCodingTreeUnit(at)->getCodingQuadTree(at);
}
/*----------------------------------------------------------------------------*/
Ptr<Syntax::CodingUnit> Picture::getCodingUnit(PelCoord at) const
{
    return getCodingQuadTree(at)->getCodingUnit();
}
/*----------------------------------------------------------------------------*/
Ptr<Syntax::PredictionUnit> Picture::getPredictionUnit(PelCoord at) const
{
    return getCodingUnit(at)->getPredictionUnit(at);
}
/*----------------------------------------------------------------------------*/
Ptr<Syntax::TransformTree> Picture::getTransformTree(PelCoord at) const
{
    return getCodingUnit(at)->getTransformTree(at);
}
/*----------------------------------------------------------------------------*/
Ptr<Syntax::TransformUnit> Picture::getTransformUnit(PelCoord at) const
{
    return getTransformTree(at)->getTransformUnit();
}
/*----------------------------------------------------------------------------*/
bool Picture::isAvailableInScanZ(PelCoord curr, PelCoord adj) const
{
    runtime_assert(curr != adj);
    runtime_assert(encloses(curr));

    if(encloses(adj))
    {
        const auto currAddrInScanZ = toAddrInScanZ(curr);
        const auto adjAddrInScanZ = toAddrInScanZ(adj);

        if(adjAddrInScanZ > currAddrInScanZ)
        {
            return false;
        }
        else
        {
            const auto currSliceAddrInRs = slice(curr)->addr().inRs;
            const auto adjSliceAddrInRs = slice(adj)->addr().inRs;

            if(adjSliceAddrInRs != currSliceAddrInRs)
            {
                return false;
            }
            else
            {
                const auto currTileId = tileId(curr);
                const auto adjTileId = tileId(adj);

                if(adjTileId != currTileId)
                {
                    return false;
                }
            }
        }

        return true;
    }

    return false;
}
/*----------------------------------------------------------------------------*/
/* 6.4.2, "Derivation process for prediction block availability" */
bool Picture::isPredictionBlockAvailable(
        PelCoord cb, Pel nCbS,
        PelCoord pb, Pel nPbW, Pel nPbH,
        int partIdx,
        PelCoord nbY) const
{
    /* default values */
    auto availableN = true;
    auto sameCb = false;

    if(PelRect(cb, nCbS, nCbS).encloses(nbY))
    {
        sameCb = true;
    }

    if(!sameCb)
    {
        availableN = isAvailableInScanZ(pb, nbY);
    }
    else if(
            (nPbW << 1) == nCbS && (nPbH << 1) == nCbS
            && partIdx == 1
            && (cb.y() + nPbH) <= nbY.y() && (cb.x() + nPbW) > nbY.x())
    {
        availableN = false;
    }

    if(availableN)
    {
        const auto cu = getCodingUnit(nbY);
        const auto cuPredMode = cu->get<Syntax::CodingUnit::CuPredMode>();

        if(PredictionMode::Intra == *cuPredMode)
        {
            availableN = false;
        }
    }

    return availableN;
}
/*----------------------------------------------------------------------------*/
void Picture::writeTo(std::ostream &dst, PelLayerId id)
{
    const auto bppY = bitDepth(Component::Luma);
    const auto bppC = bitDepth(Component::Chroma);

    for(const auto plane : EnumRange<Plane>())
    {
        const auto &layer = m_pelLayers[int(id)];

        if(isPresent(plane, chromaFormatIdc))
        {
            if( 8 < bppY || 8 < bppC)
            {
                layer[int(plane)].writeTo(
                        dst,
                        [](std::ostream &os, Sample s)
                        {
                        os.write(reinterpret_cast<const char *>(&s), sizeof(uint16_t));
                        });
            }
            else
            {
                layer[int(plane)].writeTo(
                        dst,
                        [](std::ostream &os, Sample s)
                        {
                        os.write(reinterpret_cast<const char *>(&s), sizeof(uint8_t));
                        });
            }
        }
    }
}
/*----------------------------------------------------------------------------*/
}} // HEVC::Structure
