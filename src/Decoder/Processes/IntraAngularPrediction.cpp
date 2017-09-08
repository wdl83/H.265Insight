#include <Decoder/Processes/IntraAngularPrediction.h>
#include <Structure/PelBuffer.h>
#include <log.h>


namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
ReferenceSamples deriveReferenceSamples(
        PelCoord coord,
        Log2 size, Plane plane,
        IntraPredictionMode predModeIntra,
        const IntraAdjSamples &adj)
{
    /* ITU-T H.265 v4 (12/2016)
     * 8.4.4.2.6 "Specification of intra prediction mode in the range of INTRA_ANGULAR2..INTRA_ANGULAR34" */

    // 1
    const auto sideLength = toPel(size);
    const int nTbS = toUnderlying(sideLength);
    const auto predAngle = toPredAngle(predModeIntra);
    const auto extend = 0 > predAngle && -1 > ((nTbS * predAngle) >> 5);
    const int extendBy = extend ? (nTbS * predAngle) >> 5 : 0;
    const int refOffset = extendBy;
    const int refLength = nTbS + 1 + (extend ? -extendBy : nTbS);

    ReferenceSamples refSamples({Pel{refOffset}, Pel{refOffset + refLength}});

    const auto above = IntraPredictionMode::Angular18 <= predModeIntra;

    // (8-53), (8-61), x = 0..nTbS
    for(auto i = 0_pel; i < sideLength + 1_pel; ++i)
    {
        const auto value =
            above
            ? *adj[{-1_pel + i, -1_pel}]
            : *adj[{-1_pel, -1_pel + i}];

        refSamples[i] = value;
    }

    if(extend)
    {
        const auto invAngle = toInvAngle(predModeIntra);

        // (8-54), (8-62), x = -1..(nTbS * intraPredAngle) >> 5
        for(auto i = Pel{extendBy}; 0_pel > i; ++i)
        {
            const int ii = toUnderlying(i);

            const auto value =
                above
                ? *adj[{-1_pel, -1_pel + Pel{(ii * invAngle + 128) >> 8}}]
                : *adj[{-1_pel + Pel{(ii * invAngle + 128) >> 8}, -1_pel}];

            refSamples[i] = value;
        }
    }
    else
    {
        // (8-55) or (8-63), x = nTbS + 1..2 * nTbS
        for(auto i = sideLength + 1_pel; i < sideLength * 2 + 1_pel; ++i)
        {
            const auto value =
                above
                ? *adj[{-1_pel + i, -1_pel}]
                : *adj[{-1_pel, -1_pel + i}];

            refSamples[i] = value;
        }
    }

    const auto toStr =
        [&](std::ostream &oss)
        {
            oss << "coord " << coord << '\n';
            refSamples.writeTo(
                oss,
                [](std::ostream &os, ReferenceSamples::Sample i) {pelFmt(os, i);});
        };

    const LogId logId[] =
    {
        LogId::IntraAngularRefSamplesY,
        LogId::IntraAngularRefSamplesCb,
        LogId::IntraAngularRefSamplesCr
    };

    log(logId[int(plane)], toStr);

    return refSamples;
}
/*----------------------------------------------------------------------------*/
void IntraAngularPrediction::exec(
        State &,
        Structure::PelBuffer &dst,
        PelCoord coord,
        Log2 size, Plane plane, int bitDepth,
        IntraPredictionMode predModeIntra,
        bool disableIntraBoundaryFilter,
        const IntraAdjSamples &adj)
{
    /* ITU-T H.265 v4 (12/2016)
     * 8.4.4.2.6 "Specification of intra prediction mode in the range of INTRA_ANGULAR2..INTRA_ANGULAR34" */
    // 1
    const auto refSamples = deriveReferenceSamples(coord, size, plane, predModeIntra, adj);
    // 2
    const auto sideLength = toPel(size);
    const int nTbS = toUnderlying(sideLength);
    const auto predAngle = toPredAngle(predModeIntra);
    const bool above = IntraPredictionMode::Angular18 <= predModeIntra;
    const auto adjTopLeft = *adj[{-1_pel, -1_pel}];

    const auto angle0 =
        above
        ? IntraPredictionMode::Angular26 == predModeIntra
        : IntraPredictionMode::Angular10 == predModeIntra;

    const auto filter =
        [&](Pel x, Pel y, int value)
        {
            if(
                    !disableIntraBoundaryFilter
                    && angle0
                    && Plane::Y == plane
                    && 32 > nTbS
                    && 0_pel == (above ? x : y))
            {
                // 2c.
                const auto adjValue1 = above ?  *adj[{0_pel, -1_pel}] :  *adj[{-1_pel, 0_pel}];
                const auto adjValue2 = above ? *adj[{-1_pel, y}] : *adj[{x, -1_pel}];

                // (8-60) or (8-68)
                const auto filtered =
                    clip1(bitDepth, adjValue1 + ((adjValue2 - adjTopLeft) >> 1));

                return filtered;
            }
            else
            {
                return value;
            }
        };

    // x,y = 0..nTbS - 1
    for(auto x = 0_pel; x < sideLength; ++x)
    {
        for(auto y = 0_pel; y < sideLength; ++y)
        {
            const auto i = above ? y : x;
            const auto ii = toUnderlying(i);
            const auto j = above ? x : y;

            // 2a
            // (8-56), (8-64)
            const auto iIdx = ((ii + 1) * predAngle) >> 5;
            // (8-57), (8-65)
            const auto iFact = ((ii + 1) * predAngle) & 31;

            // 2b
            // (8-58), (8-59) or (8-66), (8-67)
            const auto value =
                0 == iFact
                ? refSamples[{j + Pel{iIdx} + 1_pel}]
                : (
                        (32 - iFact)
                        * refSamples[{j + Pel{iIdx} + 1_pel}]
                        + iFact
                        * refSamples[{j + Pel{iIdx} + 2_pel}]
                        + 16) >> 5;

            dst[{coord.x() + x, coord.y() + y}] = filter(x, y, value);
        }
    }
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processses */
