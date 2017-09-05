#include <Decoder/Processes/MvCombinedBiPredMergeCandidate.h>
#include <Decoder/State.h>
#include <Syntax/PredictionUnit.h>
#include <Structure/Picture.h>
#include <Structure/RPL.h>


namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
MvMergeCandList MvCombinedBiPredMergeCandidate::exec(
        State &, Ptr<const Structure::Picture> picture,
        PelCoord puCoord,
        MvMergeCandList candList, int numOrigMergeCand, int maxNumMergeCand)
{
    using namespace Syntax;

    typedef SliceSegmentHeader SSH;
    typedef PredictionUnit PU;

    const auto slice = picture->slice(puCoord);

    if(1 < numOrigMergeCand && maxNumMergeCand > numOrigMergeCand)
    {
        const auto &rpl = slice->rpl;
        const auto diff =
            [&rpl](PicOrderCntVal x, PicOrderCntVal y)
            {
                return HEVC::Decoder::diff(rpl[RefList::L0][x], rpl[RefList::L1][y]);
            };

        // Table 8-6 "Specification of l0CandIdx and l1CandIdx"
        static const std::array<Pair<int, RefList>, 12> lCandIdx =
        {
            {
                {0, 1}, {1, 0},
                {0, 2}, {2, 0},
                {1, 2}, {2, 1},
                {0, 3}, {3, 0},
                {1, 3}, {3, 1},
                {2, 3}, {3, 2}
            }
        };

        auto combIdx = 0;
        auto combStop = false;

        while(!combStop)
        {
            const auto mvL0Cand = candList.mvL[lCandIdx[combIdx][RefList::L0]][RefList::L0];
            const auto mvL1Cand = candList.mvL[lCandIdx[combIdx][RefList::L1]][RefList::L1];
            const auto predFlagL0Cand = candList.predFlagL[lCandIdx[combIdx][RefList::L0]][RefList::L0];
            const auto predFlagL1Cand = candList.predFlagL[lCandIdx[combIdx][RefList::L1]][RefList::L1];
            const auto refIdxL0Cand = candList.refIdxL[lCandIdx[combIdx][RefList::L0]][RefList::L0];
            const auto refIdxL1Cand = candList.refIdxL[lCandIdx[combIdx][RefList::L1]][RefList::L1];

            if(
                    predFlagL0Cand && predFlagL1Cand
                    && (
                        PicOrderCntVal{0} != diff(refIdxL0Cand, refIdxL1Cand)
                        || mvL0Cand != mvL1Cand))
            {
                candList.refIdxL[candList.numCurrMergeCand] = {refIdxL0Cand, refIdxL1Cand};
                candList.predFlagL[candList.numCurrMergeCand] = {predFlagL0Cand, predFlagL1Cand};
                candList.mvL[candList.numCurrMergeCand] = {mvL0Cand, mvL1Cand};
                ++candList.numCurrMergeCand;
            }

            ++combIdx;

            if(
                    numOrigMergeCand * (numOrigMergeCand - 1) == combIdx
                    || candList.numCurrMergeCand == maxNumMergeCand)
            {
                combStop = true;
            }
        }
    }

    const auto toStr =
        [puCoord, &candList](std::ostream &oss)
        {
            oss << puCoord << ' ' << candList.toStr();
        };

    log(LogId::MvCombinedBiPredMergeCandidate, toStr);

    return candList;
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */
