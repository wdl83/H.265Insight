#include <Decoder/Processes/InterSamplesPrediction.h>
#include <Decoder/Processes/FractionalSamplesInterpolation.h>
#include <Decoder/Processes/WeightedSamplesPrediction.h>
#include <Decoder/Process.h>
#include <Decoder/State.h>
#include <Structure/Picture.h>
#include <Structure/Slice.h>
#include <Syntax/PredictionUnit.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
namespace {
/*----------------------------------------------------------------------------*/
bool deriveWeightedPredFlag(
        Ptr<const Structure::Picture> picture,
        Ptr<const Structure::Slice> slice)
{
    using namespace Syntax;
    typedef PictureParameterSet PPS;
    typedef SliceSegmentHeader SSH;

    const auto pps = picture->pps;
    const auto sh = slice->header();
    const auto sliceType = sh->get<SSH::SliceType>();

    runtime_assert(!isI(*sliceType));

    return
        isP(*sliceType)
        ? bool(*pps->get<PPS::WeightedPredFlag>())
        : bool(*pps->get<PPS::WeightedBipredFlag>());
}
/*----------------------------------------------------------------------------*/
PredSampleLx predSampleLx(
        State &decoder,
        Ptr<Structure::Picture> picture,
        Ptr<const Structure::Picture> ref,
        const Syntax::PredictionUnit &pu,
        Plane plane, Range<Pel> h, Range<Pel> v,
        RefList l)
{
    using namespace Syntax;

    typedef PredictionUnit PU;

    if(!(*pu.get<PU::PredFlagLX>())[l])
    {
        return PredSampleLx{};
    }

    const auto chromaFormatIdc = picture->chromaFormatIdc;
    const auto bitDepth = picture->bitDepth(plane);
    const auto puCoord = pu.get<PU::Coord>()->inUnits();
    const auto coord = scale(puCoord, plane, chromaFormatIdc);
    const auto width = h.length();
    const auto height = v.length();
    const auto &src = ref->pelBuffer(PelLayerId::Decoded, plane);
    const auto mv =
        Plane::Y == plane
        ? (*pu.get<PU::MvLX>())[l]
        : (*pu.get<PU::MvCLX>())[l];
    /* in 1/4 sample unit */
    const auto ratio =
        Plane::Y == plane
        ? FractionalSamplesInterpolation::BlkLuma::ratio
        : FractionalSamplesInterpolation::BlkChroma::ratio;

    const PelCoord refCoord = coord + toPel(mv, ratio);
    const auto xFracL = fraction(mv.x(), ratio);
    const auto yFracL = fraction(mv.y(), ratio);
    const auto pictureBdry = picture->boundaries(plane);
    const auto clip =
        [pictureBdry](PelCoord pos)
        {
            return ::clip(pictureBdry, pos);
        };

    if(0_sub_pel == xFracL && 0_sub_pel == yFracL)
    {
        PredSampleLx dst(width, height);
        const auto shift3 = std::max(2, 14 - bitDepth);

        for(auto yL = 0_pel; yL < height; ++yL)
        {
            for(auto xL = 0_pel; xL < width; ++xL)
            {
                dst[{xL, yL}] =
                    int(src[clip(refCoord + PelCoord{h.begin() + xL, v.begin() + yL})]) << shift3;
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
                    << " mv" << toPel(mv, Plane::Y == plane ? 2_log2 : 3_log2)
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

        const LogId logs[] =
        {
            LogId::InterPredictedSamplesY,
            LogId::InterPredictedSamplesCb,
            LogId::InterPredictedSamplesCr
        };

        log(logs[int(plane)], toStr);

        return dst;
    }
    else
    {
        return
            subprocess(
                    decoder, FractionalSamplesInterpolation(),
                    picture, ref, pu, plane, h, v, l);
    }
}
/*----------------------------------------------------------------------------*/
} /* namespace */
/*----------------------------------------------------------------------------*/
void InterSamplesPrediction::exec(
        State &decoder, Ptr<Structure::Picture> picture,
        const Syntax::PredictionUnit &pu)
{
    using namespace Syntax;

    typedef PredictionUnit PU;

    const auto chromaFormatIdc = picture->chromaFormatIdc;
    const auto puCoord = pu.get<PU::Coord>()->inUnits();
    const auto nPbW = pu.get<PU::Width>()->inUnits();
    const auto nPbH = pu.get<PU::Height>()->inUnits();
    const auto predFlag = pu.get<PU::PredFlagLX>();
    const auto refIdx = pu.get<PU::RefIdxLX>();

    const auto slice = picture->slice(puCoord);
    const auto &rpl = slice->rpl;
    const auto weightedPredFlag = deriveWeightedPredFlag(picture, slice);

    const auto ref =
        [&decoder, predFlag, refIdx, &rpl](RefList l)
        {
            return
                (*predFlag)[l]
                ? decoder.dpb.picture(rpl[l][(*refIdx)[l]])
                : nullptr;
        };

    const auto refL0 = ref(RefList::L0);
    const auto refL1 = ref(RefList::L1);

    {
        const auto delta = Pel{FractionalSamplesInterpolation::lumaSide};

        for(auto y = 0_pel; y < nPbH; y += delta)
        {
            const auto vStep = std::min(nPbH - y, delta);
            const auto yStart = puCoord.y() + y;
            const auto yEnd = yStart + vStep;

            for(auto x = 0_pel; x < nPbW; x += delta)
            {
                const auto hStep = std::min(nPbW - x, delta);
                const auto xStart = puCoord.x() + x;
                const auto xEnd = xStart + hStep;

                process(
                        decoder, WeightedSamplesPrediction(),
                        picture, slice, pu,
                        Plane::Y, Range<Pel>{xStart, xEnd}, Range<Pel>{yStart, yEnd},
                        weightedPredFlag,
                        predSampleLx(
                            decoder, picture, refL0, pu,
                            Plane::Y, {x, x + hStep}, {y, y + vStep}, RefList::L0),
                        predSampleLx(
                            decoder, picture, refL1, pu,
                            Plane::Y, {x, x + hStep}, {y, y + vStep}, RefList::L1));
            }
        }
    }

    if(isPresent(Component::Chroma, chromaFormatIdc))
    {
        const auto delta = Pel{FractionalSamplesInterpolation::chromaSide};
        const auto coord = scale(puCoord, Component::Chroma, chromaFormatIdc);
        const auto width = hScale(nPbW, Component::Chroma, chromaFormatIdc);
        const auto height = vScale(nPbH, Component::Chroma, chromaFormatIdc);

        for(auto y = 0_pel; y < height; y += delta)
        {
            const auto vStep = std::min(height - y, delta);
            const auto yStart = coord.y() + y;
            const auto yEnd = yStart + vStep;

            for(auto x = 0_pel; x < width; x += delta)
            {
                const auto hStep = std::min(width - x, delta);
                const auto xStart = coord.x() + x;
                const auto xEnd = xStart + hStep;

                process(
                        decoder, WeightedSamplesPrediction(),
                        picture, slice, pu,
                        Plane::Cb, Range<Pel>{xStart, xEnd}, Range<Pel>{yStart, yEnd},
                        weightedPredFlag,
                        predSampleLx(
                            decoder, picture, refL0, pu,
                            Plane::Cb, {x, x + hStep}, {y, y + vStep}, RefList::L0),
                        predSampleLx(
                            decoder, picture, refL1, pu,
                            Plane::Cb, {x, x + hStep}, {y, y + vStep}, RefList::L1));

                process(
                        decoder, WeightedSamplesPrediction(),
                        picture, slice, pu,
                        Plane::Cr, Range<Pel>{xStart, xEnd}, Range<Pel>{yStart, yEnd},
                        weightedPredFlag,
                        predSampleLx(
                            decoder, picture, refL0, pu,
                            Plane::Cr, {x, x + hStep}, {y, y + vStep}, RefList::L0),
                        predSampleLx(
                            decoder, picture, refL1, pu,
                            Plane::Cr, {x, x + hStep}, {y, y + vStep}, RefList::L1));
            }
        }
    }

    for(auto plane : EnumRange<Plane>())
    {
        const auto toStr =
            [picture, chromaFormatIdc, puCoord, nPbW, nPbH, &predFlag, weightedPredFlag, plane](std::ostream &oss)
            {
                const auto base = scale(puCoord, plane, chromaFormatIdc);
                const auto w = hScale(nPbW, plane, chromaFormatIdc);
                const auto h = vScale(nPbH, plane, chromaFormatIdc);
                const auto predicted = picture->pelBuffer(PelLayerId::Prediction, plane);

                oss
                    << puCoord
                    << " weightedPredFlag " << weightedPredFlag;

                if((*predFlag)[RefList::L0])
                {
                    oss << " L0";
                }

                if((*predFlag)[RefList::L1])
                {
                    oss << " L1";
                }

                oss << '\n';

                for(auto y = 0_pel; y < h; ++y)
                {
                    for(auto x = 0_pel; x < w; ++x)
                    {
                        pelFmt(oss, predicted[base + PelCoord{x, y}]);
                        oss << (w - 1_pel == x ? '\n' : ' ');
                    }
                }
            };

        const LogId logId[] =
        {
            LogId::InterWeightedSamplesPredictionY,
            LogId::InterWeightedSamplesPredictionCb,
            LogId::InterWeightedSamplesPredictionCr
        };

        log(logId[int(plane)], toStr);
    }
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */
