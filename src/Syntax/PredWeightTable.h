#ifndef HEVC_Syntax_PredWeightTable_h
#define HEVC_Syntax_PredWeightTable_h

/* HEVC */
#include <Syntax/Syntax.h>
#include <BitArray.h>

namespace HEVC { namespace Syntax { namespace PredWeightTableContent {
/*----------------------------------------------------------------------------*/
class LumaLog2WeightDenom:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
public:
    static const auto Id = ElementId::luma_log2_weight_denom;

    Log2 inUnits() const
    {
        /* 04/2013, 7.4.7.3 "Weighted prediction parameters semantics" */
        syntaxCheck(7 >= getValue());
        return Log2(getValue());
    }
};
/*----------------------------------------------------------------------------*/
class DeltaChromaLog2WeightDenom:
    public Embedded,
    public VLD::IntExpGolombCoded
{
public:
    static const auto Id = ElementId::delta_chroma_log2_weight_denom;

    Log2 inUnits() const
    {
        return Log2(getValue());
    }
};
/*----------------------------------------------------------------------------*/
class ChromaLog2WeightDenom: public Embedded
{
private:
    Log2 m_denom;
public:
    static const auto Id = ElementId::ChromaLog2WeightDenom;

    ChromaLog2WeightDenom(
            const LumaLog2WeightDenom &denom,
            const DeltaChromaLog2WeightDenom &delta):
        m_denom{Log2(denom.inUnits() + delta.inUnits())}
    {
        syntaxCheck(0_log2 <= m_denom);
        syntaxCheck(7_log2 >= m_denom);
    }

    Log2 inUnits() const
    {
        return m_denom;
    }
};
/*----------------------------------------------------------------------------*/
class RefListFlagList
{
protected:
    BitArray<Limits::RefListIdx::num> m_list;
public:
    bool operator[] (int i) const
    {
        return m_list[i];
    }
};
/*----------------------------------------------------------------------------*/
struct LumaWeightL0Flag:
    public RefListFlagList,
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::luma_weight_l0_flag;

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list[i] = getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct ChromaWeightL0Flag:
    public RefListFlagList,
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::chroma_weight_l0_flag;

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list[i] = getValue();
    }
};
/*----------------------------------------------------------------------------*/
class DeltaLumaWeightL0:
    public Embedded,
    public VLD::IntExpGolombCoded
{
    std::array<int, Limits::RefListIdx::max + 1> m_delta;
public:
    static const auto Id = ElementId::delta_luma_weight_l0;

    DeltaLumaWeightL0()
    {
        /* 04/2013, 7.4.7.3 "Weighted prediction parameters semantics"
         * inferred */
        fill(m_delta, 0);
    }

    int operator[](int i) const
    {
        return m_delta[i];
    }

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
            int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_delta[i] = getValue();
        syntaxCheck(-128 <= m_delta[i]);
        syntaxCheck(127 >= m_delta[i]);
    }
};
/*----------------------------------------------------------------------------*/
class LumaOffsetL0:
    public Embedded,
    public VLD::IntExpGolombCoded
{
    std::array<int, Limits::RefListIdx::max + 1> m_offset;
public:
    static const auto Id = ElementId::luma_offset_l0;

    LumaOffsetL0()
    {
        /* 04/2013, 7.4.7.3 "Weighted prediction parameters semantics"
         * inferred */
        fill(m_offset, 0);
    }

    int operator[] (int i) const
    {
        return m_offset[i];
    }

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
            int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_offset[i] = getValue();
        syntaxCheck(-128 <= m_offset[i]);
        syntaxCheck(127 >= m_offset[i]);
    }
};
/*----------------------------------------------------------------------------*/
class DeltaChromaWeightL0:
    public Embedded,
    public VLD::IntExpGolombCoded
{
    std::array<
        std::array<int, Limits::RefListIdx::max + 1>,
        EnumRange<Chroma>::length()> m_delta;
public:
    static const auto Id = ElementId::delta_chroma_weight_l0;

    DeltaChromaWeightL0()
    {
        fill(m_delta, 0);
    }

    int operator[] (Tuple<Chroma, int> i) const
    {
        return m_delta[int(i.get<Chroma>())][i.get<int>()];
    }

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
            Chroma chroma, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_delta[int(chroma)][i] = getValue();
        syntaxCheck(-128 <= m_delta[int(chroma)][i]);
        syntaxCheck(127 >= m_delta[int(chroma)][i]);
    }
};
/*----------------------------------------------------------------------------*/
class DeltaChromaOffsetL0:
    public Embedded,
    public VLD::IntExpGolombCoded
{
    std::array<
        std::array<int, Limits::RefListIdx::max + 1>,
        EnumRange<Chroma>::length()> m_offset;
public:
    static const auto Id = ElementId::delta_chroma_offset_l0;

    DeltaChromaOffsetL0()
    {
        fill(m_offset, 0);
    }

    int operator[] (Tuple<Chroma, int> i) const
    {
        return m_offset[int(i.get<Chroma>())][i.get<int>()];
    }

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
            Chroma chroma, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_offset[int(chroma)][i] = getValue();
        syntaxCheck(-512 <= m_offset[int(chroma)][i]);
        syntaxCheck(511 >= m_offset[int(chroma)][i]);
    }
};
/*----------------------------------------------------------------------------*/
struct LumaWeightL1Flag:
    public RefListFlagList,
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::luma_weight_l1_flag;

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list[i] = getValue();
    }
};
/*----------------------------------------------------------------------------*/
struct ChromaWeightL1Flag:
    public RefListFlagList,
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::chroma_weight_l1_flag;

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list[i] = getValue();
    }
};
/*----------------------------------------------------------------------------*/
class DeltaLumaWeightL1:
    public Embedded,
    public VLD::IntExpGolombCoded
{
    std::array<int, Limits::RefListIdx::max + 1> m_delta;
public:
    static const auto Id = ElementId::delta_luma_weight_l1;

    DeltaLumaWeightL1()
    {
        /* 04/2013, 7.4.7.3 "Weighted prediction parameters semantics"
         * inferred */
        fill(m_delta, 0);
    }

    int operator[](int i) const
    {
        return m_delta[i];
    }

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
            int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_delta[i] = getValue();
        syntaxCheck(-128 <= m_delta[i]);
        syntaxCheck(127 >= m_delta[i]);
    }
};
/*----------------------------------------------------------------------------*/
class LumaOffsetL1:
    public Embedded,
    public VLD::IntExpGolombCoded
{
    std::array<int, Limits::RefListIdx::max + 1> m_offset;
public:
    static const auto Id = ElementId::luma_offset_l1;

    LumaOffsetL1()
    {
        /* 04/2013, 7.4.7.3 "Weighted prediction parameters semantics"
         * inferred */
        fill(m_offset, 0);
    }

    int operator[] (int i) const
    {
        return m_offset[i];
    }

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
            int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_offset[i] = getValue();
        syntaxCheck(-128 <= m_offset[i]);
        syntaxCheck(127 >= m_offset[i]);
    }
};
/*----------------------------------------------------------------------------*/
class DeltaChromaWeightL1:
    public Embedded,
    public VLD::IntExpGolombCoded
{
private:
    std::array<
        std::array<int, Limits::RefListIdx::max + 1>,
        EnumRange<Chroma>::length()> m_delta;
public:
    static const auto Id = ElementId::delta_chroma_weight_l1;

    DeltaChromaWeightL1()
    {
        fill(m_delta, 0);
    }

    int operator[] (Tuple<Chroma, int> i) const
    {
        return m_delta[int(i.get<Chroma>())][i.get<int>()];
    }

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
            Chroma chroma, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_delta[int(chroma)][i] = getValue();
        syntaxCheck(-128 <= m_delta[int(chroma)][i]);
        syntaxCheck(127 >= m_delta[int(chroma)][i]);
    }
};
/*----------------------------------------------------------------------------*/
class DeltaChromaOffsetL1:
    public Embedded,
    public VLD::IntExpGolombCoded
{
    std::array<
        std::array<int, Limits::RefListIdx::max + 1>,
        EnumRange<Chroma>::length()> m_offset;
public:
    static const auto Id = ElementId::delta_chroma_offset_l1;

    DeltaChromaOffsetL1()
    {
        fill(m_offset, 0);
    }

    int operator[] (Tuple<Chroma, int> i) const
    {
        return m_offset[int(i.get<Chroma>())][i.get<int>()];
    }

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
            Chroma chroma, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_offset[int(chroma)][i] = getValue();
        syntaxCheck(-512 <= m_offset[int(chroma)][i]);
        syntaxCheck(511 >= m_offset[int(chroma)][i]);
    }
};
/*----------------------------------------------------------------------------*/
class LumaWeightLx: public Embedded
{
    std::array<
        std::array<int, Limits::RefListIdx::max + 1>,
        EnumRange<RefList>::length()> m_weight;
public:
    static const auto Id = ElementId::LumaWeightLx;

    LumaWeightLx()
    {
        fill(m_weight, 0);
    }

    void setL0(
            int numRefIdxL0ActiveMinus1,
            const LumaLog2WeightDenom &denom,
            const DeltaLumaWeightL0 &deltaL0)
    {
        const auto d = toInt(denom.inUnits());

        for(auto i = 0; i < numRefIdxL0ActiveMinus1 + 1; ++i)
        {
            m_weight[int(RefList::L0)][i] = d + deltaL0[i];
        }
    }

    int operator[] (Tuple<RefList, int> i) const
    {
        return m_weight[int(i.get<RefList>())][i.get<int>()];
    }

    void setL1(
            int numRefIdxL1ActiveMinus1,
            const LumaLog2WeightDenom &denom,
            const DeltaLumaWeightL1 &deltaL1)
    {
        const auto d = toInt(denom.inUnits());

        for(auto i = 0; i < numRefIdxL1ActiveMinus1 + 1; ++i)
        {
            m_weight[int(RefList::L1)][i] = d + deltaL1[i];
        }
    }
};
/*----------------------------------------------------------------------------*/
class ChromaWeightLx: public Embedded
{
    std::array<
        std::array<
            std::array<int, Limits::RefListIdx::max + 1>,
            EnumRange<RefList>::length()>,
        EnumRange<Chroma>::length()> m_weight;
public:
    static const auto Id = ElementId::ChromaWeightLx;

    ChromaWeightLx()
    {
        fill(m_weight, 0);
    }

    int operator[] (Tuple<Chroma, RefList, int> i) const
    {
        return m_weight[int(i.get<Chroma>())][int(i.get<RefList>())][i.get<int>()];
    }

    void setL0(
            int numRefIdxL0ActiveMinus1,
            const ChromaLog2WeightDenom &denom,
            const DeltaChromaWeightL0 &deltaL0)
    {
        const auto d = toInt(denom.inUnits());

        for(auto chroma : EnumRange<Chroma>())
        {
            for(auto i = 0; i < numRefIdxL0ActiveMinus1 + 1; ++i)
            {
                m_weight[int(chroma)][int(RefList::L0)][i] =
                    d + deltaL0[makeTuple(chroma, i)];
            }
        }
    }

    void setL1(
            int numRefIdxL1ActiveMinus1,
            const ChromaLog2WeightDenom &denom,
            const DeltaChromaWeightL1 &deltaL1)
    {
        const auto d = toInt(denom.inUnits());

        for(auto chroma : EnumRange<Chroma>())
        {
            for(auto i = 0; i < numRefIdxL1ActiveMinus1 + 1; ++i)
            {
                m_weight[int(chroma)][int(RefList::L1)][i] =
                    d + deltaL1[makeTuple(chroma, i)];
            }
        }
    }
};
/*----------------------------------------------------------------------------*/
class ChromaOffsetLx: public Embedded
{
    std::array<
        std::array<
            std::array<int, Limits::RefListIdx::max + 1>,
            EnumRange<RefList>::length()>,
        EnumRange<Chroma>::length()> m_offset;
public:
    static const auto Id = ElementId::ChromaOffsetLx;

    ChromaOffsetLx()
    {
        fill(m_offset, 0);
    }

    int operator[] (Tuple<Chroma, RefList, int> i) const
    {
        return m_offset[int(i.get<Chroma>())][int(i.get<RefList>())][i.get<int>()];
    }

    void setL0(
            int numRefIdxL0ActiveMinus1,
            const ChromaLog2WeightDenom &denom,
            const ChromaWeightL0Flag &flag,
            const DeltaChromaOffsetL0 &offset,
            const ChromaWeightLx &weightLx,
            int wpOffsetHalfRange)
    {
        const auto d = toUnderlying(denom.inUnits());

        for(auto chroma : EnumRange<Chroma>())
        {
            for(auto i = 0; i <= numRefIdxL0ActiveMinus1; ++i)
            {
                if(flag[i])
                {
                    const auto value =
                        wpOffsetHalfRange
                        + offset[makeTuple(chroma, i)]
                        - (wpOffsetHalfRange * weightLx[makeTuple(chroma, RefList::L0, i)] >> d);

                    m_offset[int(chroma)][int(RefList::L0)][i] =
                        clip3(-wpOffsetHalfRange, wpOffsetHalfRange -1, value);
                }
            }
        }
    }

    void setL1(
            int numRefIdxL1ActiveMinus1,
            const ChromaLog2WeightDenom &denom,
            const ChromaWeightL1Flag &flag,
            const DeltaChromaOffsetL1 &offset,
            const ChromaWeightLx &weightLx,
            int wpOffsetHalfRange)
    {
        const auto d = toUnderlying(denom.inUnits());

        for(auto chroma : EnumRange<Chroma>())
        {
            for(auto i = 0; i <= numRefIdxL1ActiveMinus1; ++i)
            {
                if(flag[i])
                {
                    const auto value =
                        wpOffsetHalfRange
                        + offset[makeTuple(chroma, i)]
                        - (wpOffsetHalfRange * weightLx[makeTuple(chroma, RefList::L1, i)] >> d);

                    m_offset[int(chroma)][int(RefList::L1)][i] =
                        clip3(-wpOffsetHalfRange, wpOffsetHalfRange -1, value);
                }
            }
        }
    }
};
/*----------------------------------------------------------------------------*/
} /* namespace PredWeightTableContent */

/*----------------------------------------------------------------------------*/
class PredWeightTable:
    public EmbeddedAggregator<
        PredWeightTableContent::LumaLog2WeightDenom,
        PredWeightTableContent::DeltaChromaLog2WeightDenom,
        PredWeightTableContent::LumaWeightL0Flag,
        PredWeightTableContent::ChromaWeightL0Flag,
        PredWeightTableContent::DeltaLumaWeightL0,
        PredWeightTableContent::LumaOffsetL0,
        PredWeightTableContent::DeltaChromaWeightL0,
        PredWeightTableContent::DeltaChromaOffsetL0,
        PredWeightTableContent::LumaWeightL1Flag,
        PredWeightTableContent::ChromaWeightL1Flag,
        PredWeightTableContent::DeltaLumaWeightL1,
        PredWeightTableContent::LumaOffsetL1,
        PredWeightTableContent::DeltaChromaWeightL1,
        PredWeightTableContent::DeltaChromaOffsetL1,
        PredWeightTableContent::LumaWeightLx,
        PredWeightTableContent::ChromaLog2WeightDenom,
        PredWeightTableContent::ChromaWeightLx,
        PredWeightTableContent::ChromaOffsetLx>
{
public:
    static const auto Id = ElementId::pred_weight_table;

    typedef PredWeightTableContent::LumaLog2WeightDenom LumaLog2WeightDenom;
    typedef PredWeightTableContent::DeltaChromaLog2WeightDenom DeltaChromaLog2WeightDenom;
    typedef PredWeightTableContent::LumaWeightL0Flag LumaWeightL0Flag;
    typedef PredWeightTableContent::ChromaWeightL0Flag ChromaWeightL0Flag;
    typedef PredWeightTableContent::DeltaLumaWeightL0 DeltaLumaWeightL0;
    typedef PredWeightTableContent::LumaOffsetL0 LumaOffsetL0;
    typedef PredWeightTableContent::DeltaChromaWeightL0 DeltaChromaWeightL0;
    typedef PredWeightTableContent::DeltaChromaOffsetL0 DeltaChromaOffsetL0;
    typedef PredWeightTableContent::LumaWeightL1Flag LumaWeightL1Flag;
    typedef PredWeightTableContent::ChromaWeightL1Flag ChromaWeightL1Flag;
    typedef PredWeightTableContent::DeltaLumaWeightL1 DeltaLumaWeightL1;
    typedef PredWeightTableContent::LumaOffsetL1 LumaOffsetL1;
    typedef PredWeightTableContent::DeltaChromaWeightL1 DeltaChromaWeightL1;
    typedef PredWeightTableContent::DeltaChromaOffsetL1 DeltaChromaOffsetL1;
    typedef PredWeightTableContent::LumaWeightLx LumaWeightLx;
    typedef PredWeightTableContent::ChromaLog2WeightDenom ChromaLog2WeightDenom;
    typedef PredWeightTableContent::ChromaWeightLx ChromaWeightLx;
    typedef PredWeightTableContent::ChromaOffsetLx ChromaOffsetLx;

    void toStr(std::ostream &os) const;
    void onParse(StreamAccessLayer &, Decoder::State &decoder, const SliceSegmentHeader &);

    Log2 weightDenom(Plane plane) const
    {
        return
            Plane::Y == plane
            ? get<LumaLog2WeightDenom>()->inUnits()
            : get<ChromaLog2WeightDenom>()->inUnits();
    }

    int weight(Plane plane, RefList l, int i) const
    {
        if(Plane::Cb == plane)
        {
            return (*get<ChromaWeightLx>())[makeTuple(Chroma::Cb, l, i)];
        }
        else if(Plane::Cr == plane)
        {
            return (*get<ChromaWeightLx>())[makeTuple(Chroma::Cr, l, i)];
        }
        else //if(Plane::Y == plane)
        {
            return (*get<LumaWeightLx>())[makeTuple(l, i)];
        }
    }

    int offset(Plane plane, RefList l, int i) const
    {
        if(Plane::Cb == plane)
        {
            return (*get<ChromaOffsetLx>())[makeTuple(Chroma::Cb, l, i)];
        }
        else if(Plane::Cr == plane)
        {
            return (*get<ChromaOffsetLx>())[makeTuple(Chroma::Cr, l, i)];
        }
        else //if(Plane::Y == plane)
        {
            return RefList::L0 == l ? (*get<LumaOffsetL0>())[i] : (*get<LumaOffsetL1>())[i];
        }
    }
};
/*----------------------------------------------------------------------------*/
}} // namespace HEVC::Syntax

#endif // HEVC_Syntax_PredWeightTable_h
