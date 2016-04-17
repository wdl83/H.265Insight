#ifndef HEVC_Decoder_Processes_WeightedSamplesPrediction_h
#define HEVC_Decoder_Processes_WeightedSamplesPrediction_h

/* HEVC */
#include <HEVC.h>
#include <Decoder/Fwd.h>
#include <Decoder/Processes/ProcessId.h>
#include <Decoder/Processes/FractionalSamplesInterpolation.h>
#include <Syntax/Fwd.h>
#include <Structure/Fwd.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
struct WeightedSamplesPrediction
{
    static const auto id = ProcessId::WeightedSamplesPrediction;
    void exec(
            State &, Ptr<Structure::Picture>,
            Ptr<const Structure::Slice>,
            const Syntax::PredictionUnit &,
            Plane, Range<Pel>, Range<Pel>, bool,
            PredSampleLx &&, PredSampleLx &&);
};
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */

#endif /* HEVC_Decoder_Processes_WeightedSamplesPrediction_h */
