#ifndef Range_h
#define Range_h

/* STDC++ */
#include <functional>

/*----------------------------------------------------------------------------*/
template<typename T, typename Less = std::less<T>>
class Range
{
public:
    typedef T Type;
private:
    Type m_begin, m_end;
public:
    Range()
    {}

    Range(const Type &a, const Type &b):
        m_begin(a), m_end(b)
    {}

    Range(const Range &) = default;

    Range &operator= (const Range &) = default;

    const Type &begin() const
    {
        return m_begin;
    }

    const Type &end() const
    {
        return m_end;
    }

    Type length() const
    {
        return m_end - m_begin;
    }

    bool encloses(const Type &v) const
    {
        return !Less()(v, begin()) && Less()(v, end());
    }
};

template <typename T>
inline
Range<T> makeRange(const T &begin, const T &end)
{
    return Range<T>{begin, end};
}

template <typename T>
inline
T clip(const Range<T> &r, const T &v)
{
    return r.begin() > v ? r.begin() : (r.end() < v ? r.end() : v);
}
/*----------------------------------------------------------------------------*/

#endif /* Range_h */
