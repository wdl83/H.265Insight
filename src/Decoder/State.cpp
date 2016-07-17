#include <Decoder/State.h>
#include <Decoder/Process.h>
#include <Decoder/Processes/CrossComponentPrediction.h>
#include <Decoder/Processes/DecodedPictureHash.h>
#include <Decoder/Processes/Deblock.h>
#include <Decoder/Processes/PictureOrderCount.h>
#include <Decoder/Processes/ReferencePictureSet.h>
#include <Decoder/Processes/ReferencePictureList.h>
#include <Decoder/Processes/QuantizationParameters.h>
#include <Decoder/Processes/Residuals.h>
#include <Decoder/Processes/IntraSamplesPrediction.h>
#include <Decoder/Processes/InterPrediction.h>
#include <Decoder/Processes/Reconstruction.h>
#include <Decoder/Processes/SampleAdaptiveOffset.h>
#include <Syntax/StreamNalUnit.h>
#include <Syntax/NalUnitHeader.h>
#include <Syntax/NalUnit.h>
#include <Syntax/SliceSegmentHeader.h>
#include <log.h>

namespace HEVC { namespace Decoder {

typedef Syntax::StreamNalUnit SNU;
typedef Syntax::NalUnit NU;
typedef Syntax::NalUnitHeader NUH;
typedef Syntax::VideoParameterSet VPS;
typedef Syntax::SequenceParameterSet SPS;
typedef Syntax::PictureParameterSet PPS;
typedef Syntax::ScalingListData SLD;
typedef Syntax::SliceSegmentLayerRbsp SSL;
typedef Syntax::SliceSegmentHeader SSH;
typedef Syntax::SliceSegmentData SSD;
typedef Syntax::CodingTreeUnit CTU;
typedef Syntax::CodingUnit CU;
typedef Syntax::TransformUnit TU;
typedef Syntax::ResidualCoding RC;

typedef Structure::AccessUnit AU;

/*----------------------------------------------------------------------------*/
template <typename T>
inline
auto subtree(const SNU &snu)
    -> decltype(snu.getSubtree<NU>()->getSubtree<T>())
{
    return snu.getSubtree<NU>()->getSubtree<T>();
}

/*----------------------------------------------------------------------------*/
namespace {
/*----------------------------------------------------------------------------*/
VLA<uint8_t> parse(std::istream &input)
{
    /* start code prefix 24/32 bits (reverse order) */
    static const uint8_t scp24[] = {1, 0, 0};
    static const uint8_t scp32[] = {1, 0, 0, 0};

    std::istream_iterator<uint8_t> begin(input), end;
    VLA<uint8_t> output;

    output.reserve(4 * 1024);

    /* search for byte stream NAL Unit start code prefix */
    while(
            !isMatchingAt(output.rbegin(), output.rend(), scp24)
            && !isMatchingAt(output.rbegin(), output.rend(), scp32))
    {
        /* expected syntax element: leading_zero_8bits */
        output.pushBack(*begin);

        if(0 != output.back() && 1 != output.back())
        {
            /* undo unexpected only */
            input.unget();
            runtime_assert(false);
        }
        ++begin;
    }

    /* until next NAL Unit start code prefix is encountered (or end of file),
     * treat all bytes as NAL Unit payload */

    while(begin != end)
    {
        /* NAL Unit payload including syntax element: trailing_zero_8bits */
        output.pushBack(*begin);

        const auto scp24Matched =
            isMatchingAt(output.rbegin(), output.rend(), scp24);
        /* do not search for scp32 if scp24 present */
        const auto scp32Matched =
            scp24Matched
            ? false
            : isMatchingAt(output.rbegin(), output.rend(), scp32);

        if(scp24Matched || scp32Matched)
        {
            const auto scpSize = scp24Matched ? sizeof(scp24) : sizeof(scp32);

            /* revert consumed start code prefix */
            const std::streampos absPos = input.tellg();
            const std::streamoff offset = absPos - static_cast<std::streampos>(scpSize);

            runtime_assert(absPos > static_cast<std::streampos>(sizeof(scpSize)));
            input.seekg(offset, std::ios_base::beg);
            /* truncate next NAL Unit scp from current tail */
            output.resize(output.size() - scpSize);
            break;
        }
        ++begin;
    }

    return output;
}
/*----------------------------------------------------------------------------*/
} /* namespace */
/*----------------------------------------------------------------------------*/
State::State(PictureOutput pictureOutput):
    m_context(makeHandle<Context>()),
    m_processIdMask{0},
    m_SoCVS{false},
    dpb{pictureOutput}
{}
/*----------------------------------------------------------------------------*/
void State::onStart(Syntax::SliceSegmentData &)
{
    const auto ssh = subtree<SSL>(*snu)->getSubtree<SSH>();
    const Ctb addrInRs = *ssh->get<SSH::SliceAddrRs>();
    const auto addrInTs = picture()->toAddrInTs(addrInRs);

    picture()->add(
            CtbAddr{addrInTs, addrInRs},
            ssh, subtree<SSL>(*snu)->getSubtree<SSD>());
}
/*----------------------------------------------------------------------------*/
void State::onStart(Syntax::CodingTreeUnit &ctu)
{
    const auto ssd = subtree<SSL>(*snu)->getSubtree<SSD>();

    picture()->add(Ptr<CTU>{ssd->getSubtreeList<CTU>().back()});
    auto &tile = picture()->tile(ctu.get<CTU::Coord>()->inUnits());

    /* tile within a slice */
    if(tile.sliceList().empty())
    {
        tile.add(Ptr<Structure::Slice>{picture()->sliceList().back()});
    }
}
/*----------------------------------------------------------------------------*/
void State::onStart(Syntax::CodingUnit &cu)
{
    const auto cuCoord = cu.get<CU::Coord>()->inUnits();

    // current CU (top-left) is aligned to qunatization group (top-left)
    const auto minQpGrpCoord = picture()->toPel(picture()->toMinQpGrp(cuCoord));

    if(cuCoord == minQpGrpCoord)
    {
        qpStatus.qpYpred =
            subprocess(
                    *this, Processes::QuantizationParameters(),
                    picture(), cuCoord);
    }
}
/*----------------------------------------------------------------------------*/
void State::onStart(Syntax::ResidualCoding &rc)
{
    const auto chromaFormatIdc = picture()->chromaFormatIdc;
    const auto is400 = ChromaFormatIdc::f400 == chromaFormatIdc;
    const auto isSCP = ChromaFormatIdc::fSCP == chromaFormatIdc;
    const auto rcCoord = rc.get<RC::Coord>()->inUnits();
    const auto cu = picture()->getCodingUnit(rcCoord);
    const auto cuCoord = cu->get<CU::Coord>()->inUnits();
    const Plane plane = *rc.get<RC::CIdx>();
    const auto isY = Plane::Y == plane;

    if(!cu->constructed<CU::QpY>())
    {
        const auto qpY =
            subprocess(*this, Processes::QuantizationParametersLuma(), picture());

        Syntax::embed<CU::QpY>(*cu, std::get<0>(qpY), std::get<1>(qpY));
    }

    if(!is400 && !isSCP && !isY && !cu->constructed<CU::QpC>())
    {
        const auto qpY = cu->get<CU::QpY>();

        const auto qpC =
            subprocess(
                    *this, Processes::QuantizationParametersChroma(),
                    picture(),
                    cuCoord,
                    qpY->qpY,
                    qpStatus.cuQpOffsetCb,
                    qpStatus.cuQpOffsetCr);

        Syntax::embed<CU::QpC>(*cu, std::get<0>(qpC), std::get<1>(qpC));
    }
}
/*----------------------------------------------------------------------------*/
void State::onFinish(Syntax::StreamNalUnit &)
{
    ++cntr.snu;

    const NalUnitType nut = *subtree<NUH>(*snu)->get<NUH::NalUnitType>();

    if(isVCL(nut))
    {
        if(isRSV(nut))
        {
            destruct(snu);
        }
        else
        {
            currAU()->add(std::move(snu));

            if(picture()->sizeInCtbsY == picture()->ctuNum())
            {
                process(*this, Processes::DecodedPictureHash(), picture());
                currAU()->onDecodeFinish(dpb.pictureOutput);
                m_SoCVS = false;
            }
        }
    }
    else
    {
        if(NalUnitType::VPS_NUT == nut)
        {
            const auto vps = subtree<VPS>(*snu);

            context()->add(
                    std::move(snu),
                    vps,
                    *vps->get<VPS::VpsVideoParameterSetId>());
        }
        else if(NalUnitType::SPS_NUT == nut)
        {
            const auto sps = subtree<SPS>(*snu);

            context()->add(
                    std::move(snu),
                    sps,
                    *sps->get<SPS::SeqParameterSetId>());
        }
        else if(NalUnitType::PPS_NUT == nut)
        {
            const auto pps = subtree<PPS>(*snu);

            context()->add(
                    std::move(snu),
                    pps,
                    *pps->get<PPS::PpsPicParameterSetId>());
        }
        else if(
                NalUnitType::EOS_NUT == nut
                || NalUnitType::EOB_NUT == nut
                || NalUnitType::FD_NUT == nut
                || NalUnitType::SUFFIX_SEI_NUT == nut)
        {
            currAU()->add(std::move(snu));
        }
        else if(
                NalUnitType::AUD_NUT == nut)
        {
            onAU(nut);
            currAU()->add(std::move(snu));
        }

        if(snu)
        {
            destruct(snu);
        }
    }
}
/*----------------------------------------------------------------------------*/
void State::onFinish(Syntax::NalUnit &nu)
{
    const NalUnitType nut = *nu.getSubtree<NUH>()->get<NUH::NalUnitType>();
    const int temporalId = *nu.getSubtree<NUH>()->get<NUH::TemporalId>();

    const auto toStr =
        [this, nut, temporalId](std::ostream &oss)
        {
            oss
                << "\n------ " << getName(nut)
                << " TemporalId " << temporalId << ' ';

            cntr.toStr(oss);

            oss << " ------\n";
        };

    log(LogId::Syntax, toStr);

    if(isRASL(nut))
    {
        m_processIdMask |= Processes::interOnlyMask;
        m_processIdMask |= Processes::loopFilterMask;
    }
    else
    {
        m_processIdMask = Processes::ProcessIdMask{0};
    }

    nu.parseRbsp(*this);
}
/*----------------------------------------------------------------------------*/
void State::onFinish(Syntax::ShortTermRefPicSet &stRPS)
{
    context()->get<Structure::RPSP>()->derive(stRPS);
}
/*----------------------------------------------------------------------------*/
void State::onFinish(Syntax::SliceSegmentHeaderContent::FirstSliceSegmentInPicFlag &flag)
{
    if(flag)
    {
        const NalUnitType nut = *subtree<NUH>(*snu)->get<NUH::NalUnitType>();
        const int tId = *subtree<NUH>(*snu)->get<NUH::TemporalId>();

        onAU(nut, tId);
    }
}
/*----------------------------------------------------------------------------*/
void State::onFinish(Syntax::SliceSegmentHeaderContent::SlicePicParameterSetId &ppsId)
{
    const auto pps = context()->get<PPS>(ppsId);
    const auto spsId = pps->get<PPS::PpsSeqParameterSetId>();
    const auto sps = context()->get<SPS>(*spsId);
    const auto vps = context()->get<VPS>(*sps->get<SPS::SpsVideoParameterSetId>());
    const auto ssh = subtree<SSL>(*snu)->getSubtree<SSH>();
    const NalUnitType nut = *subtree<NUH>(*snu)->get<NUH::NalUnitType>();

    if(*(ssh->get<SSH::FirstSliceSegmentInPicFlag>()))
    {
        context()->get<Structure::RPSP>()->derive(*sps);
        runtime_assert(bool(*ssh->get<SSH::FirstSliceSegmentInPicFlag>()));

        /* first AU in the bitstream in decoding order,or the first AU
         * that follows an EOS NAL unit in decoding order */
        const auto noRaslOutputFlag = m_SoCVS;

        currAU()->picture(nut, vps, sps, pps, noRaslOutputFlag, cntr.au - 1);

        if(m_SoCVS)
        {
            dpb.setup(*sps);
            log(
                    LogId::Sequence,
                    [sps](std::ostream &oss){sps->toStr(oss); oss << '\n';});
        }

        log(
                LogId::Picture,
                [pps](std::ostream &oss){pps->toStr(oss); oss << '\n';});

        /* ScalingList & ScalingFactor */
        const auto scalingListData =
            [sps, pps]()
            {
                if(*pps->get<PPS::PpsScalingListDataPresentFlag>())
                {
                    return pps->getSubtree<SLD>();
                }
                else if(
                        *sps->get<SPS::ScalingListEnabledFlag>()
                        && *sps->get<SPS::SpsScalingListDataPresentFlag>())
                {
                    return sps->getSubtree<SLD>();
                }

                return Ptr<SLD>();
            };

        const auto scalingList = context()->get<Structure::ScalingList>();
        const auto scalingFactor = context()->get<Structure::ScalingFactor>();

        scalingList->derive(scalingListData());

        log(
                LogId::ScalingList,
                [scalingList](std::ostream &oss){oss << scalingList->toStr();});

        scalingFactor->derive(*scalingList, scalingListData());

        log(
                LogId::ScalingFactor,
                [scalingFactor](std::ostream &oss){oss << scalingFactor->toStr();});
    }
}
/*----------------------------------------------------------------------------*/
void State::onFinish(Syntax::SliceSegmentHeader::PicOutputFlag &flag)
{
    const auto nut = picture()->nalUnitType;

    const auto associatedIRAP =
        [this]()
        {
            const auto i =
                std::find_if(
                        dpb.rbegin(), dpb.rend(),
                        [](const Structure::DPB::Entry &entry)
                        {
                            return
                                isBLA(entry.au()->picture()->nalUnitType)
                                || isCRA(entry.au()->picture()->nalUnitType);
                        });

            runtime_assert(i != dpb.rend());
            return i->au()->picture();
        };

    if(isRASL(nut) && associatedIRAP()->noRaslOutputFlag)
    {
        picture()->picOutputFlag = false;
    }
    else
    {
        picture()->picOutputFlag = bool(flag);
    }
}
/*----------------------------------------------------------------------------*/
void State::onFinish(Syntax::SliceSegmentHeader &ssh)
{
    const bool firstSliceSegmentInPicFlag(*ssh.get<SSH::FirstSliceSegmentInPicFlag>());
    const bool dependentSliceSegmentFlag(*ssh.get<SSH::DependentSliceSegmentFlag>());
    const Ctb addrInRs(*ssh.get<SSH::SliceAddrRs>());
    const auto addrInTs = picture()->toAddrInTs(addrInRs);

    log(LogId::Slice, [&ssh](std::ostream &oss){ssh.toStr(oss);});

    if(firstSliceSegmentInPicFlag)
    {
        log(
                LogId::DecodedPictureBuffer, "DPB state\n",
                [this](std::ostream &oss){dpb.toStr(oss);});

        /* call 8.3.1 */
        process(*this, Processes::PictureOrderCount(), picture(), ssh);
        /* call 8.3.2 */
        process(*this, Processes::ReferencePictureSet(), picture(), ssh);
        /* call 8.3.3 TODO */
        //process(*this, Processes::unavailableReferencePictures, picture(), ssh);

        updateDPB(ssh);
    }

    if(!dependentSliceSegmentFlag)
    {
        /* 8.3.4 */
        auto slice = picture()->slice(CtbAddr{addrInTs, addrInRs});

        if(!isI(*ssh.get<SSH::SliceType>()))
        {
            process(*this, Processes::ReferencePictureList(), picture(), slice, ssh);
        }
    }
}
/*----------------------------------------------------------------------------*/
void State::onFinish(Syntax::CodingTreeUnit &ctu)
{
    const auto coord = picture()->toCtb(ctu.get<CTU::Coord>()->inUnits());
    // left
    const CtbCoord lCoord{coord.x() - 1_ctb, coord.y()};
    // left above
    const CtbCoord laCoord{lCoord.x(), lCoord.y() - 1_ctb};
    // left left
    const CtbCoord llCoord{lCoord.x() - 1_ctb, lCoord.y()};
    // left left above
    const CtbCoord llaCoord{llCoord.x(), llCoord.y() - 1_ctb};
    // above
    const CtbCoord aCoord{coord.x(), coord.y() - 1_ctb};
    const auto lhs = 0_ctb == coord.x();
    const auto llhs = 0_ctb == lCoord.x();
    const auto rhs = picture()->widthInCtbsY - 1_ctb == coord.x();
    const auto top = 0_ctb == coord.y();
    const auto bottom = picture()->heightInCtbsY - 1_ctb == coord.y();

    log(
            LogId::CodingTreeUnit,
            [this, &ctu](std::ostream &oss)
            {
                oss << ctu.get<CTU::Coord>()->inUnits() << '\n';
                ctu.toStr(oss, picture()->ctbSizeY);
            },
            '\n');

    /* Deblocking filter */
    process(*this, Processes::Deblock(), picture(), ctu, EdgeType::V);

    if(!lhs)
    {
        const auto adj = picture()->getCodingTreeUnit(lCoord);
        process(*this, Processes::Deblock(), picture(), *adj, EdgeType::H);
    }

    if(rhs)
    {
        process(*this, Processes::Deblock(), picture(), ctu, EdgeType::H);
    }

    /* Sample Adaptive Offset filter */
    if(!llhs && !lhs && !top)
    {
        const auto lla = picture()->getCodingTreeUnit(llaCoord);
        process(*this, Processes::SampleAdaptiveOffset(), picture(), *lla);
    }

    if(rhs && !top)
    {
        if(!lhs)
        {
            const auto la = picture()->getCodingTreeUnit(laCoord);
            process(*this, Processes::SampleAdaptiveOffset(), picture(), *la);
        }

        const auto a = picture()->getCodingTreeUnit(aCoord);
        process(*this, Processes::SampleAdaptiveOffset(), picture(), *a);
    }

    if(!lhs && bottom)
    {
        if(!llhs)
        {
            const auto ll = picture()->getCodingTreeUnit(llCoord);
            process(*this, Processes::SampleAdaptiveOffset(), picture(), *ll);
        }

        const auto l = picture()->getCodingTreeUnit(lCoord);
        process(*this, Processes::SampleAdaptiveOffset(), picture(), *l);
    }

    if(rhs && bottom)
    {
        process(*this, Processes::SampleAdaptiveOffset(), picture(), ctu);
    }
}
/*----------------------------------------------------------------------------*/
void State::onFinish(Syntax::CodingUnit &cu)
{
    const auto chromaFormatIdc = picture()->chromaFormatIdc;
    const auto is400 = ChromaFormatIdc::f400 == chromaFormatIdc;
    const auto isSCP = ChromaFormatIdc::fSCP == chromaFormatIdc;
    const auto cuCoord = cu.get<CU::Coord>()->inUnits();
    const auto cuSize = cu.get<CU::Size>()->inUnits();

    // current CU has no RC syntax elements
    if(!cu.constructed<CU::QpY>())
    {
        const auto qpY =
            subprocess(*this, Processes::QuantizationParametersLuma(), picture());

        Syntax::embed<CU::QpY>(cu, std::get<0>(qpY), std::get<1>(qpY));
    }

    if(!is400 && !isSCP && !cu.constructed<CU::QpC>())
    {
        const auto qpY = cu.get<CU::QpY>();

        const auto qpC =
            subprocess(
                    *this, Processes::QuantizationParametersChroma(),
                    picture(),
                    cuCoord,
                    qpY->qpY,
                    qpStatus.cuQpOffsetCb,
                    qpStatus.cuQpOffsetCr);

        Syntax::embed<CU::QpC>(cu, std::get<0>(qpC), std::get<1>(qpC));
    }

    if(isInter(*cu.get<CU::CuPredMode>()) || isSkip(*cu.get<CU::CuPredMode>()))
    {
        for(const auto plane : EnumRange<Plane>())
        {
            if(!isPresent(plane, chromaFormatIdc))
            {
                continue;
            }

            process(
                    *this, Processes::Reconstruction(),
                    picture(),
                    plane,
                    cuCoord, cuSize,
                    scale(cuCoord, plane, chromaFormatIdc),
                    hScale(toPel(cuSize), plane, chromaFormatIdc),
                    vScale(toPel(cuSize), plane, chromaFormatIdc));
        }
    }
}
/*----------------------------------------------------------------------------*/
void State::onFinish(Syntax::PredictionUnit &pu)
{
    process(*this, Processes::InterPrediction(), picture(), pu);
}
/*----------------------------------------------------------------------------*/
void State::onFinish(Syntax::TransformUnit &tu)
{
    const auto tuCoord = tu.get<TU::Coord>()->inUnits();
    const auto cu = picture()->getCodingUnit(tuCoord);

    if(PredictionMode::Intra == *cu->get<CU::CuPredMode>())
    {
        const auto chromaFormatIdc = picture()->chromaFormatIdc;
        const auto is420 = ChromaFormatIdc::f420 == chromaFormatIdc;
        const auto is422 = ChromaFormatIdc::f422 == chromaFormatIdc;
        const auto is444 = ChromaFormatIdc::f444 == chromaFormatIdc;
        const auto tuRootCoord = tu.get<TU::RootCoord>()->inUnits();
        const auto tuSize = tu.get<TU::Size>()->inUnits();
        //const auto tuSide = toPel(tuSize);
        const auto blkIdx = tu.get<TU::BlkIdx>()->inUnits();
        const Log2 minSize{log2(Limits::TrafoSize::min)};

        const auto tuSizeC = std::max(2_log2, tuSize - (is444 ? 0_log2 : 1_log2));
        const auto tuCoordC = !is444 && 2_log2 == tuSize ? tuRootCoord : tuCoord;
        const auto tuCoord422C = tuCoordC + PelCoord{0_pel, toPel(tuSizeC)};

        for(const auto plane : EnumRange<Plane>())
        {
            const auto isY = Plane::Y == plane;
            const auto isMinSize = minSize == tuSize;
            const auto min444 = isMinSize && is444;
            const auto minC420 = isMinSize && is420 && !isY && 3 == blkIdx;
            const auto minC422 = isMinSize && is422 && !isY && 3 == blkIdx;
            const auto minC42x = minC420 || minC422;

            const auto inPlaneCoord = scale(isY ? tuCoord : tuCoordC, plane, chromaFormatIdc);
            const auto inPlaneSize = isY ? tuSize : tuSizeC;

            if(
                    isPresent(plane, chromaFormatIdc)
                    && (!isMinSize || isY || min444 || minC42x))
            {
                if(is444 && Plane::Cb == plane)
                {
                    process(
                            *this, Processes::CrossComponentPrediction(),
                            picture(), tu, Chroma::Cb);
                }
                else if(is444 && Plane::Cr == plane)
                {
                    process(
                            *this, Processes::CrossComponentPrediction(),
                            picture(), tu, Chroma::Cr);
                }

                process(
                        *this, Processes::IntraSamplesPrediction(),
                        picture(),
                        cu,
                        plane,
                        isY ? tuCoord : tuCoordC,
                        isY ? tuSize : tuSize,
                        inPlaneCoord, inPlaneSize);

                process(
                        *this, Processes::Reconstruction(),
                        picture(),
                        plane,
                        isY ? tuCoord : tuCoordC,
                        isY ? tuSize : tuSize,
                        inPlaneCoord, toPel(inPlaneSize), toPel(inPlaneSize));

                if(!isY && is422)
                {
                    const auto inPlaneCoord422C = scale(tuCoord422C, plane, chromaFormatIdc);

                    process(
                            *this, Processes::IntraSamplesPrediction(),
                            picture(),
                            cu,
                            plane,
                            tuCoord422C,
                            tuSize,
                            inPlaneCoord422C, inPlaneSize);

                    process(
                            *this, Processes::Reconstruction(),
                            picture(),
                            plane,
                            tuCoord422C,
                            tuSize,
                            inPlaneCoord422C, toPel(inPlaneSize), toPel(inPlaneSize));
                }
            }
        }
    }
}
/*----------------------------------------------------------------------------*/
void State::onFinish(Syntax::ResidualCoding &rc)
{
    process(*this, Processes::Residuals(), picture(), rc);
}
/*----------------------------------------------------------------------------*/
void State::exec(std::istream &bitstream, Predicate p)
{
    bitstream >> std::noskipws;

    while(!bitstream.eof() && p(*this))
    {
        snu = makeHandle<SNU>(parse(bitstream));
        parse(snu->toStreamAccessLayer(), *this, *snu);
    }

    if(!cpb.empty() && cpb.au()->picture())
    {
        if(!isRASL(cpb.au()->picture()->nalUnitType))
        {
            cpb.moveTo(dpb);
        }
        else
        {
            cpb.clear();
        }
    }

    dpb.flush();
    dpb.clear();
}
/*----------------------------------------------------------------------------*/
void State::updateDPB(const Syntax::SliceSegmentHeader &sh)
{
    const auto nut = currAU()->picture()->nalUnitType;

    if(isIRAP(nut) && picture()->noRaslOutputFlag && m_SoCVS)
    {
        const auto noOutputOfPriorPicsFlag =
            isCRA(nut) || *sh.get<SSH::NoOutputOfPriorPicsFlag>();

        if(noOutputOfPriorPicsFlag)
        {
            dpb.clear();
        }
        else
        {
            dpb.flush();
        }
    }

    dpb.eraseUnused();

    auto bumpGuard = 10 * dpb.capacity();

    while(dpb.reorderOverflow() || dpb.latencyOverflow())
    {
        runtime_assert(0 < bumpGuard);
        dpb.bump();
        --bumpGuard;
    }
}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Decoder */
