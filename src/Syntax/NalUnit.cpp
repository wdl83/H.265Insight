/* HEVC */
#include <Syntax/NalUnit.h>
#include <Decoder/State.h>
/* STDC++ */
#include <algorithm>

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
/* Nal Unit */
/*----------------------------------------------------------------------------*/
void NalUnit::onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder)
{
    auto nalUnitHeader = embedSubtree<NalUnitHeader>(*this);

    parse(streamAccessLayer, decoder, *nalUnitHeader);

    auto rbspByte = embed<RbspByte>(*this, m_numBytesInNalUnit);
    auto emulationPreventionThreeByte = embed<EmulationPreventionThreeByte>(*this);

    /* emulation prevention (reverse order) */
    static const uint8_t emulPrevent[] = {0x03, 0x00, 0x00};
    size_t rbspBytesNum = 0;

    const auto match =
        [&streamAccessLayer]()
        {
           const auto peeked24 = streamAccessLayer.peekBytes<sizeof(emulPrevent)>();

            return isMatchingAt(peeked24.rbegin(), peeked24.rend(), emulPrevent);
        };

    for(size_t i = 2; i < m_numBytesInNalUnit; ++i)
    {
        /* prevent boundary crossing */
        const bool emulPreventPresent =
            m_numBytesInNalUnit - i >= sizeof(emulPrevent) && match();

        if(i + 2 < m_numBytesInNalUnit && emulPreventPresent)
        {
            parse(streamAccessLayer, decoder, *rbspByte);
            ++rbspBytesNum;
            parse(streamAccessLayer, decoder, *rbspByte);
            ++rbspBytesNum;
            i += 2;
            parse(streamAccessLayer, decoder, *emulationPreventionThreeByte);
        }
        else
        {
            parse(streamAccessLayer, decoder, *rbspByte);
            ++rbspBytesNum;
        }
    }

    embed<NumBytesInRbsp>(*this, rbspBytesNum);
}
/*----------------------------------------------------------------------------*/
void NalUnit::parseRbsp(Decoder::State &decoder)
{
    if(empty())
    {
        return;
    }

    StreamAccessLayer streamAccessLayer(*get<RbspByte>());

    const NalUnitType nalUnitType =
        *getSubtree<NalUnitHeader>()->get<NalUnitHeader::NalUnitType>();

    const auto toStr =
        [&decoder, nalUnitType](std::ostream &oss)
        {
            oss
                << std::dec << decoder.cntr.snu << ' '
                << getName(nalUnitType) << '\n';
        };

    log(LogId::NalUnits, toStr);

    if(isVCL(nalUnitType) && !isRSV(nalUnitType))
    {
        parse(
                streamAccessLayer, decoder, *embedSubtree<SliceSegmentLayerRbsp>(*this),
                nalUnitType);
    }

    if(NalUnitType::VPS_NUT == nalUnitType)
    {
        parse(streamAccessLayer, decoder, *embedSubtree<VideoParameterSet>(*this));
    }

    if(NalUnitType::SPS_NUT == nalUnitType)
    {
        parse(streamAccessLayer, decoder, *embedSubtree<SequenceParameterSet>(*this));
    }

    if(NalUnitType::PPS_NUT == nalUnitType)
    {
        parse(streamAccessLayer, decoder, *embedSubtree<PictureParameterSet>(*this));
    }

    if(NalUnitType::AUD_NUT == nalUnitType)
    {
        parse(streamAccessLayer, decoder, *embedSubtree<AccessUnitDelimiterRbsp>(*this));
    }

    if(NalUnitType::EOS_NUT == nalUnitType)
    {
        parse(streamAccessLayer, decoder, *embedSubtree<EndOfSeqRbsp>(*this));
    }

    if(NalUnitType::EOB_NUT == nalUnitType)
    {
        parse(streamAccessLayer, decoder, *embedSubtree<EndOfBitstreamRbsp>(*this));
    }

    if(NalUnitType::FD_NUT == nalUnitType)
    {
        parse(streamAccessLayer, decoder, *embedSubtree<FillerDataRbsp>(*this));
    }

    if(
            NalUnitType::PREFIX_SEI_NUT == nalUnitType
            || NalUnitType::SUFFIX_SEI_NUT == nalUnitType)
    {
        parse(
                streamAccessLayer, decoder, *embedSubtree<SeiRbsp>(*this),
                nalUnitType);
    }

    if(isRSV(nalUnitType) || isUNSPEC(nalUnitType))
    {
        log(LogId::Debug, "Syntax::Rbsp::onParse(): RSV/unspec not supported\n");
    }
}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */
