#ifndef HEVC_Decoder_Processes_IntraPlanarPrediction_h
#define HEVC_Decoder_Processes_IntraPlanarPrediction_h

/* HEVC */
#include <HEVC.h>
#include <Decoder/Fwd.h>
#include <Decoder/Processes/ProcessId.h>
#include <Decoder/Processes/IntraAdjSamples.h>
#include <Structure/Fwd.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
struct IntraPlanarPrediction
{
    static const auto id = ProcessId::IntraPlanarPrediction;

    void exec(
            State &,
            Structure::PelBuffer &,
            PelCoord, Log2,
            const IntraAdjSamples &);
};
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processses */

#endif /* HEVC_Decoder_Processes_IntraPlanarPrediction_h */
