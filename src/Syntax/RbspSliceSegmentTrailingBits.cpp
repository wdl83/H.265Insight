#include <Syntax/RbspSliceSegmentTrailingBits.h>
#include <Syntax/Rbsp.h>

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
void RbspSliceSegmentTrailingBits::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder)
{
    auto rbspTrailingBits = embedSubtree<RbspTrailingBits>(*this);

    parse(streamAccessLayer, decoder, *rbspTrailingBits, ElementId{Id});

    if(moreRbspTrailingData(streamAccessLayer))
    {
        auto cabacZeroWord = embed<CabacZeroWord>(*this);

        while(moreRbspTrailingData(streamAccessLayer))
        {
            parse(streamAccessLayer, decoder, *cabacZeroWord);
        }
    }
}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */
