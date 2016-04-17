#ifndef utils_h
#define utils_h
/* STDC++ */
#include <type_traits>
#include <limits>
#include <memory>
#include <array>
/**/
#include <runtime_assert.h>

/* WARNING: this header should be free of dependencies other then STDC++ */

/*----------------------------------------------------------------------------*/
template <typename Type, size_t length>
constexpr size_t lengthOf(const Type(&)[length])
{
    return length;
}
/*----------------------------------------------------------------------------*/
template <typename Type> constexpr int getSizeInBits()
{
    static_assert(
            std::is_signed<Type>::value
            || std::is_unsigned<Type>::value,
            "getSizeInBits() only signed/unsigned types are supported.");

    return sizeof(Type) * 8;
}
/*----------------------------------------------------------------------------*/
template <
    typename T,
    typename = typename std::enable_if<std::is_integral<T>::value>::type,
    typename = typename std::enable_if<std::is_unsigned<T>::value>::type>
constexpr int sumBits(T value, int pos = 0, int sum = 0)
{
    return
        sizeof(T) * 8 == pos
        ? sum
        : sumBits(value >> 1, pos + 1, sum + (value & 1));
}

template <typename T>
constexpr bool isPowerOf2(T value)
{
    return 1 == sumBits(value);
}

constexpr int log2(int n, int power = 0)
{
    return 1 == n ? power : log2(n >> 1, power + 1);
}
/*----------------------------------------------------------------------------*/
template <typename T>
constexpr
T makeMask(int bitsNum, int shiftBy = 0, T value = T(0))
{
    return
        0 < bitsNum
        ? makeMask(bitsNum - 1, shiftBy, value | (T(1) << (bitsNum - 1 + shiftBy)))
        : value;
}

template <typename T>
constexpr
T mask(T value, int bitsNum, int shiftBy = 0)
{
    return value & makeMask<T>(bitsNum, shiftBy);
}
/*----------------------------------------------------------------------------*/
template <typename T, size_t n>
inline
void fill(std::array<T, n> &array, const T &value)
{
    std::fill(std::begin(array), std::end(array), value);
}

template <typename T, typename A, size_t n1, size_t n2>
inline
void fill(std::array<std::array<A, n2>, n1> &array, const T &value)
{
    for(auto &a : array)
    {
        fill(a, value);
    }
}
/*----------------------------------------------------------------------------*/
template <typename Iterator>
inline
Iterator advance(Iterator i, int n)
{
    std::advance(i, n);
    return i;
}
/*----------------------------------------------------------------------------*/
template <typename T>
inline
constexpr
bool overflow(T a, T b)
{
    return 0 < b && std::numeric_limits<T>::max() - b < a;
}

template <typename T>
inline
constexpr
bool underflow(T a, T b)
{
    return 0 > b && std::numeric_limits<T>::min() - b > a;
}
/*----------------------------------------------------------------------------*/

#endif /* utils_h */
