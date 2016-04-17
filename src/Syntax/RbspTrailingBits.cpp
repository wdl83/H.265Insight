#include <Syntax/RbspTrailingBits.h>

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
void RbspTrailingBits::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
        ElementId parentId)
{
    /* SPECIAL CASE:
     * 03/2013
     * 9.3.4.3.5 "Decoding process for binary decisions before termination" */

    const auto skipStopOneBit =
        ElementId::rbsp_slice_segment_trailing_bits == parentId
        && true == streamAccessLayer.peekPrevBit()
        && (
                streamAccessLayer.isByteAligned()
                || false == streamAccessLayer.peekBit());

    if(!skipStopOneBit)
    {
        auto rbspStopOneBit = embed<RbspStopOneBit>(*this);

        parse(streamAccessLayer, decoder, *rbspStopOneBit);
    }

    if(!streamAccessLayer.isByteAligned())
    {
        auto rbspAlignmentZeroBit = embed<RbspAlignmentZeroBit>(*this);

        while(!streamAccessLayer.isByteAligned())
        {
            parse(streamAccessLayer, decoder, *rbspAlignmentZeroBit);
        }
    }
}
/*----------------------------------------------------------------------------*/
}}
