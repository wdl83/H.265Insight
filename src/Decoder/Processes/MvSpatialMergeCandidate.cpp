#include <Decoder/Processes/MvSpatialMergeCandidate.h>
#include <Syntax/PictureParameterSet.h>
#include <Syntax/CodingUnit.h>
#include <Syntax/PredictionUnit.h>
#include <Structure/Picture.h>


namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
MvSpatialMergeCand MvSpatialMergeCandidate::exec(
        State &, Ptr<const Structure::Picture> picture,
        Ptr<const Syntax::CodingUnit> cu,
        PelCoord puCoord, Pel nPbW, Pel nPbH, int partIdx)
{
    using namespace Syntax;

    typedef PictureParameterSet PPS;
    typedef CodingUnit CU;
    typedef PredictionUnit PU;

    const auto pps = picture->pps;
    const auto parMgrLevel = 2_log2 + *pps->get<PPS::Log2ParallelMergeLevelMinus2>();

    const auto xPb = puCoord.x();
    const auto yPb = puCoord.y();

    const auto cuCoord = cu->get<CU::Coord>()->inUnits();
    const auto cuSize = cu->get<CU::Size>()->inUnits();
    const PartitionMode partMode = *cu->get<CU::PartModePseudo>();

    // wrapper for 6.4.2, used for both predictors: A and B
    const auto isAvailable =
        [picture, cuCoord, cuSize, puCoord, nPbW, nPbH, partIdx](PelCoord at)
        {
            return
                picture->isPredictionBlockAvailable(
                        cuCoord, toPel(cuSize), puCoord, nPbW, nPbH, partIdx, at);
        };

    const auto equalCond =
        [picture](PelCoord puCoordA, PelCoord puCoordB)
        {
            const auto puA = picture->getPredictionUnit(puCoordA);
            const auto puB = picture->getPredictionUnit(puCoordB);

            return
                *puA->get<PU::MvLX>() == *puB->get<PU::MvLX>()
                && *puA->get<PU::MvCLX>() == *puB->get<PU::MvCLX>()
                && *puA->get<PU::RefIdxLX>() == *puB->get<PU::RefIdxLX>();
        };

    MvSpatialMergeCand cand;

    // derive neighbour A1
    const PelCoord nbA1Coord{xPb - 1_pel, yPb + nPbH - 1_pel};

    const auto alignmentCondA1 =
        puCoord.x() >> parMgrLevel == nbA1Coord.x() >> parMgrLevel
        && puCoord.y() >> parMgrLevel == nbA1Coord.y() >> parMgrLevel;

    const auto partitionCondA1 =
        1 == partIdx
        && (
                PartitionMode::PART_Nx2N == partMode
                || PartitionMode::PART_nLx2N == partMode
                || PartitionMode::PART_nRx2N == partMode);

    const auto nbA1Avaiable =
        !alignmentCondA1
        && !partitionCondA1
        && isAvailable(nbA1Coord);

    if(nbA1Avaiable)
    {
        const auto nbPU = picture->getPredictionUnit(nbA1Coord);
        const auto nbMvLX = nbPU->get<PU::MvLX>();
        const auto nbRefIdxLX = nbPU->get<PU::RefIdxLX>();
        const auto nbPredFlagLX = nbPU->get<PU::PredFlagLX>();

        cand.availableFlag[MvSpatialMergeCand::A1] = true;
        // (8-98)
        cand.mvL[MvSpatialMergeCand::A1] = *nbMvLX;
        // (8-99)
        cand.refIdxL[MvSpatialMergeCand::A1] = *nbRefIdxLX;
        // (8-100)
        cand.predFlagL[MvSpatialMergeCand::A1] = *nbPredFlagLX;
    }

    // derive neighbour B1
    const PelCoord nbB1Coord{xPb + nPbW - 1_pel, yPb - 1_pel};

    const auto alignmentCondB1 =
        puCoord.x() >> parMgrLevel == nbB1Coord.x() >> parMgrLevel
        && puCoord.y() >> parMgrLevel == nbB1Coord.y() >> parMgrLevel;

    const auto partitionCondB1 =
        1 == partIdx
        && (
                PartitionMode::PART_2NxN == partMode
                || PartitionMode::PART_2NxnU == partMode
                || PartitionMode::PART_2NxnD == partMode);

    const auto nbB1Avaiable =
        !alignmentCondB1
        && !partitionCondB1
        && isAvailable(nbB1Coord);

    if(
            nbB1Avaiable
            && !(nbA1Avaiable && equalCond(nbA1Coord, nbB1Coord)))
    {
        const auto nbPU = picture->getPredictionUnit(nbB1Coord);
        const auto nbMvLX = nbPU->get<PU::MvLX>();
        const auto nbRefIdxLX = nbPU->get<PU::RefIdxLX>();
        const auto nbPredFlagLX = nbPU->get<PU::PredFlagLX>();

        cand.availableFlag[MvSpatialMergeCand::B1] = true;
        // (8-101)
        cand.mvL[MvSpatialMergeCand::B1] = *nbMvLX;
        // (8-102)
        cand.refIdxL[MvSpatialMergeCand::B1] = *nbRefIdxLX;
        // (8-103)
        cand.predFlagL[MvSpatialMergeCand::B1] = *nbPredFlagLX;
    }

    // derive neighbour B0
    const PelCoord nbB0Coord{xPb + nPbW, yPb - 1_pel};

    const auto alignmentCondB0 =
        puCoord.x() >> parMgrLevel == nbB0Coord.x() >> parMgrLevel
        && puCoord.y() >> parMgrLevel == nbB0Coord.y() >> parMgrLevel;

    const auto nbB0Avaiable =
        !alignmentCondB0
        && isAvailable(nbB0Coord);

    if(
            nbB0Avaiable
            && !(nbB1Avaiable && equalCond(nbB1Coord, nbB0Coord)))
    {
        const auto nbPU = picture->getPredictionUnit(nbB0Coord);
        const auto nbMvLX = nbPU->get<PU::MvLX>();
        const auto nbRefIdxLX = nbPU->get<PU::RefIdxLX>();
        const auto nbPredFlagLX = nbPU->get<PU::PredFlagLX>();

        cand.availableFlag[MvSpatialMergeCand::B0] = true;
        // (8-104)
        cand.mvL[MvSpatialMergeCand::B0] = *nbMvLX;
        // (8-105)
        cand.refIdxL[MvSpatialMergeCand::B0] = *nbRefIdxLX;
        // (8-106)
        cand.predFlagL[MvSpatialMergeCand::B0] = *nbPredFlagLX;
    }

    // derive neighbour A0
    const PelCoord nbA0Coord{xPb - 1_pel, yPb + nPbH};

    const auto alignmentCondA0 =
        puCoord.x() >> parMgrLevel == nbA0Coord.x() >> parMgrLevel
        && puCoord.y() >> parMgrLevel == nbA0Coord.y() >> parMgrLevel;

    const auto nbA0Avaiable =
        !alignmentCondA0
        && isAvailable(nbA0Coord);

    if(
            nbA0Avaiable
            && !(nbA1Avaiable && equalCond(nbA1Coord, nbA0Coord)))
    {
        const auto nbPU = picture->getPredictionUnit(nbA0Coord);
        const auto nbMvLX = nbPU->get<PU::MvLX>();
        const auto nbRefIdxLX = nbPU->get<PU::RefIdxLX>();
        const auto nbPredFlagLX = nbPU->get<PU::PredFlagLX>();

        cand.availableFlag[MvSpatialMergeCand::A0] = true;
        // (8-107)
        cand.mvL[MvSpatialMergeCand::A0] = *nbMvLX;
        // (8-108)
        cand.refIdxL[MvSpatialMergeCand::A0] = *nbRefIdxLX;
        // (8-109)
        cand.predFlagL[MvSpatialMergeCand::A0] = *nbPredFlagLX;
    }

    // derive neighbour B2
    const PelCoord nbB2Coord{xPb - 1_pel, yPb - 1_pel};

    const auto alignmentCondB2 =
        puCoord.x() >> parMgrLevel == nbB2Coord.x() >> parMgrLevel
        && puCoord.y() >> parMgrLevel == nbB2Coord.y() >> parMgrLevel;

    const auto nbB2Avaiable =
        !alignmentCondB2
        && isAvailable(nbB2Coord);

    if(
            nbB2Avaiable
            && !(nbA1Avaiable && equalCond(nbA1Coord, nbB2Coord))
            && !(nbB1Avaiable && equalCond(nbB1Coord, nbB2Coord))
            && !(
                cand.availableFlag[MvSpatialMergeCand::A0]
                && cand.availableFlag[MvSpatialMergeCand::A1]
                && cand.availableFlag[MvSpatialMergeCand::B0]
                && cand.availableFlag[MvSpatialMergeCand::B1]))
    {
        const auto nbPU = picture->getPredictionUnit(nbB2Coord);
        const auto nbMvLX = nbPU->get<PU::MvLX>();
        const auto nbRefIdxLX = nbPU->get<PU::RefIdxLX>();
        const auto nbPredFlagLX = nbPU->get<PU::PredFlagLX>();

        cand.availableFlag[MvSpatialMergeCand::B2] = true;
        // (8-110)
        cand.mvL[MvSpatialMergeCand::B2] = *nbMvLX;
        // (8-111)
        cand.refIdxL[MvSpatialMergeCand::B2] = *nbRefIdxLX;
        // (8-112)
        cand.predFlagL[MvSpatialMergeCand::B2] = *nbPredFlagLX;
    }

    const auto toStr =
        [puCoord, &cand](std::ostream &oss)
        {
            oss << puCoord << '\n';

            for(auto X : EnumRange<RefList>())
            {
                oss
                    << 'L' << int(X) << ' '
                    << "A0 "
                    << cand.availableFlag[MvSpatialMergeCand::A0] << ' '
                    << cand.mvL[MvSpatialMergeCand::A0][X] << ' '
                    << cand.refIdxL[MvSpatialMergeCand::A0][X] << ' '
                    << cand.predFlagL[MvSpatialMergeCand::A0][X] << '\n'
                    << 'L' << int(X) << ' '
                    << "A1 "
                    << cand.availableFlag[MvSpatialMergeCand::A1] << ' '
                    << cand.mvL[MvSpatialMergeCand::A1][X] << ' '
                    << cand.refIdxL[MvSpatialMergeCand::A1][X] << ' '
                    << cand.predFlagL[MvSpatialMergeCand::A1][X] << '\n'
                    << 'L' << int(X) << ' '
                    << "B0 "
                    << cand.availableFlag[MvSpatialMergeCand::B0] << ' '
                    << cand.mvL[MvSpatialMergeCand::B0][X] << ' '
                    << cand.refIdxL[MvSpatialMergeCand::B0][X] << ' '
                    << cand.predFlagL[MvSpatialMergeCand::B0][X] << '\n'
                    << 'L' << int(X) << ' '
                    << "B1 "
                    << cand.availableFlag[MvSpatialMergeCand::B1] << ' '
                    << cand.mvL[MvSpatialMergeCand::B1][X] << ' '
                    << cand.refIdxL[MvSpatialMergeCand::B1][X] << ' '
                    << cand.predFlagL[MvSpatialMergeCand::B1][X] << '\n'
                    << 'L' << int(X) << ' '
                    << "B2 "
                    << cand.availableFlag[MvSpatialMergeCand::B2] << ' '
                    << cand.mvL[MvSpatialMergeCand::B2][X] << ' '
                    << cand.refIdxL[MvSpatialMergeCand::B2][X] << ' '
                    << cand.predFlagL[MvSpatialMergeCand::B2][X] << '\n';
            }
        };

    log(LogId::MvSpatialMergeCandidate, toStr);

    return cand;
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */
