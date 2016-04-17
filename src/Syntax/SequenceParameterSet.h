#ifndef HEVC_Syntax_SequenceParameterSet_h
#define HEVC_Syntax_SequenceParameterSet_h
/* HEVC */
#include <Syntax/Syntax.h>
#include <Syntax/ProfileTierLevel.h>
#include <Syntax/ScalingListData.h>
#include <Syntax/ShortTermRefPicSet.h>
#include <Syntax/VuiParameters.h>
#include <Syntax/SpsMultilayerExtension.h>
#include <Syntax/SpsRangeExtension.h>
#include <Syntax/RbspTrailingBits.h>
#include <BitArray.h>

namespace HEVC { namespace Syntax { namespace SequenceParameterSetContent {
/*----------------------------------------------------------------------------*/
struct SpsVideoParameterSetId:
    public Embedded,
    public VLD::FixedUInt<4, uint8_t>
{
    static const auto Id = ElementId::sps_video_parameter_set_id;

    operator int () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct SpsMaxSubLayersMinus1:
    public Embedded,
    public VLD::FixedUInt<3, uint8_t>
{
    static const auto Id = ElementId::sps_max_sub_layers_minus1;

    operator int () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct SpsTemporalIdNestingFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::sps_temporal_id_nesting_flag;
};
/*----------------------------------------------------------------------------*/
struct SeqParameterSetId:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::seq_parameter_set_id;

    operator int () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct ChromaFormatIdc:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::chroma_format_idc;

    operator HEVC::ChromaFormatIdc () const
    {
        return static_cast<HEVC::ChromaFormatIdc>(getValue());
    }

    bool isPresent(Plane plane) const
    {
        if(HEVC::ChromaFormatIdc::f400 == *this && Plane::Y != plane)
        {
            return false;
        }

        return true;
    }
};
/*----------------------------------------------------------------------------*/
struct SeparateColourPlaneFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::separate_colour_plane_flag;

    SeparateColourPlaneFlag()
    {
        /* Draft 10v18, 7.4.2.2 "Sequence parameter set RBSP semantics" */
        /* inferred */
        setValue(false);
    }

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct PicWidthInLumaSamples:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::pic_width_in_luma_samples;

    Pel inUnits() const
    {
        return Pel(getValue());
    }
};
/*----------------------------------------------------------------------------*/
struct PicHeightInLumaSamples:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::pic_height_in_luma_samples;

    Pel inUnits() const
    {
        return Pel(getValue());
    }
};
/*----------------------------------------------------------------------------*/
struct ConformanceWindowFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::conformance_window_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct ConfWinLeftOffset:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::conf_win_left_offset;

    ConfWinLeftOffset()
    {
        /* Draft 10v19, 7.4.2.2. "Sequence parameter set RBSP semantics" */
        setValue(0);
    }

    operator int () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct ConfWinRightOffset:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::conf_win_right_offset;

    ConfWinRightOffset()
    {
        /* Draft 10v19, 7.4.2.2. "Sequence parameter set RBSP semantics" */
        setValue(0);
    }

    operator int () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct ConfWinTopOffset:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::conf_win_top_offset;

    ConfWinTopOffset()
    {
        /* Draft 10v19, 7.4.2.2. "Sequence parameter set RBSP semantics" */
        setValue(0);
    }

    operator int () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct ConfWinBottomOffset:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::conf_win_bottom_offset;

    ConfWinBottomOffset()
    {
        /* Draft 10v19, 7.4.2.2. "Sequence parameter set RBSP semantics" */
        setValue(0);
    }

    operator int () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct BitDepthLumaMinus8:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::bit_depth_luma_minus8;

    int inUnits() const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct BitDepthChromaMinus8:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::bit_depth_chroma_minus8;

    int inUnits() const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct MaxPicOrderCntLsbMinus4:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::log2_max_pic_order_cnt_lsb_minus4;

    Log2 inUnits() const
    {
        return Log2(getValue());
    }
};
/*----------------------------------------------------------------------------*/
struct SpsSubLayerOrderingInfoPresentFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::sps_sub_layer_ordering_info_present_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
template <typename T>
struct SubLayerList
{
protected:
    std::array<T, Limits::VpsMaxSubLayers::num> m_list;
public:
    T operator[] (int i) const
    {
        return m_list[i];
    }
};
/*----------------------------------------------------------------------------*/
struct SpsMaxDecPicBufferingMinus1:
    public SubLayerList<int>,
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::sps_max_dec_pic_buffering_minus1;

    SpsMaxDecPicBufferingMinus1()
    {
        fill(m_list, -1);
    }

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
            const SpsSubLayerOrderingInfoPresentFlag &flag, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list[i] = getValue();

        if(!flag)
        {
            std::fill(std::begin(m_list), advance(std::begin(m_list), i), m_list[i]);
        }

        syntaxCheck(Limits::MaxDpbSize::value > m_list[i]);
        syntaxCheck(0 == i || 0 < i && m_list[i] >= m_list[i - 1]);
    }
};
/*----------------------------------------------------------------------------*/
struct SpsMaxNumReorderPics:
    public SubLayerList<int>,
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::sps_max_num_reorder_pics;

    SpsMaxNumReorderPics()
    {
        fill(m_list, -1);
    }

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
            const SpsSubLayerOrderingInfoPresentFlag &flag, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list[i] = getValue();

        if(!flag)
        {
            std::fill(std::begin(m_list), advance(std::begin(m_list), i), m_list[i]);
        }

        syntaxCheck(0 == i || 0 < i && m_list[i] >= m_list[i - 1]);
    }
};
/*----------------------------------------------------------------------------*/
struct SpsMaxLatencyIncreasePlus1:
    public SubLayerList<uint32_t>,
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::sps_max_latency_increase_plus1;

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
            const SpsSubLayerOrderingInfoPresentFlag &flag, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list[i] = getValue();

        if(!flag)
        {
            std::fill(std::begin(m_list), advance(std::begin(m_list), i), m_list[i]);
        }
    }
};
/*----------------------------------------------------------------------------*/
struct MinLumaCodingBlockSizeMinus3:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::log2_min_luma_coding_block_size_minus3;

    Log2 inUnits() const
    {
        return Log2(getValue());
    }
};
/*----------------------------------------------------------------------------*/
struct DiffMaxMinLumaCodingBlockSize:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::log2_diff_max_min_luma_coding_block_size;

    Log2 inUnits() const
    {
        return Log2(getValue());
    }
};
/*----------------------------------------------------------------------------*/
struct MinTransformBlockSizeMinus2:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::log2_min_transform_block_size_minus2;

    Log2 inUnits() const
    {
        return Log2(getValue());
    }
};
/*----------------------------------------------------------------------------*/
struct DiffMaxMinTransformBlockSize:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::log2_diff_max_min_transform_block_size;

    Log2 inUnits() const
    {
        return Log2(getValue());
    }
};
/*----------------------------------------------------------------------------*/
struct MaxTransformHierarchyDepthInter:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::max_transform_hierarchy_depth_inter;

    operator ValueType() const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct MaxTransformHierarchyDepthIntra:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::max_transform_hierarchy_depth_intra;

    operator ValueType() const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct ScalingListEnabledFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::scaling_list_enabled_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct SpsScalingListDataPresentFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::sps_scaling_list_data_present_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct AmpEnabledFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::amp_enabled_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct SampleAdaptiveOffsetEnabledFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::sample_adaptive_offset_enabled_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct PcmEnabledFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::pcm_enabled_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct PcmSampleBitDepthLumaMinus1:
    public Embedded,
    public VLD::FixedUInt<4, uint8_t>
{
    static const auto Id = ElementId::pcm_sample_bit_depth_luma_minus1;

    int inUnits() const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct PcmSampleBitDepthChromaMinus1:
    public Embedded,
    public VLD::FixedUInt<4, uint8_t>
{
    static const auto Id = ElementId::pcm_sample_bit_depth_chroma_minus1;

    int inUnits() const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct MinPcmLumaCodingBlockSizeMinus3:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::log2_min_pcm_luma_coding_block_size_minus3;

    Log2 inUnits() const
    {
        return Log2(getValue());
    }
};
/*----------------------------------------------------------------------------*/
struct DiffMaxMinPcmLumaCodingBlockSize:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::log2_diff_max_min_pcm_luma_coding_block_size;

    Log2 inUnits() const
    {
        return Log2(getValue());
    }
};
/*----------------------------------------------------------------------------*/
struct PcmLoopFilterDisabledFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::pcm_loop_filter_disabled_flag;

    PcmLoopFilterDisabledFlag()
    {
        /* 04/2013, 7.4.3.2 "Sequence paramter set RBSP semantics" */
        setValue(false);
    }

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct NumShortTermRefPicSets:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::num_short_term_ref_pic_sets;

    explicit operator bool () const
    {
        return getValue();
    }

    operator int () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct LongTermRefPicsPresentFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::long_term_ref_pics_present_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct NumLongTermRefPicsSps:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::num_long_term_ref_pics_sps;

    operator int () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct LtRefPicPocLsbSps:
    public Embedded,
    public VLD::UInt
{
private:
    std::array<int, Limits::NumLongTermRefPicsSps::num> m_list;
public:
    static const auto Id = ElementId::lt_ref_pic_poc_lsb_sps;

    LtRefPicPocLsbSps(const MaxPicOrderCntLsbMinus4 &maxPicOrderCntLsbMinus4)
    {
        setLengthInBits(toUnderlying(maxPicOrderCntLsbMinus4.inUnits()) + 4);
    }

    int operator[] (int i) const
    {
        return m_list[i];
    }

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list[i] = getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct UsedByCurrPicLtSpsFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
private:
    BitArray<Limits::NumLongTermRefPicsSps::num> m_list;
public:
    static const auto Id = ElementId::used_by_curr_pic_lt_sps_flag;

    bool operator[] (int i) const
    {
        return m_list[i];
    }

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list[i] = getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct SpsTemporalMvpEnableFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::sps_temporal_mvp_enable_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct StrongIntraSmoothingEnabledFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::strong_intra_smoothing_enable_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct VuiParametersPresentFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::vui_parameters_present_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct SpsExtensionPresentFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::sps_extension_present_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct SpsRangeExtensionFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::sps_range_extension_flag;

    SpsRangeExtensionFlag()
    {
        /* 10/2014, 7.4.3.2.1 "General sequence parameter set RBSP semantics" */
        setValue(false);
    }

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct SpsMultilayerExtensionFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::sps_multilayer_extension_flag;

    SpsMultilayerExtensionFlag()
    {
        /* 10/2014, 7.4.3.2.1 "General sequence parameter set RBSP semantics" */
        setValue(false);
    }

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct SpsExtension6Bits:
    public Embedded,
    public VLD::FixedUInt<6, uint8_t>
{
    static const auto Id = ElementId::sps_extension_6bits;

    SpsExtension6Bits()
    {
        /* 10/2014, 7.4.3.2.1 "General sequence parameter set RBSP semantics" */
        setValue(false);
    }

    uint8_t inUnits() const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class SpsExtensionDataFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    VLA<bool> m_list;
public:
    static const auto Id = ElementId::sps_extension_data_flag;

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list.emplaceBack(getValue());
    }
};
/*----------------------------------------------------------------------------*/
} /* namespace SequenceParameterSetContent */
/*----------------------------------------------------------------------------*/
class SequenceParameterSet:
    public EmbeddedAggregator<
        SequenceParameterSetContent::SpsVideoParameterSetId,
        SequenceParameterSetContent::SpsMaxSubLayersMinus1,
        SequenceParameterSetContent::SpsTemporalIdNestingFlag,
        SequenceParameterSetContent::SeqParameterSetId,
        SequenceParameterSetContent::ChromaFormatIdc,
        SequenceParameterSetContent::SeparateColourPlaneFlag,
        SequenceParameterSetContent::PicWidthInLumaSamples,
        SequenceParameterSetContent::PicHeightInLumaSamples,
        SequenceParameterSetContent::ConformanceWindowFlag,
        SequenceParameterSetContent::ConfWinLeftOffset,
        SequenceParameterSetContent::ConfWinRightOffset,
        SequenceParameterSetContent::ConfWinTopOffset,
        SequenceParameterSetContent::ConfWinBottomOffset,
        SequenceParameterSetContent::BitDepthLumaMinus8,
        SequenceParameterSetContent::BitDepthChromaMinus8,
        SequenceParameterSetContent::MaxPicOrderCntLsbMinus4,
        SequenceParameterSetContent::SpsSubLayerOrderingInfoPresentFlag,
        SequenceParameterSetContent::SpsMaxDecPicBufferingMinus1,
        SequenceParameterSetContent::SpsMaxNumReorderPics,
        SequenceParameterSetContent::SpsMaxLatencyIncreasePlus1,
        SequenceParameterSetContent::MinLumaCodingBlockSizeMinus3,
        SequenceParameterSetContent::DiffMaxMinLumaCodingBlockSize,
        SequenceParameterSetContent::MinTransformBlockSizeMinus2,
        SequenceParameterSetContent::DiffMaxMinTransformBlockSize,
        SequenceParameterSetContent::MaxTransformHierarchyDepthInter,
        SequenceParameterSetContent::MaxTransformHierarchyDepthIntra,
        SequenceParameterSetContent::ScalingListEnabledFlag,
        SequenceParameterSetContent::SpsScalingListDataPresentFlag,
        SequenceParameterSetContent::AmpEnabledFlag,
        SequenceParameterSetContent::SampleAdaptiveOffsetEnabledFlag,
        SequenceParameterSetContent::PcmEnabledFlag,
        SequenceParameterSetContent::PcmSampleBitDepthLumaMinus1,
        SequenceParameterSetContent::PcmSampleBitDepthChromaMinus1,
        SequenceParameterSetContent::MinPcmLumaCodingBlockSizeMinus3,
        SequenceParameterSetContent::DiffMaxMinPcmLumaCodingBlockSize,
        SequenceParameterSetContent::PcmLoopFilterDisabledFlag,
        SequenceParameterSetContent::NumShortTermRefPicSets,
        SequenceParameterSetContent::LongTermRefPicsPresentFlag,
        SequenceParameterSetContent::NumLongTermRefPicsSps,
        SequenceParameterSetContent::LtRefPicPocLsbSps,
        SequenceParameterSetContent::UsedByCurrPicLtSpsFlag,
        SequenceParameterSetContent::SpsTemporalMvpEnableFlag,
        SequenceParameterSetContent::StrongIntraSmoothingEnabledFlag,
        SequenceParameterSetContent::VuiParametersPresentFlag,
        SequenceParameterSetContent::SpsExtensionPresentFlag,
        SequenceParameterSetContent::SpsRangeExtensionFlag,
        SequenceParameterSetContent::SpsMultilayerExtensionFlag,
        SequenceParameterSetContent::SpsExtension6Bits,
        SequenceParameterSetContent::SpsExtensionDataFlag>,
    public SubtreeAggregator<
        ProfileTierLevel,
        ScalingListData,
        VuiParameters,
        SpsRangeExtension,
        SpsMultilayerExtension,
        RbspTrailingBits>,
    public SubtreeListAggregator<
        ShortTermRefPicSet>
{
public:
    static const auto Id = ElementId::sequence_parameter_set_rbsp;

    typedef SequenceParameterSetContent::SpsVideoParameterSetId SpsVideoParameterSetId;
    typedef SequenceParameterSetContent::SpsMaxSubLayersMinus1 SpsMaxSubLayersMinus1;
    typedef SequenceParameterSetContent::SpsTemporalIdNestingFlag SpsTemporalIdNestingFlag;
    typedef SequenceParameterSetContent::SeqParameterSetId SeqParameterSetId;
    typedef SequenceParameterSetContent::ChromaFormatIdc ChromaFormatIdc;
    typedef SequenceParameterSetContent::SeparateColourPlaneFlag SeparateColourPlaneFlag;
    typedef SequenceParameterSetContent::PicWidthInLumaSamples PicWidthInLumaSamples;
    typedef SequenceParameterSetContent::PicHeightInLumaSamples PicHeightInLumaSamples;
    typedef SequenceParameterSetContent::ConformanceWindowFlag ConformanceWindowFlag;
    typedef SequenceParameterSetContent::ConfWinLeftOffset ConfWinLeftOffset;
    typedef SequenceParameterSetContent::ConfWinRightOffset ConfWinRightOffset;
    typedef SequenceParameterSetContent::ConfWinTopOffset ConfWinTopOffset;
    typedef SequenceParameterSetContent::ConfWinBottomOffset ConfWinBottomOffset;
    typedef SequenceParameterSetContent::BitDepthLumaMinus8 BitDepthLumaMinus8;
    typedef SequenceParameterSetContent::BitDepthChromaMinus8 BitDepthChromaMinus8;
    typedef SequenceParameterSetContent::MaxPicOrderCntLsbMinus4 MaxPicOrderCntLsbMinus4;
    typedef SequenceParameterSetContent::SpsSubLayerOrderingInfoPresentFlag SpsSubLayerOrderingInfoPresentFlag;
    typedef SequenceParameterSetContent::SpsMaxDecPicBufferingMinus1 SpsMaxDecPicBufferingMinus1;
    typedef SequenceParameterSetContent::SpsMaxNumReorderPics SpsMaxNumReorderPics;
    typedef SequenceParameterSetContent::SpsMaxLatencyIncreasePlus1 SpsMaxLatencyIncreasePlus1;
    typedef SequenceParameterSetContent::MinLumaCodingBlockSizeMinus3 MinLumaCodingBlockSizeMinus3;
    typedef SequenceParameterSetContent::DiffMaxMinLumaCodingBlockSize DiffMaxMinLumaCodingBlockSize;
    typedef SequenceParameterSetContent::MinTransformBlockSizeMinus2 MinTransformBlockSizeMinus2;
    typedef SequenceParameterSetContent::DiffMaxMinTransformBlockSize DiffMaxMinTransformBlockSize;
    typedef SequenceParameterSetContent::MaxTransformHierarchyDepthInter MaxTransformHierarchyDepthInter;
    typedef SequenceParameterSetContent::MaxTransformHierarchyDepthIntra MaxTransformHierarchyDepthIntra;
    typedef SequenceParameterSetContent::ScalingListEnabledFlag ScalingListEnabledFlag;
    typedef SequenceParameterSetContent::SpsScalingListDataPresentFlag SpsScalingListDataPresentFlag;
    typedef SequenceParameterSetContent::AmpEnabledFlag AmpEnabledFlag;
    typedef SequenceParameterSetContent::SampleAdaptiveOffsetEnabledFlag SampleAdaptiveOffsetEnabledFlag;
    typedef SequenceParameterSetContent::PcmEnabledFlag PcmEnabledFlag;
    typedef SequenceParameterSetContent::PcmSampleBitDepthLumaMinus1 PcmSampleBitDepthLumaMinus1;
    typedef SequenceParameterSetContent::PcmSampleBitDepthChromaMinus1 PcmSampleBitDepthChromaMinus1;
    typedef SequenceParameterSetContent::MinPcmLumaCodingBlockSizeMinus3 MinPcmLumaCodingBlockSizeMinus3;
    typedef SequenceParameterSetContent::DiffMaxMinPcmLumaCodingBlockSize DiffMaxMinPcmLumaCodingBlockSize;
    typedef SequenceParameterSetContent::PcmLoopFilterDisabledFlag PcmLoopFilterDisabledFlag;
    typedef SequenceParameterSetContent::NumShortTermRefPicSets NumShortTermRefPicSets;
    typedef SequenceParameterSetContent::LongTermRefPicsPresentFlag LongTermRefPicsPresentFlag;
    typedef SequenceParameterSetContent::NumLongTermRefPicsSps NumLongTermRefPicsSps;
    typedef SequenceParameterSetContent::LtRefPicPocLsbSps LtRefPicPocLsbSps;
    typedef SequenceParameterSetContent::UsedByCurrPicLtSpsFlag UsedByCurrPicLtSpsFlag;
    typedef SequenceParameterSetContent::SpsTemporalMvpEnableFlag SpsTemporalMvpEnableFlag;
    typedef SequenceParameterSetContent::StrongIntraSmoothingEnabledFlag StrongIntraSmoothingEnabledFlag;
    typedef SequenceParameterSetContent::VuiParametersPresentFlag VuiParametersPresentFlag;
    typedef SequenceParameterSetContent::SpsExtensionPresentFlag SpsExtensionPresentFlag;
    typedef SequenceParameterSetContent::SpsRangeExtensionFlag SpsRangeExtensionFlag;
    typedef SequenceParameterSetContent::SpsMultilayerExtensionFlag SpsMultilayerExtensionFlag;
    typedef SequenceParameterSetContent::SpsExtension6Bits SpsExtension6Bits;
    typedef SequenceParameterSetContent::SpsExtensionDataFlag SpsExtensionDataFlag;

    enum
    {
        NumberOfProfileCompatabilityFlags = 32
    };

    void toStr(std::ostream &) const;
    void onParse(StreamAccessLayer &, Decoder::State &);
};
/*----------------------------------------------------------------------------*/
}} // HEVC::Syntax

#endif // HEVC_Syntax_SequenceParameterSet_h
