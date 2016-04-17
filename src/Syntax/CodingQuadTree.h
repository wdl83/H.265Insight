#ifndef HEVC_Syntax_CodingQuadTree_h
#define HEVC_Syntax_CodingQuadTree_h

#include <Syntax/Syntax.h>
#include <Syntax/CodingUnit.h>

namespace HEVC { namespace Syntax { namespace CodingQuadTreeContent {
/*----------------------------------------------------------------------------*/
typedef EmbeddedCoord<Pel, ElementId::CodingQuadTreeCoord> Coord;
typedef EmbeddedCoord<Pel, ElementId::CodingQuadTreeOffset> Offset;
typedef EmbeddedUnit<Log2, ElementId::CodingQuadTreeSize> Size;
typedef EmbeddedUnit<int8_t, ElementId::CodingQuadTreeDepth> Depth;
/*----------------------------------------------------------------------------*/
class SplitCuFlag:
    public Embedded,
    public CABAD::FixedLength
{
public:
    static const auto Id = ElementId::split_cu_flag;

    SplitCuFlag(
            Log2 minCbSizeY,
            const Size &size):
        /* Draft 10v23, Table 9-32 */
        CABAD::FixedLength{1}
    {
        setValue(size.inUnits() > minCbSizeY);
    }

    explicit operator bool () const
    {
        return getValue();
    }

    void onParse(
            StreamAccessLayer &, Decoder::State &decoder,
            PelCoord cqtCoord, int cqtDepth);
};
/*----------------------------------------------------------------------------*/
} /* CodingQuadTreeContent */

class CodingQuadTree:
    public EmbeddedAggregator<
        CodingQuadTreeContent::Coord,
        CodingQuadTreeContent::Offset,
        CodingQuadTreeContent::Size,
        CodingQuadTreeContent::Depth,
        CodingQuadTreeContent::SplitCuFlag>,
    public SubtreeAggregator<
        CodingUnit>
{
public:
    static const auto Id = ElementId::coding_quadtree;

    typedef CodingQuadTreeContent::Coord Coord;
    typedef CodingQuadTreeContent::Offset Offset;
    typedef CodingQuadTreeContent::Size Size;
    typedef CodingQuadTreeContent::Depth Depth;
    typedef CodingQuadTreeContent::SplitCuFlag SplitCuFlag;

    CodingQuadTree(
            PelCoord cqtCoord, PelCoord cqtOffset, Log2 cqtSize, int cqtDepth)
    {
        embed<Coord>(*this, cqtCoord);
        embed<Offset>(*this, cqtOffset);
        embed<Depth>(*this, cqtDepth);
        embed<Size>(*this, cqtSize);
    }

    Ptr<CodingUnit> getCodingUnit() const
    {
        return
            *get<SplitCuFlag>()
            ? nullptr
            : Ptr<CodingUnit>{getSubtree<CodingUnit>()};
    }

    void onParse(
            StreamAccessLayer &, Decoder::State &decoder,
            const SliceSegmentHeader &,
            CodingTreeUnit &);
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */

#endif /* HEVC_Syntax_CodingQuadTree_h */
