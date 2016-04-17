#include <Syntax/SeiMessage.h>

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
void SeiMessage::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
        NalUnitType nalUnitType)
{
    auto payloadType = 0;
    auto payloadSize = 0;
    auto ffByte = embed<FfByte>(*this);
    auto lastPaylaodTypeByte = embed<LastPaylaodTypeByte>(*this);
    auto lastPaylaodSizeByte = embed<LastPaylaodSizeByte>(*this);

    while(FfByte::Value == streamAccessLayer.peekByte())
    {
        parse(streamAccessLayer, decoder, *ffByte);
        payloadType += 255;
    }

    parse(streamAccessLayer, decoder, *lastPaylaodTypeByte);
    payloadType += lastPaylaodTypeByte->inUnits();

    while(FfByte::Value == streamAccessLayer.peekByte())
    {
        parse(streamAccessLayer, decoder, *ffByte);
        payloadSize += 255;
    }

    parse(streamAccessLayer, decoder, *lastPaylaodSizeByte);
    payloadSize += lastPaylaodSizeByte->inUnits();

    auto seiPayload = embedSubtree<SEI::Payload>(*this);

    parse(
            streamAccessLayer, decoder, *seiPayload,
            nalUnitType, payloadType, payloadSize);
}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */
