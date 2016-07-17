#include <Syntax/SliceSegmentData.h>
#include <Syntax/SliceSegmentHeader.h>
#include <Syntax/SequenceParameterSet.h>
#include <Syntax/PictureParameterSet.h>
#include <Syntax/CodingTreeUnit.h>
#include <Syntax/ByteAlignment.h>
#include <Structure/Picture.h>
#include <Decoder/State.h>

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/

namespace {
/*----------------------------------------------------------------------------*/
void initialize(
        Structure::Picture &picture,
        CABAD::InitType initType,
        int sliceQpY,
        StreamAccessLayer &streamAccessLayer)
{
    picture.getCabadState().init(streamAccessLayer, initType, sliceQpY);
}
/*----------------------------------------------------------------------------*/
void restore(
        Structure::Picture &picture,
        CABAD::RestorePoint point,
        StreamAccessLayer &streamAccessLayer)
{
    picture.restore(point, streamAccessLayer);
}
/*----------------------------------------------------------------------------*/
} /* namespace */

/*----------------------------------------------------------------------------*/
void SliceSegmentData::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
        const SliceSegmentHeader &ssh)
{
    typedef SequenceParameterSet SPS;
    typedef PictureParameterSet PPS;
    typedef SliceSegmentHeader SSH;

    const auto picture = decoder.picture();
    const auto pps = picture->pps;
    const auto sh = picture->slice(picture->toCoord(*ssh.get<SSH::SliceAddrRs>()))->header();
    const auto cabacInitType = sh->get<SSH::CabacInitType>();
    const auto sliceQpY = sh->get<SSH::SliceQpY>();
    const auto dependentSliceSegmentFlag = ssh.get<SSH::DependentSliceSegmentFlag>();
    const auto independentSliceSegment = !(*dependentSliceSegmentFlag);
    const auto sliceSegmentAddress = ssh.get<SSH::SliceSegmentAddress>();

    Ctb ctbAddrInRs = *sliceSegmentAddress;
    Ctb ctbAddrInTs = picture->toAddrInTs(ctbAddrInRs);

    const auto firstInSegmentCtbAddrInTs = ctbAddrInTs;

    auto endOfSliceSegmentFlag = embed<EndOfSliceSegmentFlag>(*this);
    auto endOfSubStreamOneBit = embed<EndOfSubStreamOneBit>(*this);

    const auto picWidthInCtbsY = picture->widthInCtbsY;
    const auto entropyCodingSyncEnabledFlag = pps->get<PPS::EntropyCodingSyncEnabledFlag>();
    const auto tilesEnabledFlag = pps->get<PPS::TilesEnabledFlag>();

    do
    {
        const auto isFirstInSegment = firstInSegmentCtbAddrInTs == ctbAddrInTs;
        const auto isFirstInIndependentSegment = isFirstInSegment && independentSliceSegment;
        const auto ctuCoord = picture->toCoord(ctbAddrInRs);

        /*--------------------------------------------------------------------*/
        /* CABAD: SS/Tile/WPP support */
        {
            /* tile boundary (CTU to be parsed is first CTU in a tile) */
            const auto tileBoundary =
                *tilesEnabledFlag
                && (
                        0_ctb == ctbAddrInTs
                        ||
                        picture->tileId(ctuCoord)
                        !=
                        picture->tileId(
                            picture->toCoord(
                                picture->toAddrInRs(ctbAddrInTs - 1_ctb))));


            /* WPP boundary (CTU to be parsed is first CTU in a row) */
            const auto wppBoundary =
                *entropyCodingSyncEnabledFlag
                && 0_ctb == ctbAddrInTs % picWidthInCtbsY;

            syntaxCheck(!(tileBoundary && wppBoundary));

            if(tileBoundary)
            {
                initialize(*picture, *cabacInitType, *sliceQpY, streamAccessLayer);
            }
            else if(wppBoundary)
            {
                /* location of spatial neighbouring block T */
                const CtbCoord coordNbT {ctuCoord.x() + 1_ctb, ctuCoord.y() - 1_ctb};

                const auto availableNbT =
                    picture->isAvailableInScanZ(
                            picture->toPel(ctuCoord), picture->toPel(coordNbT));

                if(availableNbT)
                {
                    syntaxCheck(!isFirstInIndependentSegment);
                    /* 04/2013,
                     * 9.3.2 "Initialization process"
                     * WPP: restore CABAD context on first CTU in a row */
                    restore(*picture, CABAD::RestorePoint::WPP, streamAccessLayer);
                }
                else
                {
                    initialize(*picture, *cabacInitType, *sliceQpY, streamAccessLayer);
                }
            }
            else if(isFirstInIndependentSegment)
            {
                syntaxCheck(isFirstInSegment);
                initialize(*picture, *cabacInitType, *sliceQpY, streamAccessLayer);
            }
            else if(isFirstInSegment)
            {
                syntaxCheck(!isFirstInIndependentSegment);
                restore(*picture, CABAD::RestorePoint::DSS, streamAccessLayer);
            }
        }
        /*--------------------------------------------------------------------*/
        auto ctu =
            embedSubtreeInList<CodingTreeUnit>(
                    *this,
                    picture->minCbSizeY,
                    picture->ctbSizeY,
                    picture->toCoord(ctbAddrInRs),
                    CtbAddr{ctbAddrInTs, ctbAddrInRs});

        parse(streamAccessLayer, decoder, *ctu, *sh);
        /*--------------------------------------------------------------------*/
        /* CABAD: Tiles & WPP support */
        if(*entropyCodingSyncEnabledFlag && 1_ctb == ctuCoord.x())
        {
            /* 04/2013,
             * 9.3.2.3 "Storage process for context variables"
             * WPP: store CABAD context at the end of second CTU in a row */
            picture->store(CABAD::RestorePoint::WPP);
        }
        /*--------------------------------------------------------------------*/

        parse(streamAccessLayer, decoder, *endOfSliceSegmentFlag);

        if(!(*endOfSliceSegmentFlag))
        {
            /* update ctb addresses */
            ++ctbAddrInTs;
            /* check if end_of_slice_segment_flag is signalled correctly */
            syntaxCheck(picture->sizeInCtbsY > ctbAddrInTs);

            ctbAddrInRs = picture->toAddrInRs(ctbAddrInTs);

            /* tile boundary (next CTU to be parsed is first CTU in a tile) */
            const auto tileBoundary =
                *tilesEnabledFlag
                && (
                        0_ctb == ctbAddrInTs
                        ||
                        picture->tileId(ctuCoord)
                        !=
                        picture->tileId(picture->toCoord(picture->toAddrInRs(ctbAddrInTs))));

            /* WPP boundary (next CTU to be parsed is first CTU in a row) */
            const auto wppBoundary =
                *entropyCodingSyncEnabledFlag
                && 0_ctb == ctbAddrInTs % picWidthInCtbsY;

            syntaxCheck(!(tileBoundary && wppBoundary));

            if(tileBoundary || wppBoundary)
            {
                parse(streamAccessLayer, decoder, *endOfSubStreamOneBit);

                syntaxCheck(*endOfSubStreamOneBit);

                auto byteAlignment = embedSubtreeInList<ByteAlignment>(*this);

                parse(streamAccessLayer, decoder, *byteAlignment, ElementId{Id});
            }
        }
    } while(!(*endOfSliceSegmentFlag));

    picture->store(CABAD::RestorePoint::DSS);
}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */
