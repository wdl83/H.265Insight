#ifndef BitIndex_h
#define BitIndex_h

#include <limits>
#include <runtime_assert.h>

/*----------------------------------------------------------------------------*/
class BitIndex
{
public:
    typedef unsigned int ValueType;
private:
    /* WARNING: ENDIAN dependent code */
    struct Bits
    {
        ValueType bitNo: 3;
        ValueType octetNo: sizeof(ValueType) * 8 - 3;
    };

    union Cntr
    {
        Bits m_bits;
        ValueType m_value;
    };

    static_assert(
            sizeof(Cntr) == sizeof(ValueType),
            "BitIndex: unexpected Cntr size.");

    Cntr m_cntr;
public:
    BitIndex()
    {
        m_cntr.m_value = 0;
    }

    BitIndex(int i)
    {
        bdryCheck(0 <= i);
        bdryCheck(std::numeric_limits<ValueType>::max() > ValueType(i));
        m_cntr.m_value = i;
    }

    int octetNo() const
    {
        return m_cntr.m_bits.octetNo;
    }

    int bitNo() const
    {
        return m_cntr.m_bits.bitNo;
    }

    int index() const
    {
        return m_cntr.m_value;
    }

    BitIndex &operator++ ()
    {
        ++m_cntr.m_value;
        return *this;
    }

    BitIndex operator++ (int)
    {
        const BitIndex i{*this};

        ++(*this);
        return i;
    }

    BitIndex &operator-- ()
    {
        --m_cntr.m_value;
        return *this;
    }

    BitIndex operator-- (int)
    {
        const BitIndex  i{*this};

        --(*this);
        return i;
    }

    BitIndex &operator+= (int i)
    {
        while(i)
        {
            ++(*this), --i;
        }

        return *this;
    }

    BitIndex &operator-= (int i)
    {
        while(i)
        {
            --(*this), --i;
        }

        return *this;
    }

    BitIndex operator+ (int i) const
    {
        return BitIndex{*this} += i;
    }

    BitIndex operator- (int i) const
    {
        return BitIndex{*this} -= i;
    }

    int operator- (const BitIndex &other) const
    {
        return m_cntr.m_value - other.m_cntr.m_value;
    }

    bool operator== (const BitIndex &other) const
    {
        return m_cntr.m_value == other.m_cntr.m_value;
    }

    bool operator!= (const BitIndex &other) const
    {
        return !(*this == other);
    }

    bool operator< (const BitIndex &other) const
    {
        return m_cntr.m_value < other.m_cntr.m_value;
    }

    bool operator<= (const BitIndex &other) const
    {
        return m_cntr.m_value <= other.m_cntr.m_value;
    }

    bool operator> (const BitIndex &other) const
    {
        return m_cntr.m_value > other.m_cntr.m_value;
    }

    bool operator>= (const BitIndex &other) const
    {
        return m_cntr.m_value >= other.m_cntr.m_value;
    }
};
/*----------------------------------------------------------------------------*/

#endif /* BitIndex_h */
