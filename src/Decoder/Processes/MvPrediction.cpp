#include <Decoder/Processes/MvPrediction.h>
#include <Decoder/Processes/MvPredictorCandidate.h>
#include <Decoder/Processes/MvTemporalPrediction.h>
#include <Decoder/Process.h>
#include <Syntax/PredictionUnit.h>
#include <Structure/Picture.h>
#include <log.h>


namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
MotionVector MvPrediction::exec(
        State &decoder,
        Ptr<const Structure::Picture> picture,
        const Syntax::PredictionUnit &pu,
        RefList X, PicOrderCntVal refIdx)
{
    using namespace Syntax;

    typedef PredictionUnit PU;

    const auto puCoord = pu.get<PU::Coord>()->inUnits();
    const auto nPbW = pu.get<PU::Width>()->inUnits();
    const auto nPbH = pu.get<PU::Height>()->inUnits();

    // call 8.5.3.2.6
    auto cand = subprocess(decoder, MvPredictorCandidate(), picture, pu, X, refIdx);

    const auto mvLXN = cand.get<Pair<MotionVector, Neighbour>>();
    const auto availableFlagLXN = cand.get<Pair<bool, Neighbour>>();
    const auto candAvailable = availableFlagLXN[Neighbour::A] && availableFlagLXN[Neighbour::B];
    const auto candEqual = candAvailable && mvLXN[Neighbour::A] == mvLXN[Neighbour::B];

    // 2
    auto availableFlagLXCol = false;
    MotionVector mvLXCol;

    if(candAvailable && !candEqual)
    {
        // availableFlagLXCol = false;
    }
    else
    {
        // call 8.5.3.2.7
        tie(mvLXCol, availableFlagLXCol) =
            subprocess(decoder, MvTemporalPrediction(), picture, puCoord, nPbW, nPbH, X, refIdx);
    }

    // 3, 4
    std::array<MotionVector, 3> mvpListLX;
    auto i = 0;

    if(availableFlagLXN[Neighbour::A])
    {
        mvpListLX[i++] = mvLXN[Neighbour::A];
    }
    if(availableFlagLXN[Neighbour::B] && !candEqual)
    {
        mvpListLX[i++] = mvLXN[Neighbour::B];
    }
    if(availableFlagLXCol)
    {
        mvpListLX[i++] = mvLXCol;
    }

    // 5
    const auto idx =
        RefList::L0 == X
        ? bool(*pu.get<PU::MvpL0Flag>())
        : bool(*pu.get<PU::MvpL1Flag>());

    const auto toStr =
        [puCoord, X, &mvpListLX, idx](std::ostream &oss)
        {
            oss << puCoord << " L" << int(X) << ' ' << mvpListLX[idx] << '\n';
        };

    log(LogId::MvPrediction, toStr);

    return mvpListLX[idx];
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */
