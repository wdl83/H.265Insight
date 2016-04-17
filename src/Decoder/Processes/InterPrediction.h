#ifndef HEVC_Decoder_Processes_InterPrediction_h
#define HEVC_Decoder_Processes_InterPrediction_h

/* HEVC */
#include <HEVC.h>
#include <Decoder/Fwd.h>
#include <Decoder/Processes/ProcessId.h>
#include <Syntax/Fwd.h>
#include <Structure/Fwd.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
struct InterPrediction
{
    static const auto id = ProcessId::InterPrediction;

    void exec(
            State &,
            Ptr<Structure::Picture>,
            Syntax::PredictionUnit &);
};
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */

#endif /* HEVC_Decoder_Processes_InterPrediction_h */
