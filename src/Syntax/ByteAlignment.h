#ifndef HEVC_Syntax_ByteAlignment_h
#define HEVC_Syntax_ByteAlignment_h

#include <Syntax/Syntax.h>
#include <Syntax/Embedded.h>
#include <Syntax/Aggregator.h>
#include <Syntax/VLD/Descriptors.h>

namespace HEVC { namespace Syntax { namespace ByteAlignmentContent {
/*----------------------------------------------------------------------------*/
class AlignmentBitEqualToOne:
    public Embedded,
    public VLD::FixedPattern<1>
{
public:
    static const auto Id = ElementId::alignment_bit_equal_to_one;
};
/*----------------------------------------------------------------------------*/
class AlignmentBitEqualToZero:
    public Embedded,
    public VLD::FixedPattern<0>
{
public:
    static const auto Id = ElementId::alignment_bit_equal_to_zero;
};
/*----------------------------------------------------------------------------*/
} /* namespace ByteAlignmentContent */
/*----------------------------------------------------------------------------*/
class ByteAlignment:
    public EmbeddedAggregator<
        ByteAlignmentContent::AlignmentBitEqualToOne,
        ByteAlignmentContent::AlignmentBitEqualToZero>
{
public:
    static const auto Id = ElementId::byte_alignment;

    typedef ByteAlignmentContent::AlignmentBitEqualToOne AlignmentBitEqualToOne;
    typedef ByteAlignmentContent::AlignmentBitEqualToZero AlignmentBitEqualToZero;

    void onParse(StreamAccessLayer &, Decoder::State &decoder, ElementId);
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */

#endif /* HEVC_Syntax_ByteAlignment_h */
