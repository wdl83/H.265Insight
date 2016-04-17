#ifndef SyntaxEndOfBitstreamRbsp_h
#define SyntaxEndOfBitstreamRbsp_h

#include <Syntax/Syntax.h>

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
class EndOfBitstreamRbsp: public EmbeddedAggregatorTag
{
public:
    static const auto Id = ElementId::end_of_bitstream_rbsp;

    void onParse(StreamAccessLayer &, Decoder::State &);
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */

#endif /*  SyntaxEndOfBitstreamRbsp_h */
