#include <Syntax/RbspTrailingBits.h>
#include <Syntax/AccessUnitDelimiterRbsp.h>

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
void AccessUnitDelimiterRbsp::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder)
{
    auto picType = embed<PicType>(*this);

    parse(streamAccessLayer, decoder, *picType);

    auto rbspTrailingBits = embedSubtree<RbspTrailingBits>(*this);

    parse(streamAccessLayer, decoder, *rbspTrailingBits, ElementId{Id});
}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */
