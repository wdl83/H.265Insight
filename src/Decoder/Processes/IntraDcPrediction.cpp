#include <Decoder/Processes/IntraDcPrediction.h>
#include <Structure/PelBuffer.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
namespace {
/*----------------------------------------------------------------------------*/
int deriveDcVal(Log2 size, const IntraAdjSamples &adj)
{
    // (8-41)
    const auto sideLength = toPel(size);
    const int nTbS = toUnderlying(sideLength);
    // k = Log2(nTbS)
    const int k = toUnderlying(size);

    int hDcVal = 0;

    for(auto x = 0_pel; x < sideLength; ++x)
    {
        hDcVal += *adj[{x, -1_pel}];
    }

    int vDcVal = 0;

    for(auto y = 0_pel; y < sideLength; ++y)
    {
        vDcVal += *adj[{-1_pel, y}];
    }

    const int dcVal = (hDcVal + vDcVal + nTbS) >> (k + 1);

    return dcVal;
}
/*----------------------------------------------------------------------------*/
}

/*----------------------------------------------------------------------------*/
void IntraDcPrediction::exec(
        State &,
        Structure::PelBuffer &dst,
        PelCoord coord, Log2 size, Plane plane,
        const IntraAdjSamples &adj)
{
    /* 04/2013, 8.4.4.2.5, "Specification of intra prediction mode INTRA_DC" */
    const auto sideLength = toPel(size);
    const auto dcVal = deriveDcVal(size, adj);
    auto xOffset = 0_pel, yOffset = 0_pel;

    if(Plane::Y == plane && 5_log2 > size)
    {
        // (8-42)
        const auto topLeft = *adj[{-1_pel, 0_pel}] + 2 * dcVal + *adj[{0_pel, -1_pel}] + 2;

        dst[{coord.x() + 0_pel, coord.y() + 0_pel}] = topLeft >> 2;

        // (8-43)
        for(auto x = 1_pel; x < sideLength; ++x)
        {
            const auto value = *adj[{x, -1_pel}] + 3 * dcVal + 2;

            dst[{coord.x() + x, coord.y() + 0_pel}] = value >> 2;
        }
        // (8-44)
        for(auto y = 1_pel; y < sideLength; ++y)
        {
            const auto value = *adj[{-1_pel, y}] + 3 * dcVal + 2;

            dst[{coord.x() + 0_pel, coord.y() + y}] = value >> 2;
        }

        xOffset = 1_pel;
        yOffset = 1_pel;
    }

    for(auto y = yOffset; y < sideLength; ++y)
    {
        for(auto x = xOffset; x < sideLength; ++x)
        {
            // (8-45), (8-46)
            dst[{coord.x() + x, coord.y() + y}] = dcVal;
        }
    }
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processses */
