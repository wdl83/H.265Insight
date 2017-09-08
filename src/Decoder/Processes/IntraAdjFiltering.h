#ifndef HEVC_Decoder_Processes_IntraAdjFiltering_h
#define HEVC_Decoder_Processes_IntraAdjFiltering_h

/* HEVC */
#include <HEVC.h>
#include <Decoder/Fwd.h>
#include <Decoder/Processes/ProcessId.h>
#include <Decoder/Processes/IntraAdjSamples.h>
#include <Structure/Fwd.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
struct IntraAdjFiltering
{
    static const auto id = ProcessId::IntraAdjFiltering;

    IntraAdjSamples exec(
            State &,
            Ptr<const Structure::Picture>,
            PelCoord,
            Plane, Log2,
            IntraPredictionMode,
            IntraAdjSamples &&);
};
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processses */

#endif /* HEVC_Decoder_Processes_IntraAdjFiltering_h */
