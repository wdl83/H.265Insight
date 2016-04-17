#ifndef HEVC_Decoder_Processes_IntraSamplesPrediction_h
#define HEVC_Decoder_Processes_IntraSamplesPrediction_h

/* HEVC */
#include <HEVC.h>
#include <Decoder/Fwd.h>
#include <Decoder/Processes/ProcessId.h>
#include <Syntax/Fwd.h>
#include <Structure/Fwd.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
struct IntraSamplesPrediction
{
    static const auto id = ProcessId::IntraSamplesPrediction;

    void exec(
            State &,
            Ptr<Structure::Picture>,
            Ptr<const Syntax::CodingUnit>,
            Plane,
            PelCoord, Log2,
            PelCoord, Log2);
};
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */

#endif /* HEVC_Decoder_Processes_IntraSamplesPrediction_h */
