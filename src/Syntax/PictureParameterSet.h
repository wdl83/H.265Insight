#ifndef HEVC_Syntax_PictureParameterSet_h
#define HEVC_Syntax_PictureParameterSet_h

/* HEVC */
#include <Syntax/Syntax.h>
#include <Syntax/PpsMultilayerExtension.h>
#include <Syntax/PpsRangeExtension.h>
#include <Syntax/ScalingListData.h>
#include <Syntax/RbspTrailingBits.h>

namespace HEVC { namespace Syntax { namespace PictureParameterSetContent {
/*----------------------------------------------------------------------------*/
class PpsPicParameterSetId:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
public:
    static const auto Id = ElementId::pps_pic_parameter_set_id;

    operator int () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class PpsSeqParameterSetId:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
public:
    static const auto Id = ElementId::pps_seq_parameter_set_id;

    operator int () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class DependentSliceSegmentsEnabledFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
public:
    static const auto Id = ElementId::dependent_slice_segments_enabled_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class OutputFlagPresentFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
public:
    static const auto Id = ElementId::output_flag_present_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class NumExtraSliceHeaderBits:
    public Embedded,
    public VLD::FixedUInt<3, uint8_t>
{
public:
    static const auto Id = ElementId::num_extra_slice_header_bits;

    operator int () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class SignDataHidingEnabledFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
public:
    static const auto Id = ElementId::sign_data_hiding_enabled_flag;

    explicit operator bool () const
    {
        return 0 != getValue();
    }
};
/*----------------------------------------------------------------------------*/
class CabacInitPresentFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
public:
    static const auto Id = ElementId::cabac_init_present_flag;

    explicit operator bool () const
    {
        return 0 != getValue();
    }
};
/*----------------------------------------------------------------------------*/
class NumRefIdxL0DefaultActiveMinus1:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
public:
    static const auto Id = ElementId::num_ref_idx_l0_default_active_minus1;

    operator ValueType () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class NumRefIdxL1DefaultActiveMinus1:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
public:
    static const auto Id = ElementId::num_ref_idx_l1_default_active_minus1;

    operator ValueType () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class InitQpMinus26:
    public Embedded,
    public VLD::IntExpGolombCoded
{
public:
    static const auto Id = ElementId::init_qp_minus26;

    int inUnits() const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class ConstrainedIntraPredFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
public:
    static const auto Id = ElementId::constrained_intra_pred_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class TransformSkipEnabledFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
public:
    static const auto Id = ElementId::transform_skip_enabled_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class CuQpDeltaEnabledFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
public:
    static const auto Id = ElementId::cu_qp_delta_enabled_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class DiffCuQpDeltaDepth:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
public:
    static const auto Id = ElementId::diff_cu_qp_delta_depth;

    DiffCuQpDeltaDepth()
    {
        /* Draft 10v19, 7.4.2.3 "Picture parameter set RBSP semantics." */
        setValue(0);
    }

    Log2 inUnits() const
    {
        return Log2(getValue());
    }
};
/*----------------------------------------------------------------------------*/
class PpsCbQpOffset:
    public Embedded,
    public VLD::IntExpGolombCoded
{
public:
    static const auto Id = ElementId::pps_cb_qp_offset;

    int inUnits() const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class PpsCrQpOffset:
    public Embedded,
    public VLD::IntExpGolombCoded
{
public:
    static const auto Id = ElementId::pps_cr_qp_offset;

    int inUnits() const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class PpsSliceChromaQpOffsetsPresentFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
public:
    static const auto Id = ElementId::pps_slice_chroma_qp_offsets_present_flag;

    explicit operator bool() const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class WeightedPredFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
public:
    static const auto Id = ElementId::weighted_pred_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class WeightedBipredFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
public:
    static const auto Id = ElementId::weighted_bipred_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class TransquantBypassEnableFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
public:
    static const auto Id = ElementId::transquant_bypass_enable_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class TilesEnabledFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
public:
    static const auto Id = ElementId::tiles_enabled_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class EntropyCodingSyncEnabledFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
public:
    static const auto Id = ElementId::entropy_coding_sync_enabled_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class NumTileColumnsMinus1:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
public:
    static const auto Id = ElementId::num_tile_columns_minus1;

    NumTileColumnsMinus1()
    {
        /* inferred */
        setValue(0);
    }

    int inUnits() const
    {
        /* Draft 10v19, 7.4.2.3 "Picture parameter set RBSP semantics." */
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class NumTileRowsMinus1:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
public:
    static const auto Id = ElementId::num_tile_rows_minus1;

    NumTileRowsMinus1()
    {
        /* inferred */
        setValue(0);
    }

    int inUnits() const
    {
        /* Draft 10v19, 7.4.2.3 "Picture parameter set RBSP semantics." */
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class UniformSpacingFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
public:
    static const auto Id = ElementId::uniform_spacing_flag;

    UniformSpacingFlag()
    {
        /* Draft 10v19, 7.4.2.3 "Picture parameter set RBSP semantics." */
        setValue(true);
    }

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class ColumnWidthMinus1:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    std::array<uint16_t, Limits::MaxPicWidthInMinCtb::value> m_list;
public:
    static const auto Id = ElementId::column_width_minus1;

    ColumnWidthMinus1()
    {
        fill(m_list, uint16_t{0});
    }

    Ctb operator[] (int i) const
    {
        return Ctb(m_list[i]);
    }

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list[i] = getValue();
    }
};
/*----------------------------------------------------------------------------*/
class RowHeightMinus1:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    std::array<uint16_t, Limits::MaxPicWidthInMinCtb::value> m_list;
public:
    static const auto Id = ElementId::row_height_minus1;

    RowHeightMinus1()
    {
        fill(m_list, uint16_t{0});
    }

    Ctb operator[] (int i) const
    {
        return Ctb(m_list[i]);
    }

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list[i] = getValue();
    }
};
/*----------------------------------------------------------------------------*/
class LoopFilterAcrossTilesEnabledFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
public:
    static const auto Id = ElementId::loop_filter_across_tiles_enabled_flag;

    LoopFilterAcrossTilesEnabledFlag()
    {
        /* Draft 10v19, 7.4.2.3 "Picture parameter set RBSP semantics." */
        setValue(true);
    }

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class PpsLoopFilterAcrossSlicesEnabledFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
public:
    static const auto Id = ElementId::pps_loop_filter_across_slices_enabled_flag;

    explicit operator bool() const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class DeblockingFilterControlPresentFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
public:
    static const auto Id = ElementId::deblocking_filter_control_present_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class DeblockingFilterOverrideEnabledFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
public:
    static const auto Id = ElementId::deblocking_filter_override_enabled_flag;

    DeblockingFilterOverrideEnabledFlag()
    {
        /* Draft 10v19, 7.4.2.3 "Picture parameter set RBSP semantics." */
        setValue(false);
    }

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class PpsDeblockingFilterDisabledFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
public:
    static const auto Id = ElementId::pps_deblocking_filter_disabled_flag;

    PpsDeblockingFilterDisabledFlag()
    {
        /* Draft 10v19, 7.4.2.3 "Picture parameter set RBSP semantics." */
        setValue(false);
    }

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class PpsBetaOffsetDiv2:
    public Embedded,
    public VLD::IntExpGolombCoded
{
public:
    static const auto Id = ElementId::pps_beta_offset_div2;

    PpsBetaOffsetDiv2()
    {
        /* Draft 10v19, 7.4.2.3 "Picture parameter set RBSP semantics." */
        setValue(0);
    }

    operator int () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class PpsTcOffsetDiv2:
    public Embedded,
    public VLD::IntExpGolombCoded
{
public:
    static const auto Id = ElementId::pps_tc_offset_div2;

    PpsTcOffsetDiv2()
    {
        /* Draft 10v19, 7.4.2.3 "Picture parameter set RBSP semantics." */
        setValue(0);
    }

    operator int () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class PpsScalingListDataPresentFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
public:
    static const auto Id = ElementId::pps_scaling_list_data_present_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class ListsModificationPresentFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
public:
    static const auto Id = ElementId::lists_modification_present_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class Log2ParallelMergeLevelMinus2:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
public:
    static const auto Id = ElementId::log2_parallel_merge_level_minus2;

    operator Log2 () const
    {
        return Log2(getValue());
    }
};
/*----------------------------------------------------------------------------*/
class SliceSegmentHeaderExtensionPresentFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
public:
    static const auto Id = ElementId::slice_segment_header_extension_present_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class PpsExtensionPresentFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
public:
    static const auto Id = ElementId::pps_extension_present_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct PpsRangeExtensionFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::pps_range_extension_flag;

    PpsRangeExtensionFlag()
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
struct PpsMultilayerExtensionFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::pps_multilayer_extension_flag;

    PpsMultilayerExtensionFlag()
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
struct PpsExtension6Bits:
    public Embedded,
    public VLD::FixedUInt<6, uint8_t>
{
    static const auto Id = ElementId::pps_extension_6bits;

    PpsExtension6Bits()
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
class PpsExtensionDataFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    VLA<bool> m_list;
public:
    static const auto Id = ElementId::pps_extension_data_flag;

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list.emplaceBack(getValue());
    }
};
/*----------------------------------------------------------------------------*/
} /* namespace PictureParameterSetContent */

/*----------------------------------------------------------------------------*/
class PictureParameterSet:
    public EmbeddedAggregator<
        PictureParameterSetContent::PpsPicParameterSetId,
        PictureParameterSetContent::PpsSeqParameterSetId,
        PictureParameterSetContent::DependentSliceSegmentsEnabledFlag,
        PictureParameterSetContent::OutputFlagPresentFlag,
        PictureParameterSetContent::NumExtraSliceHeaderBits,
        PictureParameterSetContent::SignDataHidingEnabledFlag,
        PictureParameterSetContent::CabacInitPresentFlag,
        PictureParameterSetContent::NumRefIdxL0DefaultActiveMinus1,
        PictureParameterSetContent::NumRefIdxL1DefaultActiveMinus1,
        PictureParameterSetContent::InitQpMinus26,
        PictureParameterSetContent::ConstrainedIntraPredFlag,
        PictureParameterSetContent::TransformSkipEnabledFlag,
        PictureParameterSetContent::CuQpDeltaEnabledFlag,
        PictureParameterSetContent::DiffCuQpDeltaDepth,
        PictureParameterSetContent::PpsCbQpOffset,
        PictureParameterSetContent::PpsCrQpOffset,
        PictureParameterSetContent::PpsSliceChromaQpOffsetsPresentFlag,
        PictureParameterSetContent::WeightedPredFlag,
        PictureParameterSetContent::WeightedBipredFlag,
        PictureParameterSetContent::TransquantBypassEnableFlag,
        PictureParameterSetContent::TilesEnabledFlag,
        PictureParameterSetContent::EntropyCodingSyncEnabledFlag,
        PictureParameterSetContent::NumTileColumnsMinus1,
        PictureParameterSetContent::NumTileRowsMinus1,
        PictureParameterSetContent::UniformSpacingFlag,
        PictureParameterSetContent::ColumnWidthMinus1,
        PictureParameterSetContent::RowHeightMinus1,
        PictureParameterSetContent::LoopFilterAcrossTilesEnabledFlag,
        PictureParameterSetContent::PpsLoopFilterAcrossSlicesEnabledFlag,
        PictureParameterSetContent::DeblockingFilterControlPresentFlag,
        PictureParameterSetContent::DeblockingFilterOverrideEnabledFlag,
        PictureParameterSetContent::PpsDeblockingFilterDisabledFlag,
        PictureParameterSetContent::PpsBetaOffsetDiv2,
        PictureParameterSetContent::PpsTcOffsetDiv2,
        PictureParameterSetContent::PpsScalingListDataPresentFlag,
        PictureParameterSetContent::ListsModificationPresentFlag,
        PictureParameterSetContent::Log2ParallelMergeLevelMinus2,
        PictureParameterSetContent::SliceSegmentHeaderExtensionPresentFlag,
        PictureParameterSetContent::PpsExtensionPresentFlag,
        PictureParameterSetContent::PpsRangeExtensionFlag,
        PictureParameterSetContent::PpsMultilayerExtensionFlag,
        PictureParameterSetContent::PpsExtension6Bits,
        PictureParameterSetContent::PpsExtensionDataFlag>,
    public SubtreeAggregator<
        ScalingListData,
        PpsRangeExtension,
        PpsMultilayerExtension,
        RbspTrailingBits>
{
public:
    static const auto Id = ElementId::pic_parameter_set_rbsp;

    typedef PictureParameterSetContent::PpsPicParameterSetId PpsPicParameterSetId;
    typedef PictureParameterSetContent::PpsSeqParameterSetId PpsSeqParameterSetId;
    typedef PictureParameterSetContent::DependentSliceSegmentsEnabledFlag DependentSliceSegmentsEnabledFlag;
    typedef PictureParameterSetContent::OutputFlagPresentFlag OutputFlagPresentFlag;
    typedef PictureParameterSetContent::NumExtraSliceHeaderBits NumExtraSliceHeaderBits;
    typedef PictureParameterSetContent::SignDataHidingEnabledFlag SignDataHidingEnabledFlag;
    typedef PictureParameterSetContent::CabacInitPresentFlag CabacInitPresentFlag;
    typedef PictureParameterSetContent::NumRefIdxL0DefaultActiveMinus1 NumRefIdxL0DefaultActiveMinus1;
    typedef PictureParameterSetContent::NumRefIdxL1DefaultActiveMinus1 NumRefIdxL1DefaultActiveMinus1;
    typedef PictureParameterSetContent::InitQpMinus26 InitQpMinus26;
    typedef PictureParameterSetContent::ConstrainedIntraPredFlag ConstrainedIntraPredFlag;
    typedef PictureParameterSetContent::TransformSkipEnabledFlag TransformSkipEnabledFlag;
    typedef PictureParameterSetContent::CuQpDeltaEnabledFlag CuQpDeltaEnabledFlag;
    typedef PictureParameterSetContent::DiffCuQpDeltaDepth DiffCuQpDeltaDepth;
    typedef PictureParameterSetContent::PpsCbQpOffset PpsCbQpOffset;
    typedef PictureParameterSetContent::PpsCrQpOffset PpsCrQpOffset;
    typedef PictureParameterSetContent::PpsSliceChromaQpOffsetsPresentFlag PpsSliceChromaQpOffsetsPresentFlag;
    typedef PictureParameterSetContent::WeightedPredFlag WeightedPredFlag;
    typedef PictureParameterSetContent::WeightedBipredFlag WeightedBipredFlag;
    typedef PictureParameterSetContent::TransquantBypassEnableFlag TransquantBypassEnableFlag;
    typedef PictureParameterSetContent::TilesEnabledFlag TilesEnabledFlag;
    typedef PictureParameterSetContent::EntropyCodingSyncEnabledFlag EntropyCodingSyncEnabledFlag;
    typedef PictureParameterSetContent::NumTileColumnsMinus1 NumTileColumnsMinus1;
    typedef PictureParameterSetContent::NumTileRowsMinus1 NumTileRowsMinus1;
    typedef PictureParameterSetContent::UniformSpacingFlag UniformSpacingFlag;
    typedef PictureParameterSetContent::ColumnWidthMinus1 ColumnWidthMinus1;
    typedef PictureParameterSetContent::RowHeightMinus1 RowHeightMinus1;
    typedef PictureParameterSetContent::LoopFilterAcrossTilesEnabledFlag LoopFilterAcrossTilesEnabledFlag;
    typedef PictureParameterSetContent::PpsLoopFilterAcrossSlicesEnabledFlag PpsLoopFilterAcrossSlicesEnabledFlag;
    typedef PictureParameterSetContent::DeblockingFilterControlPresentFlag DeblockingFilterControlPresentFlag;
    typedef PictureParameterSetContent::DeblockingFilterOverrideEnabledFlag DeblockingFilterOverrideEnabledFlag;
    typedef PictureParameterSetContent::PpsDeblockingFilterDisabledFlag PpsDeblockingFilterDisabledFlag;
    typedef PictureParameterSetContent::PpsBetaOffsetDiv2 PpsBetaOffsetDiv2;
    typedef PictureParameterSetContent::PpsTcOffsetDiv2 PpsTcOffsetDiv2;
    typedef PictureParameterSetContent::PpsScalingListDataPresentFlag PpsScalingListDataPresentFlag;
    typedef PictureParameterSetContent::ListsModificationPresentFlag ListsModificationPresentFlag;
    typedef PictureParameterSetContent::Log2ParallelMergeLevelMinus2 Log2ParallelMergeLevelMinus2;
    typedef PictureParameterSetContent::SliceSegmentHeaderExtensionPresentFlag SliceSegmentHeaderExtensionPresentFlag;
    typedef PictureParameterSetContent::PpsExtensionPresentFlag PpsExtensionPresentFlag;
    typedef PictureParameterSetContent::PpsRangeExtensionFlag PpsRangeExtensionFlag;
    typedef PictureParameterSetContent::PpsMultilayerExtensionFlag PpsMultilayerExtensionFlag;
    typedef PictureParameterSetContent::PpsExtension6Bits PpsExtension6Bits;
    typedef PictureParameterSetContent::PpsExtensionDataFlag PpsExtensionDataFlag;

    void toStr(std::ostream &) const;
    void onParse(StreamAccessLayer &, Decoder::State &);
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */

#endif /* HEVC_Syntax_PictureParameterSet_h */
