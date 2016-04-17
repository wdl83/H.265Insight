/* HEVC */
#include <Syntax/PictureParameterSet.h>
#include <Syntax/SequenceParameterSet.h>
#include <Syntax/Rbsp.h>

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
void PictureParameterSet::toStr(std::ostream &os) const
{
    os
        << getName(DependentSliceSegmentsEnabledFlag::Id) << ' '
        << bool(*get<DependentSliceSegmentsEnabledFlag>()) << ' '
        << getName(TilesEnabledFlag::Id) << ' '
        << bool(*get<TilesEnabledFlag>()) << ' '
        << getName(NumTileRowsMinus1::Id) << ' '
        << get<NumTileRowsMinus1>()->inUnits() << ' '
        << getName(NumTileColumnsMinus1::Id) << ' '
        << get<NumTileColumnsMinus1>()->inUnits() << ' '
        << getName(LoopFilterAcrossTilesEnabledFlag::Id) << ' '
        << bool(*get<LoopFilterAcrossTilesEnabledFlag>()) << ' '
        << getName(PpsLoopFilterAcrossSlicesEnabledFlag::Id) << ' '
        << bool(*get<PpsLoopFilterAcrossSlicesEnabledFlag>()) << ' '
        << getName(PpsDeblockingFilterDisabledFlag::Id) << ' '
        << bool(*get<PpsDeblockingFilterDisabledFlag>());
}
/*----------------------------------------------------------------------------*/
void PictureParameterSet::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder)
{
    /* start inferrable */
    auto numTileColumnsMinus1 = embed<NumTileColumnsMinus1>(*this);
    auto numTileRowsMinus1 = embed<NumTileRowsMinus1>(*this);
    auto uniformSpacingFlag = embed<UniformSpacingFlag>(*this);
    auto loopFilterAcrossTilesEnabledFlag = embed<LoopFilterAcrossTilesEnabledFlag>(*this);
    auto deblockingFilterOverrideEnabledFlag = embed<DeblockingFilterOverrideEnabledFlag>(*this);
    auto ppsDeblockingFilterDisabledFlag = embed<PpsDeblockingFilterDisabledFlag>(*this);
    auto ppsBetaOffsetDiv2 = embed<PpsBetaOffsetDiv2>(*this);
    auto ppsTcOffsetDiv2 = embed<PpsTcOffsetDiv2>(*this);
    auto diffCuQpDeltaDepth = embed<DiffCuQpDeltaDepth>(*this);
    auto ppsRangeExtensionFlag = embed<PpsRangeExtensionFlag>(*this);
    auto ppsMultilayerExtensionFlag = embed<PpsMultilayerExtensionFlag>(*this);
    auto ppsExtension6Bits = embed<PpsExtension6Bits>(*this);
    /* end: inferrable */

    auto ppsPicParameterSetId = embed<PpsPicParameterSetId>(*this);

    parse(streamAccessLayer, decoder, *ppsPicParameterSetId);

    auto ppsSeqParameterSetId = embed<PpsSeqParameterSetId>(*this);

    parse(streamAccessLayer, decoder, *ppsSeqParameterSetId);

    auto dependentSliceSegmentsEnabledFlag = embed<DependentSliceSegmentsEnabledFlag>(*this);
    auto outputFlagPresentFlag = embed<OutputFlagPresentFlag>(*this);
    auto numExtraSliceHeaderBits = embed<NumExtraSliceHeaderBits>(*this);
    auto signDataHidingEnabledFlag = embed<SignDataHidingEnabledFlag>(*this);
    auto cabacInitPresentFlag = embed<CabacInitPresentFlag>(*this);
    auto numRefIdxL0DefaultActiveMinus1 = embed<NumRefIdxL0DefaultActiveMinus1>(*this);
    auto numRefIdxL1DefaultActiveMinus1 = embed<NumRefIdxL1DefaultActiveMinus1>(*this);
    auto initQpMinus26 = embed<InitQpMinus26>(*this);
    auto constrainedIntraPredFlag = embed<ConstrainedIntraPredFlag>(*this);
    auto transformSkipEnabledFlag = embed<TransformSkipEnabledFlag>(*this);
    auto cuQpDeltaEnabledFlag = embed<CuQpDeltaEnabledFlag>(*this);

    parse(streamAccessLayer, decoder, *dependentSliceSegmentsEnabledFlag);
    parse(streamAccessLayer, decoder, *outputFlagPresentFlag);
    parse(streamAccessLayer, decoder, *numExtraSliceHeaderBits);
    parse(streamAccessLayer, decoder, *signDataHidingEnabledFlag);
    parse(streamAccessLayer, decoder, *cabacInitPresentFlag);
    parse(streamAccessLayer, decoder, *numRefIdxL0DefaultActiveMinus1);
    parse(streamAccessLayer, decoder, *numRefIdxL1DefaultActiveMinus1);
    parse(streamAccessLayer, decoder, *initQpMinus26);
    parse(streamAccessLayer, decoder, *constrainedIntraPredFlag);
    parse(streamAccessLayer, decoder, *transformSkipEnabledFlag);
    parse(streamAccessLayer, decoder, *cuQpDeltaEnabledFlag);

    if(*cuQpDeltaEnabledFlag)
    {
        parse(streamAccessLayer, decoder, *diffCuQpDeltaDepth);
    }

    auto ppsCbQpOffset = embed<PpsCbQpOffset>(*this);
    auto ppsCrQpOffset = embed<PpsCrQpOffset>(*this);
    auto ppsSliceChromaQpOffsetsPresentFlag = embed<PpsSliceChromaQpOffsetsPresentFlag>(*this);
    auto weightedPredFlag = embed<WeightedPredFlag>(*this);
    auto weightedBipredFlag = embed<WeightedBipredFlag>(*this);
    auto transquantBypassEnableFlag = embed<TransquantBypassEnableFlag>(*this);
    auto tilesEnabledFlag = embed<TilesEnabledFlag>(*this);
    auto entropyCodingSyncEnabledFlag = embed<EntropyCodingSyncEnabledFlag>(*this);

    parse(streamAccessLayer, decoder, *ppsCbQpOffset);
    parse(streamAccessLayer, decoder, *ppsCrQpOffset);
    parse(streamAccessLayer, decoder, *ppsSliceChromaQpOffsetsPresentFlag);
    parse(streamAccessLayer, decoder, *weightedPredFlag);
    parse(streamAccessLayer, decoder, *weightedBipredFlag);
    parse(streamAccessLayer, decoder, *transquantBypassEnableFlag);
    parse(streamAccessLayer, decoder, *tilesEnabledFlag);
    parse(streamAccessLayer, decoder, *entropyCodingSyncEnabledFlag);

    if(*tilesEnabledFlag)
    {
        parse(streamAccessLayer, decoder, *numTileColumnsMinus1);
        parse(streamAccessLayer, decoder, *numTileRowsMinus1);
        parse(streamAccessLayer, decoder, *uniformSpacingFlag);

        if(!(*uniformSpacingFlag))
        {
            if(0 < numTileColumnsMinus1->inUnits())
            {
                auto columnWidthMinus1 = embed<ColumnWidthMinus1>(*this);

                for(auto i = 0; i < numTileColumnsMinus1->inUnits(); ++i)
                {
                    parse(streamAccessLayer, decoder, *columnWidthMinus1, i);
                }
            }

            if(0 < numTileRowsMinus1->inUnits())
            {
                auto rowHeightMinus1 = embed<RowHeightMinus1>(*this);

                for(auto i = 0; i < numTileRowsMinus1->inUnits(); ++i)
                {
                    parse(streamAccessLayer, decoder, *rowHeightMinus1, i);
                }
            }
        }

        parse(streamAccessLayer, decoder, *loopFilterAcrossTilesEnabledFlag);
    }

    auto ppsLoopFilterAcrossSlicesEnabledFlag = embed<PpsLoopFilterAcrossSlicesEnabledFlag>(*this);
    auto deblockingFilterControlPresentFlag = embed<DeblockingFilterControlPresentFlag>(*this);

    parse(streamAccessLayer, decoder, *ppsLoopFilterAcrossSlicesEnabledFlag);
    parse(streamAccessLayer, decoder, *deblockingFilterControlPresentFlag);

    if(*deblockingFilterControlPresentFlag)
    {
        parse(streamAccessLayer, decoder, *deblockingFilterOverrideEnabledFlag);
        parse(streamAccessLayer, decoder, *ppsDeblockingFilterDisabledFlag);

        if(!(*ppsDeblockingFilterDisabledFlag))
        {
            parse(streamAccessLayer, decoder, *ppsBetaOffsetDiv2);
            parse(streamAccessLayer, decoder, *ppsTcOffsetDiv2);
        }
    }

    auto ppsScalingListDataPresentFlag = embed<PpsScalingListDataPresentFlag>(*this);

    parse(streamAccessLayer, decoder, *ppsScalingListDataPresentFlag);

    if(*ppsScalingListDataPresentFlag)
    {
        auto scalingListData = embedSubtree<ScalingListData>(*this);

        parse(streamAccessLayer, decoder, *scalingListData);
    }

    auto listsModificationPresentFlag = embed<ListsModificationPresentFlag>(*this);
    auto log2ParallelMergeLevelMinus2 = embed<Log2ParallelMergeLevelMinus2>(*this);
    auto sliceSegmentHeaderExtensionPresentFlag = embed<SliceSegmentHeaderExtensionPresentFlag>(*this);

    parse(streamAccessLayer, decoder, *listsModificationPresentFlag);
    parse(streamAccessLayer, decoder, *log2ParallelMergeLevelMinus2);
    parse(streamAccessLayer, decoder, *sliceSegmentHeaderExtensionPresentFlag);

    auto ppsExtensionPresentFlag = embed<PpsExtensionPresentFlag>(*this);

    parse(streamAccessLayer, decoder, *ppsExtensionPresentFlag);

    if(*ppsExtensionPresentFlag)
    {
        parse(streamAccessLayer, decoder, *ppsRangeExtensionFlag);
        parse(streamAccessLayer, decoder, *ppsMultilayerExtensionFlag);
        parse(streamAccessLayer, decoder, *ppsExtension6Bits);
    }

    if(*ppsRangeExtensionFlag)
    {
        parse(streamAccessLayer, decoder, *embedSubtree<PpsRangeExtension>(*this), *this);
    }

    if(*ppsMultilayerExtensionFlag)
    {
        parse(streamAccessLayer, decoder, *embedSubtree<PpsMultilayerExtension>(*this));
    }

    if(ppsExtension6Bits->inUnits())
    {
        auto ppsExtensionDataFlag = embed<PpsExtensionDataFlag>(*this);

        while(moreRbspData(streamAccessLayer))
        {
            parse(streamAccessLayer, decoder, *ppsExtensionDataFlag);
        }
    }

    parse(
            streamAccessLayer, decoder, *embedSubtree<RbspTrailingBits>(*this),
            ElementId{Id});
}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */
