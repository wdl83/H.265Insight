#ifndef HEVC_Syntax_SEI_DecodedPictureHash_h
#define HEVC_Syntax_SEI_DecodedPictureHash_h

#include <Syntax/Syntax.h>
#include <MD5Hasher.h>

namespace HEVC { namespace Syntax { namespace SEI { namespace DecodedPictureHashContent {
/*----------------------------------------------------------------------------*/
struct HashType:
    public Embedded,
    public VLD::FixedUInt<8, uint8_t>
{
    static const auto Id = ElementId::sei_hash_type;

    enum
    {
        MD5, CRC, Checksum
    };

    int type() const
    {
        return getValue();
    }
};

class PictureMD5:
    public Embedded,
    public VLD::Bits8
{
    std::array<MD5Hasher::ValueType, EnumRange<Plane>::length()> m_md5Hash;
public:
    static const auto Id = ElementId::sei_picture_md5;

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
            ChromaFormatIdc chromaFormatIdc)
    {
        for(auto plane : EnumRange<Plane>())
        {
            if(!isPresent(plane, chromaFormatIdc))
            {
                continue;
            }

            for(auto i = 0; i < 16; ++i)
            {
                getFrom(streamAccessLayer, decoder, *this);
                m_md5Hash[int(plane)][i] = getValue();
            }
        }
    }

    MD5Hasher::ValueType operator[] (Plane plane) const
    {
        return m_md5Hash[int(plane)];
    }

    void toStr(std::ostream &os) const
    {
        for(auto plane : EnumRange<Plane>())
        {
            os << ' ' << getName(plane) << ' ';
            (*this)[plane].toStr(os);
        }
    }
};

class PictureCRC:
    public Embedded,
    public VLD::FixedUInt<16, uint16_t>
{
    std::array<uint16_t, EnumRange<Plane>::length()> m_crc;
public:
    static const auto Id = ElementId::sei_picture_crc;

    uint16_t operator[] (Plane plane) const
    {
        return m_crc[int(plane)];
    }

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
            ChromaFormatIdc chromaFormatIdc)
    {
        for(auto plane : EnumRange<Plane>())
        {
            if(!isPresent(plane, chromaFormatIdc))
            {
                continue;
            }

            getFrom(streamAccessLayer, decoder, *this);
            m_crc[int(plane)] = getValue();
        }
    }

    void toStr(std::ostream &os) const
    {
        for(auto plane : EnumRange<Plane>())
        {
            os << ' ' << getName(plane) << ' ';
            os << std::hex << (*this)[plane];
        }
    }
};

class PictureChecksum:
    public Embedded,
    public VLD::FixedUInt<32, uint32_t>
{
    std::array<uint32_t, EnumRange<Plane>::length()> m_checksum;
public:
    static const auto Id = ElementId::sei_picture_checksum;

    uint32_t operator[] (Plane plane) const
    {
        return m_checksum[int(plane)];
    }

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
            ChromaFormatIdc chromaFormatIdc)
    {
        for(auto plane : EnumRange<Plane>())
        {
            if(!isPresent(plane, chromaFormatIdc))
            {
                continue;
            }

            getFrom(streamAccessLayer, decoder, *this);
            m_checksum[int(plane)] = getValue();
        }
    }

    void toStr(std::ostream &os) const
    {
        for(auto plane : EnumRange<Plane>())
        {
            os << ' ' << getName(plane) << ' ';
            os << std::hex << (*this)[plane];
        }
    }
};
/*----------------------------------------------------------------------------*/
} /* DecodedPictureHashContent */

struct DecodedPictureHash:
    public EmbeddedAggregator<
        DecodedPictureHashContent::HashType,
        DecodedPictureHashContent::PictureMD5,
        DecodedPictureHashContent::PictureCRC,
        DecodedPictureHashContent::PictureChecksum>
{
    typedef DecodedPictureHashContent::HashType HashType;
    typedef DecodedPictureHashContent::PictureMD5 PictureMD5;
    typedef DecodedPictureHashContent::PictureCRC PictureCRC;
    typedef DecodedPictureHashContent::PictureChecksum PictureChecksum;

    static const auto Id = ElementId::sei_decoded_picture_hash;

    void onParse(StreamAccessLayer &, Decoder::State &);
};
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Syntax::SEI */

#endif /* HEVC_Syntax_SEI_DecodedPictureHash_h */
