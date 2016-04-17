#include <Decoder/Processes/IntraRefSamplesSubstitution.h>
#include <log.h>


namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
IntraAdjSamples IntraRefSamplesSubstitution::exec(
        State &,
        Plane plane, PelCoord coord, int bitDepth,
        IntraAdjSamples &&adj)
{
    if(adj.areAllUnAvailable())
    {
        auto setToMean =
            [&](const PelCoord &, IntraAdjSamples::AdjSample &sample)
            {
                sample = 1 << (bitDepth - 1);
            };

        adj.forEach(setToMean);
    }
    else
    {
        const auto sideEnd = adj.getSideEnd();

        // 1
        if(!adj[{-1_pel, sideEnd - 1_pel}])
        {
            auto findAvailable =
                [=]()
                {
                    for(auto y = sideEnd - 1_pel; y >= -1_pel; --y)
                    {
                        if(adj[{-1_pel, y}])
                        {
                            return adj[{-1_pel, y}];
                        }
                    }

                    for(auto x = 0_pel; x < sideEnd; ++x)
                    {
                        if(adj[{x, -1_pel}])
                        {
                            return adj[{x, -1_pel}];
                        }
                    }

                    runtime_assert(false);
                    return IntraAdjSamples::AdjSample();
                };

            adj[{-1_pel, sideEnd - 1_pel}] = findAvailable();
        }

        // 2
        for(auto y = sideEnd - 2_pel; y >= -1_pel; --y)
        {
            if(!adj[{-1_pel, y}])
            {
                adj[{-1_pel, y}] = adj[{-1_pel, y + 1_pel}];
            }
        }

        // 3
        for(auto x = 0_pel; x < sideEnd; ++x)
        {
            if(!adj[{x, -1_pel}])
            {
                adj[{x, -1_pel}] = adj[{x - 1_pel, -1_pel}];
            }
        }
    }

    const auto toStr =
        [coord, &adj](std::ostream &oss)
        {
            oss << coord << '\n';

            adj.writeTo(
                    oss,
                    [](std::ostream &os, IntraAdjSamples::AdjSample i) {pelFmt(os, *i);});
        };

    const LogId logId[] =
    {
        LogId::IntraAdjSubstitutedSamplesY,
        LogId::IntraAdjSubstitutedSamplesCb,
        LogId::IntraAdjSubstitutedSamplesCr
    };

    log(logId[int(plane)], toStr);
    runtime_assert(adj.areAllAvailable());
    return adj;
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processses */
