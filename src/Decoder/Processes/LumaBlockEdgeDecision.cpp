#include <Decoder/Processes/LumaBlockEdgeDecision.h>
#include <Syntax/SliceSegmentHeader.h>
#include <Structure/Picture.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
namespace {
/*----------------------------------------------------------------------------*/
// 04/2013, 8.7.2.5.6 "Decision process for luma sample"
bool deriveDecision(int p0, int p3, int q0, int q3, int dpq, int beta, int tc)
{
    const auto dSam =
        dpq < beta >> 2
        && std::abs(p3 - p0) + std::abs(q0 - q3) < beta >> 3
        && std::abs(p0 - q0) < (5 * tc + 1) >> 1;

    return dSam;
}

struct Decision
{
    int dE;
    bool dEp, dEq;

    Decision(int dE_): dE(dE_), dEp(0), dEq(0)
    {}
};
/* 04/2013, 8.7.2.5.3 "Decision process for luma block edges"
 * equations: 8-293 ... 8-310 */
Decision deriveDecision(
        const std::array<Sample, 8> &r0, const std::array<Sample, 8> &r3,
        int beta, int tc)
{
    const auto p00 = r0[3];
    const auto p10 = r0[2];
    const auto p20 = r0[1];
    const auto p30 = r0[0];

    const auto p03 = r3[3];
    const auto p13 = r3[2];
    const auto p23 = r3[1];
    const auto p33 = r3[0];

    const auto q00 = r0[4];
    const auto q10 = r0[5];
    const auto q20 = r0[6];
    const auto q30 = r0[7];

    const auto q03 = r3[4];
    const auto q13 = r3[5];
    const auto q23 = r3[6];
    const auto q33 = r3[7];
    // 1
    const auto dp0 = std::abs(p20 - (p10 << 1) + p00);
    const auto dp3 = std::abs(p23 - (p13 << 1) + p03);

    const auto dq0 = std::abs(q20 - (q10 << 1) + q00);
    const auto dq3 = std::abs(q23 - (q13 << 1) + q03);

    const auto dpq0 = dp0 + dq0;
    const auto dpq3 = dp3 + dq3;
    const auto dp = dp0 + dp3;
    const auto dq = dq0 + dq3;
    const auto d = dpq0 + dpq3;

    // 2, 3e
    Decision decision(d < beta);

    // 3
    if(decision.dE)
    {
        // 3a, 3b
        const auto dSam0 = deriveDecision(p00, p30, q00, q30, 2 * dpq0, beta, tc);
        // 3c, 3d
        const auto dSam3 = deriveDecision(p03, p33, q03, q33, 2 * dpq3, beta, tc);

        // f
        if(dSam0 && dSam3)
        {
            decision.dE = 2;
        }

        // g
        decision.dEp = dp < (beta + (beta >> 1)) >> 3;
        // h
        decision.dEq = dq < (beta + (beta >> 1)) >> 3;
    }

    return decision;
}
/*----------------------------------------------------------------------------*/
}  /* namespace */
/*----------------------------------------------------------------------------*/
BlkEdgeD LumaBlockEdgeDecision::exec(
        State &,
        Ptr<const Structure::Picture> picture,
        PelCoord coord,
        EdgeType edgeType,
        Structure::CtbEdgeBS::BS bs)
{
    using namespace Syntax;

    typedef SliceSegmentHeader SSH;

    const auto bitDepthY = picture->bitDepth(Plane::Y);
    const auto qSH = picture->slice(coord)->header();
    const auto sliceBetaOffsetDiv2 = qSH->get<SSH::SliceBetaOffsetDiv2>()->inUnits();
    const auto sliceTcOffsetDiv2 = qSH->get<SSH::SliceTcOffsetDiv2>()->inUnits();

    const auto bdScale =
        [bitDepthY](int value)
        {
            return value * (1 << (bitDepthY - 8));
        };

    // 8-288
    const int qpL = bs.bits.qpL;
    // 8-289
    const auto betaPrimeQ = clip3(0, 51, qpL + (sliceBetaOffsetDiv2 << 1));
    const auto betaPrime = deriveBetaPrime(betaPrimeQ);
    // 8-290
    const auto beta = bdScale(betaPrime);
    // 8-291
    const auto tcPrimeQ = clip3(0, 53, qpL + 2 * (bs.bits.bs - 1) + (sliceTcOffsetDiv2 << 1));
    const auto tcPrime = deriveTcPrime(tcPrimeQ);
    // 8-292
    const auto tc = bdScale(tcPrime);

    const auto derive =
        [picture, coord, edgeType, betaPrime, beta, tcPrime, tc]()
        {
            using namespace Structure;

            const auto &src = picture->pelBuffer(PelLayerId::Deblocked, Plane::Y);

            if(EdgeType::V == edgeType)
            {
                const auto decision =
                    deriveDecision(
                            src.hStripe<8>(coord + PelCoord{-4_pel, 0_pel}),
                            src.hStripe<8>(coord + PelCoord{-4_pel, 3_pel}),
                            beta, tc);

                return BlkEdgeD{decision.dE, decision.dEp, decision.dEq, tcPrime, betaPrime};
            }
            else //if(EdgeType::H == edgeType)
            {
                const auto decision =
                    deriveDecision(
                            src.vStripe<8>(coord + PelCoord{0_pel, -4_pel}),
                            src.vStripe<8>(coord + PelCoord{3_pel, -4_pel}),
                            beta, tc);

                return BlkEdgeD{decision.dE, decision.dEp, decision.dEq, tcPrime, betaPrime};
            }
        };

    const auto d = derive();

    const auto toStr =
        [=](std::ostream &oss)
        {
            oss
                << coord << ' '
                << getName(edgeType)
                << " qpL " << qpL
                << " b'q " << betaPrimeQ
                << " b " << beta
                << " b' " << betaPrime
                << " tc'q "<< tcPrimeQ
                << " tc " << tc
                << " tc' " << tcPrime << ' ';

            d.toStr(oss);
            oss << '\n';
        };

    log(LogId::DeblockD, toStr);

    return d;
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */
