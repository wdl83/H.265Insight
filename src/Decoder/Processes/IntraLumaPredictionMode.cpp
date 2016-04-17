#include <Decoder/Processes/IntraLumaPredictionMode.h>
#include <Decoder/State.h>
#include <Structure/Picture.h>
#include <Syntax/CodingUnit.h>
/* STDC++ */
#include <array>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
namespace {
/*----------------------------------------------------------------------------*/
typedef std::array<IntraPredictionMode, 3> CandModeList;
/*----------------------------------------------------------------------------*/
IntraPredictionMode deriveCandIntraPredMode(
        const Structure::Picture &picture,
        PelCoord curr, PelCoord adj, int adjNo)
{
    using namespace Syntax;

    typedef CodingUnit CU;

    // 2
    const auto available = picture.isAvailableInScanZ(curr, adj);

    if(!available)
    {
        return IntraPredictionMode::Dc;
    }
    else
    {
        const auto adjCu = picture.getCodingUnit(adj);
        const auto adjCuPredMode = adjCu->get<CU::CuPredMode>();

        if(
                PredictionMode::Intra != *adjCuPredMode
                || *adjCu->get<CU::PcmFlag>())
        {
            return IntraPredictionMode::Dc;
        }
        else
        {
            const bool above = curr.y() - 1_pel == adj.y();
            const auto currCtbAligned = picture.toPel(picture.toCtb(curr).y());
            const bool adjInAboveCtb = adj.y() < currCtbAligned;

            if(above && adjInAboveCtb)
            {
                return IntraPredictionMode::Dc;
            }
            else
            {
                return (*adjCu->get<CU::IntraPredModeY>())[adjNo];
            }
        }
    }
}
/*----------------------------------------------------------------------------*/
CandModeList deriveCandModeList(
        IntraPredictionMode left,
        IntraPredictionMode above)
{
    // 3
    if(left == above)
    {
        if(
                IntraPredictionMode::Planar == left
                || IntraPredictionMode::Dc == left)
        {
            return {
                {
                    IntraPredictionMode::Planar,
                    IntraPredictionMode::Dc,
                    IntraPredictionMode::Angular26
                }};
        }
        else
        {
            return {
                {
                    left,
                    static_cast<IntraPredictionMode>(2 + ((castToUnderlying(left) + 29) % 32)),
                    static_cast<IntraPredictionMode>(2 + ((castToUnderlying(left) - 2 + 1) % 32))
                }};
        }
    }
    else
    {
        const auto last =
            IntraPredictionMode::Planar != left
            && IntraPredictionMode::Planar != above
            ? IntraPredictionMode::Planar
            : (
                    IntraPredictionMode::Dc != left
                    && IntraPredictionMode::Dc != above
                    ? IntraPredictionMode::Dc
                    : IntraPredictionMode::Angular26);

        return
        {
            {left, above, last}
        };
    }
}
/*----------------------------------------------------------------------------*/
IntraPredictionMode deriveIntraPredictionMode(
        const Structure::Picture &,
        const Syntax::CodingUnit &cu,
        PelCoord offset,
        CandModeList candModeList)
{
    // 4
    using namespace Syntax;

    typedef CodingUnit CU;

    const auto cuSize = cu.get<CU::Size>()->inUnits();
    const auto prevIntraLumaPredFlag = cu.get<CU::PrevIntraLumaPredFlag>();

    if((*prevIntraLumaPredFlag)[makeTuple(offset, cuSize)])
    {
        const auto mpmIdx = cu.get<CU::MpmIdx>();

        runtime_assert(int(candModeList.size()) > (*mpmIdx)[makeTuple(offset, cuSize)]);
        return candModeList[(*mpmIdx)[makeTuple(offset, cuSize)]];
    }
    else
    {
        // 1)
        {

            // i
            if(candModeList[0] > candModeList[1])
            {
                std::swap(candModeList[0], candModeList[1]);
            }
            // ii
            if(candModeList[0] > candModeList[2])
            {
                std::swap(candModeList[0], candModeList[2]);
            }
            // iii
            if(candModeList[1] > candModeList[2])
            {
                std::swap(candModeList[1], candModeList[2]);
            }
        }

        // 2)
        {
            // i
            const auto remIntraLumaPredMode = cu.get<CU::RemIntraLumaPredMode>();
            auto intraPredictionMode = (*remIntraLumaPredMode)[makeTuple(offset, cuSize)];
            // ii
            for(const auto i : candModeList)
            {
                if(intraPredictionMode >= i)
                {
                    runtime_assert(
                            castToUnderlying(IntraPredictionMode::End)
                            > castToUnderlying(intraPredictionMode) + 1);

                    intraPredictionMode =
                        static_cast<IntraPredictionMode>(
                                castToUnderlying(intraPredictionMode) + 1);
                }
            }

            return intraPredictionMode;
        }
    }
}
/*----------------------------------------------------------------------------*/
} /* namespace */
/*----------------------------------------------------------------------------*/
CuIntraPredMode IntraLumaPredictionMode::exec(
        State &,
        Ptr<const Structure::Picture> picture,
        const Syntax::CodingUnit &cu)
{
    /* 04/2013, 8.4.2, "Derivation process for luma intra prediction mode" */
    using namespace Syntax;

    typedef CodingUnit CU;

    // 1
    const auto coord = cu.get<CU::Coord>()->inUnits();
    const auto size = cu.get<CU::Size>()->inUnits();
    const PartitionMode partMode = *cu.get<CU::PartModePseudo>();

    runtime_assert(
            PartitionMode::PART_2Nx2N == partMode
            || PartitionMode::PART_NxN == partMode);

    const auto partNum = partMode == PartitionMode::PART_2Nx2N ? 1 : 4;
    CuIntraPredMode mode;

    for(auto i = 0; i < partNum; ++i)
    {
        const PelCoord offset
        {
            0 == i % 2 ? 0_pel : toPel(size - 1_log2),
              0 == i / 2 ? 0_pel : toPel(size - 1_log2)
        };


        const PelCoord left{offset.x() - 1_pel, offset.y()};
        const PelCoord above{offset.x(), offset.y() - 1_pel};

        const auto deriveLeft =
            [picture, coord, offset, left, i, &mode]()
            {
                return
                    0 != i % 2
                    ? mode[i - 1]
                    : deriveCandIntraPredMode(*picture, coord + offset, coord + left, i + 1);
            };

        const auto deriveAbove =
            [picture, coord, offset, above, i, &mode]()
            {
                return
                    1 < i
                    ? mode[i - 2]
                    : deriveCandIntraPredMode(*picture, coord + offset, coord + above, i + 2);
            };

        // call(2..4)
        mode[i] =
            deriveIntraPredictionMode(
                    *picture, cu, offset, deriveCandModeList(deriveLeft(), deriveAbove()));

        log(LogId::Prediction, "Luma ", coord, ' ', getName(mode[i]), '\n');
    }

    return 1 == partNum ? CuIntraPredMode{mode[0]}: mode;
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processses */
