#include <Syntax/SAO.h>
#include <Syntax/SequenceParameterSet.h>
#include <Syntax/PictureParameterSet.h>
#include <Syntax/PpsRangeExtension.h>
#include <Syntax/SliceSegmentHeader.h>
#include <Syntax/SliceSegmentData.h>
#include <Syntax/CodingTreeUnit.h>
#include <Structure/Picture.h>
#include <Decoder/State.h>

namespace HEVC { namespace Syntax { namespace SAOContent {
/*----------------------------------------------------------------------------*/
void SaoOffsetAbs::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
        Plane cIdx, int i)
{
    const auto component = toComponent(cIdx);
    int bitDepth = decoder.picture()->bitDepth(component);

    m_cTRMax = (1 << (std::min(bitDepth, 10) - 5)) - 1;
    getFrom(streamAccessLayer, decoder, cabadState(decoder), *this);
    m_saoOffsetAbs[int(cIdx)][i] = getValue();
}
/*----------------------------------------------------------------------------*/
void SaoOffsetSign::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
        Plane cIdx, int i)
{
    getFrom(streamAccessLayer, decoder, cabadState(decoder), *this);
    m_saoOffsetSign[int(cIdx)][i] = getValue();
}
/*----------------------------------------------------------------------------*/
void SaoBandPosition::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
        Plane cIdx)
{
    getFrom(streamAccessLayer, decoder, cabadState(decoder), *this);
    m_bandPosition[int(cIdx)] = getValue();
}
/*----------------------------------------------------------------------------*/
void SaoBandPosition::merge(const SAO &adj)
{
    m_bandPosition = adj.get<SaoBandPosition>()->m_bandPosition;
}
/*----------------------------------------------------------------------------*/
/* SaoTypeIdx */
/*----------------------------------------------------------------------------*/
void SaoTypeIdx::merge(const SAO &adj)
{
    m_saoTypeIdx = adj.get<SaoTypeIdx>()->m_saoTypeIdx;
}
/*----------------------------------------------------------------------------*/
/* SaoOffsetVal */
/*----------------------------------------------------------------------------*/
void SaoOffsetVal::merge(const SAO &adj)
{
    m_saoOffset = adj.get<SaoOffsetVal>()->m_saoOffset;
}
/*----------------------------------------------------------------------------*/
/* SaoEoClass */
/*----------------------------------------------------------------------------*/
void SaoEoClass::merge(const SAO &adj)
{
    m_saoEoClass = adj.get<SaoEoClass>()->m_saoEoClass;
}
/*----------------------------------------------------------------------------*/
} /* SAOContent */
/*----------------------------------------------------------------------------*/
/* SAO */
/*----------------------------------------------------------------------------*/
void SAO::onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder)
{
    /* start: derived from arguments */
    auto saoCoord = get<Coord>()->inUnits();
    /* end: derived from arguments */

    const auto rx = saoCoord.x();
    const auto ry = saoCoord.y();

    /* start: inferrable */
    auto saoMergeLeftFlag = embed<SaoMergeLeftFlag>(*this);
    auto saoMergeUpFlag = embed<SaoMergeUpFlag>(*this);
    auto saoTypeIdxLuma = embed<SaoTypeIdxLuma>(*this);
    auto saoTypeIdxChroma = embed<SaoTypeIdxChroma>(*this);
    auto saoOffsetAbs = embed<SaoOffsetAbs>(*this);
    auto saoOffsetSign = embed<SaoOffsetSign>(*this);
    auto saoBandPosition = embed<SaoBandPosition>(*this);
    auto saoEoClassLuma = embed<SaoEoClassLuma>(*this);
    auto saoEoClassChroma = embed<SaoEoClassChroma>(*this);
    auto saoTypeIdx = embed<SaoTypeIdx>(*this);
    auto saoOffsetVal = embed<SaoOffsetVal>(*this);
    auto saoEoClass = embed<SaoEoClass>(*this);
    /* end: inferrable */

    const auto picture = decoder.picture();
    const auto chromaFormatIdc = picture->chromaFormatIdc;
    const auto slice = picture->slice(saoCoord);
    const auto sh = slice->header();
    const auto ctu = picture->getCodingTreeUnit(saoCoord);

    typedef SliceSegmentHeader SSH;
    typedef CodingTreeUnit CTU;

    const auto sliceAddrInRs = slice->addr().inRs;
    const auto ctbAddrInRs = ctu->get<CTU::CtbAddrInRs>()->inUnits();

    if(0_pel < rx)
    {
        const auto leftCtbInSliceSeg = ctbAddrInRs > sliceAddrInRs;

        const auto leftCtbInTile =
            picture->tileId(picture->toCoord(ctbAddrInRs))
            == picture->tileId(picture->toCoord(ctbAddrInRs - 1_ctb));

        if(leftCtbInSliceSeg && leftCtbInTile)
        {
            parse(streamAccessLayer, decoder, *saoMergeLeftFlag);
        }
    }

    if(0_pel < ry && !*saoMergeLeftFlag)
    {
        const auto picWidthInCtbsY = picture->widthInCtbsY;

        const auto upCtbInSliceSeg =
            (ctbAddrInRs - picWidthInCtbsY) >= sliceAddrInRs;
        const auto upCtbInTile =
            picture->tileId(picture->toCoord(ctbAddrInRs))
            == picture->tileId(picture->toCoord(ctbAddrInRs - picWidthInCtbsY));

        if(upCtbInSliceSeg && upCtbInTile)
        {
            parse(streamAccessLayer, decoder, *saoMergeUpFlag);
        }
    }

    if(*saoMergeLeftFlag || *saoMergeUpFlag)
    {
        const auto deriveAdjCoord =
            [saoCoord, saoMergeLeftFlag, saoMergeUpFlag]()
            {
                if(*saoMergeLeftFlag)
                {
                    syntaxCheck(0_pel < saoCoord.x());
                    return PelCoord{saoCoord.x() - 1_pel, saoCoord.y()};
                }
                else if(*saoMergeUpFlag)
                {
                    syntaxCheck(0_pel < saoCoord.y());
                    return PelCoord{saoCoord.x(), saoCoord.y() - 1_pel};
                }
                else
                {
                    syntaxCheck(false);
                }

                return PelCoord{saoCoord};
            };

        const auto adjCoord = deriveAdjCoord();
        const auto adj = picture->getCodingTreeUnit(adjCoord)->getSAO();

        saoTypeIdx->merge(*adj);
        saoOffsetVal->merge(*adj);
        saoBandPosition->merge(*adj);
        saoEoClass->merge(*adj);
    }
    else
    {
        for(const auto cIdx : EnumRange<Plane>())
        {
            const auto component = toComponent(cIdx);
            const auto isLuma = Component::Luma == component;
            const auto isChroma = !isLuma;

            if(
                    isLuma && !(*sh->get<SSH::SliceSaoLumaFlag>())
                    || ChromaFormatIdc::f400 == chromaFormatIdc && isChroma
                    || isChroma && !(*sh->get<SSH::SliceSaoChromaFlag>()))
            {
                continue;
            }

            if(Plane::Y == cIdx)
            {
                parse(streamAccessLayer, decoder, *saoTypeIdxLuma);
                saoTypeIdx->set(*saoTypeIdxLuma);
            }
            else if(Plane::Cb == cIdx)
            {
                parse(streamAccessLayer, decoder, *saoTypeIdxChroma);
                saoTypeIdx->set(*saoTypeIdxChroma);
            }

            if(SaoType::NotApplied != (*saoTypeIdx)[component])
            {
                for(auto i = 0; i < 4; ++i)
                {
                    parse(streamAccessLayer, decoder, *saoOffsetAbs, cIdx, i);
                }

                saoOffsetSign->init(cIdx, (*saoTypeIdx)[component]);

                if(SaoType::BandOffset == (*saoTypeIdx)[component])
                {
                    for(auto i = 0; i < 4; ++i)
                    {
                        if(0 != (*saoOffsetAbs)[makeTuple(cIdx, i)])
                        {
                            parse(streamAccessLayer, decoder, *saoOffsetSign, cIdx, i);
                        }
                    }

                    parse(streamAccessLayer, decoder, *saoBandPosition, cIdx);
                }
                else
                {
                    syntaxCheck(SaoType::EdgeOffset == (*saoTypeIdx)[component]);

                    if(Plane::Y == cIdx)
                    {
                        parse(streamAccessLayer, decoder, *saoEoClassLuma);
                    }

                    if(Plane::Cb == cIdx)
                    {
                        parse(streamAccessLayer, decoder, *saoEoClassChroma);
                    }
                }
            }
        }

        typedef PpsRangeExtension PPSRE;

        const auto ppsre = picture->ppsre;
        const auto saoOffsetScaleLuma =
            ppsre ? ppsre->get<PPSRE::SaoOffsetScaleLuma>()->inUnits() : 0_log2;
        const auto saoOffsetScaleChroma =
            ppsre ? ppsre->get<PPSRE::SaoOffsetScaleChroma>()->inUnits() : 0_log2;

        saoOffsetVal->set(
                *saoOffsetAbs, *saoOffsetSign,
                saoOffsetScaleLuma, saoOffsetScaleChroma);

        saoEoClass->set(*saoEoClassLuma, *saoEoClassChroma);
    }
}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */
