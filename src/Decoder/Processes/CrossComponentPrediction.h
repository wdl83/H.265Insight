#ifndef HEVC_Decoder_Processes_CrossComponentPrediction_h
#define HEVC_Decoder_Processes_CrossComponentPrediction_h

/* HEVC */
#include <HEVC.h>
#include <Decoder/Fwd.h>
#include <Decoder/Processes/ProcessId.h>
#include <Syntax/Fwd.h>
#include <Structure/Fwd.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
struct CrossComponentPrediction
{
    static const auto id = ProcessId::CrossComponentPrediction;

    void exec(
            State &, Ptr<Structure::Picture>,
            const Syntax::TransformUnit &,
            Chroma);
};
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processses */

#endif /* HEVC_Decoder_Processes_CrossComponentPrediction_h */
