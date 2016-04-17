#ifndef HEVC_Syntax_CodingTreeUnit_h
#define HEVC_Syntax_CodingTreeUnit_h

/* STDC++ */
#include <array>
/* HEVC */
#include <Syntax/Syntax.h>
#include <Syntax/SAO.h>
#include <Syntax/CodingQuadTree.h>
#include <Syntax/QuadTree.h>
#include <BitMatrix.h>

namespace HEVC { namespace Syntax { namespace CodingTreeUnitContent {
/*----------------------------------------------------------------------------*/
typedef EmbeddedCoord<Pel, ElementId::CodingTreeUnitCoord> Coord;
typedef EmbeddedUnit<Ctb, ElementId::CtbAddrInRs> CtbAddrInRs;
typedef EmbeddedUnit<Ctb, ElementId::CtbAddrInTs> CtbAddrInTs;
/*----------------------------------------------------------------------------*/
} /* CodingTreeUnitContent */

class CodingTreeUnit:
    public EmbeddedAggregator<
        CodingTreeUnitContent::Coord,
        CodingTreeUnitContent::CtbAddrInTs,
        CodingTreeUnitContent::CtbAddrInRs>,
    public SubtreeAggregator<
        SAO>
{
    friend class CodingQuadTree;

    typedef
        BitMatrix<
            Limits::CodingBlock::max / Limits::TrafoSize::min,
            Limits::CodingBlock::max / Limits::TrafoSize::min> CbFlag;

    static
    CbFlag::Pos toCbFlagPos(PelCoord offset)
    {
        const auto minSize = Log2{Limits::TrafoSize::log2MinSize};

        return
            CbFlag::Pos
            {
                toUnderlying(offset.x() / toPel(minSize)),
                toUnderlying(offset.y() / toPel(minSize))
            };
    }

    QuadTree<CodingQuadTree> m_cqt;
    std::array<CbFlag, EnumRange<Plane>::length()> m_cbf;
protected:
    Ptr<CodingQuadTree> embedCodingQuadTree(
            PelCoord cqtCoord, PelCoord cqtOffset, Log2 cqtSize, int cqtDepth)
    {
        const auto offset = cqtCoord - get<Coord>()->inUnits();
        return m_cqt.append(offset, cqtCoord, cqtOffset, cqtSize, cqtDepth);
    }
public:
    static const auto Id = ElementId::coding_tree_unit;

    typedef CodingTreeUnitContent::Coord Coord;
    typedef CodingTreeUnitContent::CtbAddrInTs CtbAddrInTs;
    typedef CodingTreeUnitContent::CtbAddrInRs CtbAddrInRs;

    CodingTreeUnit(Log2 minCbSizeY, Log2 ctbSizeY, CtbCoord coord, CtbAddr addr):
        m_cqt({minCbSizeY, ctbSizeY + 1_log2})
    {
        embed<Coord>(*this, toPel(coord, ctbSizeY));
        embed<CtbAddrInTs>(*this, addr.inTs);
        embed<CtbAddrInRs>(*this, addr.inRs);
    }

    CbFlag::ValueType cbf(Plane plane, PelCoord offset) const
    {
        return m_cbf[int(plane)][toCbFlagPos(offset)];
    }

    CbFlag::Reference cbf(Plane plane, PelCoord offset)
    {
        return m_cbf[int(plane)][toCbFlagPos(offset)];
    }

    Ptr<SAO> getSAO() const
    {
        return Ptr<SAO>{getSubtree<SAO>()};
    }

    Ptr<CodingQuadTree> getCodingQuadTree(PelCoord at) const
    {
        const auto offset = at - get<Coord>()->inUnits();
        return m_cqt[offset];
    }

    VLA<Ptr<CodingQuadTree>> getCodingQuadTreeListInZ() const
    {
        return m_cqt.toListInZ();
    }

    void toStr(std::ostream &, Log2) const;
    void onParse(StreamAccessLayer &, Decoder::State &, const SliceSegmentHeader &);
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */

#endif /* HEVC_Syntax_CodingTreeUnit_h */
