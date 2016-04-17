#ifndef HEVC_Syntax_VuiParameters_h
#define HEVC_Syntax_VuiParameters_h

#include <Syntax/Syntax.h>
#include <Syntax/HrdParameters.h>

namespace HEVC { namespace Syntax { namespace VuiParametersContent {
/*----------------------------------------------------------------------------*/
struct AspectRatioInfoPresentFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::aspect_ratio_info_present_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};

struct AspectRatioIdc:
    public Embedded,
    public VLD::FixedUInt<8, uint8_t>
{
    static const auto Id = ElementId::aspect_ratio_idc;

    operator HEVC::AspectRatioIdc () const
    {
        return static_cast<HEVC::AspectRatioIdc>(getValue());
    }
};

struct SarWidth:
    public Embedded,
    public VLD::FixedUInt<16, uint16_t>
{
    static const auto Id = ElementId::sar_width;
};

struct SarHeight:
    public Embedded,
    public VLD::FixedUInt<16, uint16_t>
{
    static const auto Id = ElementId::sar_height;
};

struct OverscanInfoPresentFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::overscan_info_present_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};

struct OverscanAppropriateFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::overscan_appropriate_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};

struct VideoSignalTypePresentFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::video_signal_type_present_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};

struct VideoFormat:
    public Embedded,
    public VLD::FixedUInt<3, uint8_t>
{
    static const auto Id = ElementId::video_format;
};

struct VideoFullRangeFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::video_full_range_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};

struct ColourDescriptionPresentFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::colour_description_present_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};

struct ColourPrimaries:
    public Embedded,
    public VLD::FixedUInt<8, uint8_t>
{
    static const auto Id = ElementId::colour_primaries;
};

struct TransferCharacteristics:
    public Embedded,
    public VLD::FixedUInt<8, uint8_t>
{
    static const auto Id = ElementId::transfer_characteristics;
};

struct MatrixCoeffs:
    public Embedded,
    public VLD::FixedUInt<8, uint8_t>
{
    static const auto Id = ElementId::matrix_coeffs;
};

struct ChromaLocInfoPresentFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::chroma_loc_info_present_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};

struct ChromaSampleLocTypeTopField:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::chroma_sample_loc_type_top_field;
};

struct ChromaSampleLocTypeBottomField:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::chroma_sample_loc_type_bottom_field;
};

struct NeutralChromaIndicationFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::neutral_chroma_indication_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};

struct FieldSeqFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::field_seq_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};

struct FrameFieldInfoPresentFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::frame_field_info_present_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};

struct DefaultDisplayWindowFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::default_display_window_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};

struct DefDispWinLeftOffset:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::def_disp_win_left_offset;
};

struct DefDispWinRightOffset:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::def_disp_win_right_offset;
};

struct DefDispWinTopOffset:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::def_disp_win_top_offset;
};

struct DefDispWinBottomOffset:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::def_disp_win_bottom_offset;
};

struct VuiTimingInfoPresentFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::vui_timing_info_present_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};

struct VuiNumUnitsInTick:
    public Embedded,
    public VLD::FixedUInt<32, uint32_t>
{
    static const auto Id = ElementId::vui_num_units_in_tick;
};

struct VuiTimeScale:
    public Embedded,
    public VLD::FixedUInt<32, uint32_t>
{
    static const auto Id = ElementId::vui_time_scale;
};

struct VuiPocProportionalToTimingFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::vui_poc_proportional_to_timing_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};

struct VuiNumTicksPocDiffOneMinus1:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::vui_num_ticks_poc_diff_one_minus1;
};

struct VuiHrdParametersPresentFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::vui_hrd_parameters_present_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};

struct BitstreamRestrictionFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::bitstream_restriction_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};

struct TilesFixedStructureFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::tiles_fixed_structure_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};

struct MotionVectorsOverPicBoundariesFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::motion_vectors_over_pic_boundaries_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};

struct RestrictedRefPicListsFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::restricted_ref_pic_lists_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};

struct MinSpatialSegmentationIdc:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::min_spatial_segmentation_idc;
};

struct MaxBytesPerPicDenom:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::max_bytes_per_pic_denom;
};

struct MaxBitsPerMinCuDenom:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::max_bits_per_min_cu_denom;
};

struct MaxMvLengthHorizontal:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::log2_max_mv_length_horizontal;
};

struct MaxMvLengthVertical:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::log2_max_mv_length_vertical;
};
/*----------------------------------------------------------------------------*/
} /* VuiParametersContent */

struct VuiParameters:
    public EmbeddedAggregator<
        VuiParametersContent::AspectRatioInfoPresentFlag,
        VuiParametersContent::AspectRatioIdc,
        VuiParametersContent::SarWidth,
        VuiParametersContent::SarHeight,
        VuiParametersContent::OverscanInfoPresentFlag,
        VuiParametersContent::OverscanAppropriateFlag,
        VuiParametersContent::VideoSignalTypePresentFlag,
        VuiParametersContent::VideoFormat,
        VuiParametersContent::VideoFullRangeFlag,
        VuiParametersContent::ColourDescriptionPresentFlag,
        VuiParametersContent::ColourPrimaries,
        VuiParametersContent::TransferCharacteristics,
        VuiParametersContent::MatrixCoeffs,
        VuiParametersContent::ChromaLocInfoPresentFlag,
        VuiParametersContent::ChromaSampleLocTypeTopField,
        VuiParametersContent::ChromaSampleLocTypeBottomField,
        VuiParametersContent::NeutralChromaIndicationFlag,
        VuiParametersContent::FieldSeqFlag,
        VuiParametersContent::FrameFieldInfoPresentFlag,
        VuiParametersContent::DefaultDisplayWindowFlag,
        VuiParametersContent::DefDispWinLeftOffset,
        VuiParametersContent::DefDispWinRightOffset,
        VuiParametersContent::DefDispWinTopOffset,
        VuiParametersContent::DefDispWinBottomOffset,
        VuiParametersContent::VuiTimingInfoPresentFlag,
        VuiParametersContent::VuiNumUnitsInTick,
        VuiParametersContent::VuiTimeScale,
        VuiParametersContent::VuiPocProportionalToTimingFlag,
        VuiParametersContent::VuiNumTicksPocDiffOneMinus1,
        VuiParametersContent::VuiHrdParametersPresentFlag,
        VuiParametersContent::BitstreamRestrictionFlag,
        VuiParametersContent::TilesFixedStructureFlag,
        VuiParametersContent::MotionVectorsOverPicBoundariesFlag,
        VuiParametersContent::RestrictedRefPicListsFlag,
        VuiParametersContent::MinSpatialSegmentationIdc,
        VuiParametersContent::MaxBytesPerPicDenom,
        VuiParametersContent::MaxBitsPerMinCuDenom,
        VuiParametersContent::MaxMvLengthHorizontal,
        VuiParametersContent::MaxMvLengthVertical>,
    public SubtreeAggregator<
        HrdParameters>
{
    static const auto Id = ElementId::vui_parameters;
    typedef VuiParametersContent::AspectRatioInfoPresentFlag AspectRatioInfoPresentFlag;
    typedef VuiParametersContent::AspectRatioIdc AspectRatioIdc;
    typedef VuiParametersContent::SarWidth SarWidth;
    typedef VuiParametersContent::SarHeight SarHeight;
    typedef VuiParametersContent::OverscanInfoPresentFlag OverscanInfoPresentFlag;
    typedef VuiParametersContent::OverscanAppropriateFlag OverscanAppropriateFlag;
    typedef VuiParametersContent::VideoSignalTypePresentFlag VideoSignalTypePresentFlag;
    typedef VuiParametersContent::VideoFormat VideoFormat;
    typedef VuiParametersContent::VideoFullRangeFlag VideoFullRangeFlag;
    typedef VuiParametersContent::ColourDescriptionPresentFlag ColourDescriptionPresentFlag;
    typedef VuiParametersContent::ColourPrimaries ColourPrimaries;
    typedef VuiParametersContent::TransferCharacteristics TransferCharacteristics;
    typedef VuiParametersContent::MatrixCoeffs MatrixCoeffs;
    typedef VuiParametersContent::ChromaLocInfoPresentFlag ChromaLocInfoPresentFlag;
    typedef VuiParametersContent::ChromaSampleLocTypeTopField ChromaSampleLocTypeTopField;
    typedef VuiParametersContent::ChromaSampleLocTypeBottomField ChromaSampleLocTypeBottomField;
    typedef VuiParametersContent::NeutralChromaIndicationFlag NeutralChromaIndicationFlag;
    typedef VuiParametersContent::FieldSeqFlag FieldSeqFlag;
    typedef VuiParametersContent::FrameFieldInfoPresentFlag FrameFieldInfoPresentFlag;
    typedef VuiParametersContent::DefaultDisplayWindowFlag DefaultDisplayWindowFlag;
    typedef VuiParametersContent::DefDispWinLeftOffset DefDispWinLeftOffset;
    typedef VuiParametersContent::DefDispWinRightOffset DefDispWinRightOffset;
    typedef VuiParametersContent::DefDispWinTopOffset DefDispWinTopOffset;
    typedef VuiParametersContent::DefDispWinBottomOffset DefDispWinBottomOffset;
    typedef VuiParametersContent::VuiTimingInfoPresentFlag VuiTimingInfoPresentFlag;
    typedef VuiParametersContent::VuiNumUnitsInTick VuiNumUnitsInTick;
    typedef VuiParametersContent::VuiTimeScale VuiTimeScale;
    typedef VuiParametersContent::VuiPocProportionalToTimingFlag VuiPocProportionalToTimingFlag;
    typedef VuiParametersContent::VuiNumTicksPocDiffOneMinus1 VuiNumTicksPocDiffOneMinus1;
    typedef VuiParametersContent::VuiHrdParametersPresentFlag VuiHrdParametersPresentFlag;
    typedef VuiParametersContent::BitstreamRestrictionFlag BitstreamRestrictionFlag;
    typedef VuiParametersContent::TilesFixedStructureFlag TilesFixedStructureFlag;
    typedef VuiParametersContent::MotionVectorsOverPicBoundariesFlag MotionVectorsOverPicBoundariesFlag;
    typedef VuiParametersContent::RestrictedRefPicListsFlag RestrictedRefPicListsFlag;
    typedef VuiParametersContent::MinSpatialSegmentationIdc MinSpatialSegmentationIdc;
    typedef VuiParametersContent::MaxBytesPerPicDenom MaxBytesPerPicDenom;
    typedef VuiParametersContent::MaxBitsPerMinCuDenom MaxBitsPerMinCuDenom;
    typedef VuiParametersContent::MaxMvLengthHorizontal MaxMvLengthHorizontal;
    typedef VuiParametersContent::MaxMvLengthVertical MaxMvLengthVertical;

    void onParse(StreamAccessLayer &, Decoder::State &, int);
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */

#endif /* HEVC_Syntax_VuiParameters_h */
