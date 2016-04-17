#include <Syntax/SEI/DecodedPictureHash.h>
#include <Decoder/State.h>

namespace HEVC { namespace Syntax { namespace SEI {
/*----------------------------------------------------------------------------*/
void DecodedPictureHash::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder)
{
    // last picture in decoding order
    auto hashType = embed<HashType>(*this);
    const auto chromaFormatIdc = decoder.picture()->chromaFormatIdc;

    parse(streamAccessLayer, decoder, *hashType);

    if(HashType::MD5 == hashType->type())
    {
        parse(
                streamAccessLayer, decoder, *embed<PictureMD5>(*this),
                chromaFormatIdc);

        const auto toStr =
            [this](std::ostream &oss)
            {
                oss << "MD5";

                for(auto plane : EnumRange<Plane>())
                {
                    oss << '\n' << getName(plane) << ' ';
                    (*get<PictureMD5>())[plane].toStr(oss);
                }

                oss << '\n';
            };

        log(LogId::SEI, toStr);
    }
    else if(HashType::CRC == hashType->type())
    {
        parse(
                streamAccessLayer, decoder, *embed<PictureCRC>(*this),
                chromaFormatIdc);
    }
    else if(HashType::Checksum == hashType->type())
    {
        parse(
                streamAccessLayer, decoder, *embed<PictureChecksum>(*this),
                chromaFormatIdc);
    }
    else
    {
        /* 04/2013, D.3.19 "Decoded picture hash SEI message semantics"
         * "Decoders shall ignore decoded picture hash SEI messages that contain
         * reserved values of hash_type." */
    }
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Syntax::SEI */

