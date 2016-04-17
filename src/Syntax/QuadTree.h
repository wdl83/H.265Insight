#ifndef HEVC_QuadTree_h
#define HEVC_QuadTree_h

/* STDC++ */
#include <cmath>
#include <functional>
/* HEVC */
#include <Syntax/Syntax.h>

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
template <typename T>
class QuadTree
{
    /* leaves of element with index n are: 4n+1, 4n + 2, 4n + 3, 4n + 4 */
public:
    typedef Handle<T> ValueType;
    typedef VLA<ValueType> Container;
    typedef typename Container::SizeType Index;
    typedef typename Container::SizeType SizeType;
private:
    /* get max quad tree capacity at depth
     * (returned capacity is at depth NOT to given depth) */
    static
    SizeType capacity(Log2 depth)
    {
        return toInt(depth) * toInt(depth);
    }

    /* get max quad tree capacity defined by depth range [begin, end) */
    static
    SizeType capacity(Range<Log2> depth)
    {
        auto begin = depth.begin();
        const auto end = depth.end();
        SizeType c = 0;

        while(end != begin)
        {
            c += capacity(begin);
            ++begin;
        }

        return c;
    }

    static
    constexpr
    Index toIndex(const Pos pos)
    {
        return
            Pos::TopLeft == pos
            ? 1
            : (
                    Pos::TopRight == pos
                    ? 2
                    : (
                        Pos::BottomLeft == pos
                        ? 3
                        : /* Pos::BottomRight */ 4));
    }

    const Range<Log2> m_side;
    Container m_quadTree;
    SizeType m_size;
public:
    QuadTree(Range<Log2> s):
        m_side{s},
        m_quadTree(capacity()),
        m_size(0)
    {}

    template <typename ...A_n>
    Ptr<T> append(PelCoord at, A_n &&... a_n)
    {
        syntaxCheck(side() > at.x());
        syntaxCheck(side() > at.y());

        const Index i = m_quadTree[0] ? toEmptyIndex(at, 0_log2, 0) : 0;

        m_quadTree[i].construct(std::forward<A_n>(a_n)...);
        ++m_size;
        return Ptr<T>{m_quadTree[i]};
    }

    /* get quad tree dimention (side length) at given depth */
    Pel side(Log2 depth = 0_log2) const
    {
        syntaxCheck(m_side.length() > depth);
        return toPel(m_side.end() - 1_log2 - depth);
    }

    int capacity() const
    {
        return capacity({0_log2, m_side.length()});
    }

    int size() const
    {
        return m_size;
    }

    bool empty() const
    {
        return 0 == m_size;
    }

    Ptr<T> operator[] (PelCoord at) const
    {
        syntaxCheck(side() > at.x());
        syntaxCheck(side() > at.y());

        const Index i = isRoot(0) ? toIndex(at, 0_log2, 0) : 0;

        return m_quadTree[i] ? Ptr<T>{m_quadTree[i]} : nullptr;
    }

    VLA<Ptr<T>> toListInZ() const
    {
        VLA<Ptr<T>> list(size(), nullptr);
        auto n = 0;

        if(0 < m_quadTree.size() && m_quadTree[0])
        {
            list[n] = Ptr<T>{m_quadTree[0]};
            ++n;
        }

        for(Index i = 0; i < m_quadTree.size(); ++i)
        {
            for(Index j = 1; j <= 4 && isValid(4 * i + j); ++j)
            {
                if(m_quadTree[4 * i + j])
                {
                    list[n] = Ptr<T>{m_quadTree[4 * i + j]};
                    ++n;
                }
            }
        }

        syntaxCheck(size() == n);
        return list;
    }

    void writeTo(std::ostream &os) const
    {
        for(Index i = 0; i < m_quadTree.size(); ++i)
        {
            if(isValid(i * 4 + 1))
            {
                os << '[' << m_quadTree[i] << ':';

                for(Index j = 1; j <= 4 && isValid(4 * i + j); ++j)
                {
                    os << ' ' << m_quadTree[4 * i + j];
                }

                os << ']';
            }
        }
    }
private:
    Index toEmptyIndex(PelCoord at, Log2 d, Index index) const
    {
        const auto sideDiv2 = side(d + 1_log2);
        const auto hPos = at.x() < sideDiv2 ? HPos::Left : HPos::Right;
        const auto vPos = at.y() < sideDiv2 ? VPos::Top : VPos::Bottom;
        const auto pos = hPos | vPos;
        const Index i = index * 4 + toIndex(pos);

        syntaxCheck(m_quadTree.size() > i);

        if(m_quadTree[i])
        {
            return toEmptyIndex(PelCoord{at.x() % sideDiv2, at.y() % sideDiv2}, d + 1_log2, i);
        }
        else
        {
            return i;
        }
    }

    Index toIndex(PelCoord at, Log2 d, Index index) const
    {
        const auto sideDiv2 = side(d + 1_log2);
        const auto hPos = at.x() < sideDiv2 ? HPos::Left : HPos::Right;
        const auto vPos = at.y() < sideDiv2 ? VPos::Top : VPos::Bottom;
        const auto pos = hPos | vPos;
        const Index i = 4 * index + toIndex(pos);

        if(isRoot(i))
        {
            /* search sub-trees */
            return toIndex(PelCoord{at.x() % sideDiv2, at.y() % sideDiv2}, d + 1_log2, i);
        }
        else
        {
            /* if adjacent is not present (level not complete) get its parent  */
            return i ? i : index;
        }
    }

    bool isRoot(Index index) const
    {
        const auto offset = 4 * index;
        const auto topLeft = offset + toIndex(Pos::TopLeft);
        const auto topRight = offset + toIndex(Pos::TopRight);
        const auto bottomLeft = offset + toIndex(Pos::BottomLeft);
        const auto bottomRight = offset + toIndex(Pos::BottomRight);
        const auto size = m_quadTree.size();

        return
            size > topLeft && nullptr != m_quadTree[topLeft]
            || size > topRight && nullptr != m_quadTree[topRight]
            || size > bottomLeft && nullptr != m_quadTree[bottomLeft]
            || size > bottomRight && nullptr != m_quadTree[bottomRight];
    }

    bool isValid(Index i) const
    {
        return m_quadTree.size() > i;
    }
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */

#endif /* HEVC_QuadTree_h */
