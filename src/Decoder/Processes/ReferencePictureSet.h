#ifndef HEVC_Decoder_Processes_ReferencePictureSet_h
#define HEVC_Decoder_Processes_ReferencePictureSet_h

#include <HEVC.h>
#include <Decoder/Fwd.h>
#include <Decoder/Processes/ProcessId.h>
#include <Structure/Fwd.h>
#include <Syntax/Fwd.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
struct ReferencePictureSet
{
    static const auto id = ProcessId::ReferencePictureSet;

    void exec(
            State &, Ptr<Structure::Picture>,
            const Syntax::SliceSegmentHeader &);
};
/*----------------------------------------------------------------------------*/
}}} // namespace HEVC::Decoder::Processes

#endif // HEVC_Decoder_Processes_ReferencePictureSet_h
