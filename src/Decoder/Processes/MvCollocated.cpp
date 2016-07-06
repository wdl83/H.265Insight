#include <Decoder/Processes/MvCollocated.h>
#include <Decoder/State.h>
#include <Syntax/SliceSegmentHeader.h>
#include <Syntax/CodingUnit.h>
#include <Syntax/PredictionUnit.h>
#include <Structure/Picture.h>
#include <Structure/RPL.h>
/* STDC++ */
#include <algorithm>


namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
Tuple<MotionVector, bool> MvCollocated::exec(
        State &decoder,
        Ptr<const Structure::Picture> picture, Ptr<const Structure::Picture> cpicture,
        PelCoord puCoord, PelCoord cpuCoord,
        RefList X, PicOrderCntVal refIdx)
{
    using namespace Syntax;

    typedef SliceSegmentHeader SSH;
    typedef PredictionUnit PU;

    using namespace Structure;

    auto mvL = MotionVector{};
    auto availableFlagL = false;

    const auto cCu = cpicture->getCodingUnit(cpuCoord);

    {
        const auto toStr =
            [cpicture, puCoord, cpuCoord](std::ostream &oss)
            {
                oss
                    << puCoord << '\n'
                    << "cNo " << cpicture->decodingNo << '\n'
                    << "cPU "  << cpuCoord << '\n';
            };

        log(LogId::MvCollocated, toStr);
    }

    if(PredictionMode::Intra != *cCu->get<CodingUnit::CuPredMode>())
    {
        const auto &rpl = picture->slice(puCoord)->rpl;
        const auto cPb = cpicture->getPredictionUnit(cpuCoord);
        const auto cPredFlagL = cPb->get<PU::PredFlagLX>();
        const auto cMvL = cPb->get<PU::MvLX>();
        const auto cRefIdxL = cPb->get<PU::RefIdxLX>();

        // default values if predFlagLXCol[0] == 0
        auto cmvL = (*cMvL)[RefList::L1];
        auto crefIdx = (*cRefIdxL)[RefList::L1];
        auto cL = RefList::L1;

        if((*cPredFlagL)[RefList::L0] && !(*cPredFlagL)[RefList::L1])
        {
            cmvL = (*cMvL)[RefList::L0];
            crefIdx = (*cRefIdxL)[RefList::L0];
            cL = RefList::L0;
        }
        else if((*cPredFlagL)[RefList::L0] && (*cPredFlagL)[RefList::L1])
        {
            auto diffsAreLessThanOrEqual = true;
            const auto picOrderCntVal = picture->order.get<PicOrderCntVal>();

            for(auto i : EnumRange<RefList>())
            {
                const auto j =
                    std::find_if(
                            std::begin(rpl[i]), std::end(rpl[i]),
                            [picOrderCntVal](PicOrderCntVal p)
                            {
                                return PicOrderCntVal{0} < diff(p, picOrderCntVal);
                            });

                if(j != std::end(rpl[i]))
                {
                    diffsAreLessThanOrEqual = false;
                    break;
                }
            }

            if(diffsAreLessThanOrEqual)
            {
                cmvL = (*cMvL)[X];
                crefIdx = (*cRefIdxL)[X];
                cL = X;
            }
            else
            {
                const auto sh = picture->slice(puCoord)->header();
                const auto collocatedFromL0Flag = sh->get<SSH::CollocatedFromL0Flag>();
                /* WARNING:
                 * collocated_from_l0_flag specifies that the collocated picture
                 * used for temporal motion vector prediction is derived from
                 * reference picture list 0 (but here it is used only as value) */
                const auto N = *collocatedFromL0Flag ? RefList::L1 : RefList::L0;

                cmvL = (*cMvL)[N];
                crefIdx = (*cRefIdxL)[N];
                cL = N;
            }
        }

        if(
                longTermRefPic(decoder, *picture, puCoord, X, refIdx)
                != longTermRefPic(decoder, *cpicture, cpuCoord, cL, crefIdx))
        {
            //cmvL = MotionVector{};
            //availableFlagL = false;
        }
        else
        {
            availableFlagL = true;

            /* WARNING:
             * 04/2013, 8.5.3.2.8 "Derivation process for collocated motion vectors"
             * says currPb (puCoord) but it is a BUG it should be colPb (cpuCoord)
             *
             * This is corrected in HEVCv2 (10/2014) 8.5.3.2.9 */
            const auto &crpl = cpicture->slice(cpuCoord)->rpl;
            // (8-177), (8-178)
            const auto cPocDiff =
                diff(cpicture->order.get<PicOrderCntVal>(), crpl[cL][crefIdx]);
            const auto pocDiff =
                diff(picture->order.get<PicOrderCntVal>(), rpl[X][refIdx]);

            if(
                    decoder.dpb.isLongTerm(rpl[X][refIdx])
                    || cPocDiff == pocDiff)
            {
                mvL = cmvL;
            }
            else
            {
                const MvScaleCoeff scaleCoeff{cPocDiff, pocDiff};
                const auto toStr =
                    [&scaleCoeff](std::ostream &oss)
                    {
                        scaleCoeff.toStr(oss);
                        oss << '\n';
                    };

                log(LogId::MvCollocated, toStr);
                mvL = scaleCoeff.scale(cmvL);
            }
        }
    }

    const auto toStr =
        [X, mvL, availableFlagL](std::ostream &oss)
        {
            oss
                << 'L' << int(X) << ' '
                << availableFlagL << ' ' << mvL << '\n';
        };

    log(LogId::MvCollocated, toStr);

    return makeTuple(mvL, availableFlagL);
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */
