#ifndef HEVC_Decoder_Processes_IntraLumaPredictionMode_h
#define HEVC_Decoder_Processes_IntraLumaPredictionMode_h

/* HEVC */
#include <HEVC.h>
#include <Decoder/Fwd.h>
#include <Decoder/Processes/ProcessId.h>
#include <Syntax/Fwd.h>
#include <Structure/Fwd.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
struct IntraLumaPredictionMode
{
    static const auto id = ProcessId::IntraLumaPredictionMode;

    CuIntraPredMode exec(
            State &,
            Ptr<const Structure::Picture>,
            const Syntax::CodingUnit &);
};
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processses */

#endif /* HEVC_Decoder_Processes_IntraLumaPredictionMode_h */
