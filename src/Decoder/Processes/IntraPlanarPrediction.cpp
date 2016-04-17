#include <Decoder/Processes/IntraPlanarPrediction.h>
#include <Structure/PelBuffer.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
void IntraPlanarPrediction::exec(
        State &,
        Structure::PelBuffer &dst,
        PelCoord coord, Log2 size,
        const IntraAdjSamples &adj)
{
    const auto side = toPel(size);
    const int nTbS = toUnderlying(side);

    for(auto y = 0_pel; y < side; ++y)
    {
        const auto yy = toUnderlying(y);

        for(auto x = 0_pel; x < side; ++x)
        {
            const auto xx = toUnderlying(x);
            // (8-40)
            const auto value =
                (nTbS - 1 - xx) * (*adj[{-1_pel, y}])
                + (xx + 1) * (*adj[{side, -1_pel}])
                + (nTbS - 1 - yy) * (*adj[{x, -1_pel}])
                + (yy + 1) * (*adj[{-1_pel, side}])
                + nTbS;

            const auto shiftBy = log2(nTbS) + 1;
            const auto shiftedValue = value >> shiftBy;

            dst[{coord.x() + x, coord.y() + y}] = shiftedValue;
        }
    }
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processses */
