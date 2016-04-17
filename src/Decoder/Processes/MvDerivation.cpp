#include <Decoder/Processes/MvDerivation.h>
#include <Decoder/Processes/MvMergeMode.h>
#include <Decoder/Processes/MvPrediction.h>
#include <Syntax/PredictionUnit.h>
#include <Structure/Picture.h>
#include <Structure/RPL.h>
#include <log.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
Tuple<
    // mvL
    Pair<MotionVector, RefList>,
    // refIdxL
    Pair<PicOrderCntVal, RefList>,
    // predFlagL
    Pair<bool, RefList>>
MvDerivationLuma::exec(
        State &decoder,
        Ptr<const Structure::Picture> picture,
        const Syntax::PredictionUnit &pu)
{
    using namespace Syntax;

    typedef PredictionUnit PU;

    Pair<MotionVector, RefList> mvL;
    Pair<PicOrderCntVal, RefList> refIdxL{PicOrderCntVal(-1), PicOrderCntVal(-1)};
    Pair<bool, RefList> predFlagL{false, false};

    const auto mergeFlag = pu.get<PU::MergeFlag>();

    if(*mergeFlag)
    {
        // call 8.5.3.2.1
        tie(mvL, refIdxL, predFlagL) = subprocess(decoder, MvMergeMode(), picture, pu);
    }
    else
    {
        const auto interPredIdc = pu.get<PU::InterPredIdc>();
        const auto refIdxL0 = pu.get<PU::RefIdxL0>();
        const auto refIdxL1 = pu.get<PU::RefIdxL1>();

        for(auto X : EnumRange<RefList>())
        {
            auto isL0 = RefList::L0 == X;

            // 1
            if(
                    *interPredIdc == (isL0 ? InterPredIdc::PRED_L0 : InterPredIdc::PRED_L1)
                    || *interPredIdc == InterPredIdc::PRED_BI)
            {
                // (8-69)
                refIdxL[X] = isL0 ? PicOrderCntVal(*refIdxL0) : PicOrderCntVal(*refIdxL1);
                // (8-70)
                predFlagL[X] = true;

                // 2
                // (8-73), (8-74)
                const auto mvdL = (*pu.get<PU::MvdLX>())[X];
                const auto hMvdL = toUnderlying(mvdL.x());
                const auto vMvdL = toUnderlying(mvdL.y());

                // 3
                // call 8.5.3.2.5
                auto mvpL = subprocess(decoder, MvPrediction(), picture, pu, X, refIdxL[X]);

                const auto hMvpL = toUnderlying(mvpL.x());
                const auto vMvpL = toUnderlying(mvpL.y());

                // 4
                Pair<int, Direction> uL = {0, 0};
                // (8-75)
                uL[Direction::H] = (hMvpL + hMvdL + (1 << 16)) % (1 << 16);
                // (8-76)
                mvL[X].x(SubPel(uL[Direction::H] - (uL[Direction::H] >= (1 << 15) ? 1 << 16 : 0)));
                // (8-77)
                uL[Direction::V] = (vMvpL + vMvdL + (1 << 16)) % (1 << 16);
                // (8-78)
                mvL[X].y(SubPel(uL[Direction::V] - (uL[Direction::V] >= (1 << 15) ? 1 << 16 : 0)));
            }
            else
            {
                // (8-71)
                //refIdxL[X] = PicOrderCntVal(-1);
                // (8-72)
                //predFlagL[X] = false;
            }
        }
    }

    if(!predFlagL[RefList::L0])
    {
        refIdxL[RefList::L0] = PicOrderCntVal{-1};
    }

    if(!predFlagL[RefList::L1])
    {
        refIdxL[RefList::L1] = PicOrderCntVal{-1};
    }

    const auto toStr =
        [&pu, mvL, refIdxL, predFlagL](std::ostream &oss)
        {
            const auto puCoord = pu.get<PU::Coord>()->inUnits();

            for(auto X : EnumRange<RefList>())
            {
                if(predFlagL[X])
                {
                    oss << puCoord
                        << " L" << int(X) << ' '
                        << mvL[X] << ' ' << refIdxL[X] << '\n';
                }
            }
        };

    log(LogId::MotionVectors, toStr);

    if(!predFlagL[RefList::L0])
    {
        //runtime_assert(PicOrderCntVal{-1} == refIdxL[RefList::L0]);
        runtime_assert(MotionVector{} == mvL[RefList::L0]);
    }

    if(!predFlagL[RefList::L1])
    {
        //runtime_assert(PicOrderCntVal{-1} == refIdxL[RefList::L1]);
        runtime_assert(MotionVector{} == mvL[RefList::L1]);
    }

    return makeTuple(mvL, refIdxL, predFlagL);
}
/*----------------------------------------------------------------------------*/
Pair<MotionVector, RefList> MvDeriveChroma::exec(
        State &,
        Ptr<const Structure::Picture> picture,
        Pair<MotionVector, RefList> mvL)
{
    const auto chromaFormatIdc = picture->chromaFormatIdc;
    const auto is444 = ChromaFormatIdc::f444 == chromaFormatIdc;
    const auto is422 = ChromaFormatIdc::f422 == chromaFormatIdc;

    if(is444)
    {
        return
            Pair<MotionVector, RefList>
            {
                mvL[RefList::L0] * 2_sub_pel,
                mvL[RefList::L1] * 2_sub_pel
            };
    }
    else if(is422)
    {
        return
            Pair<MotionVector, RefList>
            {
                MotionVector{mvL[RefList::L0].x(), mvL[RefList::L0].y() * 2_sub_pel},
                MotionVector{mvL[RefList::L1].x(), mvL[RefList::L1].y() * 2_sub_pel}
            };
    }

    return mvL;
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */
