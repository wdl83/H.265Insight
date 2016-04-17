#include <Decoder/Processes/Residuals.h>
#include <Decoder/Process.h>
#include <Decoder/Processes/TransformCoeffsScaling.h>
#include <Decoder/Processes/Transformation.h>
#include <Syntax/SequenceParameterSet.h>
#include <Syntax/SpsRangeExtension.h>
#include <Syntax/CodingUnit.h>
#include <Syntax/ResidualCoding.h>
#include <Structure/Picture.h>
#include <log.h>
/* stdc++ */
#include <limits>


namespace HEVC { namespace Decoder { namespace Processes {

namespace {
/*----------------------------------------------------------------------------*/
void skipTransform(
        Structure::PelBuffer &r,
        PelCoord base, Log2 size,
        int min, int max,
        int bdShift, int bdOffset, int tsShift,
        bool rotate)
{
    const auto side = toPel(size);

    if(rotate)
    {
        base += PelCoord{side - 1_pel, side - 1_pel};
    }

    for(auto y = 0_pel; y < side; ++y)
    {
        for(auto x = 0_pel; x < side; ++x)
        {
            const auto at = base + (rotate ? PelCoord{-x, -y} : PelCoord{x, y});
            // 2 (shift)
            const auto shifted = int32_t(r[at]) << tsShift;
            // 3 (bdShift)
            const auto unClipped = (shifted + bdOffset) >> bdShift;

            runtime_assert(!overflow(shifted, bdOffset));
            /* WARNING: this clip3 is not part of 04/2013 specification,
             * but is required by implementation */
            r[at] = clip3(min, max, unClipped);
        }
    }
}
/*----------------------------------------------------------------------------*/
/* 10/2014, 8.6.5
 * "Residual modification process for blocks using a transform bypass" */
void bypassTransform(
        Structure::PelBuffer &r,
        PelCoord base, Log2 size,
        Direction dir)
{
    const auto isV = Direction::H == dir;
    const auto isH = Direction::V == dir;
    const auto side = toPel(size);
    const auto xOffset = isH ? 1_pel : 0_pel;
    const auto yOffset = isV ? 1_pel : 0_pel;

    for(auto y = yOffset; y < side; ++y)
    {
        for(auto x = xOffset; x < side; ++x)
        {
            const auto dst = base + PelCoord{x, y};
            const PelCoord src{dst.x() - xOffset, dst.y() - yOffset};

            r[dst] = r[src];
        }
    }
}
/*----------------------------------------------------------------------------*/
} /* namespace */

/*----------------------------------------------------------------------------*/
void Residuals::exec(
        State &decoder,
        Ptr<Structure::Picture> picture,
        const Syntax::ResidualCoding &rc)
{
    /* 04/2013, 8.6.2 "Scaling and transformation process" */
    using namespace Syntax;

    typedef CodingUnit CU;
    typedef ResidualCoding RC;
    typedef SpsRangeExtension SPSRE;

    const auto spsre = picture->spsre;

    const auto extendedPrecisionProcessingFlag =
        spsre && bool(*spsre->get<SPSRE::ExtendedPrecisionProcessingFlag>());
    const auto transformSkipRotationEnabledFlag =
        spsre && bool(*spsre->get<SPSRE::TransformSkipRotationEnabledFlag>());
    const auto implicitRdpcmEnabledFlag =
        spsre && bool(*spsre->get<SPSRE::ImplicitRdpcmEnabledFlag>());

    const auto rcCoord = rc.get<RC::Coord>()->inUnits();
    const auto rcSize = rc.get<RC::Size>()->inUnits();
    const Plane plane = *rc.get<ResidualCoding::CIdx>();
    const bool transformSkipFlag(*rc.get<ResidualCoding::TransformSkipFlag>());

    const auto coord = scale(rcCoord, plane, picture->chromaFormatIdc);
    const auto cu = picture->getCodingUnit(rcCoord);
    const auto cuPredMode = cu->get<CU::CuPredMode>();
    const auto isIntra = HEVC::isIntra(*cuPredMode);
    const auto isInter = HEVC::isInter(*cuPredMode);
    const bool cuTransquantBypassFlag(*cu->get<CU::CuTransquantBypassFlag>());
    auto &r = picture->pelBuffer(PelLayerId::Residual, plane);

    const auto bitDepth = picture->bitDepth(plane);
    const auto min = minCoeff(extendedPrecisionProcessingFlag, bitDepth);
    const auto max = maxCoeff(extendedPrecisionProcessingFlag, bitDepth);
    const auto rotate = transformSkipRotationEnabledFlag && 2_log2 == rcSize && isIntra;

    const auto intraPredictionMode =
        [rcCoord, plane, cu]()
        {
            return  cu->intraPredictionMode(plane, rcCoord);
        };

    const auto intraRDPCM =
        isIntra
        && implicitRdpcmEnabledFlag
        && (cuTransquantBypassFlag || transformSkipFlag)
        && (
                IntraPredictionMode::Angular10 == intraPredictionMode()
                || IntraPredictionMode::Angular26 == intraPredictionMode());
    const auto interRDPCM =
        isInter
        && *rc.get<RC::ExplicitRdpcmFlag>();

    if(!cuTransquantBypassFlag)
    {
        const auto bdShift = std::max(20 - bitDepth, extendedPrecisionProcessingFlag ? 11 : 0);
        const auto bdOffset = 1 << (bdShift - 1);
        const auto tsShift =
            toUnderlying(rcSize)
            + (
                    extendedPrecisionProcessingFlag
                    ? std::min(5, bdShift - 2) : 5);

        // 1 (scale transform coefficients)
        process(decoder, TransformCoeffsScaling(), picture, *cu, rc);
        // 2
        if(transformSkipFlag)
        {
            skipTransform(
                    r,
                    coord, rcSize,
                    min, max,
                    bdShift, bdOffset, tsShift,
                    rotate);
        }
        else
        {
            process(decoder, Transformation(), picture, *cu, rc);
        }
    }
    else if(rotate)
    {
        const auto side = toPel(rcSize);
        const auto sideDiv2Minus1 = toPel(rcSize - 1_log2) - 1_pel;

        for(auto y = 0_pel; y < sideDiv2Minus1; ++y)
        {
            for(auto x = 0_pel; x < sideDiv2Minus1; ++x)
            {
                const auto atA = coord + PelCoord{x, y};
                const auto atB = coord + PelCoord{side - 1_pel - x, side - 1_pel - y};
                std::swap(r[atA], r[atB]);
            }
        }
    }

    if(intraRDPCM)
    {
        bypassTransform(
                r, rcCoord, rcSize,
                static_cast<Direction>(int(intraPredictionMode()) / 26));
    }
    else if(interRDPCM)
    {
        bypassTransform(
                r, rcCoord, rcSize,
                rc.get<RC::ExplicitRdpcmDirFlag>()->inUnits());
    }

    const auto toStr =
        [coord, rcSize, &r](std::ostream &oss)
        {
            const auto side = toPel(rcSize);

            oss << coord << '\n';

            for(auto y = 0_pel; y < side; ++y)
            {
                for(auto x = 0_pel; x < side; ++x)
                {
                    pelFmt(oss, r[coord + PelCoord{x, y}]);
                    oss << ' ';
                }

                oss << '\n';
            }
        };

    const LogId logId[] =
    {
        LogId::ResidualsY,
        LogId::ResidualsCb,
        LogId::ResidualsCr
    };

    log(logId[int(plane)], toStr);
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processses */
