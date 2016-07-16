/* STDC++ */
#include <algorithm>
/* HEVC */
#include <Syntax/ResidualCoding.h>
#include <Syntax/SequenceParameterSet.h>
#include <Syntax/SpsRangeExtension.h>
#include <Syntax/PictureParameterSet.h>
#include <Syntax/CodingUnit.h>
#include <Syntax/PredictionUnit.h>
#include <Syntax/CABAD/CtxId.h>
#include <Structure/Picture.h>
#include <Decoder/State.h>

namespace HEVC { namespace Syntax { namespace ResidualCodingContent {
/*----------------------------------------------------------------------------*/
/* CodedSubBlockFlag */
/*----------------------------------------------------------------------------*/
void CodedSubBlockFlag::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
        Plane plane, Log2 trafoSize, SubCbCoord coord)
{
    const auto contextModel =
        [this, plane, trafoSize, coord](CABAD::State &state, int binIdx) -> CABAD::Variable &
        {
            /* 04/2013, 9.3.4.2.4
             * "Derivation process of ctxInc for the syntax element coded_sub_block_flag" */
            syntaxCheck(0 == binIdx);

            auto csbfCtx = 0;
            const auto side = toSubCb(toPel(trafoSize));

            if(side - 1_sub_cb > coord.x())
            {
                csbfCtx += (*this)[{coord.x() + 1_sub_cb, coord.y()}];
            }

            if(side - 1_sub_cb > coord.y())
            {
                csbfCtx += (*this)[{coord.x(), coord.y() + 1_sub_cb}];
            }

            return
                state.getVariable(
                        CABAD::CtxId::coded_sub_block_flag,
                        Plane::Y == plane
                        ? std::min(csbfCtx, 1)
                        : 2 + std::min(csbfCtx, 1));
        };

    getFrom(streamAccessLayer, decoder, cabadState(decoder), *this, contextModel);
    m_flags[toPos(coord)] = getValue();
}
/*----------------------------------------------------------------------------*/
/* SigCoeffFlag */
/*----------------------------------------------------------------------------*/
void SigCoeffFlag::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
        const CodingUnitContent::CuTransquantBypassFlag &cuTransquantBypassFlag,
        const TransformSkipFlag &transformSkipFlag,
        const CodedSubBlockFlag &codedSubBlockFlag,
        Plane plane, ScanIdx scanIdx, Log2 trafoSize, PelCoord at)
{
    const auto contextModel =
        [
            this,
            &decoder,
            &cuTransquantBypassFlag,
            &transformSkipFlag,
            &codedSubBlockFlag,
            plane, scanIdx, trafoSize, at](
                CABAD::State &state, int binIdx) -> CABAD::Variable &
    {
        /* 04/2013 && 04/2015, 9.3.4.2.5
         * "Derivation process of ctxInc for the syntax element sig_coeff_flag" */
        syntaxCheck(0 == binIdx);

        typedef SpsRangeExtension SPSRE;

        const auto picture = decoder.picture();
        const auto spsre = picture->spsre;
        const auto transformSkipContextEnabledFlag =
            spsre && bool(*spsre->get<SPSRE::TransformSkipContextEnabledFlag>());

        /* 04/2013, 9.3.4.2.5 Table 9-39 */
        /* 04/2015, 9.3.4.2.5 Table 9-45 */
        static const int ctxIdxMap[] = {0, 1, 4, 5, 2, 3, 4, 5, 6, 6, 8, 8, 7, 7, 8};
        auto sigCtx = 0;

        if(transformSkipContextEnabledFlag && (transformSkipFlag || cuTransquantBypassFlag))
        {
            /* 04/2015 (9-31) */
            sigCtx = Plane::Y == plane ? 42 : 16;
        }
        else if(2_log2 == trafoSize)
        {
            /* 04/2013 (9-23), 04/2015 (9-32) */
            sigCtx = ctxIdxMap[toUnderlying(at.y() * 4 + at.x())];
        }
        else if(at.x() + at.y() == 0_pel)
        {
            /* 04/2013 (9-24), 04/2015 (9-33) */
            //sigCtx = 0;
        }
        else
        {
            const auto subBlkCoord = toSubCb(at);
            const PelCoord blkOffset{at.x() & 3, at.y() & 3};
            const auto side = toSubCb(toPel(trafoSize));
            auto prevCsbf = 0;

            if(side - 1_sub_cb > subBlkCoord.x())
            {
                /* 04/2013 (9-25), 04/2015 (9-34) */
                prevCsbf +=
                    codedSubBlockFlag[{subBlkCoord.x() + 1_sub_cb, subBlkCoord.y()}];
            }

            if(side - 1_sub_cb > subBlkCoord.y())
            {
                /* 04/2013 (9-26), 04/2015 (9-35) */
                prevCsbf +=
                    codedSubBlockFlag[{subBlkCoord.x(), subBlkCoord.y() + 1_sub_cb}] * 2;
            }

            if(0 == prevCsbf)
            {
                /* 04/2013 (9-27), 04/2015 (9-36) */
                sigCtx =
                    0_pel == blkOffset.x() + blkOffset.y()
                    ? 2
                    : 3_pel > blkOffset.x() + blkOffset.y();
            }
            else if(1 == prevCsbf)
            {
                /* 04/2013 (9-28), 04/2015 (9-37) */
                sigCtx = 0_pel == blkOffset.y() ? 2 : 1_pel == blkOffset.y();
            }
            else if(2 == prevCsbf)
            {
                /* 04/2013 (9-29), 04/2015 (9-38) */
                sigCtx = 0_pel == blkOffset.x() ? 2 : 1_pel == blkOffset.x();
            }
            else if(3 == prevCsbf)
            {
                /* 04/2013 (9-30), 04/2015 (9-39) */
                sigCtx = 2;
            }
            else
            {
                /* other values of prevCsbf not supported */
                syntaxCheck(false);
            }

            if(Plane::Y == plane)
            {
                if(0_sub_cb < subBlkCoord.x() + subBlkCoord.y())
                {
                    /* 04/2013 (9-31), 04/2015 (9-40) */
                    sigCtx += 3;
                }

                if(3_log2 == trafoSize)
                {
                    /* 04/2013 (9-32), 04/2015 (9-41) */
                    sigCtx += HEVC::ScanIdx::Diagonal == scanIdx ? 9 : 15;
                }
                else
                {
                    /* 04/2013 (9-33), 04/2015 (9-42) */
                    sigCtx += 21;
                }
            }
            else
            {
                if(3_log2 == trafoSize)
                {
                    /* 04/2013 (9-34), 04/2015 (9-43) */
                    sigCtx += 9;
                }
                else
                {
                    /* 04/2013 (9-35), 04/2015 (9-44) */
                    sigCtx += 12;
                }
            }
        }

        /* 04/2013 (9-36) (9-37), 04/2015 (9-45) (9-46) */
        return
            state.getVariable(
                    CABAD::CtxId::sig_coeff_flag,
                    Plane::Y == plane ? sigCtx : 27 + sigCtx);
    };

    getFrom(streamAccessLayer, decoder, cabadState(decoder), *this, contextModel);
    m_flags[toPos(at)] = getValue();
}
/*----------------------------------------------------------------------------*/
/* CoeffAbsLevelGreater1Flag */
/*----------------------------------------------------------------------------*/
void CoeffAbsLevelGreater1Flag::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
        Plane plane, PelCoord at, bool isFirstInBlock, bool isFirstInSubBlock)
{
    const auto contextModel =
        [this, plane, at, isFirstInBlock, isFirstInSubBlock](
                CABAD::State &state, int binIdx) -> CABAD::Variable &
    {
        /* 04/2013, 9.3.4.2.6
         * "Derivation process of ctxInc for the syntax element coeff_abs_level_greater1_flag" */
        syntaxCheck(0 == binIdx);

        const auto subBlkCoord = toSubCb(at);
        int ctxSet = -1;

        if(isFirstInSubBlock)
        {
            if(
                    0_sub_cb == subBlkCoord.x() && 0_sub_cb == subBlkCoord.y()
                    || Plane::Y != plane)
            {
                ctxSet = 0;
            }
            else
            {
                ctxSet = 2;
            }

            if(isFirstInBlock)
            {
                /* current sub-block is the first one to be processed
                 * for the current transform unit
                 * lastGreater1Ctx = 1 */
            }
            else
            {
                auto lastGreater1Ctx = currCtx.greater1Ctx;

                if(0 < lastGreater1Ctx)
                {
                    const auto lastGreater1Flag = currCtx.greater1Flag;

                    lastGreater1Ctx = lastGreater1Flag ? 0 : lastGreater1Ctx + 1;
                }

                if(0 == lastGreater1Ctx)
                {
                    ctxSet += 1;
                }
            }

            /* update state */
            currCtx.greater1Ctx = 1;
            currCtx.ctxSet = ctxSet;
        }
        else
        {
            const auto lastCtxSet = currCtx.ctxSet;
            ctxSet = lastCtxSet;

            auto greater1Ctx = currCtx.greater1Ctx;

            if(0 < greater1Ctx)
            {
                const auto lastGreater1Flag = currCtx.greater1Flag;

                currCtx.greater1Ctx = lastGreater1Flag ? 0 : greater1Ctx + 1;
            }
            else
            {
                currCtx.greater1Ctx = greater1Ctx;
            }

            currCtx.ctxSet = ctxSet;
        }

        return
            state.getVariable(
                    CABAD::CtxId::coeff_abs_level_greater1_flag,
                    ctxSet * 4 + std::min(3, int(currCtx.greater1Ctx))
                    + (Plane::Y == plane ? 0 : 16));
    };

    getFrom(streamAccessLayer, decoder, cabadState(decoder), *this, contextModel);
    m_flags[toPos(at)] = getValue();
    currCtx.greater1Flag = getValue();
}
/*----------------------------------------------------------------------------*/
/* CoeffAbsLevelGreater2Flag */
/*----------------------------------------------------------------------------*/
void CoeffAbsLevelGreater2Flag::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
        int ctxSet, Plane plane, PelCoord at)
{
    const auto contextModel =
        [plane, ctxSet](CABAD::State &state, int binIdx) -> CABAD::Variable &
        {
            /* 04/2013, 9.3.4.2.7
             * "Derivation process of ctxInc for the syntax element coeff_abs_level_greater2_flag" */
            syntaxCheck(0 == binIdx);

            return state.getVariable(
                    CABAD::CtxId::coeff_abs_level_greater2_flag,
                    ctxSet + (Plane::Y != plane ? 4 : 0));
        };

    getFrom(streamAccessLayer, decoder, cabadState(decoder), *this, contextModel);
    m_flags[toPos(at)] = getValue();
}
/*----------------------------------------------------------------------------*/
/* ScanIdx */
/*----------------------------------------------------------------------------*/
ScanIdx::ScanIdx(
        PelCoord coord, Log2 size, Plane plane,
        ChromaFormatIdc chromaFormatIdc,
        PredictionMode cuPredMode,
        const CodingUnit &cu,
        const ResidualCoding &):
    m_scanIdx{HEVC::ScanIdx::Diagonal}
{
    /* 04/2013, 7.4.9.11 "Residual coding semantics" */
    /* 10/2014, 7.4.9.11 "Residual coding semantics" */
    const auto is444 = ChromaFormatIdc::f444 == chromaFormatIdc;

    if(
            isIntra(cuPredMode)
            && (
                2_log2 == size
                || 3_log2 == size && Plane::Y == plane
                || 3_log2 == size && is444))
    {
        const auto predModeIntra = cu.intraPredictionMode(plane, coord);

        if(
                IntraPredictionMode::Angular5 < predModeIntra
                && IntraPredictionMode::Angular15 > predModeIntra)
        {
            m_scanIdx = HEVC::ScanIdx::Vertical;
        }
        else if(
                IntraPredictionMode::Angular21 < predModeIntra
                && IntraPredictionMode::Angular31 > predModeIntra)
        {
            m_scanIdx = HEVC::ScanIdx::Horizontal;
        }
        else
        {
            // set in constructor
            //m_scanIdx = HEVC::ScanIdx::Diagonal;
        }

    }
    else
    {
        // set in constructor
        //m_scanIdx = HEVC::ScanIdx::Diagonal;
    }
}
/*----------------------------------------------------------------------------*/
/* LastSignificantCoeffX */
LastSignificantCoeffX::LastSignificantCoeffX(const ResidualCoding &rc)
{
    const auto scanIdx = rc.get<ScanIdx>();
    const auto lastSigCoeffXPrefix = rc.get<LastSigCoeffXPrefix>();
    const auto lastSigCoeffYPrefix = rc.get<LastSigCoeffYPrefix>();

    if(HEVC::ScanIdx::Vertical == *scanIdx)
    {
        if(3 < *lastSigCoeffYPrefix)
        {
            const auto lastSigCoeffYSuffix = rc.get<LastSigCoeffYSuffix>();

            set(*lastSigCoeffYPrefix, *lastSigCoeffYSuffix);
        }
        else
        {
            set(*lastSigCoeffYPrefix);
        }
    }
    else
    {
        if(3 < *lastSigCoeffXPrefix)
        {
            const auto lastSigCoeffXSuffix = rc.get<LastSigCoeffXSuffix>();

            set(*lastSigCoeffXPrefix, *lastSigCoeffXSuffix);
        }
        else
        {
            set(*lastSigCoeffXPrefix);
        }
    }

    log(LogId::Syntax, align(getName(Id)), " ?", '\t', inUnits(), '\n');
}
/*----------------------------------------------------------------------------*/
LastSignificantCoeffY::LastSignificantCoeffY(const ResidualCoding &rc)
{
    const auto scanIdx = rc.get<ScanIdx>();
    const auto lastSigCoeffXPrefix = rc.get<LastSigCoeffXPrefix>();
    const auto lastSigCoeffYPrefix = rc.get<LastSigCoeffYPrefix>();

    if(HEVC::ScanIdx::Vertical == *scanIdx)
    {
        if(3 < *lastSigCoeffXPrefix)
        {
            const auto lastSigCoeffXSuffix = rc.get<LastSigCoeffXSuffix>();

            set(*lastSigCoeffXPrefix, *lastSigCoeffXSuffix);
        }
        else
        {
            set(*lastSigCoeffXPrefix);
        }
    }
    else
    {
        if(3 < *lastSigCoeffYPrefix)
        {
            const auto lastSigCoeffYSuffix = rc.get<LastSigCoeffYSuffix>();

            set(*lastSigCoeffYPrefix, *lastSigCoeffYSuffix);
        }
        else
        {
            set(*lastSigCoeffYPrefix);
        }
    }

    log(LogId::Syntax, align(getName(Id)), " ?", '\t', inUnits(), '\n');
}
/*----------------------------------------------------------------------------*/
} /* ResidualCodingContent */

/*----------------------------------------------------------------------------*/
/* ResidualCoding */
/*----------------------------------------------------------------------------*/
void ResidualCoding::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
        const CodingUnit &cu)
{
    const auto rcCoord = get<Coord>()->inUnits();
    const auto rcSize = get<Size>()->inUnits();
    const Plane plane = *get<CIdx>();

    const auto picture = decoder.picture();
    const auto pps = picture->pps;
    const auto chromaFormatIdc = picture->chromaFormatIdc;
    const auto bitDepth = picture->bitDepth(plane);

    typedef SequenceParameterSet SPS;
    typedef SpsRangeExtension SPSRE;
    typedef PictureParameterSet PPS;
    typedef CodingUnit CU;

    const auto spsre = picture->spsre;
    const auto implicitRdpcmEnabledFlag =
        spsre && bool(*spsre->get<SPSRE::ImplicitRdpcmEnabledFlag>());
    const auto explicitRdpcmEnabledFlag =
        spsre && bool(*spsre->get<SPSRE::ExplicitRdpcmEnabledFlag>());
    const auto extendedPrecisionProcessingFlag =
        spsre && bool(*spsre->get<SPSRE::ExtendedPrecisionProcessingFlag>());
    const auto persistentRiceAdaptationEnabledFlag =
        spsre && bool(*spsre->get<SPSRE::PersistentRiceAdaptationEnabledFlag>());
    const auto cabacBypassAlignmentEnabledFlag =
        spsre && bool(*spsre->get<SPSRE::CabacBypassAlignmentEnabledFlag>());

    const auto transformSkipEnabledFlag = pps->get<PPS::TransformSkipEnabledFlag>();

    const auto cuTransquantBypassFlag = cu.get<CU::CuTransquantBypassFlag>();
    const auto cuPredMode = cu.get<CU::CuPredMode>();

    const auto inPlaneCoord = scale(rcCoord, plane, picture->chromaFormatIdc);
    /* start: inferrable */
    auto scanIdx =
        embed<ScanIdx>(
                *this,
                rcCoord,
                rcSize,
                plane,
                chromaFormatIdc,
                *cuPredMode,
                cu,
                *this);
    /* end: inferrable */

    auto transformSkipFlag = embed<TransformSkipFlag>(*this);

    if(
            *transformSkipEnabledFlag
            && !*cuTransquantBypassFlag
            && picture->maxTransformSkipSize >= rcSize)
    {
        parse(streamAccessLayer, decoder, *transformSkipFlag, plane);
    }

    auto explicitRdpcmFlag = embed<ExplicitRdpcmFlag>(*this);

    if(
            explicitRdpcmEnabledFlag
            && isInter(*cuPredMode)
            && (*transformSkipFlag || *cuTransquantBypassFlag))
    {
        parse(streamAccessLayer, decoder, *explicitRdpcmFlag, plane);

        if(*explicitRdpcmFlag)
        {
            parse(
                    streamAccessLayer, decoder, *embed<ExplicitRdpcmDirFlag>(*this),
                    plane);
        }
    }

    auto lastSigCoeffXPrefix = embed<LastSigCoeffXPrefix>(*this, rcSize);
    auto lastSigCoeffYPrefix = embed<LastSigCoeffYPrefix>(*this, rcSize);

    parse(streamAccessLayer, decoder, *lastSigCoeffXPrefix, rcSize, plane);
    parse(streamAccessLayer, decoder, *lastSigCoeffYPrefix, rcSize, plane);

    if(3 < *lastSigCoeffXPrefix)
    {
        auto lastSigCoeffXSuffix = embed<LastSigCoeffXSuffix>(*this, *lastSigCoeffXPrefix);

        parse(streamAccessLayer, decoder, *lastSigCoeffXSuffix);
    }

    if(3 < *lastSigCoeffYPrefix)
    {
        auto lastSigCoeffYSuffix = embed<LastSigCoeffYSuffix>(*this, *lastSigCoeffYPrefix);

        parse(streamAccessLayer, decoder, *lastSigCoeffYSuffix);
    }

    auto lastSignificantCoeffX = embed<LastSignificantCoeffX>(*this, *this);
    auto lastSignificantCoeffY = embed<LastSignificantCoeffY>( *this, *this);

    const PelCoord lastCoeffCoord
    {
            lastSignificantCoeffX->inUnits(),
            lastSignificantCoeffY->inUnits()
    };

    const auto lastCoeffAddr =
        toAddr(*scanIdx, lastCoeffCoord % toPel(1_sub_cb), toPel(1_sub_cb));
    int lastScanPos = toUnderlying(lastCoeffAddr);
    const auto lastSubBlkAddr =
        toAddr(*scanIdx, toSubCb(lastCoeffCoord), toSubCb(toPel(rcSize)));
    int lastSubBlock = toUnderlying(lastSubBlkAddr);

    auto codedSubBlockFlag =
        embed<CodedSubBlockFlag>(*this, *lastSignificantCoeffX, *lastSignificantCoeffY);
    auto sigCoeffFlag =
        embed<SigCoeffFlag>(*this, *lastSignificantCoeffX, *lastSignificantCoeffY);

    auto coeffAbsLevelGreater1Flag = embed<CoeffAbsLevelGreater1Flag>(*this);
    auto coeffAbsLevelGreater2Flag = embed<CoeffAbsLevelGreater2Flag>(*this);
    auto coeffSignFlag = embed<CoeffSignFlag>(*this);
    auto coeffAbsLevelRemaining = embed<CoeffAbsLevelRemaining>(*this);
    auto &residuals = picture->pelBuffer(PelLayerId::Residual, plane);

    for(int i = lastSubBlock; i >= 0; --i)
    {
        auto escapeDataPresent = false;
        const auto subBlkCoord = toCoord(*scanIdx, SubCb(i), toSubCb(toPel(rcSize)));

        auto inferSbDcSigCoeffFlag = false;

        if(lastSubBlock > i && 0 < i)
        {
            parse(
                    streamAccessLayer, decoder, *codedSubBlockFlag,
                    plane, rcSize, subBlkCoord);
            inferSbDcSigCoeffFlag = true;
        }

        const auto isSubBlockCoded = (*codedSubBlockFlag)[subBlkCoord];

        for(
                int n = (lastSubBlock == i ? lastScanPos - 1 : 15);
                0 <= n;
                --n)
        {
            const auto coeffCoord =
                toPel(subBlkCoord)
                + toCoord(*scanIdx, Pel(n), toPel(1_sub_cb));

            if(0 == n && isSubBlockCoded && inferSbDcSigCoeffFlag)
            {
                // inferred
                (*sigCoeffFlag)[coeffCoord] = true;
            }

            if(isSubBlockCoded && (0 < n || !inferSbDcSigCoeffFlag))
            {
                parse(
                        streamAccessLayer, decoder, *sigCoeffFlag,
                        *cuTransquantBypassFlag,
                        *transformSkipFlag,
                        *codedSubBlockFlag, plane, *scanIdx, rcSize, coeffCoord);

                if((*sigCoeffFlag)[coeffCoord])
                {
                    inferSbDcSigCoeffFlag = false;
                }
            }
        }

        int firstSigScanPos = 16;
        int lastSigScanPos = -1;
        int numGreater1Flag = 0;
        int lastGreater1ScanPos = -1;
        int lastGreater1CtxSet = 0;

        for(int n = 15; n >= 0; --n)
        {
            const auto coeffCoord =
                toPel(subBlkCoord)
                + toCoord(*scanIdx, Pel(n), toPel(1_sub_cb));

            if((*sigCoeffFlag)[coeffCoord])
            {
                if(8 > numGreater1Flag)
                {
                    parse(
                            streamAccessLayer, decoder, *coeffAbsLevelGreater1Flag,
                            plane, coeffCoord,
                            lastSubBlock == i && 0 == numGreater1Flag,
                            0 == numGreater1Flag);

                    ++numGreater1Flag;

                    if(
                            (*coeffAbsLevelGreater1Flag)[coeffCoord]
                            && -1 == lastGreater1ScanPos)
                    {
                        lastGreater1ScanPos = n;
                        lastGreater1CtxSet = coeffAbsLevelGreater1Flag->currCtx.ctxSet;
                    }
                    else if((*coeffAbsLevelGreater1Flag)[coeffCoord])
                    {
                        escapeDataPresent = true;
                    }
                }
                else
                {
                    escapeDataPresent = true;
                }

                if(-1 == lastSigScanPos)
                {
                    lastSigScanPos = n;
                }

                firstSigScanPos = n;
            }
        }

        const auto signHidden =
            *cuTransquantBypassFlag
            || (
                    isIntra(*cuPredMode)
                    && implicitRdpcmEnabledFlag
                    && *transformSkipFlag
                    && (
                        IntraPredictionMode::Angular10 == cu.intraPredictionMode(plane, rcCoord)
                        || IntraPredictionMode::Angular26 == cu.intraPredictionMode(plane, rcCoord))
                    || *explicitRdpcmFlag)
            ? false
            : lastSigScanPos - firstSigScanPos > 3;

        if(-1 != lastGreater1ScanPos)
        {
            const auto lastGreater1Coord =
                toPel(subBlkCoord)
                + toCoord(*scanIdx, Pel(lastGreater1ScanPos), toPel(1_sub_cb));

            parse(
                    streamAccessLayer, decoder,
                    *coeffAbsLevelGreater2Flag,
                    lastGreater1CtxSet, plane, lastGreater1Coord);

            if((*coeffAbsLevelGreater2Flag)[lastGreater1Coord])
            {
                escapeDataPresent = true;
            }
        }

        const auto signDataHidingEnabledFlag = pps->get<PPS::SignDataHidingEnabledFlag>();

        for(int n = 15; 0 <= n; --n)
        {
            const auto coeffCoord =
                toPel(subBlkCoord)
                + toCoord(*scanIdx, Pel(n), toPel(1_sub_cb));

            if(
                    (*sigCoeffFlag)[coeffCoord]
                    && (
                        !(*signDataHidingEnabledFlag)
                        || !signHidden
                        || (firstSigScanPos != n)))
            {
                if(cabacBypassAlignmentEnabledFlag && escapeDataPresent)
                {
                    cabadState(decoder).arithmeticDecoder.align();
                }

                parse(streamAccessLayer, decoder, *coeffSignFlag, coeffCoord);
            }
        }

        int numSigCoeff = 0,  sumAbsLevel = 0;
        auto isFirstInSubBlock = true;

        for(int n = 15; 0 <= n; --n)
        {
            const auto coeffCoord = toPel(subBlkCoord) + toCoord(*scanIdx, Pel(n), toPel(1_sub_cb));

            if((*sigCoeffFlag)[coeffCoord])
            {
                auto baseLevel =
                    1
                    + (*coeffAbsLevelGreater1Flag)[coeffCoord]
                    + (*coeffAbsLevelGreater2Flag)[coeffCoord];
                /* inferred */
                auto absLevel = 0;

                int X = n == lastGreater1ScanPos ? 3 : 2;

                if(baseLevel == (8 > numSigCoeff ? X : 1))
                {
                    if(cabacBypassAlignmentEnabledFlag && escapeDataPresent)
                    {
                        cabadState(decoder).arithmeticDecoder.align();
                    }

                    parse(
                            streamAccessLayer, decoder,
                            *coeffAbsLevelRemaining,
                            baseLevel,
                            isFirstInSubBlock,
                            bool(*cuTransquantBypassFlag),
                            bool(*transformSkipFlag),
                            extendedPrecisionProcessingFlag,
                            persistentRiceAdaptationEnabledFlag,
                            plane,
                            bitDepth);

                    absLevel = coeffAbsLevelRemaining->absLevel();
                    isFirstInSubBlock = false;
                }

                int16_t transCoeffLevel =
                    (absLevel + baseLevel) * (1 - 2 * (*coeffSignFlag)[coeffCoord]);

                if(*signDataHidingEnabledFlag && signHidden)
                {
                    sumAbsLevel += absLevel + baseLevel;

                    if(firstSigScanPos == n && 1 == sumAbsLevel % 2)
                    {
                        transCoeffLevel *= -1;
                    }
                }

                residuals[inPlaneCoord + coeffCoord] = transCoeffLevel;
                ++numSigCoeff;
            }
        }
    }

    const auto toStr =
        [inPlaneCoord, rcSize, plane, &residuals](std::ostream &oss)
        {
            oss << inPlaneCoord << ' ' << getName(plane) << '\n';

            for(auto y = 0_pel; y < toPel(rcSize); ++y)
            {
                for(auto x = 0_pel; x < toPel(rcSize); ++x)
                {
                    oss
                        << std::hex << std::setw(4) << std::setfill('0')
                        << residuals[inPlaneCoord + PelCoord{x, y}] << ' ';
                }

                oss << '\n';
            }
        };

    log({LogId::TransformCoeffLevels}, toStr);
}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */
