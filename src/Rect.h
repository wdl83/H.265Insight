#ifndef Rect_h
#define Rect_h

#include <units.h>
#include <Coord.h>
#include <runtime_assert.h>

/*----------------------------------------------------------------------------*/
enum class VPos: int
{
    Top = 0x01,
    Bottom = 0x02
};

enum class HPos: int
{
    Left = 0x04,
    Right = 0x08
};

enum class Pos: int
{
    Top = castToUnderlying(VPos::Top),
    Bottom = castToUnderlying(VPos::Bottom),
    Left = castToUnderlying(HPos::Left),
    Right = castToUnderlying(HPos::Right),
    TopLeft = castToUnderlying(VPos::Top) | castToUnderlying(HPos::Left),
    TopRight = castToUnderlying(VPos::Top) | castToUnderlying(HPos::Right),
    BottomLeft = castToUnderlying(VPos::Bottom) | castToUnderlying(HPos::Left),
    BottomRight = castToUnderlying(VPos::Bottom) | castToUnderlying(HPos::Right)
};

constexpr Pos operator| (const HPos hPos, const VPos vPos)
{
    return static_cast<Pos>(castToUnderlying(hPos) | castToUnderlying(vPos));
}

constexpr Pos operator| (const VPos vPos, const HPos hPos)
{
    return hPos | vPos;
}

constexpr bool operator& (const Pos pos, const HPos hPos)
{
    return 0 != (castToUnderlying(pos) & castToUnderlying(hPos));
}

constexpr bool operator& (const HPos hPos, const Pos pos)
{
    return pos & hPos;
}

constexpr bool operator& (const Pos pos, const VPos vPos)
{
    return 0 != (castToUnderlying(pos) & castToUnderlying(vPos));
}

constexpr bool operator& (const VPos vPos, const Pos pos)
{
    return pos & vPos;
}
/*----------------------------------------------------------------------------*/
template <typename T>
class Rect
{
public:
    typedef Coord<T> CoordType;
    typedef typename CoordType::ValueType ValueType;
private:
    CoordType m_topLeft;
    ValueType m_width, m_height;
public:
    Rect(CoordType topLeft, ValueType width, ValueType height):
        m_topLeft(topLeft), m_width(width), m_height(height)
    {
        bdryCheck(ValueType{1} < m_width);
        bdryCheck(ValueType{1} < m_height);
    }

    Rect(CoordType topLeft, ValueType side):
        Rect{topLeft, side, side}
    {}

    Rect(CoordType topLeft, CoordType bottomRight):
        Rect{
                topLeft,
                bottomRight.x() - topLeft.x() + ValueType{1},
                bottomRight.y() - topLeft.y() + ValueType{1},
            }
    {}

    bool operator == (const Rect<T> &rhs) const
    {
        return
            m_topLeft == rhs.m_topLeft
            && m_width == rhs.m_width
            && m_height == rhs.m_height;
    }

    bool encloses(CoordType coord) const
    {
        return
            m_topLeft.x() <= coord.x()
            && m_topLeft.x() + m_width > coord.x()
            && m_topLeft.y() <= coord.y()
            && m_topLeft.y() + m_height > coord.y();
    }

    ValueType width() const
    {
        return m_width;
    }

    ValueType height() const
    {
        return m_height;
    }

    CoordType topLeft() const
    {
        return m_topLeft;
    }

    CoordType topRight() const
    {
        return
        {
            m_topLeft.x() + m_width - ValueType{1},
            m_topLeft.y()
        };
    }

    CoordType bottomLeft() const
    {
        return
        {
            m_topLeft.x(),
            m_topLeft.y() + m_height - ValueType{1}
        };
    }

    CoordType bottomRight() const
    {
        return
        {
            m_topLeft.x() + m_width - ValueType{1},
            m_topLeft.y() + m_height - ValueType{1}
        };
    }
};

template <typename T> Rect<T> makeRect(
        typename Rect<T>::CoordType coord,
        T width, T height)
{
    return Rect<T>{coord, width, height};
}

template <typename T> Rect<T> makeRect(
        typename Rect<T>::CoordType coord,
        T side)
{
    return Rect<T>{coord, side};
}

template <typename T>
Coord<T> clip(Rect<T> a, Coord<T> b)
{
    return
    {
        b.x() < a.topLeft().x()
        ? a.topLeft().x()
        : (b.x() > a.topRight().x() ? a.topRight().x() : b.x()),
        b.y() < a.topLeft().y()
        ? a.topLeft().y()
        : (b.y() > a.bottomLeft().y() ? a.bottomLeft().y() : b.y())
    };
}
/*----------------------------------------------------------------------------*/

#endif /* Rect_h */
