#include <Decoder/Processes/PictureOrderCount.h>
#include <Decoder/State.h>
#include <Syntax/SliceSegmentHeader.h>
#include <Structure/Picture.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
void PictureOrderCount::exec(
        State &decoder, Ptr<Structure::Picture> picture,
        const Syntax::SliceSegmentHeader &sh)
{
    using namespace Syntax;

    typedef SliceSegmentHeader SSH;

    const auto nalUnitType = picture->nalUnitType;
    const auto maxPicOrderCntLsb = picture->maxPicOrderCntLsb;
    const PicOrderCntLsb picOrderCntLsb = *sh.get<SSH::SlicePicOrderCntLsb>();

    PicOrderCntMsb picOrderCntMsb;

    if(!(isIRAP(nalUnitType) && picture->noRaslOutputFlag))
    {
        const auto prevTid0 = decoder.dpb.prevTemporalId0();
        const auto prevPicOrderCntMsb = prevTid0.get<PicOrderCntMsb>();
        const auto prevPicOrderCntLsb = prevTid0.get<PicOrderCntLsb>();

        if(
                picOrderCntLsb < prevPicOrderCntLsb
                && prevPicOrderCntLsb - picOrderCntLsb >= maxPicOrderCntLsb / 2)
        {
            picOrderCntMsb.value = prevPicOrderCntMsb.value + maxPicOrderCntLsb.value;
        }
        else if(
                picOrderCntLsb > prevPicOrderCntLsb
                && picOrderCntLsb - prevPicOrderCntLsb > maxPicOrderCntLsb / 2)
        {
            picOrderCntMsb.value = prevPicOrderCntMsb.value - maxPicOrderCntLsb.value;
        }
        else
        {
            picOrderCntMsb = prevPicOrderCntMsb;
        }

        const auto toStr =
            [&](std::ostream &oss)
            {
                if(isBLA(nalUnitType) || isCRA(nalUnitType))
                {
                    oss << "POC[" << int(nalUnitType) << "]: 0\n";
                }
                else
                {
                    oss << "POC[" << int(nalUnitType) << "]: "
                        << picOrderCntMsb.value + picOrderCntLsb.value << " (from: "
                        << prevTid0.get<PicOrderCntVal>() << " "
                        << maxPicOrderCntLsb << " "
                        << prevPicOrderCntLsb << " "
                        << prevPicOrderCntMsb << " "
                        << picOrderCntLsb << " "
                        << picOrderCntMsb << ")\n";
                }
            };

        HEVC::log(HEVC::LogId::PictureOrderCount, toStr);
    }
    else
    {
        const auto toStr =
            [&](std::ostream &oss)
            {
                oss << "POC[" << int(nalUnitType) << "]: 0\n";
            };

        HEVC::log(HEVC::LogId::PictureOrderCount, toStr);
    }

    const auto picOrderCntVal = PicOrderCntVal(picOrderCntMsb.value + picOrderCntLsb.value);

    picture->order.get<PicOrderCntLsb>() = picOrderCntLsb;
    picture->order.get<PicOrderCntMsb>() = picOrderCntMsb;
    picture->order.get<PicOrderCntVal>() = picOrderCntVal;

    std::cout
        << "[" << std::dec << picture->decodingNo << "]\t"
        << "POC: " << picOrderCntVal << ' ' << getName(nalUnitType);

    if(isRASL(nalUnitType))
    {
        std::cout << ", skipped";
    }

    std::cout << '\n';
}
/*----------------------------------------------------------------------------*/
}}} // namespace HEVC::Decoder::Processes
