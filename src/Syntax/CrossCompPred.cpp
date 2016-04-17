#include <Syntax/CrossCompPred.h>

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
void CrossCompPred::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
        Chroma chroma)
{
    auto resScaleAbsPlus1 = embed<ResScaleAbsPlus1>(*this);

    parse(streamAccessLayer, decoder, *resScaleAbsPlus1, chroma);

    if(0_log2 != resScaleAbsPlus1->inUnits())
    {
        parse(streamAccessLayer, decoder, *embed<ResScaleSignFlag>(*this), chroma);
    }
}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */
