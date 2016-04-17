#ifndef Tuple_h
#define Tuple_h

/* STDC++ */
#include <tuple>
#include <type_traits>
/**/
#include <VariadicTraits.h>

/*----------------------------------------------------------------------------*/
/* Tuple class which supports:
 *  1. [CT/RT] getting by index;
 *  2. [CT] getting by type (in case of duplicates first from LHS is used); */

template <typename ...T_s>
class Tuple;

namespace TupleImpl {
/*----------------------------------------------------------------------------*/
/* TODO:
 * Change expansion from type to index.
 * Expansion by type introduces duplicates limitation (see 2). */

template <typename N, typename T, typename ...T_s>
const N *ptr(const Tuple<T_s...> &tuple)
{
    return static_cast<const N *>(&tuple.template get<T>());
}

template <typename N, typename T, typename ...T_s>
N *ptr(Tuple<T_s...> &tuple)
{
    return static_cast<N *>(&tuple.template get<T>());
}

template <typename N, typename T, typename ...T_s>
const N &ref(const Tuple<T_s...> &tuple)
{
    return static_cast<const N &>(tuple.template get<T>());
}

template <typename N, typename T, typename ...T_s>
N &ref(Tuple<T_s...> &tuple)
{
    return static_cast<N &>(tuple.template get<T>());
}

/*----------------------------------------------------------------------------*/
} /* TupleImpl */

template <typename ...T_s>
class Tuple
{
    template <typename ...> friend class Tuple;
protected:
    std::tuple<T_s...> m_tuple;
public:
    constexpr
    Tuple()
    {}

    constexpr
    Tuple(const Tuple &) = default;

    constexpr
    Tuple(Tuple &&) = default;

    explicit Tuple(const T_s &... t_s):
        m_tuple{t_s...}
    {}

    template <typename ...A_n>
    explicit Tuple(A_n &&... a_n):
        m_tuple{std::forward<A_n>(a_n)...}
    {}

    template <typename ...A_n>
    Tuple(const Tuple<A_n...> &x):
        m_tuple{x.m_tuple}
    {}

    template <typename ...A_n>
    Tuple(Tuple<A_n...> &&x):
        m_tuple{x.m_tuple}
    {}

    template <typename ...A_n>
    Tuple(const std::tuple<A_n...> &x):
        m_tuple{x}
    {}

    template <typename ...A_n>
    Tuple(std::tuple<A_n...> &&x):
        m_tuple{x}
    {}

    Tuple &operator= (const Tuple &) = default;
    Tuple &operator= (Tuple &&) = default;

    template <typename ...A_n>
    Tuple &operator= (const Tuple<A_n...>  &x)
    {
        m_tuple = x.m_tuple;
        return *this;
    }

    template <typename ...A_n>
    Tuple &operator= (Tuple<A_n...>  &&x)
    {
        m_tuple = x.m_tuple;
        return *this;
    }

    template <size_t n>
    auto get() const -> decltype(std::get<n>(m_tuple))
    {
        return std::get<n>(m_tuple);
    }

    template <size_t n>
    auto get() -> decltype(std::get<n>(m_tuple))
    {
        return std::get<n>(m_tuple);
    }

    template <typename T>
    const T &get() const
    {
        return get<VariadicTraits::IndexOf<T, T_s...>::value>();
    }

    template <typename T>
    T &get()
    {
        return get<VariadicTraits::IndexOf<T, T_s...>::value>();
    }

    template <typename T>
    const T *ptr(int i) const
    {
        typedef const T *(*fptr)(const Tuple<T_s...>&);

        constexpr fptr fptrs[] = {&TupleImpl::ptr<T, T_s, T_s...>...};
        return fptrs[i](*this);
    }

    template <typename T>
    T *ptr(int i)
    {
        typedef T *(*fptr)(Tuple<T_s...>&);

        constexpr fptr fptrs[] = {&TupleImpl::ptr<T, T_s, T_s...>...};
        return fptrs[i](*this);
    }

    template <typename T>
    const T &ref(int i) const
    {
        typedef const T &(*fptr)(const Tuple<T_s...>&);
        constexpr fptr fptrs[] = {&TupleImpl::ref<T, T_s, T_s...>...};
        return fptrs[i](*this);
    }

    template <typename T>
    T &ref(int i)
    {
        typedef T &(*fptr)(Tuple<T_s...>&);
        constexpr fptr fptrs[] = {&TupleImpl::ref<T, T_s, T_s...>...};
        return fptrs[i](*this);
    }
};


template <typename ...T_s>
inline
Tuple<typename std::decay<T_s>::type...> makeTuple(T_s &&... v)
{
    // TODO: strip std::reference_wrapper
    return Tuple<typename std::decay<T_s>::type...>(std::forward<T_s>(v)...);
}

template <typename ...T_s>
inline
Tuple<T_s &...> tie(T_s &... t_s)
{
    return Tuple<T_s &...>{t_s...};
}

template <size_t, typename ...>
struct TupleElement;

template <typename ...T_s>
struct TupleElement<0, T_s...>
{
    template <typename F>
    static
    void apply(const Tuple<T_s...> &, F)
    {}
};

template <size_t I, typename ...T_s>
struct TupleElement
{
    template <typename F>
    static
    void apply(const Tuple<T_s...> &t, F f)
    {
        f(t.template get<sizeof...(T_s) - I>());
        TupleElement<I - 1, T_s...>::apply(t, f);
    }
};

template <typename F, typename ...T_s>
void apply(const Tuple<T_s...> &t, F f)
{
    TupleElement<sizeof...(T_s), T_s...>::apply(t, f);
}
/*----------------------------------------------------------------------------*/

#endif /* Tuple_h */
