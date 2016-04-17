#include <Decoder/Processes/BoundaryFilteringStrength.h>
#include <Decoder/Processes/DeblockEdges.h>
#include <Syntax/SequenceParameterSet.h>
#include <Syntax/CodingTreeUnit.h>
#include <Syntax/CodingQuadTree.h>
#include <Syntax/CodingUnit.h>
#include <Syntax/PredictionUnit.h>
#include <Syntax/TransformTree.h>
#include <Syntax/TransformUnit.h>
#include <Structure/Picture.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
namespace {
/*----------------------------------------------------------------------------*/
typedef Structure::CtbEdgeBS CtbEdgeBS;
typedef std::function<bool(const Syntax::CodingUnit &)> IntraPredicate;
typedef std::function<bool(PelCoord)> NonZeroCoeffPredicate;
typedef std::function<bool(const Syntax::CodingUnit &)> BypassPredicate;

/* 04/2013, 8.7.2.4 "Derivation process of boundary filtering strength" */
bool interCond(
        Ptr<const Structure::Picture> picture,
        Ptr<const Syntax::CodingUnit> pCU,
        Ptr<const Syntax::CodingUnit> qCU,
        PelCoord p0, PelCoord q0)
{
    using namespace Syntax;

    typedef SliceSegmentHeader SSH;
    typedef PredictionUnit PU;

    const auto pSlice = picture->slice(p0);
    const auto qSlice = picture->slice(q0);
    const auto &pRPL = pSlice->rpl;
    const auto &qRPL = qSlice->rpl;
    const auto pPU = pCU->getPredictionUnit(p0);
    const auto qPU = qCU->getPredictionUnit(q0);
    const auto pPredFlag = pPU->get<PU::PredFlagLX>();
    const auto qPredFlag = qPU->get<PU::PredFlagLX>();
    const auto pRefIdx = pPU->get<PU::RefIdxLX>();
    const auto qRefIdx = qPU->get<PU::RefIdxLX>();
    const auto pMv = pPU->get<PU::MvLX>();
    const auto qMv = qPU->get<PU::MvLX>();
    //const int pMvNum = (*pPredFlag)[RefList::L0] + (*pPredFlag)[RefList::L1];
    //const int qMvNum = (*qPredFlag)[RefList::L0] + (*qPredFlag)[RefList::L1];

    const auto pPOC =
        [&pRPL, pPredFlag, pRefIdx](RefList l)
        {
            runtime_assert((*pPredFlag)[l]);
            return pRPL[l][(*pRefIdx)[l]];
        };

    const auto qPOC =
        [&qRPL, qPredFlag, qRefIdx](RefList l)
        {
            runtime_assert((*qPredFlag)[l]);
            return qRPL[l][(*qRefIdx)[l]];
        };

    const auto ppDiffRef =
        [&pPOC, pPredFlag]()
        {
            return
                !(*pPredFlag)[RefList::L0] && (*pPredFlag)[RefList::L1]
                || (*pPredFlag)[RefList::L0] && !(*pPredFlag)[RefList::L1]
                ||
                (*pPredFlag)[RefList::L0] && (*pPredFlag)[RefList::L1]
                && pPOC(RefList::L0) != pPOC(RefList::L1);
        };

    const auto pqDiffRef =
        [&pPOC, &qPOC, pPredFlag, qPredFlag](RefList pL, RefList qL)
        {
            return
                !(*pPredFlag)[pL] && (*qPredFlag)[qL]
                || (*pPredFlag)[pL] && !(*qPredFlag)[qL]
                || (*pPredFlag)[pL] && (*qPredFlag)[qL] && pPOC(pL) != qPOC(qL);
        };

    const auto pqEqRef =
        [&pqDiffRef](RefList pL, RefList qL)
        {
            return !pqDiffRef(pL, qL);
        };

    const auto diffMv4 =
        [](MotionVector a, MotionVector b)
        {
            return 4_sub_pel <= abs(a.x() - b.x()) || 4_sub_pel <= abs(a.y() - b.y());
        };

    const auto pqDiffMv =
        [pPredFlag, qPredFlag, pMv, qMv, diffMv4](RefList pL, RefList qL)
        {
            return
                !(*pPredFlag)[pL] && (*qPredFlag)[qL]
                || (*pPredFlag)[pL] && !(*qPredFlag)[qL]
                ||
                (*pPredFlag)[pL] && (*qPredFlag)[qL]
                && diffMv4((*pMv)[pL], (*qMv)[qL]);
        };

    if(
            isB(*pSlice->header()->get<SSH::SliceType>())
            || isB(*qSlice->header()->get<SSH::SliceType>()))
    {
        if(
                pqEqRef(RefList::L0, RefList::L0) && pqEqRef(RefList::L1, RefList::L1)
                || pqEqRef(RefList::L0, RefList::L1) && pqEqRef(RefList::L1, RefList::L0))
        {
            if(ppDiffRef())
            {
                if(pqEqRef(RefList::L0, RefList::L0))
                {
                    const auto cond =
                        pqDiffMv(RefList::L0, RefList::L0) || pqDiffMv(RefList::L1, RefList::L1);

                    return cond;
                }
                else
                {
                    const auto cond =
                        pqDiffMv(RefList::L0, RefList::L1) || pqDiffMv(RefList::L1, RefList::L0);

                    return cond;
                }
            }
            else
            {
                const auto cond =
                    /* part 1 - inferred as true, from conditions above */
                    /* part 2 */
                    (pqDiffMv(RefList::L0, RefList::L0) || pqDiffMv(RefList::L1, RefList::L1))
                    /* part 3 */
                    && (pqDiffMv(RefList::L0, RefList::L1) || pqDiffMv(RefList::L1, RefList::L0));

                return cond;
            }
        }
        else
        {
            return true;
        }
    }
    else // p & q are in slice P
    {
        const auto cond =
            pqDiffRef(RefList::L0, RefList::L0)
            || pqDiffMv(RefList::L0, RefList::L0);

        return cond;
    }
}

CtbEdgeBS deriveVBS(
        Ptr<const Structure::Picture> picture,
        const Syntax::CodingTreeUnit &ctu,
        const DeblockEdges &edges,
        IntraPredicate isIntra,
        NonZeroCoeffPredicate isNoneZeroCoeff,
        BypassPredicate isBypass)
{
    using namespace Syntax;

    typedef CodingTreeUnit CTU;
    typedef CodingUnit CU;

    const auto ctuSize = picture->ctbSizeY;
    const auto ctuCoord = ctu.get<CTU::Coord>()->inUnits();
    const auto xN = toUnderlying(toPel(ctuSize - 3_log2));
    const auto yN = toUnderlying(toPel(ctuSize - 2_log2));

    CtbEdgeBS vBS;

    for(auto xDi = 0; xDi < xN; ++xDi)
    {
        for(auto yDi = 0; yDi < yN; ++yDi)
        {
            const bool isPredEdge = edges.prediction[{xDi << 1, yDi}];
            const bool isTransEdge = edges.transform[{xDi << 1, yDi}];

            if(isPredEdge || isTransEdge)
            {
                const PelCoord pOffset{Pel{(xDi << 3) - 1}, Pel{yDi << 2}};
                const PelCoord qOffset{Pel{xDi << 3}, Pel{yDi << 2}};
                const PelCoord p0(ctuCoord + pOffset);
                const PelCoord q0(ctuCoord + qOffset);
                const auto pCU = picture->getCodingUnit(p0);
                const auto qCU = picture->getCodingUnit(q0);

                CtbEdgeBS::BS bs(
                        0,
                        pCU->get<CU::QpY>()->qpY,
                        qCU->get<CU::QpY>()->qpY,
                        isBypass(*pCU), isBypass(*qCU));

                if(isIntra(*pCU) || isIntra(*qCU))
                {
                    bs.bits.bs = 2;
                }
                else if(
                        isTransEdge
                        && (isNoneZeroCoeff(p0) || isNoneZeroCoeff(q0)))
                {
                    bs.bits.bs = 1;
                }
                else if(interCond(picture, pCU, qCU, p0, q0))
                {
                    bs.bits.bs = 1;
                }

                vBS[{xDi, yDi}] = bs;
            }
        }
    }

    const auto toStr =
        [ctuCoord, ctuSize, &vBS](std::ostream &oss)
        {
            oss << ctuCoord << " V\n";
            vBS.writeTo(oss, ctuSize, EdgeType::V);
        };

    log(LogId::DeblockBS, toStr);
    return vBS;
}
/*----------------------------------------------------------------------------*/
CtbEdgeBS deriveHBS(
        Ptr<const Structure::Picture> picture,
        const Syntax::CodingTreeUnit &ctu,
        const DeblockEdges &edges,
        IntraPredicate isIntra,
        NonZeroCoeffPredicate isNoneZeroCoeff,
        BypassPredicate isBypass)
{
    using namespace Syntax;

    typedef CodingTreeUnit CTU;
    typedef CodingUnit CU;

    const auto ctuSize = picture->ctbSizeY;
    const auto ctuCoord = ctu.get<CTU::Coord>()->inUnits();
    const auto xN = toUnderlying(toPel(ctuSize - 2_log2));
    const auto yN = toUnderlying(toPel(ctuSize - 3_log2));

    CtbEdgeBS hBS;

    for(auto xDi = 0; xDi < xN; ++xDi)
    {
        for(auto yDi = 0; yDi < yN; ++yDi)
        {
            const bool isPredEdge = edges.prediction[{xDi, yDi << 1}];
            const bool isTransEdge = edges.transform[{xDi, yDi << 1}];

            if(isPredEdge || isTransEdge)
            {
                const PelCoord pOffset{Pel{xDi << 2}, Pel{(yDi << 3) - 1}};
                const PelCoord qOffset{Pel{xDi << 2}, Pel{yDi << 3}};
                const PelCoord p0(ctuCoord + pOffset);
                const PelCoord q0(ctuCoord + qOffset);
                const auto pCU = picture->getCodingUnit(p0);
                const auto qCU = picture->getCodingUnit(q0);

                CtbEdgeBS::BS bs(
                        0,
                        pCU->get<CU::QpY>()->qpY,
                        qCU->get<CU::QpY>()->qpY,
                        isBypass(*pCU), isBypass(*qCU));

                if(isIntra(*pCU) || isIntra(*qCU))
                {
                    bs.bits.bs = 2;
                }
                else if(
                        isTransEdge
                        && (isNoneZeroCoeff(p0) || isNoneZeroCoeff(q0)))
                {
                    bs.bits.bs = 1;
                }
                else if(interCond(picture, pCU, qCU, p0, q0))
                {
                    bs.bits.bs = 1;
                }

                hBS[{xDi, yDi}] = bs;
            }
        }
    }

    const auto toStr =
        [ctuCoord, ctuSize, &hBS](std::ostream &oss)
        {
            oss << ctuCoord << " H\n";
            hBS.writeTo(oss, ctuSize, EdgeType::H);
        };

    log(LogId::DeblockBS, toStr);
    return hBS;
}
/*----------------------------------------------------------------------------*/
} /* namespace */
/*----------------------------------------------------------------------------*/
Structure::CtbEdgeBS BoundaryFilteringStrength::exec(
        State &,
        Ptr<const Structure::Picture> picture,
        const Syntax::CodingTreeUnit &ctu,
        const DeblockEdges &edges,
        EdgeType edgeType)
{
    using namespace Syntax;

    typedef SequenceParameterSet SPS;
    typedef CodingTreeUnit CTU;
    typedef TransformTree TT;

    const auto sps = picture->sps;
    const bool pcmLoopFilterDisabledFlag(*sps->get<SPS::PcmLoopFilterDisabledFlag>());
    const auto ctuCoord = ctu.get<CTU::Coord>()->inUnits();
    const PelRect ctuBdry{ctuCoord, toPel(picture->ctbSizeY)};

    const auto toCtuBdry =
        [picture](PelCoord coord)
        {
            return picture->toPel(picture->toCtb(coord));
        };

    const auto isIntra =
        [](const CodingUnit &cu)
        {
            return HEVC::isIntra(*cu.get<CodingUnit::CuPredMode>());
        };

    const auto isNoneZeroCoeff =
        [picture, &ctu, ctuCoord, &ctuBdry, &toCtuBdry](PelCoord at)
        {
            return
                ctuBdry.encloses(at)
                ? ctu.cbf(Plane::Y, at - ctuCoord)
                : picture->getCodingTreeUnit(at)->cbf(Plane::Y, at - toCtuBdry(at));
        };

    const auto isBypass =
        [pcmLoopFilterDisabledFlag](const CodingUnit &cu)
        {
            return
                pcmLoopFilterDisabledFlag && cu.isPCM()
                || cu.isCuTransQuantBypass();
        };

    return
        isV(edgeType)
        ? deriveVBS(picture, ctu, edges, isIntra, isNoneZeroCoeff, isBypass)
        : deriveHBS(picture, ctu, edges, isIntra, isNoneZeroCoeff, isBypass);
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */
