#ifndef HEVC_Syntax_Fwd_h
#define HEVC_Syntax_Fwd_h

#include <Syntax/CABAD/Fwd.h>

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
struct EmbeddedAggregatorTag;


class AccessUnitDelimiterRbsp;
class ActiveSet;
class ByteAlignment;

/*----------------------------------------------------------------------------*/
class CodingQuadTree;
namespace CodingQuadTreeContent {
class SplitCuFlag;
class IsCuQpDeltaCoded;
class CuQpDeltaVal;
}
/*----------------------------------------------------------------------------*/
class CodingTreeUnit;
namespace CodingTreeUnitContent {
}
/*----------------------------------------------------------------------------*/
class CodingUnit;
namespace CodingUnitContent {
class CuTransquantBypassFlag;
class CuSkipFlag;
class PredModeFlag;
class CuPredMode;
class PartModeDebinarizer;
class PartMode;
class PartModePseudo;
class PcmFlag;
class PcmAlignmentZeroBit;
class PrevIntraLumaPredFlag;
class MpmIdx;
class RemIntraLumaPredMode;
class IntraChromaPredModeDebinarizer;
class IntraChromaPredMode;
class RqtRootCbf;
class IntraSplitFlag;
class MaxTrafoDepth;
class IntraPredModeY;
class IntraPredModeC;
struct QpY;
struct QpC;
}
/*----------------------------------------------------------------------------*/
class Descriptor;
class Element;
class Embedded;
class EndOfBitstreamRbsp;
class EndOfSeqRbsp;
class FillerDataRbsp;
class HrdParameters;
/*----------------------------------------------------------------------------*/
class MvdCoding;
namespace MvdCodingContent {
class AbsMvdGreater0Flag;
class AbsMvdGreater1Flag;
class AbsMvdMinus2;
class MvdSignFlag;
class LMvd;
}
/*----------------------------------------------------------------------------*/
class NalUnit;
namespace NalUnitContent {
class NumBytesInRbsp;
class RbspByte;
class EmulationPreventionThreeByte;
}
/*----------------------------------------------------------------------------*/
class NalUnitHeader;
namespace NalUnitHeaderContent {
class ForbiddenZeroBit;
class NalUnitType;
class NuhLayerId;
class NuhTemporalIdPlus1;
class TemporalId;
}
/*----------------------------------------------------------------------------*/
class PcmSample;
namespace PcmSampleContent {
struct PcmSampleLuma;
struct PcmSampleChroma;
}
/*----------------------------------------------------------------------------*/
class PictureParameterSet;
namespace PictureParameterSetContent {
class PpsPicParameterSetId;
class PpsSeqParameterSetId;
class DependentSliceSegmentsEnabledFlag;
class OutputFlagPresentFlag;
class NumExtraSliceHeaderBits;
class SignDataHidingEnabledFlag;
class CabacInitPresentFlag;
class NumRefIdxL0DefaultActiveMinus1;
class NumRefIdxL1DefaultActiveMinus1;
class InitQpMinus26;
class ConstrainedIntraPredFlag;
class TransformSkipEnabledFlag;
class CuQpDeltaEnabledFlag;
class DiffCuQpDeltaDepth;
class PpsCbQpOffset;
class PpsCrQpOffset;
class PpsSliceChromaQpOffsetsPresentFlag;
class WeightedPredFlag;
class WeightedBipredFlag;
class TransquantBypassEnableFlag;
class TilesEnabledFlag;
class EntropyCodingSyncEnabledFlag;
class NumTileColumnsMinus1;
class NumTileRowsMinus1;
class UniformSpacingFlag;
class ColumnWidthMinus1;
class RowHeightMinus1;
class LoopFilterAcrossTilesEnabledFlag;
class LoopFilterAcrossSlicesEnabledFlag;
class DeblockingFilterControlPresentFlag;
class DeblockingFilterOverrideEnabledFlag;
class PpsDeblockingFilterDisabledFlag;
class PpsBetaOffsetDiv2;
class PpsTcOffsetDiv2;
class PpsScalingListDataPresentFlag;
class ListsModificationPresentFlag;
class Log2ParallelMergeLevelMinus2;
class SliceSegmentHeaderExtensionPresentFlag;
class PpsExtensionFlag;
class PpsExtensionDataFlag;
}
/*----------------------------------------------------------------------------*/
struct PpsRangeExtension;
namespace PpsRangeExtensionContent {
struct ChromaQpOffsetListLenMinus1;
}
/*----------------------------------------------------------------------------*/
class PredWeightTable;
namespace PredWeightTableContent {
class LumaLog2WeightDenom;
class DeltaChromaLog2WeightDenom;
class ChromaLog2WeightDenom;
struct LumaWeightL0Flag;
struct ChromaWeightL0Flag;
class DeltaLumaWeightL0;
class LumaOffsetL0;
class DeltaChromaWeightL0;
class DeltaChromaOffsetL0;
struct LumaWeightL1Flag;
struct ChromaWeightL1Flag;
class DeltaLumaWeightL1;
class LumaOffsetL1;
class DeltaChromaWeightL1;
class DeltaChromaOffsetL1;
class LumaWeightLx;
class ChromaWeightLx;
class ChromaOffsetLx;
}
/*----------------------------------------------------------------------------*/
class PredictionUnit;
namespace PredictionUnitContent {
class MvLX;
class MvCLX;
class PredFlag;
class RefIdxLX;
class MergeIdx;
class MergeFlag;
class InterPredIdcDebinarizer;
class InterPredIdc;
class RefIdxL0;
class RefIdxL1;
class MvpL0Flag;
class MvpL1Flag;
}
/*----------------------------------------------------------------------------*/
class ProfileTierLevel;
namespace ProfileTierLevelContent {
struct GeneralProfileSpace;
struct GeneralTierFlag;
struct GeneralProfileIdc;
struct GeneralProfileCompatibilityFlag;
struct GeneralProgressiveSourceFlag;
struct GeneralInterlacedSourceFlag;
struct GeneralNonPackedConstraintFlag;
struct GeneralFrameOnlyConstraintFlag;
struct GeneralReservedZero44Bits;
struct GeneralLevelIdc;
struct SubLayerProfilePresentFlag;
struct SubLayerLevelPresentFlag;
struct ReservedZero2Bits;
struct SubLayerProfileSpace;
struct SubLayerTierFlag;
struct SubLayerProfileIdc;
struct SubLayerProfileCompatibilityFlag;
struct SubLayerProgressiveSourceFlag;
struct SubLayerInterlacedSourceFlag;
struct SubLayerNonPackedConstraintFlag;
struct SubLayerFrameOnlyConstraintFlag;
struct SubLayerReservedZero44Bits;
struct SubLayerLevelIdc;
}
/*----------------------------------------------------------------------------*/
class Rbsp;
class RbspSliceSegmentTrailingBits;
class RbspTrailingBits;
/*----------------------------------------------------------------------------*/
class RefPicListModification;
namespace RefPicListModificationContent {
class RefPicListModificationFlagL0;
class ListEntryL0 ;
class RefPicListModificationFlagL1;
class ListEntryL1;
}
/*----------------------------------------------------------------------------*/
class ResidualCoding;
namespace ResidualCodingContent {
class CoeffIdx;
class CIdx;
class ScanIdx;
class TransformSkipFlag;
class LastSigCoeffXPrefix;
class LastSigCoeffYPrefix;
class LastSigCoeffXSuffix;
class LastSigCoeffYSuffix;
class LastSignificantCoeff;
class LastSignificantCoeffX;
class LastSignificantCoeffY;
class CodedSubBlockFlag;
class InferSbDcSigCoeffFlag;
class SigCoeffFlag;
class CoeffAbsLevelGreater1Flag;
class CoeffAbsLevelGreater2Flag;
class CoeffSignFlag;
class CoeffAbsLevelRemainingDebinarizer;
class CoeffAbsLevelRemaining;
class TransformCoeffLevels;
}
/*----------------------------------------------------------------------------*/
class SAO;
namespace SAOContent {
class SaoMergeLeftFlag;
class SaoMergeUpFlag;
class SaoTypeIdxLuma;
class SaoTypeIdxChroma;
class SaoOffsetAbs;
class SaoOffsetSign;
class SaoBandPosition;
class SaoEoClassLuma;
class SaoEoClassChroma;
class SaoTypeIdx;
class SaoOffsetVal;
class IndexToOffset;
class SaoEoClass;
}
/*----------------------------------------------------------------------------*/
class ScalingListData;
namespace ScalingListDataContent {
class ScalingListPredModeFlag;
class ScalingListPredMatrixIdDelta;
class ScalingListDcCoefMinus8;
class ScalingListDeltaCoef;
}
/*----------------------------------------------------------------------------*/
class SequenceParameterSet;
namespace SequenceParameterSetContent {
struct SpsVideoParameterSetId;
struct SpsMaxSubLayersMinus1;
struct SpsTemporalIdNestingFlag;
struct SeqParameterSetId;
struct ChromaFormatIdc;
struct SeparateColourPlaneFlag;
struct PicWidthInLumaSamples;
struct PicHeightInLumaSamples;
struct ConformanceWindowFlag;
struct ConfWinLeftOffset;
struct ConfWinRightOffset;
struct ConfWinTopOffset;
struct ConfWinBottomOffset;
struct BitDepthLumaMinus8;
struct BitDepthChromaMinus8;
struct MaxPicOrderCntLsbMinus4;
struct SpsSubLayerOrderingInfoPresentFlag;
struct SpsMaxDecPicBufferingMinus1;
struct SpsMaxNumReorderPics;
struct SpsMaxLatencyIncrease;
struct MinLumaCodingBlockSizeMinus3;
struct DiffMaxMinLumaCodingBlockSize;
struct MinTransformBlockSizeMinus2;
struct DiffMaxMinTransformBlockSize;
struct MaxTransformHierarchyDepthInter;
struct MaxTransformHierarchyDepthIntra;
struct ScalingListEnabledFlag;
struct SpsScalingListDataPresentFlag;
struct AmpEnabledFlag;
struct SampleAdaptiveOffsetEnabledFlag;
struct PcmEnabledFlag;
struct PcmSampleBitDepthLumaMinus1;
struct PcmSampleBitDepthChromaMinus1;
struct MinPcmLumaCodingBlockSizeMinus3;
struct DiffMaxMinPcmLumaCodingBlockSize;
struct PcmLoopFilterDisableFlag;
struct NumShortTermRefPicSets;
struct LongTermRefPicsPresentFlag;
struct NumLongTermRefPicsSps;
struct LtRefPicPocLsbSps;
struct UsedByCurrPicLtSpsFlag;
struct SpsTemporalMvpEnableFlag;
struct StrongIntraSmoothingEnabledFlag;
struct VuiParametersPresentFlag;
struct SpsExtensionFlag;
class SpsExtensionDataFlag;
}
/*----------------------------------------------------------------------------*/
struct SpsRangeExtension;
/*----------------------------------------------------------------------------*/
class ShortTermRefPicSet;
namespace ShortTermRefPicSetContent {
class InterRefPicSetPredictionFlag;
class DeltaIdxMinus1;
class DeltaRpsSign;
class AbsDeltaRpsMinus1;
struct UsedByCurrPicFlag;
struct UseDeltaFlag;
class NumNegativePics;
class NumPositivePics;
struct DeltaPocS0Minus1;
struct UsedByCurrPicS0Flag;
struct DeltaPocS1Minus1;
struct UsedByCurrPicS1Flag;
class StRpsIdx;
}
/*----------------------------------------------------------------------------*/
class SliceSegmentData;
namespace SliceSegmentDataContent {
class EndOfSliceSegmentFlag;
class EndOfSubStreamOneBit;
}
/*----------------------------------------------------------------------------*/
class SliceSegmentHeader;
namespace SliceSegmentHeaderContent {
class FirstSliceSegmentInPicFlag;
class NoOutputOfPriorPicsFlag;
class SlicePicParameterSetId;
class DependentSliceSegmentFlag;
class SliceSegmentAddress;
class SliceReservedFlag;
class SliceType;
class PicOutputFlag;
class ColourPlaneId;
class SlicePicOrderCntLsb;
class ShortTermRefPicSetSpsFlag;
class ShortTermRefPicSetIdx;
class NumLongTermSps;
class NumLongTermPics;
class LtIdxSps;
class PocLsbLt;
class UsedByCurrPicLtFlag;
class DeltaPocMsbPresentFlag;
class DeltaPocMsbCycleLt;
class SliceTemporalMvpEnableFlag;
class SliceSaoLumaFlag;
class SliceSaoChromaFlag;
class NumRefIdxActiveOverrideFlag;
class NumRefIdxL0ActiveMinus1;
class NumRefIdxL1ActiveMinus1;
class MvdL1ZeroFlag;
class CabacInitFlag;
class CollocatedFromL0Flag;
class CollocatedRefIdx;
class FiveMinusMaxNumMergeCand;
class SliceQpDelta;
class SliceCbQpOffset;
class SliceCrQpOffset;
class DeblockingFilterOverrideFlag;
class SliceDisableDeblockingFilterFlag;
class SliceBetaOffsetDiv2;
class SliceTcOffsetDiv2;
class SliceLoopFilterAcrossSlicesEnabledFlag;
class NumEntryPointOffsets;
class OffsetLenMinus1;
class EntryPointOffsetMinus1;
class SliceSegmentHeaderExtensionLength;
class SliceSegmentHeaderExtensionDataByte;
class MaxNumMergeCand;
class SliceAddrRs;
class CurrRpsIdx;
class SliceQpY;
class CabacInitType;
}
/*----------------------------------------------------------------------------*/
class SliceSegmentLayerRbsp;
/*----------------------------------------------------------------------------*/
class StreamNalUnit;
namespace StreamNalUnitContent {
class LeadingZero8Bits;
class TrailingZero8Bits;
class ZeroByte;
class StartCodePrefixOne3Bytes;
class NumBytesInNalUnit;
}
/*----------------------------------------------------------------------------*/
class TransformTree;
namespace TransformTreeContent {
class InterSplitFlag;
class SplitTransformFlag;
class CbfCb;
class CbfCr;
class CbfLuma;
}
/*----------------------------------------------------------------------------*/
class TransformUnit;
namespace TransformUnitContent {
class CuQpDeltaAbsDebinarizer;
struct CuQpDeltaAbs;
class CuQpDeltaSignFlag;
}
/*----------------------------------------------------------------------------*/
class VideoParameterSet;
namespace VideoParameterSetContent {
struct VpsVideoParameterSetId;
struct VpsReservedThree2bits;
struct VpsMaxLayersMinus1;
struct VpsMaxSubLayersMinus1;
struct VpsTemporalIdNestingFlag;
struct VpsReserved0xffff16bits;
struct VpsSubLayerOrderingInfoPresentFlag;
struct VpsMaxDecPicBufferingMinus1;
struct VpsMaxNumReorderPics;
struct VpsMaxLatencyIncrease;
struct VpsMaxLayerId;
struct VpsNumLayerSetsMinus1;
struct LayerIdIncludedFlag;
struct VpsTimingInfoPresentFlag;
struct VpsNumUnitsInTick;
struct VpsTimeScale;
struct VpsPocProportionalToTimingFlag;
struct VpsNumTicksPocDiffOneMinus1;
struct VpsNumHrdParameters;
class HrdLayerSetIdx;
class CprmsPresentFlag;
struct VpsExtensionFlag;
class VpsExtensionDataFlag;
}
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Descriptors */
/*----------------------------------------------------------------------------*/

namespace VLD {

struct TagVLD;

}
/*----------------------------------------------------------------------------*/
}} // namespace HEVC::Syntax

#endif // HEVC_Syntax_Fwd_h
