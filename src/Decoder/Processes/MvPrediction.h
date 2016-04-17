#ifndef HEVC_Decoder_Processes_MvPrediction_h
#define HEVC_Decoder_Processes_MvPrediction_h

/* HEVC */
#include <HEVC.h>
#include <Decoder/Fwd.h>
#include <Decoder/Processes/ProcessId.h>
#include <Syntax/Fwd.h>
#include <Structure/Fwd.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
struct MvPrediction
{
    static const auto id = ProcessId::MvPrediction;

    MotionVector exec(
            State &, Ptr<const Structure::Picture>,
            const Syntax::PredictionUnit &,
            RefList, PicOrderCntVal);
};
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */

#endif /* HEVC_Decoder_Processes_MvPrediction_h */
