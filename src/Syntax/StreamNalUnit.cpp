#include <Syntax/StreamNalUnit.h>

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
/* StreamNalUnit */
/*----------------------------------------------------------------------------*/
void StreamNalUnit::onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder)
{
    syntaxCheck(streamAccessLayer.isByteAligned());
    parseNalUnitPrefix(streamAccessLayer, decoder);
    parseNalUnitSuffix(streamAccessLayer, decoder);

    const auto nalUnitSizeInBytes = streamAccessLayer.getSizeInBytes();
    auto numBytesInNalUnit = embed<NumBytesInNalUnit>(*this, nalUnitSizeInBytes);

    auto nalUnit = embedSubtree<NalUnit>(*this, *numBytesInNalUnit);

    parse(streamAccessLayer, decoder, *nalUnit);
}
/*----------------------------------------------------------------------------*/
void StreamNalUnit::parseNalUnitPrefix(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder)
{
    auto leadingZero8Bits = embed<LeadingZero8Bits>(*this);

    /* start code prefix 24/32 bits (reverse order) */
    const uint8_t scp24[] = {1, 0, 0};
    const uint8_t scp32[] = {1, 0, 0, 0};

    bool scp24Matched = false, scp32Matched = false;

    while(true)
    {
        const auto peeked24 = streamAccessLayer.peekBytes<sizeof(scp24)>();

        if(!(scp24Matched = isMatchingAt(peeked24.rbegin(), peeked24.rend(), scp24)))
        {
            const auto peeked32 = streamAccessLayer.peekBytes<sizeof(scp32)>();

            if(!(scp32Matched = isMatchingAt(peeked32.rbegin(), peeked32.rend(), scp32)))
            {
                /* consume one byte from stream and search again */
                parse(streamAccessLayer, decoder, *leadingZero8Bits);
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
        }
    }

    syntaxCheck(scp24Matched != scp32Matched);

    if(scp32Matched)
    {
        auto zeroByte = embed<ZeroByte>(*this);

        parse(streamAccessLayer, decoder, *zeroByte);
    }

    auto startCodePrefixOne3Bytes = embed<StartCodePrefixOne3Bytes>(*this);

    parse(streamAccessLayer, decoder, *startCodePrefixOne3Bytes);
}
/*----------------------------------------------------------------------------*/
void StreamNalUnit::parseNalUnitSuffix(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder)
{
    auto trailingZero8Bits = embed<TrailingZero8Bits>(*this);

    while(true)
    {
        if(uint8_t(0) == streamAccessLayer.peekTailByte())
        {
            parse(streamAccessLayer, decoder, *trailingZero8Bits);
        }
        else
        {
            break;
        }
    }
}
/*----------------------------------------------------------------------------*/
}}
