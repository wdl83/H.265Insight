#include <Syntax/SequenceParameterSet.h>
#include <Syntax/PictureParameterSet.h>
#include <Syntax/CodingTreeUnit.h>
#include <Syntax/CodingQuadTree.h>
#include <Syntax/CodingUnit.h>
#include <Syntax/CABAD/CtxId.h>
#include <Structure/Picture.h>
#include <Decoder/State.h>
#include <log.h>

namespace HEVC { namespace Syntax { namespace CodingQuadTreeContent {
/*----------------------------------------------------------------------------*/
void SplitCuFlag::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
        PelCoord cqtCoord, int cqtDepth)
{
    const auto contextModel =
        [&decoder, cqtCoord, cqtDepth](CABAD::State &state, int binIdx) -> CABAD::Variable &
        {
            typedef CodingQuadTree CQT;

           syntaxCheck(0 == binIdx || 1 == binIdx || 2 == binIdx);

            /* (x0, y0) - top-left luma sample of the current luma block relative
             * to top-left sample of the current picture */
            const auto curr = cqtCoord;
            const PelCoord left{curr.x() - 1_pel, curr.y()};
            const PelCoord above{curr.x(), curr.y() - 1_pel};

            auto picture = decoder.picture();

            /*  call "availability derivation process for a block in z-scan order" (6.4.1) */
            const auto availableL = picture->isAvailableInScanZ(curr, left);
            const auto availableA = picture->isAvailableInScanZ(curr, above);
            /* 10v34, 9.3.4.2.2 Table 9-37 */
            int i = 0;

            if(availableL || availableA)
            {
                const auto cqtDepthC = cqtDepth;

                if(availableL)
                {
                    const auto cqtDepthL =
                        picture->getCodingQuadTree(left)->get<CQT::Depth>()->inUnits();
                    const auto condL = cqtDepthL > cqtDepthC;
                    i += condL;
                }

                if(availableA)
                {
                    const auto cqtDepthA =
                        picture->getCodingQuadTree(above)->get<CQT::Depth>()->inUnits();
                    const auto condA = cqtDepthA > cqtDepthC;
                    i += condA;
                }
            }

            return state.getVariable(CABAD::CtxId::split_cu_flag, i);
        };

    getFrom(streamAccessLayer, decoder, cabadState(decoder), *this, contextModel);
}
/*----------------------------------------------------------------------------*/
} /* CodingQuadTreeContent */

/*----------------------------------------------------------------------------*/
/* CodingQuadTree */
/*----------------------------------------------------------------------------*/
void CodingQuadTree::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
        const SliceSegmentHeader &sh,
        CodingTreeUnit &ctu)
{
    typedef PictureParameterSet PPS;
    typedef SliceSegmentHeader SSH;

    const auto cqtCoord = get<Coord>()->inUnits();
    const auto offset = get<Offset>()->inUnits();
    const auto cqtDepth = get<Depth>()->inUnits();
    const auto cqtSize = get<Size>()->inUnits();

    const auto x0 = cqtCoord.x();
    const auto y0 = cqtCoord.y();

    auto picture = decoder.picture();
    auto pps = picture->pps;

    const auto picWidthInLumaSamples = picture->widthInLumaSamples;
    const auto picHeightInLumaSamples = picture->heightInLumaSamples;
    const auto minCbSizeY = picture->minCbSizeY;

    /* start: inferrable */
    auto splitCuFlag = embed<SplitCuFlag>(*this, minCbSizeY, cqtSize);
    /* end: inferrable */

    if(
            x0 + toPel(cqtSize) <= picWidthInLumaSamples
            && y0 + toPel(cqtSize) <= picHeightInLumaSamples
            && cqtSize > minCbSizeY)
    {
        parse(streamAccessLayer, decoder, *splitCuFlag, cqtCoord, cqtDepth);
    }

    const bool cuQpDeltaEnabledFlag(*pps->get<PPS::CuQpDeltaEnabledFlag>());
    const bool cuChromaQpOffsetEnabledFlag(*sh.get<SSH::CuChromaQpOffsetEnabledFlag>());

    if(
            !cuQpDeltaEnabledFlag
            || cuQpDeltaEnabledFlag && cqtSize >= picture->getMinCuQpDeltaSize())
    {
        decoder.qpStatus.isCuQpDeltaCoded = false;
        decoder.qpStatus.cuQpDeltaVal = 0;
    }

    if(
            !cuChromaQpOffsetEnabledFlag
            || cuChromaQpOffsetEnabledFlag && cqtSize >= picture->minCuChromaQpOffsetSize())
    {
        decoder.qpStatus.isCuQpOffsetCoded = false;
        decoder.qpStatus.cuQpOffsetCb = 0;
        decoder.qpStatus.cuQpOffsetCr = 0;
    }

    if(*splitCuFlag)
    {
        const auto subSize = cqtSize - 1_log2;
        const auto subSizeInPels = toPel(subSize);
        const auto subDepth = cqtDepth + 1;
        const auto x1 = x0 + subSizeInPels;
        const auto y1 = y0 + subSizeInPels;

        {
            const PelCoord subOffset{offset.x() + 0_pel, offset.y() + 0_pel};
            auto cqt =
                ctu.embedCodingQuadTree(PelCoord{x0, y0}, subOffset, subSize, subDepth);

            subparse(streamAccessLayer, decoder, *cqt, sh, ctu);
        }

        if(x1 < picWidthInLumaSamples)
        {
            const PelCoord subOffset{offset.x() + subSizeInPels, offset.y() + 0_pel};
            auto cqt =
                ctu.embedCodingQuadTree(PelCoord{x1, y0}, subOffset, subSize, subDepth);

            subparse(streamAccessLayer, decoder, *cqt, sh, ctu);
        }

        if(y1 < picHeightInLumaSamples)
        {
            const PelCoord subOffset{offset.x() + 0_pel, offset.y() + subSizeInPels};
            auto cqt =
                ctu.embedCodingQuadTree(PelCoord{x0, y1}, subOffset, subSize, subDepth);

            subparse(streamAccessLayer, decoder, *cqt, sh, ctu);
        }

        if(x1 < picWidthInLumaSamples && y1 < picHeightInLumaSamples)
        {
            const PelCoord subOffset{offset.x() + subSizeInPels, offset.y() + subSizeInPels};
            auto cqt =
                ctu.embedCodingQuadTree(PelCoord{x1, y1}, subOffset, subSize, subDepth);

            subparse(streamAccessLayer, decoder, *cqt, sh, ctu);
        }
    }
    else
    {
        auto cu =
            embedSubtree<CodingUnit>(
                    *this, picture->minTrafoSize, PelCoord{x0, y0}, cqtSize);

        parse(streamAccessLayer, decoder, *cu, sh, ctu);
    }
}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */
