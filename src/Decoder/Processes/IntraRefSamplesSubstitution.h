#ifndef HEVC_Decoder_Processes_IntraRefSampleSubstitution_h
#define HEVC_Decoder_Processes_IntraRefSampleSubstitution_h

/* HEVC */
#include <HEVC.h>
#include <Decoder/Fwd.h>
#include <Decoder/Processes/ProcessId.h>
#include <Decoder/Processes/IntraAdjSamples.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
struct IntraRefSamplesSubstitution
{
    static const auto id = ProcessId::IntraRefSamplesSubstitution;

    IntraAdjSamples exec(
            State &,
            Plane, PelCoord, int,
            IntraAdjSamples &&);
};
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processses */

#endif /* HEVC_Decoder_Processes_IntraRefSampleSubstitution_h */
