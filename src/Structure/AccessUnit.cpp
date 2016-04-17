#include <Structure/AccessUnit.h>
#include <Structure/Picture.h>
#include <Decoder/State.h>
#include <Syntax/StreamNalUnit.h>
#include <Syntax/NalUnit.h>
#include <Syntax/SeiRbsp.h>
#include <Syntax/SeiMessage.h>
#include <Syntax/SEI/Payload.h>
#include <Syntax/SEI/DecodedPictureHash.h>
/* STDC++ */
#include <fstream>

namespace HEVC { namespace Structure {
/*----------------------------------------------------------------------------*/
namespace {
/*----------------------------------------------------------------------------*/
template <typename T>
std::chrono::nanoseconds ns(const T duration)
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
}

std::string toStrProcessStats(const AccessUnit &au)
{
    using namespace Decoder::Processes;

    const auto totalDuration = au.stats.duration();
    std::ostringstream oss;

    oss
        << "#PICTURE " << au.picture()->decodingNo << '\t' << totalDuration.count() << "us\n";

    for(auto id : EnumRange<ProcessId>{})
    {
        const auto count = au.stats[id].count;

        if(0 < count)
        {
            const auto duration = au.stats[id].duration;
            const auto totalNorm = (float(duration.count()) / totalDuration.count()) * 100;

            oss
                << align(getName(id))
                << ' '
                << std::setw(10) << count << ' '
                << std::setw(10) << duration.count() << "us "
                << std::setw(10) << ns(duration).count() / count << "ns/p"
                << " Tnorm " << std::setprecision(2) << std::setw(10) << totalNorm << '%'
                << '\n';
        }
    }

    const auto major =
        au.stats[ProcessId::IntraSamplesPrediction].duration
        + au.stats[ProcessId::InterPrediction].duration
        + au.stats[ProcessId::Residuals].duration
        + au.stats[ProcessId::Deblock].duration
        + au.stats[ProcessId::SampleAdaptiveOffset].duration;

    oss
        << "summary (Intra/Inter/Residuals/Deblock/SAO)"
        << ' ' << std::setprecision(2)
        << (float(major.count()) / totalDuration.count()) * 100 << "%\n";

    return oss.str();
}
/*----------------------------------------------------------------------------*/
std::string toStrParserStats(const AccessUnit &au)
{
    const auto totalDuration = ns(au.stats.duration());
    std::ostringstream oss;

    oss
        << "#PICTURE " << au.picture()->decodingNo << '\t'
        << totalDuration.count() << "ns\n";

    for(auto id : EnumRange<Syntax::ElementId>())
    {
        uint64_t count{0};
        Syntax::StreamNalUnit::ElementStats::Duration duration{0};

        for(const auto &strmNAL : au.list)
        {
            count += strmNAL->stats[id].count;
            duration += strmNAL->stats[id].duration;
        }

        if(0 < count)
        {
            const auto tNorm = (float(duration.count()) / totalDuration.count()) * 100;

            oss
                << align(getName(id))
                << ' '
                << std::setw(10) << count << ' '
                << std::setw(10) << duration.count() << "ns"
                << " Tnorm " << std::setprecision(2) << std::setw(10) << tNorm << '%'
                << '\n';
        }
    }

    return oss.str();
}
/*----------------------------------------------------------------------------*/
} /* namespace */
/*----------------------------------------------------------------------------*/
void AccessUnit::toStr(std::ostream &os)const
{
    os
        << "TID " << temporalId
        << " SNUs " << cntr.snu;

    if(picture())
    {
        os
            << ' ' << getName(picture()->nalUnitType)
            << " decodingNo " << picture()->decodingNo
            << " outputNo " << picture()->outputNo
            << " picOutputFlag " << (picture()->picOutputFlag ? "1" : "0")
            << " POC[VAL " << picture()->order.get<PicOrderCntVal>()
            << " MSB " << picture()->order.get<PicOrderCntMsb>()
            << " LSB " << picture()->order.get<PicOrderCntLsb>()
            << "] " << getName(picture()->reference.get<RefPicType>());

        int64_t size = 0;

        for(auto layerId : EnumRange<PelLayerId>())
        {
            for(auto plane : EnumRange<Plane>())
            {
                size += picture()->pelBuffer(layerId, plane).capacity();
            }
        }

        os << " size " << size;
    }
}
/*----------------------------------------------------------------------------*/
void AccessUnit::onDecodeFinish(PictureOutput pictureOutput)
{
    stats.endTime = std::chrono::high_resolution_clock::now();

    log(
            LogId::ParserStats,
                [this](std::ostream &oss){oss << toStrParserStats(*this);});
    log(
            LogId::ProcessStats,
                [this](std::ostream &oss){oss << toStrProcessStats(*this);});

    storePicture(pictureOutput, "decoding_order_");
}
/*----------------------------------------------------------------------------*/
void AccessUnit::storePicture(PictureOutput pictureOutput, const std::string &prefix) const
{
    if(PictureOutput::Disabled == pictureOutput)
    {
        return;
    }

    if(PictureOutput::Discrete == pictureOutput)
    {
        const auto bppY = picture()->bitDepth(Component::Luma);
        const auto bppC = picture()->bitDepth(Component::Chroma);
        const auto head =
            "outputNo" + std::to_string(picture()->outputNo) + '_'
            + "decodingNo" + std::to_string(picture()->decodingNo) + '_'
            + "POC" + std::to_string(picture()->order.get<PicOrderCntVal>().value) + '_'
            + (picture()->picOutputFlag ? "Y" : "N") + '_';

        std::ostringstream tail;

        tail
            << '_' << picture()->widthInLumaSamples
            << 'x' << picture()->heightInLumaSamples
            << '_' <<  getName(picture()->chromaFormatIdc)
            << "p" << (8 < bppY || 8 < bppC ? "l_" : "_") << std::max(bppY, bppC)
            << '_' << 'Y' << bppY << 'C' << bppC
            << ".yuv";

        for(auto id : EnumRange<PelLayerId>())
        {
            std::ofstream file(
                    prefix + head + getName(id) + tail.str(),
                    std::ofstream::binary);

            picture()->writeTo(file, id);
        }
    }
    else if(PictureOutput::Continuous == pictureOutput)
    {
        for(auto id : EnumRange<PelLayerId>())
        {
            std::ofstream file(
                    prefix + getName(id) + ".yuv",
                    std::ofstream::binary | std::ofstream::app);

            picture()->writeTo(file, id);
        }
    }
}
/*----------------------------------------------------------------------------*/
void AccessUnit::onDPB()
{
    using namespace Syntax;

    typedef NalUnit NU;
    typedef NalUnitHeader NUH;
    typedef SEI::DecodedPictureHash DPH;

     const auto toDPH =
         [](const SeiRbsp &rbsp)
         {
             for(const auto &msg : rbsp.getSubtreeList<SeiMessage>())
             {
                 const auto payload = msg->getSubtree<SEI::Payload>();

                 if(
                         SEI::PayloadId::decoded_picture_hash ==
                         *payload->get<SEI::Payload::PayloadType>())
                 {
                     return Ptr<DPH>{payload->getSubtree<DPH>()};
                 }
             }

             return Ptr<DPH>{};
         };

    if(m_inDPB && !m_decoder.isMasked(Decoder::Processes::ProcessId::DecodedPictureHash))
    {
        auto verified = false;

        /* verify SEI checksum, if present */
        for(auto i = list.crbegin(); i != list.crend(); ++i)
        {
            const auto nu = (*i)->getSubtree<NU>();
            const auto nuh = nu->getSubtree<NUH>();

            if(NalUnitType::SUFFIX_SEI_NUT == *nuh->get<NUH::NalUnitType>())
            {
                const auto dph = toDPH(*nu->getSubtree<SeiRbsp>());

                if(dph)
                {
                    const auto chromaFormatIdc = picture()->chromaFormatIdc;
                    const auto isMD5 = DPH::HashType::MD5 == dph->get<DPH::HashType>()->type();
                    const auto isCRC = DPH::HashType::CRC == dph->get<DPH::HashType>()->type();
                    const auto isChecksum = DPH::HashType::Checksum == dph->get<DPH::HashType>()->type();

                    runtime_assert(isMD5 || isCRC || isChecksum);

                    std::cout << "[" << picture()->decodingNo << "]\tDPH ";

                    if(isMD5)
                    {
                        std::cout << "MD5:";
                    }
                    else if(isCRC)
                    {
                        std::cout << "CRC:";
                    }
                    else if(isChecksum)
                    {
                        std::cout << "Checksum:";
                    }

                    for(auto plane : EnumRange<Plane>())
                    {
                        if(!isPresent(plane, chromaFormatIdc))
                        {
                            continue;
                        }

                        std::cout << ' ' << getName(plane);

                        if(
                                isMD5 && (*dph->get<DPH::PictureMD5>())[plane] != picture()->md5Hash(plane)
                                || isCRC && (*dph->get<DPH::PictureCRC>())[plane] != picture()->crcHash(plane)
                                || isChecksum && (*dph->get<DPH::PictureChecksum>())[plane] != picture()->checksumHash(plane))

                        {
                            std::cout << " mismatch";
#ifdef ABORT_ON_SEI_HASH_MISMATCH
                            runtime_assert(false);
#endif
                        }
                    }

                    verified = true;
                    std::cout << '\n';
                }
            }

        }

        if(!verified)
        {
            std::cout << "[" << picture()->decodingNo << "]\tDPH  warning checksum missing\n";
        }

    }

    m_inDPB = !m_inDPB;
}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Structure */
