#ifndef HEVC_Decoder_Processes_BoundaryFilteringStrength_h
#define HEVC_Decoder_Processes_BoundaryFilteringStrength_h

/* HEVC */
#include <HEVC.h>
#include <Decoder/Fwd.h>
#include <Decoder/Processes/ProcessId.h>
#include <Decoder/Processes/Fwd.h>
#include <Syntax/Fwd.h>
#include <Structure/Fwd.h>
#include <Structure/CtbEdgeFlags.h>
#include <Structure/CtbEdgeBS.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
struct BoundaryFilteringStrength
{
    static const auto id = ProcessId::BoundaryFilteringStrength;

    Structure::CtbEdgeBS exec(
            State &, Ptr<const Structure::Picture>,
            const Syntax::CodingTreeUnit &,
            const DeblockEdges &,
            EdgeType);
};
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */

#endif /* HEVC_Decoder_Processes_BoundaryFilteringStrength_h */
