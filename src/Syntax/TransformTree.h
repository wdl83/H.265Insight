#ifndef HEVC_Syntax_TransformTree_h
#define HEVC_Syntax_TransformTree_h

#include <Syntax/Syntax.h>
#include <Syntax/SequenceParameterSet.h>
#include<Syntax/TransformUnit.h>

namespace HEVC { namespace Syntax { namespace TransformTreeContent {
/*----------------------------------------------------------------------------*/
typedef EmbeddedCoord<Pel, ElementId::TransformTreeCoord> Coord;
typedef EmbeddedCoord<Pel, ElementId::TransformTreeOffset> CuOffset;
typedef EmbeddedUnit<Log2, ElementId::TransformTreeSize> Size;
typedef EmbeddedUnit<int8_t, ElementId::TransformTreeDepth> Depth;
typedef EmbeddedUnit<int8_t, ElementId::TransformTreeBlkIdx> BlkIdx;
/*----------------------------------------------------------------------------*/
class InterSplitFlag: public Embedded
{
private:
    bool m_interSplitFlag;
public:
    static const auto Id = ElementId::interSplitFlag;


    InterSplitFlag(
            const SequenceParameterSet::MaxTransformHierarchyDepthInter &maxTransformHierarchyDepthInter,
            const PredictionMode cuPredMode,
            const PartitionMode partMode,
            const Depth &depth):
        m_interSplitFlag(
                0 == maxTransformHierarchyDepthInter
                && PredictionMode::Inter == cuPredMode
                && PartitionMode::PART_2Nx2N != partMode
                && 0 == depth.inUnits())
    {}

    explicit operator bool () const
    {
        return m_interSplitFlag;
    }
};
/*----------------------------------------------------------------------------*/
class SplitTransformFlag:
    public Embedded,
    public CABAD::FixedLength
{
public:
    static const auto Id = ElementId::split_transform_flag;

    SplitTransformFlag(
            const Size &size,
            Log2 maxSize,
            const Depth &depth,
            bool intraSplitFlag,
            const InterSplitFlag &interSplitFlag):
        /* 10v34, 9.3.3.1, Table 9-32 */
        CABAD::FixedLength{1}
    {
        setValue(
                size.inUnits() > maxSize
                || intraSplitFlag && 0 == depth.inUnits()
                || interSplitFlag ? true : false);
    }

    explicit operator bool () const
    {
        return getValue();
    }

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
            Log2 size)
    {
        const auto contextModel =
            [size](CABAD::State &state, int binIdx) -> CABAD::Variable &
        {
            syntaxCheck(0 == binIdx);
            syntaxCheck(5_log2 >= size);
            return
                state.getVariable(
                    CABAD::CtxId::split_transform_flag,
                    toUnderlying(5_log2 - size));
        };

        getFrom(streamAccessLayer, decoder, cabadState(decoder), *this, contextModel);
    }
};
/*----------------------------------------------------------------------------*/
class CbfCb:
    public Embedded,
    public CABAD::FixedLength
{
public:
    static const auto Id = ElementId::cbf_cb;

    CbfCb():
        /* 10v34, 9.3.3.1, Table 9-32 */
        CABAD::FixedLength{1}
    {}

    explicit operator bool () const
    {
        return getValue();
    }

    void onParse(
            StreamAccessLayer &, Decoder::State &,
            CodingTreeUnit &, PelCoord, Log2, int depth);
};
/*----------------------------------------------------------------------------*/
class CbfCr:
    public Embedded,
    public CABAD::FixedLength
{
public:
    static const auto Id = ElementId::cbf_cr;

    CbfCr():
        /* 10v34, 9.3.3.1, Table 9-32 */
        CABAD::FixedLength{1}
    {}

    explicit operator bool () const
    {
        return getValue();
    }

    void onParse(
            StreamAccessLayer &, Decoder::State &,
            CodingTreeUnit &, PelCoord, Log2, int depth);
};
/*----------------------------------------------------------------------------*/
class CbfLuma:
    public Embedded,
    public CABAD::FixedLength
{
public:
    static const auto Id = ElementId::cbf_luma;

    CbfLuma():
        /* 10v34, 9.3.3.1, Table 9-32 */
        CABAD::FixedLength{1}
    {}

    explicit operator bool () const
    {
        return getValue();
    }

    void onParse(
            StreamAccessLayer &, Decoder::State &,
            CodingTreeUnit &, PelCoord, Log2, int depth);
};
/*----------------------------------------------------------------------------*/
} /* TransformTreeContent */

class TransformTree:
    public EmbeddedAggregator<
        TransformTreeContent::Coord,
        TransformTreeContent::CuOffset,
        TransformTreeContent::Size,
        TransformTreeContent::Depth,
        TransformTreeContent::BlkIdx,
        TransformTreeContent::InterSplitFlag,
        TransformTreeContent::SplitTransformFlag,
        TransformTreeContent::CbfCb,
        TransformTreeContent::CbfCr,
        TransformTreeContent::CbfLuma>,
    public SubtreeAggregator<
        TransformUnit>
{
public:
    static const auto Id = ElementId::transform_tree;

    typedef TransformTreeContent::Coord Coord;
    typedef TransformTreeContent::CuOffset CuOffset;
    typedef TransformTreeContent::Size Size;
    typedef TransformTreeContent::Depth Depth;
    typedef TransformTreeContent::BlkIdx BlkIdx;
    typedef TransformTreeContent::InterSplitFlag InterSplitFlag;
    typedef TransformTreeContent::SplitTransformFlag SplitTransformFlag;
    typedef TransformTreeContent::CbfCb CbfCb;
    typedef TransformTreeContent::CbfCr CbfCr;
    typedef TransformTreeContent::CbfLuma CbfLuma;

    TransformTree(
            PelCoord ttCoord, PelCoord cuOffset,
            Log2 size,
            int depth, int blkIdx)
    {
        embed<Coord>(*this, ttCoord);
        embed<CuOffset>(*this, cuOffset);
        embed<Size>(*this, size);
        embed<Depth>(*this, depth);
        embed<BlkIdx>(*this, blkIdx);
    }

    Ptr<TransformUnit> getTransformUnit() const
    {
        return
            *get<SplitTransformFlag>()
            ? nullptr
            : Ptr<TransformUnit>{getSubtree<TransformUnit>()};
    }

    void onParse(
            StreamAccessLayer &, Decoder::State &decoder,
            const SliceSegmentHeader &,
            CodingTreeUnit &,
            CodingUnit &,
            PelCoord,
            bool = false, bool = false);
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */

#endif /* HEVC_Syntax_TransformTree_h */
