#include <Decoder/Processes/DecodedPictureHash.h>
#include <Structure/Picture.h>
#include <MD5Hasher.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
void DecodedPictureHash::exec(
        State &,
        Ptr<Structure::Picture> picture)
{
    const auto calcCRC =
        [](uint16_t crc, uint8_t data)
        {
            for(auto i = 0; i < 8; ++i)
            {
                const auto v = (data >> (7 - i)) & 1;
                crc =
                    (((crc << 1) + v) & 0xFFFF)
                    ^ (((crc >> 15) & 1) * 0x1021);
            }

            return crc;
        };

    const auto calcChecksum =
        [](uint32_t checksum, int x, int y, uint8_t data)
        {
            const auto xorMask = (x & 0xFF) ^ (y & 0xFF) ^ (x >> 8) ^ (y >> 8);
            return (checksum + ((data & 0xFF) ^ xorMask)) & 0xFFFFFFFF;
        };

    /* calculate MD5 (as defined by RFC 1321), CRC, checksum
     * of each color plane */
    for(auto plane : EnumRange<Plane>())
    {
        const auto width = picture->widthInPels(toComponent(plane));
        const auto height = picture->heightInPels(toComponent(plane));
        const auto &src = picture->pelBuffer(PelLayerId::Decoded, plane);
        MD5Hasher hasher;
        uint16_t crc = 0xFFFF;
        uint32_t checksum = 0;

        for(auto y = 0_pel; y < height; ++y)
        {
            for(auto x = 0_pel; x < width; ++x)
            {
                const auto lsb = src[{x, y}] & 0xFF;

                hasher.calc(lsb);
                crc = calcCRC(crc, lsb);
                checksum = calcChecksum(checksum, toUnderlying(x), toUnderlying(y), lsb);

                if(8 < picture->bitDepth(plane))
                {
                    const auto msb = src[{x, y}] >> 8;

                    hasher.calc(msb);
                    crc = calcCRC(crc, msb);
                    checksum = calcChecksum(checksum, toUnderlying(x), toUnderlying(y), msb);
                }
            }
        }

        crc = calcCRC(crc, 0);
        crc = calcCRC(crc, 0);

        picture->md5Hash(plane, hasher.value());
        picture->crcHash(plane, crc);
        picture->checksumHash(plane, checksum);
    }
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */
