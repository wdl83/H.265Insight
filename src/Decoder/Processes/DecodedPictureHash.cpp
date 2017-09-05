#include <Decoder/Processes/DecodedPictureHash.h>
#include <Structure/Picture.h>
#include <MD5Hasher.h>
#include <CRCHasher.h>
#include <ChecksumHasher.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
void DecodedPictureHash::exec(
        State &,
        Ptr<Structure::Picture> picture)
{
    /* calculate MD5 (as defined by RFC 1321), CRC, checksum
     * of each color plane */
    for(auto plane : EnumRange<Plane>())
    {
        const auto width = picture->widthInPels(toComponent(plane));
        const auto height = picture->heightInPels(toComponent(plane));
        const auto &src = picture->pelBuffer(PelLayerId::Decoded, plane);
        MD5Hasher md5Hasher;
        CRCHasher crcHasher;
        ChecksumHasher checksumHasher;

        for(auto y = 0_pel; y < height; ++y)
        {
            for(auto x = 0_pel; x < width; ++x)
            {
                const auto lsb = src[{x, y}] & 0xFF;

                md5Hasher.calc(lsb);
                crcHasher.calc(lsb);
                checksumHasher.calc(toUnderlying(x), toUnderlying(y), lsb);

                if(8 < picture->bitDepth(plane))
                {
                    const auto msb = src[{x, y}] >> 8;

                    md5Hasher.calc(msb);
                    crcHasher.calc(msb);
                    checksumHasher.calc(toUnderlying(x), toUnderlying(y), msb);
                }
            }
        }

        crcHasher.calc(0);
        crcHasher.calc(0);

        picture->md5Hash(plane, md5Hasher.value());
        picture->crcHash(plane, crcHasher.value());
        picture->checksumHash(plane, checksumHasher.value());
    }
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */
