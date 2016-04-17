#ifndef HEVC_Syntax_PredictionUnit_h
#define HEVC_Syntax_PredictionUnit_h

#include <Syntax/Syntax.h>
#include <Syntax/SliceSegmentHeader.h>
#include <Syntax/MvdCoding.h>

namespace HEVC { namespace Syntax { namespace PredictionUnitContent {
/*----------------------------------------------------------------------------*/
typedef EmbeddedCoord<Pel, ElementId::PredictionUnitCoord> Coord;
typedef EmbeddedUnit<Pel, ElementId::nPbW> Width;
typedef EmbeddedUnit<Pel, ElementId::nPbH> Height;
typedef EmbeddedUnit<int, ElementId::PredictionUnitPartIdx> PartIdx;
/*----------------------------------------------------------------------------*/
class MvLX:
    public Embedded,
    public Pair<MotionVector, RefList>
{
public:
    static const auto Id = ElementId::MvLX;

    MvLX(Pair<MotionVector, RefList> mvLX):
        Pair<MotionVector, RefList>{mvLX}
    {}

    friend
    bool operator== (const MvLX &x, const MvLX &y)
    {
        return
            x[RefList::L0] == y[RefList::L0]
            && x[RefList::L1] == y[RefList::L1];
    }
};
/*----------------------------------------------------------------------------*/
class MvCLX:
    public Embedded,
    public Pair<MotionVector, RefList>
{
public:
    static const auto Id = ElementId::MvCLX;

    MvCLX(Pair<MotionVector, RefList> mvLX):
        Pair<MotionVector, RefList>{mvLX}
    {}

    friend
    bool operator== (const MvCLX &x, const MvCLX &y)
    {
        return
            x[RefList::L0] == y[RefList::L0]
            && x[RefList::L1] == y[RefList::L1];
    }
};
/*----------------------------------------------------------------------------*/
class PredFlagLX:
    public Embedded,
    public Pair<bool, RefList>
{
public:
    static const auto Id = ElementId::PredFlagLX;

    PredFlagLX(Pair<bool, RefList> predFlagLX):
        Pair<bool, RefList>{predFlagLX}
    {}

    friend
    bool operator== (const PredFlagLX &x, const PredFlagLX &y)
    {
        return
            x[RefList::L0] == y[RefList::L0]
            && x[RefList::L1] == y[RefList::L1];
    }
};
/*----------------------------------------------------------------------------*/
class RefIdxLX:
    public Embedded,
    public Pair<PicOrderCntVal, RefList>
{
public:
    static const auto Id = ElementId::RefIdxLX;

    RefIdxLX(Pair<PicOrderCntVal, RefList> refIdxLX):
        Pair<PicOrderCntVal, RefList>{refIdxLX}
    {}

    friend
    bool operator== (const RefIdxLX &x, const RefIdxLX &y)
    {
        return
            x[RefList::L0] == y[RefList::L0]
            && x[RefList::L1] == y[RefList::L1];
    }
};
/*----------------------------------------------------------------------------*/
class MergeIdx:
    public Embedded,
    public CABAD::TruncatedRice
{
public:
    static const auto Id = ElementId::merge_idx;

    MergeIdx(const SliceSegmentHeader::MaxNumMergeCand &maxNumMergeCand):
        /* 04/2013, * 9.3.3 "Binarization process", Table 9-32 */
        CABAD::TruncatedRice{0, maxNumMergeCand - 1}
    {
        /* inferred */
        setValue(0);
    }

    operator int () const
    {
        return getValue();
    }

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder)
    {
        const auto contextModel =
            [](CABAD::State &state, int binIdx) -> CABAD::Variable *
            {
                /* 04/2013,
                 * 9.3.4.2 "Derivation process for ctxTable, ctxIdx and bypassFlag",
                 * Table 9-37 */

                syntaxCheck(4 > binIdx);

                return
                    0 == binIdx
                    ? &state.getVariable(CABAD::CtxId::merge_idx, 0)
                    : nullptr;
            };

        getFrom(streamAccessLayer, decoder, cabadState(decoder), *this, contextModel);
    }
};
/*----------------------------------------------------------------------------*/
class MergeFlag:
    public Embedded,
    public CABAD::FixedLength
{
public:
    static const auto Id = ElementId::merge_flag;

    MergeFlag(PredictionMode cuPredMode):
        /* 04/2013, * 9.3.3 "Binarization process", Table 9-32 */
        CABAD::FixedLength{1}
    {
        /* inferred */
        setValue(PredictionMode::Skip == cuPredMode);
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
                /* 04/2013,
                 * 9.3.4.2 "Derivation process for ctxTable, ctxIdx and bypassFlag",
                 * Table 9-37 */
                syntaxCheck(0 == binIdx);
                return state.getVariable(CABAD::CtxId::merge_flag, 0);
            };

        getFrom(streamAccessLayer, decoder, cabadState(decoder), *this, contextModel);
    }
};
/*----------------------------------------------------------------------------*/
class InterPredIdcDebinarizer: public CABAD::Debinarizer
{
public:
    static const auto debinarizerId = CABAD::DebinarizerId::Special;

    static
    CABAD::BinString getBinarization(
            StreamAccessLayer &streamAccessLayer,
            CABAD::State &state,
            Pel nPbW, Pel nPbH, int cqtDepth);

    static
    CABAD::BinValueType getBinarizationValue(
            CABAD::BinString binarization,
            Pel nPbW, Pel nPbH)
    {
        /* 10v34, 9.3.3.7, Table 9-36 */

        if(12_pel != nPbW + nPbH)
        {
            static const std::map<CABAD::BinString, CABAD::BinValueType> values =
            {
                {{0, 0}, 0},
                {{0, 1}, 1},
                {{1}, 2},
            };

            syntaxCheck(values.count(binarization));
            return values.at(binarization);
        }
        else
        {
            static const std::map<CABAD::BinString, CABAD::BinValueType> values =
            {
                {{0}, 0},
                {{1}, 1},
            };

            syntaxCheck(values.count(binarization));
            return values.at(binarization);
        }
    }

    CABAD::BinValueType getBinarizationValue(
            StreamAccessLayer &streamAccessLayer,
            CABAD::State &state,
            Pel nPbW, Pel nPbH, int cqtDepth)
    {
        m_binarization =
            getBinarization(streamAccessLayer, state, nPbW, nPbH, cqtDepth);
        return getBinarizationValue(m_binarization, nPbW, nPbH);
    }
};
/*----------------------------------------------------------------------------*/
class InterPredIdc:
    public Embedded,
    public CABAD::ContextAdaptive<InterPredIdcDebinarizer>
{
public:
    static const auto Id = ElementId::inter_pred_idc;

    InterPredIdc()
    {
        /* inferred */
        setValue(int(HEVC::InterPredIdc::PRED_L0));
    }

    operator HEVC::InterPredIdc () const
    {
        return static_cast<HEVC::InterPredIdc>(getValue());
    }

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
            Pel nPbW, Pel nPbH, int cqtDepth)
    {
        getFrom(streamAccessLayer, decoder, cabadState(decoder), *this, nPbW, nPbH, cqtDepth);
    }
};
/*----------------------------------------------------------------------------*/
class RefIdxL0:
    public Embedded,
    public CABAD::TruncatedRice
{
public:
    static const auto Id = ElementId::ref_idx_l0;

    RefIdxL0(const SliceSegmentHeader::NumRefIdxL0ActiveMinus1 &numRefIdxL0ActiveMinus1):
        /* 04/2013, * 9.3.3 "Binarization process", Table 9-32 */
        CABAD::TruncatedRice{0, numRefIdxL0ActiveMinus1}
    {
        /* inferred */
        setValue(0);
    }

    operator PicOrderCntVal () const
    {
        return PicOrderCntVal(getValue());
    }

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder)
    {
        const auto contextModel =
            [](CABAD::State &state, int binIdx) -> CABAD::Variable *
            {
                return
                    2 > binIdx
                    ? &state.getVariable(CABAD::CtxId::ref_idx, binIdx)
                    : nullptr;
            };

        getFrom(streamAccessLayer, decoder, cabadState(decoder), *this, contextModel);
    }
};
/*----------------------------------------------------------------------------*/
class RefIdxL1:
    public Embedded,
    public CABAD::TruncatedRice
{
public:
    static const auto Id = ElementId::ref_idx_l1;

    RefIdxL1(const SliceSegmentHeader::NumRefIdxL1ActiveMinus1 &numRefIdxL1ActiveMinus1):
        /* 04/2013, * 9.3.3 "Binarization process", Table 9-32 */
        CABAD::TruncatedRice{0, numRefIdxL1ActiveMinus1}
    {
        /* inferred */
        setValue(0);
    }

    operator PicOrderCntVal () const
    {
        return PicOrderCntVal(getValue());
    }

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder)
    {
        const auto contextModel =
            [](CABAD::State &state, int binIdx) -> CABAD::Variable *
            {
                return
                    2 > binIdx
                    ? &state.getVariable(CABAD::CtxId::ref_idx, binIdx)
                    : nullptr;
            };

        getFrom(streamAccessLayer, decoder, cabadState(decoder), *this, contextModel);
    }
};
/*----------------------------------------------------------------------------*/
class MvpL0Flag:
    public Embedded,
    public CABAD::FixedLength
{
public:
    static const auto Id = ElementId::mvp_l0_flag;

    MvpL0Flag():
        /* 04/2013, * 9.3.3 "Binarization process", Table 9-32 */
        CABAD::FixedLength{1}
    {
        /* inferred */
        setValue(0);
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
                return state.getVariable(CABAD::CtxId::mvp_flag, 0);
            };

        getFrom(streamAccessLayer, decoder, cabadState(decoder), *this, contextModel);
    }
};
/*----------------------------------------------------------------------------*/
class MvpL1Flag:
    public Embedded,
    public CABAD::FixedLength
{
public:
    static const auto Id = ElementId::mvp_l1_flag;

    MvpL1Flag():
        /* 04/2013, * 9.3.3 "Binarization process", Table 9-32 */
        CABAD::FixedLength{1}
    {
        /* inferred */
        setValue(0);
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
                return state.getVariable(CABAD::CtxId::mvp_flag, 0);
            };

        getFrom(streamAccessLayer, decoder, cabadState(decoder), *this, contextModel);
    }
};
/*----------------------------------------------------------------------------*/
class MvdLX:
    public Embedded,
    public Pair<MotionVector, RefList>
{
public:
    static const auto Id = ElementId::MvdLX;

    using Pair<MotionVector, RefList>::Pair;
};
/*----------------------------------------------------------------------------*/
} /* PredictionUnitContent */

class PredictionUnit:
    public EmbeddedAggregator<
        PredictionUnitContent::Coord,
        PredictionUnitContent::Width,
        PredictionUnitContent::Height,
        PredictionUnitContent::PartIdx,
        PredictionUnitContent::MergeIdx,
        PredictionUnitContent::MergeFlag,
        PredictionUnitContent::InterPredIdcDebinarizer,
        PredictionUnitContent::InterPredIdc,
        PredictionUnitContent::RefIdxL0,
        PredictionUnitContent::RefIdxL1,
        PredictionUnitContent::MvpL0Flag,
        PredictionUnitContent::MvpL1Flag,
        PredictionUnitContent::MvLX,
        PredictionUnitContent::MvCLX,
        PredictionUnitContent::PredFlagLX,
        PredictionUnitContent::RefIdxLX,
        PredictionUnitContent::MvdLX>,
    public SubtreeListAggregator<
        MvdCoding>
{
public:
    static const auto Id = ElementId::prediction_unit;

    typedef PredictionUnitContent::Coord Coord;
    typedef PredictionUnitContent::Width Width;
    typedef PredictionUnitContent::Height Height;
    typedef PredictionUnitContent::PartIdx PartIdx;
    typedef PredictionUnitContent::MergeIdx MergeIdx;
    typedef PredictionUnitContent::MergeFlag MergeFlag;
    typedef PredictionUnitContent::InterPredIdcDebinarizer InterPredIdcDebinarizer;
    typedef PredictionUnitContent::InterPredIdc InterPredIdc;
    typedef PredictionUnitContent::RefIdxL0 RefIdxL0;
    typedef PredictionUnitContent::RefIdxL1 RefIdxL1;
    typedef PredictionUnitContent::MvpL0Flag MvpL0Flag;
    typedef PredictionUnitContent::MvpL1Flag MvpL1Flag;
    typedef PredictionUnitContent::MvLX MvLX;
    typedef PredictionUnitContent::MvCLX MvCLX;
    typedef PredictionUnitContent::PredFlagLX PredFlagLX;
    typedef PredictionUnitContent::RefIdxLX RefIdxLX;
    typedef PredictionUnitContent::MvdLX MvdLX;

    PredictionUnit(PelCoord puCoord, Pel puWidth, Pel puHeight, int partIdx)
    {
        embed<Coord>(*this, puCoord);
        embed<Width>(*this, puWidth);
        embed<Height>(*this, puHeight);
        embed<PartIdx>(*this, partIdx);
    }

    bool encloses(PelCoord at) const
    {
        return
            PelRect
            {
                get<Coord>()->inUnits(),
                get<Width>()->inUnits(),
                get<Height>()->inUnits()
            }.encloses(at);
    }

    void onParse(StreamAccessLayer &, Decoder::State &);
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */

#endif /* HEVC_Syntax_PredictionUnit_h */
