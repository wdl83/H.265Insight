#ifndef HEVC_Decoder_Processes_MvTemporalPrediction_h
#define HEVC_Decoder_Processes_MvTemporalPrediction_h

/* HEVC */
#include <HEVC.h>
#include <Decoder/Fwd.h>
#include <Decoder/Processes/ProcessId.h>
#include <Syntax/Fwd.h>
#include <Structure/Fwd.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
struct MvTemporalPrediction
{
    static const auto id = ProcessId::MvTemporalPrediction;

    Tuple<MotionVector, bool> exec(
            State &,
            Ptr<const Structure::Picture>,
            PelCoord, Pel, Pel,
            RefList, PicOrderCntVal);
};
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */

#endif /* HEVC_Decoder_Processes_MvTemporalPrediction_h */
