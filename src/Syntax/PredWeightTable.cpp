/* HEVC */
#include <Syntax/SpsRangeExtension.h>
#include <Syntax/PredWeightTable.h>
#include <Syntax/SliceSegmentHeader.h>
#include <Structure/Picture.h>
#include <Decoder/State.h>

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
void PredWeightTable::toStr(std::ostream &os) const
{
    os
        << getName(LumaLog2WeightDenom::Id) << ' '
        << get<LumaLog2WeightDenom>()->inUnits() << '\n'
        << getName(LumaWeightLx::Id) << '\n';

    for(auto l : EnumRange<RefList>())
    {
        os << getName(l);

        for(auto i = 0; i < Limits::RefListIdx::max + 1; ++i)
        {
            os << ' ' << (*get<LumaWeightLx>())[makeTuple(l, i)];
        }

        os << '\n';
    }

    os << getName(LumaOffsetL0::Id);

    for(auto i = 0; i < Limits::RefListIdx::max + 1; ++i)
    {
        os << ' ' << (*get<LumaOffsetL0>())[i];
    }

    os << '\n' << getName(LumaOffsetL1::Id);

    for(auto i = 0; i < Limits::RefListIdx::max + 1; ++i)
    {
        os << ' ' << (*get<LumaOffsetL1>())[i];
    }

    os
        << '\n' << getName(ChromaLog2WeightDenom::Id) << ' '
        << get<ChromaLog2WeightDenom>()->inUnits() << '\n'
        << getName(ChromaWeightLx::Id) << '\n';

    for(auto chroma : EnumRange<Chroma>())
    {
        os << getName(chroma) << '\n';

        for(auto l : EnumRange<RefList>())
        {
            os << getName(l);

            for(auto i = 0; i < Limits::RefListIdx::max + 1; ++i)
            {
                os << ' ' << (*get<ChromaWeightLx>())[makeTuple(chroma, l, i)];
            }

            os << '\n';
        }
    }

    os << getName(ChromaOffsetLx::Id) << '\n';

    for(auto chroma : EnumRange<Chroma>())
    {
        os << getName(chroma) << '\n';

        for(auto l : EnumRange<RefList>())
        {
            os << getName(l);

            for(auto i = 0; i < Limits::RefListIdx::max + 1; ++i)
            {
                os << ' ' << (*get<ChromaOffsetLx>())[makeTuple(chroma, l, i)];
            }

            os << '\n';
        }
    }
}
/*----------------------------------------------------------------------------*/
void PredWeightTable::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
        const SliceSegmentHeader &sh)
{
    typedef SpsRangeExtension SPSRE;
    typedef SliceSegmentHeader SSH;

    const auto picture = decoder.picture();

    const auto spsre = picture->spsre;
    const auto heighPrecisionOffsetsEnabledFlag =
        spsre && bool(*spsre->get<SPSRE::HighPrecisionOffsetsEnabledFlag>());

    /* Luma */
    auto lumaLog2WeightDenom = embed<LumaLog2WeightDenom>(*this);
    auto lumaWeightL0Flag = embed<LumaWeightL0Flag>(*this);
    auto deltaLumaWeightL0 = embed<DeltaLumaWeightL0>(*this);
    auto lumaOffsetL0 = embed<LumaOffsetL0>(*this);
    auto lumaWeightL1Flag = embed<LumaWeightL1Flag>(*this);
    auto deltaLumaWeightL1 = embed<DeltaLumaWeightL1>(*this);
    auto lumaOffsetL1 = embed<LumaOffsetL1>(*this);

    /* pseudo (Luma) */
    auto lumaWeightLx = embed<LumaWeightLx>(*this);

    const auto chromaFormatIdc = picture->chromaFormatIdc;
    const auto monochrome = chromaFormatIdc == ChromaFormatIdc::f400;

    /* Chroma */
    auto deltaChromaWeightL0 =
        monochrome ? nullptr : embed<DeltaChromaWeightL0>(*this);
    auto deltaChromaOffsetL0 =
        monochrome ? nullptr : embed<DeltaChromaOffsetL0>(*this);
    auto deltaChromaWeightL1 =
        monochrome ? nullptr : embed<DeltaChromaWeightL1>(*this);
    auto deltaChromaOffsetL1 =
        monochrome ? nullptr : embed<DeltaChromaOffsetL1>(*this);

    /* pseudo (Chroma) */
    auto chromaWeightLx =
        monochrome ? nullptr : embed<ChromaWeightLx>(*this);
    auto chromaOffsetLx =
        monochrome ? nullptr : embed<ChromaOffsetLx>(*this);

    parse(streamAccessLayer, decoder, *lumaLog2WeightDenom);

    if(!monochrome)
    {
        auto deltaChromaLog2WeightDenom = embed<DeltaChromaLog2WeightDenom>(*this);

        parse(streamAccessLayer, decoder, *deltaChromaLog2WeightDenom);
        embed<ChromaLog2WeightDenom>(*this, *lumaLog2WeightDenom, *deltaChromaLog2WeightDenom);
    }

    const auto numRefIdxL0ActiveMinus1 = sh.get<SSH::NumRefIdxL0ActiveMinus1>();

    for(auto i = 0; i <= int(*numRefIdxL0ActiveMinus1); ++i)
    {
        parse(streamAccessLayer, decoder, *lumaWeightL0Flag, i);
    }

    if(!monochrome)
    {
        auto chromaWeightL0Flag = embed<ChromaWeightL0Flag>(*this);

        for(auto i = 0; i <= *numRefIdxL0ActiveMinus1; ++i)
        {
            parse(streamAccessLayer, decoder, *chromaWeightL0Flag, i);
        }
    }

    for(auto i = 0; i <= int(*numRefIdxL0ActiveMinus1); ++i)
    {
        if((*lumaWeightL0Flag)[i])
        {
            parse(streamAccessLayer, decoder, *deltaLumaWeightL0, i);
            parse(streamAccessLayer, decoder, *lumaOffsetL0, i);
        }

        if(!monochrome && (*get<ChromaWeightL0Flag>())[i])
        {
            for(auto chroma : EnumRange<Chroma>())
            {
                parse(streamAccessLayer, decoder, *deltaChromaWeightL0, chroma, i);
                parse(streamAccessLayer, decoder, *deltaChromaOffsetL0, chroma, i);
            }
        }
    }

    lumaWeightLx->setL0(
            *numRefIdxL0ActiveMinus1,
            *lumaLog2WeightDenom,
            *deltaLumaWeightL0);

    if(!monochrome)
    {
        chromaWeightLx->setL0(
                *numRefIdxL0ActiveMinus1,
                *get<ChromaLog2WeightDenom>(),
                *deltaChromaWeightL0);

        chromaOffsetLx->setL0(
                *numRefIdxL0ActiveMinus1,
                *get<ChromaLog2WeightDenom>(),
                *get<ChromaWeightL0Flag>(),
                *deltaChromaOffsetL0,
                *chromaWeightLx,
                weightedPredictionOffsetHalfRange(
                    heighPrecisionOffsetsEnabledFlag,
                    picture->bitDepth(Component::Chroma)));
    }

    if(isB(*sh.get<SSH::SliceType>()))
    {
        const auto numRefIdxL1ActiveMinus1 = sh.get<SSH::NumRefIdxL1ActiveMinus1>();

        for(auto i = 0; i <= int(*numRefIdxL1ActiveMinus1); ++i)
        {
            parse(streamAccessLayer, decoder, *lumaWeightL1Flag, i);
        }

        if(!monochrome)
        {
            auto chromaWeightL1Flag = embed<ChromaWeightL1Flag>(*this);

            for(auto i = 0; i <= int(*numRefIdxL1ActiveMinus1); ++i)
            {
                parse(streamAccessLayer, decoder, *chromaWeightL1Flag, i);
            }
        }

        for(auto i = 0; i <= int(*numRefIdxL1ActiveMinus1); ++i)
        {
            if((*lumaWeightL1Flag)[i])
            {
                parse(streamAccessLayer, decoder, *deltaLumaWeightL1, i);
                parse(streamAccessLayer, decoder, *lumaOffsetL1, i);
            }

            if(!monochrome && (*get<ChromaWeightL1Flag>())[i])
            {
                for(auto chroma : EnumRange<Chroma>())
                {
                    parse(streamAccessLayer, decoder, *deltaChromaWeightL1, chroma, i);
                    parse(streamAccessLayer, decoder, *deltaChromaOffsetL1, chroma, i);
                }
            }
        }

        lumaWeightLx->setL1(*numRefIdxL1ActiveMinus1, *lumaLog2WeightDenom, *deltaLumaWeightL1);

        if(!monochrome)
        {
            chromaWeightLx->setL1(
                    *numRefIdxL1ActiveMinus1,
                    *get<ChromaLog2WeightDenom>(),
                    *deltaChromaWeightL1);

            chromaOffsetLx->setL1(
                    *numRefIdxL1ActiveMinus1,
                    *get<ChromaLog2WeightDenom>(),
                    *get<ChromaWeightL1Flag>(),
                    *deltaChromaOffsetL1,
                    *chromaWeightLx,
                    weightedPredictionOffsetHalfRange(
                        heighPrecisionOffsetsEnabledFlag,
                        picture->bitDepth(Component::Chroma)));
        }
    }

    log(
            LogId::PredWeightTable,
            [this](std::ostream &oss){toStr(oss);});
}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */
