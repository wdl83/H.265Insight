#ifndef Matrix_h
#define Matrix_h

#include <Vector.h>
#include <Coord.h>
#include <runtime_assert.h>

template <typename T, int w, int h>
class Matrix
{
public:
    static const auto width = w;
    static const auto height = h;
    static const auto size = width * height;
    static const auto isSquare = width == height;
    typedef T ValueType;
    typedef Coord<int> Pos;
private:

    static int calcOffset(Pos i)
    {
        bdryCheck(width > i.x());
        bdryCheck(height > i.y());
        return i.y() * width + i.x();
    }

    std::array<ValueType, size> m_m;
public:
    Matrix()
    {
        std::fill(std::begin(m_m), std::end(m_m), ValueType(0));
    }

    Matrix(std::initializer_list<ValueType> l)
    {
        bdryCheck(l.size() == m_m.size());
        std::copy(std::begin(l), std::end(l), std::begin(m_m));
    }

    const T &operator[] (Pos i) const
    {
        return m_m[calcOffset(i)];
    }

    T &operator[] (Pos i)
    {
        return m_m[calcOffset(i)];
    }

    template<typename TT>
    friend
    Vector<TT, h> operator* (const Matrix &m, const Vector<TT, w> &v)
    {
        Vector<TT, h> r;

        for(auto y = 0; y < height; ++y)
        {
            for(auto x = 0; x < width; ++x)
            {
                const auto delta = m[{x, y}] * v[x];

                numericCheck(!overflow(r[y], delta));
                numericCheck(!underflow(r[y], delta));
                r[y] += delta;
            }
        }

        return r;
    }
};

#endif /* Matrix_h */
