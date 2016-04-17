#ifndef HEVC_Syntax_SliceSegmentLayerRbsp_h
#define HEVC_Syntax_SliceSegmentLayerRbsp_h

#include <Syntax/Syntax.h>
#include <Syntax/SliceSegmentHeader.h>
#include <Syntax/SliceSegmentData.h>
#include <Syntax/RbspSliceSegmentTrailingBits.h>

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
class SliceSegmentLayerRbsp:
    public EmbeddedAggregatorTag,
    public SubtreeAggregator<
        SliceSegmentHeader,
        SliceSegmentData,
        RbspSliceSegmentTrailingBits>
{
public:
    static const auto Id = ElementId::slice_segment_layer_rbsp;

    void onParse(StreamAccessLayer &, Decoder::State &decoder, NalUnitType);
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */

#endif /* HEVC_Syntax_SliceSegmentLayerRbsp_h */
