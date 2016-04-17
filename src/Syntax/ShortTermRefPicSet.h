#ifndef HEVC_Syntax_ShortTermRefPicSet_h
#define HEVC_Syntax_ShortTermRefPicSet_h

/* HEVC */
#include <Syntax/Syntax.h>
#include <BitArray.h>

namespace HEVC { namespace Syntax { namespace ShortTermRefPicSetContent {
/*----------------------------------------------------------------------------*/
class InterRefPicSetPredictionFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
public:
    static const auto Id = ElementId::inter_ref_pic_set_prediction_flag;

    InterRefPicSetPredictionFlag()
    {
        /* 04/2013, 7.4.8, "Short-term reference picture set semantics" */
        setValue(false);
    }

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class DeltaIdxMinus1:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
public:
    static const auto Id = ElementId::delta_idx_minus1;

    DeltaIdxMinus1()
    {
        /* 04/2013, 7.4.8, "Short-term reference picture set semantics" */
        /* inferred */
        setValue(0);
    }

    operator int () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class DeltaRpsSign:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
public:
    static const auto Id = ElementId::delta_rps_sign;

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class AbsDeltaRpsMinus1:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    /* 04/2013, 7.4.8 "Short-term reference picture set semantics" (7-46)
     * 0 <= abs_delta_rps_minus1 <= 2^15 - 1 */
public:
    static const auto Id = ElementId::abs_delta_rps_minus1;

    operator int () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class DeltaRps: public Embedded
{
private:
    int m_deltaRps;
public:
    static const auto Id = ElementId::DeltaRps;

    DeltaRps(const DeltaRpsSign &sign, const AbsDeltaRpsMinus1 &absMinus1):
        m_deltaRps((absMinus1 + 1) * (sign ? -1 : 1))
    {}

    operator int () const
    {
        return m_deltaRps;
    }
};
/*----------------------------------------------------------------------------*/
class DpbFlagList
{
protected:
    BitArray<Limits::MaxDpbSize::value> m_list;
public:
    bool operator[] (int i) const
    {
        return m_list[i];
    }
};

template <typename T>
class DpbList
{
protected:
    std::array<T, Limits::MaxDpbSize::value> m_list;
public:
    T operator[] (int i) const
    {
        return m_list[i];
    }
};
/*----------------------------------------------------------------------------*/
struct UsedByCurrPicFlag:
    public DpbFlagList,
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::used_by_curr_pic_flag;

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list[i] = getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct UseDeltaFlag:
    public DpbFlagList,
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::use_delta_flag;

    UseDeltaFlag()
    {
        m_list.fill(true);
    }

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list[i] = getValue();
    }
};
/*----------------------------------------------------------------------------*/
class NumNegativePics:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
public:
    static const auto Id = ElementId::num_negative_pics;

    operator int () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class NumPositivePics:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
public:
    static const auto Id = ElementId::num_positive_pics;

    operator int () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct DeltaPocS0Minus1:
    /* 04/2013, 7.4.8 "Short-term reference picture set semantics" */
    public DpbList<int16_t>,
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::delta_poc_s0_minus1;

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list[i] = getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct UsedByCurrPicS0Flag:
    public DpbFlagList,
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::used_by_curr_pic_s0_flag;

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list[i] = getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct DeltaPocS1Minus1:
    /* 04/2013, 7.4.8 "Short-term reference picture set semantics" */
    public DpbList<int16_t>,
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::delta_poc_s1_minus1;

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list[i] = getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct UsedByCurrPicS1Flag:
    public DpbFlagList,
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::used_by_curr_pic_s1_flag;

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list[i] = getValue();
    }
};
/*----------------------------------------------------------------------------*/
class StRpsIdx: public Embedded
{
private:
    int m_stRpsIdx;
public:
    static const auto Id = ElementId::StRpsIdx;

    StRpsIdx(int stRpsIdx): m_stRpsIdx(stRpsIdx)
    {}

    operator int () const
    {
        return m_stRpsIdx;
    }
};
/*----------------------------------------------------------------------------*/
class RefRpsIdx: public Embedded
{
private:
    int m_idx;
public:
    static const auto Id = ElementId::RefRpsIdx;

    RefRpsIdx(const StRpsIdx &idx, const DeltaIdxMinus1 &deltaMinus1):
        m_idx(int(idx) - (deltaMinus1 + 1))
    {}

    operator int () const
    {
        return m_idx;
    }
};
/*----------------------------------------------------------------------------*/
} /* namespace ShortTermRefPicSetContent */

/*----------------------------------------------------------------------------*/
class ShortTermRefPicSet:
    public EmbeddedAggregator<
        ShortTermRefPicSetContent::InterRefPicSetPredictionFlag,
        ShortTermRefPicSetContent::DeltaIdxMinus1,
        ShortTermRefPicSetContent::DeltaRpsSign,
        ShortTermRefPicSetContent::AbsDeltaRpsMinus1,
        ShortTermRefPicSetContent::DeltaRps,
        ShortTermRefPicSetContent::UsedByCurrPicFlag,
        ShortTermRefPicSetContent::UseDeltaFlag,
        ShortTermRefPicSetContent::NumNegativePics,
        ShortTermRefPicSetContent::NumPositivePics,
        ShortTermRefPicSetContent::DeltaPocS0Minus1,
        ShortTermRefPicSetContent::UsedByCurrPicS0Flag,
        ShortTermRefPicSetContent::DeltaPocS1Minus1,
        ShortTermRefPicSetContent::UsedByCurrPicS1Flag,
        ShortTermRefPicSetContent::StRpsIdx,
        ShortTermRefPicSetContent::RefRpsIdx>
{
public:
    static const auto Id = ElementId::short_term_ref_pic_set;

    typedef ShortTermRefPicSetContent::InterRefPicSetPredictionFlag InterRefPicSetPredictionFlag;
    typedef ShortTermRefPicSetContent::DeltaIdxMinus1 DeltaIdxMinus1;
    typedef ShortTermRefPicSetContent::DeltaRpsSign DeltaRpsSign;
    typedef ShortTermRefPicSetContent::AbsDeltaRpsMinus1 AbsDeltaRpsMinus1;
    typedef ShortTermRefPicSetContent::DeltaRps DeltaRps;
    typedef ShortTermRefPicSetContent::UsedByCurrPicFlag UsedByCurrPicFlag;
    typedef ShortTermRefPicSetContent::UseDeltaFlag UseDeltaFlag;
    typedef ShortTermRefPicSetContent::NumNegativePics NumNegativePics;
    typedef ShortTermRefPicSetContent::NumPositivePics NumPositivePics;
    typedef ShortTermRefPicSetContent::DeltaPocS0Minus1 DeltaPocS0Minus1;
    typedef ShortTermRefPicSetContent::UsedByCurrPicS0Flag UsedByCurrPicS0Flag;
    typedef ShortTermRefPicSetContent::DeltaPocS1Minus1 DeltaPocS1Minus1;
    typedef ShortTermRefPicSetContent::UsedByCurrPicS1Flag UsedByCurrPicS1Flag;
    typedef ShortTermRefPicSetContent::StRpsIdx StRpsIdx;
    typedef ShortTermRefPicSetContent::RefRpsIdx RefRpsIdx;

    void onParse(
            StreamAccessLayer &, Decoder::State &decoder,
            int, int, Structure::RPSP &);
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */

#endif /* HEVC_Syntax_ShortTermRefPicSet_h */
