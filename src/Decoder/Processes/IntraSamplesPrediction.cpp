#include <Decoder/Processes/IntraSamplesPrediction.h>
#include <Decoder/Process.h>
#include <Decoder/Processes/IntraRefSamplesSubstitution.h>
#include <Decoder/Processes/IntraAdjFiltering.h>
#include <Decoder/Processes/IntraPlanarPrediction.h>
#include <Decoder/Processes/IntraDcPrediction.h>
#include <Decoder/Processes/IntraAngularPrediction.h>
#include <Syntax/SequenceParameterSet.h>
#include <Syntax/SpsRangeExtension.h>
#include <Syntax/PictureParameterSet.h>
#include <Syntax/CodingUnit.h>
#include <Structure/Picture.h>
#include <log.h>


namespace HEVC { namespace Decoder { namespace Processes {

namespace {
/*----------------------------------------------------------------------------*/
void logPredictedSamples(
        Plane plane,
        PelCoord coord, PelCoord at, Pel side, const Structure::PelBuffer &src,
        IntraPredictionMode mode)
{
    const auto toStr =
        [&](std::ostream &oss)
        {
            oss << coord << ' ' << getName(mode) << '\n';

            for(auto y = at.y(); y < at.y() + side; ++y)
            {
                for(auto x = at.x(); x < at.x() + side; ++x)
                {
                    pelFmt(oss, src[{x, y}]);
                    oss << (at.x() + side - 1_pel == x ? '\n' : ' ');
                }
            }
        };

    const LogId logId[] =
    {
        LogId::IntraPredictedSamplesY,
        LogId::IntraPredictedSamplesCb,
        LogId::IntraPredictedSamplesCr
    };

    log(logId[int(plane)], toStr);
}
/*----------------------------------------------------------------------------*/
IntraAdjSamples deriveAdjSamples(
        Ptr<const Structure::Picture> picture,
        Plane plane,
        Log2, Log2 inPlaneSize,
        PelCoord coord, PelCoord inPlaneCoord)
{
    using namespace Syntax;

    typedef PictureParameterSet PPS;

    const auto pps = picture->pps;
    const auto constrainedIntraPredFlag = pps->get<PPS::ConstrainedIntraPredFlag>();
    const auto side = toPel(inPlaneSize + 1_log2);

    IntraAdjSamples adjSamples(side);

    const auto isNotIntra =
        [picture](PelCoord at)
        {
            const auto cuPredMode =
                picture->getCodingUnit(at)->get<CodingUnit::CuPredMode>();

            return PredictionMode::Intra != *cuPredMode;
        };

    const auto &reconstructed = picture->pelLayer(PelLayerId::Reconstructed);

    const auto deriveAdjSample =
        [&](PelCoord offset)
        {
            const auto inPlaneAdjCoord = inPlaneCoord + offset;
            const auto adjCoord =
                invScale(
                        inPlaneAdjCoord, plane,
                        picture->chromaFormatIdc);

            const auto adjAvailableInScanZ =
                picture->isAvailableInScanZ(coord, adjCoord);

            if(
                    !adjAvailableInScanZ
                    || *constrainedIntraPredFlag && isNotIntra(adjCoord))
            {
                /* adj sample not available for intra prediction */
            }
            else
            {
                const auto adjSampleValue = reconstructed[int(plane)][inPlaneAdjCoord];
                adjSamples[offset] = adjSampleValue;
            }
        };

    for(auto x = -1_pel; x < adjSamples.getSideEnd(); ++x)
    {
        deriveAdjSample({x, -1_pel});
    }

    for(auto y = 0_pel; y < adjSamples.getSideEnd(); ++y)
    {
        deriveAdjSample({-1_pel, y});
    }

    const auto toStr =
        [coord, &adjSamples](std::ostream &oss)
        {
            oss << coord << '\n';

            adjSamples.writeTo(
                    oss,
                    [](std::ostream &os, IntraAdjSamples::AdjSample i){pelFmt(os, *i);});
        };

    const LogId logId[] =
    {
        LogId::IntraAdjSamplesY,
        LogId::IntraAdjSamplesCb,
        LogId::IntraAdjSamplesCr
    };

    log(logId[int(plane)], toStr);
    return adjSamples;
}
/*----------------------------------------------------------------------------*/
} /* namespace */

/*----------------------------------------------------------------------------*/
void IntraSamplesPrediction::exec(
        State &decoder,
        Ptr<Structure::Picture> picture,
        Ptr<const Syntax::CodingUnit> cu,
        Plane plane,
        PelCoord coord, Log2 size,
        PelCoord inPlaneCoord, Log2 inPlaneSize)
{
    using namespace Syntax;

    typedef SequenceParameterSet SPS;
    typedef SpsRangeExtension SPSRE;
    typedef CodingUnit CU;

    auto &dst = picture->pelBuffer(PelLayerId::Prediction, plane);
    const auto chromaFormatIdc = picture->chromaFormatIdc;
    const auto is444 = ChromaFormatIdc::f444 == chromaFormatIdc;
    const auto bitDepth = picture->bitDepth(plane);

    const auto spsre = picture->spsre;
    const bool implicitRdpcmEnabledFlag =
        spsre && bool(*spsre->get<SPSRE::ImplicitRdpcmEnabledFlag>());
    const bool intraSmoothingDisabledFlag =
        spsre && bool(*spsre->get<SPSRE::IntraSmoothingDisabledFlag>());

    const auto predModeIntra = cu->intraPredictionMode(plane, coord);
    const bool cuTransquantBypassFlag(*cu->get<CU::CuTransquantBypassFlag>());
    const auto disableIntraBoundaryFilter = implicitRdpcmEnabledFlag && cuTransquantBypassFlag;

    runtime_assert(IntraPredictionMode::Undefined != predModeIntra);

    auto adjSamples =
        deriveAdjSamples(picture, plane, size, inPlaneSize, coord, inPlaneCoord);

    if(!adjSamples.areAllAvailable())
    {
        /* call: 04/2013, 8.4.4.2.2,
         * "Reference sample substitution process for intra sample prediction" */
        adjSamples =
            subprocess(
                    decoder, IntraRefSamplesSubstitution(),
                    plane, coord, bitDepth, std::move(adjSamples));
    }

    runtime_assert(adjSamples.areAllAvailable());

    if(!intraSmoothingDisabledFlag && (Plane::Y == plane || is444))
    {
        /* call: 04/2013, 8.4.4.2.3
         * "Filtering process of neighbouring samples" */
        adjSamples =
            subprocess(
                    decoder, IntraAdjFiltering(),
                    picture, plane, inPlaneSize, predModeIntra, std::move(adjSamples));
    }

    runtime_assert(adjSamples.areAllAvailable());

    /* after substitution and filtering adj samples become reference samples
     * for prediction */
    {
        const auto toStr =
            [coord, &adjSamples](std::ostream &oss)
            {
                oss << coord << '\n';

                adjSamples.writeTo(
                        oss,
                        [](std::ostream &os, IntraAdjSamples::AdjSample i){pelFmt(os, *i);});
            };

        const LogId logId[] =
        {
            LogId::IntraRefSamplesY,
            LogId::IntraRefSamplesCb,
            LogId::IntraRefSamplesCr
        };

        log(logId[int(plane)], toStr);
    }

    if(IntraPredictionMode::Planar == predModeIntra)
    {
        /* call: 04/2013, 8.4.4.2.4
         * "Specification of intra prediction mode INTRA_PLANAR" */
        process(
                decoder, IntraPlanarPrediction(),
                dst, inPlaneCoord, inPlaneSize, adjSamples);
    }
    else if(IntraPredictionMode::Dc == predModeIntra)
    {
        /* call: 04/2013, 8.4.4.2.5
         * "Specification of intra prediction mode INTRA_DC" */
        process(
                decoder, IntraDcPrediction(),
                dst, inPlaneCoord, inPlaneSize, plane, adjSamples);
    }
    else
    {
        /* call: 04/2013, 8.4.4.2.6
         * "Specification of intra prediction mode in range
         * INTRA_ANGULAR2.. INTRA_ANGULAR34" */
        process(
                decoder, IntraAngularPrediction(),
                dst,
                inPlaneCoord, inPlaneSize,
                plane, bitDepth,
                predModeIntra,
                disableIntraBoundaryFilter,
                adjSamples);
    }

    logPredictedSamples(plane, coord, inPlaneCoord, toPel(inPlaneSize), dst, predModeIntra);
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */
