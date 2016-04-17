#ifndef HEVC_Decoder_Processes_SampleAdaptiveOffset_h
#define HEVC_Decoder_Processes_SampleAdaptiveOffset_h

/* HEVC */
#include <HEVC.h>
#include <Decoder/Fwd.h>
#include <Decoder/Processes/ProcessId.h>
#include <Syntax/Fwd.h>
#include <Structure/Fwd.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
struct SampleAdaptiveOffset
{
    static const auto id = ProcessId::SampleAdaptiveOffset;

    void exec(
            State &,
            Ptr<Structure::Picture>,
            const Syntax::CodingTreeUnit &);
};
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */

#endif /* HEVC_Decoder_Processes_SampleAdaptiveOffset_h */
