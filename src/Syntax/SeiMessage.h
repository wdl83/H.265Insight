#ifndef HEVC_Syntax_SeiMessage_h
#define HEVC_Syntax_SeiMessage_h

#include <Syntax/Syntax.h>
#include <Syntax/SEI/PayloadId.h>
#include <Syntax/SEI/Payload.h>

namespace HEVC { namespace Syntax { namespace SeiMessageContent {
/*----------------------------------------------------------------------------*/
class FfByte:
    public Embedded,
    public VLD::FixedPattern<1, 1, 1, 1, 1, 1, 1, 1 /* 0xFF */>
{
public:
    static const auto Id = ElementId::sei_ff_byte;

    enum
    {
        Value = 0xFF
    };
};

class LastPaylaodTypeByte:
    public Embedded,
    public VLD::FixedUInt<8, uint8_t>
{
public:
    static const auto Id = ElementId::sei_last_payload_type_byte;

    int inUnits() const
    {
        return getValue();
    }
};

class LastPaylaodSizeByte:
    public Embedded,
    public VLD::FixedUInt<8, uint8_t>
{
public:
    static const auto Id = ElementId::sei_last_payload_size_byte;

    int inUnits() const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
} /* namespace SeiMessageContent */

/*----------------------------------------------------------------------------*/
class SeiMessage:
    public EmbeddedAggregator<
        SeiMessageContent::FfByte,
        SeiMessageContent::LastPaylaodTypeByte,
        SeiMessageContent::LastPaylaodSizeByte>,
    public SubtreeAggregator<
        SEI::Payload>
{
public:
    typedef SeiMessageContent::FfByte FfByte;
    typedef SeiMessageContent::LastPaylaodTypeByte LastPaylaodTypeByte;
    typedef SeiMessageContent::LastPaylaodSizeByte LastPaylaodSizeByte;

    static const auto Id = ElementId::sei_message;
    void onParse(StreamAccessLayer &, Decoder::State &decoder, NalUnitType);
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */

#endif /* HEVC_Syntax_SeiMessage_h */
