#ifndef BitMatrix_h
#define BitMatrix_h

/* STDC++ */
#include <algorithm>
#include <cstdint>
#include <type_traits>
#include <limits>
#include <ostream>
/**/
#include <BitArray.h>
#include <Coord.h>
#include <runtime_assert.h>

/*----------------------------------------------------------------------------*/

template <int w, int h>
class BitMatrix
{
public:
    static const int widthInBits = w;
    static const int heightInBits = h;
    static const int sizeInBits = widthInBits * heightInBits;

    typedef BitArray<widthInBits * heightInBits> BitArrayType;
    typedef typename BitArrayType::ValueType ValueType;
    typedef typename BitArrayType::Reference Reference;
    typedef typename BitArrayType::ConstReference ConstReference;
    typedef Coord<int> Pos;
private:
    BitArrayType m_bitArray;
public:
    BitMatrix()
    {}

    BitMatrix(bool value): m_bitArray(value)
    {}

    void fill(bool value)
    {
        m_bitArray.fill(value);
    }

    static BitIndex calcOffset(Pos at)
    {
        return {at.y() * widthInBits + at.x()};
    }

    auto operator[] (BitIndex i) const -> decltype(m_bitArray[i])
    {
        return m_bitArray[i];
    }

    auto operator[] (BitIndex i) -> decltype(m_bitArray[i])
    {
        return m_bitArray[i];
    }

    auto operator[] (Pos at) const
        -> decltype(m_bitArray[calcOffset(at)])
    {
        return m_bitArray[calcOffset(at)];
    }

    auto operator[] (Pos at)
        -> decltype(m_bitArray[calcOffset(at)])
    {
        return m_bitArray[calcOffset(at)];
    }

    BitMatrix &operator|= (const BitMatrix &other)
    {
        m_bitArray |= other.m_bitArray;
        return *this;
    }

    BitMatrix &operator&= (const BitMatrix &other)
    {
        m_bitArray &= other.m_bitArray;
        return *this;
    }

    void toStr(
            std::ostream &os,
            int width = widthInBits,
            int height = heightInBits,
            char trueGlyph = '1', char falseGlyph = '0') const
    {
        for(auto y = 0; y < height; ++y)
        {
            for(auto x = 0; x < width; ++x)
            {
                os
                    << ((*this)[{x, y}] ? trueGlyph: falseGlyph)
                    << (width - 1 == x ? '\n' : ' ');
            }
        }
    }

    friend
    BitMatrix operator| (const BitMatrix &x, const BitMatrix &y)
    {
        return BitMatrix(x) |= y;
    }

    friend
    BitMatrix operator& (const BitMatrix &x, const BitMatrix &y)
    {
        return BitMatrix(x) &= y;
    }
};


/*----------------------------------------------------------------------------*/

#endif /* BitMatrix_h */
