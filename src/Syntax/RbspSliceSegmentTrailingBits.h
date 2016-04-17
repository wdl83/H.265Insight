#ifndef SyntaxRbspSliceTrailingBits_h
#define SyntaxRbspSliceTrailingBits_h

#include <Syntax/Syntax.h>
#include <Syntax/RbspTrailingBits.h>

namespace HEVC { namespace Syntax { namespace RbspSliceSegmentTrailingBitsContent {
/*----------------------------------------------------------------------------*/
class CabacZeroWord:
    public Embedded,
    public VLD::FixedPattern<0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 /* 0x0000 */>
{
public:
    static const auto Id = ElementId::cabac_zero_word;
};
/*----------------------------------------------------------------------------*/
} /* namespace RbspSliceSegmentTrailingBitsContent */
/*----------------------------------------------------------------------------*/
class RbspSliceSegmentTrailingBits:
    public EmbeddedAggregator<
        RbspSliceSegmentTrailingBitsContent::CabacZeroWord>,
    public SubtreeAggregator<
        RbspTrailingBits>
{
public:
    static const auto Id = ElementId::rbsp_slice_segment_trailing_bits;

    typedef RbspSliceSegmentTrailingBitsContent::CabacZeroWord CabacZeroWord;

    void onParse(StreamAccessLayer &, Decoder::State &);
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */

#endif /* SyntaxRbspSliceTrailingBits_h */
