#include <Decoder/Processes/ReferencePictureSet.h>
#include <Decoder/State.h>
#include <Syntax/SequenceParameterSet.h>
#include <Syntax/SliceSegmentHeader.h>
#include <Structure/Picture.h>
#include <Structure/RPS.h>
#include <Structure/RPSP.h>
/* STDC++ */
#include <sstream>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/

namespace {
/*----------------------------------------------------------------------------*/
Structure::RPS::POC calcPOC(
        Structure::RPS::POC &poc,
        const Structure::Picture &picture,
        const Structure::RPSP &rpsp,
        const Syntax::SliceSegmentHeader &sh,
        int rpsIdx)
{
    using namespace Syntax;

    typedef SliceSegmentHeader SSH;

    const Structure::RPSP::Entry &curr = rpsp.entry(rpsIdx);
    const auto picOrderCntVal = picture.order.get<PicOrderCntVal>();
    const auto picOrderCntLsb = picture.order.get<PicOrderCntLsb>();
    const auto maxPicOrderCntLsb = picture.maxPicOrderCntLsb;

    auto j = 0;
    auto k = 0;

    for(int i = 0; i < curr.numNegativePics; ++i)
    {
        if(curr.usedByCurrPicS0[i])
        {
            poc.stCurrBefore[j++] = picOrderCntVal.value + curr.deltaPocS0[i];
        }
        else
        {
            poc.stFoll[k++] = picOrderCntVal.value + curr.deltaPocS0[i];
        }
    }

    poc.numStCurrBefore = j;

    j = 0;

    for(auto i = 0; i < curr.numPositivePics; ++i)
    {
        if(curr.usedByCurrPicS1[i])
        {
            poc.stCurrAfter[j++] = picOrderCntVal.value + curr.deltaPocS1[i];
        }
        else
        {
            poc.stFoll[k++] = picOrderCntVal.value + curr.deltaPocS1[i];
        }
    }

    poc.numStCurrAfter = j;
    poc.numStFoll = k;

    const auto numLongTermSps = sh.get<SSH::NumLongTermSps>();
    const auto numLongTermPics = sh.get<SSH::NumLongTermPics>();
    const auto numLongTerm = *numLongTermSps + *numLongTermPics;

    j = 0;
    k = 0;

    for(auto i = 0; i < numLongTerm; ++i)
    {
        PicOrderCntBase::ValueType pocLt = rpsp.pocLsbLt[i];
        const auto deltaPocMsbPresentFlag = sh.get<SSH::DeltaPocMsbPresentFlag>();

        if((*deltaPocMsbPresentFlag)[i])
        {
            pocLt +=
                picOrderCntVal.value
                - maxPicOrderCntLsb.value * rpsp.deltaPocMsbCycleLt[i]
                - picOrderCntLsb.value;
        }

        if(rpsp.usedByCurrPicLt[i])
        {
            poc.ltCurr[j] = pocLt;
            poc.currDeltaMsbPresentFlag[j++] = (*deltaPocMsbPresentFlag)[i];
        }
        else
        {
            poc.ltFoll[k] = pocLt;
            poc.follDeltaMsbPresentFlag[k++] = (*deltaPocMsbPresentFlag)[i];
        }
    }

    poc.numLtCurr = j;
    poc.numLtFoll = k;

    return poc;
}
/*----------------------------------------------------------------------------*/
void makeLongTerm(State &decoder, Structure::RPS &rps, const Structure::RPS::POC &poc)
{
    auto &dpb = decoder.dpb;

    for(auto i = 0; i < rps.numLtCurr; ++i)
    {
        if(!poc.currDeltaMsbPresentFlag[i])
        {
            const auto picture = dpb.picture(PicOrderCntLsb(poc.ltCurr[i]));

            if(picture)
            {
                dpb.mark(picture->order.get<PicOrderCntVal>(), RefPicType::LongTerm);
                rps.ltCurr[i] = picture->order.get<PicOrderCntVal>();
            }
        }
        else
        {
            const auto picture = dpb.picture(PicOrderCntVal(poc.ltCurr[i]));

            if(picture)
            {
                dpb.mark(picture->order.get<PicOrderCntVal>(), RefPicType::LongTerm);
                rps.ltCurr[i] = picture->order.get<PicOrderCntVal>();
            }
        }
    }

    for(auto i = 0; i < rps.numLtFoll; ++i)
    {
        if(!poc.follDeltaMsbPresentFlag[i])
        {
            const auto picture = dpb.picture(PicOrderCntLsb(poc.ltFoll[i]));

            if(picture)
            {
                dpb.mark(picture->order.get<PicOrderCntVal>(), RefPicType::LongTerm);
                rps.ltFoll[i] = picture->order.get<PicOrderCntVal>();
            }
            else
            {
                /* 04/2013, 8.3.3.1
                 * "General decoding process for generating unavailable reference pictures"
                 * WARNING: 8.3.3.2 is not implemented (not required) */
                rps.ltFoll[i] = poc.ltFoll[i];
            }
        }
        else
        {
            const auto picture = dpb.picture(PicOrderCntVal(poc.ltFoll[i]));

            if(picture)
            {
                dpb.mark(picture->order.get<PicOrderCntVal>(), RefPicType::LongTerm);
                rps.ltFoll[i] = picture->order.get<PicOrderCntVal>();
            }
            else
            {
                /* 04/2013, 8.3.3.1
                 * "General decoding process for generating unavailable reference pictures"
                 * WARNING: 8.3.3.2 is not implemented (not required) */
                rps.ltFoll[i] = poc.ltFoll[i];
            }
        }
    }
}
/*----------------------------------------------------------------------------*/
void makeShortTerm(
        State &decoder,
        const Structure::RPS::POC::StPOC &list,
        int num,
        Structure::RPS::StList &dst, bool isStFoll = false)
{
    auto &dpb = decoder.dpb;

    for(auto i = 0; i < num; ++i)
    {
        const auto picture = dpb.picture(PicOrderCntVal(list[i]));

        if(picture)
        {
            dpb.mark(picture->order.get<PicOrderCntVal>(), RefPicType::ShortTerm);
            dst[i] = picture->order.get<PicOrderCntVal>();
        }
        else if(isStFoll)
        {
            /* 04/2013, 8.3.3.1
             * "General decoding process for generating unavailable reference pictures"
             * WARNING: 8.3.3.2 is not implemented (not required) */
            dst[i] = PicOrderCntVal(list[i]);
        }
    }
}
/*----------------------------------------------------------------------------*/
void makeUnused(State &decoder, const Structure::RPS &rps)
{
    auto &dpb = decoder.dpb;

    for(const auto &entry : dpb)
    {
        const auto au = entry.au();

        if(isUnused(au->picture()->reference.get<RefPicType>()))
        {
            continue;
        }

        const auto picOrderCntVal = au->picture()->order.get<PicOrderCntVal>();
        bool unused = true;

        for(auto i = 0; unused && i < rps.numStCurrBefore; ++i)
        {
            if(
                    rps.stCurrBefore[i]
                    && *rps.stCurrBefore[i] == picOrderCntVal)
            {
                unused = false;
                break;
            }
        }

        for(auto i = 0; unused && i < rps.numStCurrAfter; ++i)
        {
            if(
                    rps.stCurrAfter[i]
                    && *rps.stCurrAfter[i] == picOrderCntVal)
            {
                unused = false;
                break;
            }
        }

        for(auto i = 0; unused && i < rps.numStFoll; ++i)
        {
            if(
                    rps.stFoll[i]
                    && *rps.stFoll[i] == picOrderCntVal)
            {
                unused = false;
                break;
            }
        }

        for(auto i = 0; unused && i < rps.numLtCurr; ++i)
        {
            if(
                    rps.ltCurr[i]
                    && *rps.ltCurr[i] == picOrderCntVal)
            {
                unused = false;
                break;
            }
        }

        for(auto i = 0; unused && i < rps.numLtFoll; ++i)
        {
            if(
                    rps.ltFoll[i]
                    && *rps.ltFoll[i] == picOrderCntVal)
            {
                unused = false;
                break;
            }
        }

        if(unused)
        {
            dpb.mark(picOrderCntVal, RefPicType::Unused);
        }
    }
}
/*----------------------------------------------------------------------------*/
void sanitize(const Structure::RPS::POC &poc, PicOrderCntBase::ValueType maxPicOrderCntLsb)
{
    for(auto i = 0; i < poc.numLtCurr; ++i)
    {
        if(poc.currDeltaMsbPresentFlag[i])
        {
            for(auto j = 0; j < poc.numStCurrBefore; ++j)
            {
                runtime_assert(poc.ltCurr[i] != poc.stCurrBefore[j]);
            }

            for(auto j = 0; j < poc.numStCurrAfter; ++j)
            {
                runtime_assert(poc.ltCurr[i] != poc.stCurrAfter[j]);
            }

            for(auto j = 0; j < poc.numStFoll; ++j)
            {
                runtime_assert(poc.ltCurr[i] != poc.stFoll[j]);
            }

            for(auto j = 0; j < poc.numLtCurr; ++j)
            {
                if(i != j)
                {
                    runtime_assert(poc.ltCurr[i] != poc.ltCurr[j]);
                }
            }
        }
    }

    for(auto i = 0; i < poc.numLtFoll; ++i)
    {
        if(poc.follDeltaMsbPresentFlag[i])
        {
            for(auto j = 0; j < poc.numStCurrBefore; ++j)
            {
                runtime_assert(poc.ltFoll[i] != poc.stCurrBefore[j]);
            }

            for(auto j = 0; j < poc.numStCurrAfter; ++j)
            {
                runtime_assert(poc.ltFoll[i] != poc.stCurrAfter[j]);
            }

            for(auto j = 0; j < poc.numStFoll; ++j)
            {
                runtime_assert(poc.ltFoll[i] != poc.stFoll[j]);
            }

            for(auto j = 0; j < poc.numLtFoll; ++j)
            {
                if(i != j)
                {
                    runtime_assert(poc.ltFoll[i] != poc.ltFoll[j]);
                }
            }

            for(auto j = 0; j < poc.numLtCurr; ++j)
            {
                runtime_assert(poc.ltFoll[i] != poc.ltCurr[j]);
            }
        }
    }

    for(auto i = 0; i < poc.numLtCurr; ++i)
    {
        if(!poc.currDeltaMsbPresentFlag[i])
        {
            for(auto j = 0; j < poc.numStCurrBefore; ++j)
            {
                runtime_assert(poc.ltCurr[i] != (poc.stCurrBefore[j] & (maxPicOrderCntLsb - 1)));
            }

            for(auto j = 0; j < poc.numStCurrAfter; ++j)
            {
                runtime_assert(poc.ltCurr[i] != (poc.stCurrAfter[j] & (maxPicOrderCntLsb - 1)));
            }

            for(auto j = 0; j < poc.numStFoll; ++j)
            {
                runtime_assert(poc.ltCurr[i] != (poc.stFoll[j] & (maxPicOrderCntLsb - 1)));
            }

            for(auto j = 0; j < poc.numLtCurr; ++j)
            {
                if(i != j)
                {
                    runtime_assert(poc.ltCurr[i] != (poc.ltCurr[j] & (maxPicOrderCntLsb - 1)));
                }
            }
        }
    }

    for(auto i = 0; i < poc.numLtFoll; ++i)
    {
        if(!poc.follDeltaMsbPresentFlag[i])
        {
            for(auto j = 0; j < poc.numStCurrBefore; ++j)
            {
                runtime_assert(poc.ltFoll[i] != (poc.stCurrBefore[j] & (maxPicOrderCntLsb - 1)));
            }

            for(auto j = 0; j < poc.numStCurrAfter; ++j)
            {
                runtime_assert(poc.ltFoll[i] != (poc.stCurrAfter[j] & (maxPicOrderCntLsb - 1)));
            }

            for(auto j = 0; j < poc.numStFoll; ++j)
            {
                runtime_assert(poc.ltFoll[i] != (poc.stFoll[j] & (maxPicOrderCntLsb - 1)));
            }

            for(auto j = 0; j < poc.numLtFoll; ++j)
            {
                if(i != j)
                {
                    runtime_assert(poc.ltFoll[i] != (poc.ltFoll[j] & (maxPicOrderCntLsb - 1)));
                }
            }

            for(auto j = 0; j < poc.numLtCurr; ++j)
            {
                runtime_assert(poc.ltFoll[i] != (poc.ltCurr[j] & (maxPicOrderCntLsb - 1)));
            }
        }
    }
}
/*----------------------------------------------------------------------------*/
void sanitize(const Structure::RPS &rps)
{
    /* 04/2013, 8.3.2 "Decoding process for reference picture set"
     *
     * It is a requirement of bitstream conformance that the RPS is restricted
     * as follows:
     * There shall be no entry in
     * RefPicSetStCurrBefore, RefPicSetStCurrAfter, or RefPicSetLtCurr
     * for which one or more of the following are true:
     * - The entry is equal to "no reference picture".
     * TODO
     * - The entry is a sub-layer non-reference picture and has TemporalId
     *   equal to that of the current picture.
     * TODO
     * - The entry is a picture that has TemporalId greater than that of
     *   the current picture. */

    for(auto i = 0; i < rps.numStCurrBefore; ++i)
    {
        runtime_assert(rps.stCurrBefore[i]);
    }

    for(auto i = 0; i < rps.numStCurrAfter; ++i)
    {
        runtime_assert(rps.stCurrAfter[i]);
    }

    for(auto i = 0; i < rps.numLtCurr; ++i)
    {
        runtime_assert(rps.ltCurr[i]);
    }
}
/*----------------------------------------------------------------------------*/
} /* namespace */

/*----------------------------------------------------------------------------*/
void ReferencePictureSet::exec(
        State &decoder,
        Ptr<Structure::Picture> picture,
        const Syntax::SliceSegmentHeader &sh)
{
    using namespace Syntax;

    typedef SliceSegmentHeader SSH;

    auto &dpb = decoder.dpb;
    const auto nalUnitType = picture->nalUnitType;

    if(isIRAP(nalUnitType) && picture->noRaslOutputFlag)
    {
        dpb.markAll(RefPicType::Unused);
    }

    if(!isIDR(nalUnitType))
    {
        const auto rpsIdx = sh.get<SSH::CurrRpsIdx>();

        {
            dpb.markAll(RefPicType::Unused);
        }

        using namespace Structure;

        const auto &rpsp = *decoder.context()->get<RPSP>();
        RPS::POC poc;

        calcPOC(poc, *picture, rpsp, sh, *rpsIdx);

        log(
                LogId::ReferencePictureSetParam,
                [&poc](std::ostream &oss){poc.toStr(oss);},
                '\n');

        sanitize(poc, picture->maxPicOrderCntLsb.value);

        if(isBLA(nalUnitType) || isCRA(nalUnitType))
        {
            runtime_assert(0 == rpsp.numPocTotalCurr);
        }

        auto &rps = *picture->rps();

        rps.numStCurrBefore = poc.numStCurrBefore;
        rps.numStCurrAfter = poc.numStCurrAfter;
        rps.numStFoll = poc.numStFoll;
        rps.numLtCurr = poc.numLtCurr;
        rps.numLtFoll = poc.numLtFoll;

        makeLongTerm(decoder, rps, poc);
        makeShortTerm(decoder, poc.stCurrBefore, rps.numStCurrBefore, rps.stCurrBefore);
        makeShortTerm(decoder, poc.stCurrAfter, rps.numStCurrAfter, rps.stCurrAfter);
        makeShortTerm(decoder, poc.stFoll, rps.numStFoll, rps.stFoll, true);
        makeUnused(decoder, rps);
        sanitize(rps);

        const auto toStr =
            [&rps, picture](std::ostream &oss)
            {
                oss << picture->order.get<PicOrderCntVal>() << " [";

                for(auto i = 0; i < rps.numStCurrBefore; ++i)
                {
                    oss << rps.stCurrBefore[i] << " ";
                }

                oss << "] [";

                for(auto i = 0; i < rps.numStCurrAfter; ++i)
                {
                    oss << rps.stCurrAfter[i] << " ";
                }

                oss << "] [";

                for(auto i = 0; i < rps.numLtCurr; ++i)
                {
                    oss << rps.ltCurr[i] << " ";
                }

                oss << "]\n";
            };

        if(!isI(*sh.get<SSH::SliceType>()))
        {
            log(LogId::ReferencePictureSet, toStr);
        }
    }

    /* mark current picture as a short term reference (C.3.4)
     *
     * WARNING: this is inconsistent with:
     *
     * 8.1 "General decoding process" 4:
     * 4. "After all slices of the current picture have been decoded,
     * the decoded picture is marked as "used for short-term reference".*/
    picture->reference.get<RefPicType>() = RefPicType::ShortTerm;
}
/*----------------------------------------------------------------------------*/
}}} // namespace HEVC::Decoder::Processes
