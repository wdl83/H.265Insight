#ifndef HEVC_Decoder_Processes_Transformation_h
#define HEVC_Decoder_Processes_Transformation_h

/* HEVC */
#include <HEVC.h>
#include <Decoder/Fwd.h>
#include <Decoder/Processes/ProcessId.h>
#include <Syntax/Fwd.h>
#include <Structure/Fwd.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
struct Transformation
{
    static const auto id = ProcessId::Transformation;

    void exec(
            State &, Ptr<Structure::Picture>,
            const Syntax::CodingUnit &,
            const Syntax::ResidualCoding &);
};
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processses */

#endif /* HEVC_Decoder_Processes_Transformation_h */
