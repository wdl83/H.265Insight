#include <Decoder/Processes/MvTemporalPrediction.h>
#include <Decoder/Processes/MvCollocated.h>
#include <Decoder/Process.h>
#include <Decoder/State.h>
#include <Syntax/SliceSegmentHeader.h>
#include <Syntax/PredictionUnit.h>
#include <Structure/Picture.h>
#include <log.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
Tuple<MotionVector, bool> MvTemporalPrediction::exec(
        State &decoder, Ptr<const Structure::Picture> picture,
        PelCoord puCoord, Pel nPbW, Pel nPbH,
        RefList X, PicOrderCntVal refIdx)
{
    using namespace Syntax;
    using namespace Structure;

    typedef SliceSegmentHeader SSH;
    typedef PredictionUnit PU;

    const auto xPb = puCoord.x();
    const auto yPb = puCoord.y();
    const auto &rpl = picture->slice(puCoord)->rpl;

    MotionVector mvLXCol;
    auto availableFlagLXCol = false;

    const auto sh = picture->slice(puCoord)->header();
    const auto sliceTemporalMvpEnabledFlag = sh->get<SSH::SliceTemporalMvpEnableFlag>();

    if(*sliceTemporalMvpEnabledFlag)
    {
        // 1
        const auto sliceType = sh->get<SSH::SliceType>();
        const auto collocatedFromL0Flag = sh->get<SSH::CollocatedFromL0Flag>();
        const auto collocatedRefIdx = sh->get<SSH::CollocatedRefIdx>();

        const auto colPic =
            isB(*sliceType) && !(*collocatedFromL0Flag)
            ? decoder.dpb.picture(rpl[RefList::L1][*collocatedRefIdx])
            : decoder.dpb.picture(rpl[RefList::L0][*collocatedRefIdx]);

        runtime_assert(!isUnused(colPic->reference.get<RefPicType>()));

        auto deriveCollocated =
            [&](Pel x, Pel y)
            {
                const PelCoord colPbCoord((x >> 4) << 4, (y >> 4) << 4);

                // call 8.5.3.2.8
                tie(mvLXCol, availableFlagLXCol) =
                    subprocess(
                            decoder, MvCollocated(),
                            picture, colPic, puCoord, colPbCoord, X, refIdx);
            };

        // 2
        // (8-173), (8-174)
        const auto xColBr = xPb + nPbW;
        const auto yColBr = yPb + nPbH;

        if(
                picture->toCtb(yPb) == picture->toCtb(yColBr)
                && picture->encloses({xColBr, yColBr}))
        {
            deriveCollocated(xColBr, yColBr);
        }

        // 3
        if(!availableFlagLXCol)
        {
            // (8-175), (8-176)
            deriveCollocated(xPb + (nPbW >> 1), yPb + (nPbH >> 1));
        }
    }

    const auto toStr =
        [puCoord, X, mvLXCol, availableFlagLXCol](std::ostream &oss)
        {
            oss
                << puCoord
                << " L" << int(X) << mvLXCol << ' ' << availableFlagLXCol << '\n';
        };

    log(LogId::MvTemporalPrediction, toStr);

    return makeTuple(mvLXCol, availableFlagLXCol);
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */
