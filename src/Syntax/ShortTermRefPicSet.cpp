/* HEVC */
#include <Syntax/ShortTermRefPicSet.h>
#include <Structure/RPSP.h>

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
void ShortTermRefPicSet::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
        int idx, int numShortTermRefPicSets, Structure::RPSP &rpsp)
{
    /* start: derived from arguments */
    auto stRpsIdx = embed<StRpsIdx>(*this, idx);
    /* end: derived from arguments */

    /* start: inferrable */
    auto interRefPicSetPredictionFlag = embed<InterRefPicSetPredictionFlag>(*this);
    auto useDeltaFlag = embed<UseDeltaFlag>(*this);
    /* end: inferrable */

    if(0 != *stRpsIdx)
    {
        parse(streamAccessLayer, decoder, *interRefPicSetPredictionFlag);
    }

    if(*interRefPicSetPredictionFlag)
    {
        auto deltaIdxMinus1 = embed<DeltaIdxMinus1>(*this);

        if(*stRpsIdx == numShortTermRefPicSets)
        {
            parse(streamAccessLayer, decoder, *deltaIdxMinus1);
        }

        const auto refRpsIdx = embed<RefRpsIdx>(*this, *stRpsIdx, *deltaIdxMinus1);
        auto deltaRpsSign = embed<DeltaRpsSign>(*this);
        auto absDeltaRpsMinus1 = embed<AbsDeltaRpsMinus1>(*this);

        parse(streamAccessLayer, decoder, *deltaRpsSign);
        parse(streamAccessLayer, decoder, *absDeltaRpsMinus1);

        embed<DeltaRps>(*this, *deltaRpsSign, *absDeltaRpsMinus1);

        auto usedByCurrPicFlag = embed<UsedByCurrPicFlag>(*this);
        const auto numDeltaPocs = rpsp.entry(*refRpsIdx).numDeltaPocs();

        for(auto i = 0; i <= numDeltaPocs; ++i)
        {
            parse(streamAccessLayer, decoder, *usedByCurrPicFlag, i);

            if(!(*usedByCurrPicFlag)[i])
            {
                parse(streamAccessLayer, decoder, *useDeltaFlag, i);
            }
        }
    }
    else
    {
        auto numNegativePics = embed<NumNegativePics>(*this);
        auto numPositivePics = embed<NumPositivePics>(*this);

        parse(streamAccessLayer, decoder, *numNegativePics);
        parse(streamAccessLayer, decoder, *numPositivePics);

        if(0 < *numNegativePics)
        {
            auto deltaPocS0Minus1 = embed<DeltaPocS0Minus1>(*this);
            auto usedByCurrPicS0Flag = embed<UsedByCurrPicS0Flag>(*this);

            for(auto i = 0; i < *numNegativePics; ++i)
            {
                parse(streamAccessLayer, decoder, *deltaPocS0Minus1, i);
                parse(streamAccessLayer, decoder, *usedByCurrPicS0Flag, i);
            }
        }

        if(0 < *numPositivePics)
        {
            auto deltaPocS1Minus1 = embed<DeltaPocS1Minus1>(*this);
            auto usedByCurrPicS1Flag = embed<UsedByCurrPicS1Flag>(*this);

            for(auto i = 0; i < *numPositivePics; ++i)
            {
                parse(streamAccessLayer, decoder, *deltaPocS1Minus1, i);
                parse(streamAccessLayer, decoder, *usedByCurrPicS1Flag, i);
            }
        }
    }
}
/*----------------------------------------------------------------------------*/
}} // HEVC::Syntax
