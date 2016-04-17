#include <Syntax/SliceSegmentHeader.h>
#include <Syntax/PictureParameterSet.h>
#include <Syntax/PpsRangeExtension.h>
#include <Syntax/SequenceParameterSet.h>
#include <Structure/Picture.h>
#include <Structure/RPSP.h>
#include <Decoder/State.h>
/* STDC++ */
#include <algorithm>

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
void SliceSegmentHeader::toStr(std::ostream &os) const
{
    if(!*get<DependentSliceSegmentFlag>())
    {
        os
            << getName(FirstSliceSegmentInPicFlag::Id) << ' '
            << bool(*get<FirstSliceSegmentInPicFlag>()) << ' '
            << getName(SliceSegmentAddress::Id) << ' '
            << get<SliceSegmentAddress>()->inUnits() << ' '
            << getName(SliceAddrRs::Id) << ' '
            << get<SliceAddrRs>()->inUnits() << ' '
            << getName(SliceType::Id) << ' '
            << getName(HEVC::SliceType(*get<SliceType>())) << ' '
            << getName(SliceSaoLumaFlag::Id) << ' '
            << bool(*get<SliceSaoLumaFlag>()) << ' '
            << getName(SliceSaoChromaFlag::Id) << ' '
            << bool(*get<SliceSaoChromaFlag>()) << ' '
            << getName(DeblockingFilterOverrideFlag::Id) << ' '
            << bool(*get<DeblockingFilterOverrideFlag>()) << ' '
            << getName(SliceDeblockingFilterDisabledFlag::Id) << ' '
            << bool(*get<SliceDeblockingFilterDisabledFlag>()) << ' '
            << getName(SliceLoopFilterAcrossSlicesEnabledFlag::Id) << ' '
            << bool(*get<SliceLoopFilterAcrossSlicesEnabledFlag>()) << '\n';
    }
}
/*----------------------------------------------------------------------------*/
void SliceSegmentHeader::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
        NalUnitType nalUnitType)
{
    /* start: inferrable */
    auto noOutputOfPriorPicsFlag = embed<NoOutputOfPriorPicsFlag>(*this);
    auto dependentSliceSegmentFlag = embed<DependentSliceSegmentFlag>(*this);
    auto sliceSegmentAddress = embed<SliceSegmentAddress>(*this);
    auto picOutputFlag = embed<PicOutputFlag>(*this);
    auto slicePicOrderCntLsb = embed<SlicePicOrderCntLsb>(*this);
    auto shortTermRefPicSetIdx = embed<ShortTermRefPicSetIdx>(*this);
    auto numLongTermSps = embed<NumLongTermSps>(*this);
    auto numLongTermPics = embed<NumLongTermPics>(*this);
    auto ltIdxSps = embed<LtIdxSps>(*this);
    auto deltaPocMsbCycleLt = embed<DeltaPocMsbCycleLt>(*this);
    auto sliceTemporalMvpEnableFlag = embed<SliceTemporalMvpEnableFlag>(*this);
    auto sliceSaoLumaFlag = embed<SliceSaoLumaFlag>(*this);
    auto sliceSaoChromaFlag = embed<SliceSaoChromaFlag>(*this);
    auto cabacInitFlag = embed<CabacInitFlag>(*this);
    auto collocatedFromL0Flag = embed<CollocatedFromL0Flag>(*this);
    auto sliceCbQpOffset = embed<SliceCbQpOffset>(*this);
    auto sliceCrQpOffset = embed<SliceCrQpOffset>(*this);
    auto cuChromaQpOffsetEnabledFlag = embed<CuChromaQpOffsetEnabledFlag>(*this);
    auto deblockingFilterOverrideFlag = embed<DeblockingFilterOverrideFlag>(*this);
    auto numEntryPointOffsets = embed<NumEntryPointOffsets>(*this);
    /* end: inferrable */

    auto firstSliceSegmentInPicFlag = embed<FirstSliceSegmentInPicFlag>(*this);

    parse(streamAccessLayer, decoder, *firstSliceSegmentInPicFlag);

    if(isIRAP(nalUnitType))
    {
        parse(streamAccessLayer, decoder, *noOutputOfPriorPicsFlag);
    }

    auto slicePicParameterSetId = embed<SlicePicParameterSetId>(*this);

    parse(streamAccessLayer, decoder, *slicePicParameterSetId);

    const auto rpsp = decoder.context()->get<Structure::RPSP>();
    const auto picture = decoder.picture();
    const auto sps = picture->sps;
    const auto pps = picture->pps;
    const auto ppsre = picture->ppsre;

    typedef SequenceParameterSet SPS;
    typedef PictureParameterSet PPS;
    typedef PpsRangeExtension PPSRE;

    const auto chromaFormatIdc = picture->chromaFormatIdc;

    /* start: inferrable from PPS */
    auto numRefIdxL0ActiveMinus1 =
        embed<NumRefIdxL0ActiveMinus1>(
                *this,
                *pps->get<PPS::NumRefIdxL0DefaultActiveMinus1>());

    auto numRefIdxL1ActiveMinus1 =
        embed<NumRefIdxL1ActiveMinus1>(
                *this,
                *pps->get<PPS::NumRefIdxL1DefaultActiveMinus1>());

    auto sliceDeblockingFilterDisabledFlag =
        embed<SliceDeblockingFilterDisabledFlag>(
                *this,
                *pps->get<PPS::PpsDeblockingFilterDisabledFlag>());

    auto sliceBetaOffsetDiv2 =
        embed<SliceBetaOffsetDiv2>(
                *this,
                *pps->get<PPS::PpsBetaOffsetDiv2>());

    auto sliceTcOffsetDiv2 =
        embed<SliceTcOffsetDiv2>(
                *this,
                *pps->get<PPS::PpsTcOffsetDiv2>());

    auto sliceLoopFilterAcrossSlicesEnabledFlag =
        embed<SliceLoopFilterAcrossSlicesEnabledFlag>(
                *this,
                *pps->get<PPS::PpsLoopFilterAcrossSlicesEnabledFlag>());
    /* end: inferrable from PPS */

    if(!(*firstSliceSegmentInPicFlag))
    {
        if(*pps->get<PPS::DependentSliceSegmentsEnabledFlag>())
        {
            parse(streamAccessLayer, decoder, *dependentSliceSegmentFlag);
        }

        parse(
                streamAccessLayer, decoder, *sliceSegmentAddress,
                picture->sizeInCtbsY);
    }

    log(
            LogId::All,
            "PPS id ",
                int(*slicePicParameterSetId),
                " slice segment address ",
                toUnderlying(Ctb(*sliceSegmentAddress)),
                " is dependent ",
                bool(*dependentSliceSegmentFlag),
                '\n');

    const auto calcSliceAddrRs =
        [picture, dependentSliceSegmentFlag, sliceSegmentAddress]()
        {
            if(*dependentSliceSegmentFlag)
            {
                const auto addrInTs = picture->toAddrInTs(*sliceSegmentAddress);
                const auto prevAddrInTs = addrInTs - 1_ctb;
                const auto prevAddrInRs = picture->toAddrInRs(prevAddrInTs);
                return picture->slice(picture->toCoord(prevAddrInRs))->addr().inRs;
            }
            else
            {
                return Ctb(*sliceSegmentAddress);
            }
        };

    embed<SliceAddrRs>(*this, calcSliceAddrRs());

    if(!(*dependentSliceSegmentFlag))
    {
        const auto numExtraSliceHeaderBits = pps->get<PPS::NumExtraSliceHeaderBits>();

        if(0 < *numExtraSliceHeaderBits)
        {
            auto sliceReservedFlag = embed<SliceReservedFlag>(*this);

            for(auto i = 0; i < *numExtraSliceHeaderBits; ++i)
            {
                parse(streamAccessLayer, decoder, *sliceReservedFlag, i);
            }
        }

        auto sliceType = embed<SliceType>(*this);

        parse(streamAccessLayer, decoder, *sliceType);

        const auto outputFlagPresentFlag = pps->get<PPS::OutputFlagPresentFlag>();

        if(*outputFlagPresentFlag)
        {
            parse(streamAccessLayer, decoder, *picOutputFlag);
        }

        const auto separateColourPlaneFlag = sps->get<SPS::SeparateColourPlaneFlag>();

        if(*separateColourPlaneFlag)
        {
            auto colourPlaneId = embed<ColourPlaneId>(*this);

            parse(streamAccessLayer, decoder, *colourPlaneId);
        }

        if(!isIDR(nalUnitType))
        {
            auto shortTermRefPicSetSpsFlag = embed<ShortTermRefPicSetSpsFlag>(*this);

            parse(
                    streamAccessLayer, decoder, *slicePicOrderCntLsb,
                    *sps->get<SPS::MaxPicOrderCntLsbMinus4>());
            parse(streamAccessLayer, decoder, *shortTermRefPicSetSpsFlag);

            const auto numShortTermRefPicSets = sps->get<SPS::NumShortTermRefPicSets>();

            if(!(*shortTermRefPicSetSpsFlag))
            {
                auto shortTermRefPicSet = embedSubtree<ShortTermRefPicSet>(*this);

                parse(
                        streamAccessLayer, decoder,
                        *shortTermRefPicSet,
                        *numShortTermRefPicSets,
                        *numShortTermRefPicSets,
                        *rpsp);
            }
            else
            {
                parse(
                        streamAccessLayer, decoder, *shortTermRefPicSetIdx,
                        *numShortTermRefPicSets);
            }

            /* pseudo-elements */
            embed<CurrRpsIdx>(
                    *this,
                    *shortTermRefPicSetSpsFlag,
                    *shortTermRefPicSetIdx,
                    *numShortTermRefPicSets);

            const auto longTermRefPicsPresentFlag =
                sps->get<SPS::LongTermRefPicsPresentFlag>();

            if(*longTermRefPicsPresentFlag)
            {
                const auto numLongTermRefPicsSps =
                    sps->get<SPS::NumLongTermRefPicsSps>();

                if(0 < *numLongTermRefPicsSps)
                {
                    parse(streamAccessLayer, decoder, *numLongTermSps);
                }

                parse(streamAccessLayer, decoder, *numLongTermPics);

                const auto numLongTerm = *numLongTermSps + *numLongTermPics;

                if(0 < numLongTerm)
                {
                    auto deltaPocMsbPresentFlag = embed<DeltaPocMsbPresentFlag>(*this);
                    auto pocLsbLt = embed<PocLsbLt>(*this);
                    auto usedByCurrPicLtFlag = embed<UsedByCurrPicLtFlag>(*this);

                    for(auto i = 0; i < numLongTerm; ++i)
                    {
                        if(i < *numLongTermSps)
                        {
                            if(*numLongTermRefPicsSps > 1)
                            {
                                parse(
                                        streamAccessLayer, decoder, *ltIdxSps,
                                        *numLongTermRefPicsSps, i);
                            }
                        }
                        else
                        {
                            parse(
                                    streamAccessLayer, decoder, *pocLsbLt,
                                    *sps->get<SPS::MaxPicOrderCntLsbMinus4>(), i);

                            parse(streamAccessLayer, decoder, *usedByCurrPicLtFlag, i);
                        }

                        parse(streamAccessLayer, decoder, *deltaPocMsbPresentFlag, i);

                        if((*deltaPocMsbPresentFlag)[i])
                        {
                            parse(streamAccessLayer, decoder, *deltaPocMsbCycleLt, i);
                        }
                    }
                }
            }

            rpsp->derive(*sps, *this);

            const auto spsTemporalMvpEnableFlag =
                sps->get<SPS::SpsTemporalMvpEnableFlag>();

            if(*spsTemporalMvpEnableFlag)
            {
                parse(streamAccessLayer, decoder, *sliceTemporalMvpEnableFlag);
            }
        }

        const auto sampleAdaptiveOffsetEnabledFlag =
            sps->get<SPS::SampleAdaptiveOffsetEnabledFlag>();

        if(*sampleAdaptiveOffsetEnabledFlag)
        {
            parse(streamAccessLayer, decoder, *sliceSaoLumaFlag);

            if(ChromaFormatIdc::f400 != chromaFormatIdc)
            {
                parse(streamAccessLayer, decoder, *sliceSaoChromaFlag);
            }
        }

        if(isP(*sliceType) || isB(*sliceType))
        {
            auto numRefIdxActiveOverrideFlag = embed<NumRefIdxActiveOverrideFlag>(*this);

            parse(streamAccessLayer, decoder, *numRefIdxActiveOverrideFlag);

            if(*numRefIdxActiveOverrideFlag)
            {
                parse(streamAccessLayer, decoder, *numRefIdxL0ActiveMinus1);

                if(isB(*sliceType))
                {
                    parse(streamAccessLayer, decoder, *numRefIdxL1ActiveMinus1);
                }
            }

            const auto listsModificationPresentFlag =
                pps->get<PPS::ListsModificationPresentFlag>();

            if(*listsModificationPresentFlag && rpsp->numPocTotalCurr > 1)
            {
                auto refPicListModification = embedSubtree<RefPicListModification>(*this);

                parse(
                        streamAccessLayer, decoder, *refPicListModification,
                        *this, *rpsp);
            }

            if(isB(*sliceType))
            {
                auto mvdL1ZeroFlag = embed<MvdL1ZeroFlag>(*this);

                parse(streamAccessLayer, decoder, *mvdL1ZeroFlag);
            }

            const auto cabacInitPresentFlag = pps->get<PPS::CabacInitPresentFlag>();

            if(*cabacInitPresentFlag)
            {
                parse(streamAccessLayer, decoder, *cabacInitFlag);
            }

            if(*sliceTemporalMvpEnableFlag)
            {
                if(isB(*sliceType))
                {
                    parse(streamAccessLayer, decoder, *collocatedFromL0Flag);
                }

                auto collocatedRefIdx = embed<CollocatedRefIdx>(*this);

                if(
                        *collocatedFromL0Flag && 0 < *numRefIdxL0ActiveMinus1
                        || !(*collocatedFromL0Flag) && 0 < *numRefIdxL1ActiveMinus1)
                {
                    parse(streamAccessLayer, decoder, *collocatedRefIdx);
                }
            }

            const auto weightedPredFlag = pps->get<PPS::WeightedPredFlag>();
            const auto weightedBipredFlag = pps->get<PPS::WeightedBipredFlag>();

            if(
                    *weightedPredFlag && isP(*sliceType)
                    || *weightedBipredFlag && isB(*sliceType))
            {
                auto predWeightTable = embedSubtree<PredWeightTable>(*this);

                parse(streamAccessLayer, decoder, *predWeightTable, *this);
            }

            auto fiveMinusMaxNumMergeCand = embed<FiveMinusMaxNumMergeCand>(*this);

            parse(streamAccessLayer, decoder, *fiveMinusMaxNumMergeCand);

            embed<MaxNumMergeCand>(*this, *fiveMinusMaxNumMergeCand);
        }

        auto sliceQpDelta = embed<SliceQpDelta>(*this);

        parse(streamAccessLayer, decoder, *sliceQpDelta);

        const auto ppsSliceChromaQpOffsetsPresentFlag =
            pps->get<PPS::PpsSliceChromaQpOffsetsPresentFlag>();

        if(*ppsSliceChromaQpOffsetsPresentFlag)
        {
            parse(streamAccessLayer, decoder, *sliceCbQpOffset);
            parse(streamAccessLayer, decoder, *sliceCrQpOffset);
        }

        if(ppsre && *ppsre->get<PPSRE::ChromaQpOffsetListEnabledFlag>())
        {
            parse(streamAccessLayer, decoder, *cuChromaQpOffsetEnabledFlag);
        }

        const auto deblockingFilterControlPresentFlag =
            pps->get<PPS::DeblockingFilterControlPresentFlag>();

        if(*deblockingFilterControlPresentFlag)
        {
            const auto deblockingFilterOverrideEnabledFlag =
                pps->get<PPS::DeblockingFilterOverrideEnabledFlag>();

            if(*deblockingFilterOverrideEnabledFlag)
            {
                parse(streamAccessLayer, decoder, *deblockingFilterOverrideFlag);
            }

            if(*deblockingFilterOverrideFlag)
            {
                parse(streamAccessLayer, decoder, *sliceDeblockingFilterDisabledFlag);

                if(!*sliceDeblockingFilterDisabledFlag)
                {
                    parse(streamAccessLayer, decoder, *sliceBetaOffsetDiv2);
                    parse(streamAccessLayer, decoder, *sliceTcOffsetDiv2);
                }
            }
        }

        const auto loopFilterAcrossSlicesEnabledFlag =
            pps->get<PPS::PpsLoopFilterAcrossSlicesEnabledFlag>();

        if(
                *loopFilterAcrossSlicesEnabledFlag
                && (
                    *sliceSaoLumaFlag
                    || *sliceSaoChromaFlag
                    || !(*sliceDeblockingFilterDisabledFlag)))
        {
            parse(streamAccessLayer, decoder, *sliceLoopFilterAcrossSlicesEnabledFlag);
        }

        /* pseudo syntax elements */
        embed<SliceQpY>(*this, *pps->get<PPS::InitQpMinus26>(), *get<SliceQpDelta>());
        embed<CabacInitType>(*this, *get<SliceType>(), *cabacInitFlag);
    }

    const auto tilesEnabledFlag = pps->get<PPS::TilesEnabledFlag>();
    const auto entropyCodingSyncEnabledFlag =
        pps->get<PPS::EntropyCodingSyncEnabledFlag>();

    if(*tilesEnabledFlag || *entropyCodingSyncEnabledFlag)
    {
        parse(streamAccessLayer, decoder, *numEntryPointOffsets);

        if(0 < *numEntryPointOffsets)
        {
            auto offsetLenMinus1 = embed<OffsetLenMinus1>(*this);

            parse(streamAccessLayer, decoder, *offsetLenMinus1);

            auto entryPointOffsetMinus1 = embed<EntryPointOffsetMinus1>(*this, *offsetLenMinus1);

            for(auto i = 0; i < *numEntryPointOffsets; ++i)
            {
                parse(streamAccessLayer, decoder, *entryPointOffsetMinus1, i);
            }
        }
    }

    const auto sliceSegmentHeaderExtensionPresentFlag =
        pps->get<PPS::SliceSegmentHeaderExtensionPresentFlag>();

    if(*sliceSegmentHeaderExtensionPresentFlag)
    {
        auto sliceSegmentHeaderExtensionLength =
            embed<SliceSegmentHeaderExtensionLength>(*this);

        parse(streamAccessLayer, decoder, *sliceSegmentHeaderExtensionLength);

        if(0 < *sliceSegmentHeaderExtensionLength)
        {
            auto sliceSegmentHeaderExtensionDataByte =
                embed<SliceSegmentHeaderExtensionDataByte>(*this);

            for(auto i = 0; i < *sliceSegmentHeaderExtensionLength; ++i)
            {
                parse(streamAccessLayer, decoder, *sliceSegmentHeaderExtensionDataByte, i);
            }
        }
    }

    auto byteAlignment = embedSubtree<ByteAlignment>(*this);

    parse(streamAccessLayer, decoder, *byteAlignment, ElementId{Id});
}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */
