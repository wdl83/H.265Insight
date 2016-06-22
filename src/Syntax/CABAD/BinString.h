#ifndef HEVC_Syntax_CABAD_BinString_h
#define HEVC_Syntax_CABAD_BinString_h

/* STDC++ */
#include <cstdint>
#include <initializer_list>
#include <ostream>
/* HEVC */
#include <Syntax/check.h>

namespace HEVC { namespace Syntax { namespace CABAD {
/*----------------------------------------------------------------------------*/
class BinString
{
public:
    typedef uint64_t Underlying;
    static const int capacity = sizeof(Underlying) * 8 - 8;

    union Payload
    {
        /* WARNING: ENDIAN dependent code */
        struct Bits
        {
            Underlying bins : capacity;
            Underlying size : 8;
        } bits;

        Underlying value;

        Payload(): value(0)
        {}
    };

    static_assert(
            sizeof(Payload) == sizeof(Underlying),
            "alignment different then expected.");
private:
    Payload m_payload;
public:
    BinString()
    {}

    BinString(std::initializer_list<bool> l):
        BinString()
    {
        for(auto i : l)
        {
            pushBack(i);
        }
    }

    int size() const
    {
        return m_payload.bits.size;
    }

    bool operator[] (int i) const
    {
        return m_payload.bits.bins & (Underlying(1) << (size() - 1 - i));
    }

    bool back() const
    {
        return (*this)[size() - 1];
    }

    void pushBack(bool value)
    {
        syntaxCheck(capacity >= size());

        m_payload.bits.bins <<= 1;

        if(value)
        {
            m_payload.bits.bins |= Underlying(1);
        }

        ++m_payload.bits.size;
    }

    explicit operator Underlying () const
    {
        return m_payload.bits.bins;
    }

    friend
    bool operator< (const BinString &x, const BinString &y)
    {
        return x.m_payload.value < y.m_payload.value;
    }

    friend
    bool operator== (const BinString &x, const BinString &y)
    {
        return x.m_payload.value == y.m_payload.value;
    }
};

inline
void append(BinString &dst, BinString src)
{
    const auto size = src.size();

    for(int i = 0; i < size; ++i)
    {
        dst.pushBack(src[i]);
    }
}

inline
BinString join(BinString x, BinString y)
{
    const auto size = y.size();
    BinString z{x};

    for(int i = 0; i < size; ++i)
    {
        z.pushBack(y[i]);
    }

    return z;
}

inline
BinString copy(BinString src, int begin, int end)
{
    BinString dst;

    for(int i = begin; i < end; ++i)
    {
        dst.pushBack(src[i]);
    }

    return dst;
}

inline
std::ostream &operator<< (std::ostream &os, const BinString &x)
{
    const auto size = x.size();

    for(int i = 0; i < size; ++i)
    {
        os << (x[i] ? '1' : '0');
    }

    return os;
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Syntax::CABAD */

#endif /* HEVC_Syntax_CABAD_BinString_h */
