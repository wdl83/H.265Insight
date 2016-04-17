#ifndef HEVC_units_h
#define HEVC_units_h
/* STC++ */
#include <cstdint>
#include <ostream>
#include <type_traits>
#include <limits>
#include <cmath>
/* HEVC */
#include <StoreByValue.h>
#include <EnumUtils.h>

namespace HEVC {
/*----------------------------------------------------------------------------*/
template<
    typename T, T v,
    typename =
        typename std::enable_if<
            std::is_enum<T>::value
            && std::is_integral<typename std::underlying_type<T>::type>::value>::type,
    int = static_cast<int>(v)>
struct UnitTraits
{
    typedef T Category;
    static const auto unit = v;
};
/*----------------------------------------------------------------------------*/
enum class AlgebraicUnits
{
    Linear,
    Log2
};

typedef UnitTraits<AlgebraicUnits, AlgebraicUnits::Linear> LinearUnit;
typedef UnitTraits<AlgebraicUnits, AlgebraicUnits::Log2> Log2Unit;
/*----------------------------------------------------------------------------*/
template <
            typename U, typename S,
            typename = typename std::enable_if<std::is_pod<S>::value>::type>
class LinearArithmetic;

template <typename U, typename S> S toUnderlying(LinearArithmetic<U, S>);

template <typename U, typename S, typename>
class LinearArithmetic : protected StoreByValue<S>
{
public:
    typedef U UnitType;
    typedef S StoragePrimitive;
    using typename StoreByValue<StoragePrimitive>::ValueType;
    friend StoragePrimitive toUnderlying <> (LinearArithmetic value);
protected:
    using StoreByValue<StoragePrimitive>::getValue;
    using StoreByValue<StoragePrimitive>::setValue;
public:
    constexpr LinearArithmetic():
        StoreByValue<StoragePrimitive>{0}
    {}

    constexpr explicit LinearArithmetic(const StoragePrimitive value):
        StoreByValue<StoragePrimitive>{value}
    {}

    constexpr LinearArithmetic(const LinearArithmetic &) = default;

    LinearArithmetic &operator= (const LinearArithmetic &) = default;

    LinearArithmetic &operator+= (const LinearArithmetic &other)
    {
        setValue(getValue() + other.getValue());
        return *this;
    }

    LinearArithmetic operator+ (const LinearArithmetic &other) const
    {
        return LinearArithmetic(*this) += other;
    }

    LinearArithmetic &operator-= (const LinearArithmetic &other)
    {
        setValue(getValue() - other.getValue());
        return *this;
    }

    LinearArithmetic operator- (const LinearArithmetic &other) const
    {
        return LinearArithmetic(*this) -= other;
    }

    LinearArithmetic &operator/= (const LinearArithmetic &other)
    {
        setValue(getValue() / other.getValue());
        return *this;
    }

    LinearArithmetic operator/ (const LinearArithmetic &other) const
    {
        return LinearArithmetic(*this) /= other;
    }

    LinearArithmetic &operator*= (const LinearArithmetic &other)
    {
        setValue(getValue() * other.getValue());
        return *this;
    }

    LinearArithmetic operator* (const LinearArithmetic &other) const
    {
        return LinearArithmetic(*this) *= other;
    }

    LinearArithmetic &operator%= (const LinearArithmetic &other)
    {
        setValue(getValue() % other.getValue());
        return *this;
    }

    LinearArithmetic operator% (const LinearArithmetic &other) const
    {
        return LinearArithmetic(*this) %= other;
    }

    LinearArithmetic &operator++ ()
    {
        setValue(getValue() + 1);
        return *this;
    }

    LinearArithmetic &operator-- ()
    {
        setValue(getValue() - 1);
        return *this;
    }

    LinearArithmetic operator- () const
    {
        return LinearArithmetic(-getValue());
    }

    bool operator> (const LinearArithmetic &other) const
    {
        return getValue() > other.getValue();
    }

    friend
    bool operator< (const LinearArithmetic &x, const LinearArithmetic &y)
    {
        return x.getValue() < y.getValue();
    }

    bool operator>= (const LinearArithmetic &other) const
    {
        return (*this) > other || (*this) == other;
    }

    bool operator<= (const LinearArithmetic &other) const
    {
        return (*this) < other || (*this) == other;
    }

    bool operator== (const LinearArithmetic &other) const
    {
        return getValue() == other.getValue();
    }

    bool operator!= (const LinearArithmetic &other) const
    {
        return !(*this == other);
    }

    friend
    LinearArithmetic operator& (LinearArithmetic x, int mask)
    {
        return LinearArithmetic(x.getValue() & mask);
    }

    static constexpr LinearArithmetic max()
    {
        return LinearArithmetic{std::numeric_limits<StoragePrimitive>::max()};
    }

    static constexpr LinearArithmetic min()
    {
        return LinearArithmetic{std::numeric_limits<StoragePrimitive>::min()};
    }
};
/*----------------------------------------------------------------------------*/
template <typename U, typename S>
inline
S toUnderlying(LinearArithmetic<U, S> value)
{
    return value.getValue();
}

template <typename U, typename S>
std::ostream &operator<< (std::ostream &os, const LinearArithmetic<U, S> value)
{
    os << toUnderlying(value);
    return os;
}


template <typename U, typename S>
inline
float roundUp(LinearArithmetic<U, S> x, LinearArithmetic<U, S> y)
{
    return std::ceil(float(toUnderlying(x)) / toUnderlying(y));
}
/*----------------------------------------------------------------------------*/
/* Int */
/*----------------------------------------------------------------------------*/
template <typename U, typename S>
LinearArithmetic<U, S> operator* (LinearArithmetic<U, S> arg, int n)
{
    return LinearArithmetic<U, S>{S(toUnderlying(arg) * n)};
}

template <typename U, typename S>
LinearArithmetic<U, S> operator* (int n, LinearArithmetic<U, S> arg)
{
    return LinearArithmetic<U, S>{S(n * toUnderlying(arg))};
}

template <typename U, typename S>
LinearArithmetic<U, S> operator/ (LinearArithmetic<U, S> arg, int n)
{
    return LinearArithmetic<U, S>{S(toUnderlying(arg) / n)};
}

template <typename U, typename S>
LinearArithmetic<U, S> operator% (LinearArithmetic<U, S> arg, int n)
{
    return LinearArithmetic<U, S>{S(toUnderlying(arg) % n)};
}

/* TODO: impl. as recursive constexpr */
template <typename U, typename S>
LinearArithmetic<U, S> toPower(LinearArithmetic<U, S> value, int p)
{
    LinearArithmetic<U, S> r(1);

    while(0 != p)
    {
        if(0 > p)
        {
            r /= value;
            ++p;
        }
        else if(0 < p)
        {
            r *= value;
            --p;
        }
    }

    return r;
}

template <typename U, typename S>
LinearArithmetic<U, S> operator>> (LinearArithmetic<U, S> x, int n)
{
    return LinearArithmetic<U, S>{toUnderlying(x) >> n};
}

template <typename U, typename S>
LinearArithmetic<U, S> operator<< (LinearArithmetic<U, S> x, int n)
{
    return LinearArithmetic<U, S>{toUnderlying(x) << n};
}
/*----------------------------------------------------------------------------*/
/* Log2 */
/*----------------------------------------------------------------------------*/
typedef LinearArithmetic<Log2Unit, int> Log2;
/*----------------------------------------------------------------------------*/
constexpr Log2 operator "" _log2 (unsigned long long value)
{
    return Log2(static_cast<Log2::ValueType>(value));
}

inline
int toInt(Log2 x)
{
    return 1 << toUnderlying(x);
}
/*----------------------------------------------------------------------------*/
// returns Ceil(Log2(value))
template <typename T,
          typename = typename std::enable_if<std::is_integral<T>::value>::type>
inline
T log2(T x)
{
    static const uint64_t t[6] =
    {
        0xFFFFFFFF00000000ull,
        0x00000000FFFF0000ull,
        0x000000000000FF00ull,
        0x00000000000000F0ull,
        0x000000000000000Cull,
        0x0000000000000002ull
    };

    int32_t y = (x & (x - 1)) == 0 ? 0 : 1;
    int32_t j = 32;

    for (auto i = 0u; i < 6; ++i)
    {
        int32_t k = (x & t[i]) == 0 ? 0 : j;
        y += k;
        x >>= k;
        j >>= 1;
    }

    return T(y);
}
/*----------------------------------------------------------------------------*/
// returns Ceil(Log2(value))
template <typename U, typename S>
inline
Log2 toLog2(LinearArithmetic<U, S> value)
{
    uint64_t x = toUnderlying(value);
    return Log2(log2(x));
}
/*----------------------------------------------------------------------------*/
template <typename U>
inline
bool isAlignedTo(U value, U boundary)
{
    return U(0) == value % boundary;
}

template <typename U>
U alignTo(U value, U boundary)
{
    const auto unalignment = value % boundary;
    return U(0) == unalignment ? value : value - unalignment;
}
/*----------------------------------------------------------------------------*/
} /* HEVC */

#endif /* HEVC_units_h */
