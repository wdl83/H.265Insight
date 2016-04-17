#include <Syntax/CodingUnit.h>
#include <Syntax/PictureParameterSet.h>
#include <Syntax/SliceSegmentHeader.h>
#include <Syntax/CodingQuadTree.h>
#include <Syntax/PcmSample.h>
#include <Syntax/PredictionUnit.h>
#include <Syntax/TransformTree.h>
#include <Syntax/TransformUnit.h>
#include <Syntax/ResidualCoding.h>
#include <Structure/Picture.h>
#include <Decoder/State.h>
#include <Decoder/Process.h>
#include <Decoder/Processes/IntraLumaPredictionMode.h>
#include <Decoder/Processes/IntraChromaPredictionMode.h>
#include <Decoder/Processes/Deblock.h>
#include <log.h>

namespace HEVC { namespace Syntax { namespace CodingUnitContent {
/*----------------------------------------------------------------------------*/
static const PredictionUnit::MergeFlag *partition(
        CodingUnit &cu,
        StreamAccessLayer &streamAccessLayer,
        Decoder::State &decoder,
        const Coord &coord,
        const Size &size,
        const PartModePseudo &partModePseudo)
{
    const auto x0 = coord.x();
    const auto y0 = coord.y();
    const auto nCbS = toPel(size.inUnits());

    const PredictionUnit::MergeFlag *mergeFlag{nullptr};

    if(PartitionMode::PART_2Nx2N == partModePseudo)
    {
        auto pu =
            embedSubtreeInList<PredictionUnit>(
                    cu, PelCoord{x0, y0}, nCbS, nCbS, 0);

        parse(streamAccessLayer, decoder, *pu);
        mergeFlag = pu->get<PredictionUnit::MergeFlag>();
    }
    else if(PartitionMode::PART_2NxN == partModePseudo)
    {
        {
            auto pu =
                embedSubtreeInList<PredictionUnit>(
                        cu, PelCoord{x0, y0}, nCbS, nCbS / 2, 0);

            parse(streamAccessLayer, decoder, *pu);
            mergeFlag = pu->get<PredictionUnit::MergeFlag>();
        }

        {
            auto pu =
                embedSubtreeInList<PredictionUnit>(
                        cu, PelCoord{x0, y0 + nCbS / 2}, nCbS, nCbS / 2, 1);

            parse(streamAccessLayer, decoder, *pu);
        }
    }
    else if(PartitionMode::PART_Nx2N == partModePseudo)
    {
        {
            auto pu =
                embedSubtreeInList<PredictionUnit>(
                        cu, PelCoord{x0, y0}, nCbS / 2, nCbS, 0);

            parse(streamAccessLayer, decoder, *pu);
            mergeFlag = pu->get<PredictionUnit::MergeFlag>();
        }

        {
            auto pu =
                embedSubtreeInList<PredictionUnit>(
                        cu, PelCoord{x0 + nCbS / 2, y0}, nCbS / 2, nCbS, 1);

            parse(streamAccessLayer, decoder, *pu);
        }
    }
    else if(PartitionMode::PART_2NxnU == partModePseudo)
    {
        {
            auto pu =
                embedSubtreeInList<PredictionUnit>(
                        cu, PelCoord{x0, y0}, nCbS, nCbS / 4, 0);

            parse(streamAccessLayer, decoder, *pu);
            mergeFlag = pu->get<PredictionUnit::MergeFlag>();
        }

        {
            auto pu =
                embedSubtreeInList<PredictionUnit>(
                        cu, PelCoord{x0, y0 + nCbS / 4}, nCbS, (nCbS * 3) / 4, 1);

            parse(streamAccessLayer, decoder, *pu);
        }
    }
    else if(PartitionMode::PART_2NxnD == partModePseudo)
    {
        {
            auto pu =
                embedSubtreeInList<PredictionUnit>(
                        cu, PelCoord{x0, y0}, nCbS, (nCbS * 3) / 4, 0);

            parse(streamAccessLayer, decoder, *pu);
            mergeFlag = pu->get<PredictionUnit::MergeFlag>();
        }

        {
            auto pu =
                embedSubtreeInList<PredictionUnit>(
                        cu, PelCoord{x0, y0 + (nCbS * 3) / 4}, nCbS, nCbS / 4, 1);

            parse(streamAccessLayer, decoder, *pu);
        }
    }
    else if(PartitionMode::PART_nLx2N == partModePseudo)
    {
        {
            auto pu =
                embedSubtreeInList<PredictionUnit>(
                        cu, PelCoord{x0, y0}, nCbS / 4, nCbS, 0);

            parse(streamAccessLayer, decoder, *pu);
            mergeFlag = pu->get<PredictionUnit::MergeFlag>();
        }

        {
            auto pu =
                embedSubtreeInList<PredictionUnit>(
                        cu, PelCoord{x0 + nCbS / 4, y0}, (nCbS * 3) / 4, nCbS, 1);

            parse(streamAccessLayer, decoder, *pu);
        }
    }
    else if(PartitionMode::PART_nRx2N == partModePseudo)
    {
        {
            auto pu =
                embedSubtreeInList<PredictionUnit>(
                        cu, PelCoord{x0, y0}, (nCbS * 3) / 4, nCbS, 0);

            parse(streamAccessLayer, decoder, *pu);
            mergeFlag = pu->get<PredictionUnit::MergeFlag>();
        }

        {
            auto pu =
                embedSubtreeInList<PredictionUnit>(
                        cu, PelCoord{x0 + (nCbS * 3) / 4, y0}, nCbS / 4, nCbS, 1);

            parse(streamAccessLayer, decoder, *pu);
        }
    }
    else if(PartitionMode::PART_NxN == partModePseudo)
    {
        {
            auto pu =
                embedSubtreeInList<PredictionUnit>(
                        cu, PelCoord{x0, y0}, nCbS / 2, nCbS / 2, 0);

            parse(streamAccessLayer, decoder, *pu);
            mergeFlag = pu->get<PredictionUnit::MergeFlag>();
        }

        {
            auto pu =
                embedSubtreeInList<PredictionUnit>(
                        cu, PelCoord{x0 + nCbS / 2, y0}, nCbS / 2, nCbS / 2, 1);

            parse(streamAccessLayer, decoder, *pu);
        }

        {
            auto pu =
                embedSubtreeInList<PredictionUnit>(
                        cu, PelCoord{x0, y0 + nCbS / 2}, nCbS / 2, nCbS / 2, 2);

            parse(streamAccessLayer, decoder, *pu);
        }

        {
            auto pu =
                embedSubtreeInList<PredictionUnit>(
                        cu, PelCoord{x0 + nCbS / 2, y0 + nCbS / 2}, nCbS / 2, nCbS / 2, 3);

            parse(streamAccessLayer, decoder, *pu);
        }
    }

    return mergeFlag;
}
/*----------------------------------------------------------------------------*/
/* CuSkipFlag */
/*----------------------------------------------------------------------------*/
void CuSkipFlag::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
        PelCoord cuCoord)
{
    const auto ctxInc =
        [cuCoord, &decoder](int binIdx)
        {
            /* 04/2013,
             * 9.3.4.2.2, "Derivation process for ctxInc using left and above syntax elements" */
            int i =0;

            syntaxCheck(0 == binIdx);

            const auto curr = cuCoord;
            const PelCoord left{curr.x() - 1_pel, curr.y()};
            const PelCoord above{curr.x(), curr.y() - 1_pel};

            auto picture = decoder.picture();

            /*  call "availability derivation process for a block in z-scan order" (6.4.1) */
            const auto availableL = picture->isAvailableInScanZ(curr, left);
            const auto availableA = picture->isAvailableInScanZ(curr, above);

            if(availableL)
            {
                const auto leftCuSkipFlag = picture->getCodingUnit(left)->get<CuSkipFlag>();
                i += *leftCuSkipFlag ? 1 : 0;
            }

            if(availableA)
            {
                const auto aboveCuSkipFlag = picture->getCodingUnit(above)->get<CuSkipFlag>();
                i += *aboveCuSkipFlag ? 1 : 0;
            }

            return i;
        };

    const auto contextModel =
        [ctxInc](CABAD::State &state, int binIdx) -> CABAD::Variable &
        {
            return state.getVariable(CABAD::CtxId::cu_skip_flag, ctxInc(binIdx));
        };

    getFrom(streamAccessLayer, decoder, cabadState(decoder), *this, contextModel);
}
/*----------------------------------------------------------------------------*/
/* CuPredMode */
/*----------------------------------------------------------------------------*/
void CuPredMode::set(
        const SliceSegmentHeader::SliceType &sliceType,
        const CuSkipFlag &cuSkipFlag)
{
    if(isI(sliceType))
    {
        m_mode = PredictionMode::Intra;
    }
    else if(isP(sliceType) || isB(sliceType))
    {
        if(cuSkipFlag)
        {
            m_mode = PredictionMode::Skip;
        }
        else
        {
            syntaxCheck(false);
        }
    }

    onSet();
}

void CuPredMode::set(const PredModeFlag &predModeFlag)
{
    if(predModeFlag)
    {
        m_mode = PredictionMode::Intra;
    }
    else
    {
        m_mode = PredictionMode::Inter;
    }

    onSet();
}
/*----------------------------------------------------------------------------*/
/* PartModeDebinarizer */
/*----------------------------------------------------------------------------*/
CABAD::BinString PartModeDebinarizer::getBinarization(
        StreamAccessLayer &streamAccessLayer,
        CABAD::State &state,
        PredictionMode cuPredMode,
        Log2 minCbSizeY,
        bool ampEnabledFlag,
        Log2 size)
{
    auto &arithmeticDecoder = state.arithmeticDecoder;
    CABAD::BinString binarization;

    /* 10v34, 9.3.3.5 "Binarization process for part_mode", Table 9-34 */

    const auto decodeBin =
        [&streamAccessLayer, &state, &arithmeticDecoder](int ctxInc)
        {
            return
                arithmeticDecoder.decodeBin(
                        streamAccessLayer,
                        state.getVariable(CABAD::CtxId::part_mode, ctxInc));
        };

    if(PredictionMode::Intra == cuPredMode)
    {
        syntaxCheck(size == minCbSizeY);
        syntaxCheck(3_log2 == size || 3_log2 < size);

        const auto binValue = decodeBin(0);

        binarization.pushBack(binValue);
    }
    else if(PredictionMode::Inter == cuPredMode)
    {
        // binIdx == 0
        {
            const auto binValue = decodeBin(0);

            binarization.pushBack(binValue);

            if(binValue)
            {
                /* binarization == 1b */
                goto done;
            }
        }
        // binIdx == 1
        {
            const auto binValue = decodeBin(1);

            binarization.pushBack(binValue);

            if(
                    binValue
                    && (
                        size > minCbSizeY && !ampEnabledFlag
                        ||
                        size == minCbSizeY
                        && (size == 3_log2 || size > 3_log2)))
            {
                /* binarization == 01b */
                goto done;
            }
            else if(
                    !binValue
                    && (
                        size > minCbSizeY && !ampEnabledFlag
                        || size == minCbSizeY && size == 3_log2))
            {
                /* binarization == 00b */
                goto done;
            }
        }
        // binIdx == 2
        {
            const auto binValue = decodeBin(2 + int(size > minCbSizeY));

            binarization.pushBack(binValue);

            if(
                    binValue
                    && (
                        size > minCbSizeY && ampEnabledFlag
                        || size == minCbSizeY && size > 3_log2))
            {
                /* binarization == 001b || 011b */
                goto done;
            }
            else if(!binValue && size == minCbSizeY && size > 3_log2)
            {
                /* binarization == 000b */
                goto done;
            }
        }
        // binIdx == 3
        {
            const auto binValue = arithmeticDecoder.decodeBin(streamAccessLayer);

            binarization.pushBack(binValue);
            /* binarization == 0100b || 0101b || 0000b || 0001b
             * are defined only when conditions below are valid.
             * Otherwise binarization process should be terminated earlier */
            syntaxCheck(size > minCbSizeY);
            syntaxCheck(ampEnabledFlag);
        }
    }
    else
    {
        syntaxCheck(false);
    }

done:
    return binarization;
}
/*----------------------------------------------------------------------------*/
/* IntraChromaPredModeDebinarizer */
/*----------------------------------------------------------------------------*/
CABAD::BinString IntraChromaPredModeDebinarizer::getBinarization(
        StreamAccessLayer &streamAccessLayer,
        CABAD::State &state)
{
    /* 10v34, 9.3.3.6 "Binarization process for intra_chroma_pred_mode" */

    auto &arithmeticDecoder = state.arithmeticDecoder;
    CABAD::BinString binarization;

    {
        const auto binValue =
            arithmeticDecoder.decodeBin(
                    streamAccessLayer,
                    state.getVariable(CABAD::CtxId::intra_chroma_pred_mode, 0));

        binarization.pushBack(binValue);

        if(!binValue)
        {
            goto done;
        }
    }

    {
        const auto maxBinNum = 3;

        for(auto binIdx = 1; binIdx < maxBinNum; ++binIdx)
        {
            const auto binValue = arithmeticDecoder.decodeBin(streamAccessLayer);
            binarization.pushBack(binValue);
        }
    }

done:
    return binarization;
}
/*----------------------------------------------------------------------------*/
} /* CodingUnitContent */

/*----------------------------------------------------------------------------*/
/* CodingUnit */
/*----------------------------------------------------------------------------*/
void CodingUnit::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
        const SliceSegmentHeader &sh, CodingTreeUnit &ctu)
{
    const auto cuCoord = get<Coord>()->inUnits();
    const auto size = get<Size>()->inUnits();

    //log(LogId::Prediction, cuCoord, '\n');

    /* start: inferrable */
    auto cuTransquantBypassFlag = embed<CuTransquantBypassFlag>(*this);
    auto cuSkipFlag = embed<CuSkipFlag>(*this);
    auto pcmFlag = embed<PcmFlag>(*this);
    /* end: inferrable */

    auto picture = decoder.picture();
    const auto chromaFormatIdc = picture->chromaFormatIdc;
    const auto is400 = ChromaFormatIdc::f400 == chromaFormatIdc;
    const auto is444 = ChromaFormatIdc::f444 == chromaFormatIdc;
    const auto sps = picture->sps;
    const auto pps = picture->pps;

    syntaxCheck(picture->widthInLumaSamples >= cuCoord.x() + toPel(size));
    syntaxCheck(picture->heightInLumaSamples >= cuCoord.y() + toPel(size));

    typedef SequenceParameterSet SPS;
    typedef PictureParameterSet PPS;
    typedef SliceSegmentHeader SSH;

    const auto transquantBypassEnableFlag = pps->get<PPS::TransquantBypassEnableFlag>();

    if(*transquantBypassEnableFlag)
    {
        parse(streamAccessLayer, decoder, *cuTransquantBypassFlag);
    }

    const auto sliceType = sh.get<SSH::SliceType>();

    if (!isI(*sliceType))
    {
        parse(streamAccessLayer, decoder, *cuSkipFlag, cuCoord);
    }

    const auto nCbS = toPel(size);
    /* further parsing depends on value of PredictionUnit(x0, y0)::MergeFlag */
    const PredictionUnit::MergeFlag *mergeFlag{nullptr};

    auto cuPredMode = embed<CuPredMode>(*this);
    auto partModePseudo = embed<PartModePseudo>(*this);
    auto intraSplitFlag = embed<IntraSplitFlag>(*this);

    if(*cuSkipFlag)
    {
        cuPredMode->set(*sliceType, *cuSkipFlag);

        auto pu = embedSubtreeInList<PredictionUnit>(*this, cuCoord, nCbS, nCbS, 0);

        parse(streamAccessLayer, decoder, *pu);

        mergeFlag = pu->get<PredictionUnit::MergeFlag>();
    }
    else
    {
        if(!isI(*sliceType))
        {
            auto predModeFlag = embed<PredModeFlag>(*this);

            parse(streamAccessLayer, decoder, *predModeFlag);
            cuPredMode->set(*predModeFlag);
        }
        else
        {
            cuPredMode->set(*sliceType, *cuSkipFlag);
        }

        const auto minCbSizeY = picture->minCbSizeY;

        if(PredictionMode::Intra != *cuPredMode || size == minCbSizeY)
        {
            auto partMode = embed<PartMode>(*this);

            parse(
                    streamAccessLayer, decoder,
                    *partMode,
                    *cuPredMode,
                    minCbSizeY,
                    bool(*sps->get<SPS::AmpEnabledFlag>()),
                    size);

            partModePseudo->set(*cuPredMode, *partMode);
            intraSplitFlag->set(*cuPredMode, *partMode);
        }
        else
        {
            partModePseudo->set();
        }

        if(*cuPredMode == PredictionMode::Intra)
        {
            const auto pcmEnabledFlag = sps->get<SPS::PcmEnabledFlag>();

            if(*pcmEnabledFlag)
            {
                if(
                        PartitionMode::PART_2Nx2N == *partModePseudo
                        && size >= picture->minIpcmCbSizeY
                        && size <= picture->maxIpcmCbSizeY)
                {
                    parse(streamAccessLayer, decoder, *pcmFlag);
                }
            }

            if(*pcmFlag)
            {
                if(!streamAccessLayer.isByteAligned())
                {
                    auto pcmAlignmentZeroBit = embed<PcmAlignmentZeroBit>(*this);

                    while(!streamAccessLayer.isByteAligned())
                    {
                        parse(streamAccessLayer, decoder, *pcmAlignmentZeroBit);
                    }
                }

                auto pcmSample = embedSubtree<PcmSample>(*this, cuCoord, size);

                parse(streamAccessLayer, decoder, *pcmSample);
            }
            else
            {
                auto prevIntraLumaPredFlag = embed<PrevIntraLumaPredFlag>(*this);
                auto mpmIdx = embed<MpmIdx>(*this);
                auto remIntraLumaPredMode = embed<RemIntraLumaPredMode>(*this);
                auto intraChromaPredMode = embed<IntraChromaPredMode>(*this);

                const auto isNxN = PartitionMode::PART_NxN == *partModePseudo;
                const auto pbOffset = isNxN ? nCbS / 2 : nCbS;

                const auto n = nCbS / pbOffset;

                for(auto j = 0_pel; j < n; ++j)
                {
                    for(auto i = 0_pel; i < n; ++i)
                    {
                        parse(
                                streamAccessLayer, decoder,
                                *prevIntraLumaPredFlag,
                                PelCoord{i * pbOffset, j * pbOffset}, size);
                    }
                }

                for(auto j = 0_pel; j < n; ++j)
                {
                    for(auto i = 0_pel; i < n; ++i)
                    {
                        const PelCoord at{i * pbOffset, j * pbOffset};

                        if((*prevIntraLumaPredFlag)[makeTuple(at, size)])
                        {
                            parse(streamAccessLayer, decoder, *mpmIdx, at, size);
                        }
                        else
                        {
                            parse(
                                    streamAccessLayer, decoder,
                                    *remIntraLumaPredMode, at, size);
                        }
                    }
                }

                for(auto j = 0_pel; !is400 && j < (is444 ? n : 1_pel); ++j)
                {
                    for(auto i = 0_pel; !is400 && i < (is444 ? n : 1_pel); ++i)
                    {
                        const PelCoord at{i * pbOffset, j * pbOffset};

                        parse(
                                streamAccessLayer, decoder,
                                *intraChromaPredMode, at, size, isNxN);
                    }
                }

                using namespace Decoder;
                using namespace Decoder::Processes;

                embed<IntraPredModeY>(
                        *this,
                        subprocess(
                            decoder, IntraLumaPredictionMode(), picture, *this));

                if(!is400)
                {
                    embed<IntraPredModeC>(
                            *this,
                            subprocess(
                                decoder, IntraChromaPredictionMode(), picture, *this));
                }
            }
        }
        else
        {
           mergeFlag =
               partition(*this, streamAccessLayer, decoder, cuCoord, size, *partModePseudo);
        }

        if(!*pcmFlag)
        {
            auto rqtRootCbf = embed<RqtRootCbf>(*this);

            if(
                    PredictionMode::Intra != *cuPredMode
                    && !(
                        PartitionMode::PART_2Nx2N == *partModePseudo
                        && *mergeFlag))
            {
                parse(streamAccessLayer, decoder, *rqtRootCbf);
            }

            if(*rqtRootCbf)
            {
                embed<MaxTrafoDepth>(
                        *this,
                        *sps->get<SPS::MaxTransformHierarchyDepthIntra>(),
                        *sps->get<SPS::MaxTransformHierarchyDepthInter>(),
                        *cuPredMode,
                        *intraSplitFlag);

                auto tt =
                    embedTransformTree(cuCoord, PelCoord{0_pel, 0_pel}, size, 0, 0);

                parse(streamAccessLayer, decoder, *tt, sh, ctu, *this, cuCoord);
            }
        }
    }

}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */
