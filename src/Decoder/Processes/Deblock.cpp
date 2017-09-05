#include <array>

#include <Decoder/Process.h>
#include <Decoder/Processes/Deblock.h>
#include <Decoder/Processes/DeblockEdges.h>
#include <Decoder/Processes/BoundaryFilteringStrength.h>
#include <Decoder/Processes/LumaBlockEdgeDecision.h>
#include <Decoder/Processes/QuantizationParameters.h>
#include <Structure/Picture.h>
#include <Structure/PelBuffer.h>
#include <Syntax/SliceSegmentHeader.h>
#include <Syntax/CodingTreeUnit.h>
#include <Syntax/CodingUnit.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
namespace {
/*----------------------------------------------------------------------------*/
typedef Structure::CtbEdgeBS CtbEdgeBS;

template <int n>
struct PQ
{
    static_assert(0 == n % 2, "PQ: expected even number of samples.");
    std::array<Sample, n> pq;

    PQ(const std::array<Sample, n> &s): pq(s)
    {}

    PQ(std::initializer_list<int> x)
    {
        runtime_assert(x.size() == pq.size());
        std::copy(std::begin(x), std::end(x), std::begin(pq));
    }

    Sample p(int i) const
    {
        return pq[n / 2 - 1 - i];
    }

    void p(int i, Sample x)
    {
        pq[n / 2 - 1 - i] = x;
    }

    Sample q(int i) const
    {
        return pq[n / 2 + i];
    }

    void q(int i, Sample x)
    {
        pq[n / 2 + i] = x;
    }
};
/*----------------------------------------------------------------------------*/
/* Luma */
/*----------------------------------------------------------------------------*/
typedef PQ<8> LumaIn;
typedef PQ<6> LumaOut;

inline
LumaOut lumaStrongFilter(const LumaIn &s, int tc)
{
    /* 04/2013, 8.7.2.5.7 "Filtering process for a luma sample" */

    const auto r = 2 * tc;

    return
    {
        // p2' 8-332
        clip3(
                s.p(2) - r, s.p(2) + r,
                (2 * s.p(3) + 3 * s.p(2) + s.p(1) + s.p(0) + s.q(0) + 4) >> 3),
        // p1' 8-331
        clip3(
                s.p(1) - r, s.p(1) + r,
                (s.p(2) + s.p(1) + s.p(0) + s.q(0) + 2) >> 2),
        // p0' 8-330
        clip3(
                s.p(0) - r, s.p(0) + r,
                (s.p(2) + 2 * s.p(1) + 2 * s.p(0) + 2 * s.q(0) + s.q(1) + 4) >> 3),
        // q0' 8-333
        clip3(
                s.q(0) - r, s.q(0) + r,
                (s.p(1) + 2 * s.p(0) + 2 * s.q(0) + 2 * s.q(1) + s.q(2) + 4) >> 3),
        // q1' 8-334
        clip3(
                s.q(1) - r, s.q(1) + r,
                (s.p(0) + s.q(0) + s.q(1) + s.q(2) + 2) >> 2),
        // q2' 8-335
        clip3(
                s.q(2) - r, s.q(2) + r,
                (s.p(0) + s.q(0) + s.q(1) + 3 * s.q(2) + 2 * s.q(3) + 4) >> 3)
    };
}
/*----------------------------------------------------------------------------*/
inline
LumaOut lumaWeakFilter(const LumaIn &s, int bitDepth, bool dEp, bool dEq, int tc)
{
    /* 04/2013, 8.7.2.5.7 "Filtering process for a luma sample" */

    // 8-336
    const auto delta = (9 * (s.q(0) - s.p(0)) - 3 * (s.q(1) - s.p(1)) + 8) >> 4;

    LumaOut r{s.p(2), s.p(1), s.p(0), s.q(0), s.q(1), s.q(2)};

    if(10 * tc > std::abs(delta))
    {
        // 8-337
        const auto delta0 = clip3(-tc, tc, delta);
        // 8-338
        r.p(0, clip1(bitDepth, s.p(0) + delta0));
        // 8-339
        r.q(0, clip1(bitDepth, s.q(0) - delta0));

        if(dEp)
        {
            // 8-340
            const auto delta1 =
                clip3(
                        -(tc >> 1), tc >> 1,
                        (((s.p(2) + s.p(0) + 1) >> 1) - s.p(1) + delta0) >>  1);

            // 8-341
            r.p(1, clip1(bitDepth, s.p(1) + delta1));
        }

        if(dEq)
        {
            // 8-342
            const auto delta1 =
                clip3(
                        -(tc >> 1), tc >> 1,
                        (((s.q(2) + s.q(0) + 1) >> 1) - s.q(1) - delta0) >> 1);
            // 8-343
            r.q(1, clip1(bitDepth, s.q(1) + delta1));
        }
    }

    return r;
}
/*----------------------------------------------------------------------------*/
inline
LumaOut lumaFilter(const LumaIn &s, int bitDepth, int dE, bool dEp, bool dEq, int tc)
{
    return
        2 == dE
        ? lumaStrongFilter(s, tc)
        : lumaWeakFilter(s, bitDepth, dEp, dEq, tc);
}
/*----------------------------------------------------------------------------*/
inline
void vFilterLumaBlk(
        Structure::PelBuffer &buf,
        PelCoord base,
        int bitDepthY,
        int dE, bool dEp, bool dEq, int tc,
        bool pBypass, bool qBypass)
{
    if(0 == dE)
    {
        return;
    }
    // 2
    for(auto y = 0; y < 4; ++y)
    {
        const PelCoord at{base.x() - 4_pel, base.y() + Pel{y}};
        const auto in = LumaIn{buf.hStripe<8>(at)};
        auto out = lumaFilter(in, bitDepthY, dE, dEp, dEq, tc);

        for(auto i = 0; pBypass && i < 3; ++i)
        {
            out.p(i, in.p(i));
        }

        for(auto i = 0; qBypass && i < 3; ++i)
        {
            out.q(i, in.q(i));
        }

        buf.hStripe({at.x() + 1_pel, at.y()}, out.pq);
    }
}
/*----------------------------------------------------------------------------*/
inline
void hFilterLumaBlk(
        Structure::PelBuffer &buf,
        PelCoord base,
        int bitDepthY,
        int dE, bool dEp, bool dEq, int tc,
        bool pBypass, bool qBypass)
{
    if(0 == dE)
    {
        return;
    }
    // 2
    for(auto x = 0; x < 4; ++x)
    {
        const PelCoord at{base.x() + Pel{x}, base.y() - 4_pel};
        const auto in = LumaIn{buf.vStripe<8>(at)};
        auto out = lumaFilter(in, bitDepthY, dE, dEp, dEq, tc);

        for(auto i = 0; pBypass && i < 3; ++i)
        {
            out.p(i, in.p(i));
        }

        for(auto i = 0; qBypass && i < 3; ++i)
        {
            out.q(i, in.q(i));
        }

        buf.vStripe({at.x(), at.y() + 1_pel}, out.pq);
    }
}
/*----------------------------------------------------------------------------*/
void vFilterLumaCU(
        State &decoder,
        Ptr<Structure::Picture> picture,
        Structure::PelBuffer &buf,
        PelCoord base, Log2 size,
        const CtbEdgeBS &ctbEdgeBS)
{
    const auto bitDepthY = picture->bitDepth(Plane::Y);
    const auto nD = toInt(size - 3_log2);

    for(auto k = 0; k < nD; ++k)
    {
        for(auto m = 0; m < (nD << 1); ++m)
        {
            const auto xDk = k << 3;
            const auto yDm = m << 2;
            const PelCoord offset{Pel{xDk}, Pel{yDm}};
            const PelCoord at{base + offset};
            const CtbEdgeBS::BS bs = ctbEdgeBS[{k, m}];

            if(0 < bs.bs() && !bs.pqBypass())
            {
                // call 8.7.2.5.3
                const auto d =
                    subprocess(
                            decoder, LumaBlockEdgeDecision(),
                            picture, at, EdgeType::V, bs);
                // call 8.7.2.5.4
                vFilterLumaBlk(
                        buf,
                        at,
                        bitDepthY,
                        d.dE(), d.dEp(), d.dEq(), d.tc(bitDepthY),
                        bs.pBypass(), bs.qBypass());
            }
        }
    }
}
/*----------------------------------------------------------------------------*/
void hFilterLumaCU(
        State &decoder,
        Ptr<Structure::Picture> picture,
        Structure::PelBuffer &buf,
        PelCoord base, Log2 size,
        const CtbEdgeBS &ctbEdgeBS)
{
    const auto bitDepthY = picture->bitDepth(Plane::Y);
    const auto nD = toInt(size - 3_log2);

    for(auto m = 0; m < nD; ++m)
    {
        for(auto k = 0; k < (nD << 1); ++k)
        {
            const auto xDk = k << 2;
            const auto yDm = m << 3;
            const PelCoord offset{Pel{xDk}, Pel{yDm}};
            const PelCoord at{base + offset};
            const CtbEdgeBS::BS bs = ctbEdgeBS[{k, m}];

            if(0 < bs.bs() && !bs.pqBypass())
            {
                // call 8.7.2.5.3
                const auto d =
                    subprocess(decoder, LumaBlockEdgeDecision(),
                            picture, at, EdgeType::H, bs);

                // call 8.7.2.5.4
                hFilterLumaBlk(
                        buf,
                        at,
                        bitDepthY,
                        d.dE(), d.dEp(), d.dEq(), d.tc(bitDepthY),
                        bs.pBypass(), bs.qBypass());
            }
        }
    }
}
/*----------------------------------------------------------------------------*/
/* Chroma */
/*----------------------------------------------------------------------------*/
typedef PQ<4> ChromaIn;
typedef PQ<2> ChromaOut;

inline
ChromaOut chromaFilter(const ChromaIn &s, int bitDepth, int tc)
{
    /* 04/2013, 8.7.2.5.8 "Filtering process for a chroma sample" */

    // 8-344
    const auto delta =
        clip3(
                -tc, tc,
                (((s.q(0) - s.p(0)) << 2) + s.p(1) - s.q(1) + 4) >> 3);

    return
    {
        // 8-345
        clip1(bitDepth, s.p(0) + delta),
        // 8-346
        clip1(bitDepth, s.q(0) - delta)
    };
}
/*----------------------------------------------------------------------------*/
inline
void vFilterChromaBlk(
        Structure::PelBuffer &buf,
        PelCoord base,
        int bitDepthC,
        int tc,
        bool pBypass, bool qBypass)
{
    for(auto y = 0; y < 4; ++y)
    {
        const PelCoord at{base.x() - 2_pel, base.y() + Pel{y}};
        const auto in = ChromaIn{buf.hStripe<4>(at)};
        auto out = chromaFilter(in, bitDepthC, tc);

        if(pBypass)
        {
            out.p(0, in.p(0));
        }

        if(qBypass)
        {
            out.q(0, in.q(0));
        }

        buf.hStripe({at.x() + 1_pel, at.y()}, out.pq);
    }
}
/*----------------------------------------------------------------------------*/
inline
void hFilterChromaBlk(
        Structure::PelBuffer &buf,
        PelCoord base,
        int bitDepthC,
        int tc,
        bool pBypass, bool qBypass)
{
    for(auto x = 0; x < 4; ++x)
    {
        const PelCoord at{base.x() + Pel{x}, base.y() - 2_pel};
        const auto in = ChromaIn{buf.vStripe<4>(at)};
        auto out = chromaFilter(in, bitDepthC, tc);

        if(pBypass)
        {
            out.p(0, in.p(0));
        }

        if(qBypass)
        {
            out.q(0, in.q(0));
        }

        buf.vStripe({at.x(), at.y() + 1_pel}, out.pq);
    }
}
/*----------------------------------------------------------------------------*/
/* at - is expected to be expressed in Luma */
int deriveChromaTc(
        ChromaFormatIdc chromaFormatIdc,
        int bitDepthC,
        int sliceTcOffsetDiv2,
        int cQpPicOffset,
        int qpL)
{
    const auto qPi = qpL + cQpPicOffset;
    const auto qPc = qPiToQpC(qPi, chromaFormatIdc);
    const auto tcPrimeQ = clip3(0, 53, qPc + 2 + (sliceTcOffsetDiv2 << 1));
    const auto tcPrime = deriveTcPrime(tcPrimeQ);
    const auto tc = tcPrime * (1 << (bitDepthC - 8));

    return tc;
}
/*----------------------------------------------------------------------------*/
void vFilterChromaCU(
    Ptr<Structure::Picture> picture,
    Structure::PelBuffer &bufCb, Structure::PelBuffer &bufCr,
    PelCoord base, Log2 size,
    const CtbEdgeBS &ctbEdgeBS)
{
    using namespace Syntax;

    typedef SliceSegmentHeader SSH;

    const auto chromaFormatIdc = picture->chromaFormatIdc;
    const auto bitDepthC = picture->bitDepth(Component::Chroma);
    const auto cbQpOffset = picture->getQpOffset(Chroma::Cb);
    const auto crQpOffset = picture->getQpOffset(Chroma::Cr);
    const auto sh = picture->slice(base)->header();
    const auto tcOffsetDiv2 = sh->get<SSH::SliceTcOffsetDiv2>()->inUnits();
    const auto baseInPlane = scale(base, Component::Chroma, chromaFormatIdc);
    const auto hnD = toPel(hScale(size - 3_log2, Component::Chroma, chromaFormatIdc));
    const auto vnD = toPel(vScale(size - 2_log2, Component::Chroma, chromaFormatIdc));
    const auto hInvScale =
        [chromaFormatIdc](Pel i)
        {
            return toUnderlying(HEVC::hInvScale(i, Component::Chroma, chromaFormatIdc));
        };
    const auto vInvScale =
        [chromaFormatIdc](Pel i)
        {
            return toUnderlying(HEVC::vInvScale(i, Component::Chroma, chromaFormatIdc));
        };

    for(auto k = 0_pel; k < hnD; ++k)
    {
        for(auto m = 0_pel; m < vnD; ++m)
        {
            const auto xDk = k << 3;
            const auto yDm = m << 2;
            const PelCoord offset{xDk, yDm};
            const PelCoord at{baseInPlane + offset};
            const CtbEdgeBS::BS bs = ctbEdgeBS[{hInvScale(k), vInvScale(m)}];

            if(2 == bs.bs() && !bs.pqBypass())
            {
                // call 8.7.2.5.5
                const auto tcCb =
                    deriveChromaTc(chromaFormatIdc, bitDepthC, tcOffsetDiv2, cbQpOffset, bs.bits.qpL);

                vFilterChromaBlk(
                        bufCb, at, bitDepthC, tcCb,
                        bs.pBypass(), bs.qBypass());

                const auto tcCr =
                    deriveChromaTc(chromaFormatIdc, bitDepthC, tcOffsetDiv2, crQpOffset, bs.bits.qpL);

                vFilterChromaBlk(
                        bufCr, at, bitDepthC, tcCr,
                        bs.pBypass(), bs.qBypass());
            }
        }
    }
}
/*----------------------------------------------------------------------------*/
void hFilterChromaCU(
    Ptr<Structure::Picture> picture,
    Structure::PelBuffer &bufCb, Structure::PelBuffer &bufCr,
    PelCoord base, Log2 size,
    const CtbEdgeBS &ctbEdgeBS)
{
    using namespace Syntax;

    typedef SliceSegmentHeader SSH;

    const auto chromaFormatIdc = picture->chromaFormatIdc;
    const auto bitDepthC = picture->bitDepth(Component::Chroma);
    const auto cbQpOffset = picture->getQpOffset(Chroma::Cb);
    const auto crQpOffset = picture->getQpOffset(Chroma::Cr);
    const auto sh = picture->slice(base)->header();
    const auto tcOffsetDiv2 = sh->get<SSH::SliceTcOffsetDiv2>()->inUnits();
    const auto baseInPlane = scale(base, Component::Chroma, chromaFormatIdc);
    const auto hnD = toPel(hScale(size - 2_log2, Component::Chroma, chromaFormatIdc));
    const auto vnD = toPel(vScale(size - 3_log2, Component::Chroma, chromaFormatIdc));
    const auto hInvScale =
        [chromaFormatIdc](Pel i)
        {
            return toUnderlying(HEVC::hInvScale(i, Component::Chroma, chromaFormatIdc));
        };
    const auto vInvScale =
        [chromaFormatIdc](Pel i)
        {
            return toUnderlying(HEVC::vInvScale(i, Component::Chroma, chromaFormatIdc));
        };

    for(auto m = 0_pel; m < vnD; ++m)
    {
        for(auto k = 0_pel; k < hnD; ++k)
        {
            const auto xDk = k << 2;
            const auto yDm = m << 3;
            const PelCoord offset{xDk, yDm};
            const PelCoord at{baseInPlane + offset};
            const CtbEdgeBS::BS bs = ctbEdgeBS[{hInvScale(k), vInvScale(m)}];

            if(2 == bs.bs() && !bs.pqBypass())
            {
                // call 8.7.2.5.5
                const auto tcCb =
                    deriveChromaTc(chromaFormatIdc, bitDepthC, tcOffsetDiv2, cbQpOffset, bs.bits.qpL);

                hFilterChromaBlk(
                        bufCb, at, bitDepthC, tcCb,
                        bs.pBypass(), bs.qBypass());

                const auto tcCr =
                    deriveChromaTc(chromaFormatIdc, bitDepthC, tcOffsetDiv2, crQpOffset, bs.bits.qpL);

                hFilterChromaBlk(
                        bufCr, at, bitDepthC, tcCr,
                        bs.pBypass(), bs.qBypass());
            }
        }
    }
}
/*----------------------------------------------------------------------------*/
/* Common Filtering */
/*----------------------------------------------------------------------------*/
void vFilterCU(
        State &decoder,
        Ptr<Structure::Picture> picture,
        PelCoord base, Log2 size,
        const CtbEdgeBS &bs)
{
    auto &bufY = picture->pelBuffer(PelLayerId::Deblocked, Plane::Y);

    vFilterLumaCU(decoder, picture, bufY, base, size, bs);

    if(ChromaFormatIdc::f400 != picture->chromaFormatIdc)
    {
        auto &bufCb = picture->pelBuffer(PelLayerId::Deblocked, Plane::Cb);
        auto &bufCr = picture->pelBuffer(PelLayerId::Deblocked, Plane::Cr);

        vFilterChromaCU(picture, bufCb, bufCr, base, size, bs);
    }
}

void hFilterCU(
        State &decoder,
        Ptr<Structure::Picture> picture,
        PelCoord base, Log2 size,
        const CtbEdgeBS &bs)
{
    auto &bufY = picture->pelBuffer(PelLayerId::Deblocked, Plane::Y);

    hFilterLumaCU(decoder, picture, bufY, base, size, bs);

    if(ChromaFormatIdc::f400 != picture->chromaFormatIdc)
    {
        auto &bufCb = picture->pelBuffer(PelLayerId::Deblocked, Plane::Cb);
        auto &bufCr = picture->pelBuffer(PelLayerId::Deblocked, Plane::Cr);

        hFilterChromaCU(picture, bufCb, bufCr, base, size, bs);
    }
}
/*----------------------------------------------------------------------------*/
void copy(
        Structure::Picture &picture,
        Plane plane,
        PelCoord base, Pel hSide, Pel vSide)
{
    const auto &src = picture.pelBuffer(PelLayerId::Reconstructed, plane);
    auto &dst = picture.pelBuffer(PelLayerId::Deblocked, plane);
    const auto bdry = picture.boundaries(plane);

    for(auto y = 0_pel; y < vSide; ++y)
    {
        for(auto x = 0_pel; x < hSide; ++x)
        {
            const PelCoord at{base + PelCoord{x, y}};

            if(bdry.encloses(at))
            {
                dst[at] = src[at];
            }
        }
    }
}
/*----------------------------------------------------------------------------*/
} /* namespace */
/*----------------------------------------------------------------------------*/
void Deblock::exec(
        State &decoder,
        Ptr<Structure::Picture> picture,
        const Syntax::CodingTreeUnit &ctu,
        EdgeType edgeType)
{
    using namespace Syntax;

    typedef SliceSegmentHeader SSH;
    typedef CodingTreeUnit CTU;
    typedef CodingUnit CU;

    const auto chromaFormatIdc = picture->chromaFormatIdc;
    const auto ctuCoord = ctu.get<CTU::Coord>()->inUnits();
    const auto ctuSize = picture->ctbSizeY;
    const auto sh = picture->slice(ctuCoord)->header();
    const auto sliceDeblockingFilterDisabledFlag =
        bool(*sh->get<SSH::SliceDeblockingFilterDisabledFlag>());

    if(isV(edgeType))
    {
        for(auto plane : EnumRange<Plane>())
        {
            if(!isPresent(plane, chromaFormatIdc))
            {
                continue;
            }

            copy(
                    *picture,
                    plane,
                    scale(ctuCoord, plane, chromaFormatIdc),
                    hScale(toPel(ctuSize), plane, chromaFormatIdc),
                    vScale(toPel(ctuSize), plane, chromaFormatIdc));
        }
    }

    if(!sliceDeblockingFilterDisabledFlag)
    {
        const auto edges =
            subprocess(decoder, PredTransEdges(), picture, ctu, edgeType);
        const auto bss =
            subprocess(decoder, BoundaryFilteringStrength(), picture, ctu, edges, edgeType);

        if(isV(edgeType))
        {
            vFilterCU(decoder, picture, ctuCoord, ctuSize, bss);
        }
        else //if(isH(edgeType))
        {
            hFilterCU(decoder, picture, ctuCoord, ctuSize, bss);
        }
    }
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processess */
