#include <Syntax/SeiRbsp.h>
#include <Syntax/Rbsp.h>

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
void SeiRbsp::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
        NalUnitType nalUnitType)
{
    do
    {
        auto seiMessage =
            embedSubtreeInList<SeiMessage>(*this);

        parse(streamAccessLayer, decoder, *seiMessage, nalUnitType);
    }
    while(moreRbspData(streamAccessLayer));

    auto rbspTrailingBits = embedSubtree<RbspTrailingBits>(*this);

    parse(streamAccessLayer, decoder, *rbspTrailingBits, ElementId{Id});
}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */
