#ifndef HEVC_Decoder_Processes_ReferencePictureList_h
#define HEVC_Decoder_Processes_ReferencePictureList_h

#include <HEVC.h>
#include <Decoder/Fwd.h>
#include <Decoder/Processes/ProcessId.h>
#include <Syntax/Fwd.h>
#include <Structure/Fwd.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
/* 8.3.4 */
struct ReferencePictureList
{
    static const auto id = ProcessId::ReferencePictureList;

    void exec(
            State &, Ptr<Structure::Picture>,
            Ptr<Structure::Slice>,
            const Syntax::SliceSegmentHeader &);
};
/*----------------------------------------------------------------------------*/
}}} // HEVC::Decoder::Processes

#endif // HEVC_Decoder_Processes_ReferencePictureList_h
