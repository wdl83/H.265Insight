#ifndef VLA_h
#define VLA_h

/* STDC++ */
#include <vector>
#include <initializer_list>
#include <utility>
/**/
#include <runtime_assert.h>

/*----------------------------------------------------------------------------*/
/* Variable Length Array */
template <typename T>
class VLA
{
public:
    typedef std::vector<T> Container;

    typedef typename Container::value_type ValueType;
    typedef typename Container::reference Reference;
    typedef typename Container::const_reference ConstReference;
    typedef typename Container::pointer Pointer;
    typedef typename Container::const_pointer ConstPointer;

    typedef typename Container::size_type SizeType;
    typedef typename Container::iterator Iterator;
    typedef typename Container::reverse_iterator ReverseIterator;
    typedef typename Container::const_iterator ConstIterator;
    typedef typename Container::const_reverse_iterator ConstReverseIterator;
protected:
    Container m_container;
public:
    VLA()
    {}

    VLA(const VLA &) = default;
    VLA(VLA &&) = default;
    VLA &operator= (const VLA &) = default;
    VLA &operator= (VLA &&) = default;

    VLA(SizeType s):
        m_container(s)
    {}

    VLA(SizeType s, ConstReference v):
        m_container(s, v)
    {}

    VLA(std::initializer_list<ValueType> l):
        m_container(l)
    {}

    template <typename I>
    VLA(I begin, I end): m_container(begin, end)
    {}

    void reserve(SizeType length)
    {
        m_container.reserve(length);
    }

    void resize(SizeType length, ConstReference value = ValueType())
    {
        m_container.resize(length, value);
    }

    Iterator begin()
    {
        return m_container.begin();
    }

    ConstIterator begin() const
    {
        return m_container.begin();
    }

    ConstIterator cbegin() const
    {
        return m_container.cbegin();
    }

    ReverseIterator rbegin()
    {
        return m_container.rbegin();
    }

    ConstReverseIterator rbegin() const
    {
        return m_container.rbegin();
    }

    ConstReverseIterator crbegin() const
    {
        return m_container.crbegin();
    }

    Iterator end()
    {
        return m_container.end();
    }

    ConstIterator end() const
    {
        return m_container.end();
    }

    ConstIterator cend() const
    {
        return m_container.cend();
    }

    ReverseIterator rend()
    {
        return m_container.rend();
    }

    ConstReverseIterator rend() const
    {
        return m_container.rend();
    }

    ConstReverseIterator crend() const
    {
        return m_container.crend();
    }

    void erase(Iterator i)
    {
        bdryCheck(!m_container.empty());
        m_container.erase(i);
    }

    void erase(ReverseIterator i)
    {
        bdryCheck(!m_container.empty());
        m_container.erase(i);
    }

    ConstReference front() const
    {
        bdryCheck(!m_container.empty());
        return m_container.front();
    }

    Reference front()
    {
        bdryCheck(!m_container.empty());
        return m_container.front();
    }

    ConstReference back() const
    {
        bdryCheck(!m_container.empty());
        return m_container.back();
    }

    Reference back()
    {
        bdryCheck(!m_container.empty());
        return m_container.back();
    }

    ConstReference operator[] (SizeType i) const
    {
        bdryCheck(m_container.size() > i);
        return m_container[i];
    }

    Reference operator[] (SizeType i)
    {
        bdryCheck(m_container.size() > i);
        return m_container[i];
    }

    void pushBack(ConstReference v)
    {
        m_container.push_back(v);
    }

    void pushBack(ValueType &&v)
    {
        m_container.push_back(std::move(v));
    }

    template <typename ... A_n>
    void emplaceBack(A_n &&... a_n)
    {
        m_container.emplace_back(std::move(a_n)...);
    }

    SizeType capacity() const
    {
        return m_container.capacity();
    }

    SizeType size() const
    {
        return m_container.size();
    }

    bool empty() const
    {
        return m_container.empty();
    }

    friend
    bool operator== (const VLA &x, const VLA &y)
    {
        return x.m_container == y.m_container;
    }

    friend
    bool operator!= (const VLA &x, const VLA &y)
    {
        return !(x == y);
    }
};

template <typename T>
using IsVLA =
    typename std::enable_if<std::is_same<VLA<typename T::ValueType>, T>::value>::type;
/*----------------------------------------------------------------------------*/

#endif /* VLA_h */
