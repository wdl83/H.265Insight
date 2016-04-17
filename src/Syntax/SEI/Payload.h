#ifndef HEVC_Syntax_SEI_Payload_h
#define HEVC_Syntax_SEI_Payload_h

#include <Syntax/Syntax.h>
#include <Syntax/SEI/PayloadId.h>
#include <Syntax/SEI/BufferingPeriod.h>
#include <Syntax/SEI/DecodedPictureHash.h>

namespace HEVC { namespace Syntax { namespace SEI { namespace PayloadContent {
/*----------------------------------------------------------------------------*/
class ReservedPayloadExtensionData:
    public Embedded,
    public VLD::UInt
{
public:
    static const auto Id = ElementId::sei_reserved_payload_extension_data;
};

class PayloadBitEqualToOne:
    public Embedded,
    public VLD::FixedPattern<1>
{
public:
    static const auto Id = ElementId::sei_payload_bit_equal_to_one;
};

class PaylaodBitEqualToZero:
    public Embedded,
    public VLD::FixedPattern<0>
{
public:
    static const auto Id = ElementId::sei_payload_bit_equal_to_zero;
};

struct PayloadType:
    public Embedded
{
    const PayloadId payloadId;

    static const auto Id = ElementId::sei_PayloadId;

    PayloadType(PayloadId id): payloadId{id}
    {}

    operator PayloadId () const
    {
        return payloadId;
    }
};
/*----------------------------------------------------------------------------*/
} /* PayloadContent */

struct Payload:
    public EmbeddedAggregator<
        PayloadContent::PayloadType,
        PayloadContent::ReservedPayloadExtensionData,
        PayloadContent::PayloadBitEqualToOne,
        PayloadContent::PaylaodBitEqualToZero>,
    public SubtreeAggregator<
        DecodedPictureHash>
{
    typedef PayloadContent::PayloadType PayloadType;
    typedef PayloadContent::ReservedPayloadExtensionData ReservedPayloadExtensionData;
    typedef PayloadContent::PayloadBitEqualToOne PayloadBitEqualToOne;
    typedef PayloadContent::PaylaodBitEqualToZero PaylaodBitEqualToZero;

    static const auto Id = ElementId::sei_payload;

    void onParse(
            StreamAccessLayer &, Decoder::State &decoder,
            NalUnitType, int, int);
};
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Syntax::SEI */

#endif /* HEVC_Syntax_SEI_Payload_h */
