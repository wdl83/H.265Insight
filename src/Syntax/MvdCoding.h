#ifndef HEVC_Syntax_MvdCoding_h
#define HEVC_Syntax_MvdCoding_h

#include <Syntax/Syntax.h>

namespace HEVC { namespace Syntax { namespace MvdCodingContent {
/*----------------------------------------------------------------------------*/
class AbsMvdGreater0Flag:
    public Embedded,
    public CABAD::FixedLength
{
private:
    std::array<bool, EnumRange<Direction>::length()> m_absMvdGreater0Flag;
public:
    static const auto Id = ElementId::abs_mvd_greater0_flag;

    AbsMvdGreater0Flag():
        /* 04/2013, 9.3.3 "Binarization process", Table 9-32 */
        CABAD::FixedLength{1}
    {}

    bool operator[] (Direction d) const
    {
        return m_absMvdGreater0Flag[int(d)];
    }

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
            Direction d)
    {
        const auto contextModel =
            [](CABAD::State &state, int binIdx) -> CABAD::Variable &
            {
                /* 04/2013, 9.3.4.2
                 * "Derivation process for ctxTable, ctxIdx and bypassFlag",
                 * Table 9-37 */
                syntaxCheck(0 == binIdx);
                return state.getVariable(CABAD::CtxId::abs_mvd_greater0_flag, 0);
            };

        getFrom(streamAccessLayer, decoder, cabadState(decoder), *this, contextModel);
        m_absMvdGreater0Flag[int(d)] = getValue();
    }
};
/*----------------------------------------------------------------------------*/
class AbsMvdGreater1Flag:
    public Embedded,
    public CABAD::FixedLength
{
private:
    std::array<bool, EnumRange<Direction>::length()> m_absMvdGreater1Flag;
public:
    static const auto Id = ElementId::abs_mvd_greater1_flag;

    AbsMvdGreater1Flag():
        /* 04/2013, 9.3.3 "Binarization process", Table 9-32 */
        CABAD::FixedLength{1},
        /* 04/2013, 7.4.9.9 "Motion vector difference semantics" */
        m_absMvdGreater1Flag
        {
            {false, false}
        }
    {}

    bool operator[] (Direction d) const
    {
        return m_absMvdGreater1Flag[int(d)];
    }

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
            Direction d)
    {
        const auto contextModel =
            [](CABAD::State &state, int binIdx) -> CABAD::Variable &
            {
                /* 04/2013, 9.3.4.2
                 * "Derivation process for ctxTable, ctxIdx and bypassFlag",
                 * Table 9-37 */
                syntaxCheck(0 == binIdx);
                return state.getVariable(CABAD::CtxId::abs_mvd_greater1_flag, 0);
            };

        getFrom(streamAccessLayer, decoder, cabadState(decoder), *this, contextModel);
        m_absMvdGreater1Flag[int(d)] = getValue();
    }
};
/*----------------------------------------------------------------------------*/
class AbsMvdMinus2:
    public Embedded,
    public CABAD::ExpGolomb
{
private:
    std::array<int, EnumRange<Direction>::length()> m_absMvdMinus2;
public:
    static const auto Id = ElementId::abs_mvd_minus2;

    AbsMvdMinus2():
        /* 04/2013, 9.3.3 "Binarization process", Table 9-32 */
        CABAD::ExpGolomb{1},
        /* 04/2013, 7.4.9.9 "Motion vector difference semantics" */
        m_absMvdMinus2
        {
            {-1, -1}
        }
    {}

    int operator[] (Direction d) const
    {
        return m_absMvdMinus2[int(d)];
    }

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
            Direction d)
    {
        getFrom(streamAccessLayer, decoder, cabadState(decoder), *this);
        m_absMvdMinus2[int(d)] = getValue();
    }
};
/*----------------------------------------------------------------------------*/
class MvdSignFlag:
    public Embedded,
    public CABAD::FixedLength
{
private:
    std::array<bool, EnumRange<Direction>::length()> m_absMvdSignFlag;
public:
    static const auto Id = ElementId::mvd_sign_flag;

    MvdSignFlag():
        /* 04/2013, 9.3.3 "Binarization process", Table 9-32 */
        CABAD::FixedLength{1},
        m_absMvdSignFlag
        {
            {false, false}
        }
    {}

    bool operator[] (Direction d) const
    {
        return m_absMvdSignFlag[int(d)];
    }

    void onParse(
            StreamAccessLayer &streamAccessLayer,
            Decoder::State &decoder,
            Direction d)
    {
        getFrom(streamAccessLayer, decoder, cabadState(decoder), *this);
        m_absMvdSignFlag[int(d)] = getValue();
    }
};
/*----------------------------------------------------------------------------*/
class LMvd:
    public Embedded,
    public MotionVector
{
public:
    static const auto Id = ElementId::lMvd;

    LMvd(
            const AbsMvdGreater0Flag &absMvdGreater0Flag,
            const AbsMvdMinus2 &absMvdMinus2,
            const MvdSignFlag &mvdSignFlag):
        MotionVector
        {
            MotionVector::ValueType
            {
                    absMvdGreater0Flag[Direction::H]
                    * (absMvdMinus2[Direction::H] + 2)
                    * (1 - 2 * mvdSignFlag[Direction::H])
            },
            MotionVector::ValueType
            {
                    absMvdGreater0Flag[Direction::V]
                    * (absMvdMinus2[Direction::V] + 2)
                    * (1 - 2 * mvdSignFlag[Direction::V])
            },
        }
    {}
};
/*----------------------------------------------------------------------------*/
} /* MvdCodingContent */

class MvdCoding:
    public EmbeddedAggregator<
        MvdCodingContent::AbsMvdGreater0Flag,
        MvdCodingContent::AbsMvdGreater1Flag,
        MvdCodingContent::AbsMvdMinus2,
        MvdCodingContent::MvdSignFlag,
        MvdCodingContent::LMvd>
{
public:
    static const auto Id = ElementId::mvd_coding;

    typedef MvdCodingContent::AbsMvdGreater0Flag AbsMvdGreater0Flag;
    typedef MvdCodingContent::AbsMvdGreater1Flag AbsMvdGreater1Flag;
    typedef MvdCodingContent::AbsMvdMinus2 AbsMvdMinus2;
    typedef MvdCodingContent::MvdSignFlag MvdSignFlag;
    typedef MvdCodingContent::LMvd LMvd;

    MvdCoding(PelCoord, RefList refList):
        m_refList(refList)
    {}

    RefList refList() const
    {
        return m_refList;
    }

    void onParse(StreamAccessLayer &, Decoder::State &);
private:
    RefList m_refList;
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */

#endif /* HEVC_Syntax_MvdCoding_h */
