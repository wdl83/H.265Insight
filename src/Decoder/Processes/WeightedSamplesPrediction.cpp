#include <Decoder/Processes/WeightedSamplesPrediction.h>
#include <Syntax/SpsRangeExtension.h>
#include <Syntax/PictureParameterSet.h>
#include <Syntax/SliceSegmentHeader.h>
#include <Syntax/PredictionUnit.h>
#include <Syntax/PredWeightTable.h>
#include <Structure/Picture.h>


namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
namespace {
/*----------------------------------------------------------------------------*/
void defaultWeighted(
        int bitDepth,
        Range<Pel> h, Range<Pel> v,
        const PredSampleLx &src,
        Structure::PelBuffer &dst)
{
    /* 04/2013, 8.5.3.3.4.2 "Default weighted sample predition process" */
    const auto shift = 14 - bitDepth;
    const auto offset = 0 < shift ? 1 << (shift - 1) : 0;
    const auto max = (1 << bitDepth) - 1;
    const auto width = h.length();
    const auto height = v.length();

    for(auto y = 0_pel; y < height; ++y)
    {
        for(auto x = 0_pel; x < width; ++x)
        {
            dst[{h.begin() + x, v.begin() + y}] =
                clip3(0, max, (src[{x, y}] + offset) >> shift);
        }
    }
}
/*----------------------------------------------------------------------------*/
void defaultWeighted(
        int bitDepth,
        Range<Pel> h, Range<Pel> v,
        const PredSampleLx &srcL0, const PredSampleLx &srcL1,
        Structure::PelBuffer &dst)
{
    /* 04/2013, 8.5.3.3.4.2 "Default weighted sample predition process" */
    const auto shift = 15 - bitDepth;
    const auto offset = 1 << (shift - 1);
    const auto max = (1 << bitDepth) - 1;
    const auto width = h.length();
    const auto height = v.length();

    for(auto y = 0_pel; y < height; ++y)
    {
        for(auto x = 0_pel; x < width; ++x)
        {
            dst[{h.begin() + x, v.begin() + y}] =
                clip3(0, max, (srcL0[{x, y}] + srcL1[{x, y}] + offset) >> shift);
        }
    }
}
/*----------------------------------------------------------------------------*/
void explicitWeighted(
        int bitDepth,
        Range<Pel> h, Range<Pel> v,
        const PredSampleLx &src,
        Log2 wd, int w, int o,
        Structure::PelBuffer &dst)
{
    /* 04/2013, 8.5.3.3.4.3 "Explicit weighted sample predition process" */
    const auto cond = 1_log2 <= wd;
    const auto shift = toUnderlying(wd);
    const auto offset = toInt(wd - 1_log2);
    const auto width = h.length();
    const auto height = v.length();

    for(auto y = 0_pel; y < height; ++y)
    {
        for(auto x = 0_pel; x < width; ++x)
        {
            const auto baseValue = src[{x, y}] * w;
            const auto value = o + (cond ? (baseValue + offset) >> shift : baseValue);

            dst[{x + h.begin(), y + v.begin()}] = clip3(0, (1 << bitDepth) - 1, value);
        }
    }
}
/*----------------------------------------------------------------------------*/
void explicitWeighted(
        int bitDepth,
        const Range<Pel> h, const Range<Pel> v,
        const PredSampleLx &srcL0, const PredSampleLx &srcL1,
        Log2 wd,
        int w0, int w1,
        int o0, int o1,
        Structure::PelBuffer &dst)
{
    /* 04/2013, 8.5.3.3.4.3 "Explicit weighted sample predition process" */
    const auto shift = toUnderlying(wd);
    const auto width = h.length();
    const auto height = v.length();

    for(auto y = 0_pel; y < height; ++y)
    {
        for(auto x = 0_pel; x < width; ++x)
        {
            const int baseValue0 = int(srcL0[{x, y}]) * w0;
            const int baseValue1 = int(srcL1[{x, y}]) * w1;
            const auto value =
                (baseValue0 + baseValue1 + ((o0 + o1  + 1) << shift)) >> (shift + 1);
            dst[{x + h.begin(), y + v.begin()}] = clip3(0, (1 << bitDepth) - 1, value);
        }
    }
}
/*----------------------------------------------------------------------------*/
void weight(
        Ptr<Structure::Picture> picture,
        Ptr<const Structure::Slice> slice,
        const Syntax::PredictionUnit &pu,
        Plane plane, Range<Pel> h, Range<Pel> v,
        bool weightedPredFlag,
        PredSampleLx &&srcL0, PredSampleLx &&srcL1)
{
    /* 04/2013, 8.5.3.3.4 "Weighted sample prediction process" */
    using namespace Syntax;

    typedef SpsRangeExtension SPSRE;
    typedef PredWeightTable PWT;
    typedef PredictionUnit PU;

    const auto spsre = picture->spsre;

    const auto heighPrecisionOffsetsEnabledFlag =
        spsre && bool(*spsre->get<SPSRE::HighPrecisionOffsetsEnabledFlag>());

    const auto bitDepth = picture->bitDepth(plane);
    const auto predFlag = pu.get<PU::PredFlagLX>();

    using namespace Structure;

    auto &dst = picture->pelBuffer(PelLayerId::Prediction, plane);

    if(!weightedPredFlag)
    {
        if((*predFlag)[RefList::L0] && (*predFlag)[RefList::L1])
        {
            defaultWeighted(bitDepth, h, v, srcL0, srcL1, dst);
        }
        else if((*predFlag)[RefList::L0])
        {
            defaultWeighted(bitDepth, h, v, srcL0, dst);
        }
        else if((*predFlag)[RefList::L1])
        {
            defaultWeighted(bitDepth, h, v, srcL1, dst);
        }
    }
    else
    {
        const auto shift1 = 14 - bitDepth;
        //const auto shift2 = 1 << (bitDepth - 8);
        const auto wpOffsetBdShift =
            weightedPredictionOffsetBdShift(heighPrecisionOffsetsEnabledFlag, bitDepth);
#if 0
        const auto wpOffsetHalfRange =
            weightedPredictionOffsetHalfRange(heighPrecisionOffsetsEnabledFlag, bitDepth);
#endif

        const auto refIdxLx = pu.get<PU::RefIdxLX>();
        const auto refIdxL0 = (*refIdxLx)[RefList::L0];
        const auto refIdxL1 = (*refIdxLx)[RefList::L1];
        const auto pwt = slice->header()->getSubtree<PredWeightTable>();

        const auto weightDenom = pwt->weightDenom(plane);
        const auto wd = weightDenom + Log2(shift1);

        if((*predFlag)[RefList::L0] && (*predFlag)[RefList::L1])
        {
            const auto w0 = pwt->weight(plane, RefList::L0, int(refIdxL0.value));
            const auto w1 = pwt->weight(plane, RefList::L1, int(refIdxL1.value));
            const auto o0 = pwt->offset(plane, RefList::L0, int(refIdxL0.value)) << wpOffsetBdShift;
            const auto o1 = pwt->offset(plane, RefList::L1, int(refIdxL1.value)) << wpOffsetBdShift;

            explicitWeighted(bitDepth, h, v, srcL0, srcL1, wd, w0, w1, o0, o1, dst);
        }
        else if((*predFlag)[RefList::L0])
        {
            const auto w = pwt->weight(plane, RefList::L0, int(refIdxL0.value));
            const auto o = pwt->offset(plane, RefList::L0, int(refIdxL0.value)) << wpOffsetBdShift;

            explicitWeighted(bitDepth, h, v, srcL0, wd, w, o, dst);
        }
        else if((*predFlag)[RefList::L1])
        {
            const auto w = pwt->weight(plane, RefList::L1, int(refIdxL1.value));
            const auto o = pwt->offset(plane, RefList::L1, int(refIdxL1.value)) << wpOffsetBdShift;

            explicitWeighted(bitDepth, h, v, srcL1, wd, w, o, dst);
        }
    }
}
/*----------------------------------------------------------------------------*/
} /* namespace */
/*----------------------------------------------------------------------------*/
void WeightedSamplesPrediction::exec(
        State &, Ptr<Structure::Picture> picture,
        Ptr<const Structure::Slice> slice,
        const Syntax::PredictionUnit &pu,
        Plane plane, Range<Pel> h, Range<Pel> v,
        bool weightedPredFlag,
        PredSampleLx &&srcL0, PredSampleLx &&srcL1)
{
    weight(
            picture, slice, pu,
            plane, h, v,
            weightedPredFlag,
            std::move(srcL0), std::move(srcL1));
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */
