#ifndef HEVC_Syntax_Aggregator_h
#define HEVC_Syntax_Aggregator_h
/* STDC++ */
#include <type_traits>
/* HEVC */
#include <Syntax/Syntax.h>
#include <Syntax/check.h>

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
namespace AggregatorImpl {

template <typename T>
struct Payload
{
    static const auto size = sizeof(T);

    alignas(T) uint8_t bytes[size];

    const void *addr() const
    {
        return static_cast<const void *>(bytes);
    }

    void *addr()
    {
        return static_cast<void *>(bytes);
    }

    const T *ptr() const
    {
        return static_cast<const T *>(addr());
    }

    T *ptr()
    {
        return static_cast<T *>(addr());
    }

    template <typename ...A_n>
    void construct(A_n &&... a_n)
    {
        new (addr()) T(std::forward<A_n>(a_n)...);
    }

    void destruct()
    {
        ptr()->~T();
    }
};

} /* AggregatorImpl */
/*----------------------------------------------------------------------------*/
struct EmbeddedAggregatorTag
{};

template <typename ...E_s>
class EmbeddedAggregator: public EmbeddedAggregatorTag
{
    void destruct()
    {}

    template <typename E, typename ...T_s>
    void destruct(AggregatorImpl::Payload<E> &payload, AggregatorImpl::Payload<T_s> &... t_s)
    {
        if(m_constructed[VariadicTraits::IndexOf<E, E_s...>::value])
        {
            payload.destruct();
        }

        destruct(t_s...);
    }

    Tuple<AggregatorImpl::Payload<E_s>...> m_embedded;
    BitArray<sizeof...(E_s)> m_constructed;
public:
    ~EmbeddedAggregator()
    {
        destruct(m_embedded.template get<AggregatorImpl::Payload<E_s>>()...);
    }

    template <typename E, typename ...A_n>
    void construct(A_n &&... a_n)
    {
        runtime_assert((!m_constructed[VariadicTraits::IndexOf<E, E_s...>::value]));
        m_embedded.template get<AggregatorImpl::Payload<E>>().construct(std::forward<A_n>(a_n)...);
        m_constructed[VariadicTraits::IndexOf<E, E_s...>::value] = true;
    }

    template <
        typename E,
        typename = typename VariadicTraits::IsOneOf<E, E_s...>::type>
    bool constructed() const
    {
        return m_constructed[VariadicTraits::IndexOf<E, E_s...>::value];
    }

    template <
        typename E,
        typename = typename VariadicTraits::IsOneOf<E, E_s...>::type>
    const E *get() const
    {
        return m_embedded.template get<AggregatorImpl::Payload<E>>().ptr();
    }

    template <
        typename E,
        typename = typename VariadicTraits::IsOneOf<E, E_s...>::type>
    E *get()
    {
        return m_embedded.template get<AggregatorImpl::Payload<E>>().ptr();
    }
};

template <typename E, typename ...E_s, typename ...A_n>
auto embed(EmbeddedAggregator<E_s...> &aggregator, A_n &&... a_n)
    -> decltype(aggregator.template get<E>())
{
    syntaxCheck(!aggregator.template constructed<E>());
    aggregator.template construct<E>(std::forward<A_n>(a_n)...);
    return aggregator.template get<E>();
}
/*----------------------------------------------------------------------------*/
struct SubtreeAggregatorTag
{};

template <typename ...E_s>
class SubtreeAggregator: public SubtreeAggregatorTag
{
private:
    Tuple<Handle<E_s>...> m_subtree;
public:
    template <typename E, typename ...A_n>
    void construct(A_n &&... a_n)
    {
        m_subtree.template get<Handle<E>>().construct(std::forward<A_n>(a_n)...);
    }

    template <
        typename E,
        typename = typename VariadicTraits::IsOneOf<E, E_s...>::type>
    Ptr<E> getSubtree() const
    {
        return Ptr<E>{m_subtree.template get<Handle<E>>()};
    }
};

template <typename E, typename ...E_s, typename ...A_n>
auto embedSubtree(SubtreeAggregator<E_s...> &aggregator, A_n &&... a_n)
    -> decltype(aggregator.template getSubtree<E>())
{
    aggregator.template construct<E>(std::forward<A_n>(a_n)...);
    return aggregator.template getSubtree<E>();
}
/*----------------------------------------------------------------------------*/
struct SubtreeArrayAggregatorTag
{};

template <typename E, int n>
struct SubtreeArray
{
    typedef E ValueType;
    static const auto size = n;
};

template <typename ...E_s>
class SubtreeArrayAggregator: public SubtreeArrayAggregatorTag
{
    template <typename E>
    using Array = std::array<Handle<typename E::ValueType>, E::size>;

    Tuple<std::array<Handle<typename E_s::ValueType>, E_s::size>...> m_subtree;
public:
    template <typename E, typename ...A_n>
    void construct(int i, A_n &&... a_n)
    {
        m_subtree.template get<Array<E>>()[i].construct(std::forward<A_n>(a_n)...);
    }

    template <typename E>
    Ptr<typename E::ValueType> subtree(int i) const
    {
        return
            m_subtree.template get<Array<E>>()[i]
            ? Ptr<typename E::ValueType>{m_subtree.template get<Array<E>>()[i]}
            : nullptr;
    }
};

template <typename E, typename ...E_s, typename ...A_n>
auto embedSubtreeInArray(SubtreeArrayAggregator<E_s...> &aggregator, int i, A_n &&... a_n)
    -> decltype(aggregator.template subtree<E>(i))
{
    aggregator.template construct<E>(i, std::forward<A_n>(a_n)...);
    return aggregator.template subtree<E>(i);
}
/*----------------------------------------------------------------------------*/
struct SubtreeListAggregatorTag
{};

template <typename ...E_s>
class SubtreeListAggregator: public SubtreeListAggregatorTag
{
    Tuple<VLA<Handle<E_s>>...> m_subtree;
public:
    template <typename E, typename ...A_n>
    void construct(A_n &&... a_n)
    {
        typedef Handle<E> H;

        m_subtree.template get<VLA<H>>().emplaceBack();
        m_subtree.template get<VLA<H>>().back().construct(std::forward<A_n>(a_n)...);
    }

    template <typename E>
    Ptr<E> back() const
    {
        return Ptr<E>{m_subtree.template get<VLA<Handle<E>>>().back()};
    }

    template <typename E>
    const VLA<Handle<E>> &getSubtreeList() const
    {
        return m_subtree.template get<VLA<Handle<E>>>();
    }
};

template <typename E, typename ...E_s, typename ...A_n>
auto embedSubtreeInList(SubtreeListAggregator<E_s...> &aggregator, A_n &&... a_n)
    -> decltype(aggregator.template back<E>())
{
    aggregator.template construct<E>(std::forward<A_n>(a_n)...);
    return aggregator.template back<E>();
}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */

#endif /* HEVC_Syntax_Aggregator_h */
