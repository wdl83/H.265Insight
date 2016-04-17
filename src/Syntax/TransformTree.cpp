#include <Syntax/TransformTree.h>
#include <Syntax/SequenceParameterSet.h>
#include <Syntax/CodingTreeUnit.h>
#include <Syntax/CodingUnit.h>
#include <Syntax/TransformUnit.h>
#include <Structure/Picture.h>
#include <Decoder/State.h>

namespace HEVC { namespace Syntax { namespace TransformTreeContent {
/*----------------------------------------------------------------------------*/
void CbfCb::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
        CodingTreeUnit &ctu, PelCoord ctuOffset, Log2 ttSize, int depth)
{
    const auto contextModel =
        [depth](CABAD::State &state, int binIdx) -> CABAD::Variable &
        {
            syntaxCheck(0 == binIdx);
            return state.getVariable(CABAD::CtxId::cbf_chroma, depth);
        };

    getFrom(streamAccessLayer, decoder, cabadState(decoder), *this, contextModel);

    const auto side = toPel(ttSize);

    for(auto y = 0_pel; y < side; ++y)
    {
        for(auto x = 0_pel; x < side; ++x)
        {
            ctu.cbf(Plane::Cb, ctuOffset + PelCoord{x, y}) = getValue();
        }
    }
}
/*----------------------------------------------------------------------------*/
void CbfCr::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
        CodingTreeUnit &ctu, PelCoord ctuOffset, Log2 ttSize, int depth)
{
    const auto contextModel =
        [depth](CABAD::State &state, int binIdx) -> CABAD::Variable &
        {
            syntaxCheck(0 == binIdx);
            return state.getVariable(CABAD::CtxId::cbf_chroma, depth);
        };

    getFrom(streamAccessLayer, decoder, cabadState(decoder), *this, contextModel);

    const auto side = toPel(ttSize);

    for(auto y = 0_pel; y < side; ++y)
    {
        for(auto x = 0_pel; x < side; ++x)
        {
            ctu.cbf(Plane::Cr, ctuOffset + PelCoord{x, y}) = getValue();
        }
    }
}
/*----------------------------------------------------------------------------*/
void CbfLuma::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
        CodingTreeUnit &ctu, PelCoord ctuOffset, Log2 ttSize, int depth)
{
    const auto contextModel =
        [depth](CABAD::State &state, int binIdx) -> CABAD::Variable &
        {
            syntaxCheck(0 == binIdx);
            return state.getVariable(CABAD::CtxId::cbf_luma, 0 == depth ? 1 : 0);
        };

    getFrom(streamAccessLayer, decoder, cabadState(decoder), *this, contextModel);

    const auto side = toPel(ttSize);

    for(auto y = 0_pel; y < side; ++y)
    {
        for(auto x = 0_pel; x < side; ++x)
        {
            ctu.cbf(Plane::Y, ctuOffset + PelCoord{x, y}) = getValue();
        }
    }
}
/*----------------------------------------------------------------------------*/
} /* TransformTreeContent */

/*----------------------------------------------------------------------------*/
void TransformTree::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
        const SliceSegmentHeader &sh,
        CodingTreeUnit &ctu,
        CodingUnit &cu,
        PelCoord rootCoord,
        bool defaultCbfCb, bool defaultCbfCr)
{
    const auto ttCoord = get<Coord>()->inUnits();
    const auto cuOffset = get<CuOffset>()->inUnits();
    const auto ttSize = get<Size>()->inUnits();
    const auto depth = get<Depth>()->inUnits();
    const auto blkIdx = get<BlkIdx>()->inUnits();
    const auto x0 = ttCoord.x();
    const auto y0 = ttCoord.y();

    const auto picture = decoder.picture();
    const auto chromaFormatIdc = picture->chromaFormatIdc;
    const auto is400 = ChromaFormatIdc::f400 == chromaFormatIdc;
    const auto is422 = ChromaFormatIdc::f422 == chromaFormatIdc;
    const auto is444 = ChromaFormatIdc::f444 == chromaFormatIdc;
    const auto isSCP = ChromaFormatIdc::fSCP == chromaFormatIdc;
    const auto sps = picture->sps;

    typedef SequenceParameterSet SPS;
    typedef CodingUnit CU;

    const auto ctuOffset = ttCoord - picture->toPel(picture->toCtb(ttCoord));
    const auto minSize = picture->minTrafoSize;
    const auto maxSize = picture->maxTrafoSize;
    const auto intraSplitFlag = bool(*cu.get<CodingUnit::IntraSplitFlag>());
    const auto cuCoord = cu.get<CU::Coord>()->inUnits();
    const auto maxDepth = cu.get<CU::MaxTrafoDepth>()->inUnits();
    const PredictionMode cuPredMode = *cu.get<CU::CuPredMode>();

    if(0 == depth)
    {
        /* 10/2014, 7.4.9.8 "Transform tree semantics" */
        const auto side = toPel(ttSize);

        for(auto y = 0_pel; y < side; ++y)
        {
            for(auto x = 0_pel; x < side; ++x)
            {
                ctu.cbf(Plane::Y, ctuOffset + PelCoord{x, y}) = true;
            }
        }
    }

    /* start: inferrable */
    auto interSplitFlag =
        embed<InterSplitFlag>(
                *this,
                *sps->get<SPS::MaxTransformHierarchyDepthInter>(),
                cuPredMode,
                *cu.get<CU::PartModePseudo>(),
                depth);

    auto splitTransformFlag =
        embed<SplitTransformFlag>(
                *this,
                ttSize, maxSize, depth, intraSplitFlag, *interSplitFlag);
   /* end: inferrable */

    if(
            maxSize >= ttSize && minSize < ttSize
            && maxDepth > depth
            && !(intraSplitFlag && 0 == depth))
    {
        parse(streamAccessLayer, decoder, *splitTransformFlag, ttSize);
    }

    auto cbf422 = false;

    if(2_log2 < ttSize && !isSCP && !is400 || is444)
    {
        if(0 == depth || defaultCbfCb)
        {
            auto cbfCb = embed<CbfCb>(*this);

            parse(streamAccessLayer, decoder, *cbfCb, ctu, ctuOffset, ttSize, depth);
            defaultCbfCb = bool(*cbfCb);

            if(is422 && (!*splitTransformFlag || 3_log2 == ttSize))
            {
                const PelCoord offset{ctuOffset.x(), ctuOffset.y() + toPel(ttSize - 1_log2)};

                parse(streamAccessLayer, decoder, *cbfCb, ctu, offset, ttSize - 1_log2, depth);
                cbf422 = cbf422 || bool(*cbfCb);
            }
        }

        if(0 == depth || defaultCbfCr)
        {
            auto cbfCr = embed<CbfCr>(*this);

            parse(streamAccessLayer, decoder, *cbfCr, ctu, ctuOffset, ttSize, depth);
            defaultCbfCr = bool(*cbfCr);

            if(is422 && (!*splitTransformFlag || 3_log2 == ttSize))
            {
                const PelCoord offset{ctuOffset.x(), ctuOffset.y() + toPel(ttSize - 1_log2)};

                parse(streamAccessLayer, decoder, *cbfCr, ctu, offset, ttSize - 1_log2, depth);
                cbf422 = cbf422 || bool(*cbfCr);
            }
        }
    }

    if(*splitTransformFlag)
    {
        const auto subSize = ttSize - 1_log2;
        const auto subDepth = depth + 1;
        const auto subSide = toPel(subSize);
        const auto x1 = x0 + subSide;
        const auto y1 = y0 + subSide;

        {
            auto tt =
                cu.embedTransformTree(
                        PelCoord{x0, y0},
                        PelCoord{x0, y0} - cuCoord,
                        subSize, subDepth,
                        0);

            subparse(
                    streamAccessLayer, decoder, *tt,
                    sh, ctu, cu,
                    ttCoord,
                    defaultCbfCb, defaultCbfCr);
        }

        {
            auto tt =
                cu.embedTransformTree(
                        PelCoord{x1, y0},
                        PelCoord{x1, y0} - cuCoord,
                        subSize, subDepth,
                        1);

            subparse(
                    streamAccessLayer, decoder, *tt,
                    sh, ctu, cu,
                    ttCoord,
                    defaultCbfCb, defaultCbfCr);
        }

        {
            auto tt =
                cu.embedTransformTree(
                        PelCoord{x0, y1},
                        PelCoord{x0, y1} - cuCoord,
                        subSize, subDepth,
                        2);

            subparse(
                    streamAccessLayer, decoder, *tt,
                    sh, ctu, cu,
                    ttCoord,
                    defaultCbfCb, defaultCbfCr);
        }

        {
            auto tt =
                cu.embedTransformTree(
                        PelCoord{x1, y1},
                        PelCoord{x1, y1} - cuCoord,
                        subSize, subDepth,
                        3);

            subparse(
                    streamAccessLayer, decoder, *tt,
                    sh, ctu, cu,
                    ttCoord,
                    defaultCbfCb, defaultCbfCr);
        }
    }
    else
    {
        if(
                isIntra(cuPredMode)
                || 0 != depth
                || defaultCbfCb
                || defaultCbfCr
                || cbf422)
        {
            parse(
                    streamAccessLayer, decoder, *embed<CbfLuma>(*this),
                    ctu, ctuOffset, ttSize, depth);
        }

        auto tu =
            embedSubtree<TransformUnit>(
                    *this, ttCoord, cuOffset, rootCoord, ttSize, depth, blkIdx);

        parse(streamAccessLayer, decoder, *tu, sh, ctu, cu);
    }
}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */
