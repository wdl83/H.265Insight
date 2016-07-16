#include <Syntax/TransformUnit.h>
#include <Syntax/SequenceParameterSet.h>
#include <Syntax/PictureParameterSet.h>
#include <Syntax/PpsRangeExtension.h>
#include <Syntax/SliceSegmentHeader.h>
#include <Syntax/CodingTreeUnit.h>
#include <Syntax/CodingUnit.h>
#include <Syntax/TransformTree.h>
#include <Syntax/ResidualCoding.h>
#include <Structure/Picture.h>
#include <Decoder/State.h>

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
void TransformUnit::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
        const SliceSegmentHeader &sh,
        const CodingTreeUnit &ctu,
        const CodingUnit &cu)
{
    typedef CodingUnit CU;

    const auto tuCoord = get<Coord>()->inUnits();
    const auto cuOffset = get<CuOffset>()->inUnits();
    const auto rootCoord = get<RootCoord>()->inUnits();
    const auto tuSize = get<Size>()->inUnits();
    const auto blkIdx = get<BlkIdx>()->inUnits();

    syntaxCheck(cuOffset == tuCoord - cu.get<CU::Coord>()->inUnits());

    const auto picture = decoder.picture();
    const auto chromaFormatIdc = picture->chromaFormatIdc;
    const auto is422 = ChromaFormatIdc::f422 == chromaFormatIdc;
    const auto is444 = ChromaFormatIdc::f444 == chromaFormatIdc;
    const auto pps = picture->pps;
    const auto ppsre = picture->ppsre;

    typedef PictureParameterSet PPS;
    typedef PpsRangeExtension PPSRE;
    typedef SliceSegmentHeader SSH;
    typedef CodingUnit CU;

    const auto ctuCoord = picture->toPel(picture->toCtb(tuCoord));
    const auto tuSizeC = std::max(2_log2, tuSize - (is444 ? 0_log2 : 1_log2));
    //const auto tuDepthC = tuDepth - (!is444 && 2_log2 == tuSize ? 1 : 0);
    const auto tuCoordC = !is444 && 2_log2 == tuSize ? rootCoord : tuCoord;
    const auto tuCoord422C = tuCoordC + PelCoord{0_pel, toPel(tuSizeC)};
    const auto cuSize = cu.get<CU::Size>()->inUnits();
    const auto cbfY = ctu.cbf(Plane::Y, tuCoord - ctuCoord);
    const auto cbfCb = ctu.cbf(Plane::Cb, tuCoordC - ctuCoord);
    const auto cbfCr = ctu.cbf(Plane::Cr, tuCoordC - ctuCoord);
    const auto cbf422Cb = is422 && ctu.cbf(Plane::Cb, tuCoord422C - ctuCoord);
    const auto cbf422Cr = is422 && ctu.cbf(Plane::Cr, tuCoord422C - ctuCoord);
    const auto cbfC = cbfCb || cbfCr || cbf422Cb || cbf422Cr;

    if(cbfY || cbfC)
    {
        const auto cuQpDeltaEnabledFlag = pps->get<PPS::CuQpDeltaEnabledFlag>();

        if(*cuQpDeltaEnabledFlag && !decoder.qpStatus.isCuQpDeltaCoded)
        {
            auto cuQpDeltaAbs = embed<CuQpDeltaAbs>(*this);
            auto cuQpDeltaSignFlag = embed<CuQpDeltaSignFlag>(*this);

            parse(streamAccessLayer, decoder, *cuQpDeltaAbs);

            if(0 != cuQpDeltaAbs->inUnits())
            {
                parse(streamAccessLayer, decoder, *cuQpDeltaSignFlag);
            }

            decoder.qpStatus.isCuQpDeltaCoded = true;
            decoder.qpStatus.cuQpDeltaVal =
                (*cuQpDeltaSignFlag ? -1 : 1) * cuQpDeltaAbs->inUnits();
        }

        if(
                *sh.get<SSH::CuChromaQpOffsetEnabledFlag>()
                && cbfC
                && !(*cu.get<CU::CuTransquantBypassFlag>())
                && !decoder.qpStatus.isCuQpOffsetCoded)
        {
            auto cuChromaQpOffsetFlag = embed<CuChromaQpOffsetFlag>(*this);
            auto cuChromaQpOffsetIdx =
                embed<CuChromaQpOffsetIdx>(*this, *ppsre->get<PPSRE::ChromaQpOffsetListLenMinus1>());

            parse(streamAccessLayer, decoder, *cuChromaQpOffsetFlag);

            if(*cuChromaQpOffsetFlag)
            {
                if(0 < ppsre->get<PPSRE::ChromaQpOffsetListLenMinus1>()->inUnits())
                {
                    parse(streamAccessLayer, decoder, *cuChromaQpOffsetIdx);
                }

                decoder.qpStatus.cuQpOffsetCb =
                    (*ppsre->get<PPSRE::CbQpOffsetList>())[cuChromaQpOffsetIdx->inUnits()];
                decoder.qpStatus.cuQpOffsetCr =
                    (*ppsre->get<PPSRE::CrQpOffsetList>())[cuChromaQpOffsetIdx->inUnits()];
            }
            else
            {
                decoder.qpStatus.cuQpOffsetCb = 0;
                decoder.qpStatus.cuQpOffsetCr = 0;
            }

            decoder.qpStatus.isCuQpOffsetCoded = true;
        }

        if(cbfY)
        {
            parse(
                    streamAccessLayer, decoder,
                    *embedSubtreeInArray<ResidualCodingArray>(
                        *this, int(RCI::Y), tuCoord, tuSize, Plane::Y),
                    cu);
        }

        if(2_log2 < tuSize || is444)
        {
            const auto crossComponentPredictionEnabled =
                ppsre
                && *ppsre->get<PPSRE::CrossComponentPredictionEnabledFlag>()
                && (
                        PredictionMode::Inter == *cu.get<CU::CuPredMode>()
                        || 4 == (*cu.get<CU::IntraChromaPredMode>())[makeTuple(cuOffset, cuSize)]);

            if(cbfY && crossComponentPredictionEnabled)
            {
                parse(
                        streamAccessLayer, decoder,
                        *embedSubtreeInArray<CrossCompPredArray>(
                            *this, int(Chroma::Cb)), Chroma::Cb);
            }

            if(cbfCb)
            {
                parse(
                        streamAccessLayer, decoder,
                        *embedSubtreeInArray<ResidualCodingArray>(
                            *this, int(RCI::Cb), tuCoordC, tuSizeC, Plane::Cb),
                        cu);
            }

            if(cbf422Cb)
            {
                parse(
                        streamAccessLayer, decoder,
                        *embedSubtreeInArray<ResidualCodingArray>(
                            *this, int(RCI::Cb1), tuCoord422C, tuSizeC, Plane::Cb),
                        cu);
            }

            if(cbfY && crossComponentPredictionEnabled)
            {
                parse(
                        streamAccessLayer, decoder,
                        *embedSubtreeInArray<CrossCompPredArray>(
                            *this, int(Chroma::Cr)), Chroma::Cr);
            }

            if(cbfCr)
            {
                parse(
                        streamAccessLayer, decoder,
                        *embedSubtreeInArray<ResidualCodingArray>(
                            *this, int(RCI::Cr), tuCoordC, tuSizeC, Plane::Cr),
                        cu);
            }

            if(cbf422Cr)
            {
                parse(
                        streamAccessLayer, decoder,
                        *embedSubtreeInArray<ResidualCodingArray>(
                            *this, int(RCI::Cr1), tuCoord422C, tuSizeC, Plane::Cr),
                        cu);
            }
        }
        else if(3 == blkIdx)
        {

            if(cbfCb)
            {
                parse(
                        streamAccessLayer, decoder,
                        *embedSubtreeInArray<ResidualCodingArray>(
                            *this, int(RCI::Cb), rootCoord, tuSizeC, Plane::Cb),
                        cu);
            }

            if(cbf422Cb)
            {
                parse(
                        streamAccessLayer, decoder,
                        *embedSubtreeInArray<ResidualCodingArray>(
                            *this, int(RCI::Cb1), tuCoord422C, tuSizeC, Plane::Cb),
                        cu);
            }

            if(cbfCr)
            {
                parse(
                        streamAccessLayer, decoder,
                        *embedSubtreeInArray<ResidualCodingArray>(
                            *this, int(RCI::Cr), rootCoord, tuSizeC, Plane::Cr),
                        cu);
            }

            if(cbf422Cr)
            {
                parse(
                        streamAccessLayer, decoder,
                        *embedSubtreeInArray<ResidualCodingArray>(
                            *this, int(RCI::Cr1), tuCoord422C, tuSizeC, Plane::Cr),
                        cu);
            }
        }
    }
}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */
