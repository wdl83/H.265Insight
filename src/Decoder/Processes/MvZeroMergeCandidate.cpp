#include <Decoder/Processes/MvZeroMergeCandidate.h>
#include <Syntax/PredictionUnit.h>
#include <Structure/Picture.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
MvMergeCandList MvZeroMergeCandiate::exec(
        State &, Ptr<const Structure::Picture>,
        SliceType sliceType,
        Pair<int, RefList> numRefIdxLxActive,
        PelCoord puCoord,
        MvMergeCandList candList, int maxNumMergeCand)
{
    PicOrderCntVal numRefIdx(
            isP(sliceType)
            ? numRefIdxLxActive[RefList::L0]
            : std::min(numRefIdxLxActive[RefList::L0], numRefIdxLxActive[RefList::L1]));

    if(maxNumMergeCand > candList.numCurrMergeCand)
    {
        PicOrderCntVal zeroIdx;
        auto &i = candList.numCurrMergeCand;

        while(candList.numCurrMergeCand != maxNumMergeCand)
        {
            // 1
            if(isP(sliceType))
            {
                candList.refIdxL[i] =
                {
                    zeroIdx < numRefIdx ? zeroIdx : PicOrderCntVal{0},
                    PicOrderCntVal{-1}
                };
                candList.predFlagL[i] = {true, false};
                candList.mvL[i] = {MotionVector(), MotionVector()};
                ++i;
            }
            else if(isB(sliceType))
            {
                candList.refIdxL[i] =
                {
                    zeroIdx < numRefIdx ? zeroIdx : PicOrderCntVal{0},
                    zeroIdx < numRefIdx ? zeroIdx : PicOrderCntVal{0}
                };
                candList.predFlagL[i] = {true, true};
                candList.mvL[i] = {MotionVector(), MotionVector()};
                ++i;
            }
            else
            {
                runtime_assert(false);
            }

            // 2
            ++zeroIdx;
        }
    }

    const auto toStr =
        [puCoord, &candList](std::ostream &oss)
        {
            oss << puCoord << " [" << candList.toStr() << "]\n";
        };

    log(LogId::MvZeroMergeCandidate, toStr);

    return candList;
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */
