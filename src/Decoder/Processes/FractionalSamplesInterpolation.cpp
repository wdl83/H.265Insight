#include <Decoder/Processes/FractionalSamplesInterpolation.h>
#include <Decoder/State.h>
#include <Syntax/PredictionUnit.h>
#include <Structure/Picture.h>
#include <Structure/PelBuffer.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
namespace {
/*----------------------------------------------------------------------------*/
typedef FractionalSamplesInterpolation::BlkLuma BlkLuma;
typedef FractionalSamplesInterpolation::BlkChroma BlkChroma;

template <size_t n>
using Input = std::array<int, n>;

template <size_t m, size_t n>
using Coeff = std::array<std::array<int8_t, n>, m>;

template <typename Blk>
struct FilterTraits;

template <>
struct FilterTraits<BlkLuma>
{
    typedef BlkLuma BlkType;
    typedef Input<8> InputType;
    typedef Coeff<3, 8> CoeffType;
    static const CoeffType coeffs;
};

/* horizontal & vertical filter coefficients:
 * a, b, c (8-199, 8-200, 8-201)
 * d, h, n (8-202, 8-203, 8-204)
 * e, i, p (8-205, 8-206, 8-207)
 * f, j, q (8-208, 8-209, 8-210)
 * g, k, r (8-211, 8-212, 8-213) */
const FilterTraits<BlkLuma>::CoeffType FilterTraits<BlkLuma>::coeffs
{{
    // A[-3, -2, -1, 0, 1, 2, 3, 4],j ||  Ai,[-3, -2, -1, 0, 1, 2, 3, 4]
    {{-1, 4, -10, 58, 17, -5, 1, 0}},
    {{-1, 4, -11, 40, 40, -11, 4, -1}},
    {{0, 1, -5, 17, 58, -10, 4, -1}}
}};

template <>
struct FilterTraits<BlkChroma>
{
    typedef BlkChroma BlkType;
    typedef Input<4> InputType;
    typedef Coeff<7, 4> CoeffType;
    static const CoeffType coeffs;
};

/* horizontal & vertical filter coefficients:
 * ab, ac, ad, ae, af, ag, ah (8-216, 8-217, 8-218, 8-219, 8-220, 8-221, 8-222)
 * ba, ca, da, ea, fa, ga, ha (8-223, 8-224, 8-225, 8-226, 8-227, 8-228, 8-229)
 * bX, cX, dX, eX, fX, gX, hX (8-230, 8-231, 8-232, 8-233, 8-234, 8-235, 8-236) */
const FilterTraits<BlkChroma>::CoeffType FilterTraits<BlkChroma>::coeffs
{{
    // B[-1, 0, 1, 2],j || Bi,[-1, 0, 1, 2]
    {{-2, 58, 10, -2}},
    {{-4, 54, 16, -2}},
    {{-6, 46, 28, -4}},
    {{-4, 36, 36, -4}},
    {{-4, 28, 46, -6}},
    {{-2, 16, 54, -4}},
    {{-2, 10, 58, -2}}
}};

/* multiply accumulate */
template <size_t n>
inline
int mac(const std::array<int8_t, n> &coeff, const std::array<int, n> &x)
{
    int sum = 0;

    for(auto i = 0; i < int(n); ++i)
    {
        sum += coeff[i] * x[i];
    }

    return sum;
}
/*----------------------------------------------------------------------------*/
typedef std::function<Sample (PelCoord)> RefSample;

template <typename Blk>
void interpolateH(
        Plane plane,
        int bitDepth,
        SubPelCoord offset, Pel width, Pel height,
        const Blk &src, Blk &dst)
{
    typedef typename FilterTraits<Blk>::InputType InputType;

    if(0_sub_pel == offset.x()) return;

    const auto shift1 = std::min(4, bitDepth - 8);

    // horizontal interpolation (a, b, c)
    for(auto y = -Blk::topOffset; y < height + Blk::bottomOffset; ++y)
    {
        auto xPos = -Blk::leftOffset;
        for(auto x = 0_pel; x < width; ++x, ++xPos)
        {
            InputType in;

            for(auto i = 0; i < int(in.size()); ++i)
            {
                in[i] = src[{xPos + Pel{i}, y}];
            }

            dst[{x, y}] =
                mac(FilterTraits<Blk>::coeffs[toUnderlying(offset.x()) - 1], in) >> shift1;
        }
    }

    const LogId logName[] =
    {
        LogId::InterFractionalSamplesInterpolationY,
        LogId::InterFractionalSamplesInterpolationCb,
        LogId::InterFractionalSamplesInterpolationCr
    };

    log(
        logName[int(plane)],
        [&](std::ostream &oss)
        {
            oss << "H offset " << offset
            << " width " << width << " height " << height << '\n';
            dst.toStr(oss);
        });
}

template <typename Blk>
void interpolateV(
        Plane plane,
        int bitDepth,
        SubPelCoord offset, Pel width, Pel height,
        const Blk &src, Blk &dst)
{
    typedef typename FilterTraits<Blk>::InputType InputType;

    if(0_sub_pel == offset.y()) return;

    const auto shift1 = std::min(4, bitDepth - 8);
    const auto shift2 = 6;
    const auto shift = 0_sub_pel == offset.x() ? shift1 : shift2;

    // vertical interpolation (d, h, n, e, i, p, f, j, q, g, k, r)
    auto yPos = -Blk::topOffset;
    for(auto y = 0_pel; y < height; ++y, ++yPos)
    {
        for(auto x = 0_pel; x < width; ++x)
        {
            InputType in;

            for(auto i = 0; i < int(in.size()); ++i)
            {
                in[i] = src[{x, yPos + Pel{i}}];
            }

            dst[{x, y}] =
                mac(FilterTraits<Blk>::coeffs[toUnderlying(offset.y()) - 1], in) >> shift;
        }
    }

    const LogId logName[] =
    {
        LogId::InterFractionalSamplesInterpolationY,
        LogId::InterFractionalSamplesInterpolationCb,
        LogId::InterFractionalSamplesInterpolationCr
    };

    log(
       logName[int(plane)],
       [&](std::ostream &oss)
       {
           oss << "V offset " << offset
           << " width " << width << " height " << height << '\n';
           dst.toStr(oss);
       });
}
/*----------------------------------------------------------------------------*/
PredSampleLx deriveLuma(
        State &,
        Ptr<Structure::Picture> picture,
        Ptr<const Structure::Picture> ref,
        const Syntax::PredictionUnit &pu,
        Range<Pel> h, Range<Pel> v,
        RefList l)
{
    using namespace Syntax;

    typedef PredictionUnit PU;

    const auto bitDepth = picture->bitDepth(Component::Luma);
    const auto coord = pu.get<PU::Coord>()->inUnits();
    const auto width = h.length();
    const auto height = v.length();
    const auto mv = (*pu.get<PU::MvLX>())[l];
    const auto &src = ref->pelBuffer(PelLayerId::Decoded, Plane::Y);
    const PelCoord refCoord = coord + toPel(mv, 2_log2);
    /* in 1/4 sample unit */
    const auto ratio = BlkLuma::ratio;
    const auto xFracL = fraction(mv.x(), ratio);
    const auto yFracL = fraction(mv.y(), ratio);
    const auto pictureBdry = picture->boundaries();
    const auto clip =
        [pictureBdry](PelCoord pos)
        {
            return ::clip(pictureBdry, pos);
        };

    PredSampleLx dst(width, height);

    BlkLuma blk0, blk1;
    auto *srcBlk = &blk0;
    auto *dstBlk = &blk1;

    /* fill block with reference samples
     * (which are located at integer sample positions) */
    for(auto y = -3_pel; y < height + 5_pel; ++y)
    {
        for(auto x = -3_pel; x < width + 4_pel; ++x)
        {
            const PelCoord offset{x, y};

            (*srcBlk)[offset] = src[clip(refCoord + offset + PelCoord{h.begin(), v.begin()})];
        }
    }

    log(
        LogId::InterFractionalSamplesInterpolationY,
        [&](std::ostream &oss)
        {
            oss
                << "coord " << coord << coord + PelCoord{h.begin(), v.begin()}
                << " L" << int(l)
                << " mv" << mv
                << " ref["
                << "POC " << ref->order.get<PicOrderCntVal>()
                << " mv" << toPel(mv, 2_log2)
                << " frac(" << xFracL << ", " << yFracL << ")]\n";
            srcBlk->toStr(oss);
        });

    interpolateH(Plane::Y, bitDepth, {xFracL, yFracL}, width, height, *srcBlk, *dstBlk);

    if(0_sub_pel < yFracL && 0_sub_pel < xFracL)
    {
            std::swap(srcBlk, dstBlk);
            dstBlk->clear();
    }

    interpolateV(Plane::Y, bitDepth, {xFracL, yFracL}, width, height, *srcBlk, *dstBlk);

    for(auto y = 0_pel; y < height; ++y)
    {
        for(auto x = 0_pel; x < width; ++x)
        {
            dst[{x, y}] = (*dstBlk)[{x, y}];
        }
    }

    log(
        LogId::InterPredictedSamplesY,
        [&](std::ostream &oss)
        {
            oss
                << "coord " << coord << ' ' << coord + PelCoord(h.begin(), v.begin())
                << " L" << int(l)
                << " mv" << mv
                << " ref["
                << "POC " << ref->order.get<PicOrderCntVal>()
                << " mv" << toPel(mv, 2_log2)
                << " frac(" << xFracL << ", " << yFracL << ")]\n";

            for(auto y = 0_pel; y < height; ++y)
            {
                for(auto x = 0_pel; x < width; ++x)
                {
                    pelFmt(oss, dst[PelCoord{x, y}]);
                    oss << (width - 1_pel == x ? '\n' : ' ');
                }
            }
        });

    return dst;
}
/*----------------------------------------------------------------------------*/
PredSampleLx deriveChroma(
        State &,
        Ptr<Structure::Picture> picture,
        Ptr<const Structure::Picture> ref,
        const Syntax::PredictionUnit &pu,
        Range<Pel> h, Range<Pel> v,
        RefList l,
        Chroma chroma)
{
    using namespace Syntax;

    typedef PredictionUnit PU;

    const auto bitDepth = picture->bitDepth(Component::Chroma);
    const auto chromaFormatIdc = picture->chromaFormatIdc;
    const auto puCoord = pu.get<PU::Coord>()->inUnits();
    const auto coord = scale(puCoord, Component::Chroma, chromaFormatIdc);
    const auto width = h.length();
    const auto height = v.length();
    const auto mv = (*pu.get<PU::MvCLX>())[l];
    const auto &src = ref->pelBuffer(PelLayerId::Decoded, toPlane(chroma));
    const PelCoord refCoord = coord + toPel(mv, 3_log2);
    /* in 1/8 sample unit */
    const auto ratio = BlkChroma::ratio;
    const auto xFracL = fraction(mv.x(), ratio);
    const auto yFracL = fraction(mv.y(), ratio);
    const auto pictureBdry = picture->boundaries(toPlane(chroma));
    const auto clip =
        [pictureBdry](PelCoord pos)
        {
            return ::clip(pictureBdry, pos);
        };

    PredSampleLx dst(width, height);

    BlkChroma blk0, blk1;
    auto srcBlk = &blk0;
    auto dstBlk = &blk1;

    /* fill block with reference samples
     * (which are located at integer sample positions) */
    for(auto y = -1_pel; y < height + 3_pel; ++y)
    {
        for(auto x = -1_pel; x < width + 3_pel; ++x)
        {
            const PelCoord offset{x, y};

            (*srcBlk)[offset] = src[clip(refCoord + offset + PelCoord{h.begin(), v.begin()})];
        }
    }

    {
        const LogId logName[] =
        {
            LogId::InterFractionalSamplesInterpolationCb,
            LogId::InterFractionalSamplesInterpolationCr
        };

        log(
            logName[int(chroma)],
            [&](std::ostream &oss)
            {
                oss << "coord " << coord << coord + PelCoord{h.begin(), v.begin()}
                    << " L" << int(l)
                    << " mv" << mv
                    << " ref["
                    << "POC " << ref->order.get<PicOrderCntVal>()
                    << " mv" << toPel(mv, 3_log2)
                    << " frac(" << xFracL << ", " << yFracL << ")]\n";
                srcBlk->toStr(oss);
            });
    }

    interpolateH(toPlane(chroma), bitDepth, {xFracL, yFracL}, width, height, *srcBlk, *dstBlk);

    if(0_sub_pel < yFracL && 0_sub_pel < xFracL)
    {
        std::swap(srcBlk, dstBlk);
        dstBlk->clear();
    }

    interpolateV(toPlane(chroma), bitDepth, {xFracL, yFracL}, width, height, *srcBlk, *dstBlk);

    for(auto y = 0_pel; y < height; ++y)
    {
        for(auto x = 0_pel; x < width; ++x)
        {
            dst[{x, y}] = (*dstBlk)[{x, y}];
        }
    }

    const auto toStr =
        [&](std::ostream &oss)
        {
            oss
                << "coord " << coord << ' ' << coord + PelCoord(h.begin(), v.begin())
                << " L" << int(l)
                << " mv" << mv
                << " ref["
                << "POC " << ref->order.get<PicOrderCntVal>()
                << " mv" << toPel(mv, 3_log2)
                << " frac(" << xFracL << ", " << yFracL << ")]\n";

            for(auto y = 0_pel; y < height; ++y)
            {
                for(auto x = 0_pel; x < width; ++x)
                {
                    pelFmt(oss, dst[PelCoord{x, y}]);
                    oss << (width - 1_pel == x ? '\n' : ' ');
                }
            }
        };

    const LogId logName[] =
    {
        LogId::InterPredictedSamplesCb,
        LogId::InterPredictedSamplesCr
    };

    log(logName[int(chroma)], toStr);

    return dst;
}
/*----------------------------------------------------------------------------*/
} // namespace
/*----------------------------------------------------------------------------*/
PredSampleLx FractionalSamplesInterpolation::exec(
        State &decoder,
        Ptr<Structure::Picture> picture,
        Ptr<const Structure::Picture> ref,
        const Syntax::PredictionUnit &pu,
        Plane plane, Range<Pel> h, Range<Pel> v,
        RefList l)
{
    runtime_assert((*pu.get<Syntax::PredictionUnit::PredFlagLX>())[l]);

    if(Plane::Cb == plane)
    {
        /* 04/2013, 8.5.3.3.3.3 "Chroma sample interpolation process" */
        return deriveChroma(decoder, picture, ref, pu, h, v, l, Chroma::Cb);
    }
    else if(Plane::Cr == plane)
    {
        return deriveChroma(decoder, picture, ref, pu, h, v, l, Chroma::Cr);
    }
    else //if(Plane::Y == plane)
    {
        /* 04/2013, 8.5.3.3.3.2 "Luma sample interpolation process" */
        return deriveLuma(decoder, picture, ref, pu, h, v, l);
    }
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */
