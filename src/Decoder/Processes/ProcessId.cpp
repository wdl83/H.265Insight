#include <Decoder/Processes/ProcessId.h>
#include <utils.h>
#include <EnumUtils.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
const std::string &getName(ProcessId id)
{
    static const std::string name[] =
    {
        "PictureOrderCount",
        "ReferencePictureSet",
        "ReferencePictureList",
        "IntraLumaPredictionMode",
        "IntraChromaPredictionMode",
        "IntraSamplesPrediction",
        "IntraRefSamplesSubstitution",
        "IntraAdjFiltering",
        "IntraPlanarPrediction",
        "IntraDcPrediction",
        "IntraAngularPrediction",
        "InterPrediction",
        "MvDerivationLuma",
        "MvDerivationChroma",
        "MvMergeMode",
        "MvSpatialMergeCandidate",
        "MvCombinedBiPredMergeCandidate",
        "MvZeroMergeCandidate",
        "MvPrediction",
        "MvPredictorCandidate",
        "MvTemporalPrediction",
        "MvCollocated",
        "InterSamplesPrediction",
        "FractionalSamplesInterpolation",
        "WeightedSamplesPrediction",
        "QuantizationParameters",
        "QuantizationParametersLuma",
        "QuantizationParametersChroma",
        "Residuals",
        "TransformCoeffsScaling",
        "Transformation",
        "CrossComponentPrediction",
        "Reconstruction",
        "Deblock",
        "DeblockEdges",
        "BoundaryFilteringStrength",
        "LumaBlockEdgeDecision",
        "SampleAdaptiveOffset",
        "DecodedPictureHash",
        "Undefined"
    };

    static_assert(
            sizeof(uint64_t) * 8 > int(EnumRange<ProcessId>::length()) + 1,
            "ProcessId: too many ids to fit into ProcessIdMask");
    static_assert(
            int(EnumRange<ProcessId>::length()) + 1 == lengthOf(name),
            "ProcessId: mismatch in numbers of enumerations and strings");

    return name[int(id)];
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */
