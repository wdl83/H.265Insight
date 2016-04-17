#include <Syntax/HrdParameters.h>


namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
void HrdParameters::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
        bool commonInfPresentFlag, int maxNumSubLayersMinus1)
{
    auto nalHrdParametersPresentFlag = embed<NalHrdParametersPresentFlag>(*this);
    auto vclHrdParametersPresentFlag = embed<VclHrdParametersPresentFlag>(*this);
    auto initialCpbRemovalDelayLengthMinus1 = embed<InitialCpbRemovalDelayLengthMinus1>(*this);
    auto auCpbRemovalDelayLengthMinus1 = embed<AuCpbRemovalDelayLengthMinus1>(*this);
    auto dpbOutputDelayLengthMinus1 = embed<DpbOutputDelayLengthMinus1>(*this);
    auto subPicHrdParamsPresentFlag = embed<SubPicHrdParamsPresentFlag>(*this);

    if(commonInfPresentFlag)
    {
        parse(streamAccessLayer, decoder, *nalHrdParametersPresentFlag);
        parse(streamAccessLayer, decoder, *vclHrdParametersPresentFlag);

        if(*nalHrdParametersPresentFlag || *vclHrdParametersPresentFlag)
        {
            parse(streamAccessLayer, decoder, *subPicHrdParamsPresentFlag);

            if(*subPicHrdParamsPresentFlag)
            {
                auto tickDivisorDiv2 =
                    embed<TickDivisorDiv2>(*this);
                auto duCpbRemovalDelayIncrementLengthMinus1 =
                    embed<DuCpbRemovalDelayIncrementLengthMinus1>(*this);
                auto subPicCpbParamsInPicTimingSeiFlag =
                    embed<SubPicCpbParamsInPicTimingSeiFlag>(*this);
                auto dpbOutputDelayDuLengthMinus1 =
                    embed<DpbOutputDelayDuLengthMinus1>(*this);

                parse(streamAccessLayer, decoder, *tickDivisorDiv2);
                parse(streamAccessLayer, decoder, *duCpbRemovalDelayIncrementLengthMinus1);
                parse(streamAccessLayer, decoder, *subPicCpbParamsInPicTimingSeiFlag);
                parse(streamAccessLayer, decoder, *dpbOutputDelayDuLengthMinus1);
            }

            parse(streamAccessLayer, decoder, *embed<BitRateScale>(*this));
            parse(streamAccessLayer, decoder, *embed<CpbSizeScale>(*this));

            if(*subPicHrdParamsPresentFlag)
            {
                parse(streamAccessLayer, decoder, *embed<CpbSizeDuScale>(*this));
            }

            parse(
                streamAccessLayer, decoder, *initialCpbRemovalDelayLengthMinus1);
            parse(streamAccessLayer, decoder, *auCpbRemovalDelayLengthMinus1);
            parse(streamAccessLayer, decoder, *dpbOutputDelayLengthMinus1);
        }
    }

    auto fixedPicRateGeneralFlag = embed<FixedPicRateGeneralFlag>(*this);
    auto fixedPicRateWithinCvsFlag = embed<FixedPicRateWithinCvsFlag>(*this);
    auto elementalDurationInTcMinus1 = embed<ElementalDurationInTcMinus1>(*this);
    auto lowDelayHrdFlag = embed<LowDelayHrdFlag>(*this);
    auto cpbCntMinus1 = embed<CpbCntMinus1>(*this);

    for(auto i = 0; i <= maxNumSubLayersMinus1; ++i)
    {
        parse(streamAccessLayer, decoder, *fixedPicRateGeneralFlag, i);

        if(!(*fixedPicRateGeneralFlag)[i])
        {
            parse(streamAccessLayer, decoder, *fixedPicRateWithinCvsFlag, i);
        }

        if((*fixedPicRateWithinCvsFlag)[i])
        {
            parse(streamAccessLayer, decoder, *elementalDurationInTcMinus1, i);
        }
        else
        {
            parse(streamAccessLayer, decoder, *lowDelayHrdFlag, i);
        }

        if(!(*lowDelayHrdFlag)[i])
        {
            parse(streamAccessLayer, decoder, *cpbCntMinus1, i);
        }

        if(*get<NalHrdParametersPresentFlag>())
        {
            auto subLayerHrdParameters =
                embedSubtreeInList<SubLayerHrdParameters>(*this, i);

            parse(
                    streamAccessLayer, decoder, *subLayerHrdParameters,
                    (*cpbCntMinus1)[i], bool(*get<SubPicHrdParamsPresentFlag>()));
        }

        if(*get<VclHrdParametersPresentFlag>())
        {
            auto subLayerHrdParameters =
                embedSubtreeInList<SubLayerHrdParameters>(*this, i);

            parse(
                    streamAccessLayer, decoder, *subLayerHrdParameters,
                    (*cpbCntMinus1)[i], bool(*get<SubPicHrdParamsPresentFlag>()));
        }
    }
}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */
