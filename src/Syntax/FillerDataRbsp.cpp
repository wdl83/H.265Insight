#include <Syntax/FillerDataRbsp.h>

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
void FillerDataRbsp::onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder)
{
    if(FfByte::Value == streamAccessLayer.peekByte())
    {
        auto ffByte = embed<FfByte>(*this);

        while(FfByte::Value == streamAccessLayer.peekByte())
        {
            parse(streamAccessLayer, decoder, *ffByte);
        }
    }

    auto rbspTrailingBits = embedSubtree<RbspTrailingBits>(*this);

    parse(streamAccessLayer, decoder, *rbspTrailingBits, ElementId{Id});
}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */

