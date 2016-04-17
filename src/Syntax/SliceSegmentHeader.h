#ifndef HEVC_Syntax_SliceSegmentHeader_h
#define HEVC_Syntax_SliceSegmentHeader_h

/* HEVC */
#include <Syntax/Syntax.h>
#include <Syntax/SequenceParameterSet.h>
#include <Syntax/PictureParameterSet.h>
#include <Syntax/ShortTermRefPicSet.h>
#include <Syntax/RefPicListModification.h>
#include <Syntax/PredWeightTable.h>
#include <Syntax/ByteAlignment.h>
#include <BitArray.h>

namespace HEVC { namespace Syntax { namespace SliceSegmentHeaderContent {
/*----------------------------------------------------------------------------*/
class FirstSliceSegmentInPicFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
public:
    static const auto Id = ElementId::first_slice_segment_in_pic_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class NoOutputOfPriorPicsFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
public:
    static const auto Id = ElementId::no_output_of_prior_pics_flag;

    NoOutputOfPriorPicsFlag()
    {
        /* may be inferred (but should not)
         * TODO: what is the meaning of above claim? */
        setValue(true);
    }

    explicit operator bool () const
    {
        /* Draft 10v18, 7.4.6.1 "General slice segment header semantics" */
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class SlicePicParameterSetId:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
public:
    static const auto Id = ElementId::slice_pic_parameter_set_id;

    operator int () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class DependentSliceSegmentFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
public:
    static const auto Id = ElementId::dependent_slice_segment_flag;

    DependentSliceSegmentFlag()
    {
        /* inferred */
        setValue(false);
    }

    explicit operator bool () const
    {
        /* Draft 10v18, 7.4.6.1 "General slice segment header semantics" */
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class SliceSegmentAddress:
    public Embedded,
    public VLD::UInt
{
public:
    static const auto Id = ElementId::slice_segment_address;

    SliceSegmentAddress()
    {
        /* inferred */
        setValue(0);
    }

    Ctb inUnits() const
    {
        /* Draft 10v18, 7.4.6.1 "General slice segment header semantics" */
        return Ctb(getValue());
    }

    operator Ctb () const
    {
        return inUnits();
    }

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
            Ctb picSizeInCtbsY)
    {
        setLengthInBits(toUnderlying(toLog2(picSizeInCtbsY)));
        getFrom(streamAccessLayer, decoder, *this);
    }
};
/*----------------------------------------------------------------------------*/
class SliceReservedFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    BitArray<1 << PictureParameterSet::NumExtraSliceHeaderBits::lengthInBits> m_list;
public:
    static const auto Id = ElementId::slice_reserved_flag;

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
class SliceType:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
public:
    static const auto Id = ElementId::slice_type;

    operator HEVC::SliceType () const
    {
        return static_cast<HEVC::SliceType>(getValue());
    }
};
/*----------------------------------------------------------------------------*/
class PicOutputFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
public:
    static const auto Id = ElementId::pic_output_flag;

    PicOutputFlag()
    {
        /* 04/2013, 7.4.7.1 "General slice segment header semantics"
         * inferred */
        setValue(true);
    }

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class ColourPlaneId:
    public Embedded,
    public VLD::FixedUInt<2, uint8_t>
{
public:
    static const auto Id = ElementId::colour_plane_id;

    operator ValueType () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class SlicePicOrderCntLsb:
    public Embedded,
    public VLD::UInt
{
public:
    static const auto Id = ElementId::slice_pic_order_cnt_lsb;

    SlicePicOrderCntLsb()
    {
        /* Draft 10v18, 8.3.3.1,
         * "General decoding process for generating unavailable
         * reference pictures"
         * inferred = PocLtFoll[i] & (MaxPicOrderCntLsb - 1) */
        setValue(0);
    }

    operator PicOrderCntLsb () const
    {
        /* Draft 10v18, 7.4.6.1 "General slice segment header semantics" */
        return PicOrderCntLsb(getValue());
    }

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
            const SequenceParameterSet::MaxPicOrderCntLsbMinus4 &value)
    {
        setLengthInBits(toUnderlying(value.inUnits()) + 4);
        getFrom(streamAccessLayer, decoder, *this);
    }
};
/*----------------------------------------------------------------------------*/
class ShortTermRefPicSetSpsFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
public:
    static const auto Id = ElementId::short_term_ref_pic_set_sps_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class ShortTermRefPicSetIdx:
    public Embedded,
    public VLD::UInt
{
public:
    static const auto Id = ElementId::short_term_ref_pic_set_idx;

    ShortTermRefPicSetIdx()
    {
        /* inferred */
        setValue(0);
    }

    operator int () const
    {
        /* Draft 10v18, 7.4.6.1 "General slice segment header semantics" */
        return getValue();
    }

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
            const SequenceParameterSet::NumShortTermRefPicSets &numShortTermRefPicSets)
    {
        setLengthInBits(log2(int(numShortTermRefPicSets)));
        getFrom(streamAccessLayer, decoder, *this);
    }
};
/*----------------------------------------------------------------------------*/
class NumLongTermSps:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
public:
    static const auto Id = ElementId::num_long_term_sps;

    NumLongTermSps()
    {
        /* inferred */
        setValue(0);
    }

    operator int () const
    {
        /* Draft 10v18, 7.4.6.1 "General slice segment header semantics" */
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class NumLongTermPics:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
public:
    static const auto Id = ElementId::num_long_term_pics;

    NumLongTermPics()
    {
        /* inferred */
        setValue(0);
    }

    operator int () const
    {
        /* Draft 10v18, 7.4.6.1 "General slice segment header semantics" */
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class LtIdxSps:
    public Embedded,
    public VLD::UInt
{
    std::array<int, Limits::NumLongTerm::num> m_idx;
public:
    static const auto Id = ElementId::lt_idx_sps;

    LtIdxSps()
    {
        // inferred
        std::fill(std::begin(m_idx), std::end(m_idx), 0);
    }

    int operator[] (int i) const
    {
        syntaxCheck(int(m_idx.size()) > i);
        return m_idx[i];
    }

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
            const SequenceParameterSet::NumLongTermRefPicsSps &numLongTermRefPicsSps,
            int i)
    {
        setLengthInBits(log2(int(numLongTermRefPicsSps)));
        getFrom(streamAccessLayer, decoder, *this);
        m_idx[i] = getValue();
    }
};
/*----------------------------------------------------------------------------*/
class PocLsbLt:
    public Embedded,
    public VLD::UInt
{
    std::array<int, Limits::NumLongTerm::num> m_pocLsb;
    int m_size;
public:
    static const auto Id = ElementId::poc_lsb_lt;

    PocLsbLt(): m_size(0)
    {
        std::fill(std::begin(m_pocLsb), std::end(m_pocLsb), 0);
    }

    int operator[] (int i) const
    {
        syntaxCheck(m_size > i);
        return m_pocLsb[i];
    }

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
            const SequenceParameterSet::MaxPicOrderCntLsbMinus4 &maxPicOrderCntLsbMinus4,
            int i)
    {
        setLengthInBits(toUnderlying(maxPicOrderCntLsbMinus4.inUnits()) + 4);
        getFrom(streamAccessLayer, decoder, *this);
        m_pocLsb[i] = getValue();
        m_size = i + 1;
    }
};
/*----------------------------------------------------------------------------*/
class UsedByCurrPicLtFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    std::array<bool, Limits::NumLongTerm::num> m_flag;
    int m_size;
public:
    static const auto Id = ElementId::used_by_curr_pic_lt_flag;

    UsedByCurrPicLtFlag(): m_size(0)
    {
        std::fill(std::begin(m_flag), std::end(m_flag), false);
    }

    bool operator[] (int i) const
    {
        syntaxCheck(m_size > i);
        return m_flag[i];
    }

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_flag[i] = getValue();
        m_size = i + 1;
    }
};
/*----------------------------------------------------------------------------*/
class DeltaPocMsbPresentFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    BitArray<Limits::NumLongTerm::num> m_list;
public:
    static const auto Id = ElementId::delta_poc_msb_present_flag;

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
class DeltaPocMsbCycleLt:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    std::array<int, Limits::NumLongTerm::num> m_pocMsb;
public:
    static const auto Id = ElementId::delta_poc_msb_cycle_lt;

    DeltaPocMsbCycleLt()
    {
        // inferred
        std::fill(std::begin(m_pocMsb), std::end(m_pocMsb), 0);
    }

    int operator[] (int i) const
    {
        syntaxCheck(int(m_pocMsb.size()) > i);
        return m_pocMsb[i];
    }

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_pocMsb[i] = getValue();
    }
};
/*----------------------------------------------------------------------------*/
class SliceTemporalMvpEnableFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
public:
    static const auto Id = ElementId::slice_temporal_mvp_enable_flag;

    SliceTemporalMvpEnableFlag()
    {
        /* inferred */
        setValue(false);
    }

    explicit operator bool () const
    {
        /* 04/2013, 7.4.7.1 "General slice segment header semantics" */
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class SliceSaoLumaFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
public:
    static const auto Id = ElementId::slice_sao_luma_flag;

    SliceSaoLumaFlag()
    {
        /* inferred */
        setValue(false);
    }

    explicit operator bool () const
    {
        /* Draft 10v18, 7.4.6.1 "General slice segment header semantics" */
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class SliceSaoChromaFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
public:
    static const auto Id = ElementId::slice_sao_chroma_flag;

    SliceSaoChromaFlag()
    {
        /* inferred */
        setValue(false);
    }

    explicit operator bool () const
    {
        /* Draft 10v18, 7.4.6.1 "General slice segment header semantics" */
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class NumRefIdxActiveOverrideFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
public:
    static const auto Id = ElementId::num_ref_idx_active_override_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class NumRefIdxL0ActiveMinus1:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
public:
    static const auto Id = ElementId::num_ref_idx_l0_active_minus1;

    NumRefIdxL0ActiveMinus1(
            const PictureParameterSet::NumRefIdxL0DefaultActiveMinus1 &numRefIdxL0DefaultActiveMinus1)
    {
        // inferred
        setValue(numRefIdxL0DefaultActiveMinus1);
    }

    operator int () const
    {
        /* Draft 10v18, 7.4.6.1 "General slice segment header semantics" */
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class NumRefIdxL1ActiveMinus1:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
public:
    static const auto Id = ElementId::num_ref_idx_l1_active_minus1;

    NumRefIdxL1ActiveMinus1(
            const PictureParameterSet::NumRefIdxL1DefaultActiveMinus1 &numRefIdxL1DefaultActiveMinus1)
    {
        // inferred
        setValue(numRefIdxL1DefaultActiveMinus1);
    }

    operator int () const
    {
        /* Draft 10v18, 7.4.6.1 "General slice segment header semantics" */
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class MvdL1ZeroFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
public:
    static const auto Id = ElementId::mvd_l1_zero_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class CabacInitFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
public:
    static const auto Id = ElementId::cabac_init_flag;

    CabacInitFlag()
    {
        /* inferred */
        setValue(false);
    }

    explicit operator bool () const
    {
        /* Draft 10v18, 7.4.6.1 "General slice segment header semantics" */
        return getValue();

    }
};
/*----------------------------------------------------------------------------*/
class CollocatedFromL0Flag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
public:
    static const auto Id = ElementId::collocated_from_l0_flag;

    CollocatedFromL0Flag()
    {
        /* inferred */
        setValue(true);
    }

    explicit operator bool () const
    {
        /* Draft 10v18, 7.4.6.1 "General slice segment header semantics" */
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class CollocatedRefIdx:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
public:
    static const auto Id = ElementId::collocated_ref_idx;

    CollocatedRefIdx()
    {
        /* WARNING: 7.4.7.1 (semantics) does not explicitly says so
         * but 7.3.6.1 (syntax) implies it" */
        setValue(0);
    }

    operator PicOrderCntVal () const
    {
        /* 04/2013, 7.4.7.1 "General slice segment header semantics" */
        return PicOrderCntVal(getValue());
    }
};
/*----------------------------------------------------------------------------*/
class FiveMinusMaxNumMergeCand:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
public:
    static const auto Id = ElementId::five_minus_max_num_merge_cand;

    operator ValueType () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class SliceQpDelta:
    public Embedded,
    public VLD::IntExpGolombCoded
{
public:
    static const auto Id = ElementId::slice_qp_delta;

    int inUnits() const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class SliceCbQpOffset:
    public Embedded,
    public VLD::IntExpGolombCoded
{
public:
    static const auto Id = ElementId::slice_cb_qp_offset;

    SliceCbQpOffset()
    {
        /* inferred */
        setValue(0);
    }

    int inUnits() const
    {
        /* Draft 10v18, 7.4.6.1 "General slice segment header semantics" */
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class SliceCrQpOffset:
    public Embedded,
    public VLD::IntExpGolombCoded
{
public:
    static const auto Id = ElementId::slice_cr_qp_offset;

    SliceCrQpOffset()
    {
        /* inferred */
        setValue(0);
    }

    int inUnits() const
    {
        /* Draft 10v18, 7.4.6.1 "General slice segment header semantics" */
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct CuChromaQpOffsetEnabledFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::cu_chroma_qp_offset_enabled_flag;

    CuChromaQpOffsetEnabledFlag()
    {
        /* 10/2014, 7.4.7.1 "General slice segment header semantics" */
        setValue(false);
    }

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class DeblockingFilterOverrideFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
public:
    static const auto Id = ElementId::deblocking_filter_override_flag;

    DeblockingFilterOverrideFlag()
    {
        /* inferred */
        setValue(false);
    }

    explicit operator bool () const
    {
        /* Draft 10v18, 7.4.6.1 "General slice segment header semantics" */
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class SliceDeblockingFilterDisabledFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
public:
    static const auto Id = ElementId::slice_deblocking_filter_disabled_flag;

    SliceDeblockingFilterDisabledFlag(
            const PictureParameterSet::PpsDeblockingFilterDisabledFlag &ppsDeblockingFilterDisabledFlag)
    {
        /* inferred */
        setValue(bool(ppsDeblockingFilterDisabledFlag));
    }

    explicit operator bool () const
    {
        /* Draft 10v18, 7.4.6.1 "General slice segment header semantics" */
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class SliceBetaOffsetDiv2:
    public Embedded,
    public VLD::IntExpGolombCoded
{
public:
    static const auto Id = ElementId::slice_beta_offset_div2;

    SliceBetaOffsetDiv2(
            const PictureParameterSet::PpsBetaOffsetDiv2 &ppsBetaOffsetDiv2)
    {
        setValue(ppsBetaOffsetDiv2);
    }

    int inUnits() const
    {
        /* Draft 10v18, 7.4.6.1 "General slice segment header semantics" */
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class SliceTcOffsetDiv2:
    public Embedded,
    public VLD::IntExpGolombCoded
{
public:
    static const auto Id = ElementId::slice_tc_offset_div2;

    SliceTcOffsetDiv2(
            const PictureParameterSet::PpsTcOffsetDiv2 &ppsTcOffsetDiv2)
    {
        setValue(ppsTcOffsetDiv2);
    }

    int inUnits() const
    {
        /* Draft 10v18, 7.4.6.1 "General slice segment header semantics" */
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class SliceLoopFilterAcrossSlicesEnabledFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
public:
    static const auto Id = ElementId::slice_loop_filter_across_slices_enabled_flag;

    SliceLoopFilterAcrossSlicesEnabledFlag(
            const PictureParameterSet::PpsLoopFilterAcrossSlicesEnabledFlag &loopFilterAcrossSlicesEnabledFlag)
    {
        setValue(bool(loopFilterAcrossSlicesEnabledFlag));
    }

    explicit operator bool () const
    {
        /* Draft 10v18, 7.4.6.1 "General slice segment header semantics" */
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class NumEntryPointOffsets:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
public:
    static const auto Id = ElementId::num_entry_point_offsets;

    NumEntryPointOffsets()
    {
        /* inferred */
        setValue(0);
    }

    operator int () const
    {
        /* Draft 10v18, 7.4.6.1 "General slice segment header semantics" */
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class OffsetLenMinus1:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
public:
    static const auto Id = ElementId::offset_len_minus1;

    operator ValueType () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class EntryPointOffsetMinus1:
    public Embedded,
    public VLD::UInt
{
    std::array<uint32_t, Limits::MaxPicHeightInMinCtb::value> m_list;
public:
    static const auto Id = ElementId::entry_point_offset_minus1;

    EntryPointOffsetMinus1(const OffsetLenMinus1 &offsetLenMinus1)
    {
        fill(m_list, uint32_t{0});
        setLengthInBits(offsetLenMinus1 + 1);
    }

    uint32_t operator[] (int i) const
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
class SliceSegmentHeaderExtensionLength:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
public:
    static const auto Id = ElementId::slice_segment_header_extension_length;

    operator int () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class SliceSegmentHeaderExtensionDataByte:
    public Embedded,
    public VLD::FixedUInt<8, uint8_t>
{
    /* 04/2013, 7.4.7.1 "General slice segment header semantics" */
    std::array<uint8_t, 256> m_list;
public:
    static const auto Id = ElementId::slice_segment_header_extension_data_byte;

    SliceSegmentHeaderExtensionDataByte()
    {
        fill(m_list, uint8_t{0});
    }

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list[i] = getValue();
    }
};
/*----------------------------------------------------------------------------*/
/* Pseudo Syntax Elements */
/*----------------------------------------------------------------------------*/
class MaxNumMergeCand:
    public Embedded
{
private:
    int m_maxNumMergeCand;
public:
    static const auto Id = ElementId::MaxNumMergeCand;

    MaxNumMergeCand(const FiveMinusMaxNumMergeCand &fiveMinusMaxNumMergeCand):
        m_maxNumMergeCand(5 - fiveMinusMaxNumMergeCand)
    {}

    operator int () const
    {
        return m_maxNumMergeCand;
    }
};
/*----------------------------------------------------------------------------*/
class SliceAddrRs:
    public Embedded
{
private:
    Ctb m_addrInRs;
public:
    static const auto Id = ElementId::SliceAddrRs;

    SliceAddrRs(Ctb addrInRs): m_addrInRs(addrInRs)
    {}

    operator Ctb () const
    {
        return m_addrInRs;
    }

    Ctb inUnits() const
    {
        return m_addrInRs;
    }
};
/*----------------------------------------------------------------------------*/
class CurrRpsIdx:
    public Embedded
{
private:
    int m_idx;
public:
    static const auto Id = ElementId::CurrRpsIdx;

    CurrRpsIdx(
            const ShortTermRefPicSetSpsFlag &shortTermRefPicSetSpsFlag,
            const ShortTermRefPicSetIdx &shortTermRefPicSetIdx,
            const SequenceParameterSet::NumShortTermRefPicSets &numShortTermRefPicSets):
        m_idx(shortTermRefPicSetSpsFlag ? shortTermRefPicSetIdx : numShortTermRefPicSets)
    {}

    operator int () const
    {
        return m_idx;
    }
};
/*----------------------------------------------------------------------------*/
class SliceQpY:
    public Embedded
{
private:
    int m_sliceQpY;
public:
    static const auto Id = ElementId::SliceQpY;

    SliceQpY(
            const PictureParameterSet::InitQpMinus26 &initQpMinus26,
            const SliceQpDelta &sliceQpDelta):
        m_sliceQpY(26 + initQpMinus26.inUnits() + sliceQpDelta.inUnits())
    {}

    operator int () const
    {
        return m_sliceQpY;
    }
};
/*----------------------------------------------------------------------------*/
class CabacInitType:
    public Embedded
{
private:
    CABAD::InitType m_initType;
public:
    static const auto Id = ElementId::CabacInitType;

    CabacInitType(const SliceType &sliceType, const CabacInitFlag &cabacInitFlag):
        m_initType(CABAD::InitType_Undefined)
    {
        /* Draft 10v24,
         * 9.2.1.1 "Initialization process for context variables", (9-7) */
        if(isI(sliceType))
        {
            m_initType = CABAD::InitType_0;
        }
        else if(isP(sliceType))
        {
            m_initType = cabacInitFlag ? CABAD::InitType_2 : CABAD::InitType_1;
        }
        else if(isB(sliceType))
        {
            m_initType = cabacInitFlag ? CABAD::InitType_1 : CABAD::InitType_2;
        }
    }

    operator CABAD::InitType () const
    {
        return m_initType;
    }
};
/*----------------------------------------------------------------------------*/
} /* namespace SliceSegmentHeaderContent */

/*----------------------------------------------------------------------------*/
class SliceSegmentHeader:
    public EmbeddedAggregator<
        SliceSegmentHeaderContent::FirstSliceSegmentInPicFlag,
        SliceSegmentHeaderContent::NoOutputOfPriorPicsFlag,
        SliceSegmentHeaderContent::SlicePicParameterSetId,
        SliceSegmentHeaderContent::DependentSliceSegmentFlag,
        SliceSegmentHeaderContent::SliceSegmentAddress,
        SliceSegmentHeaderContent::SliceReservedFlag,
        SliceSegmentHeaderContent::SliceType,
        SliceSegmentHeaderContent::PicOutputFlag,
        SliceSegmentHeaderContent::ColourPlaneId,
        SliceSegmentHeaderContent::SlicePicOrderCntLsb,
        SliceSegmentHeaderContent::ShortTermRefPicSetSpsFlag,
        SliceSegmentHeaderContent::ShortTermRefPicSetIdx,
        SliceSegmentHeaderContent::NumLongTermSps,
        SliceSegmentHeaderContent::NumLongTermPics,
        SliceSegmentHeaderContent::LtIdxSps,
        SliceSegmentHeaderContent::PocLsbLt,
        SliceSegmentHeaderContent::UsedByCurrPicLtFlag,
        SliceSegmentHeaderContent::DeltaPocMsbPresentFlag,
        SliceSegmentHeaderContent::DeltaPocMsbCycleLt,
        SliceSegmentHeaderContent::SliceTemporalMvpEnableFlag,
        SliceSegmentHeaderContent::SliceSaoLumaFlag,
        SliceSegmentHeaderContent::SliceSaoChromaFlag,
        SliceSegmentHeaderContent::NumRefIdxActiveOverrideFlag,
        SliceSegmentHeaderContent::NumRefIdxL0ActiveMinus1,
        SliceSegmentHeaderContent::NumRefIdxL1ActiveMinus1,
        SliceSegmentHeaderContent::MvdL1ZeroFlag,
        SliceSegmentHeaderContent::CabacInitFlag,
        SliceSegmentHeaderContent::CollocatedFromL0Flag,
        SliceSegmentHeaderContent::CollocatedRefIdx,
        SliceSegmentHeaderContent::FiveMinusMaxNumMergeCand,
        SliceSegmentHeaderContent::SliceQpDelta,
        SliceSegmentHeaderContent::SliceCbQpOffset,
        SliceSegmentHeaderContent::SliceCrQpOffset,
        SliceSegmentHeaderContent::CuChromaQpOffsetEnabledFlag,
        SliceSegmentHeaderContent::DeblockingFilterOverrideFlag,
        SliceSegmentHeaderContent::SliceDeblockingFilterDisabledFlag,
        SliceSegmentHeaderContent::SliceBetaOffsetDiv2,
        SliceSegmentHeaderContent::SliceTcOffsetDiv2,
        SliceSegmentHeaderContent::SliceLoopFilterAcrossSlicesEnabledFlag,
        SliceSegmentHeaderContent::NumEntryPointOffsets,
        SliceSegmentHeaderContent::OffsetLenMinus1,
        SliceSegmentHeaderContent::EntryPointOffsetMinus1,
        SliceSegmentHeaderContent::SliceSegmentHeaderExtensionLength,
        SliceSegmentHeaderContent::SliceSegmentHeaderExtensionDataByte,
        SliceSegmentHeaderContent::MaxNumMergeCand,
        SliceSegmentHeaderContent::SliceQpY,
        SliceSegmentHeaderContent::CabacInitType,
        SliceSegmentHeaderContent::SliceAddrRs,
        SliceSegmentHeaderContent::CurrRpsIdx>,
    public SubtreeAggregator<
        ShortTermRefPicSet,
        RefPicListModification,
        PredWeightTable,
        ByteAlignment>
{
public:
    static const auto Id = ElementId::slice_segment_header;

    typedef SliceSegmentHeaderContent::FirstSliceSegmentInPicFlag FirstSliceSegmentInPicFlag;
    typedef SliceSegmentHeaderContent::NoOutputOfPriorPicsFlag NoOutputOfPriorPicsFlag;
    typedef SliceSegmentHeaderContent::SlicePicParameterSetId SlicePicParameterSetId;
    typedef SliceSegmentHeaderContent::DependentSliceSegmentFlag DependentSliceSegmentFlag;
    typedef SliceSegmentHeaderContent::SliceSegmentAddress SliceSegmentAddress;
    typedef SliceSegmentHeaderContent::SliceReservedFlag SliceReservedFlag;
    typedef SliceSegmentHeaderContent::SliceType SliceType;
    typedef SliceSegmentHeaderContent::PicOutputFlag PicOutputFlag;
    typedef SliceSegmentHeaderContent::ColourPlaneId ColourPlaneId;
    typedef SliceSegmentHeaderContent::SlicePicOrderCntLsb SlicePicOrderCntLsb;
    typedef SliceSegmentHeaderContent::ShortTermRefPicSetSpsFlag ShortTermRefPicSetSpsFlag;
    typedef SliceSegmentHeaderContent::ShortTermRefPicSetIdx ShortTermRefPicSetIdx;
    typedef SliceSegmentHeaderContent::NumLongTermSps NumLongTermSps;
    typedef SliceSegmentHeaderContent::NumLongTermPics NumLongTermPics;
    typedef SliceSegmentHeaderContent::LtIdxSps LtIdxSps;
    typedef SliceSegmentHeaderContent::PocLsbLt PocLsbLt;
    typedef SliceSegmentHeaderContent::UsedByCurrPicLtFlag UsedByCurrPicLtFlag;
    typedef SliceSegmentHeaderContent::DeltaPocMsbPresentFlag DeltaPocMsbPresentFlag;
    typedef SliceSegmentHeaderContent::DeltaPocMsbCycleLt DeltaPocMsbCycleLt;
    typedef SliceSegmentHeaderContent::SliceTemporalMvpEnableFlag SliceTemporalMvpEnableFlag;
    typedef SliceSegmentHeaderContent::SliceSaoLumaFlag SliceSaoLumaFlag;
    typedef SliceSegmentHeaderContent::SliceSaoChromaFlag SliceSaoChromaFlag;
    typedef SliceSegmentHeaderContent::NumRefIdxActiveOverrideFlag NumRefIdxActiveOverrideFlag;
    typedef SliceSegmentHeaderContent::NumRefIdxL0ActiveMinus1 NumRefIdxL0ActiveMinus1;
    typedef SliceSegmentHeaderContent::NumRefIdxL1ActiveMinus1 NumRefIdxL1ActiveMinus1;
    typedef SliceSegmentHeaderContent::MvdL1ZeroFlag MvdL1ZeroFlag;
    typedef SliceSegmentHeaderContent::CabacInitFlag CabacInitFlag;
    typedef SliceSegmentHeaderContent::CollocatedFromL0Flag CollocatedFromL0Flag;
    typedef SliceSegmentHeaderContent::CollocatedRefIdx CollocatedRefIdx;
    typedef SliceSegmentHeaderContent::FiveMinusMaxNumMergeCand FiveMinusMaxNumMergeCand;
    typedef SliceSegmentHeaderContent::SliceQpDelta SliceQpDelta;
    typedef SliceSegmentHeaderContent::SliceCbQpOffset SliceCbQpOffset;
    typedef SliceSegmentHeaderContent::SliceCrQpOffset SliceCrQpOffset;
    typedef SliceSegmentHeaderContent::CuChromaQpOffsetEnabledFlag CuChromaQpOffsetEnabledFlag;
    typedef SliceSegmentHeaderContent::DeblockingFilterOverrideFlag DeblockingFilterOverrideFlag;
    typedef SliceSegmentHeaderContent::SliceDeblockingFilterDisabledFlag SliceDeblockingFilterDisabledFlag;
    typedef SliceSegmentHeaderContent::SliceBetaOffsetDiv2 SliceBetaOffsetDiv2;
    typedef SliceSegmentHeaderContent::SliceTcOffsetDiv2 SliceTcOffsetDiv2;
    typedef SliceSegmentHeaderContent::SliceLoopFilterAcrossSlicesEnabledFlag SliceLoopFilterAcrossSlicesEnabledFlag;
    typedef SliceSegmentHeaderContent::NumEntryPointOffsets NumEntryPointOffsets;
    typedef SliceSegmentHeaderContent::OffsetLenMinus1 OffsetLenMinus1;
    typedef SliceSegmentHeaderContent::EntryPointOffsetMinus1 EntryPointOffsetMinus1;
    typedef SliceSegmentHeaderContent::SliceSegmentHeaderExtensionLength SliceSegmentHeaderExtensionLength;
    typedef SliceSegmentHeaderContent::SliceSegmentHeaderExtensionDataByte SliceSegmentHeaderExtensionDataByte;
    typedef SliceSegmentHeaderContent::MaxNumMergeCand MaxNumMergeCand;
    typedef SliceSegmentHeaderContent::SliceQpY SliceQpY;
    typedef SliceSegmentHeaderContent::CabacInitType CabacInitType;
    typedef SliceSegmentHeaderContent::SliceAddrRs SliceAddrRs;
    typedef SliceSegmentHeaderContent::CurrRpsIdx CurrRpsIdx;

public:
    bool isDependent() const;
    bool isIndependent() const
    {
        return !isDependent();
    }

    void toStr(std::ostream &) const;
    void onParse(
            StreamAccessLayer &, Decoder::State &decoder,
            NalUnitType);
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */

#endif /* HEVC_Syntax_SliceSegmentHeader_h */
