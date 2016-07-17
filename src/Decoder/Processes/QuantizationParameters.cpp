#include <Decoder/Processes/QuantizationParameters.h>
#include <Decoder/State.h>
#include <Syntax/PictureParameterSet.h>
#include <Syntax/SliceSegmentHeader.h>
#include <Syntax/CodingQuadTree.h>
#include <Syntax/CodingUnit.h>
#include <Structure/Picture.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
namespace {
/*----------------------------------------------------------------------------*/
std::tuple<int, int> deriveQpC(
        Ptr<const Structure::Picture> picture,
        PelCoord curr,
        int qpY,
        int cuQpOffsetCb, int cuQpOffsetCr)
{
    using namespace Syntax;

    typedef PictureParameterSet PPS;
    typedef SliceSegmentHeader SSH;

    const auto pps = picture->pps;
    const auto chromaFormatIdc = picture->chromaFormatIdc;
    const auto sh = picture->slice(curr)->header();
    const auto qpBdOffsetC = picture->qpBdOffset(Component::Chroma);
    const auto ppsCbQpOffset = pps->get<PPS::PpsCbQpOffset>()->inUnits();
    const auto ppsCrQpOffset = pps->get<PPS::PpsCrQpOffset>()->inUnits();
    const auto sliceCbQpOffset = sh->get<SSH::SliceCbQpOffset>()->inUnits();
    const auto sliceCrQpOffset = sh->get<SSH::SliceCrQpOffset>()->inUnits();
    const auto qPiCb = clip3(-qpBdOffsetC, 57, qpY + ppsCbQpOffset + sliceCbQpOffset + cuQpOffsetCb);
    const auto qPiCr = clip3(-qpBdOffsetC, 57, qpY + ppsCrQpOffset + sliceCrQpOffset + cuQpOffsetCr);
    const auto qpPrimeCb = qPiToQpC(qPiCb, chromaFormatIdc) + qpBdOffsetC;
    const auto qpPrimeCr = qPiToQpC(qPiCr, chromaFormatIdc) + qpBdOffsetC;

    return std::make_tuple(qpPrimeCb, qpPrimeCr);
}
/*----------------------------------------------------------------------------*/
int deriveQpYprev(
        Ptr<const Structure::Picture> picture,
        PelCoord curr, PelCoord currMinQpGrp)
{
    // 1
    using namespace Syntax;

    typedef CodingUnit CU;

    typedef PictureParameterSet PPS;
    typedef SliceSegmentHeader SSH;
    typedef CodingTreeUnit CTU;

    const auto isFirstInSlice =
        [picture, currMinQpGrp]()
        {
            const auto slice = picture->slice(currMinQpGrp);
            const auto addrInRs = slice->addr().inRs;
            return currMinQpGrp == picture->toPel(picture->toCoord(addrInRs));
        };

    const auto isFirstInTile =
        [picture, currMinQpGrp]()
        {
            const auto &tile = picture->tile(currMinQpGrp);
            const auto addrInRs = picture->toAddrInRs(tile.bdryInTs().begin());
            return currMinQpGrp == picture->toPel(picture->toCoord(addrInRs));
        };

    const auto isFirstInWpp =
        [picture, currMinQpGrp]()
        {
            const auto pps = picture->pps;
            const auto entropyCodingSyncEnabledFlag =
                pps->get<PPS::EntropyCodingSyncEnabledFlag>();

            return
                *entropyCodingSyncEnabledFlag
                && 0_pel == currMinQpGrp.x()
                && picture->toPel(picture->toCtb(currMinQpGrp.y())) == currMinQpGrp.y();
        };

    if(isFirstInSlice() || isFirstInTile() || isFirstInWpp())
    {
        const auto sh = picture->slice(curr)->header();
        return *sh->get<SSH::SliceQpY>();
    }
    else
    {
        const auto ctbSize = picture->ctbSizeY;
        const auto minCuQpDeltaSize = picture->getMinCuQpDeltaSize();
        const auto ctbSizeInMinQpGrp = MinQpGrp{toInt(ctbSize - minCuQpDeltaSize)};
        const auto numInCtb = toPower(ctbSizeInMinQpGrp, 2);

        /* curr pel offset relative to current CTB */
        const PelCoord currOffset{curr.x() % toPel(ctbSize), curr.y() % toPel(ctbSize)};

        /* current quantization group offset relative to current CTB */
        const auto currOffsetInMinQpGrp = picture->toMinQpGrp(currOffset);

        const auto currOffsetAddrInMinQpGrp =
            toAddrScanZ(currOffsetInMinQpGrp, ctbSizeInMinQpGrp, 1_min_qpgrp);

        const auto isInPictureBoundaries =
            [picture](MinQpGrpCoord coord)
            {
                return picture->encloses(picture->toPel(coord));
            };

        const auto limitToPictureBoundaries =
            [picture](PelCoord coord)
            {
                const auto width = picture->widthInLumaSamples;
                const auto height = picture->heightInLumaSamples;

                return
                    PelCoord
                    {
                        std::min(width - 1_pel, coord.x()),
                        std::min(height - 1_pel, coord.y())
                    };
            };

        const auto calcPrevInMinQpGrp =
            [=](MinQpGrpCoord base)
            {
                MinQpGrp prevOffsetAddrInMinQpGrp = currOffsetAddrInMinQpGrp;
                MinQpGrpCoord prevOffsetInMinQpGrp = currOffsetInMinQpGrp;

                do
                {
                    prevOffsetAddrInMinQpGrp =
                        (prevOffsetAddrInMinQpGrp + numInCtb - 1_min_qpgrp) % numInCtb;
                    prevOffsetInMinQpGrp =
                        toCoordScanZ(prevOffsetAddrInMinQpGrp, ctbSizeInMinQpGrp, 1_min_qpgrp);
                } while(!isInPictureBoundaries(base + prevOffsetInMinQpGrp));

                return prevOffsetInMinQpGrp;
            };

        if(0_min_qpgrp == currOffsetAddrInMinQpGrp)
        {
            /* prev and curr (qunatization groups) are in different CTB
             * qpYprev is equal to qpY of last coding unit in prev CTB
             * (in decoding order) as it: "last coding unit in the previous
             * quantization group in decoding order" */
            const auto currCtu = picture->getCodingTreeUnit(curr);
            const auto currCtbAddrInTs = currCtu->get<CTU::CtbAddrInTs>()->inUnits();

            runtime_assert(0_ctb < currCtbAddrInTs);
            /* determine prev CTB in decoding order */
            const auto prevCtbAddrInTs = currCtbAddrInTs - 1_ctb;
            const auto prevCtbAddrInRs = picture->toAddrInRs(prevCtbAddrInTs);
            const auto prevCtbCoord = picture->toCoord(prevCtbAddrInRs);
            const auto prev = picture->toPel(prevCtbCoord);
            /* because last coding unit of previous quantization group
             * in previous CTB is always located at bottom right corner of CTB */
            const PelCoord bottomRightOffset
            {
                toPel(ctbSize) - 1_pel,
                toPel(ctbSize) - 1_pel
            };

            const auto cu =
                picture->getCodingUnit(limitToPictureBoundaries(prev + bottomRightOffset));

            return cu->get<CU::QpY>()->qpY;
        }
        else
        {
            /* prev and curr (quantization groups) are in the same CTB */
            const PelCoord currBase{curr - currOffset};
            const auto currBaseInMinQpGrp = picture->toMinQpGrp(currBase);
            const auto prevOffsetInMinQpGrp = calcPrevInMinQpGrp(currBaseInMinQpGrp);
            const auto prevInMinQpGrp = currBaseInMinQpGrp + prevOffsetInMinQpGrp;
            const auto prev = picture->toPel(prevInMinQpGrp);
            /* because last coding unit in previous quantization group is
             * always located at bottom right corner of previous quantization group */
            const PelCoord bottomRightOffset
            {
                toPel(minCuQpDeltaSize) - 1_pel,
                toPel(minCuQpDeltaSize) - 1_pel
            };

            const auto cu =
                picture->getCodingUnit(limitToPictureBoundaries(prev + bottomRightOffset));

            return cu->get<CU::QpY>()->qpY;
        }
    }
}
/*----------------------------------------------------------------------------*/
int deriveQpYn(
        Ptr<const Structure::Picture> picture,
        PelCoord curr, PelCoord currMinQpGrp, PelCoord adj)
{
    using namespace Syntax;

    typedef CodingUnit CU;

    const auto adjIsNotAvailable =
        [picture, curr, adj]()
        {
            return !picture->isAvailableInScanZ(curr, adj);
        };

    const auto adjInDifferentCtb =
        [picture, curr, adj]()
        {
            using namespace Syntax;

            const auto currCtbAddrInTs =
                picture->getCodingTreeUnit(curr)->get<CodingTreeUnit::CtbAddrInTs>();
            const auto adjCtbAddrInTs =
                picture->getCodingTreeUnit(adj)->get<CodingTreeUnit::CtbAddrInTs>();

            return currCtbAddrInTs->inUnits() != adjCtbAddrInTs->inUnits();
        };

    if(adjIsNotAvailable() || adjInDifferentCtb())
    {
        return deriveQpYprev(picture, curr, currMinQpGrp);
    }
    else
    {
        auto adjCu = picture->getCodingUnit(adj);

        return adjCu->get<CU::QpY>()->qpY;
    }
}
/*----------------------------------------------------------------------------*/
/* curr is expected to be aligned to MinQpGrp boundary */
int deriveQpYpred(
        Ptr<const Structure::Picture> picture,
        PelCoord curr, PelCoord currMinQpGrp)
{
    /* names mapping:
     * curr == (xCb, yCb),
     * currMinQpGrp == (xQg, yQg) */

    // 2 (potentially calls 1)
    const auto qpYa =
        deriveQpYn(
                picture,
                curr, currMinQpGrp,
                PelCoord{currMinQpGrp.x() - 1_pel, currMinQpGrp.y()});
    // 3 (potentially calls 1)
    const auto qpYb =
        deriveQpYn(
                picture,
                curr, currMinQpGrp,
                PelCoord{currMinQpGrp.x(), currMinQpGrp.y() - 1_pel});

    const auto qpYpred = (qpYa + qpYb + 1) >> 1;

    return qpYpred;
}
/*----------------------------------------------------------------------------*/
std::tuple<int, int> deriveQpY(
        Ptr<const Structure::Picture> picture,
        int qpYpred,
        int cuQpDeltaVal)
{
    const auto qpBdOffsetY = picture->qpBdOffset(Component::Luma);

    const int qpY =
        ((qpYpred + cuQpDeltaVal + 52 + 2 * qpBdOffsetY)
         % (52 + qpBdOffsetY))
        - qpBdOffsetY;

    const auto qpPrimeY = qpY + qpBdOffsetY;

    return std::make_tuple(qpY, qpPrimeY);
}
/*----------------------------------------------------------------------------*/
} /* namespace */
/*----------------------------------------------------------------------------*/
int QuantizationParameters::exec(
        State &, Ptr<const Structure::Picture> picture, PelCoord curr)
{
    const auto currInMinQpGrp = picture->toMinQpGrp(curr);
    const auto currMinQpGrp = picture->toPel(currInMinQpGrp);
    const auto qpYpred = deriveQpYpred(picture, curr, currMinQpGrp);

    log(LogId::QuantizationYpred, curr, ' ', qpYpred, '\n');

    return qpYpred;
}

std::tuple<int, int> QuantizationParametersLuma::exec(
        State &decoder, Ptr<const Structure::Picture> picture)
{
    return
        deriveQpY(
                picture,
                decoder.qpStatus.qpYpred,
                decoder.qpStatus.cuQpDeltaVal);
}

std::tuple<int, int> QuantizationParametersChroma::exec(
        State &,
        Ptr<const Structure::Picture> picture,
        PelCoord curr,
        int qpY, int cuQpOffsetCb, int cuQpOffsetCr)
{
    const ChromaFormatIdc chromaFormatIdc = picture->chromaFormatIdc;

    runtime_assert(ChromaFormatIdc::f400 != chromaFormatIdc);

    return deriveQpC(picture, curr, qpY, cuQpOffsetCb, cuQpOffsetCr);
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */
