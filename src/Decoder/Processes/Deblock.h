#ifndef HEVC_Decoder_Processes_Deblock_h
#define HEVC_Decoder_Processes_Deblock_h

/* HEVC */
#include <HEVC.h>
#include <Decoder/Fwd.h>
#include <Decoder/Processes/ProcessId.h>
#include <Syntax/Fwd.h>
#include <Structure/Fwd.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
struct Deblock
{
    static const auto id = ProcessId::Deblock;

    void exec(
            State &,
            Ptr<Structure::Picture>,
            const Syntax::CodingTreeUnit &,
            EdgeType);
};
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */

#endif /* HEVC_Decoder_Processes_Deblock_h */
