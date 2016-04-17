#ifndef EnumUtils_h
#define EnumUtils_h

/* STDC++ */
#include <cstddef>
#include <type_traits>
/*----------------------------------------------------------------------------*/
template <
    typename T,
    typename = typename std::enable_if<std::is_enum<T>::value>::type>
constexpr typename std::underlying_type<T>::type castToUnderlying(T value)
{
    return static_cast<typename std::underlying_type<T>::type>(value);
}
/*----------------------------------------------------------------------------*/
template <
    typename T,
    T Begin = T::Begin,
    T End = T::End,
    typename = typename std::enable_if<std::is_enum<T>::value>::type>
class Iterator
{
public:
    typedef T Type;
    typedef typename std::underlying_type<Type>::type UnderlyingType;
private:
    UnderlyingType m_value;
public:
    constexpr Iterator(Type value): m_value{castToUnderlying(value)}
    {}

    constexpr Iterator(const Iterator &) = default;
    Iterator &operator= (const Iterator &) = default;

    Type operator* () const
    {
        return static_cast<Type>(m_value);
    }

    Iterator &operator++ ()
    {
        if(End != **this)
        {
            m_value += 1;
        }

        return *this;
    }

    bool operator == (const Iterator &iterator)
    {
        return m_value == iterator.m_value;
    }

    bool operator != (const Iterator &iterator)
    {
        return !(*this == iterator);
    }
};
/*----------------------------------------------------------------------------*/
template <
    typename Type,
    Type Begin = Type::Begin,
    Type End = Type::End,
    typename = typename std::enable_if<std::is_enum<Type>::value>::type>
class EnumRange
{
public:
    typedef Iterator<Type, Begin, End> IteratorType;

    constexpr EnumRange()
    {}

    static constexpr IteratorType begin()
    {
        return IteratorType(Begin);
    }

    static constexpr IteratorType end()
    {
        return IteratorType(End);
    }

    static constexpr size_t length()
    {
        return castToUnderlying(End) - castToUnderlying(Begin);
    }

    static constexpr bool encloses(Type value)
    {
        return
            castToUnderlying(Begin) <= castToUnderlying(value)
            && castToUnderlying(End) > castToUnderlying(value);
    }
};
/*----------------------------------------------------------------------------*/

#endif /* EnumUtils_h */
