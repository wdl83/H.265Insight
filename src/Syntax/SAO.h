#ifndef HEVC_Syntax_SAO_h
#define HEVC_Syntax_SAO_h

/* STDC++ */
#include <algorithm>
#include <array>
/* HEVC */
#include <Syntax/Syntax.h>

/* 04/2013, 7.4.9.3 "Sample adaptive offset semantics" */
namespace HEVC { namespace Syntax { namespace SAOContent {
/*----------------------------------------------------------------------------*/
typedef EmbeddedCoord<Pel, ElementId::SaoCoord> Coord;
/*----------------------------------------------------------------------------*/
class SaoMergeLeftFlag:
    public Embedded,
    public CABAD::FixedLength
{
public:
    static const auto Id = ElementId::sao_merge_left_flag;

    SaoMergeLeftFlag():
        /* 10v34, 9.3.3.1 Table 9-32 */
        CABAD::FixedLength{1}
    {
        /* inferred */
        setValue(false);
    }

    explicit operator bool () const
    {
        return getValue();
    }

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder)
    {
        const auto contextModel =
            [](CABAD::State &state, int binIdx) -> CABAD::Variable &
            {
                syntaxCheck(0 == binIdx);
                return state.getVariable(CABAD::CtxId::sao_merge_flag, 0);
            };

        getFrom(streamAccessLayer, decoder, cabadState(decoder), *this, contextModel);
    }
};
/*----------------------------------------------------------------------------*/
class SaoMergeUpFlag:
    public Embedded,
    public CABAD::FixedLength
{
public:
    static const auto Id = ElementId::sao_merge_up_flag;

    SaoMergeUpFlag():
        /* 10v34, 9.3.3.1 Table 9-32 */
        CABAD::FixedLength{1}
    {
        /* Draft 10v24, 7.4.8.3 "Sample adaptive offset semantics" */
        /* inferred */
        setValue(false);
    }

    explicit operator bool () const
    {
        return getValue();
    }

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder)
    {
        const auto contextModel =
            [](CABAD::State &state, int binIdx) -> CABAD::Variable &
            {
                syntaxCheck(0 == binIdx);
                return state.getVariable(CABAD::CtxId::sao_merge_flag, 0);
            };

        getFrom(streamAccessLayer, decoder, cabadState(decoder), *this, contextModel);
    }
};
/*----------------------------------------------------------------------------*/
class SaoTypeIdxLuma:
    public Embedded,
    public CABAD::TruncatedRice
{
public:
    static const auto Id = ElementId::sao_type_idx_luma;

    SaoTypeIdxLuma():
        /* 10v34, 9.3.3.1 Table 9-32 */
        CABAD::TruncatedRice{0, 2}
    {
        setValue(int(SaoType::NotApplied));
    }

    operator SaoType () const
    {
        return static_cast<SaoType>(getValue());
    }

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder)
    {
        const auto contextModel =
            [](CABAD::State &state, int binIdx) -> CABAD::Variable *
            {
                syntaxCheck(0 == binIdx || 1 == binIdx);

                return
                    0 == binIdx
                    ? &state.getVariable(CABAD::CtxId::sao_type_idx, 0)
                    : nullptr;
            };

        getFrom(streamAccessLayer, decoder, cabadState(decoder), *this, contextModel);
    }
};
/*----------------------------------------------------------------------------*/
class SaoTypeIdxChroma:
    public Embedded,
    public CABAD::TruncatedRice
{
public:
    static const auto Id = ElementId::sao_type_idx_chroma;

    SaoTypeIdxChroma():
        /* 10v34, 9.3.3.1 Table 9-32 */
        CABAD::TruncatedRice{0, 2}
    {
        setValue(int(SaoType::NotApplied));
    }

    operator SaoType () const
    {
        return static_cast<SaoType>(getValue());
    }

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder)
    {
        const auto contextModel =
            [](CABAD::State &state, int binIdx) -> CABAD::Variable *
            {
                syntaxCheck(0 == binIdx || 1 == binIdx);

                return
                    0 == binIdx
                    ? &state.getVariable(CABAD::CtxId::sao_type_idx, 0)
                    : nullptr;
            };

        getFrom(streamAccessLayer, decoder, cabadState(decoder), *this, contextModel);
    }
};
/*----------------------------------------------------------------------------*/
class SaoOffsetAbs:
    public Embedded,
    public CABAD::TruncatedRice
{
    std::array<std::array<int, 4>, EnumRange<Plane>::length()> m_saoOffsetAbs;
public:
    static const auto Id = ElementId::sao_offset_abs;

    SaoOffsetAbs():
        CABAD::TruncatedRice{0, 0}
    {
        for(auto &i : m_saoOffsetAbs)
        {
            std::fill(std::begin(i), std::end(i), 0);
        }
    }

    int operator[] (Tuple<Plane, int> i) const
    {
        return m_saoOffsetAbs[int(i.get<Plane>())][i.get<int>()];
    }

    void onParse(StreamAccessLayer &, Decoder::State &decoder, Plane, int);
};
/*----------------------------------------------------------------------------*/
class SaoOffsetSign:
    public Embedded,
    public CABAD::FixedLength
{
    std::array<std::array<bool, 4>, EnumRange<Plane>::length()> m_saoOffsetSign;
public:
    static const auto Id = ElementId::sao_offset_sign;

    SaoOffsetSign():
        CABAD::FixedLength{1}
    {
        for(auto &i : m_saoOffsetSign)
        {
            std::fill(std::begin(i), std::end(i), false);
        }
    }

    bool operator[] (Tuple<Plane, int> i) const
    {
        return m_saoOffsetSign[int(i.get<Plane>())][i.get<int>()];
    }

    void init(Plane cIdx, SaoType type)
    {
        if(SaoType::EdgeOffset == type)
        {
            m_saoOffsetSign[int(cIdx)][2] = true;
            m_saoOffsetSign[int(cIdx)][3] = true;
        }
    }

    void onParse(StreamAccessLayer &, Decoder::State &decoder, Plane, int);
};
/*----------------------------------------------------------------------------*/
class SaoBandPosition:
    public Embedded,
    public CABAD::FixedLength
{
    std::array<int, EnumRange<Plane>::length()> m_bandPosition;
public:
    static const auto Id = ElementId::sao_band_position;

    SaoBandPosition():
        CABAD::FixedLength{31}
    {
        std::fill(std::begin(m_bandPosition), std::end(m_bandPosition), 0);
    }

    int operator[] (Plane plane) const
    {
        return m_bandPosition[int(plane)];
    }

    void onParse(StreamAccessLayer &, Decoder::State &decoder, Plane);

    void merge(const SAO &);
};
/*----------------------------------------------------------------------------*/
class SaoEoClassLuma:
    public Embedded,
    public CABAD::FixedLength
{
public:
    static const auto Id = ElementId::sao_eo_class_luma;

    SaoEoClassLuma():
        CABAD::FixedLength{3}
    {}

    operator SaoEdgeOffsetClass () const
    {
        return static_cast<SaoEdgeOffsetClass>(getValue());
    }
};
/*----------------------------------------------------------------------------*/
class SaoEoClassChroma:
    public Embedded,
    public CABAD::FixedLength
{
public:
    static const auto Id = ElementId::sao_eo_class_chroma;

    SaoEoClassChroma():
        CABAD::FixedLength{3}
    {}

    operator SaoEdgeOffsetClass () const
    {
        return static_cast<SaoEdgeOffsetClass>(getValue());
    }
};
/*----------------------------------------------------------------------------*/
class SaoTypeIdx: public Embedded
{
    std::array<SaoType, EnumRange<Component>::length()> m_saoTypeIdx;
public:
    static const auto Id = ElementId::SaoTypeIdx;

    SaoTypeIdx()
    {
        fill(m_saoTypeIdx, SaoType::NotApplied);
    }

    SaoType operator[] (Component cIdx) const
    {
        return m_saoTypeIdx[int(cIdx)];
    }

    void set(const SaoTypeIdxLuma &saoTypeIdxLuma)
    {
        m_saoTypeIdx[int(Component::Luma)] = saoTypeIdxLuma;
    }

    void set(const SaoTypeIdxChroma &saoTypeIdxChroma)
    {
        m_saoTypeIdx[int(Component::Chroma)] = saoTypeIdxChroma;
    }

    void merge(const SAO &);
};
/*----------------------------------------------------------------------------*/
class SaoOffsetVal: public Embedded
{
    std::array<
        std::array<int, 5>,
        EnumRange<Plane>::length()> m_saoOffset;
public:
    static const auto Id = ElementId::SaoOffsetVal;

    class IndexToOffset
    {
        friend SaoOffsetVal;

        std::array<int, 5> m_offset;

        IndexToOffset(const std::array<int, 5> &offset): m_offset(offset)
        {}
    public:
        int operator[] (int i) const
        {
            return m_offset[i];
        }
    };

    IndexToOffset operator[] (Plane plane) const
    {
        return IndexToOffset(m_saoOffset[int(plane)]);
    }

    bool operator[] (Tuple<Plane, int> i) const
    {
        return m_saoOffset[int(i.get<Plane>())][i.get<int>()];
    }

    void set(
            const SaoOffsetAbs &offsetAbs,
            const SaoOffsetSign &offsetSign,
            Log2 offsetScaleY, Log2 offsetScaleC)
    {
        /* 04/2013, 7.4.9.3 "Sample adaptive offset semantics" */

        for(auto plane : EnumRange<Plane>())
        {
            m_saoOffset[int(plane)][0] = 0;

            for(auto i = 0; i < 4; ++i)
            {
                m_saoOffset[int(plane)][i + 1] =
                    (1 - 2 * offsetSign[makeTuple(plane, i)])
                    * offsetAbs[makeTuple(plane, i)]
                    << (
                            Plane::Y == plane
                            ? toUnderlying(offsetScaleY)
                            : toUnderlying(offsetScaleC));
            }
        }
    }

    void merge(const SAO &);
};
/*----------------------------------------------------------------------------*/
class SaoEoClass: public Embedded
{
    std::array<SaoEdgeOffsetClass, EnumRange<Component>::length()> m_saoEoClass;
public:
    static const auto Id = ElementId::SaoEoClass;

    SaoEoClass()
    {
        std::fill(
                std::begin(m_saoEoClass),
                std::end(m_saoEoClass),
                SaoEdgeOffsetClass::Degree0);
    }

    SaoEdgeOffsetClass
    operator[] (Component component) const
    {
        return m_saoEoClass[int(component)];
    }

    void set(
            const SaoEoClassLuma &eoClassLuma,
            const SaoEoClassChroma &eoClassChroma)
    {
        m_saoEoClass[int(Component::Luma)] = eoClassLuma;
        m_saoEoClass[int(Component::Chroma)] = eoClassChroma;
    }

    void merge(const SAO &);
};
/*----------------------------------------------------------------------------*/
} /* SAOContent */

class SAO:
    public EmbeddedAggregator<
        SAOContent::Coord,
        SAOContent::SaoMergeLeftFlag,
        SAOContent::SaoMergeUpFlag,
        SAOContent::SaoTypeIdxLuma,
        SAOContent::SaoTypeIdxChroma,
        SAOContent::SaoOffsetAbs,
        SAOContent::SaoOffsetSign,
        SAOContent::SaoBandPosition,
        SAOContent::SaoEoClassLuma,
        SAOContent::SaoEoClassChroma,
        SAOContent::SaoTypeIdx,
        SAOContent::SaoOffsetVal,
        SAOContent::SaoEoClass>
{
public:
    static const auto Id = ElementId::sao;

    typedef SAOContent::Coord Coord;
    typedef SAOContent::SaoMergeLeftFlag SaoMergeLeftFlag;
    typedef SAOContent::SaoMergeUpFlag SaoMergeUpFlag;
    typedef SAOContent::SaoTypeIdxLuma SaoTypeIdxLuma;
    typedef SAOContent::SaoTypeIdxChroma SaoTypeIdxChroma;
    typedef SAOContent::SaoOffsetAbs SaoOffsetAbs;
    typedef SAOContent::SaoOffsetSign SaoOffsetSign;
    typedef SAOContent::SaoBandPosition SaoBandPosition;
    typedef SAOContent::SaoEoClassLuma SaoEoClassLuma;
    typedef SAOContent::SaoEoClassChroma SaoEoClassChroma;
    typedef SAOContent::SaoTypeIdx SaoTypeIdx;
    typedef SAOContent::SaoOffsetVal SaoOffsetVal;
    typedef SAOContent::SaoEoClass SaoEoClass;

    SAO(PelCoord saoCoord)
    {
        embed<Coord>(*this, saoCoord);
    }

    void onParse(StreamAccessLayer &, Decoder::State &decoder);
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */

#endif /* HEVC_Syntax_SAO_h */
