#include <Syntax/PredictionUnit.h>
#include <Syntax/CodingQuadTree.h>
#include <Syntax/CodingUnit.h>
#include <Decoder/State.h>
#include <Structure/Picture.h>
#include <log.h>

namespace HEVC { namespace Syntax { namespace PredictionUnitContent {
/*----------------------------------------------------------------------------*/
/* InterPredIdcDebinarizer */
/*----------------------------------------------------------------------------*/
CABAD::BinString InterPredIdcDebinarizer::getBinarization(
        StreamAccessLayer &streamAccessLayer,
        CABAD::State &state,
        Pel nPbW, Pel nPbH, int cqtDepth)
{
    /* Draft 9v13 9.2.2.10 Table 9-36 */
    auto &arithmeticDecoder = state.arithmeticDecoder;
    CABAD::BinString binarization;

    {
        const auto binValue =
            arithmeticDecoder.decodeBin(
                    streamAccessLayer,
                    state.getVariable(
                        CABAD::CtxId::inter_pred_idc,
                        12_pel != nPbW + nPbH ? cqtDepth : 4));

        binarization.pushBack(binValue);

        if(binValue || 12_pel == nPbW + nPbH)
        {
            goto done;
        }
    }

    {
        const auto binValue =
            arithmeticDecoder.decodeBin(
                    streamAccessLayer,
                    state.getVariable(CABAD::CtxId::inter_pred_idc, 4));

        binarization.pushBack(binValue);
    }

done:
    return binarization;
}
/*----------------------------------------------------------------------------*/
} /* PredictionUnitContent */

/*----------------------------------------------------------------------------*/
/* PredictionUnit */
/*----------------------------------------------------------------------------*/
void PredictionUnit::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder)
{
    const auto coord = get<Coord>()->inUnits();
    const auto width = get<Width>()->inUnits();
    const auto height = get<Height>()->inUnits();
    const auto picture = decoder.picture();
    const auto sh = picture->slice(coord)->header();
    const auto cqt = picture->getCodingQuadTree(coord);
    const auto cu = picture->getCodingUnit(coord);

    syntaxCheck(picture->widthInLumaSamples >= coord.x() + width);
    syntaxCheck(picture->heightInLumaSamples >= coord.y() + height);

    typedef SliceSegmentHeader SSH;

    const auto maxNumMergeCand = sh->get<SSH::MaxNumMergeCand>();
    const auto numRefIdxL0ActiveMinus1 = sh->get<SSH::NumRefIdxL0ActiveMinus1>();
    const auto numRefIdxL1ActiveMinus1 = sh->get<SSH::NumRefIdxL1ActiveMinus1>();
    const auto cuSkipFlag = cu->get<CodingUnit::CuSkipFlag>();
    const auto cuPredMode = cu->get<CodingUnit::CuPredMode>();

    auto mergeIdx = embed<MergeIdx>(*this, *maxNumMergeCand);
    auto mergeFlag = embed<MergeFlag>(*this, *cuPredMode);
    auto interPredIdc = embed<InterPredIdc>(*this);
    auto refIdxL0 = embed<RefIdxL0>(*this, *numRefIdxL0ActiveMinus1);
    auto refIdxL1 = embed<RefIdxL1>(*this, *numRefIdxL1ActiveMinus1);
    auto mvpL0Flag = embed<MvpL0Flag>(*this);
    auto mvpL1Flag = embed<MvpL1Flag>(*this);
    auto mvdLX = embed<MvdLX>(*this);


    if(*cuSkipFlag) /* Intra */
    {
        if(1 < *maxNumMergeCand)
        {
            parse(streamAccessLayer, decoder, *mergeIdx);
        }
    }
    else /* Inter */
    {
        parse(streamAccessLayer, decoder, *mergeFlag);

        if(*mergeFlag)
        {
            if(1 < *maxNumMergeCand)
            {
                parse(streamAccessLayer, decoder, *mergeIdx);
            }
        }
        else
        {
            const auto sliceType = sh->get<SSH::SliceType>();

            if(isB(*sliceType))
            {
                parse(
                        streamAccessLayer, decoder, *interPredIdc,
                        width, height, cqt->get<CodingQuadTree::Depth>()->inUnits());
            }

            if(HEVC::InterPredIdc::PRED_L1 != *interPredIdc)
            {
                if(0 < *numRefIdxL0ActiveMinus1)
                {
                    parse(streamAccessLayer, decoder, *refIdxL0);
                }

                auto mvdc = embedSubtreeInList<MvdCoding>(*this, coord, RefList::L0);

                parse(streamAccessLayer, decoder, *mvdc);
                (*mvdLX)[RefList::L0] = *mvdc->get<MvdCoding::LMvd>();
                parse(streamAccessLayer, decoder, *mvpL0Flag);
            }

            if(HEVC::InterPredIdc::PRED_L0 != *interPredIdc)
            {
                if(0 < *numRefIdxL1ActiveMinus1)
                {
                    parse(streamAccessLayer, decoder, *refIdxL1);
                }

                const auto mvdL1ZeroFlag = sh->get<SSH::MvdL1ZeroFlag>();

                if(*mvdL1ZeroFlag && HEVC::InterPredIdc::PRED_BI == *interPredIdc)
                {
                    //(*mvdLX)[RefList::L1] = MotionVector{};
                }
                else
                {
                    auto mvdc = embedSubtreeInList<MvdCoding>(*this, coord, RefList::L1);

                    parse(streamAccessLayer, decoder, *mvdc);
                    (*mvdLX)[RefList::L1] = *mvdc->get<MvdCoding::LMvd>();
                }

                parse(streamAccessLayer, decoder, *mvpL1Flag);
            }
        }
    }
}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */
