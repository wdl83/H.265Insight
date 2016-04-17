#ifndef HEVC_Decoder_Processes_IntraChromaPredictionMode_h
#define HEVC_Decoder_Processes_IntraChromaPredictionMode_h

/* HEVC */
#include <HEVC.h>
#include <Decoder/Fwd.h>
#include <Decoder/Processes/ProcessId.h>
#include <Structure/Fwd.h>
#include <Syntax/Fwd.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
struct IntraChromaPredictionMode
{
    static const auto id = ProcessId::IntraChromaPredictionMode;

    CuIntraPredMode exec(
            State &, Ptr<const Structure::Picture>,
            const Syntax::CodingUnit &);
};
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processses */

#endif /* HEVC_Decoder_Processes_IntraChromaPredictionMode_h */
