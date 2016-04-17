#ifndef Coord_h
#define Coord_h

/* STDC++ */
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <sstream>

template <typename T>
class Coord
{
public:
    typedef T ValueType;
private:
    ValueType m_x;
    ValueType m_y;
public:
    Coord()
    {}

    constexpr Coord(ValueType xValue, ValueType yValue):
        m_x{xValue}, m_y{yValue}
    {}

    constexpr Coord(const Coord &) = default;
    Coord &operator= (const Coord &) = default;

    bool operator== (const Coord &other) const
    {
        return m_x == other.m_x && m_y == other.m_y;
    }

    bool operator!= (const Coord &other) const
    {
        return !(*this == other);
    }

    ValueType x() const
    {
        return m_x;
    }

    void x(ValueType xValue)
    {
        m_x = xValue;
    }

    ValueType y() const
    {
        return m_y;
    }

    void y(ValueType yValue)
    {
        m_y = yValue;
    }

    ValueType getAddr(ValueType stride) const
    {
        return y() * stride + x();
    }

    Coord &operator-= (const Coord &other)
    {
        m_x -= other.m_x;
        m_y -= other.m_y;

        return *this;
    }

    Coord &operator+= (const Coord &other)
    {
        m_x += other.m_x;
        m_y += other.m_y;

        return *this;
    }

    bool operator< (const Coord &other) const
    {
        return m_y < other.m_y || m_y == other.m_y && m_x < other.m_x;
    }

    bool operator<= (const Coord &other) const
    {
        return (*this) < other || (*this) == other;
    }

    bool operator> (const Coord &other) const
    {
        return m_y > other.m_y || m_y == other.m_y && m_x > other.m_x;
    }

    bool operator>= (const Coord &other) const
    {
        return (*this) > other || (*this) == other;
    }

    template <typename U>
    Coord &operator*= (U i)
    {
        m_x *= i;
        m_y *= i;
        return *this;
    }

    template <typename U>
    Coord operator* (U i) const
    {
        return Coord{*this} *= i;
    }

    template <typename U>
    Coord &operator/= (U i)
    {
        m_x /= i;
        m_y /= i;
        return *this;
    }

    template <typename U>
    Coord &operator%= (U i)
    {
        m_x %= i;
        m_y %= i;
        return *this;
    }

    template <typename U>
    Coord operator/ (U i) const
    {
        return Coord{*this} /= i;
    }

    template <typename U>
    Coord operator% (U i) const
    {
        return Coord{*this} %= i;
    }

    friend
    Coord operator- (const Coord &arg1, const Coord &arg2)
    {
        return Coord{arg1} -= arg2;
    }

    friend
    Coord operator+ (const Coord &arg1, const Coord &arg2)
    {
        return Coord{arg1} += arg2;
    }

    friend
    std::ostream &operator<< (std::ostream &os, const Coord &coord)
    {
        os << '(' << std::dec << coord.x() << ", " << std::dec << coord.y() << ')';
        return os;
    }
};
/*----------------------------------------------------------------------------*/

#endif /* Coord_h */
