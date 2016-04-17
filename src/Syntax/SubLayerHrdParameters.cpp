#include <Syntax/SubLayerHrdParameters.h>

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
void SubLayerHrdParameters::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
        int cpbCnt, bool subPicHrdParamsPresentFlag)
{
    auto bitRateValueMinus1 = embed<BitRateValueMinus1>(*this);
    auto cpbSizeValueMinus1 = embed<CpbSizeValueMinus1>(*this);
    auto cpbSizeDuValueMinus1 = embed<CpbSizeDuValueMinus1>(*this);
    auto bitRateDuValueMinus1 = embed<BitRateDuValueMinus1>(*this);
    auto cbrFlag = embed<CbrFlag>(*this);

    for(auto i = 0; i <= cpbCnt; ++i)
    {
        parse(streamAccessLayer, decoder, *bitRateValueMinus1, i);
        parse(streamAccessLayer, decoder, *cpbSizeValueMinus1, i);

        if(subPicHrdParamsPresentFlag)
        {
            parse(streamAccessLayer, decoder, *cpbSizeDuValueMinus1, i);
            parse(streamAccessLayer, decoder, *bitRateDuValueMinus1, i);
        }

        parse(streamAccessLayer, decoder, *cbrFlag, i);
    }
}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */
