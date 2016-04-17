#ifndef HEVC_Decoder_Processes_MvZeroMergeCandiate_h
#define HEVC_Decoder_Processes_MvZeroMergeCandiate_h

#include <HEVC.h>
#include <Decoder/Fwd.h>
#include <Decoder/Processes/ProcessId.h>
#include <Decoder/Processes/MvMergeMode.h>
#include <Syntax/Fwd.h>
#include <Structure/Fwd.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
struct MvZeroMergeCandiate
{
    static const auto id = ProcessId::MvZeroMergeCandidate;

    MvMergeCandList exec(
            State &, Ptr<const Structure::Picture>,
            SliceType,
            Pair<int, RefList>,
            PelCoord,
            MvMergeCandList, int);
};
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */

#endif /* HEVC_Decoder_Processes_MvZeroMergeCandiate_h */
