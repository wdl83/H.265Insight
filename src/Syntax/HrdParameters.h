#ifndef HEVC_Syntax_HrdParameters_h
#define HEVC_Syntax_HrdParameters_h

/* HEVC */
#include <Syntax/Syntax.h>
#include <Syntax/SubLayerHrdParameters.h>

namespace HEVC { namespace Syntax { namespace HrdParametersContent {
/*----------------------------------------------------------------------------*/
struct NalHrdParametersPresentFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::nal_hrd_parameters_present_flag;

    NalHrdParametersPresentFlag()
    {
        setValue(false);
    }

    explicit operator bool () const
    {
        return getValue();
    }
};

struct VclHrdParametersPresentFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::vcl_hrd_parameters_present_flag;

    VclHrdParametersPresentFlag()
    {
        setValue(false);
    }

    explicit operator bool () const
    {
        return getValue();
    }
};

struct SubPicHrdParamsPresentFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::sub_pic_hrd_params_present_flag;

    SubPicHrdParamsPresentFlag()
    {
        setValue(0);
    }

    explicit operator bool () const
    {
        return getValue();
    }
};

struct TickDivisorDiv2:
    public Embedded,
    public VLD::FixedUInt<8, uint8_t>
{
    static const auto Id = ElementId::tick_divisor_minus2;
};

struct DuCpbRemovalDelayIncrementLengthMinus1:
    public Embedded,
    public VLD::FixedUInt<5, uint8_t>
{
    static const auto Id = ElementId::du_cpb_removal_delay_increment_length_minus1;
};

struct SubPicCpbParamsInPicTimingSeiFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::sub_pic_cpb_params_in_pic_timing_sei_flag;
};

struct DpbOutputDelayDuLengthMinus1:
    public Embedded,
    public VLD::FixedUInt<5, uint8_t>
{
    static const auto Id = ElementId::dpb_output_delay_du_length_minus1;
};

struct BitRateScale:
    public Embedded,
    public VLD::FixedUInt<4, uint8_t>
{
    static const auto Id = ElementId::bit_rate_scale;
};

struct CpbSizeScale:
    public Embedded,
    public VLD::FixedUInt<4, uint8_t>
{
    static const auto Id = ElementId::cpb_size_scale;
};

struct CpbSizeDuScale:
    public Embedded,
    public VLD::FixedUInt<4, uint8_t>
{
    static const auto Id = ElementId::cpb_size_du_scale;
};

struct InitialCpbRemovalDelayLengthMinus1:
    public Embedded,
    public VLD::FixedUInt<5, uint8_t>
{
    static const auto Id = ElementId::initial_cpb_removal_delay_length_minus1;

    InitialCpbRemovalDelayLengthMinus1()
    {
        setValue(23);
    }

    operator int () const
    {
        return getValue();
    }
};

struct AuCpbRemovalDelayLengthMinus1:
    public Embedded,
    public VLD::FixedUInt<5, uint8_t>
{
    static const auto Id = ElementId::au_cpb_removal_delay_length_minus1;

    AuCpbRemovalDelayLengthMinus1()
    {
        setValue(23);
    }

    operator int () const
    {
        return getValue();
    }
};

struct DpbOutputDelayLengthMinus1:
    public Embedded,
    public VLD::FixedUInt<5, uint8_t>
{
    static const auto Id = ElementId::dpb_output_delay_length_minus1;

    DpbOutputDelayLengthMinus1()
    {
        setValue(23);
    }

    operator int () const
    {
        return getValue();
    }
};

struct SubLayerFlagList
{
protected:
    BitArray<Limits::VpsMaxSubLayers::num> m_flag;
public:
    SubLayerFlagList(bool value = false):
        m_flag(value)
    {}

    bool operator[] (int i) const
    {
        return m_flag[i];
    }
};

struct FixedPicRateGeneralFlag:
    public SubLayerFlagList,
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::fixed_pic_rate_general_flag;

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_flag[i] = getValue();
    }
};

struct FixedPicRateWithinCvsFlag:
    public SubLayerFlagList,
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::fixed_pic_rate_within_cvs_flag;

    FixedPicRateWithinCvsFlag():
        SubLayerFlagList(true)
    {}

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_flag[i] = getValue();
    }
};

class ElementalDurationInTcMinus1:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    std::array<int, Limits::VpsMaxSubLayers::num> m_flag;
public:
    static const auto Id = ElementId::elemental_duration_in_tc_minus1;

    ElementalDurationInTcMinus1()
    {
        fill(m_flag, 0);
    }

    int operator[] (int i) const
    {
        return m_flag[i];
    }

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        syntaxCheck(2047 >= getValue());
        m_flag[i] = getValue();
    }
};

struct LowDelayHrdFlag:
    public SubLayerFlagList,
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::low_delay_hrd_flag;

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_flag[i] = getValue();
    }
};

class CpbCntMinus1:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    std::array<int, Limits::VpsMaxSubLayers::num> m_flag;
public:
    static const auto Id = ElementId::cpb_cnt_minus1;

    CpbCntMinus1()
    {
        fill(m_flag, 0);
    }

    int operator[] (int i) const
    {
        return m_flag[i];
    }

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        syntaxCheck(31 >= getValue());
        m_flag[i] = getValue();
    }
};
/*----------------------------------------------------------------------------*/
} /* HrdParametersContent */

class HrdParameters:
    public EmbeddedAggregator<
        HrdParametersContent::NalHrdParametersPresentFlag,
        HrdParametersContent::VclHrdParametersPresentFlag,
        HrdParametersContent::SubPicHrdParamsPresentFlag,
        HrdParametersContent::TickDivisorDiv2,
        HrdParametersContent::DuCpbRemovalDelayIncrementLengthMinus1,
        HrdParametersContent::SubPicCpbParamsInPicTimingSeiFlag,
        HrdParametersContent::DpbOutputDelayDuLengthMinus1,
        HrdParametersContent::BitRateScale,
        HrdParametersContent::CpbSizeScale,
        HrdParametersContent::CpbSizeDuScale,
        HrdParametersContent::InitialCpbRemovalDelayLengthMinus1,
        HrdParametersContent::AuCpbRemovalDelayLengthMinus1,
        HrdParametersContent::DpbOutputDelayLengthMinus1,
        HrdParametersContent::FixedPicRateGeneralFlag,
        HrdParametersContent::FixedPicRateWithinCvsFlag,
        HrdParametersContent::ElementalDurationInTcMinus1,
        HrdParametersContent::LowDelayHrdFlag,
        HrdParametersContent::CpbCntMinus1>,
    public SubtreeListAggregator<
        SubLayerHrdParameters>
{
public:
    static const auto Id = ElementId::hrd_parameters;

    typedef HrdParametersContent::NalHrdParametersPresentFlag NalHrdParametersPresentFlag;
    typedef HrdParametersContent::VclHrdParametersPresentFlag VclHrdParametersPresentFlag;
    typedef HrdParametersContent::SubPicHrdParamsPresentFlag SubPicHrdParamsPresentFlag;
    typedef HrdParametersContent::TickDivisorDiv2 TickDivisorDiv2;
    typedef HrdParametersContent::DuCpbRemovalDelayIncrementLengthMinus1 DuCpbRemovalDelayIncrementLengthMinus1;
    typedef HrdParametersContent::SubPicCpbParamsInPicTimingSeiFlag SubPicCpbParamsInPicTimingSeiFlag;
    typedef HrdParametersContent::DpbOutputDelayDuLengthMinus1 DpbOutputDelayDuLengthMinus1;
    typedef HrdParametersContent::BitRateScale BitRateScale;
    typedef HrdParametersContent::CpbSizeScale CpbSizeScale;
    typedef HrdParametersContent::CpbSizeDuScale CpbSizeDuScale;
    typedef HrdParametersContent::InitialCpbRemovalDelayLengthMinus1 InitialCpbRemovalDelayLengthMinus1;
    typedef HrdParametersContent::AuCpbRemovalDelayLengthMinus1 AuCpbRemovalDelayLengthMinus1;
    typedef HrdParametersContent::DpbOutputDelayLengthMinus1 DpbOutputDelayLengthMinus1;
    typedef HrdParametersContent::FixedPicRateGeneralFlag FixedPicRateGeneralFlag;
    typedef HrdParametersContent::FixedPicRateWithinCvsFlag FixedPicRateWithinCvsFlag;
    typedef HrdParametersContent::ElementalDurationInTcMinus1 ElementalDurationInTcMinus1;
    typedef HrdParametersContent::LowDelayHrdFlag LowDelayHrdFlag;
    typedef HrdParametersContent::CpbCntMinus1 CpbCntMinus1;

    /* 04/2013, E.3.2 "HRD parameters semantics" */
    typedef NalHrdParametersPresentFlag NalHrdBpPresentFlag;
    typedef VclHrdParametersPresentFlag VclHrdBpPresentFlag;

    void onParse(StreamAccessLayer &, Decoder::State &decoder, bool, int);
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */

#endif /* HEVC_Syntax_HrdParameters_h */
