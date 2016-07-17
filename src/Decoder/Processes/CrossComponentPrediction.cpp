#include <Decoder/Processes/CrossComponentPrediction.h>
#include <Structure/Picture.h>
#include <Syntax/SpsRangeExtension.h>
#include <Syntax/PpsRangeExtension.h>
#include <Syntax/TransformUnit.h>


namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/

void CrossComponentPrediction::exec(
        State &, Ptr<Structure::Picture> picture,
        const Syntax::TransformUnit &tu, Chroma chroma)
{
    using namespace Syntax;

    typedef SpsRangeExtension SPSRE;
    typedef PpsRangeExtension PPSRE;
    typedef TransformUnit TU;

    const auto spsre = picture->spsre;
    const auto ppsre = picture->ppsre;
    const auto extendedPrecisionProcessingFlag =
        spsre && bool(*spsre->get<SPSRE::ExtendedPrecisionProcessingFlag>());
    const auto crossComponentPredictionEnabledFlag =
        ppsre && bool(*ppsre->get<PPSRE::CrossComponentPredictionEnabledFlag>());

    if(crossComponentPredictionEnabledFlag)
    {
        const auto bitDepthY = picture->bitDepth(Component::Luma);
        const auto bitDepthC = picture->bitDepth(Component::Chroma);
        const auto min = minCoeff(extendedPrecisionProcessingFlag, bitDepthC);
        const auto max = maxCoeff(extendedPrecisionProcessingFlag, bitDepthC);

        const auto &src = picture->pelBuffer(PelLayerId::Residual, Plane::Y);
        auto &dst = picture->pelBuffer(PelLayerId::Residual, toPlane(chroma));

        const auto tuCoord = tu.get<TU::Coord>()->inUnits();
        const auto tuSize = tu.get<TU::Size>()->inUnits();
        const auto side = toPel(tuSize);
        const auto resScaleVal = tu.resScaleVal(chroma);

        for(auto y = 0_pel; y < side; ++y)
        {
            for(auto x = 0_pel; x < side; ++x)
            {
                const auto at = tuCoord + PelCoord{x, y};
                const auto unClipped =
                    (resScaleVal * ((int32_t(src[at]) << bitDepthC) >> bitDepthY)) >> 3;
                dst[at] = clip3(min, max, int32_t(dst[at]) + unClipped);
            }
        }

        const auto toStr =
            [tuCoord, side, resScaleVal, &dst](std::ostream &os)
            {
                os << tuCoord << ' ' << resScaleVal << '\n';

                for(auto y = 0_pel; y < side; ++y)
                {
                    for(auto x = 0_pel; x < side; ++x)
                    {
                        pelFmt(os, dst[tuCoord + PelCoord{x, y}]);
                        os << (side - 1_pel == x ? '\n' : ' ');
                    }
                }
            };

        const LogId logId[] =
        {
            LogId::CrossComponentPredictionCb,
            LogId::CrossComponentPredictionCr
        };

        log(logId[int(chroma)], toStr);
    }
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processses */
