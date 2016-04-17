#ifndef HEVC_Decoder_Processes_ProcessId_h
#define HEVC_Decoder_Processes_ProcessId_h

/* STDC++ */
#include <cstdint>
#include <string>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
typedef uint64_t ProcessIdMask;

enum class ProcessId
{
    /* 04/2013, 8.3.1 "Decoding process for picture order count" */
    PictureOrderCount, Begin = PictureOrderCount,
    /* 04/2013, 8.3.2 "Decoding process for reference picture set" */
    ReferencePictureSet,
    /* 04/2013, 8.3.4 "Decoding process for reference picture list construction" */
    ReferencePictureList,
    /* 04/2013, 8.4.2 "Decoding process for luma intra prediction mode" */
    IntraLumaPredictionMode,
    /* 04/2013, 8.4.3 "Decoding process for chroma intra prediction mode" */
    IntraChromaPredictionMode,
    /* 04/2013, 8.4.4.2.1 "General intra sample prediction" */
    IntraSamplesPrediction,
    /* 04/2013, 8.4.4.2.2 "Reference sample substitution process for intra sample prediction" */
    IntraRefSamplesSubstitution,
    /* 04/2013, 8.4.4.2.3 "Filtering process of neighbouring samples" */
    IntraAdjFiltering,
    /* 04/2013, 8.4.4.2.4 "Specification of intra prediction mode INTRA_PLANAR" */
    IntraPlanarPrediction,
    /* 04/2013, 8.4.4.2.5 "Specification of intra prediction mode INTRA_PLANAR" */
    IntraDcPrediction,
    /* 04/2013, 8.4.4.2.6 "Specification of intra prediction mode INTRA_ANGULAR2.. INTRA_ANGULAR34" */
    IntraAngularPrediction,
    /* 04/2013, 8.5.2 "Inter prediction process" */
    InterPrediction,
    /* 04/2013, 8.5.3.2 "Derivation process for motion vector components and reference indices" */
    MvDerivationLuma,
    MvDerivationChroma,
    /* 04/2013, 8.5.3.2.1 "Derivation process for luma motion vectors for merge mode" */
    MvMergeMode,
    /* 04/2013, 8.5.3.2.2 "Derivation process for spatial merging candidates" */
    MvSpatialMergeCandidate,
    /* 04/2013, 8.5.3.2.3 "Derivation process for combined bi-predictive merging candidates" */
    MvCombinedBiPredMergeCandidate,
    /* 04/2013, 8.5.3.2.4 "Derivation process for zero motion vector merging candidates" */
    MvZeroMergeCandidate,
    /* 04/2013, 8.5.3.2.5 "Derivation process for luma motion vector prediction" */
    MvPrediction,
    /* 04/2013, 8.5.3.2.6 "Derivation process for motion vector predictor candidates" */
    MvPredictorCandidate,
    /* 04/2013, 8.5.3.2.7 "Derivation process for temporal luma motion vector predictor" */
    MvTemporalPrediction,
    /* 04/2013, 8.5.3.2.8 "Derivation process for collocated motion vectors" */
    MvCollocated,
    /* 04/2013, 8.5.3.2.9 "Derivation process for chroma motion vectors" */
    // MvDerivation,
    /* 04/2103, 8.5.3.3 "Decoding process for inter prediction samples" */
    InterSamplesPrediction,
    /* 04/2013, 8.5.3.3.3 "Fractional sample interpolation process" */
    FractionalSamplesInterpolation,
    /* 04/2013, 8.5.3.3.4 "Weighted sample prediction process" */
    WeightedSamplesPrediction,
    /* 04/2013, 8.6.1 "Derivation process for quantization parameters" */
    QuantizationParameters,
    QuantizationParametersLuma,
    QuantizationParametersChroma,
    /* 04/2013, 8.6.2 "Scaling and transformation process" */
    Residuals,
    /* 04/2013, 8.6.3 "Scaling process for transform coefficients" */
    TransformCoeffsScaling,
    /* 04/2013, 8.6.4 "Transformation process for scaled coefficients" */
    Transformation,
    /* 10/2014, 8.6.6 "Residual modification process for transform blocks using cross-component prediction" */
    CrossComponentPrediction,
    /* 04/2013, 8.6.5 "Picture construction process prior to in-loop filter process"
     * 10/2014, 8.6.7 "Picture construction process prior to in-loop filter process" */
    Reconstruction,
    /* 04/2013, 8.7 "In-loop filter process" */
    /* 04/2013, 8.7.2 "Deblocking filter process" */
    Deblock,
    /* 04/2013, 8.7.2.2 "Derivation process for transform block boundary"
     * 04/2013, 8.7.2.2 "Derivation process for prediction block boundary" */
    DeblockEdges,
    /* 04/2013, 8.7.2.4 "Derivation process of boundary filtering strength" */
    BoundaryFilteringStrength,
    /* 04/2013, 8.7.2.5.3 "Decision process for luma block edges" */
    LumaBlockEdgeDecision,
    /* 04/2013, 8.7.3 "Sample adaptive offset process" */
    SampleAdaptiveOffset,
    /* D.3.19 "Decoded picture hash SEI message semantics" */
    DecodedPictureHash,
    Undefined, End = Undefined
};

const std::string &getName(ProcessId id);

static const ProcessIdMask intraOnlyMask =
    ProcessIdMask{1} << int(ProcessId::IntraLumaPredictionMode)
    | ProcessIdMask{1} << int(ProcessId::IntraChromaPredictionMode)
    | ProcessIdMask{1} << int(ProcessId::IntraSamplesPrediction)
    | ProcessIdMask{1} << int(ProcessId::IntraRefSamplesSubstitution)
    | ProcessIdMask{1} << int(ProcessId::IntraAdjFiltering)
    | ProcessIdMask{1} << int(ProcessId::IntraPlanarPrediction)
    | ProcessIdMask{1} << int(ProcessId::IntraDcPrediction)
    | ProcessIdMask{1} << int(ProcessId::IntraAngularPrediction);

static const ProcessIdMask interOnlyMask =
    ProcessIdMask{1} << int(ProcessId::ReferencePictureSet)
    | ProcessIdMask{1} << int(ProcessId::ReferencePictureList)
    | ProcessIdMask{1} << int(ProcessId::InterPrediction)
    | ProcessIdMask{1} << int(ProcessId::MvDerivationLuma)
    | ProcessIdMask{1} << int(ProcessId::MvDerivationChroma)
    | ProcessIdMask{1} << int(ProcessId::MvMergeMode)
    | ProcessIdMask{1} << int(ProcessId::MvSpatialMergeCandidate)
    | ProcessIdMask{1} << int(ProcessId::MvCombinedBiPredMergeCandidate)
    | ProcessIdMask{1} << int(ProcessId::MvZeroMergeCandidate)
    | ProcessIdMask{1} << int(ProcessId::MvPrediction)
    | ProcessIdMask{1} << int(ProcessId::MvPredictorCandidate)
    | ProcessIdMask{1} << int(ProcessId::MvTemporalPrediction)
    | ProcessIdMask{1} << int(ProcessId::MvCollocated)
    | ProcessIdMask{1} << int(ProcessId::InterSamplesPrediction)
    | ProcessIdMask{1} << int(ProcessId::FractionalSamplesInterpolation)
    | ProcessIdMask{1} << int(ProcessId::WeightedSamplesPrediction);

static const ProcessIdMask mandatoryMask =
    ProcessIdMask{1} << int(ProcessId::PictureOrderCount)
    | ProcessIdMask{1} << int(ProcessId::QuantizationParameters)
    | ProcessIdMask{1} << int(ProcessId::QuantizationParametersLuma)
    | ProcessIdMask{1} << int(ProcessId::QuantizationParametersChroma)
    | ProcessIdMask{1} << int(ProcessId::Residuals)
    | ProcessIdMask{1} << int(ProcessId::TransformCoeffsScaling)
    | ProcessIdMask{1} << int(ProcessId::Transformation)
    | ProcessIdMask{1} << int(ProcessId::Reconstruction);

static const ProcessIdMask loopFilterMask =
    ProcessIdMask{1} << int(ProcessId::Deblock)
    | ProcessIdMask{1} << int(ProcessId::DeblockEdges)
    | ProcessIdMask{1} << int(ProcessId::BoundaryFilteringStrength)
    | ProcessIdMask{1} << int(ProcessId::LumaBlockEdgeDecision)
    | ProcessIdMask{1} << int(ProcessId::SampleAdaptiveOffset);

inline
bool isMasked(ProcessIdMask mask, ProcessId id)
{
    return ProcessIdMask{0} != (mask & (ProcessIdMask{1} << int(id)));
}

inline
ProcessIdMask mask(ProcessIdMask mask, ProcessId id)
{
    return mask | (ProcessIdMask{1} << int(id));
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */

#endif /* HEVC_Decoder_Processes_ProcessId_h */
