#ifndef HEVC_Decoder_Processes_InterSamplesPrediction_h
#define HEVC_Decoder_Processes_InterSamplesPrediction_h

/* HEVC */
#include <HEVC.h>
#include <Decoder/Fwd.h>
#include <Decoder/Processes/ProcessId.h>
#include <Syntax/Fwd.h>
#include <Structure/Fwd.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
struct  InterSamplesPrediction
{
    static const auto id = ProcessId::InterSamplesPrediction;

    void exec(
            State &, Ptr<Structure::Picture>,
            const Syntax::PredictionUnit &pu);
};
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */

#endif /* HEVC_Decoder_Processes_InterSamplesPrediction_h */
