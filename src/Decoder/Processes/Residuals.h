#ifndef HEVC_Decoder_Processes_Residuals_h
#define HEVC_Decoder_Processes_Residuals_h

/* HEVC */
#include <HEVC.h>
#include <Decoder/Fwd.h>
#include <Decoder/Processes/ProcessId.h>
#include <Syntax/Fwd.h>
#include <Structure/Fwd.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
struct Residuals
{
    static const auto id = ProcessId::Residuals;

    void exec(
            State &, Ptr<Structure::Picture>,
            const Syntax::ResidualCoding &);
};
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processses */

#endif /* HEVC_Decoder_Processes_Residuals_h */
