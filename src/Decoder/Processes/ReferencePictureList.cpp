#include <Decoder/Processes/ReferencePictureList.h>
#include <Decoder/State.h>
#include <Structure/Picture.h>
#include <Structure/RPL.h>
#include <Structure/RPSP.h>
#include <Structure/RPS.h>
#include <Syntax/RefPicListModification.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
void ReferencePictureList::exec(
        State &decoder, Ptr<Structure::Picture> picture,
        Ptr<Structure::Slice> slice,
        const Syntax::SliceSegmentHeader &sh)
{
    using namespace Syntax;
    using namespace Structure;

    typedef PictureParameterSet PPS;
    typedef SliceSegmentHeader SSH;
    typedef RefPicListModification RPLM;

    const SliceType sliceType = *sh.get<SSH::SliceType>();

    runtime_assert(!isI(sliceType));

    auto &rpl = slice->rpl;
    const auto &rpsp = *decoder.context()->get<RPSP>();
    const auto pps = picture->pps;
    const auto &rps = *picture->rps();
    const bool listsModificationPresentFlag(*pps->get<PPS::ListsModificationPresentFlag>());
    const auto numPocTotalCurr = rpsp.numPocTotalCurr;

    const auto rplm =
        listsModificationPresentFlag && numPocTotalCurr > 1
        ? sh.getSubtree<RefPicListModification>()
        : nullptr;

    /* RefPicList0 */
    const auto numRefIdxL0ActiveMinus1 = sh.get<SSH::NumRefIdxL0ActiveMinus1>();
    const auto numRpsCurrTempList0 = std::max(*numRefIdxL0ActiveMinus1 + 1, numPocTotalCurr + 0);

    RPL::List temp0;

    /* 8-8 */
    auto rIdx = 0;

    while(rIdx < numRpsCurrTempList0)
    {
        for(
                auto i = 0;
                i < rps.numStCurrBefore && rIdx < numRpsCurrTempList0;
                ++rIdx, ++i)
        {
            temp0.insert(rIdx, *rps.stCurrBefore[i]);
        }

        for(
                auto i = 0;
                i < rps.numStCurrAfter && rIdx < numRpsCurrTempList0;
                ++rIdx, ++i)
        {
            temp0.insert(rIdx, *rps.stCurrAfter[i]);
        }

        for(
                auto i = 0;
                i < rps.numLtCurr && rIdx < numRpsCurrTempList0;
                ++rIdx, ++i)
        {
            temp0.insert(rIdx, *rps.ltCurr[i]);
        }
    }

    if(rplm)
    {
        const auto flagL0 = rplm->get<RPLM::RefPicListModificationFlagL0>();
        const auto listEntryL0 = rplm->get<RPLM::ListEntryL0>();

        /* 8-9 */
        for(rIdx = 0; rIdx <= *numRefIdxL0ActiveMinus1; ++rIdx)
        {
            rpl.insert(
                    RefList::L0, rIdx,
                    *flagL0 ? temp0[(*listEntryL0)[rIdx]] : temp0[rIdx]);
        }
    }
    else
    {
        /* 8-9 */
        for (rIdx = 0; rIdx <= *numRefIdxL0ActiveMinus1; ++rIdx)
        {
            rpl.insert(RefList::L0, rIdx, temp0[rIdx]);
        }
    }

    if(isB(sliceType))
    {
        /* RefPicList1 */
        const auto numRefIdxL1ActiveMinus1 = sh.get<SSH::NumRefIdxL1ActiveMinus1>();
        const auto numRpsCurrTempList1 = std::max(*numRefIdxL1ActiveMinus1 + 1, numPocTotalCurr + 0);

        RPL::List temp1;

        /* 8-10 */
        rIdx = 0;

        while(rIdx < numRpsCurrTempList1)
        {
            for(
                    auto i = 0;
                    i < rps.numStCurrAfter && rIdx < numRpsCurrTempList1;
                    ++rIdx, ++i)
            {
                temp1.insert(rIdx, *rps.stCurrAfter[i]);
            }

            for(
                    auto i = 0;
                    i < rps.numStCurrBefore && rIdx < numRpsCurrTempList1;
                    ++rIdx, ++i)
            {
                temp1.insert(rIdx, *rps.stCurrBefore[i]);
            }

            for(
                    auto i = 0;
                    i < rps.numLtCurr && rIdx < numRpsCurrTempList1;
                    ++rIdx, ++i)
            {
                temp1.insert(rIdx, *rps.ltCurr[i]);
            }
        }

        if(rplm)
        {
            const auto flagL1 = rplm->get<RPLM::RefPicListModificationFlagL1>();
            const auto listEntryL1 = rplm->get<RPLM::ListEntryL1>();

            /* 8-11 */
            for(rIdx = 0; rIdx <= *numRefIdxL1ActiveMinus1; ++rIdx)
            {
                rpl.insert(
                        RefList::L1, rIdx,
                        *flagL1 ? temp1[(*listEntryL1)[rIdx]] : temp1[rIdx]);
            }
        }
        else
        {
            /* 8-11 */
            for(rIdx = 0; rIdx <= *numRefIdxL1ActiveMinus1; ++rIdx)
            {
                rpl.insert(RefList::L1, rIdx, temp1[rIdx]);
            }
        }
    }

    const auto toStr =
        [sliceType, &rpl](std::ostream &oss)
        {
            oss
                << (isP(sliceType) ? 'P' : 'B') << '\n'
                << "L0 " << rpl.size(RefList::L0) << " [";

            for (auto idx = 0; idx < rpl.size(RefList::L0); ++idx)
            {
                oss << rpl[RefList::L0][idx] << " ";
            }

            oss << "]\n";

            if(isB(sliceType))
            {
                oss << "L1 " << rpl.size(RefList::L1) << " [";

                for(auto idx = 0; idx < rpl.size(RefList::L1); ++idx)
                {
                    oss << rpl[RefList::L1][idx] << " ";
                }

                oss << "]";
            }

            oss << '\n';
        };

    log(LogId::ReferencePictureList, toStr);
}
/*----------------------------------------------------------------------------*/
}}} // namespace HEVC::Decoder::Processes
