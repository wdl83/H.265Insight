#include <Decoder/Processes/IntraAdjFiltering.h>
#include <Syntax/SequenceParameterSet.h>
#include <Structure/Picture.h>
#include <log.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
namespace {
/*----------------------------------------------------------------------------*/
int deriveMinDistVerHor(IntraPredictionMode predModeIntra)
{
    const auto minDistVerHor =
        std::min(
                std::abs(castToUnderlying(predModeIntra) - 26),
                std::abs(castToUnderlying(predModeIntra) - 10));

    return minDistVerHor;
}
/*----------------------------------------------------------------------------*/
int deriveIntraHorVerDistThres(Log2 size)
{
    /* 04/2013 && 10/2014, 8.4.4.2.3, "Filtering process of neighbouring samples"
     * Table 8-3 (10/2014 Table 8-4) */
    runtime_assert(3_log2 == size || 4_log2 == size || 5_log2 == size);
    return
        3_log2 == size
        ? 7
        : (
                4_log2 == size
                ? 1
                : 0 /* 5_log2 */);
}
/*----------------------------------------------------------------------------*/
bool deriveFilterFlag(int minDistVerHor, int intraHorVerDistThres)
{
    return minDistVerHor > intraHorVerDistThres;
}
/*----------------------------------------------------------------------------*/
bool deriveBiIntFlag(
        const Structure::Picture &picture,
        Plane plane,
        Log2 size,
        const IntraAdjSamples &adj)
{
    using namespace Syntax;

    typedef SequenceParameterSet SPS;

    if(Plane::Y == plane && 5_log2 == size)
    {
        const auto sps = picture.sps;
        const bool strongIntraSmoothingEnabledFlag =
            bool(*sps->get<SPS::StrongIntraSmoothingEnabledFlag>());
        const auto bitDepthY = picture.bitDepth(Component::Luma);

        const auto sideEnd = adj.getSideEnd();
        const auto halfSideEnd = sideEnd / 2;

        /* (-1, -1) */
        const auto topLeft = *adj[{-1_pel, -1_pel}];
        /* (nTbS * 2 - 1, -1) */
        const auto topRight = *adj[{sideEnd - 1_pel, -1_pel}];
        /* (nTbS - 1, -1) */
        const auto topMiddle = *adj[{halfSideEnd - 1_pel, -1_pel}];
        /* (-1, nTbS * 2 - 1)*/
        const auto bottomLeft = *adj[{-1_pel, sideEnd - 1_pel}];
        /* (-1, nTbS - 1) */
        const auto leftMiddle = *adj[{-1_pel, halfSideEnd - 1_pel}];

        const auto topBelowAvg =
            std::abs(topLeft + topRight - 2 * topMiddle) < (1 << (bitDepthY - 5));
        const auto bottomBelowAvg =
            std::abs(topLeft + bottomLeft - 2 * leftMiddle) < (1 << (bitDepthY - 5));

        const auto biIntFlag =
            strongIntraSmoothingEnabledFlag
            && topBelowAvg
            && bottomBelowAvg;

        return biIntFlag;
    }

    return false;
}
/*----------------------------------------------------------------------------*/
IntraAdjSamples result(
        Plane plane,
        IntraAdjSamples &&adj)
{
    const LogId logId[] =
    {
        LogId::IntraAdjFilteredSamplesY,
        LogId::IntraAdjFilteredSamplesCb,
        LogId::IntraAdjFilteredSamplesCr
    };

    log(
        logId[int(plane)],
        [&](std::ostream &oss)
        {
            adj.writeTo(
                    oss,
                    [](std::ostream &os, IntraAdjSamples::AdjSample i) {pelFmt(os, *i);});
        });

    runtime_assert(adj.areAllAvailable());

    return adj;
}
/*----------------------------------------------------------------------------*/
} /* namespace */

/*----------------------------------------------------------------------------*/
IntraAdjSamples IntraAdjFiltering::exec(
        State &,
        Ptr<const Structure::Picture> picture,
        PelCoord coord,
        Plane plane, Log2 size,
        IntraPredictionMode predModeIntra,
        IntraAdjSamples &&adj)
{
    runtime_assert(adj.areAllAvailable());

    const auto filterFlag =
            IntraPredictionMode::Dc == predModeIntra || 2_log2 == size
            ? false
            : deriveFilterFlag(
                    deriveMinDistVerHor(predModeIntra),
                    deriveIntraHorVerDistThres(size));

    const LogId logId[] =
    {
        LogId::IntraAdjFilteredSamplesY,
        LogId::IntraAdjFilteredSamplesCb,
        LogId::IntraAdjFilteredSamplesCr
    };

    log(
        logId[int(plane)],
        [&](std::ostream &oss)
        {
            oss << "coord " << coord;
            oss << " filterFlag " << filterFlag << '\n';
        });

    if(filterFlag)
    {
        const auto biIntFlag = deriveBiIntFlag(*picture, plane, size, adj);
        const auto topLeft = *adj[{-1_pel, -1_pel}];

        log(
            logId[int(plane)],
            [&](std::ostream &oss)
            {
                oss << "biIntFlag " << biIntFlag << '\n';
            });

        if(biIntFlag)
        {
            runtime_assert(5_log2 == size);

            const auto bottomLeft = *adj[{-1_pel, 63_pel}];
            const auto topRight = *adj[{63_pel, -1_pel}];

            // (8-30)
            //adj[{-1_pel, -1_pel}] = topLeft;

            // (8-31)
            for(auto y = 0_pel; y < 63_pel; ++y)
            {
                const auto value =
                    (63 - toUnderlying(y)) * topLeft
                    + (toUnderlying(y) + 1) * bottomLeft
                    + 32;

                adj[{-1_pel, y}] = value >> 6;
            }

            // (8-32)
            //adj[{-1_pel, 63_pel}] = bottomLeft;

            // (8-33)
            for(auto x = 0_pel; x < 63_pel; ++x)
            {
                const auto value =
                    (63 - toUnderlying(x)) * topLeft
                    + (toUnderlying(x) + 1) * topRight
                    + 32;

                adj[{x, -1_pel}] = value >> 6;
            }

            // (8-34)
            //adj[{63_pel, -1_pel}] = topRight;

            return result(plane, std::move(adj));
        }
        else
        {

            IntraAdjSamples filteredAdjSamples(toPel(size) * 2);
            const auto sideEnd = filteredAdjSamples.getSideEnd();

            {
                // (8-35)
                const auto value =
                    *adj[{-1_pel, 0_pel}] + 2 * topLeft + *adj[{0_pel, -1_pel}] + 2;

                filteredAdjSamples[{-1_pel, -1_pel}] = value >> 2;
            }

            // (8-36)
            for(auto y = 0_pel; y < sideEnd - 1_pel; ++y)
            {
                const auto value =
                    *adj[{-1_pel, y + 1_pel}] + 2 * *adj[{-1_pel, y}] + *adj[{-1_pel, y - 1_pel}] + 2;

                filteredAdjSamples[{-1_pel, y}] = value >> 2;
            }

            {
                // (8-37)
                const auto value = *adj[{-1_pel, sideEnd - 1_pel}];

                filteredAdjSamples[{-1_pel, sideEnd - 1_pel}] = value;
            }

            // (8-38)
            for(auto x = 0_pel; x < sideEnd - 1_pel; ++x)
            {
                const auto value =
                    *adj[{x - 1_pel, -1_pel}] + 2 * *adj[{x, -1_pel}] + *adj[{x + 1_pel, -1_pel}] + 2;

                filteredAdjSamples[{x, -1_pel}] = value >> 2;
            }

            {
                // (8-39)
                const auto value = *adj[{sideEnd - 1_pel, -1_pel}];

                filteredAdjSamples[{sideEnd - 1_pel, -1_pel}] = value;
            }

            return result(plane, std::move(filteredAdjSamples));
        }
    }
    else
    {
        /* no filtering */
        return result(plane, std::move(adj));
    }
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processses */
