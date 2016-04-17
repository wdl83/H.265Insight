#ifndef StreamAccessLayer_h
#define StreamAccessLayer_h
/* STDC++ */
#include <array>
#include <string>
#include <ostream>
#include <memory>
#include <iterator>
/* HEVC */
#include <utils.h>
#include <Counter.h>
#include <VLA.h>
#include <runtime_assert.h>

namespace HEVC {
/*----------------------------------------------------------------------------*/
class StreamAccessLayer
{
public:
    typedef VLA<uint8_t> ByteVector;

    class ConstIterator: public std::iterator<std::random_access_iterator_tag, bool>
    {
    private:
        ByteVector::ConstIterator m_byteOffset;
        Counter<uint8_t, 3 /* 2^3 == 8 */ > m_bitOffset;
    public:
        ConstIterator(ByteVector::ConstIterator byteOffset, uint8_t bitOffset = 0):
            m_byteOffset(byteOffset), m_bitOffset(bitOffset)
        {}

        ConstIterator &operator++ ()
        {
            ++m_bitOffset;

            if(0 == m_bitOffset)
            {
                ++m_byteOffset;
            }

            return *this;
        }

        ConstIterator operator++ (int)
        {
            const ConstIterator i{*this};

            ++(*this);
            return i;
        }

        ConstIterator &operator-- ()
        {
            if(0 == m_bitOffset)
            {
                --m_byteOffset;
            }

            --m_bitOffset;

            return *this;
        }

        ConstIterator operator-- (int)
        {
            const ConstIterator  i{*this};

            --(*this);
            return i;
        }

        ConstIterator &operator+= (difference_type i)
        {
            while(0 < i)
            {
                ++(*this);
                --i;
            }

            return *this;
        }

        ConstIterator &operator-= (difference_type i)
        {
            while(0 < i)
            {
                --(*this);
                --i;
            }

            return *this;
        }

        ConstIterator operator+ (difference_type i) const
        {
            return ConstIterator{*this} += i;
        }

        ConstIterator operator- (difference_type i) const
        {
            return ConstIterator{*this} -= i;
        }

        difference_type operator- (const ConstIterator &other) const
        {
            return
                (m_byteOffset - other.m_byteOffset) * 8
                + (m_bitOffset - other.m_bitOffset);
        }

        bool operator* () const
        {
            return 0 != (*m_byteOffset & (1 << (7 - m_bitOffset)));
        }

        bool operator== (const ConstIterator &other) const
        {
            return
                m_byteOffset == other.m_byteOffset
                && m_bitOffset == other.m_bitOffset;
        }

        bool operator!= (const ConstIterator &other) const
        {
            return !(*this == other);
        }

        bool operator< (const ConstIterator &other) const
        {
            return
                m_byteOffset < other.m_byteOffset
                || m_byteOffset == other.m_byteOffset && m_bitOffset < other.m_bitOffset;
        }

        bool operator<= (const ConstIterator &other) const
        {
            return *this < other || *this == other;
        }

        bool operator> (const ConstIterator &other) const
        {
            return
                m_byteOffset > other.m_byteOffset
                || m_byteOffset == other.m_byteOffset && m_bitOffset > other.m_bitOffset;
        }

        bool operator>= (const ConstIterator &other) const
        {
            return *this > other || *this == other;
        }

        bool isByteAligned() const
        {
            return 0 == m_bitOffset;
        }
    };
    /*------------------------------------------------------------------------*/
private:
    ByteVector m_ownPayload;
    const ByteVector &m_payload;
    ConstIterator m_begin;
    ConstIterator m_end;
public:
    explicit
    StreamAccessLayer(ByteVector &&payload):
        m_ownPayload{std::move(payload)},
        m_payload{m_ownPayload},
        m_begin{m_payload.begin()},
        m_end{m_payload.end()}
    {}

    StreamAccessLayer(StreamAccessLayer &&other) = default;

    explicit
    StreamAccessLayer(const ByteVector &payload):
        m_payload{payload},
        m_begin{m_payload.begin()},
        m_end{m_payload.end()}
    {}

    template <typename ByteConstIter>
    explicit
    StreamAccessLayer(ByteConstIter begin, ByteConstIter &end):
        m_ownPayload{begin, end},
        m_payload{m_ownPayload},
        m_begin{m_payload.begin()},
        m_end{m_payload.end()}
    {}

    StreamAccessLayer(const StreamAccessLayer &other) = delete;
    StreamAccessLayer& operator=(const StreamAccessLayer &other) = delete;
    StreamAccessLayer& operator=(const StreamAccessLayer &&other) = delete;

    const ByteVector &getPayload() const
    {
        return m_payload;
    }

    bool isEndOfStream() const
    {
        return m_begin == m_end;
    }

    /* WARNING:
     * All byte-oriented operations will fail if stream is not byte-aligned. */
    bool isByteAligned() const
    {
        return m_begin.isByteAligned();
    }

    int getSizeInBytes() const
    {
        runtime_assert(isByteAligned());
        return (m_end - m_begin) / 8;
    }

    uint8_t getByte()
    {
        const auto value = peekByte();

        m_begin += 8;
        return value;
    }

    uint8_t peekByte() const
    {
        runtime_assert(isByteAligned());
        runtime_assert(m_begin + 8 <= m_end);

        uint8_t value = 0;

        for(uint8_t i = 0; i < 8; ++i)
        {
            value |= *(m_begin + i) << (7 - i);
        }

        return value;
    }

    template <int n>
    std::array<uint8_t, n> getBytes()
    {
        const auto bytes = peekBytes<n>();

        m_begin += n * 8;
        return bytes;
    }

    template <int n>
    std::array<uint8_t, n> peekBytes() const
    {
        runtime_assert(isByteAligned());
        runtime_assert(m_begin + (n * 8) <= m_end);

        std::array<uint8_t, n> bytes;

        for(int i = 0; i < n; ++i)
        {
            bytes[i] = 0;

            for(uint8_t j = 0; j < 8; ++j)
            {
                bytes[i] |= *(m_begin + i * 8 + j) << (7 - j);
            }
        }

        return bytes;
    }

    uint8_t getTailByte()
    {
        const auto value = peekTailByte();

        m_end -= 8;
        return value;
    }

    uint8_t peekTailByte() const
    {
        runtime_assert(isByteAligned());
        runtime_assert(m_begin + 8 <= m_end);

        uint8_t value = 0;

        for(uint8_t i = 0; i < 8; ++i)
        {
            value |= *(m_end - 1 - i) << i;
        }

        return value;
    }

    /* Bit-oriented operations */
    int getSizeInBits() const
    {
        return m_end - m_begin;
    }

    bool getBit()
    {
        runtime_assert(m_begin < m_end);
        return *(m_begin++);
    }

    bool peekBit() const
    {
        runtime_assert(m_begin < m_end);
        return *m_begin;
    }

    bool peekPrevBit() const
    {
        runtime_assert(ConstIterator{m_payload.begin()} < m_begin);
        return *(--ConstIterator{m_begin});
    }


    template <typename T>
    T peekBits(int) const;

    template <typename T>
    T getBits(int n)
    {
        const auto value = peekBits<T>(n);

        m_begin += n;
        return value;
    }

    ConstIterator curr() const
    {
        return m_begin;
    }

    ConstIterator begin() const
    {
        return m_begin;
    }

    ConstIterator end() const
    {
        return m_end;
    }
};
/*----------------------------------------------------------------------------*/
template <>
inline
uint32_t StreamAccessLayer::peekBits<uint32_t>(int n) const
{
    uint32_t value = 0;
    auto i = m_begin;

    runtime_assert(32 >= n);

    while(n)
    {
        value |= *i << (n - 1);
        --n;
        ++i;
    }

    return value;
}
/*----------------------------------------------------------------------------*/
template <>
inline
int32_t StreamAccessLayer::peekBits<int32_t>(int n) const
{
    const auto value = peekBits<uint32_t>(n);
    const auto valuesMaxNum = uint32_t{1} << n;

    return value < valuesMaxNum / 2 ? value : value - (valuesMaxNum + 1);
}
/*----------------------------------------------------------------------------*/
} /* HEVC */

#endif /* StreamAccessLayer_h */
