#ifndef VariadicTraits_h
#define VariadicTraits_h

#include <type_traits>

namespace VariadicTraits
{
/*----------------------------------------------------------------------------*/
namespace Impl {
/*----------------------------------------------------------------------------*/
template <
    /* used by impl. to terminate recursion */
    bool Terminate,
    /* calculated index (of type N) */
    int n,
    /* type which index is calculated */
    typename N,
    /* list to search */
    typename ...Ts>
struct IndexOf;

/* terminal state */
template <
    int n,
    typename N,
    typename ...Ts>
struct IndexOf<true, n, N, Ts...>
{
    static const int value = n;
};

/* intermediate state */
template <
    int n,
    typename N,
    typename T,
    typename ...Ts>
struct IndexOf<false, n, N, T, Ts...>
{
    static const int value =
        IndexOf<std::is_same<N, T>::value, n + 1, N, Ts...>::value;
};
/*----------------------------------------------------------------------------*/
template <
    /* used by impl. to terminate recursion */
    bool Terminate,
    /* type to match */
    typename N,
    /* list to search */
    typename ...Ts>
struct IsOneOf;

/* terminal state */
template <
    typename N,
    typename ...Ts>
struct IsOneOf<true, N, Ts...>
{
    typedef N type;
};

/* intermediate state */
template <
    typename N,
    typename T,
    typename ...Ts>
struct IsOneOf<false, N, T, Ts...>
{
    typedef typename IsOneOf<std::is_same<N, T>::value, N, Ts...>::type type;
};
/*----------------------------------------------------------------------------*/
} /* Impl */
/*----------------------------------------------------------------------------*/

template <
    typename N,
    typename T,
    typename ...Ts>
struct IndexOf
{
    static const int value =
        Impl::IndexOf<std::is_same<N, T>::value, 0, N, Ts...>::value;
};
/*----------------------------------------------------------------------------*/
template <
    typename N,
    typename T,
    typename ...Ts>
struct IsOneOf
{
    typedef typename Impl::IsOneOf<std::is_same<N, T>::value, N, Ts...>::type type;
};
/*----------------------------------------------------------------------------*/
template <unsigned int ...>
struct TupleIndices
{};

template<
    unsigned int I,
    typename IndexTuple,
    typename... Types>
struct MakeTupleIndices_;

template<
    unsigned int I,
    unsigned int... Indices,
    typename T,
    typename... Types>
struct MakeTupleIndices_<I, TupleIndices<Indices...>, T, Types...>
{
    typedef typename MakeTupleIndices_<I + 1, TupleIndices<Indices..., I>, Types...>::Type Type;
};

template<
    unsigned int I,
    unsigned int... Indices>
struct MakeTupleIndices_<I, TupleIndices<Indices...> >
{
    typedef TupleIndices<Indices...> Type;
};

template<typename... Types>
struct MakeTupleIndices : public MakeTupleIndices_<0, TupleIndices<>, Types...>
{};
/*----------------------------------------------------------------------------*/
}

#endif /* VariadicTraits_h */
