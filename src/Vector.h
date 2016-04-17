#ifndef Vector_h
#define Vector_h

#include <array>
#include <initializer_list>
#include <algorithm>
#include <runtime_assert.h>

template <typename T, int n>
class Vector
{
public:
    typedef T ValueType;
    static const auto size = n;
private:
    std::array<ValueType, size> m_v;
public:
    Vector()
    {
        std::fill(std::begin(m_v), std::end(m_v), ValueType(0));
    }

    Vector(std::initializer_list<ValueType> l)
    {
        bdryCheck(l.size() == m_v.size());
        std::copy(std::begin(l), std::end(l), std::begin(m_v));
    }

    T operator[] (int i) const
    {
        bdryCheck(size > i);
        return m_v[i];
    }

    T &operator[] (int i)
    {
        bdryCheck(size > i);
        return m_v[i];
    }
};

#endif /* Vector_h */
