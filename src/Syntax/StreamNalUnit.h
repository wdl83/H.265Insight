#ifndef HEVC_Syntax_StreamNalUnit_h
#define HEVC_Syntax_StreamNalUnit_h

/* STDC++ */
#include <chrono>
/* HEVC */
#include <Syntax/Syntax.h>
#include <Syntax/NalUnit.h>

namespace HEVC { namespace Syntax { namespace StreamNalUnitContent {
/*----------------------------------------------------------------------------*/
class LeadingZero8Bits:
    public Embedded,
    public VLD::FixedPattern<0, 0, 0, 0, 0, 0, 0, 0 /* 0x00 */>
{
public:
    static const auto Id = ElementId::leading_zero_8bits;
};
/*----------------------------------------------------------------------------*/
/* trailing_zero_8bits should use fixed pattern descriptor but because
 * it requires reading from tail of byte stream it is implemented
 * as special case */
class TrailingZero8Bits:
    public Embedded,
    public VLD::TagVLD,
    public Descriptor,
    public StoreByValue<uint8_t>
{
public:
    static const auto Id = ElementId::trailing_zero_8bits;
    static const auto descriptorId = DescriptorId::FixedPattern;

    TrailingZero8Bits():
        StoreByValue(0)
    {}

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder)
    {
        getFrom(streamAccessLayer, decoder, *this);
    }

    /* Descriptor interface */
    void onGetFrom(StreamAccessLayer &streamAccessLayer)
    {
        if(0 != streamAccessLayer.getTailByte())
        {
            syntaxCheck(false);
        }
    }
};
/*----------------------------------------------------------------------------*/
class ZeroByte:
    public Embedded,
    public VLD::FixedPattern<0, 0, 0, 0, 0, 0, 0, 0 /* 0x00 */>
{
public:
    static const auto Id = ElementId::zero_byte;
};
/*----------------------------------------------------------------------------*/
class StartCodePrefixOne3Bytes:
    public Embedded,
    public VLD::FixedPattern
        <
            0, 0, 0, 0, 0, 0, 0, 0, /* 0x00 */
            0, 0, 0, 0, 0, 0, 0, 0, /* 0x00 */
            0, 0, 0, 0, 0, 0, 0, 1 /* 0x01 */
        >
{
public:
    static const auto Id = ElementId::start_code_prefix_one_3bytes;
};
/*----------------------------------------------------------------------------*/
class NumBytesInNalUnit : public Embedded
{
private:
    const size_t m_nalUnitSizeInBytes;
public:
    static const auto Id = ElementId::NumBytesInNALunit;

    NumBytesInNalUnit(size_t nalUnitSizeInBytes):
        m_nalUnitSizeInBytes(nalUnitSizeInBytes)
    {}

    operator size_t () const
    {
        return m_nalUnitSizeInBytes;
    }
};
/*----------------------------------------------------------------------------*/
} /* namespace StreamNalUnitContent */

/*----------------------------------------------------------------------------*/
class StreamNalUnit:
    public EmbeddedAggregator<
        StreamNalUnitContent::LeadingZero8Bits,
        StreamNalUnitContent::TrailingZero8Bits,
        StreamNalUnitContent::ZeroByte,
        StreamNalUnitContent::StartCodePrefixOne3Bytes,
        StreamNalUnitContent::NumBytesInNalUnit>,
    public SubtreeAggregator<
        NalUnit>
{
    StreamAccessLayer m_streamAccessLayer;
public:
    struct ElementStats
    {
        typedef std::chrono::nanoseconds Duration;
        Duration duration;
        uint64_t count;

        ElementStats():
            duration{0},
            count{0}
        {}
    };

    struct Stats
    {
        /* total time required to decode current access unit */
        std::chrono::high_resolution_clock::time_point beginTime, endTime;
        std::array<
            ElementStats,
            EnumRange<ElementId>::length()> element;

        const ElementStats &operator[] (ElementId id) const
        {
            return element[int(id)];
        }

        ElementStats &operator[] (ElementId id)
        {
            return element[int(id)];
        }

        std::chrono::nanoseconds duration() const
        {
            return
                std::chrono::duration_cast<ElementStats::Duration>(
                        endTime - beginTime);
        }

        Stats():
            beginTime{std::chrono::high_resolution_clock::now()},
            endTime{beginTime}
        {}
    };

    Stats stats;

    static const auto Id = ElementId::byte_stream_nal_unit;

    typedef StreamNalUnitContent::LeadingZero8Bits LeadingZero8Bits;
    typedef StreamNalUnitContent::TrailingZero8Bits TrailingZero8Bits;
    typedef StreamNalUnitContent::ZeroByte ZeroByte;
    typedef StreamNalUnitContent::StartCodePrefixOne3Bytes StartCodePrefixOne3Bytes;
    typedef StreamNalUnitContent::NumBytesInNalUnit NumBytesInNalUnit;

    StreamNalUnit(const VLA<uint8_t> &bytes):
        m_streamAccessLayer{bytes}
    {}

    StreamNalUnit(VLA<uint8_t> &&bytes):
        m_streamAccessLayer{std::move(bytes)}
    {}

    StreamAccessLayer &toStreamAccessLayer()
    {
        return m_streamAccessLayer;
    }

    void onParse(StreamAccessLayer &, Decoder::State &);
private:
    /* LeadingZero8Bits, ZeroByte and StartCodePrefixOne3Byte */
    void parseNalUnitPrefix(StreamAccessLayer &, Decoder::State &);
    /* TrailingZero8Bits */
    void parseNalUnitSuffix(StreamAccessLayer &, Decoder::State &);
};

inline
const NalUnit &toNalUnit(const StreamNalUnit &snu)
{
    return *snu.getSubtree<NalUnit>();
}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */

#endif /* HEVC_Syntax_StreamNalUnit_h */
