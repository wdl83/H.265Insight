#include <Decoder/Processes/MvPredictorCandidate.h>
#include <Decoder/State.h>
#include <Syntax/CodingUnit.h>
#include <Syntax/PredictionUnit.h>
#include <Syntax/MvdCoding.h>
#include <Structure/Picture.h>
#include <Structure/RPL.h>
#include <log.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
namespace {
/*----------------------------------------------------------------------------*/
typedef std::function<bool (PelCoord)> AvailabilityPredicate;

Tuple<MotionVector, bool> deriveA(
        State &decoder,
        Ptr<const Structure::Picture> currPic,
        const Structure::RPL &refPicList,
        Pel xPb, Pel yPb, Pel, Pel height,
        RefList X, PicOrderCntVal refIdxLX,
        bool &isScaledFlagLX,
        AvailabilityPredicate isAvailable)
{
    using namespace Syntax;

    typedef PredictionUnit PU;

    // mvLXA & availableFlagLXA
    const auto nbNum = 2;

    // 1, 2
    std::array<PelCoord, nbNum> nbAk
    {
        {
            {xPb - 1_pel, yPb + height},
            {xPb - 1_pel, yPb + height - 1_pel}
        }
    };
    auto availableFlagLXA = false;
    MotionVector mvLXA;
    std::array<bool, nbNum> availableAk{{false, false}};

    // 3, 4, 5
    for(auto k = 0; k < nbNum; ++k)
    {
        availableAk[k] = isAvailable(nbAk[k]);
        isScaledFlagLX |= availableAk[k];
    }

    // 6
    for(auto k = 0; k < nbNum; ++k)
    {
        if(availableAk[k] && !availableFlagLXA)
        {
            const auto Y = RefList::L0 == X ? RefList::L1 : RefList::L0;
            const auto nbPU = currPic->getPredictionUnit(nbAk[k]);
            const auto nbPredFlagLX = (*nbPU->get<PU::PredFlagLX>())[X];
            const auto nbPredFlagLY = (*nbPU->get<PU::PredFlagLX>())[Y];
            const auto nbRefIdxLX = (*nbPU->get<PU::RefIdxLX>())[X];
            const auto nbRefIdxLY = (*nbPU->get<PU::RefIdxLX>())[Y];

            if(
                    nbPredFlagLX
                    &&
                    PicOrderCntVal{0}
                    == diff(refPicList[X][nbRefIdxLX], refPicList[X][refIdxLX]))
            {
                availableFlagLXA = true;
                // (8-144)
                mvLXA = (*nbPU->get<PU::MvLX>())[X];
            }
            else if(
                    nbPredFlagLY
                    &&
                    PicOrderCntVal{0}
                    == diff(refPicList[Y][nbRefIdxLY], refPicList[X][refIdxLX]))
            {
                availableFlagLXA = true;
                // (8-145)
                mvLXA = (*nbPU->get<PU::MvLX>())[Y];
            }
        }
    }

    // 7
    for(auto k = 0; k < nbNum && !availableFlagLXA; ++k)
    {
        const auto Y = RefList::L0 == X ? RefList::L1 : RefList::L0;
        PicOrderCntVal refIdxA;
        const Structure::RPL::List *refPicListA = nullptr;

        if(availableAk[k] && !availableFlagLXA)
        {
            const auto nbPU = currPic->getPredictionUnit(nbAk[k]);
            const auto nbPredFlagLX = (*nbPU->get<PU::PredFlagLX>())[X];
            const auto nbPredFlagLY = (*nbPU->get<PU::PredFlagLX>())[Y];
            const auto nbRefIdxLX = (*nbPU->get<PU::RefIdxLX>())[X];
            const auto nbRefIdxLY = (*nbPU->get<PU::RefIdxLX>())[Y];


            if(
                    nbPredFlagLX
                    &&
                    longTermRefPic(decoder, *currPic, {xPb, yPb}, X, refIdxLX)
                    == longTermRefPic(decoder, *currPic, {xPb, yPb}, X, nbRefIdxLX))
            {
                availableFlagLXA = true;
                // (8-146)
                mvLXA = (*nbPU->get<PU::MvLX>())[X];
                // (8-147)
                refIdxA = nbRefIdxLX;
                // (8-148)
                refPicListA = &refPicList[X];
            }
            else if(
                    nbPredFlagLY
                    &&
                    longTermRefPic(decoder, *currPic, {xPb, yPb}, X, refIdxLX)
                    == longTermRefPic(decoder, *currPic, {xPb, yPb}, Y, nbRefIdxLY))
            {
                availableFlagLXA = true;
                // (8-149)
                mvLXA = (*nbPU->get<PU::MvLX>())[Y];
                // (8-150)
                refIdxA = nbRefIdxLY;
                // (8-151)
                refPicListA = &refPicList[Y];
            }
        }

        if(
                availableFlagLXA
                &&
                PicOrderCntVal{0}
                != diff((*refPicListA)[refIdxA], refPicList[X][refIdxLX])
                && decoder.dpb.isShortTerm((*refPicListA)[refIdxA])
                && decoder.dpb.isShortTerm(refPicList[X][refIdxLX]))
        {
            // (8-152)
            const auto diffTd =
                diff(currPic->order.get<PicOrderCntVal>(), (*refPicListA)[refIdxA]);
            // (8-153)
            const auto diffTb =
                diff(currPic->order.get<PicOrderCntVal>(), refPicList[X][refIdxLX]);

            mvLXA = MvScaleCoeff{diffTd, diffTb}.scale(mvLXA);
        }
    }

    return makeTuple(mvLXA, availableFlagLXA);
}
/*----------------------------------------------------------------------------*/
Tuple<MotionVector, bool> deriveB(
        State &decoder,
        Ptr<const Structure::Picture> currPic,
        const Structure::RPL &refPicList,
        Pel xPb, Pel yPb, Pel width, Pel,
        RefList X, PicOrderCntVal refIdxLX,
        bool &isScaledFlagLX,
        MotionVector &mvLXA, bool &availableFlagLXA,
        AvailabilityPredicate isAvailable)
{
    using namespace Syntax;

    typedef PredictionUnit PU;

    const auto nbNum = 3;
    const Structure::RPL::List *refPicListB = nullptr;
    PicOrderCntVal refIdxB;

    // 1, 2
    std::array<PelCoord, 3> nbBk
    {
        {
            {xPb + width, yPb - 1_pel},
            {xPb + width - 1_pel, yPb - 1_pel},
            {xPb - 1_pel, yPb - 1_pel}
        }
    };
    auto availableFlagLXB = false;
    MotionVector mvLXB;
    std::array<bool, nbNum> availableBk;

    // 3
    for(auto k = 0; k < nbNum; ++k)
    {
        availableBk[k] = isAvailable(nbBk[k]);

        if(availableBk[k] && !availableFlagLXB)
        {
            const auto Y = RefList::L0 == X ? RefList::L1 : RefList::L0;
            const auto nbPU = currPic->getPredictionUnit(nbBk[k]);
            const auto nbPredFlagLX = (*nbPU->get<PU::PredFlagLX>())[X];
            const auto nbPredFlagLY = (*nbPU->get<PU::PredFlagLX>())[Y];
            const auto nbRefIdxLX = (*nbPU->get<PU::RefIdxLX>())[X];
            const auto nbRefIdxLY = (*nbPU->get<PU::RefIdxLX>())[Y];

            if(
                    nbPredFlagLX
                    &&
                    PicOrderCntVal{0}
                    == diff(refPicList[X][nbRefIdxLX], refPicList[X][refIdxLX]))
            {
                availableFlagLXB = true;
                // (8-157)
                mvLXB = (*nbPU->get<PU::MvLX>())[X];
                // (8-158)
                refIdxB = nbRefIdxLX;
            }
            else if(
                    nbPredFlagLY
                    &&
                    PicOrderCntVal{0}
                    == diff(refPicList[Y][nbRefIdxLY], refPicList[X][refIdxLX]))
            {
                availableFlagLXB = true;
                // (8-157)
                mvLXB = (*nbPU->get<PU::MvLX>())[Y];
                // (8-158)
                refIdxB = nbRefIdxLY;
            }
        }
    }

    // 4
    if(!isScaledFlagLX && availableFlagLXB)
    {
        availableFlagLXA = true;
        // (8-158)
        mvLXA = mvLXB;
    }

    // 5
    if(!isScaledFlagLX)
    {
        availableFlagLXB = false;

        for(auto k = 0; k < nbNum && !availableFlagLXB; ++k)
        {
            availableBk[k] = isAvailable(nbBk[k]);

            if(availableBk[k] && !availableFlagLXB)
            {
                const auto Y = RefList::L0 == X ? RefList::L1 : RefList::L0;
                const auto nbPU = currPic->getPredictionUnit(nbBk[k]);
                const auto nbPredFlagLX = (*nbPU->get<PU::PredFlagLX>())[X];
                const auto nbPredFlagLY = (*nbPU->get<PU::PredFlagLX>())[Y];
                const auto nbRefIdxLX = (*nbPU->get<PU::RefIdxLX>())[X];
                const auto nbRefIdxLY = (*nbPU->get<PU::RefIdxLX>())[Y];

                if(
                        nbPredFlagLX
                        &&
                        longTermRefPic(decoder, *currPic, {xPb, yPb}, X, refIdxLX)
                        == longTermRefPic(decoder, *currPic, {xPb, yPb}, X, nbRefIdxLX))
                {
                    availableFlagLXB = true;
                    // (8-162)
                    mvLXB = (*nbPU->get<PU::MvLX>())[X];
                    // (8-163)
                    refIdxB = nbRefIdxLX;
                    // (8-164)
                    refPicListB = &refPicList[X];
                }
                else if(
                        nbPredFlagLY
                        &&
                        longTermRefPic(decoder, *currPic, {xPb, yPb}, X, refIdxLX)
                        == longTermRefPic(decoder, *currPic, {xPb, yPb}, Y, nbRefIdxLY))
                {
                    availableFlagLXB = true;
                    // (8-165)
                    mvLXB = (*nbPU->get<PU::MvLX>())[Y];
                    // (8-166)
                    refIdxB = nbRefIdxLY;
                    // (8-167)
                    refPicListB = &refPicList[Y];
                }
            }

            if(
                    availableFlagLXB
                    &&
                    PicOrderCntVal{0}
                    != diff((*refPicListB)[refIdxB], refPicList[X][refIdxLX])
                    && decoder.dpb.isShortTerm((*refPicListB)[refIdxB])
                    && decoder.dpb.isShortTerm(refPicList[X][refIdxLX]))
            {
                // (8-171)
                const auto diffTd =
                    diff(currPic->order.get<PicOrderCntVal>(), (*refPicListB)[refIdxB]);
                // (8-172)
                const auto diffTb =
                    diff(currPic->order.get<PicOrderCntVal>(), refPicList[X][refIdxLX]);

                mvLXB = MvScaleCoeff{diffTd, diffTb}.scale(mvLXB);
            }
        }
    }

    return makeTuple(mvLXB, availableFlagLXB);
}
/*----------------------------------------------------------------------------*/
} /* namespace */
/*----------------------------------------------------------------------------*/
Tuple<Pair<MotionVector, Neighbour>, Pair<bool, Neighbour>> MvPredictorCandidate::exec(
        State &decoder,
        Ptr<const Structure::Picture> currPic,
        const Syntax::PredictionUnit &pu,
        RefList X, PicOrderCntVal refIdxLX)
{
    using namespace Syntax;
    using namespace Structure;

    typedef CodingUnit CU;
    typedef PredictionUnit PU;

    const auto puCoord = pu.get<PU::Coord>()->inUnits();
    const auto xPb = puCoord.x();
    const auto yPb = puCoord.y();
    const auto puWidth = pu.get<PU::Width>()->inUnits();
    const auto puHeight = pu.get<PU::Height>()->inUnits();
    const auto puPartIdx = pu.get<PU::PartIdx>()->inUnits();

    const auto cu = currPic->getCodingUnit(puCoord);
    const auto cuCoord = cu->get<CU::Coord>()->inUnits();
    const auto cuSize = cu->get<CU::Size>()->inUnits();

    const auto &rpl = currPic->slice(puCoord)->rpl;

    // wrapper for 6.4.2, used for both predictors: A and B
    const auto isAvailable =
        [currPic, cuCoord, cuSize, puCoord, puWidth, puHeight, puPartIdx](PelCoord at)
        {
            return
                currPic->isPredictionBlockAvailable(
                        cuCoord, toPel(cuSize),
                        puCoord, puWidth, puHeight, puPartIdx,
                        at);
        };

    auto isScaledFlagLX = false;

    MotionVector mvLXA;
    auto availableFlagLXA = false;

    tie(mvLXA, availableFlagLXA) =
        deriveA(
                decoder,
                currPic,
                rpl,
                xPb, yPb, puWidth, puHeight,
                X, refIdxLX,
                isScaledFlagLX,
                isAvailable);

    MotionVector mvLXB;
    auto availableFlagLXB = false;

    tie(mvLXB, availableFlagLXB) =
        deriveB(
                decoder,
                currPic,
                rpl,
                xPb, yPb, puWidth, puHeight,
                X, refIdxLX,
                isScaledFlagLX,
                mvLXA, availableFlagLXA,
                isAvailable);

    const auto toStr =
        [puCoord, mvLXA, mvLXB, availableFlagLXA, availableFlagLXB](std::ostream &oss)
        {
            oss
                << puCoord
                << " A " << mvLXA << ' ' << availableFlagLXA << '\n'
                << puCoord
                << " B " << mvLXB << ' ' << availableFlagLXB << '\n';
        };

    log(LogId::MvPredictorCandidate, toStr);

    return
        makeTuple(
                Pair<MotionVector, Neighbour>{mvLXA, mvLXB},
                Pair<bool, Neighbour>{availableFlagLXA, availableFlagLXB});
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */
