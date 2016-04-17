#include <Syntax/PpsRangeExtension.h>
#include <Syntax/PictureParameterSet.h>

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
void PpsRangeExtension::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
        const PictureParameterSet &pps)
{
    typedef PictureParameterSet PPS;

    if(*pps.get<PPS::TransformSkipEnabledFlag>())
    {
        parse(streamAccessLayer, decoder, *embed<MaxTransformSkipBlockSizeMinus2>(*this));
    }

    parse(streamAccessLayer, decoder, *embed<CrossComponentPredictionEnabledFlag>(*this));

    auto chromaQpOffsetListEnabledFlag = embed<ChromaQpOffsetListEnabledFlag>(*this);

    parse(streamAccessLayer, decoder, *chromaQpOffsetListEnabledFlag);

    if(*chromaQpOffsetListEnabledFlag)
    {
        parse(streamAccessLayer, decoder, *embed<DiffCuChromaQpOffsetDepth>(*this));

        auto chromaQpOffsetListLenMinus1 = embed<ChromaQpOffsetListLenMinus1>(*this);

        parse(streamAccessLayer, decoder, *chromaQpOffsetListLenMinus1);

        auto cbQpOffsetList = embed<CbQpOffsetList>(*this);
        auto crQpOffsetList = embed<CrQpOffsetList>(*this);

        for(auto i = 0; i <= chromaQpOffsetListLenMinus1->inUnits(); ++i)
        {
            parse(streamAccessLayer, decoder, *cbQpOffsetList, i);
            parse(streamAccessLayer, decoder, *crQpOffsetList, i);
        }
    }

    parse(streamAccessLayer, decoder, *embed<SaoOffsetScaleLuma>(*this));
    parse(streamAccessLayer, decoder, *embed<SaoOffsetScaleChroma>(*this));
}
/*----------------------------------------------------------------------------*/
}} // HEVC::Syntax
