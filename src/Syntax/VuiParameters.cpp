#include <Syntax/VuiParameters.h>

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
void VuiParameters::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
        int spsMaxSubLayersMinus1)
{
    auto aspectRatioInfoPresentFlag = embed<AspectRatioInfoPresentFlag>(*this);

    parse(streamAccessLayer, decoder, *aspectRatioInfoPresentFlag);

    if(*aspectRatioInfoPresentFlag)
    {
        auto aspectRatioIdc = embed<AspectRatioIdc>(*this);

        parse(streamAccessLayer, decoder, *aspectRatioIdc);

        if(HEVC::AspectRatioIdc::EXTENDED_SAR == *aspectRatioIdc)
        {
            parse(streamAccessLayer, decoder, *embed<SarWidth>(*this));
            parse(streamAccessLayer, decoder, *embed<SarHeight>(*this));
        }
    }

    auto overscanInfoPresentFlag = embed<OverscanInfoPresentFlag>(*this);

    parse(streamAccessLayer, decoder, *overscanInfoPresentFlag);

    if(*overscanInfoPresentFlag)
    {
        parse(streamAccessLayer, decoder, *embed<OverscanAppropriateFlag>(*this));
    }

    auto videoSignalTypePresentFlag = embed<VideoSignalTypePresentFlag>(*this);

    parse(streamAccessLayer, decoder, *videoSignalTypePresentFlag);

    if(*videoSignalTypePresentFlag)
    {
        parse(streamAccessLayer, decoder, *embed<VideoFormat>(*this));
        parse(streamAccessLayer, decoder, *embed<VideoFullRangeFlag>(*this));

        auto colourDescriptionPresentFlag = embed<ColourDescriptionPresentFlag>(*this);

        parse(streamAccessLayer, decoder, *colourDescriptionPresentFlag);

        if(*colourDescriptionPresentFlag)
        {
            parse(streamAccessLayer, decoder, *embed<ColourPrimaries>(*this));
            parse(streamAccessLayer, decoder, *embed<TransferCharacteristics>(*this));
            parse(streamAccessLayer, decoder, *embed<MatrixCoeffs>(*this));
        }
    }

    auto chromaLocInfoPresentFlag = embed<ChromaLocInfoPresentFlag>(*this);

    parse(streamAccessLayer, decoder, *chromaLocInfoPresentFlag);

    if(*chromaLocInfoPresentFlag)
    {
        parse(streamAccessLayer, decoder, *embed<ChromaSampleLocTypeTopField>(*this));
        parse(streamAccessLayer, decoder, *embed<ChromaSampleLocTypeBottomField>(*this));
    }

    parse(streamAccessLayer, decoder, *embed<NeutralChromaIndicationFlag>(*this));
    parse(streamAccessLayer, decoder, *embed<FieldSeqFlag>(*this));
    parse(streamAccessLayer, decoder, *embed<FrameFieldInfoPresentFlag>(*this));

    auto defaultDisplayWindowFlag = embed<DefaultDisplayWindowFlag>(*this);

    parse(streamAccessLayer, decoder, *defaultDisplayWindowFlag);

    if(*defaultDisplayWindowFlag)
    {
        parse(streamAccessLayer, decoder, *embed<DefDispWinLeftOffset>(*this));
        parse(streamAccessLayer, decoder, *embed<DefDispWinRightOffset>(*this));
        parse(streamAccessLayer, decoder, *embed<DefDispWinTopOffset>(*this));
        parse(streamAccessLayer, decoder, *embed<DefDispWinBottomOffset>(*this));
    }

    auto vuiTimingInfoPresentFlag = embed<VuiTimingInfoPresentFlag>(*this);

    parse(streamAccessLayer, decoder, *vuiTimingInfoPresentFlag);

    if(*vuiTimingInfoPresentFlag)
    {
        parse(streamAccessLayer, decoder, *embed<VuiNumUnitsInTick>(*this));
        parse(streamAccessLayer, decoder, *embed<VuiTimeScale>(*this));

        auto vuiPocProportionalToTimingFlag = embed<VuiPocProportionalToTimingFlag>(*this);

        parse(streamAccessLayer, decoder, *vuiPocProportionalToTimingFlag);

        if(*vuiPocProportionalToTimingFlag)
        {
            parse(streamAccessLayer, decoder, *embed<VuiNumTicksPocDiffOneMinus1>(*this));
        }

        auto vuiHrdParametersPresentFlag = embed<VuiHrdParametersPresentFlag>(*this);

        parse(streamAccessLayer, decoder, *vuiHrdParametersPresentFlag);

        if(*vuiHrdParametersPresentFlag)
        {
            parse(
                    streamAccessLayer, decoder, *embedSubtree<HrdParameters>(*this),
                    true, spsMaxSubLayersMinus1);
        }
    }

    auto bitstreamRestrictionFlag = embed<BitstreamRestrictionFlag>(*this);

    parse(streamAccessLayer, decoder, *bitstreamRestrictionFlag);

    if(*bitstreamRestrictionFlag)
    {
        parse(streamAccessLayer, decoder, *embed<TilesFixedStructureFlag>(*this));
        parse(streamAccessLayer, decoder, *embed<MotionVectorsOverPicBoundariesFlag>(*this));
        parse(streamAccessLayer, decoder, *embed<RestrictedRefPicListsFlag>(*this));
        parse(streamAccessLayer, decoder, *embed<MinSpatialSegmentationIdc>(*this));
        parse(streamAccessLayer, decoder, *embed<MaxBytesPerPicDenom>(*this));
        parse(streamAccessLayer, decoder, *embed<MaxBitsPerMinCuDenom>(*this));
        parse(streamAccessLayer, decoder, *embed<MaxMvLengthHorizontal>(*this));
        parse(streamAccessLayer, decoder, *embed<MaxMvLengthVertical>(*this));
    }
}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */
