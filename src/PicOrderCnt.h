#ifndef HEVC_PicOrderCnt_h
#define HEVC_PicOrderCnt_h

#include <cstdint>
#include <ostream>

#include <Tuple.h>

namespace HEVC {
/*----------------------------------------------------------------------------*/
enum class PicOrderCntId
{
    Lsb, Msb, Val
};

struct PicOrderCntBase
{
    typedef int64_t ValueType;
};

template <PicOrderCntId id>
struct PicOrderCnt: public PicOrderCntBase
{
    static const auto Id = id;
    ValueType value;

    PicOrderCnt(): value(0)
    {}

    explicit
    PicOrderCnt(ValueType v): value(v)
    {}

    friend
    bool operator== (const PicOrderCnt &x, const PicOrderCnt &y)
    {
        return x.value == y.value;
    }

    friend
    bool operator!= (const PicOrderCnt &x, const PicOrderCnt &y)
    {
        return !(x == y);
    }

    friend
    bool operator< (const PicOrderCnt &x, const PicOrderCnt &y)
    {
        return x.value < y.value;
    }

    friend
    bool operator<= (const PicOrderCnt &x, const PicOrderCnt &y)
    {
        return x.value <= y.value;
    }

    friend
    bool operator> (const PicOrderCnt &x, const PicOrderCnt &y)
    {
        return x.value > y.value;
    }

    friend
    bool operator>= (const PicOrderCnt &x, const PicOrderCnt &y)
    {
        return x.value >= y.value;
    }

    PicOrderCnt &operator++ ()
    {
        ++value;
        return *this;
    }

    PicOrderCnt &operator+= (const PicOrderCnt &other)
    {
        value += other.value;
        return *this;
    }

    friend
    PicOrderCnt operator+ (const PicOrderCnt &x, const PicOrderCnt &y)
    {
        return PicOrderCnt(x) += y;
    }

    PicOrderCnt &operator-= (const PicOrderCnt &other)
    {
        value -= other.value;
        return *this;
    }

    friend
    PicOrderCnt operator- (const PicOrderCnt &x, const PicOrderCnt &y)
    {
        return PicOrderCnt(x) -= y;
    }

    PicOrderCnt &operator/= (int ratio)
    {
        value /= ratio;
        return *this;
    }

    friend
    PicOrderCnt operator/ (const PicOrderCnt &x, int ratio)
    {
        return PicOrderCnt(x) /= ratio;
    }

    PicOrderCnt &operator*= (int ratio)
    {
        value *= ratio;
        return *this;
    }

    friend
    PicOrderCnt operator* (const PicOrderCnt &x, int ratio)
    {
        return PicOrderCnt(x) *= ratio;
    }

    friend
    std::ostream &operator<< (std::ostream &os, const PicOrderCnt &cnt)
    {
        os << cnt.value;
        return os;
    }
};

typedef PicOrderCnt<PicOrderCntId::Lsb> PicOrderCntLsb;
typedef PicOrderCnt<PicOrderCntId::Msb> PicOrderCntMsb;
typedef PicOrderCnt<PicOrderCntId::Val> PicOrderCntVal;
typedef Tuple<PicOrderCntVal, PicOrderCntMsb, PicOrderCntLsb> PicOrderCntSet;
/*----------------------------------------------------------------------------*/
} /* HEVC */

#endif /* HEVC_PicOrderCnt_h */

