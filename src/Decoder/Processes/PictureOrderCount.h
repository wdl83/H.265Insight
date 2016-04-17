#ifndef HEVC_Decoder_Processes_PictureOrderCount_h
#define HEVC_Decoder_Processes_PictureOrderCount_h

#include <HEVC.h>
#include <Decoder/Fwd.h>
#include <Decoder/Processes/ProcessId.h>
#include <Syntax/Fwd.h>
#include <Structure/Fwd.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
/* 8.3.1 */
struct PictureOrderCount
{
    static const auto id = ProcessId::PictureOrderCount;

    void exec(
            State &, Ptr<Structure::Picture>,
            const Syntax::SliceSegmentHeader &);
};
/*----------------------------------------------------------------------------*/
}}} // namespace HEVC::Decoder::Processes

#endif /* HEVC_Decoder_Processes_PictureOrderCount_h */
