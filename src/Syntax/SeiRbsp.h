#ifndef HEVC_Syntax_SeiRbsp_h
#define HEVC_Syntax_SeiRbsp_h

#include <Syntax/Syntax.h>
#include <Syntax/RbspTrailingBits.h>
#include <Syntax/SeiMessage.h>

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
class SeiRbsp:
    public EmbeddedAggregatorTag,
    public SubtreeAggregator<
        RbspTrailingBits>,
    public SubtreeListAggregator<
        SeiMessage>
{
public:
    static const auto Id = ElementId::sei_rbsp;
    void onParse(StreamAccessLayer &, Decoder::State &decoder, NalUnitType);
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */

#endif /* HEVC_Syntax_SeiRbsp_h */
