#ifndef HEVC_Syntax_CodingUnit_h
#define HEVC_Syntax_CodingUnit_h
/* HEVC */
#include <Syntax/Syntax.h>
#include <Syntax/SequenceParameterSet.h>
#include <Syntax/SliceSegmentHeader.h>
#include <Syntax/PcmSample.h>
#include <Syntax/PredictionUnit.h>
#include <Syntax/TransformTree.h>
#include <Syntax/QuadTree.h>
#include <Optional.h>

namespace HEVC { namespace Syntax { namespace CodingUnitContent {
/*----------------------------------------------------------------------------*/
typedef EmbeddedCoord<Pel, ElementId::CodingUnitCoord> Coord;
typedef EmbeddedUnit<Log2, ElementId::CodingUnitSize> Size;
/*----------------------------------------------------------------------------*/
class CuTransquantBypassFlag:
    public Embedded,
    public CABAD::FixedLength
{
public:
    static const auto Id = ElementId::cu_transquant_bypass_flag;

    CuTransquantBypassFlag():
        CABAD::FixedLength{1 /* cMax Draft 10v23, Table 9-32 */}
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
                return state.getVariable(CABAD::CtxId::cu_transquant_bypass_flag, 0);
            };

        getFrom(streamAccessLayer, decoder, cabadState(decoder), *this, contextModel);
    }
};
/*----------------------------------------------------------------------------*/
class CuSkipFlag:
    public Embedded,
    public CABAD::FixedLength
{
public:
    static const auto Id = ElementId::cu_skip_flag;

    CuSkipFlag():
        /* cMax 04/2013, Table 9-32 */
        CABAD::FixedLength{1}
    {
        /* inferred */
        setValue(false);
    }

    explicit operator bool () const
    {
        return getValue();
    }

    void onParse(StreamAccessLayer &, Decoder::State &, PelCoord);
};
/*----------------------------------------------------------------------------*/
class PredModeFlag:
    public Embedded,
    public CABAD::FixedLength
{
public:
    static const auto Id = ElementId::pred_mode_flag;

    PredModeFlag():
        /* cMax Draft 10v23, Table 9-32 */
        CABAD::FixedLength{1}
    {}

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
                /* 10v34, 9.3.4.2.1 Table 9-37 */
                return state.getVariable(CABAD::CtxId::pred_mode_flag, 0);
            };

        getFrom(streamAccessLayer, decoder, cabadState(decoder), *this, contextModel);
    }
};
/*----------------------------------------------------------------------------*/
class CuPredMode: public Embedded
{
public:
    static const auto Id = ElementId::CuPredMode;
private:
    Optional<PredictionMode> m_mode;
public:
    /* TODO: is it possible to set value during contruction? */
    operator PredictionMode () const
    {
        return *m_mode;
    }

    void set(
            const SliceSegmentHeader::SliceType &sliceType,
            const CuSkipFlag &cuSkipFlag);

    void set(const PredModeFlag &predModeFlag);

    void onSet() const
    {
        log(LogId::Prediction, getName(*m_mode), '\n');
    }
};
/*----------------------------------------------------------------------------*/
class PartModeDebinarizer: public CABAD::Debinarizer
{
public:
    static const auto debinarizerId = CABAD::DebinarizerId::Special;

    static CABAD::BinString getBinarization(
            StreamAccessLayer &streamAccessLayer,
            CABAD::State &state,
            PredictionMode cuPredMode,
            Log2 minCbSizeY,
            bool ampEnabledFlag,
            Log2 size);

    static CABAD::BinValueType getBinarizationValue(CABAD::BinString binarization)
    {
        /* Draft 9v13 9.2.2.7 Table 9-34 */
        static const std::map<CABAD::BinString, CABAD::BinValueType> values =
        {
            {{0}, 1},
            {{1}, 0},
            {{0, 0}, 2},
            {{0, 1}, 1},
            {{0, 0, 0}, 3},
            {{0, 0, 1}, 2},
            {{0, 1, 1}, 1},
            {{0, 0, 0, 0}, 6},
            {{0, 0, 0, 1}, 7},
            {{0, 1, 0, 0}, 4},
            {{0, 1, 0, 1}, 5}
        };

        syntaxCheck(values.count(binarization));
        return values.at(binarization);
    }

    CABAD::BinValueType getBinarizationValue(
            StreamAccessLayer &streamAccessLayer,
            CABAD::State &state,
            PredictionMode cuPredMode,
            Log2 minCbSizeY,
            bool ampEnabledFlag,
            Log2 size)
    {
        m_binarization =
            getBinarization(
                    streamAccessLayer,
                    state,
                    cuPredMode, minCbSizeY, ampEnabledFlag, size);
        return getBinarizationValue(m_binarization);
    }
};
/*----------------------------------------------------------------------------*/
class PartMode:
    public Embedded,
    public CABAD::ContextAdaptive<PartModeDebinarizer>
{
public:
    static const auto Id = ElementId::part_mode;

    operator int () const
    {
        return getValue();
    }

    void onParse(
            StreamAccessLayer &streamAccessLayer,
            Decoder::State &decoder,
            PredictionMode cuPredMode,
            Log2 minCbSizeY,
            bool ampEnabledFlag,
            Log2 size)
    {
        getFrom(
                streamAccessLayer, decoder, cabadState(decoder), *this,
                cuPredMode, minCbSizeY, ampEnabledFlag, size);
    }
};
/*----------------------------------------------------------------------------*/
class PartModePseudo: public Embedded
{
    friend class CodingUnit;
public:
    static const auto Id = ElementId::PartMode;
private:
    Optional<PartitionMode> m_mode;
public:
    PartModePseudo(): m_mode{PartitionMode::PART_2Nx2N}
    {}

    operator PartitionMode() const
    {
        return *m_mode;
    }

    PartitionMode get() const
    {
        return *m_mode;
    }

    void set()
    {
        m_mode = PartitionMode::PART_2Nx2N;
        onSet();
    }

    void set(const CuPredMode &cuPredMode, const PartMode &partMode)
    {
        /* 04/2013, 7.4.9.5, "Coding unit semantics", Table 7-10 */
        if(PredictionMode::Intra == cuPredMode)
        {
            const PartitionMode modes[] =
            {
                PartitionMode::PART_2Nx2N,
                PartitionMode::PART_NxN
            };

            syntaxCheck(int(lengthOf(modes)) > partMode);
            m_mode = modes[partMode];
        }
        else if(PredictionMode::Inter == cuPredMode)
        {
            const PartitionMode modes[] =
            {
                PartitionMode::PART_2Nx2N,
                PartitionMode::PART_2NxN,
                PartitionMode::PART_Nx2N,
                PartitionMode::PART_NxN,
                PartitionMode::PART_2NxnU,
                PartitionMode::PART_2NxnD,
                PartitionMode::PART_nLx2N,
                PartitionMode::PART_nRx2N
            };

            syntaxCheck(int(lengthOf(modes)) > partMode);
            m_mode = modes[partMode];
        }

        onSet();
    }

    void onSet() const
    {
        log(LogId::Prediction, getName(*m_mode), '\n');
    }
};
/*----------------------------------------------------------------------------*/
class PcmFlag:
    public Embedded,
    public CABAD::FixedLength
{
private:
    CABAD::NonAdaptiveVariable m_nonAdaptive;
public:
    static const auto Id = ElementId::pcm_flag;

    PcmFlag():
        /* 04/2013, Table 9-32 */
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
            [this](CABAD::State &, int binIdx) -> CABAD::Variable &
            {
                syntaxCheck(0 == binIdx);
                return m_nonAdaptive;
            };

        getFrom(streamAccessLayer, decoder, cabadState(decoder), *this, contextModel);
    }
};
/*----------------------------------------------------------------------------*/
class PcmAlignmentZeroBit:
    public Embedded,
    public VLD::FixedPattern<0>
{
public:
    static const auto Id = ElementId::pcm_alignment_zero_bit;
};
/*----------------------------------------------------------------------------*/
inline
int toCuIntraPartNo(PelCoord cuOffset, Log2 cuSize)
{
    const auto no =
        2 * (toPel(cuSize - 1_log2) <= cuOffset.y())
        + (toPel(cuSize - 1_log2) <= cuOffset.x());

    return no;
}

template <typename T>
class CuIntraPart
{
protected:
    std::array<T, 4> m_part;

    CuIntraPart(T value)
    {
        fill(m_part, value);
    }
public:
    T &operator[] (Tuple<PelCoord, Log2> at)
    {
        return m_part[toCuIntraPartNo(at.get<PelCoord>(), at.get<Log2>())];
    }

    T operator[] (Tuple<PelCoord, Log2> at) const
    {
        return m_part[toCuIntraPartNo(at.get<PelCoord>(), at.get<Log2>())];
    }
};
/*----------------------------------------------------------------------------*/
class PrevIntraLumaPredFlag:
    public CuIntraPart<bool>,
    public Embedded,
    public CABAD::FixedLength
{
public:
    static const auto Id = ElementId::prev_intra_luma_pred_flag;

    PrevIntraLumaPredFlag():
        CuIntraPart<bool>{false},
        /* Draft 10v34, 9.3.3.1 Table 9-32 */
        CABAD::FixedLength{1}
    {}

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
            PelCoord at, Log2 cuSize)
    {
        const auto contextModel =
            [](CABAD::State &state, int binIdx) -> CABAD::Variable &
            {
                syntaxCheck(0 == binIdx);
                return state.getVariable(CABAD::CtxId::prev_intra_luma_pred_flag, 0);
            };

        getFrom(streamAccessLayer, decoder, cabadState(decoder), *this, contextModel);
        (*this)[makeTuple(at, cuSize)] = getValue();
    }
};
/*----------------------------------------------------------------------------*/
class MpmIdx:
    public CuIntraPart<int>,
    public Embedded,
    public CABAD::TruncatedRice
{
public:
    static const auto Id = ElementId::mpm_idx;

    MpmIdx():
        CuIntraPart<int>{0},
        /* Draft 10v23, Table 9-32 */
        CABAD::TruncatedRice(0, 2)
    {}

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
            PelCoord at, Log2 cuSize)
    {
        getFrom(streamAccessLayer, decoder, cabadState(decoder), *this);
        (*this)[makeTuple(at, cuSize)] = getValue();
    }
};
/*----------------------------------------------------------------------------*/
class RemIntraLumaPredMode:
    public CuIntraPart<IntraPredictionMode>,
    public Embedded,
    public CABAD::FixedLength
{
public:
    static const auto Id = ElementId::rem_intra_luma_pred_mode;

    RemIntraLumaPredMode():
        CuIntraPart<IntraPredictionMode>{IntraPredictionMode::Undefined},
        /* Draft 10v23, Table 9-32 */
        CABAD::FixedLength{31}
    {}

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
            PelCoord at, Log2 cuSize)
    {
        getFrom(streamAccessLayer, decoder, cabadState(decoder), *this);
        (*this)[makeTuple(at, cuSize)] = static_cast<IntraPredictionMode>(getValue());
    }
};
/*----------------------------------------------------------------------------*/
class IntraChromaPredModeDebinarizer: public CABAD::Debinarizer
{
public:
    static const auto debinarizerId = CABAD::DebinarizerId::Special;

    static CABAD::BinString getBinarization(
            StreamAccessLayer &streamAccessLayer,
            CABAD::State &state);

    static CABAD::BinValueType getBinarizationValue(CABAD::BinString binarization)
    {
        /* Draft 9v13 9.2.2.9 Table 9-35 */
        static const std::map<CABAD::BinString, CABAD::BinValueType> values =
        {
            {{0}, 4},
            {{1, 0, 0}, 0},
            {{1, 0, 1}, 1},
            {{1, 1, 0}, 2},
            {{1, 1, 1}, 3}
        };

        syntaxCheck(values.count(binarization));
        return values.at(binarization);
    }

    CABAD::BinValueType getBinarizationValue(
            StreamAccessLayer &streamAccessLayer,
            CABAD::State &state)
    {
        m_binarization = getBinarization(streamAccessLayer, state);
        return getBinarizationValue(m_binarization);
    }
};
/*----------------------------------------------------------------------------*/
class IntraChromaPredMode:
    public CuIntraPart<int>,
    public Embedded,
    public CABAD::ContextAdaptive<IntraChromaPredModeDebinarizer>
{
public:
    static const auto Id = ElementId::intra_chroma_pred_mode;

    IntraChromaPredMode():
        CuIntraPart<int>{-1}
    {}

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
            PelCoord at, Log2 cuSize, bool isNxN)
    {
        getFrom(streamAccessLayer, decoder, cabadState(decoder), *this);
        (*this)[makeTuple(at, cuSize)] = getValue();

        if(!isNxN)
        {
            auto begin = std::begin(m_part);

            std::advance(begin, 1);
            std::fill(begin, std::end(m_part), *std::begin(m_part));
        }
    }
};
/*----------------------------------------------------------------------------*/
class RqtRootCbf:
    public Embedded,
    public CABAD::FixedLength
{
public:
    static const auto Id = ElementId::rqt_root_cbf;

    RqtRootCbf():
        /* 10v34, 9.3.3.1 Table 9-32 */
        CABAD::FixedLength{1}
    {
        /* inferred */
        setValue(true);
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
                return state.getVariable(CABAD::CtxId::rqt_root_cbf, 0);
            };

        getFrom(streamAccessLayer, decoder, cabadState(decoder), *this, contextModel);
    }
};
/*----------------------------------------------------------------------------*/
/* Pseudo Elements */
/*----------------------------------------------------------------------------*/
class IntraSplitFlag: public Embedded
{
    friend class CodingUnit;
private:
    bool m_intraSplitFlag;
public:
    static const auto Id = ElementId::IntraSplitFlag;

    IntraSplitFlag():
        m_intraSplitFlag(false)
    {}

    explicit operator bool () const
    {
        return m_intraSplitFlag;
    }

    void set(const CuPredMode &cuPredMode, const PartMode &partMode)
    {
        if(PredictionMode::Intra == cuPredMode && 1 == partMode)
        {
            m_intraSplitFlag = true;
        }
        else
        {
            m_intraSplitFlag = false;
        }
    }
};
/*----------------------------------------------------------------------------*/
class MaxTrafoDepth: public Embedded
{
private:
    int m_maxTrafoDepth;
public:
    static const auto Id = ElementId::MaxTrafoDepth;

    MaxTrafoDepth(
            const SequenceParameterSet::MaxTransformHierarchyDepthIntra &maxTransformHierarchyDepthIntra,
            const SequenceParameterSet::MaxTransformHierarchyDepthInter &maxTransformHierarchyDepthInter,
            const CuPredMode &cuPredMode,
            const IntraSplitFlag &intraSplitFlag):
        m_maxTrafoDepth(
                PredictionMode::Intra == cuPredMode
                ? maxTransformHierarchyDepthIntra + (intraSplitFlag ? 1 : 0)
                : maxTransformHierarchyDepthInter)
    {}

    int inUnits() const
    {
        return m_maxTrafoDepth;
    }
};
/*----------------------------------------------------------------------------*/
class IntraPredModeY: public Embedded
{
    CuIntraPredMode m_mode;
public:
    static const auto Id = ElementId::IntraPredModeY;

    IntraPredModeY(const CuIntraPredMode &mode): m_mode(mode)
    {}

    IntraPredictionMode operator[] (int i) const
    {
        return m_mode[i];
    }

    operator const CuIntraPredMode & () const
    {
        return m_mode;
    }
};
/*----------------------------------------------------------------------------*/
class IntraPredModeC: public Embedded
{
    CuIntraPredMode m_mode;
public:
    static const auto Id = ElementId::IntraPredModeC;

    IntraPredModeC(const CuIntraPredMode &mode): m_mode(mode)
    {}

    IntraPredictionMode operator[] (int i) const
    {
        return m_mode[i];
    }

    operator const CuIntraPredMode & () const
    {
        return m_mode;
    }
};
/*----------------------------------------------------------------------------*/
struct QpY: public Embedded
{
    const int qpY;
    const int qpPrimeY;

    static const auto Id = ElementId::QpY;

    QpY(int qpY_, int qpPrimeY_): qpY{qpY_}, qpPrimeY{qpPrimeY_}
    {
        log(LogId::QuantizationY, ' ', qpY, ' ', qpPrimeY, '\n');
    }
};

struct QpC: public Embedded
{
    const int qpPrimeCb;
    const int qpPrimeCr;

    static const auto Id = ElementId::QpC;

    QpC(int qpPrimeCb_, int qpPrimeCr_): qpPrimeCb{qpPrimeCb_}, qpPrimeCr{qpPrimeCr_}
    {
        log(LogId::QuantizationCb, ' ', qpPrimeCb, '\n');
        log(LogId::QuantizationCr, ' ', qpPrimeCr, '\n');
    }
};
/*----------------------------------------------------------------------------*/
} /* CodingUnitContent */

class CodingUnit:
    public EmbeddedAggregator<
        CodingUnitContent::Coord,
        CodingUnitContent::Size,
        CodingUnitContent::CuTransquantBypassFlag,
        CodingUnitContent::CuSkipFlag,
        CodingUnitContent::PredModeFlag,
        CodingUnitContent::CuPredMode,
        CodingUnitContent::PartModeDebinarizer,
        CodingUnitContent::PartMode,
        CodingUnitContent::PartModePseudo,
        CodingUnitContent::PcmFlag,
        CodingUnitContent::PcmAlignmentZeroBit,
        CodingUnitContent::PrevIntraLumaPredFlag,
        CodingUnitContent::MpmIdx,
        CodingUnitContent::RemIntraLumaPredMode,
        CodingUnitContent::IntraChromaPredModeDebinarizer,
        CodingUnitContent::IntraChromaPredMode,
        CodingUnitContent::RqtRootCbf,
        CodingUnitContent::IntraSplitFlag,
        CodingUnitContent::MaxTrafoDepth,
        CodingUnitContent::IntraPredModeY,
        CodingUnitContent::IntraPredModeC,
        CodingUnitContent::QpY,
        CodingUnitContent::QpC>,
    public SubtreeAggregator<
        PcmSample>,
    public SubtreeListAggregator<
        PredictionUnit>
{
    friend class TransformTree;
private:
    QuadTree<TransformTree> m_tt;
protected:
    Ptr<TransformTree> embedTransformTree(
            PelCoord ttCoord, PelCoord ttOffset,
            Log2 trafoSize,
            int trafoDepth, int blkIdx)
    {
        const auto offset = ttCoord - get<Coord>()->inUnits();
        return m_tt.append(offset, ttCoord, ttOffset, trafoSize, trafoDepth, blkIdx);
    }
public:
    static const auto Id = ElementId::coding_unit;

    typedef CodingUnitContent::Coord Coord;
    typedef CodingUnitContent::Size Size;
    typedef CodingUnitContent::CuTransquantBypassFlag CuTransquantBypassFlag;
    typedef CodingUnitContent::CuSkipFlag CuSkipFlag;
    typedef CodingUnitContent::PredModeFlag PredModeFlag;
    typedef CodingUnitContent::CuPredMode CuPredMode;
    typedef CodingUnitContent::PartModeDebinarizer PartModeDebinarizer;
    typedef CodingUnitContent::PartMode PartMode;
    typedef CodingUnitContent::PartModePseudo PartModePseudo;
    typedef CodingUnitContent::PcmFlag PcmFlag;
    typedef CodingUnitContent::PcmAlignmentZeroBit PcmAlignmentZeroBit;
    typedef CodingUnitContent::PrevIntraLumaPredFlag PrevIntraLumaPredFlag;
    typedef CodingUnitContent::MpmIdx MpmIdx;
    typedef CodingUnitContent::RemIntraLumaPredMode RemIntraLumaPredMode;
    typedef CodingUnitContent::IntraChromaPredModeDebinarizer IntraChromaPredModeDebinarizer;
    typedef CodingUnitContent::IntraChromaPredMode IntraChromaPredMode;
    typedef CodingUnitContent::RqtRootCbf RqtRootCbf;
    typedef CodingUnitContent::IntraSplitFlag IntraSplitFlag;
    typedef CodingUnitContent::MaxTrafoDepth MaxTrafoDepth;
    typedef CodingUnitContent::IntraPredModeY IntraPredModeY;
    typedef CodingUnitContent::IntraPredModeC IntraPredModeC;
    typedef CodingUnitContent::QpY QpY;
    typedef CodingUnitContent::QpC QpC;

    CodingUnit(Log2 minTrafoSize, PelCoord cuCoord, Log2 cuSize):
        m_tt({minTrafoSize, cuSize + 1_log2})
    {
        embed<Coord>(*this, cuCoord);
        embed<Size>(*this, cuSize);

        log(LogId::QuantizationY, cuCoord);
        log(LogId::QuantizationCb, cuCoord);
        log(LogId::QuantizationCr, cuCoord);
    }

    bool encloses(PelCoord coord) const
    {
        return
            PelRect
            {
                get<Coord>()->inUnits(),
                toPel(get<Size>()->inUnits())
            }.encloses(coord);
    }

    IntraPredictionMode intraPredictionMode(Plane plane, PelCoord at) const
    {
        syntaxCheck(encloses(at));

        const auto coord = get<Coord>()->inUnits();
        const auto side = toPel(get<Size>()->inUnits() - 1_log2);

        syntaxCheck(0_pel < side);

        const auto offset = at - coord;
        const int i = toUnderlying(2 * (offset.y() / side) + offset.x() / side);

        return
            Plane::Y == plane
            ? (*get<IntraPredModeY>())[i]
            : (*get<IntraPredModeC>())[i];
    }

    bool isSkipped() const
    {
        return bool(*get<CuSkipFlag>());
    }

    bool isCuTransQuantBypass() const
    {
        return bool(*get<CuTransquantBypassFlag>());
    }

    bool isPCM() const
    {
        return bool(*get<PcmFlag>());
    }

    bool enclosesTransformTree() const
    {
        return
            !isSkipped()
            && !isPCM()
            && bool(*get<RqtRootCbf>());
    }

    Ptr<PredictionUnit> getPredictionUnit(PelCoord at) const
    {
        const auto &puList = getSubtreeList<PredictionUnit>();
        const auto i =
            std::find_if(
                    std::begin(puList), std::end(puList),
                    [at](const Handle<PredictionUnit> &pu){return pu->encloses(at);});

        syntaxCheck(std::end(puList) != i);
        return Ptr<PredictionUnit>{*i};
    }

    Ptr<TransformTree> getTransformTree(PelCoord at) const
    {
        const auto offset = at - get<Coord>()->inUnits();
        return m_tt[offset];
    }

    VLA<Ptr<TransformTree>> getTransformTreeListInZ() const
    {
        return m_tt.toListInZ();
    }

    void onParse(
            StreamAccessLayer &, Decoder::State &,
            const SliceSegmentHeader &, CodingTreeUnit &);
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */

#endif /* HEVC_Syntax_CodingUnit_h */
