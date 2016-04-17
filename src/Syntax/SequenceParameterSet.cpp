#include <Syntax/SequenceParameterSet.h>
#include <Syntax/Rbsp.h>
#include <Decoder/State.h>

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
void SequenceParameterSet::toStr(std::ostream &os) const
{
    os
        << getName(ChromaFormatIdc::Id) << ' '
        << getName(HEVC::ChromaFormatIdc(*get<ChromaFormatIdc>())) << ' '
        << getName(PicWidthInLumaSamples::Id) << ' '
        << get<PicWidthInLumaSamples>()->inUnits() << ' '
        << getName(PicHeightInLumaSamples::Id) << ' '
        << get<PicHeightInLumaSamples>()->inUnits() << ' '
        << getName(MinLumaCodingBlockSizeMinus3::Id) << ' '
        << get<MinLumaCodingBlockSizeMinus3>()->inUnits() << ' '
        << getName(DiffMaxMinLumaCodingBlockSize::Id) << ' '
        << get<DiffMaxMinLumaCodingBlockSize>()->inUnits() << ' '
        << getName(SampleAdaptiveOffsetEnabledFlag::Id) << ' '
        << bool(*get<SampleAdaptiveOffsetEnabledFlag>()) << ' '
        << getName(PcmEnabledFlag::Id) << ' '
        << bool(*get<PcmEnabledFlag>()) << ' '
        << getName(PcmLoopFilterDisabledFlag::Id) << ' '
        << bool(*get<PcmLoopFilterDisabledFlag>());
}
/*----------------------------------------------------------------------------*/
void SequenceParameterSet::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder)
{
    /* start: inferrable */
    auto separateColourPlaneFlag = embed<SeparateColourPlaneFlag>(*this);
    auto confWinLeftOffset = embed<ConfWinLeftOffset>(*this);
    auto confWinRightOffset = embed<ConfWinRightOffset>(*this);
    auto confWinTopOffset = embed<ConfWinTopOffset>(*this);
    auto confWinBottomOffset = embed<ConfWinBottomOffset>(*this);
    auto spsRangeExtensionFlag = embed<SpsRangeExtensionFlag>(*this);
    auto spsMultilayerExtensionFlag = embed<SpsMultilayerExtensionFlag>(*this);
    auto spsExtension6Bits = embed<SpsExtension6Bits>(*this);
    /* end: inferrable */

    auto spsVideoParameterSetId = embed<SpsVideoParameterSetId>(*this);

    parse(streamAccessLayer, decoder, *spsVideoParameterSetId);

    auto spsMaxSubLayersMinus1 = embed<SpsMaxSubLayersMinus1>(*this);
    auto spsTemporalIdNestingFlag = embed<SpsTemporalIdNestingFlag>(*this);

    parse(streamAccessLayer, decoder, *spsMaxSubLayersMinus1);
    parse(streamAccessLayer, decoder, *spsTemporalIdNestingFlag);

    parse(
            streamAccessLayer, decoder, *embedSubtree<ProfileTierLevel>(*this),
            true, *spsMaxSubLayersMinus1);

    auto seqParameterSetId = embed<SeqParameterSetId>(*this);

    parse(streamAccessLayer, decoder, *seqParameterSetId);

    auto chromaFormatIdc = embed<ChromaFormatIdc>(*this);

    parse(streamAccessLayer, decoder, *chromaFormatIdc);

    if(HEVC::ChromaFormatIdc::f444 == *chromaFormatIdc)
    {
        parse(streamAccessLayer, decoder, *separateColourPlaneFlag);
    }

    auto picWidthInLumaSamples = embed<PicWidthInLumaSamples>(*this);
    auto picHeightInLumaSamples = embed<PicHeightInLumaSamples>(*this);

    parse(streamAccessLayer, decoder, *picWidthInLumaSamples);
    parse(streamAccessLayer, decoder, *picHeightInLumaSamples);

    auto conformanceWindowFlag = embed<ConformanceWindowFlag>(*this);

    parse(streamAccessLayer, decoder, *conformanceWindowFlag);

    if(*conformanceWindowFlag)
    {
        parse(streamAccessLayer, decoder, *confWinLeftOffset);
        parse(streamAccessLayer, decoder, *confWinRightOffset);
        parse(streamAccessLayer, decoder, *confWinTopOffset);
        parse(streamAccessLayer, decoder, *confWinBottomOffset);
    }

    auto bitDepthLumaMinus8 = embed<BitDepthLumaMinus8>(*this);
    auto bitDepthChromaMinus8 = embed<BitDepthChromaMinus8>(*this);

    parse(streamAccessLayer, decoder, *bitDepthLumaMinus8);
    parse(streamAccessLayer, decoder, *bitDepthChromaMinus8);

    auto maxPicOrderCntLsbMinus4 = embed<MaxPicOrderCntLsbMinus4>(*this);

    parse(streamAccessLayer, decoder, *maxPicOrderCntLsbMinus4);

    auto spsSubLayerOrderingInfoPresentFlag = embed<SpsSubLayerOrderingInfoPresentFlag>(*this);
    auto spsMaxDecPicBufferingMinus1 = embed<SpsMaxDecPicBufferingMinus1>(*this);
    auto spsMaxNumReorderPics = embed<SpsMaxNumReorderPics>(*this);
    auto spsMaxLatencyIncreasePlus1 = embed<SpsMaxLatencyIncreasePlus1>(*this);

    parse(streamAccessLayer, decoder, *spsSubLayerOrderingInfoPresentFlag);

    for(
            auto i = *spsSubLayerOrderingInfoPresentFlag ? 0 : *spsMaxSubLayersMinus1;
            i <= *spsMaxSubLayersMinus1;
            ++i)
    {
        parse(
                streamAccessLayer, decoder, *spsMaxDecPicBufferingMinus1,
                *spsSubLayerOrderingInfoPresentFlag, i);
        parse(
                streamAccessLayer, decoder, *spsMaxNumReorderPics,
                *spsSubLayerOrderingInfoPresentFlag, i);

        syntaxCheck((*spsMaxDecPicBufferingMinus1)[i] >= (*spsMaxNumReorderPics)[i]);

        parse(
                streamAccessLayer, decoder, *spsMaxLatencyIncreasePlus1,
                *spsSubLayerOrderingInfoPresentFlag, i);
    }

    auto minLumaCodingBlockSizeMinus3 = embed<MinLumaCodingBlockSizeMinus3>(*this);
    auto diffMaxMinLumaCodingBlockSize = embed<DiffMaxMinLumaCodingBlockSize>(*this);
    auto minTransformBlockSizeMinus2 = embed<MinTransformBlockSizeMinus2>(*this);
    auto diffMaxMinTransformBlockSize = embed<DiffMaxMinTransformBlockSize>(*this);
    auto maxTransformHierarchyDepthInter = embed<MaxTransformHierarchyDepthInter>(*this);
    auto maxTransformHierarchyDepthIntra = embed<MaxTransformHierarchyDepthIntra>(*this);
    auto scalingListEnabledFlag = embed<ScalingListEnabledFlag>(*this);

    parse(streamAccessLayer, decoder, *minLumaCodingBlockSizeMinus3);
    parse(streamAccessLayer, decoder, *diffMaxMinLumaCodingBlockSize);
    parse(streamAccessLayer, decoder, *minTransformBlockSizeMinus2);
    parse(streamAccessLayer, decoder, *diffMaxMinTransformBlockSize);
    parse(streamAccessLayer, decoder, *maxTransformHierarchyDepthInter);
    parse(streamAccessLayer, decoder, *maxTransformHierarchyDepthIntra);
    parse(streamAccessLayer, decoder, *scalingListEnabledFlag);

    if(*scalingListEnabledFlag)
    {
        auto spsScalingListDataPresentFlag = embed<SpsScalingListDataPresentFlag>(*this);

        parse(streamAccessLayer, decoder, *spsScalingListDataPresentFlag);

        if(*spsScalingListDataPresentFlag)
        {
            auto scalingListData = embedSubtree<ScalingListData>(*this);

            parse(streamAccessLayer, decoder, *scalingListData);
        }
    }

    auto ampEnabledFlag = embed<AmpEnabledFlag>(*this);
    auto sampleAdaptiveOffsetEnabledFlag = embed<SampleAdaptiveOffsetEnabledFlag>(*this);
    auto pcmEnabledFlag = embed<PcmEnabledFlag>(*this);
    auto pcmLoopFilterDisabledFlag = embed<PcmLoopFilterDisabledFlag>(*this);

    parse(streamAccessLayer, decoder, *ampEnabledFlag);
    parse(streamAccessLayer, decoder, *sampleAdaptiveOffsetEnabledFlag);
    parse(streamAccessLayer, decoder, *pcmEnabledFlag);

    if(*pcmEnabledFlag)
    {
        auto pcmSampleBitDepthLumaMinus1 = embed<PcmSampleBitDepthLumaMinus1>(*this);
        auto pcmSampleBitDepthChromaMinus1 = embed<PcmSampleBitDepthChromaMinus1>(*this);
        auto minPcmLumaCodingBlockSizeMinus3 = embed<MinPcmLumaCodingBlockSizeMinus3>(*this);
        auto diffMaxMinPcmLumaCodingBlockSize = embed<DiffMaxMinPcmLumaCodingBlockSize>(*this);

        parse(streamAccessLayer, decoder, *pcmSampleBitDepthLumaMinus1);
        parse(streamAccessLayer, decoder, *pcmSampleBitDepthChromaMinus1);
        parse(streamAccessLayer, decoder, *minPcmLumaCodingBlockSizeMinus3);
        parse(streamAccessLayer, decoder, *diffMaxMinPcmLumaCodingBlockSize);
        parse(streamAccessLayer, decoder, *pcmLoopFilterDisabledFlag);
    }

    auto numShortTermRefPicSets = embed<NumShortTermRefPicSets>(*this);

    parse(streamAccessLayer, decoder, *numShortTermRefPicSets);

    if(*numShortTermRefPicSets)
    {
        for(auto i = 0; i < *numShortTermRefPicSets; ++i)
        {
            auto shortTermRefPicSet = embedSubtreeInList<ShortTermRefPicSet>(*this);

            parse(
                    streamAccessLayer, decoder, *shortTermRefPicSet,
                    i,
                    *numShortTermRefPicSets,
                    *decoder.context()->get<Structure::RPSP>());
        }
    }

    auto longTermRefPicsPresentFlag = embed<LongTermRefPicsPresentFlag>(*this);

    parse(streamAccessLayer, decoder, *longTermRefPicsPresentFlag);

    if(*longTermRefPicsPresentFlag)
    {
        auto numLongTermRefPicsSps = embed<NumLongTermRefPicsSps>(*this);

        parse(streamAccessLayer, decoder, *numLongTermRefPicsSps);

        if(*numLongTermRefPicsSps)
        {
            auto ltRefPicPocLsbSps = embed<LtRefPicPocLsbSps>(*this, *maxPicOrderCntLsbMinus4);
            auto usedByCurrPicLtSpsFlag = embed<UsedByCurrPicLtSpsFlag>(*this);

            for(auto i = 0; i < *numLongTermRefPicsSps; ++i)
            {
                parse(streamAccessLayer, decoder, *ltRefPicPocLsbSps, i);
                parse(streamAccessLayer, decoder, *usedByCurrPicLtSpsFlag, i);
            }
        }
    }

    auto spsTemporalMvpEnableFlag = embed<SpsTemporalMvpEnableFlag>(*this);
    auto strongIntraSmoothingEnabledFlag = embed<StrongIntraSmoothingEnabledFlag>(*this);
    auto vuiParametersPresentFlag = embed<VuiParametersPresentFlag>(*this);

    parse(streamAccessLayer, decoder, *spsTemporalMvpEnableFlag);
    parse(streamAccessLayer, decoder, *strongIntraSmoothingEnabledFlag);
    parse(streamAccessLayer, decoder, *vuiParametersPresentFlag);

    if(*vuiParametersPresentFlag)
    {
        parse(
                streamAccessLayer, decoder, *embedSubtree<VuiParameters>(*this),
                int(*spsMaxSubLayersMinus1));
    }

    auto spsExtensionPresentFlag = embed<SpsExtensionPresentFlag>(*this);

    parse(streamAccessLayer, decoder, *spsExtensionPresentFlag);

    if(*spsExtensionPresentFlag)
    {
        parse(streamAccessLayer, decoder, *spsRangeExtensionFlag);
        parse(streamAccessLayer, decoder, *spsMultilayerExtensionFlag);
        parse(streamAccessLayer, decoder, *spsExtension6Bits);
    }

    if(*spsRangeExtensionFlag)
    {
        parse(streamAccessLayer, decoder, *embedSubtree<SpsRangeExtension>(*this));
    }

    if(*spsMultilayerExtensionFlag)
    {
        parse(streamAccessLayer, decoder, *embedSubtree<SpsMultilayerExtension>(*this));
    }

    if(spsExtension6Bits->inUnits())
    {
        auto spsExtensionDataFlag = embed<SpsExtensionDataFlag>(*this);

        while(moreRbspData(streamAccessLayer))
        {
            parse(streamAccessLayer, decoder, *spsExtensionDataFlag);
        }
    }

    parse(
            streamAccessLayer, decoder, *embedSubtree<RbspTrailingBits>(*this),
            ElementId{Id});
}
/*----------------------------------------------------------------------------*/
}} // HEVC::Syntax
