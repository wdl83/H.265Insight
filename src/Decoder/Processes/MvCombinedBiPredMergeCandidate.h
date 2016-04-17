#ifndef HEVC_Decoder_Processes_MvCombinedBiPredMergeCandidate_h
#define HEVC_Decoder_Processes_MvCombinedBiPredMergeCandidate_h

/* HEVC */
#include <HEVC.h>
#include <Decoder/Fwd.h>
#include <Decoder/Processes/ProcessId.h>
#include <Decoder/Processes/MvMergeMode.h>
#include <Syntax/Fwd.h>
#include <Structure/Fwd.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
struct MvCombinedBiPredMergeCandidate
{
    static const auto id = ProcessId::MvCombinedBiPredMergeCandidate;

    MvMergeCandList exec(
            State &, Ptr<const Structure::Picture>,
            PelCoord,
            MvMergeCandList, int, int);
};
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */

#endif /* HEVC_Decoder_Processes_MvCombinedBiPredMergeCandidate_h */
