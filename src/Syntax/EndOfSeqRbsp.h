#ifndef HEVC_Syntax_EndOfSeqRbsp_h
#define HEVC_Syntax_EndOfSeqRbsp_h

#include <Syntax/Syntax.h>

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
class EndOfSeqRbsp: public EmbeddedAggregatorTag
{
public:
    static const auto Id = ElementId::end_of_seq_rbsp;

    void onParse(StreamAccessLayer &, Decoder::State &);
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */

#endif /*  HEVC_Syntax_EndOfSeqRbsp_h */
