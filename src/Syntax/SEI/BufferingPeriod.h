#ifndef HEVC_Syntax_BufferingPeriod_h
#define HEVC_Syntax_BufferingPeriod_h

#include <Syntax/Syntax.h>
#include <Syntax/HrdParameters.h>

namespace HEVC { namespace Syntax { namespace SEI { namespace BufferingPeriodContent {
/*----------------------------------------------------------------------------*/
struct BpSeqParameterSetId:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::sei_bp_seq_parameter_set_id;

    int inUnits() const
    {
        return getValue();
    }
};

struct IrapCpbParamsPresentFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::sei_irap_cpb_params_present_flag;

    explicit
    operator bool () const
    {
        return getValue();
    }
};

struct CpbDelayOffset:
    public Embedded,
    public VLD::UInt
{
    static const auto Id = ElementId::sei_cpb_delay_offset;

    CpbDelayOffset(const HrdParameters::AuCpbRemovalDelayLengthMinus1 &lengthMinus1):
        VLD::UInt{lengthMinus1 + 1}
    {}
};

struct DpbDelayOffset:
    public Embedded,
    public VLD::UInt
{
    static const auto Id = ElementId::sei_dpb_delay_offset;

    DpbDelayOffset(const HrdParameters::DpbOutputDelayLengthMinus1 &lengthMinus1):
        VLD::UInt{lengthMinus1 + 1}
    {}
};

struct ConcatenationFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::sei_concatenation_flag;
};

struct AuCpbRemovalDelayDeltaMinus1:
    public Embedded,
    public VLD::UInt
{
    static const auto Id = ElementId::sei_au_cpb_removal_delay_delta_minus1;

    AuCpbRemovalDelayDeltaMinus1(const HrdParameters::AuCpbRemovalDelayLengthMinus1 &lengthMinus1):
        VLD::UInt{lengthMinus1 + 1}
    {}
};

struct CpbValueList
{
protected:
    std::array<int, Limits::CpbCnt::num> m_flag;
public:
    int operator[] (int i) const
    {
        return m_flag[i];
    }
};

struct NalInitialCpbRemovalDelay:
    public CpbValueList,
    public Embedded,
    public VLD::UInt
{
    static const auto Id = ElementId::sei_nal_initial_cpb_removal_delay;

    NalInitialCpbRemovalDelay(const HrdParameters::InitialCpbRemovalDelayLengthMinus1 &lengthMinus1):
        VLD::UInt{lengthMinus1 + 1}
    {}

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_flag[i] = getValue();
    }
};

struct NalInitialCpbRemovalOffset:
    public CpbValueList,
    public Embedded,
    public VLD::UInt
{
    static const auto Id = ElementId::sei_nal_initial_cpb_removal_offset;

    NalInitialCpbRemovalOffset(const HrdParameters::InitialCpbRemovalDelayLengthMinus1 &lengthMinus1):
        VLD::UInt{lengthMinus1 + 1}
    {}

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_flag[i] = getValue();
    }
};

struct NalInitialAltCpbRemovalDelay:
    public CpbValueList,
    public Embedded,
    public VLD::UInt
{
    static const auto Id = ElementId::sei_nal_initial_alt_cpb_removal_delay;

    NalInitialAltCpbRemovalDelay(const HrdParameters::InitialCpbRemovalDelayLengthMinus1 &lengthMinus1):
        VLD::UInt{lengthMinus1 + 1}
    {}

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_flag[i] = getValue();
    }
};

struct NalInitialAltCpbRemovalOffset:
    public CpbValueList,
    public Embedded,
    public VLD::UInt
{
    static const auto Id = ElementId::sei_nal_initial_alt_cpb_removal_offset;

    NalInitialAltCpbRemovalOffset(const HrdParameters::InitialCpbRemovalDelayLengthMinus1 &lengthMinus1):
        VLD::UInt{lengthMinus1 + 1}
    {}

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_flag[i] = getValue();
    }
};

struct VclInitialCpbRemovalDelay:
    public CpbValueList,
    public Embedded,
    public VLD::UInt
{
    static const auto Id = ElementId::sei_vcl_initial_cpb_removal_delay;

    VclInitialCpbRemovalDelay(const HrdParameters::InitialCpbRemovalDelayLengthMinus1 &lengthMinus1):
        VLD::UInt{lengthMinus1 + 1}
    {}

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_flag[i] = getValue();
    }
};

struct VclInitialCpbRemovalOffset:
    public CpbValueList,
    public Embedded,
    public VLD::UInt
{
    static const auto Id = ElementId::sei_vcl_initial_cpb_removal_offset;

    VclInitialCpbRemovalOffset(const HrdParameters::InitialCpbRemovalDelayLengthMinus1 &lengthMinus1):
        VLD::UInt{lengthMinus1 + 1}
    {}

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_flag[i] = getValue();
    }
};

struct VclInitialAltCpbRemovalDelay:
    public CpbValueList,
    public Embedded,
    public VLD::UInt
{
    static const auto Id = ElementId::sei_vcl_initial_alt_cpb_removal_delay;

    VclInitialAltCpbRemovalDelay(const HrdParameters::InitialCpbRemovalDelayLengthMinus1 &lengthMinus1):
        VLD::UInt{lengthMinus1 + 1}
    {}

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_flag[i] = getValue();
    }
};

struct VclInitialAltCpbRemovalOffset:
    public CpbValueList,
    public Embedded,
    public VLD::UInt
{
    static const auto Id = ElementId::sei_vcl_initial_alt_cpb_removal_offset;

    VclInitialAltCpbRemovalOffset(const HrdParameters::InitialCpbRemovalDelayLengthMinus1 &lengthMinus1):
        VLD::UInt{lengthMinus1 + 1}
    {}

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_flag[i] = getValue();
    }
};
/*----------------------------------------------------------------------------*/
} /* BufferingPeriodContent */

struct BufferingPeriod:
    public EmbeddedAggregator<
        BufferingPeriodContent::BpSeqParameterSetId,
        BufferingPeriodContent::IrapCpbParamsPresentFlag,
        BufferingPeriodContent::CpbDelayOffset,
        BufferingPeriodContent::DpbDelayOffset,
        BufferingPeriodContent::ConcatenationFlag,
        BufferingPeriodContent::AuCpbRemovalDelayDeltaMinus1,
        BufferingPeriodContent::NalInitialCpbRemovalDelay,
        BufferingPeriodContent::NalInitialCpbRemovalOffset,
        BufferingPeriodContent::NalInitialAltCpbRemovalDelay,
        BufferingPeriodContent::NalInitialAltCpbRemovalOffset,
        BufferingPeriodContent::VclInitialCpbRemovalDelay,
        BufferingPeriodContent::VclInitialCpbRemovalOffset,
        BufferingPeriodContent::VclInitialAltCpbRemovalOffset,
        BufferingPeriodContent::VclInitialAltCpbRemovalOffset>
{
    static const auto Id = ElementId::sei_buffering_period;

    typedef BufferingPeriodContent::BpSeqParameterSetId BpSeqParameterSetId;
    typedef BufferingPeriodContent::IrapCpbParamsPresentFlag IrapCpbParamsPresentFlag;
    typedef BufferingPeriodContent::CpbDelayOffset CpbDelayOffset;
    typedef BufferingPeriodContent::DpbDelayOffset DpbDelayOffset;
    typedef BufferingPeriodContent::ConcatenationFlag ConcatenationFlag;
    typedef BufferingPeriodContent::AuCpbRemovalDelayDeltaMinus1 AuCpbRemovalDelayDeltaMinus1;
    typedef BufferingPeriodContent::NalInitialCpbRemovalDelay NalInitialCpbRemovalDelay;
    typedef BufferingPeriodContent::NalInitialCpbRemovalOffset NalInitialCpbRemovalOffset;
    typedef BufferingPeriodContent::NalInitialAltCpbRemovalDelay NalInitialAltCpbRemovalDelay;
    typedef BufferingPeriodContent::NalInitialAltCpbRemovalOffset NalInitialAltCpbRemovalOffset;
    typedef BufferingPeriodContent::VclInitialCpbRemovalDelay VclInitialCpbRemovalDelay;
    typedef BufferingPeriodContent::VclInitialCpbRemovalOffset VclInitialCpbRemovalOffset;
    typedef BufferingPeriodContent::VclInitialAltCpbRemovalOffset VclInitialAltCpbRemovalDelay;
    typedef BufferingPeriodContent::VclInitialAltCpbRemovalOffset VclInitialAltCpbRemovalOffset;

    void onParse(
            StreamAccessLayer &, Decoder::State &decoder,
            const HrdParameters &, int);
};
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Syntax::SEI */

#endif /* HEVC_Syntax_BufferingPeriod_h */
