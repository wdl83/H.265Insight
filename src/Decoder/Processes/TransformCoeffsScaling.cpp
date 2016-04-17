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

typedef std::array<int8_t, 6> LevelScale;

static const LevelScale levelScale =
{{
    40, 45, 51, 57, 64, 72
}};

namespace {
/*----------------------------------------------------------------------------*/
int calcQp(
        Ptr<const Structure::Picture>,
        const Syntax::CodingUnit &cu,
        Plane plane)
{
    using namespace Syntax;

    typedef CodingUnit CU;

    return
        Plane::Cb == plane
        ? cu.get<CU::QpC>()->qpPrimeCb
        : (
                Plane::Cr == plane
                ? cu.get<CU::QpC>()->qpPrimeCr
                : cu.get<CU::QpY>()->qpPrimeY);
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
    using namespace Syntax;

    typedef SequenceParameterSet SPS;
    typedef SpsRangeExtension SPSRE;
    typedef ResidualCoding RC;
    typedef Structure::ScalingFactor::Factor Factor;

    const auto sps = picture->sps;
    const auto spsre = picture->spsre;
    const auto extendedPrecisionProcessingFlag =
        spsre && bool(*spsre->get<SPSRE::ExtendedPrecisionProcessingFlag>());

    const bool scalingListEnabledFlag(*sps->get<SPS::ScalingListEnabledFlag>());
    const auto rcCoord = rc.get<RC::Coord>()->inUnits();
    const auto rcSize = rc.get<RC::Size>()->inUnits();
    const Plane plane = *rc.get<RC::CIdx>();
    const bool transformSkipFlag(*rc.get<RC::TransformSkipFlag>());
    const auto bitDepth = picture->bitDepth(plane);

    const auto transformRange =
        extendedPrecisionProcessingFlag
        ? std::max(15_log2, Log2{bitDepth} + 6_log2)
        : 15_log2;
    const int bdShift = bitDepth + toUnderlying(rcSize) + 10 - toUnderlying(transformRange);
    const int64_t bdOffset = 1 << (bdShift - 1);
    const auto min = minCoeff(extendedPrecisionProcessingFlag, bitDepth);
    const auto max = maxCoeff(extendedPrecisionProcessingFlag, bitDepth);

    const auto coord = scale(rcCoord, plane, picture->chromaFormatIdc);
    const auto qp = calcQp(picture, cu, plane);
    const auto qpQuotient = qp / 6;
    const auto qpRemainder = qp % 6;
    const int64_t scale = levelScale[qpRemainder];
    const auto &f = getFactor(decoder, picture, cu, plane, rcSize);
    const auto side = toPel(rcSize);
    auto &residuals = picture->pelBuffer(PelLayerId::Residual, plane);

    for(auto y = 0_pel; y < side; ++y)
    {
        for(auto x = 0_pel; x < side; ++x)
        {
            const auto at = coord + PelCoord{x, y};
            const int64_t level = residuals[at];

            const int64_t m =
                !scalingListEnabledFlag || transformSkipFlag && 2_log2 > rcSize
                ? 16
                : f[Factor::Pos(toUnderlying(x), toUnderlying(y))];

            const int64_t value = (((level * m * scale) << qpQuotient) + bdOffset) >> bdShift;
            const auto clippedValue = clip3(min, max, value);
            residuals[at] = clippedValue;
        }
    }

    const auto toStr =
        [coord, rcSize, &residuals](std::ostream &oss)
        {
            oss << coord << '\n';

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
