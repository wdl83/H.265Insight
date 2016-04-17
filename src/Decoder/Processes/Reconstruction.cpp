#include <Decoder/Processes/Reconstruction.h>
#include <Structure/Picture.h>
#include <log.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
void Reconstruction::exec(
        State &,
        Ptr<Structure::Picture> picture,
        Plane plane,
        PelCoord coord, Log2,
        PelCoord inPlaneCoord, Pel width, Pel height)
{
    using namespace Syntax;
    using namespace Structure;

    const auto bitDepth = picture->bitDepth(plane);
    const auto &prediction = picture->pelBuffer(PelLayerId::Prediction, plane);
    const auto &residual = picture->pelBuffer(PelLayerId::Residual, plane);
    auto &dst = picture->pelBuffer(PelLayerId::Reconstructed, plane);

    const auto yBegin = inPlaneCoord.y();
    const auto yEnd = inPlaneCoord.y() + height;
    const auto xBegin = inPlaneCoord.x();
    const auto xEnd = inPlaneCoord.x() + width;

    for(auto y = yBegin; y < yEnd; ++y)
    {
        for(auto x = xBegin; x < xEnd; ++x)
        {
            const PelCoord i{x, y};

            dst[i] = clip1(bitDepth, prediction[i] + residual[i]);
        }
    }

    const auto toStr =
        [coord, &dst, yBegin, yEnd, xBegin, xEnd](std::ostream &oss)
        {
            oss << coord << '\n';

            for(auto y = yBegin; y < yEnd; ++y)
            {
                for(auto x = xBegin; x < xEnd; ++x)
                {
                    pelFmt(oss, dst[{x, y}]);
                    oss << (xEnd - 1_pel == x ? '\n' : ' ');
                }
            }
        };

    const LogId logId[] =
    {
        LogId::ReconstructedY,
        LogId::ReconstructedCb,
        LogId::ReconstructedCr
    };

    log(logId[int(plane)], toStr);
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */
