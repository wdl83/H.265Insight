#include <Syntax/VideoParameterSet.h>
#include <Syntax/Rbsp.h>

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
void VideoParameterSet::onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder)
{
    auto vpsVideoParameterSetId = embed<VpsVideoParameterSetId>(*this);
    auto vpsBaseLayerInternalFlag = embed<VpsBaseLayerInternalFlag>(*this);
    auto vpsBaseLayerAvailableFlag = embed<VpsBaseLayerAvailableFlag>(*this);
    auto vpsMaxLayersMinus1 = embed<VpsMaxLayersMinus1>(*this);
    auto vpsMaxSubLayersMinus1 = embed<VpsMaxSubLayersMinus1>(*this);
    auto vpsTemporalIdNestingFlag = embed<VpsTemporalIdNestingFlag>(*this);
    auto vpsReserved0xffff16bits = embed<VpsReserved0xffff16bits>(*this);

    parse(streamAccessLayer, decoder, *vpsVideoParameterSetId);
    parse(streamAccessLayer, decoder, *vpsBaseLayerInternalFlag);
    parse(streamAccessLayer, decoder, *vpsBaseLayerAvailableFlag);
    parse(streamAccessLayer, decoder, *vpsMaxLayersMinus1);
    parse(streamAccessLayer, decoder, *vpsMaxSubLayersMinus1);
    parse(streamAccessLayer, decoder, *vpsTemporalIdNestingFlag);
    parse(streamAccessLayer, decoder, *vpsReserved0xffff16bits);

    parse(
            streamAccessLayer, decoder, *embedSubtree<ProfileTierLevel>(*this),
            true, *vpsMaxSubLayersMinus1);

    auto vpsSubLayerOrderingInfoPresentFlag =
        embed<VpsSubLayerOrderingInfoPresentFlag>(*this);

    parse(streamAccessLayer, decoder, *vpsSubLayerOrderingInfoPresentFlag);

    auto vpsMaxDecPicBufferingMinus1 = embed<VpsMaxDecPicBufferingMinus1>(*this);
    auto vpsMaxNumReorderPics = embed<VpsMaxNumReorderPics>(*this);
    auto vpsMaxLatencyIncreasePlus1 = embed<VpsMaxLatencyIncreasePlus1>(*this);

    for(
            int i = *vpsSubLayerOrderingInfoPresentFlag ? 0 : *vpsMaxSubLayersMinus1;
            i <= *vpsMaxSubLayersMinus1;
            ++i)
    {
        parse(
                streamAccessLayer, decoder, *vpsMaxDecPicBufferingMinus1,
                *vpsSubLayerOrderingInfoPresentFlag, i);

        parse(
                streamAccessLayer, decoder, *vpsMaxNumReorderPics,
                *vpsSubLayerOrderingInfoPresentFlag, i);

        syntaxCheck((*vpsMaxDecPicBufferingMinus1)[i] >= (*vpsMaxNumReorderPics)[i]);

        parse(
                streamAccessLayer, decoder, *vpsMaxLatencyIncreasePlus1,
                *vpsSubLayerOrderingInfoPresentFlag, i);
    }

    auto vpsMaxLayerId = embed<VpsMaxLayerId>(*this);
    auto vpsNumLayerSetsMinus1 = embed<VpsNumLayerSetsMinus1>(*this);

    parse(streamAccessLayer, decoder, *vpsMaxLayerId);
    parse(streamAccessLayer, decoder, *vpsNumLayerSetsMinus1);

    auto layerIdIncludedFlag = embed<LayerIdIncludedFlag>(*this);

    for(auto i = 1; i <= *vpsNumLayerSetsMinus1; ++i)
    {
        for(auto j = 0; j <= *vpsMaxLayerId; ++j)
        {
            /* see: LayerIdIncludedFlag (constrained by allowed nuh_layer_id range) */
            syntaxCheck(0 == *vpsMaxLayerId);
            parse(streamAccessLayer, decoder, *layerIdIncludedFlag, i, j);
        }
    }

    auto vpsTimingInfoPresentFlag = embed<VpsTimingInfoPresentFlag>(*this);

    parse(streamAccessLayer, decoder, *vpsTimingInfoPresentFlag);

    if(*vpsTimingInfoPresentFlag)
    {
        auto vpsPocProportionalToTimingFlag = embed<VpsPocProportionalToTimingFlag>(*this);
        auto vpsNumHrdParameters = embed<VpsNumHrdParameters>(*this);

        parse(streamAccessLayer, decoder, *embed<VpsNumUnitsInTick>(*this));
        parse(streamAccessLayer, decoder, *embed<VpsTimeScale>(*this));
        parse(streamAccessLayer, decoder, *vpsPocProportionalToTimingFlag);

        if(*vpsPocProportionalToTimingFlag)
        {
            parse(streamAccessLayer, decoder, *embed<VpsNumTicksPocDiffOneMinus1>(*this));
        }

        parse(streamAccessLayer, decoder, *vpsNumHrdParameters);

        if(*vpsNumHrdParameters)
        {
            auto hrdLayerSetIdx = embed<HrdLayerSetIdx>(*this);
            auto cprmsPresentFlag = embed<CprmsPresentFlag>(*this);

            for(auto i = 0; i < *vpsNumHrdParameters; ++i)
            {
                parse(streamAccessLayer, decoder, *hrdLayerSetIdx, i);

                if(0 < i)
                {
                    parse(streamAccessLayer, decoder, *cprmsPresentFlag, i);
                }

                auto hrdParameters = embedSubtreeInList<HrdParameters>(*this);

                parse(
                        streamAccessLayer, decoder, *hrdParameters,
                        (*cprmsPresentFlag)[i], *vpsMaxSubLayersMinus1);
            }
        }
    }

    auto vpsExtensionFlag = embed<VpsExtensionFlag>(*this);

    parse(streamAccessLayer, decoder, *vpsExtensionFlag);

    if(*vpsExtensionFlag)
    {
        auto vpsExtensionDataFlag = embed<VpsExtensionDataFlag>(*this);

        while(moreRbspData(streamAccessLayer))
        {
            parse(streamAccessLayer, decoder, *vpsExtensionDataFlag);
        }
    }

    auto rbspTrailingBits = embedSubtree<RbspTrailingBits>(*this);

    parse(streamAccessLayer, decoder, *rbspTrailingBits, ElementId{Id});
}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */
