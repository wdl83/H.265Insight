#ifndef HEVC_Decoder_Processes_MvCollocated_h
#define HEVC_Decoder_Processes_MvCollocated_h

/* HEVC */
#include <HEVC.h>
#include <Decoder/Fwd.h>
#include <Decoder/Processes/ProcessId.h>
#include <Syntax/Fwd.h>
#include <Structure/Fwd.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
struct MvCollocated
{
    static const auto id = ProcessId::MvCollocated;

    Tuple<MotionVector, bool> exec(
            State &,
            Ptr<const Structure::Picture>, Ptr<const Structure::Picture>,
            PelCoord, PelCoord,
            RefList, PicOrderCntVal);
};
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */

#endif /* HEVC_Decoder_Processes_MvCollocated_h */
