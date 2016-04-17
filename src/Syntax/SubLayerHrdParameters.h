#ifndef HEVC_Syntax_SubLayerHrdParameters_h
#define HEVC_Syntax_SubLayerHrdParameters_h

#include <Syntax/Syntax.h>
#include <BitArray.h>

namespace HEVC { namespace Syntax { namespace SubLayerHrdParametersContent {
/*----------------------------------------------------------------------------*/
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

struct BitRateValueMinus1:
    public CpbValueList,
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::bit_rate_value_minus1;

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_flag[i] = getValue();
    }
};

struct CpbSizeValueMinus1:
    public CpbValueList,
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::cpb_size_value_minus1;

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_flag[i] = getValue();
    }
};

struct CpbSizeDuValueMinus1:
    public CpbValueList,
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::cpb_size_du_value_minus1;

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_flag[i] = getValue();
    }
};

struct BitRateDuValueMinus1:
    public CpbValueList,
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::bit_rate_du_value_minus1;

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_flag[i] = getValue();
    }
};

class CbrFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    BitArray<Limits::CpbCnt::num> m_flag;
public:
    static const auto Id = ElementId::cbr_flag;

    bool operator[] (int i) const
    {
        return m_flag[i];
    }

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_flag[i] = getValue();
    }
};
/*----------------------------------------------------------------------------*/
} /* SubLayerHrdParameters */

struct SubLayerHrdParameters:
    public EmbeddedAggregator<
        SubLayerHrdParametersContent::BitRateValueMinus1,
        SubLayerHrdParametersContent::CpbSizeValueMinus1,
        SubLayerHrdParametersContent::CpbSizeDuValueMinus1,
        SubLayerHrdParametersContent::BitRateDuValueMinus1,
        SubLayerHrdParametersContent::CbrFlag>
{
    const int subLayerId;

    typedef SubLayerHrdParametersContent::BitRateValueMinus1 BitRateValueMinus1;
    typedef SubLayerHrdParametersContent::CpbSizeValueMinus1 CpbSizeValueMinus1;
    typedef SubLayerHrdParametersContent::CpbSizeDuValueMinus1 CpbSizeDuValueMinus1;
    typedef SubLayerHrdParametersContent::BitRateDuValueMinus1 BitRateDuValueMinus1;
    typedef SubLayerHrdParametersContent::CbrFlag CbrFlag;

    static const auto Id = ElementId::sub_layer_hrd_parameters;

    SubLayerHrdParameters(int id): subLayerId(id)
    {}

    void onParse(StreamAccessLayer &, Decoder::State &decoder, int, bool);
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */

#endif /* HEVC_Syntax_SubLayerHrdParameters_h */
