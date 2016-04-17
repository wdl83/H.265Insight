#ifndef HEVC_Syntax_PpsMultilayerExtension_h
#define HEVC_Syntax_PpsMultilayerExtension_h

/* HEVC */
#include <Syntax/Syntax.h>

namespace HEVC { namespace Syntax { namespace PpsMultilayerExtensionContent {
/*----------------------------------------------------------------------------*/
struct PocResetInfoPresentFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::poc_reset_info_present_flag;
};

struct PpsInferScalingListFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::pps_infer_scaling_list_flag;
};

struct PpsScalingListRefLayerId:
    public Embedded,
    public VLD::FixedUInt<6, uint8_t>
{
    static const auto Id = ElementId::pps_scaling_list_ref_layer_id;
};

struct NumRefLocOffsets:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::num_ref_loc_offsets;
};

struct RefLocOffsetLayerId:
    public Embedded,
    public VLD::FixedUInt<6, uint8_t>
{
    static const auto Id = ElementId::ref_loc_offset_layer_id;
};

struct ScaledRefLayerOffsetPresentFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::scaled_ref_layer_offset_present_flag;
};

struct ScaledRefLayerLeftOffset:
    public Embedded,
    public VLD::IntExpGolombCoded
{
    static const auto Id = ElementId::scaled_ref_layer_left_offset;
};

struct ScaledRefLayerTopOffset:
    public Embedded,
    public VLD::IntExpGolombCoded
{
    static const auto Id = ElementId::scaled_ref_layer_top_offset;
};

struct ScaledRefLayerRightOffset:
    public Embedded,
    public VLD::IntExpGolombCoded
{
    static const auto Id = ElementId::scaled_ref_layer_right_offset;
};

struct ScaledRefLayerBottomOffset:
    public Embedded,
    public VLD::IntExpGolombCoded
{
    static const auto Id = ElementId::scaled_ref_layer_bottom_offset;
};

struct RefRegionOffsetPresentFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::ref_region_offset_present_flag;
};

struct RefRegionLeftOffset:
    public Embedded,
    public VLD::IntExpGolombCoded
{
    static const auto Id = ElementId::ref_region_left_offset;
};

struct RefRegionTopOffset:
    public Embedded,
    public VLD::IntExpGolombCoded
{
    static const auto Id = ElementId::ref_region_top_offset;
};

struct RefRegionRightOffset:
    public Embedded,
    public VLD::IntExpGolombCoded
{
    static const auto Id = ElementId::ref_region_right_offset;
};

struct RefRegionBottomOffset:
    public Embedded,
    public VLD::IntExpGolombCoded
{
    static const auto Id = ElementId::ref_region_bottom_offset;
};

struct ResamplePhaseSetPresentFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::resample_phase_set_present_flag;
};

struct PhaseHorLuma:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::phase_hor_luma;
};

struct PhaseVerLuma:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::phase_ver_luma;
};

struct PhaseHorChromaPlus8:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::phase_hor_chroma_plus8;
};

struct PhaseVerChromaPlus8:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::phase_ver_chroma_plus8;
};

struct ColourMappingEnabledFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::colour_mapping_enabled_flag;
};
/*----------------------------------------------------------------------------*/
} /* spsMultilayerExtensionContent */
/*----------------------------------------------------------------------------*/
struct PpsMultilayerExtension:
    public EmbeddedAggregator<
        PpsMultilayerExtensionContent::PocResetInfoPresentFlag,
        PpsMultilayerExtensionContent::PpsInferScalingListFlag,
        PpsMultilayerExtensionContent::PpsScalingListRefLayerId,
        PpsMultilayerExtensionContent::NumRefLocOffsets,
        PpsMultilayerExtensionContent::RefLocOffsetLayerId,
        PpsMultilayerExtensionContent::ScaledRefLayerOffsetPresentFlag,
        PpsMultilayerExtensionContent::ScaledRefLayerLeftOffset,
        PpsMultilayerExtensionContent::ScaledRefLayerTopOffset,
        PpsMultilayerExtensionContent::ScaledRefLayerRightOffset,
        PpsMultilayerExtensionContent::ScaledRefLayerBottomOffset,
        PpsMultilayerExtensionContent::RefRegionOffsetPresentFlag,
        PpsMultilayerExtensionContent::RefRegionLeftOffset,
        PpsMultilayerExtensionContent::RefRegionTopOffset,
        PpsMultilayerExtensionContent::RefRegionRightOffset,
        PpsMultilayerExtensionContent::RefRegionBottomOffset,
        PpsMultilayerExtensionContent::ResamplePhaseSetPresentFlag,
        PpsMultilayerExtensionContent::PhaseVerLuma,
        PpsMultilayerExtensionContent::PhaseHorLuma,
        PpsMultilayerExtensionContent::PhaseVerChromaPlus8,
        PpsMultilayerExtensionContent::PhaseHorChromaPlus8,
        PpsMultilayerExtensionContent::ColourMappingEnabledFlag>
{
    typedef PpsMultilayerExtensionContent::PocResetInfoPresentFlag PocResetInfoPresentFlag;
    typedef PpsMultilayerExtensionContent::PpsInferScalingListFlag PpsInferScalingListFlag;
    typedef PpsMultilayerExtensionContent::PpsScalingListRefLayerId PpsScalingListRefLayerId;
    typedef PpsMultilayerExtensionContent::NumRefLocOffsets NumRefLocOffsets;
    typedef PpsMultilayerExtensionContent::RefLocOffsetLayerId RefLocOffsetLayerId;
    typedef PpsMultilayerExtensionContent::ScaledRefLayerOffsetPresentFlag ScaledRefLayerOffsetPresentFlag;
    typedef PpsMultilayerExtensionContent::ScaledRefLayerLeftOffset ScaledRefLayerLeftOffset;
    typedef PpsMultilayerExtensionContent::ScaledRefLayerTopOffset ScaledRefLayerTopOffset;
    typedef PpsMultilayerExtensionContent::ScaledRefLayerRightOffset ScaledRefLayerRightOffset;
    typedef PpsMultilayerExtensionContent::ScaledRefLayerBottomOffset ScaledRefLayerBottomOffset;
    typedef PpsMultilayerExtensionContent::RefRegionOffsetPresentFlag RefRegionOffsetPresentFlag;
    typedef PpsMultilayerExtensionContent::RefRegionLeftOffset RefRegionLeftOffset;
    typedef PpsMultilayerExtensionContent::RefRegionTopOffset RefRegionTopOffset;
    typedef PpsMultilayerExtensionContent::RefRegionRightOffset RefRegionRightOffset;
    typedef PpsMultilayerExtensionContent::RefRegionBottomOffset RefRegionBottomOffset;
    typedef PpsMultilayerExtensionContent::ResamplePhaseSetPresentFlag ResamplePhaseSetPresentFlag;
    typedef PpsMultilayerExtensionContent::PhaseVerLuma PhaseVerLuma;
    typedef PpsMultilayerExtensionContent::PhaseHorLuma PhaseHorLuma;
    typedef PpsMultilayerExtensionContent::PhaseVerChromaPlus8 PhaseVerChromaPlus8;
    typedef PpsMultilayerExtensionContent::PhaseHorChromaPlus8 PhaseHorChromaPlus8;
    typedef PpsMultilayerExtensionContent::ColourMappingEnabledFlag ColourMappingEnabledFlag;

    static const auto Id = ElementId::pps_multilayer_extension;

    void onParse(StreamAccessLayer &, Decoder::State &);
};
/*----------------------------------------------------------------------------*/
}} // HEVC::Syntax

#endif /* HEVC_Syntax_PpsMultilayerExtension_h */
