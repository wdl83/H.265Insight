#ifndef BitArray_h
#define BitArray_h

#include <algorithm>
#include <cstdint>
#include <type_traits>
#include <limits>
/**/
#include <BitIndex.h>
#include <utils.h>
#include <runtime_assert.h>

/*----------------------------------------------------------------------------*/
template <int n>
class BitArray
{
    static_assert(
            0 < n,
            "BitArray: invalid size in bits n.");
    static_assert(
            std::numeric_limits<int>::max() > n,
            "BitArray: size in bits n unsupported.");
public:
    static const int sizeInBits = n;
    /* rounded up to byte boundary */
    static const int sizeInOctets = n / 8 + (0 != n % 8);
    static const int sizeInBytes = sizeInOctets;
private:
    /*------------------------------------------------------------------------*/
    class ConstBitProxy
    {
        friend class BitArray;

        const uint8_t *const m_octet;
        const uint8_t m_mask;
    protected:
        ConstBitProxy(const uint8_t *octet, int offset):
            m_octet(octet), m_mask(1 << (7 - (offset & 7)))
        {}
    public:
        ConstBitProxy(const ConstBitProxy &) = default;

        inline operator bool () const noexcept
        {
            return *m_octet & m_mask;
        }
    };

    class BitProxy
    {
        friend class BitArray;

        uint8_t *const m_octet;
        const uint8_t m_mask;
    protected:
        BitProxy(uint8_t *octet, int offset):
            m_octet(octet), m_mask(1 << (7 - (offset & 7)))
        {}
    public:
        BitProxy(const BitProxy &) = default;

        inline operator bool () const noexcept
        {
            return *m_octet & m_mask;
        }

        const BitProxy &operator=(const BitProxy &) = delete;
        const BitProxy &operator=(BitProxy &&) = delete;

        inline const BitProxy &operator= (bool value) noexcept
        {
            if(value)
            {
                *m_octet |= m_mask;
            }
            else
            {
                *m_octet &= ~m_mask;
            }

            return *this;
        }
    };
    /*------------------------------------------------------------------------*/
    uint8_t m_octets[sizeInOctets];
public:
    typedef bool ValueType;
    typedef BitProxy Reference;
    typedef ConstBitProxy ConstReference;

    BitArray()
    {
        std::fill_n(m_octets, sizeInOctets, 0);
    }

    BitArray(bool value)
    {
        fill(value);
    }

    BitArray(std::initializer_list<bool> iLst):
        BitArray()
    {
        auto begin = iLst.begin();

        for(BitIndex i{0}; i < iLst.size() && i < sizeInBits; ++i, ++begin)
        {
            (*this)[i] = *begin;
        }
    }

    BitArray(std::initializer_list<int> iLst):
        BitArray()
    {
        auto begin = iLst.begin();

        for(BitIndex i{0}; i < iLst.size() && i < sizeInBits; ++i, ++begin)
        {
            (*this)[i] = *begin;
        }
    }

    void fill(bool value)
    {
        std::fill_n(m_octets, lengthOf(m_octets), value ? 0xFF : 0x00);
    }

    inline
    bool operator[] (BitIndex i) const
    {
        bdryCheck(sizeInBits > i.index());
        return ConstBitProxy{&m_octets[i.octetNo()], i.bitNo()};
    }

    inline
    BitProxy operator[] (BitIndex i)
    {
        bdryCheck(sizeInBits > i.index());
        return {&m_octets[i.octetNo()], i.bitNo()};
    }

    BitArray &operator|= (const BitArray &other)
    {
        for(int i = 0; i < sizeInBytes; ++i)
        {
            m_octets[i] |= other.m_octets[i];
        }

        return *this;
    }

    BitArray &operator&= (const BitArray &other)
    {
        for(int i = 0; i < sizeInBytes; ++i)
        {
            m_octets[i] &= other.m_octets[i];
        }

        return *this;
    }
};

template <int n>
BitArray<n> operator| (const BitArray<n> &x, const BitArray<n> &y)
{
    return BitArray<n>(x) |= y;
}

template <int n>
BitArray<n> operator& (const BitArray<n> &x, const BitArray<n> &y)
{
    return BitArray<n>(x) &= y;
}
/*----------------------------------------------------------------------------*/

#endif /* BitArray_h */
