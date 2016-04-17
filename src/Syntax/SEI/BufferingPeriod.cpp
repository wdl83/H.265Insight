#include <Syntax/SEI/BufferingPeriod.h>
#include <Syntax/HrdParameters.h>


namespace HEVC { namespace Syntax { namespace SEI {
/*----------------------------------------------------------------------------*/
void BufferingPeriod::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
        const HrdParameters &hrdp, int cpbCnt)
{
    typedef HrdParameters HrdP;

    const auto initialCpbRemovalDelayLengthMinus1 =
        hrdp.get<HrdP::InitialCpbRemovalDelayLengthMinus1>();

    auto bpSeqParameterSetId = embed<BpSeqParameterSetId>(*this);
    auto irapCpbParamsPresentFlag = embed<IrapCpbParamsPresentFlag>(*this);

    parse(streamAccessLayer, decoder, *bpSeqParameterSetId);

    if(!*hrdp.get<HrdP::SubPicHrdParamsPresentFlag>())
    {
        parse(streamAccessLayer, decoder, *irapCpbParamsPresentFlag);
    }

    if(*irapCpbParamsPresentFlag)
    {
        parse(
                streamAccessLayer, decoder,
                *embed<CpbDelayOffset>(
                    *this, *hrdp.get<HrdP::AuCpbRemovalDelayLengthMinus1>()));
        parse(
                streamAccessLayer, decoder,
                *embed<DpbDelayOffset>(
                    *this, *hrdp.get<HrdP::DpbOutputDelayLengthMinus1>()));
    }

    parse(streamAccessLayer, decoder, *embed<ConcatenationFlag>(*this));
    parse(
            streamAccessLayer, decoder,
            *embed<AuCpbRemovalDelayDeltaMinus1>(
                *this, *hrdp.get<HrdP::AuCpbRemovalDelayLengthMinus1>()));

    if(*hrdp.get<HrdP::NalHrdBpPresentFlag>())
    {
        auto nalInitialCpbRemovalDelay =
            embed<NalInitialCpbRemovalDelay>(*this, *initialCpbRemovalDelayLengthMinus1);
        auto nalInitialCpbRemovalOffset =
            embed<NalInitialCpbRemovalOffset>(*this, *initialCpbRemovalDelayLengthMinus1);
        auto nalInitialAltCpbRemovalDelay =
            embed<NalInitialAltCpbRemovalDelay>(*this, *initialCpbRemovalDelayLengthMinus1);
        auto nalInitialAltCpbRemovalOffset =
            embed<NalInitialAltCpbRemovalOffset>(*this, *initialCpbRemovalDelayLengthMinus1);

        for(auto i = 0; i <= cpbCnt; ++i)
        {
            parse(streamAccessLayer, decoder, *nalInitialCpbRemovalDelay, i);
            parse(streamAccessLayer, decoder, *nalInitialCpbRemovalOffset, i);

            if(
                    *hrdp.get<HrdP::SubPicHrdParamsPresentFlag>()
                    || *irapCpbParamsPresentFlag)
            {
                parse(streamAccessLayer, decoder, *nalInitialAltCpbRemovalDelay, i);
                parse(streamAccessLayer, decoder, *nalInitialAltCpbRemovalOffset, i);
            }
        }
    }

    if(*hrdp.get<HrdP::VclHrdBpPresentFlag>())
    {
        auto vclInitialCpbRemovalDelay =
            embed<NalInitialCpbRemovalDelay>(*this, *initialCpbRemovalDelayLengthMinus1);
        auto vclInitialCpbRemovalOffset =
            embed<NalInitialCpbRemovalOffset>(*this, *initialCpbRemovalDelayLengthMinus1);
        auto vclInitialAltCpbRemovalDelay =
            embed<NalInitialAltCpbRemovalDelay>(*this, *initialCpbRemovalDelayLengthMinus1);
        auto vclInitialAltCpbRemovalOffset =
            embed<NalInitialAltCpbRemovalOffset>(*this, *initialCpbRemovalDelayLengthMinus1);

        for(auto i = 0; i <= cpbCnt; ++i)
        {
            parse(streamAccessLayer, decoder, *vclInitialCpbRemovalDelay, i);
            parse(streamAccessLayer, decoder, *vclInitialCpbRemovalOffset, i);

            if(
                    *hrdp.get<HrdP::SubPicHrdParamsPresentFlag>()
                    || *irapCpbParamsPresentFlag)
            {
                parse(streamAccessLayer, decoder, *vclInitialAltCpbRemovalDelay, i);
                parse(streamAccessLayer, decoder, *vclInitialAltCpbRemovalOffset, i);
            }
        }
    }
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Syntax::SEI */
