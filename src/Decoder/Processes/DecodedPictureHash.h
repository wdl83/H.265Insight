#ifndef HEVC_Decoder_Processes_DecodedPictureHash_h
#define HEVC_Decoder_Processes_DecodedPictureHash_h

/* HEVC */
#include <HEVC.h>
#include <Decoder/Fwd.h>
#include <Decoder/Processes/ProcessId.h>
#include <Structure/Fwd.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
struct DecodedPictureHash
{
    static const auto id = ProcessId::DecodedPictureHash;

    void exec(
            State &,
            Ptr<Structure::Picture>);
};
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */

#endif /* HEVC_Decoder_Processes_DecodedPictureHash_h */
