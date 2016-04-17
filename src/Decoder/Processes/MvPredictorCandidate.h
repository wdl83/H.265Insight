#ifndef HEVC_Decoder_Processes_MvPredictorCandidate_h
#define HEVC_Decoder_Processes_MvPredictorCandidate_h

/* HEVC */
#include <HEVC.h>
#include <Decoder/Fwd.h>
#include <Decoder/Processes/ProcessId.h>
#include <Syntax/Fwd.h>
#include <Structure/Fwd.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
struct MvPredictorCandidate
{
    static const auto id = ProcessId::MvPredictorCandidate;

    Tuple<Pair<MotionVector, Neighbour>, Pair<bool, Neighbour>> exec(
            State &, Ptr<const Structure::Picture>,
            const Syntax::PredictionUnit &,
            RefList, PicOrderCntVal);
};
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */

#endif /* HEVC_Decoder_Processes_MvPredictorCandidate_h */
