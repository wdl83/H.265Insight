#ifndef HEVC_Decoder_Processes_IntraDcPrediction_h
#define HEVC_Decoder_Processes_IntraDcPrediction_h

/* HEVC */
#include <HEVC.h>
#include <Decoder/Fwd.h>
#include <Decoder/Processes/ProcessId.h>
#include <Decoder/Processes/IntraAdjSamples.h>
#include <Structure/Fwd.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
struct IntraDcPrediction
{
    static const auto id = ProcessId::IntraDcPrediction;

    void exec(
            State &,
            Structure::PelBuffer &,
            PelCoord, Log2, Plane,
            const IntraAdjSamples &);
};
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processses */

#endif /* HEVC_Decoder_Processes_IntraDcPrediction_h */
