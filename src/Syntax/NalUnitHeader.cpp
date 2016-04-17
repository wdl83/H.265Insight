#include <Syntax/NalUnitHeader.h>

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
void NalUnitHeader::onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder)
{
    auto forbiddenZeroBit = embed<ForbiddenZeroBit>(*this);
    auto nalUnitType = embed<NalUnitType>(*this);
    auto nuhLayerId = embed<NuhLayerId>(*this);
    auto nuhTemporalIdPlus1 = embed<NuhTemporalIdPlus1>(*this);

    parse(streamAccessLayer, decoder, *forbiddenZeroBit);
    parse(streamAccessLayer, decoder, *nalUnitType);
    parse(streamAccessLayer, decoder, *nuhLayerId);
    parse(streamAccessLayer, decoder, *nuhTemporalIdPlus1);

    /* pseudo syntax elements */
    embed<TemporalId>(*this, *nuhTemporalIdPlus1);
}
/*----------------------------------------------------------------------------*/
}}
