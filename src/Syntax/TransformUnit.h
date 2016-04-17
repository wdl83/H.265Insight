#ifndef HEVC_Syntax_TransformUnit_h
#define HEVC_Syntax_TransformUnit_h

#include <Syntax/Syntax.h>
#include <Syntax/PpsRangeExtension.h>
#include <Syntax/CrossCompPred.h>
#include <Syntax/ResidualCoding.h>

namespace HEVC { namespace Syntax { namespace TransformUnitContent {
/*----------------------------------------------------------------------------*/
typedef EmbeddedCoord<Pel, ElementId::TransformUnitCoord> Coord;
typedef EmbeddedCoord<Pel, ElementId::TransformUnitCuOffset> CuOffset;
typedef EmbeddedCoord<Pel, ElementId::TransformUnitRootCoord> RootCoord;
typedef EmbeddedUnit<Log2, ElementId::TransformUnitSize> Size;
typedef EmbeddedUnit<int8_t, ElementId::TransformUnitDepth> Depth;
typedef EmbeddedUnit<int8_t, ElementId::TransformUnitBlkIdx> BlkIdx;
/*----------------------------------------------------------------------------*/
class CuQpDeltaAbsDebinarizer: public CABAD::Debinarizer
{
public:
    static const auto debinarizerId = CABAD::DebinarizerId::Special;

    struct Binarization
    {
        CABAD::BinString prefix;
        CABAD::BinString suffix;
        CABAD::BinValueType prefixValue;

        Binarization(
                CABAD::BinString prefix_, CABAD::BinString suffix_,
                CABAD::BinValueType prefixValue_):
            prefix(prefix_), suffix(suffix_),
            prefixValue(prefixValue_)
        {}
    };

    static
    Binarization getBinarization(
            StreamAccessLayer &streamAccessLayer, CABAD::State &state)
    {
        const auto contextModel =
            [](CABAD::State &s, int binIdx) -> CABAD::Variable *
            {
                return
                    5 > binIdx
                    ? &s.getVariable(CABAD::CtxId::cu_qp_delta_abs, 0 == binIdx ? 0 : 1)
                    : nullptr;
            };

        /* 04/2013, 9.3.3.8, "Binarization process for cu_qp_delta_abs"
         *  prefixVal = Min(cu_qp_delta_abs, 5)
         *  prefixVal bin string is specified by invoking TR binarization with
         *   cMax = 5 and cRiceParam = 0 */
        const auto cMax = 5;
        const auto cRiceParam = 0;
        const auto prefix =
            CABAD::TruncatedRiceDebinarizer::getBinarization(
                    streamAccessLayer, state,
                    cRiceParam, cMax,
                    contextModel);

        const auto prefixValue =
            CABAD::TruncatedRice::getBinarizationValue(prefix, cRiceParam, cMax);

        /* when prefix is greater then 4, the suffix bin string is present */
        if(4 < prefixValue)
        {
            /* 04/2013, 9.3.3.8, "Binarization process for cu_qp_delta_abs"
             * suffixVal = cu_qp_delta_abs - 5
             * suffixVal bin string is specified by invoking EGk binarization with
             * k = 0 */
            const auto codeOrder = 0;
            const auto suffix =
                CABAD::ExpGolombDebinarizer::getBinarization(
                        streamAccessLayer, state,
                        codeOrder);

            return {prefix, suffix, prefixValue};
        }

        return {prefix, CABAD::BinString{}, prefixValue};
    }

    CABAD::BinValueType getBinarizationValue(
            StreamAccessLayer &streamAccessLayer, CABAD::State &state)
    {
        const auto binarization =
            getBinarization(streamAccessLayer, state);

        m_binarization = join(binarization.prefix, binarization.suffix);

        if(4 < binarization.prefixValue)
        {
            const auto codeOrder = 0;
            const auto suffixValue =
                CABAD::ExpGolombDebinarizer::getBinarizationValue(
                        binarization.suffix, codeOrder);

            return binarization.prefixValue + suffixValue;
        }

        return binarization.prefixValue;
    }
};

struct CuQpDeltaAbs:
    public Embedded,
    public CABAD::ContextAdaptive<CuQpDeltaAbsDebinarizer>
{
    static const auto Id = ElementId::cu_qp_delta_abs;

    int inUnits() const
    {
        return getValue();
    }
};

class CuQpDeltaSignFlag:
    public Embedded,
    public CABAD::FixedLength
{
public:
    static const auto Id = ElementId::cu_qp_delta_sign_flag;

    CuQpDeltaSignFlag():
        CABAD::FixedLength{1}
    {
        /* inferred */
        setValue(false);
    }

    explicit operator bool () const
    {
        return getValue();
    }
};

struct CuChromaQpOffsetFlag:
    public Embedded,
    public CABAD::FixedLength
{
    static const auto Id = ElementId::cu_chroma_qp_offset_flag;

    CuChromaQpOffsetFlag():
        /* 10/2014, 9.3.3 "Binarization process", Table 9-38 */
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
                return state.getVariable(CABAD::CtxId::cu_chroma_qp_offset_flag, 0);
            };

        getFrom(streamAccessLayer, decoder, cabadState(decoder), *this, contextModel);
    }
};

struct CuChromaQpOffsetIdx:
    public Embedded,
    public CABAD::TruncatedRice
{
    static const auto Id = ElementId::cu_chroma_qp_offset_idx;

    CuChromaQpOffsetIdx(const PpsRangeExtension::ChromaQpOffsetListLenMinus1 &listLenMinus1):
        /* 10/2014, 9.3.3 "Binarization process", Table 9-38 */
        CABAD::TruncatedRice{0, listLenMinus1.inUnits()}
    {
        setValue(0);
    }

    int inUnits() const
    {
        return getValue();
    }

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder)
    {
        const auto contextModel =
            [](CABAD::State &state, int binIdx) -> CABAD::Variable &
            {
                syntaxCheck(0 <= binIdx);
                syntaxCheck(5 > binIdx);
                return state.getVariable(CABAD::CtxId::cu_chroma_qp_offset_idx, 0);
            };

        getFrom(streamAccessLayer, decoder, cabadState(decoder), *this, contextModel);
    }
};

typedef SubtreeArray<CrossCompPred, EnumRange<Chroma>::length()> CrossCompPredArray;

/* Residual Coding Index */
enum class RCI
{
    Begin,
    Y = Begin,
    /* 422 uses Cx0/1 */
    Cb, Cb0 = Cb, Cb1,
    Cr, Cr0 = Cr, Cr1,
    End
};

typedef SubtreeArray<ResidualCoding, EnumRange<RCI>::length()> ResidualCodingArray;
/*----------------------------------------------------------------------------*/
} /* TransformUnitContent */

class TransformUnit:
    public EmbeddedAggregator<
        TransformUnitContent::Coord,
        TransformUnitContent::CuOffset,
        TransformUnitContent::RootCoord,
        TransformUnitContent::Size,
        TransformUnitContent::Depth,
        TransformUnitContent::BlkIdx,
        TransformUnitContent::CuQpDeltaAbs,
        TransformUnitContent::CuQpDeltaSignFlag,
        TransformUnitContent::CuChromaQpOffsetFlag,
        TransformUnitContent::CuChromaQpOffsetIdx>,
    public SubtreeArrayAggregator<
        TransformUnitContent::CrossCompPredArray,
        TransformUnitContent::ResidualCodingArray>
{
public:
    static const auto Id = ElementId::transform_unit;

    typedef TransformUnitContent::Coord Coord;
    typedef TransformUnitContent::CuOffset CuOffset;
    typedef TransformUnitContent::RootCoord RootCoord;
    typedef TransformUnitContent::Size Size;
    typedef TransformUnitContent::Depth Depth;
    typedef TransformUnitContent::BlkIdx BlkIdx;
    typedef TransformUnitContent::CuQpDeltaAbs CuQpDeltaAbs;
    typedef TransformUnitContent::CuQpDeltaSignFlag CuQpDeltaSignFlag;
    typedef TransformUnitContent::CuChromaQpOffsetFlag CuChromaQpOffsetFlag;
    typedef TransformUnitContent::CuChromaQpOffsetIdx CuChromaQpOffsetIdx;
    typedef TransformUnitContent::CrossCompPredArray CrossCompPredArray;
    typedef TransformUnitContent::ResidualCodingArray ResidualCodingArray;
    typedef TransformUnitContent::RCI RCI;

    TransformUnit(
        PelCoord tuCoord, PelCoord cuOffset, PelCoord rootCoord,
        Log2 trafoSize,
        int depth, int blkIdx)
    {
        embed<Coord>(*this, tuCoord);
        embed<CuOffset>(*this, cuOffset);
        embed<RootCoord>(*this, rootCoord);
        embed<Size>(*this, trafoSize);
        embed<Depth>(*this, depth);
        embed<BlkIdx>(*this, blkIdx);
    }

    PelRect boundaries() const
    {
        return {get<Coord>()->inUnits(), toPel(get<Size>()->inUnits())};
    };

    int resScaleVal(Chroma chroma) const
    {
        if(Chroma::Cb == chroma && subtree<CrossCompPredArray>(int(Chroma::Cb)))
        {
            return  subtree<CrossCompPredArray>(int(Chroma::Cb))->resScaleVal();
        }
        else if(Chroma::Cr == chroma && subtree<CrossCompPredArray>(int(Chroma::Cr)))
        {
            return  subtree<CrossCompPredArray>(int(Chroma::Cr))->resScaleVal();
        }

        return 0;
    }

    void onParse(
            StreamAccessLayer &, Decoder::State &,
            const SliceSegmentHeader &,
            const CodingTreeUnit &,
            const CodingUnit &);
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */

#endif /* HEVC_Syntax_TransformUnit_h */
