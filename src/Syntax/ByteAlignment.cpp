#include <Syntax/ByteAlignment.h>

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
void ByteAlignment::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
        ElementId parentId)
{
    /* SPECIAL CASE:
     * 04/2013
     * 9.3.4.3.5 "Decoding process for binary decisions before termination" */

    const auto skipBitEqualToOne =
        ElementId::slice_segment_data == parentId
        && true == streamAccessLayer.peekPrevBit()
        && (
                streamAccessLayer.isByteAligned()
                || false == streamAccessLayer.peekBit());

    if(!skipBitEqualToOne)
    {
        auto alignmentBitEqualToOne = embed<AlignmentBitEqualToOne>(*this);

        parse(streamAccessLayer, decoder, *alignmentBitEqualToOne);
    }

    if(!streamAccessLayer.isByteAligned())
    {
        auto alignmentBitEqualToZero = embed<AlignmentBitEqualToZero>(*this);

        while(!streamAccessLayer.isByteAligned())
        {
            parse(streamAccessLayer, decoder, *alignmentBitEqualToZero);
        }
    }
}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */
