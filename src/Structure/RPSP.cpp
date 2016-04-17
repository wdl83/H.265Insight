#include <Structure/RPSP.h>
#include <Syntax/SequenceParameterSet.h>
#include <Syntax/SliceSegmentHeader.h>
#include <Syntax/ShortTermRefPicSet.h>

namespace HEVC { namespace Structure {
/*----------------------------------------------------------------------------*/
void RPSP::derive(const Syntax::ShortTermRefPicSet &stRPS)
{
    using namespace Syntax;

    typedef ShortTermRefPicSet STRPS;

    /*------------------------------------------------------------------------*/
    /* deltaPocS0/1, usedByCurrPicS0/1, numNegativePics, numPositivePics */

    const auto stRpsIdx = stRPS.get<STRPS::StRpsIdx>();
    auto &dst = (*this)[*stRpsIdx];

    if(*stRPS.get<STRPS::InterRefPicSetPredictionFlag>())
    {
        const auto refRpsIdx = stRPS.get<STRPS::RefRpsIdx>();
        const auto deltaRps = stRPS.get<STRPS::DeltaRps>();
        const auto useDeltaFlag = stRPS.get<STRPS::UseDeltaFlag>();
        const auto usedByCurrPicFlag = stRPS.get<STRPS::UsedByCurrPicFlag>();
        const auto &src = (*this)[*refRpsIdx];
        auto i = 0;

        for(int j = src.numPositivePics - 1; j >= 0; --j)
        {
            auto dPoc = src.deltaPocS1[j] + *deltaRps;

            if(0 > dPoc && (*useDeltaFlag)[src.numNegativePics + j])
            {
                dst.deltaPocS0[i] =  dPoc;
                dst.usedByCurrPicS0[i++] = (*usedByCurrPicFlag)[src.numNegativePics + j];
            }
        }

        if(0 > *deltaRps && (*useDeltaFlag)[src.numDeltaPocs()])
        {
            dst.deltaPocS0[i] = *deltaRps;
            dst.usedByCurrPicS0[i++] = (*usedByCurrPicFlag)[src.numDeltaPocs()];
        }

        for(int j = 0; j < src.numNegativePics; ++j)
        {
            auto dPoc = src.deltaPocS0[j] + *deltaRps;

            if(0 > dPoc && (*useDeltaFlag)[j])
            {
                dst.deltaPocS0[i] =  dPoc;
                dst.usedByCurrPicS0[i++] = (*usedByCurrPicFlag)[j];
            }
        }

        dst.numNegativePics = i;

        i = 0;

        for(int j = src.numNegativePics - 1; j >= 0; --j)
        {
            auto dPoc = src.deltaPocS0[j] + *deltaRps;

            if(0 < dPoc && (*useDeltaFlag)[j])
            {
                dst.deltaPocS1[i] =  dPoc;
                dst.usedByCurrPicS1[i++] = (*usedByCurrPicFlag)[j];
            }
        }

        if(0 < *deltaRps && (*useDeltaFlag)[src.numDeltaPocs()])
        {
            dst.deltaPocS1[i] = *deltaRps;
            dst.usedByCurrPicS1[i++] = (*usedByCurrPicFlag)[src.numDeltaPocs()];
        }

        for(int j = 0; j < src.numPositivePics; ++j)
        {
            auto dPoc = src.deltaPocS1[j] + *deltaRps;

            if(0 < dPoc && (*useDeltaFlag)[src.numNegativePics + j])
            {
                dst.deltaPocS1[i] =  dPoc;
                dst.usedByCurrPicS1[i++] = (*usedByCurrPicFlag)[src.numNegativePics + j];
            }
        }

        dst.numPositivePics = i;
    }
    else
    {
        dst.numNegativePics = *stRPS.get<STRPS::NumNegativePics>();
        dst.numPositivePics = *stRPS.get<STRPS::NumPositivePics>();

        for(auto i = 0; i < dst.numNegativePics; ++i)
        {
            dst.usedByCurrPicS0[i] = (*stRPS.get<STRPS::UsedByCurrPicS0Flag>())[i];

            if(0 == i)
            {
                dst.deltaPocS0[i] = -((*stRPS.get<STRPS::DeltaPocS0Minus1>())[i] + 1);
            }
            else
            {
                dst.deltaPocS0[i] =
                    dst.deltaPocS0[i - 1]
                    -((*stRPS.get<STRPS::DeltaPocS0Minus1>())[i] + 1);
            }
        }

        for(auto i = 0; i < dst.numPositivePics; ++i)
        {
            dst.usedByCurrPicS1[i] = (*stRPS.get<STRPS::UsedByCurrPicS1Flag>())[i];

            if(0 == i)
            {
                dst.deltaPocS1[i] = (*stRPS.get<STRPS::DeltaPocS1Minus1>())[i] + 1;
            }
            else
            {
                dst.deltaPocS1[i] =
                    dst.deltaPocS1[i - 1]
                    + (*stRPS.get<STRPS::DeltaPocS1Minus1>())[i] + 1;
            }
        }
    }
}
/*----------------------------------------------------------------------------*/
void RPSP::derive(const Syntax::SequenceParameterSet &sps)
{
    const auto &list = sps.getSubtreeList<Syntax::ShortTermRefPicSet>();

    for(const auto &stRPS : list)
    {
        derive(*stRPS);
    }
}
/*----------------------------------------------------------------------------*/
void RPSP::derive(
        const Syntax::SequenceParameterSet &sps,
        const Syntax::SliceSegmentHeader &ssh)
{
    using namespace Syntax;

    typedef SequenceParameterSet SPS;
    typedef SliceSegmentHeader SSH;

    /*------------------------------------------------------------------------*/
    /* PocLsbLt, UsedByCurrPicLt */
    const auto numLongTermSps = ssh.get<SSH::NumLongTermSps>();
    const auto numLongTermPics = ssh.get<SSH::NumLongTermPics>();
    auto numLongTerm = *numLongTermSps + *numLongTermPics;

    for(auto i = 0; i < numLongTerm; ++i)
    {
        if(*numLongTermSps > i)
        {
            pocLsbLt[i] =
                (*sps.get<SPS::LtRefPicPocLsbSps>())[(*ssh.get<SSH::LtIdxSps>())[i]];
            usedByCurrPicLt[i] =
                (*sps.get<SPS::UsedByCurrPicLtSpsFlag>())[(*ssh.get<SSH::LtIdxSps>())[i]];
        }
        else
        {
            pocLsbLt[i] = (*ssh.get<SSH::PocLsbLt>())[i];
            usedByCurrPicLt[i] = (*ssh.get<SSH::UsedByCurrPicLtFlag>())[i];
        }
    }
    /*------------------------------------------------------------------------*/
    /* NumPocTotalCurr */
    const auto currRpsIdx = ssh.get<SSH::CurrRpsIdx>();
    auto &curr = (*this)[*currRpsIdx];

    numPocTotalCurr = 0;

    for(auto i = 0; i < curr.numNegativePics; ++i)
    {
        if(curr.usedByCurrPicS0[i])
        {
            ++numPocTotalCurr;
        }
    }

    for(auto i = 0; i < curr.numPositivePics; ++i)
    {
        if(curr.usedByCurrPicS1[i])
        {
            ++numPocTotalCurr;
        }
    }

    for(auto i = 0; i < numLongTerm; ++i)
    {
        if(usedByCurrPicLt[i])
        {
            ++numPocTotalCurr;
        }
    }
    /*------------------------------------------------------------------------*/
    /* DeltaPocMsbCycleLt */
    const auto sshDeltaPocMsbCycleLt = ssh.get<SSH::DeltaPocMsbCycleLt>();

    for(auto i = 0; i < numLongTerm; ++i)
    {
        if(0 == i || *numLongTermSps == i)
        {
            deltaPocMsbCycleLt[i] = (*sshDeltaPocMsbCycleLt)[i];
        }
        else
        {
            deltaPocMsbCycleLt[i] =
                deltaPocMsbCycleLt[i - 1]
                + (*sshDeltaPocMsbCycleLt)[i];
        }
    }

    const auto toStr =
        [this, &currRpsIdx, &curr](std::ostream &oss)
        {
            oss << "CurrRpsIdx " << int(*currRpsIdx) << '\n';
            oss << "pocLsbLt: ";

            for(auto i : pocLsbLt)
            {
                oss << i << ' ';
            }

            oss << "\ndeltaPocMsbCycleLt: ";

            for(auto i : deltaPocMsbCycleLt)
            {
                oss << i << ' ';
            }

            oss << "\nusedByCurrPicLt: ";

            for(auto i : usedByCurrPicLt)
            {
                oss << i << ' ';
            }

            oss
                << "\nnumPocTotalCurr " << numPocTotalCurr
                << " numDeltaPocs " << curr.numDeltaPocs()
                << " numNegativePics " << curr.numNegativePics
                << " numPositivePics " << curr.numPositivePics;

            for(auto i = 0; i < curr.numNegativePics; ++i)
            {
                oss << ' ' << curr.deltaPocS0[i] << ':' << curr.usedByCurrPicS0[i];
            }

            for(auto i = 0; i < curr.numPositivePics; ++i)
            {
                oss << ' ' << curr.deltaPocS1[i] << ':' << curr.usedByCurrPicS1[i];
            }

            oss << '\n';
        };

    log(LogId::ReferencePictureSetParam, toStr);
}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Structure */
