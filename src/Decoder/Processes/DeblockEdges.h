#ifndef HEVC_Decoder_Processes_DeblockEdges_h
#define HEVC_Decoder_Processes_DeblockEdges_h

/* HEVC */
#include <HEVC.h>
#include <Decoder/Fwd.h>
#include <Decoder/Processes/ProcessId.h>
#include <Syntax/Fwd.h>
#include <Structure/Fwd.h>
#include <Structure/CtbEdgeFlags.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
struct DeblockEdges
{
    Structure::CtbEdgeFlags prediction;
    Structure::CtbEdgeFlags transform;
};

struct PredTransEdges
{
    static const auto id = ProcessId::DeblockEdges;

    DeblockEdges exec(
            State &,
            Ptr<const Structure::Picture>,
            const Syntax::CodingTreeUnit &,
            EdgeType);
};
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */

#endif /* HEVC_Decoder_Processes_DeblockEdges_h */
