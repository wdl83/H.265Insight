#ifndef HEVC_Decoder_Processes_Reconstruction_h
#define HEVC_Decoder_Processes_Reconstruction_h

/* HEVC */
#include <HEVC.h>
#include <Decoder/Fwd.h>
#include <Decoder/Processes/ProcessId.h>
#include <Structure/Fwd.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
struct Reconstruction
{
    static const auto id = ProcessId::Reconstruction;

    void exec(
            State &,
            Ptr<Structure::Picture>,
            Plane,
            PelCoord, Log2,
            PelCoord, Pel, Pel);
};
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */

#endif /* HEVC_Decoder_Processes_Reconstruction_h */
