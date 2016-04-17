#ifndef HEVC_Syntax_VideoParameterSet_h
#define HEVC_Syntax_VideoParameterSet_h

/* HEVC */
#include <Syntax/Syntax.h>
#include <Syntax/ProfileTierLevel.h>
#include <Syntax/HrdParameters.h>
#include <Syntax/RbspTrailingBits.h>
#include <BitArray.h>

namespace HEVC { namespace Syntax { namespace VideoParameterSetContent {
/*----------------------------------------------------------------------------*/
struct VpsVideoParameterSetId:
    public Embedded,
    public VLD::FixedUInt<4, uint8_t>
{
    static const auto Id = ElementId::vps_video_parameter_set_id;

    operator int () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct VpsBaseLayerInternalFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::vps_base_layer_internal_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct VpsBaseLayerAvailableFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::vps_base_layer_available_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct VpsMaxLayersMinus1:
    public Embedded,
    public VLD::FixedUInt<6, uint8_t>
{
    static const auto Id = ElementId::vps_max_layers_minus1;
};
/*----------------------------------------------------------------------------*/
struct VpsMaxSubLayersMinus1:
    public Embedded,
    public VLD::FixedUInt<3, uint8_t>
{
    static const auto Id = ElementId::vps_max_sub_layers_minus1;

    operator int () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct VpsTemporalIdNestingFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::vps_temporal_id_nesting_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct VpsReserved0xffff16bits:
    public Embedded,
    public VLD::FixedUInt<16, uint16_t>
{
    static const auto Id = ElementId::vps_reserved_0xffff_16bits;
};
/*----------------------------------------------------------------------------*/
struct VpsSubLayerOrderingInfoPresentFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::vps_sub_layer_ordering_info_present_flag;

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
struct VpsMaxDecPicBufferingMinus1:
    public SubLayerList<int>,
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::vps_max_dec_pic_buffering_minus1;

    VpsMaxDecPicBufferingMinus1()
    {
        fill(m_list, -1);
    }

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
            const VpsSubLayerOrderingInfoPresentFlag &flag, int i)
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
struct VpsMaxNumReorderPics:
    public SubLayerList<int>,
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::vps_max_num_reorder_pics;

    VpsMaxNumReorderPics()
    {
        fill(m_list, -1);
    }

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
            const VpsSubLayerOrderingInfoPresentFlag &flag, int i)
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
struct VpsMaxLatencyIncreasePlus1:
    public SubLayerList<uint32_t>,
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::vps_max_latency_increase_plus1;

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
            const VpsSubLayerOrderingInfoPresentFlag &flag, int i)
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
struct VpsMaxLayerId:
    public Embedded,
    public VLD::FixedUInt<6, uint8_t>
{
    static const auto Id = ElementId::vps_max_layer_id;

    operator int () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct VpsNumLayerSetsMinus1:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::vps_num_layer_sets_minus1;

    operator int () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct LayerIdIncludedFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::layer_id_included_flag;
private:
    BitArray<Limits::VpsNumLayerSets::num * 1 /* range of nuh_layer_id */> m_list;
public:
    bool operator[] (std::pair<int, int> i) const
    {
        return m_list[std::get<0>(i) * 1 + std::get<1>(i)];
    }

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i, int j)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list[i * 1 + j] = getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct VpsTimingInfoPresentFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::vps_timing_info_present_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct VpsNumUnitsInTick:
    public Embedded,
    public VLD::FixedUInt<32, uint32_t>
{
    static const auto Id = ElementId::vps_num_units_in_tick;

    operator uint32_t () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct VpsTimeScale:
    public Embedded,
    public VLD::FixedUInt<32, uint32_t>
{
    static const auto Id = ElementId::vps_time_scale;
};
/*----------------------------------------------------------------------------*/
struct VpsPocProportionalToTimingFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::vps_poc_proportional_to_timing_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct VpsNumTicksPocDiffOneMinus1:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::vps_num_ticks_poc_diff_one_minus1;
};
/*----------------------------------------------------------------------------*/
struct VpsNumHrdParameters:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::vps_num_hrd_parameters;

    operator int () const
    {
        syntaxCheck(1024 >= getValue());
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class HrdLayerSetIdx:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    std::array<int16_t, Limits::VpsNumHrdParameters::num> m_idx;
public:
    static const auto Id = ElementId::hrd_layer_set_idx;

    HrdLayerSetIdx()
    {
        fill(m_idx, int16_t{0});
    }

    int operator[] (int i) const
    {
        return m_idx[i];
    }

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        syntaxCheck(1023 >= getValue());
        m_idx[i] = getValue();
    }
};
/*----------------------------------------------------------------------------*/
class CprmsPresentFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    BitArray<Limits::VpsNumHrdParameters::num> m_flags;
public:
    static const auto Id = ElementId::cprms_present_flag;

    CprmsPresentFlag()
    {
        /* 04/2013, 7.4.3.1 "Video parameter set RBSP semantics"
         * inferred */
        m_flags[0] = 1;
    }

    bool operator[] (int i) const
    {
        return m_flags[i];
    }

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_flags[i] = getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct VpsExtensionFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::vps_extension_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class VpsExtensionDataFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    VLA<bool> m_list;
public:
    static const auto Id = ElementId::vps_extension_data_flag;

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list.emplaceBack(getValue());
    }
};
/*----------------------------------------------------------------------------*/
} /* namespace VideoParameterSetContent */
/*----------------------------------------------------------------------------*/
class VideoParameterSet:
    public EmbeddedAggregator<
        VideoParameterSetContent::VpsVideoParameterSetId,
        VideoParameterSetContent::VpsBaseLayerInternalFlag,
        VideoParameterSetContent::VpsBaseLayerAvailableFlag,
        VideoParameterSetContent::VpsMaxLayersMinus1,
        VideoParameterSetContent::VpsMaxSubLayersMinus1,
        VideoParameterSetContent::VpsTemporalIdNestingFlag,
        VideoParameterSetContent::VpsReserved0xffff16bits,
        VideoParameterSetContent::VpsSubLayerOrderingInfoPresentFlag,
        VideoParameterSetContent::VpsMaxDecPicBufferingMinus1,
        VideoParameterSetContent::VpsMaxNumReorderPics,
        VideoParameterSetContent::VpsMaxLatencyIncreasePlus1,
        VideoParameterSetContent::VpsMaxLayerId,
        VideoParameterSetContent::VpsNumLayerSetsMinus1,
        VideoParameterSetContent::LayerIdIncludedFlag,
        VideoParameterSetContent::VpsTimingInfoPresentFlag,
        VideoParameterSetContent::VpsNumUnitsInTick,
        VideoParameterSetContent::VpsTimeScale,
        VideoParameterSetContent::VpsPocProportionalToTimingFlag,
        VideoParameterSetContent::VpsNumTicksPocDiffOneMinus1,
        VideoParameterSetContent::VpsNumHrdParameters,
        VideoParameterSetContent::HrdLayerSetIdx,
        VideoParameterSetContent::CprmsPresentFlag,
        VideoParameterSetContent::VpsExtensionFlag,
        VideoParameterSetContent::VpsExtensionDataFlag>,
    public SubtreeAggregator<
        ProfileTierLevel,
        RbspTrailingBits>,
    public SubtreeListAggregator<
        HrdParameters>
{
public:
    static const auto Id = ElementId::video_parameter_set_rbsp;

    typedef VideoParameterSetContent::VpsVideoParameterSetId VpsVideoParameterSetId;
    typedef VideoParameterSetContent::VpsBaseLayerInternalFlag VpsBaseLayerInternalFlag;
    typedef VideoParameterSetContent::VpsBaseLayerAvailableFlag VpsBaseLayerAvailableFlag;
    typedef VideoParameterSetContent::VpsMaxLayersMinus1 VpsMaxLayersMinus1;
    typedef VideoParameterSetContent::VpsMaxSubLayersMinus1 VpsMaxSubLayersMinus1;
    typedef VideoParameterSetContent::VpsTemporalIdNestingFlag VpsTemporalIdNestingFlag;
    typedef VideoParameterSetContent::VpsReserved0xffff16bits VpsReserved0xffff16bits;
    typedef VideoParameterSetContent::VpsSubLayerOrderingInfoPresentFlag VpsSubLayerOrderingInfoPresentFlag;
    typedef VideoParameterSetContent::VpsMaxDecPicBufferingMinus1 VpsMaxDecPicBufferingMinus1;
    typedef VideoParameterSetContent::VpsMaxNumReorderPics VpsMaxNumReorderPics;
    typedef VideoParameterSetContent::VpsMaxLatencyIncreasePlus1 VpsMaxLatencyIncreasePlus1;
    typedef VideoParameterSetContent::VpsMaxLayerId VpsMaxLayerId;
    typedef VideoParameterSetContent::VpsNumLayerSetsMinus1 VpsNumLayerSetsMinus1;
    typedef VideoParameterSetContent::LayerIdIncludedFlag LayerIdIncludedFlag;
    typedef VideoParameterSetContent::VpsTimingInfoPresentFlag VpsTimingInfoPresentFlag;
    typedef VideoParameterSetContent::VpsNumUnitsInTick VpsNumUnitsInTick;
    typedef VideoParameterSetContent::VpsTimeScale VpsTimeScale;
    typedef VideoParameterSetContent::VpsPocProportionalToTimingFlag VpsPocProportionalToTimingFlag;
    typedef VideoParameterSetContent::VpsNumTicksPocDiffOneMinus1 VpsNumTicksPocDiffOneMinus1;
    typedef VideoParameterSetContent::VpsNumHrdParameters VpsNumHrdParameters;
    typedef VideoParameterSetContent::HrdLayerSetIdx HrdLayerSetIdx;
    typedef VideoParameterSetContent::CprmsPresentFlag CprmsPresentFlag;
    typedef VideoParameterSetContent::VpsExtensionFlag VpsExtensionFlag;
    typedef VideoParameterSetContent::VpsExtensionDataFlag VpsExtensionDataFlag;

    void onParse(StreamAccessLayer &, Decoder::State &);
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */

#endif /* HEVC_Syntax_VideoParameterSet_h */
