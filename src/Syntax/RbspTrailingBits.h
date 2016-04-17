#ifndef SyntaxRbspTrailingBits_h
#define SyntaxRbspTrailingBits_h

#include <Syntax/Syntax.h>

namespace HEVC { namespace Syntax { namespace RbspTrailingBitsContent {
/*----------------------------------------------------------------------------*/
class RbspStopOneBit:
    public Embedded,
    public VLD::FixedPattern<1>
{
public:
    static const auto Id = ElementId::rbsp_stop_one_bit;
};
/*----------------------------------------------------------------------------*/
class RbspAlignmentZeroBit:
    public Embedded,
    public VLD::FixedPattern<0>
{
public:
    static const auto Id = ElementId::rbsp_alignment_zero_bit;
};
/*----------------------------------------------------------------------------*/
} /* namespace RbspTrailingBitsContent */
/*----------------------------------------------------------------------------*/
class RbspTrailingBits:
    public EmbeddedAggregator<
    RbspTrailingBitsContent::RbspStopOneBit,
    RbspTrailingBitsContent::RbspAlignmentZeroBit>
{
public:
    static const auto Id = ElementId::rbsp_trailing_bits;

    typedef RbspTrailingBitsContent::RbspStopOneBit RbspStopOneBit;
    typedef RbspTrailingBitsContent::RbspAlignmentZeroBit RbspAlignmentZeroBit;

    void onParse(StreamAccessLayer &, Decoder::State &decoder, ElementId);
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */

#endif /* SyntaxRbspTrailingBits_h */
