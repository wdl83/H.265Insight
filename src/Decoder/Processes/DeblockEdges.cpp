#include <Decoder/Processes/DeblockEdges.h>
#include <Syntax/CodingTreeUnit.h>
#include <Syntax/CodingQuadTree.h>
#include <Syntax/CodingUnit.h>
#include <Syntax/TransformTree.h>
#include <Syntax/TransformUnit.h>
#include <Structure/Picture.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
namespace {
/*----------------------------------------------------------------------------*/
typedef Structure::CtbEdgeFlags CtbEdgeFlags;
/*----------------------------------------------------------------------------*/
inline
void hSet(CtbEdgeFlags &flags, PelCoord offset, Log2 size, bool value = true)
{
    const auto side = toInt(size - Log2{CtbEdgeFlags::ratio});
    const auto xOffset = toUnderlying(offset.x()) >> CtbEdgeFlags::ratio;
    const auto yOffset = toUnderlying(offset.y()) >> CtbEdgeFlags::ratio;

    for(auto i = 0; i < side; ++i)
    {
        flags[{xOffset + i, yOffset}] = value;
    }
}

inline
void vSet(CtbEdgeFlags &flags, PelCoord offset, Log2 size, bool value = true)
{
    const auto side = toInt(size - Log2{CtbEdgeFlags::ratio});
    const auto xOffset = toUnderlying(offset.x()) >> CtbEdgeFlags::ratio;
    const auto yOffset = toUnderlying(offset.y()) >> CtbEdgeFlags::ratio;

    for(auto i = 0; i < side; ++i)
    {
        flags[{xOffset, yOffset + i}] = value;
    }
}
/*----------------------------------------------------------------------------*/
void vPredBdry(
        PelCoord cuOffset,
        const Syntax::CodingUnit &cu,
        CtbEdgeFlags &vFlags)
{
    using namespace Syntax;

    typedef CodingUnit CU;

    const auto xOffset = cuOffset.x();
    const auto yOffset = cuOffset.y();
    const auto size = cu.get<CU::Size>()->inUnits();
    const auto partMode = cu.get<CU::PartModePseudo>()->get();

    if(
            PartitionMode::PART_Nx2N == partMode
            && Log2{CtbEdgeFlags::ratio} <= (size - 1_log2))
    {
        vSet(vFlags, {xOffset + toPel(size - 1_log2) , yOffset}, size);
    }
    else if(
            PartitionMode::PART_NxN == partMode
            && Log2{CtbEdgeFlags::ratio} <= (size - 1_log2))
    {
        vSet(vFlags, {xOffset + toPel(size - 1_log2) , yOffset}, size);
    }
    else if(
            PartitionMode::PART_nLx2N == partMode
            && Log2{CtbEdgeFlags::ratio} <= (size - 2_log2))
    {
        vSet(vFlags, {xOffset + toPel(size - 2_log2) , yOffset}, size);
    }
    else if(
            PartitionMode::PART_nRx2N == partMode
            && Log2{CtbEdgeFlags::ratio} <= (size - 2_log2))
    {
        vSet(vFlags, {xOffset + 3 * toPel(size - 2_log2) , yOffset}, size);
    }
    else if(PartitionMode::PART_2NxN == partMode)
    {
    }
    else if(PartitionMode::PART_2NxnU == partMode)
    {
    }
    else if(PartitionMode::PART_2NxnD == partMode)
    {
    }
}
/*----------------------------------------------------------------------------*/
void hPredBdry(
        PelCoord cuOffset,
        const Syntax::CodingUnit &cu,
        CtbEdgeFlags &hFlags)
{
    using namespace Syntax;

    typedef CodingUnit CU;

    const auto xOffset = cuOffset.x();
    const auto yOffset = cuOffset.y();
    const auto size = cu.get<CU::Size>()->inUnits();
    const auto partMode = cu.get<CU::PartModePseudo>()->get();

    if(PartitionMode::PART_Nx2N == partMode)
    {
    }
    else if(
            PartitionMode::PART_NxN == partMode
            && Log2{CtbEdgeFlags::ratio} <= (size - 1_log2))
    {
        hSet(hFlags, {xOffset, yOffset + toPel(size - 1_log2)}, size);
    }
    else if(PartitionMode::PART_nLx2N == partMode)
    {
    }
    else if(PartitionMode::PART_nRx2N == partMode)
    {
    }
    else if(
            PartitionMode::PART_2NxN == partMode
            && Log2{CtbEdgeFlags::ratio} <= (size - 1_log2))
    {
        hSet(hFlags, {xOffset, yOffset + toPel(size - 1_log2)}, size);
    }
    else if(
            PartitionMode::PART_2NxnU == partMode
            && Log2{CtbEdgeFlags::ratio} <= (size - 2_log2))
    {
        hSet(hFlags, {xOffset, yOffset + toPel(size - 2_log2)}, size);
    }
    else if(
            PartitionMode::PART_2NxnD == partMode
            && Log2{CtbEdgeFlags::ratio} <= (size - 2_log2))
    {
        hSet(hFlags, {xOffset, yOffset + 3 * toPel(size - 2_log2)}, size);
    }
}
/*----------------------------------------------------------------------------*/
void predBdry(
        PelCoord cuOffset,
        const Syntax::CodingUnit &cu, EdgeType edgeType,
        CtbEdgeFlags &flags)
{
    /* 04/2013, 8.7.2.3 "Derivation process of prediction block boundary" */
    using namespace Syntax;

    typedef CodingUnit CU;

    const PredictionMode predMode = *cu.get<CU::CuPredMode>();

    if(PredictionMode::Skip != predMode)
    {
        if(isV(edgeType))
        {
            vPredBdry(cuOffset, cu, flags);
        }
        else //if(isH(edgeType))
        {
            hPredBdry(cuOffset, cu, flags);
        }
    }
}
/*----------------------------------------------------------------------------*/
void transBdry(
        PelCoord cuOffset,
        const Syntax::CodingUnit &cu, EdgeType edgeType,
        CtbEdgeFlags &flags)
{
    using namespace Syntax;

    typedef CodingUnit CU;
    typedef TransformUnit TU;

    const auto cuCoord = cu.get<CU::Coord>()->inUnits();
    const auto cuSize = cu.get<CU::Size>()->inUnits();
    const auto ttList = cu.getTransformTreeListInZ();

    for(auto tt : ttList)
    {
        const auto tu = tt->getTransformUnit();

        if(tu)
        {
            const auto tuCoord = tu->get<TU::Coord>()->inUnits();
            const auto tuSize = tu->get<TU::Size>()->inUnits();

            if(isV(edgeType) && Log2{CtbEdgeFlags::ratio} <= tuSize)
            {
                vSet(flags, (tuCoord - cuCoord) + cuOffset, tuSize);
            }
            else if(isH(edgeType) && Log2{CtbEdgeFlags::ratio} <= tuSize)
            {
                hSet(flags, (tuCoord - cuCoord) + cuOffset, tuSize);
            }
        }
    }

    if(ttList.empty())
    {
        if(isV(edgeType))
        {
            vSet(flags, cuOffset, cuSize);
        }
        else // if(isH(edgeType))
        {
            hSet(flags, cuOffset, cuSize);
        }
    }
}
/*----------------------------------------------------------------------------*/
bool isTileBdry(
        Ptr<const Structure::Picture> picture,
        PelCoord coord, PelCoord adjCoord)
{
    using namespace Syntax;

    const auto pps = picture->pps;
    const bool loopFilterAcrossTilesEnabledFlag
    {
        *pps->get<PictureParameterSet::LoopFilterAcrossTilesEnabledFlag>()
    };

    if(loopFilterAcrossTilesEnabledFlag)
    {
        return false;
    }
    else
    {
        const auto currTileId = picture->tileId(coord);
        const auto adjTileId = picture->tileId(adjCoord);

        return currTileId != adjTileId;
    }
}
/*----------------------------------------------------------------------------*/
bool isSliceBdry(
        Ptr<const Structure::Picture> picture,
        PelCoord coord, PelCoord adjCoord)
{
    using namespace Syntax;

    typedef SliceSegmentHeader SSH;

    const auto currS = picture->slice(coord);
    const auto currSH = currS->header();
    const bool sliceLoopFilterAcrossSlicesEnabledFlag
    {
        *currSH->get<SSH::SliceLoopFilterAcrossSlicesEnabledFlag>()
    };

    if(sliceLoopFilterAcrossSlicesEnabledFlag)
    {
        return false;
    }
    else
    {
        const auto currAddrInRs = currS->addr().inRs;
        const auto adjAddrInRs = picture->slice(adjCoord)->addr().inRs;

        return currAddrInRs != adjAddrInRs;
    }
}
/*----------------------------------------------------------------------------*/
bool isLeftBdry(
        Ptr<const Structure::Picture> picture,
        PelCoord coord)
{
    /* 04/2013, 8.7.2 "Deblcking filter process" */

    // 2 (The variable filterLeftEdgeFlag is derived as follows:)
    const auto isPictureBdry =
        [coord]()
        {
            return 0_pel == coord.x();
        };

    const PelCoord leftCoord{coord.x() - 1_pel, coord.y()};

    const bool bdry =
        isPictureBdry()
        || isTileBdry(picture, coord, leftCoord)
        || isSliceBdry(picture, coord, leftCoord);

    return bdry;
}
/*----------------------------------------------------------------------------*/
bool isTopBdry(
        Ptr<const Structure::Picture> picture,
        PelCoord coord)
{
    /* 04/2013, 8.7.2 "Deblcking filter process" */

    // 2 (The variable filterTopEdgeFlag is derived as follows:)
    const auto isPictureBdry =
        [coord]()
        {
            return 0_pel == coord.y();
        };

    const PelCoord topCoord{coord.x(), coord.y() - 1_pel};

    const bool bdry =
        isPictureBdry()
        || isTileBdry(picture, coord, topCoord)
        || isSliceBdry(picture, coord, topCoord);

    return bdry;
}
/*----------------------------------------------------------------------------*/
void maskBdry(
        Ptr<const Structure::Picture> picture,
        PelCoord coord, Log2 size,
        EdgeType edgeType,
        DeblockEdges &edges)
{
    if(isV(edgeType))
    {
        const auto bdry = isLeftBdry(picture, coord);

        if(bdry)
        {
            vSet(edges.prediction, {0_pel, 0_pel}, size, false);
            vSet(edges.transform, {0_pel, 0_pel}, size, false);
        }
    }
    else //if(isH(edgeType))
    {
        const auto bdry = isTopBdry(picture, coord);

        if(bdry)
        {
            hSet(edges.prediction, {0_pel, 0_pel}, size, false);
            hSet(edges.transform, {0_pel, 0_pel}, size, false);
        }
    }
}
/*----------------------------------------------------------------------------*/
} /* namespace */
/*----------------------------------------------------------------------------*/
DeblockEdges PredTransEdges::exec(
        State &,
        Ptr<const Structure::Picture> picture,
        const Syntax::CodingTreeUnit &ctu,
        EdgeType edgeType)
{
    using namespace Syntax;

    typedef CodingTreeUnit CTU;
    typedef CodingUnit CU;

    const auto ctuCoord = ctu.get<CTU::Coord>()->inUnits();
    const auto ctuSize = picture->ctbSizeY;
    const auto cqtListInZ = ctu.getCodingQuadTreeListInZ();

    DeblockEdges edges;

    for(auto i : cqtListInZ)
    {
        const auto cu = i->getCodingUnit();

        if(cu)
        {
            const auto cuCoord = cu->get<CU::Coord>()->inUnits();
            const auto cuOffset = cuCoord - ctuCoord;

            predBdry(cuOffset, *cu, edgeType, edges.prediction);
            transBdry(cuOffset, *cu, edgeType, edges.transform);
        }
    }

    maskBdry(picture, ctuCoord, ctuSize, edgeType, edges);

    {
        const auto toStr =
            [edgeType, ctuCoord, ctuSize, &edges](std::ostream &oss)
            {
                oss << ctuCoord << ' ' << getName(edgeType) << '\n';
                edges.prediction.toStr(oss, ctuSize, edgeType);
            };

        log(LogId::DeblockPredictionEdges, toStr);
    }

    {
        const auto toStr =
            [edgeType, ctuCoord, ctuSize, &edges](std::ostream &oss)
            {
                oss << ctuCoord << ' ' << getName(edgeType) << '\n';
                edges.transform.toStr(oss, ctuSize, edgeType);
            };

        log(LogId::DeblockTransformEdges, toStr);
    }

    {
        const auto toStr =
            [edgeType, ctuCoord, ctuSize, &edges](std::ostream &oss)
            {
                oss << ctuCoord << ' ' << getName(edgeType) << '\n';
                (edges.prediction | edges.transform).toStr(oss, ctuSize, edgeType);
            };

        log(LogId::DeblockEdges, toStr);
    }

    return edges;
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */
