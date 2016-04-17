#ifndef Pair_h
#define Pair_h

/* STDC++ */
#include <array>

/*----------------------------------------------------------------------------*/
template<
    typename T,
    typename I = int>
class Pair
{
public:
    typedef T ValueType;
    typedef I SizeType;
private:
    std::array<ValueType, 2> m_pair;
public:
    Pair()
    {}

    Pair(const T &x, const T &y):
        m_pair{{x, y}}
    {}

    Pair(const Pair &) = default;
    Pair& operator= (const Pair &) = default;

    const ValueType &operator[](SizeType i) const
    {
        return m_pair[int(i)];
    }

    ValueType &operator[](SizeType i)
    {
        return m_pair[int(i)];
    }

    constexpr
    static
    SizeType size()
    {
        return SizeType(2);
    }
};
/*----------------------------------------------------------------------------*/

#endif /* Pair_h */
