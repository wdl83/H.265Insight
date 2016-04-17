#include <Syntax/SliceSegmentLayerRbsp.h>
#include <Decoder/State.h>

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
void SliceSegmentLayerRbsp::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
        NalUnitType nalUnitType)
{
    auto sliceSegmentHeader = embedSubtree<SliceSegmentHeader>(*this);

    parse(streamAccessLayer, decoder, *sliceSegmentHeader, nalUnitType);

#ifndef SKIP_SLICE_SEGMENT_DATA
    auto sliceSegmentData = embedSubtree<SliceSegmentData>(*this);

    parse(streamAccessLayer, decoder, *sliceSegmentData, *sliceSegmentHeader);

    auto rbspSliceSegmentTrailingBits = embedSubtree<RbspSliceSegmentTrailingBits>(*this);

    parse(streamAccessLayer, decoder, *rbspSliceSegmentTrailingBits);
#endif // SKIP_SLICE_SEGMENT_DATA
}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */
