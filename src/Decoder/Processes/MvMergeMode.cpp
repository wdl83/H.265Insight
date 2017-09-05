#include <Decoder/Processes/MvMergeMode.h>
#include <Decoder/Processes/MvSpatialMergeCandidate.h>
#include <Decoder/Processes/MvTemporalPrediction.h>
#include <Decoder/Processes/MvCombinedBiPredMergeCandidate.h>
#include <Decoder/Processes/MvZeroMergeCandidate.h>
#include <Decoder/Process.h>
#include <Syntax/PictureParameterSet.h>
#include <Syntax/SliceSegmentHeader.h>
#include <Syntax/CodingUnit.h>
#include <Syntax/PredictionUnit.h>
#include <Structure/Picture.h>


namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
Tuple<
    // mvL
    Pair<MotionVector, RefList>,
    // refIdxL
    Pair<PicOrderCntVal, RefList>,
    // predFlagL
    Pair<bool, RefList>>
MvMergeMode::exec(
        State &decoder, Ptr<const Structure::Picture> picture,
        const Syntax::PredictionUnit &pu)
{
    using namespace Syntax;

    typedef PictureParameterSet PPS;
    typedef SliceSegmentHeader SSH;
    typedef CodingUnit CU;
    typedef PredictionUnit PU;

    const auto pps = picture->pps;
    const auto parMgrLevel = 2_log2 + *pps->get<PPS::Log2ParallelMergeLevelMinus2>();
    const auto puCoord = pu.get<PU::Coord>()->inUnits();
    const auto puWidth = pu.get<PU::Width>()->inUnits();
    const auto puHeight = pu.get<PU::Height>()->inUnits();
    const auto cu = picture->getCodingUnit(puCoord);
    const auto cuCoord = cu->get<CU::Coord>()->inUnits();
    const auto cuSize = cu->get<CU::Size>()->inUnits();
    const auto sh = picture->slice(puCoord)->header();
    const auto sliceType = sh->get<SSH::SliceType>();
    const auto maxNumMergeCand = sh->get<SSH::MaxNumMergeCand>();
    const Pair<int, RefList> numRefIdxLxActive
    {
        *sh->get<SSH::NumRefIdxL0ActiveMinus1>() + 1,
        *sh->get<SSH::NumRefIdxL1ActiveMinus1>() + 1
    };

    const bool sharedMergeCandList = 2_log2 < parMgrLevel && 3_log2 == cuSize;

    const auto pbCoord = sharedMergeCandList ? cuCoord : puCoord;
    const auto nPbW = sharedMergeCandList ? toPel(cuSize) : puWidth;
    const auto nPbH = sharedMergeCandList ? toPel(cuSize) : puHeight;
    const auto partIdx = sharedMergeCandList ? 0 : pu.get<PU::PartIdx>()->inUnits();

    // 1
    // call 8.5.3.2.2
    auto cand =
        subprocess(
                decoder, MvSpatialMergeCandidate(),
                picture, cu, pbCoord, nPbW, nPbH, partIdx);
    // 2
    Pair<PicOrderCntVal, RefList> refIdxLCol
    {
        PicOrderCntVal{0},
        isB(*sliceType) ? PicOrderCntVal{0} : PicOrderCntVal{-1}
    };
    Pair<bool, RefList> availableFlagLCol{false, false};
    Pair<bool, RefList> predFlagLCol{false, false};
    Pair<MotionVector, RefList> mvLCol;

    // 3
    // call 8.5.3.2.7
    tie(mvLCol[RefList::L0], availableFlagLCol[RefList::L0]) =
        subprocess(
                decoder, MvTemporalPrediction(),
                picture, pbCoord, nPbW, nPbH, RefList::L0, refIdxLCol[RefList::L0]);

    // (8-86)
    auto availableFlagCol = availableFlagLCol[RefList::L0];
    // (8-87)
    predFlagLCol[RefList::L0] = availableFlagLCol[RefList::L0];

    // 4
    if(isB(*sliceType))
    {
        tie(mvLCol[RefList::L1], availableFlagLCol[RefList::L1]) =
            subprocess(
                    decoder, MvTemporalPrediction(),
                    picture, pbCoord, nPbW, nPbH, RefList::L1, refIdxLCol[RefList::L1]);

        // (8-89)
        availableFlagCol |= availableFlagLCol[RefList::L1];
        // (8-90)
        predFlagLCol[RefList::L1] = availableFlagLCol[RefList::L1];
    }

    // 5
    MvMergeCandList candList;
    auto i = 0;

    const auto appendIfAvailable =
        [&candList, &cand, &i](MvSpatialMergeCand::Location l)
        {
            if(cand.availableFlag[l])
            {
                candList.mvL[i] = cand.mvL[l];
                candList.refIdxL[i] = cand.refIdxL[l];
                candList.predFlagL[i] = cand.predFlagL[l];
                ++i;
            }
        };

    // (8-91)
    appendIfAvailable(MvSpatialMergeCand::A1);
    appendIfAvailable(MvSpatialMergeCand::B1);
    appendIfAvailable(MvSpatialMergeCand::B0);
    appendIfAvailable(MvSpatialMergeCand::A0);
    appendIfAvailable(MvSpatialMergeCand::B2);

    if(availableFlagCol)
    {
        candList.mvL[i] = mvLCol;
        candList.refIdxL[i] = refIdxLCol;
        candList.predFlagL[i] = predFlagLCol;
        ++i;
    }

    // 6
    candList.numCurrMergeCand = i;
    auto numOrigMergeCand = i;

    {
        const auto toStr =
            [&](std::ostream &oss)
            {
                oss << puCoord << " Tmprl+Sptl CAND[" << candList.toStr() << "]\n";
            };

        log(LogId::MvMergeMode, toStr);
    }

    // 7
    if(isB(*sliceType))
    {
        // call 8.5.3.2.3
        candList =
            subprocess(
                    decoder, MvCombinedBiPredMergeCandidate(),
                    picture, pbCoord, candList, numOrigMergeCand, *maxNumMergeCand);
    }

    // 8
    // call 8.5.3.2.4
    candList =
        subprocess(
                decoder, MvZeroMergeCandiate(),
                picture, *sliceType, numRefIdxLxActive, pbCoord, candList, *maxNumMergeCand);

    // 9
    const auto mergeIdx = pu.get<PU::MergeIdx>();
    auto mvL = candList.mvL[*mergeIdx];
    auto refIdxL = candList.refIdxL[*mergeIdx];
    auto predFlagL = candList.predFlagL[*mergeIdx];

    // 10
    if(
            predFlagL[RefList::L0] && predFlagL[RefList::L1]
            && 12_pel == puWidth + puHeight)
    {
        // WARNING: setting mvL to (0, 0) is not specified but is required
        mvL[RefList::L1] = MotionVector();
        refIdxL[RefList::L1] = PicOrderCntVal{-1};
        predFlagL[RefList::L1] = false;
    }

    const auto toStr =
        [&](std::ostream &oss)
        {
            oss
                << puCoord
                << " IDX " << int(*mergeIdx)
                << " CAND[" << candList.toStr() << "] ";

            for(auto X : EnumRange<RefList>())
            {
                oss
                    << puCoord
                    << " L" << int(X) << ' '
                    << mvL[X]
                    << " refIdx " << refIdxL[X] << ' '
                    << " predFlag " << predFlagL[X] << ' ';
            }

            oss << '\n';
        };

    log(LogId::MvMergeMode, toStr);

    return makeTuple(mvL, refIdxL, predFlagL);
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */
