#include <Decoder/Processes/TransformCoeffsScaling.h>
#include <Decoder/State.h>
#include <Syntax/SequenceParameterSet.h>
#include <Syntax/SpsRangeExtension.h>
#include <Syntax/CodingUnit.h>
#include <Syntax/ResidualCoding.h>
#include <Structure/ScalingFactor.h>
#include <Structure/Picture.h>
/* STDC++ */
#include <array>

namespace HEVC { namespace Decoder { namespace Processes {


/* ITU-T H.265 v4 12/2016
 * 8.6.3 "Scaling process for transform coefficients" */
typedef std::array<int8_t, 6> LevelScale;

static const LevelScale levelScale =
{{
    40, 45, 51, 57, 64, 72
}};

namespace {
/*----------------------------------------------------------------------------*/
int calcQp(
        Ptr<const Structure::Picture> picture,
        const Syntax::CodingUnit &cu,
        Plane plane)
{
    using namespace Syntax;

    typedef CodingUnit CU;

    const auto calcQpY =
        [picture](int qpPrimeY)
        {
            const auto qpBdOffsetY = picture->qpBdOffset(Component::Luma);
            // (8-293)
            return clip3(0, 51 + qpBdOffsetY, qpPrimeY);
        };

    const auto calcQpC =
        [](int qpPrimeC)
        {
            // (8-294)
            // (8-295)
            return qpPrimeC;
        };

    return
        Plane::Cb == plane
        ? calcQpC(cu.get<CU::QpC>()->qpPrimeCb)
        : (
                Plane::Cr == plane
                ? calcQpC(cu.get<CU::QpC>()->qpPrimeCr)
                : calcQpY(cu.get<CU::QpY>()->qpPrimeY));
}
/*----------------------------------------------------------------------------*/
const Structure::ScalingFactor::Factor &getFactor(
        State &decoder,
        Ptr<const Structure::Picture>,
        const Syntax::CodingUnit &cu,
        Plane plane, Log2 size)
{
    using namespace Syntax;

    const auto side = toInt(size);
    const auto sizeId = toSizeId(side);
    const auto cuPredMode = cu.get<CodingUnit::CuPredMode>();
    const auto matrixId = toMatrixId(plane, *cuPredMode);
    const auto scalingFactor = decoder.context()->get<Structure::ScalingFactor>();

    return (*scalingFactor)[makeTuple(sizeId, matrixId)];
}
/*----------------------------------------------------------------------------*/
} /* namespace */
/*----------------------------------------------------------------------------*/
void TransformCoeffsScaling::exec(
        State &decoder, Ptr<Structure::Picture> picture,
        const Syntax::CodingUnit &cu,
        const Syntax::ResidualCoding &rc)
{
    /* ITU-T H.265 v4 12/2016
     * 8.6.2 "Scaling and transformation process"
     * 8.6.3 "Scaling process for transform coefficients" */

    using namespace Syntax;

    typedef SequenceParameterSet SPS;
    typedef SpsRangeExtension SPSRE;
    typedef ResidualCoding RC;
    typedef Structure::ScalingFactor::Factor Factor;

    const auto sps = picture->sps;
    const auto spsre = picture->spsre;
    const auto extendedPrecisionProcessingFlag = spsre && bool(*spsre->get<SPSRE::ExtendedPrecisionProcessingFlag>());
    const bool scalingListEnabledFlag(*sps->get<SPS::ScalingListEnabledFlag>());
    const Plane plane = *rc.get<RC::CIdx>();
    const auto rcCoord = rc.get<RC::Coord>()->inUnits();
    const auto coord = scale(rcCoord, plane, picture->chromaFormatIdc);
    const auto rcSize = rc.get<RC::Size>()->inUnits();
    const auto side = toPel(rcSize);
    const auto &f = getFactor(decoder, picture, cu, plane, rcSize);
    auto &residuals = picture->pelBuffer(PelLayerId::Residual, plane);
    const bool transformSkipFlag(*rc.get<RC::TransformSkipFlag>());
    const auto bitDepth = picture->bitDepth(plane);
    // (8-302), (8-306)
    const auto transformRange = extendedPrecisionProcessingFlag ? std::max(15_log2, Log2{bitDepth} + 6_log2) : 15_log2;
    // (8-303), (8-307)
    const auto bdShift = bitDepth + toUnderlying(rcSize) + 10 - toUnderlying(transformRange);
    // (8-304), (8-305), (8-308), (8-309)
    const auto min = minCoeff(extendedPrecisionProcessingFlag, bitDepth);
    const auto max = maxCoeff(extendedPrecisionProcessingFlag, bitDepth);
    // (8-310)
    const auto scalingFactor16 = !scalingListEnabledFlag || transformSkipFlag && 2_log2 < rcSize;
    const auto qp = calcQp(picture, cu, plane);
    // (8-311)
    const auto qpQuotient = qp / 6;
    const auto qpRemainder = qp % 6;
    const int64_t scale = levelScale[qpRemainder];
    // (8-311)
    const int64_t bdOffset = 1 << (bdShift - 1);

    for(auto y = 0_pel; y < side; ++y)
    {
        for(auto x = 0_pel; x < side; ++x)
        {
            const auto at = coord + PelCoord{x, y};
            const int64_t m = scalingFactor16 ? 16 : f[Factor::Pos(toUnderlying(x), toUnderlying(y))];
            const int64_t level = residuals[at];
            const int64_t value = (((level * m * scale) << qpQuotient) + bdOffset) >> bdShift;
            const auto clippedValue = clip3(min, max, value);
            residuals[at] = clippedValue;
        }
    }

    const auto toStr =
        [&](std::ostream &oss)
        {
            oss << coord;
            oss << " scale " << scale;
            if(scalingFactor16) oss << " SF16";
            oss << '\n';

            for(auto y = 0_pel; y < toPel(rcSize); ++y)
            {
                for(auto x = 0_pel; x < toPel(rcSize); ++x)
                {
                    pelFmt(oss, residuals[coord + PelCoord{x, y}]);
                    oss  << ' ';
                }

                oss << '\n';
            }
        };

    const LogId logId[] =
    {
        LogId::ScaledTransformCoeffsY,
        LogId::ScaledTransformCoeffsCb,
        LogId::ScaledTransformCoeffsCr
    };

    log(logId[int(plane)], toStr);
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processses */
