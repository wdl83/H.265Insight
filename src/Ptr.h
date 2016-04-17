#ifndef Ptr_h
#define Ptr_h

#include <Handle.h>
#include <runtime_assert.h>

template <typename T>
class Ptr
{
    template <typename O> friend class Ptr;
public:
    typedef T ValueType;
    typedef ValueType * Pointer;
    typedef ValueType & Reference;
    typedef const ValueType * ConstPointer;
    typedef const ValueType & ConstReference;

    typedef Handle<ValueType> HandleType;
    typedef HandleType * HandlePointer;
    typedef HandleType & HandleReference;
    typedef const HandleType * ConstHandlePointer;
    typedef const HandleType & ConstHandleReference;
private:
    Pointer m_ptr;

    void check() const
    {
        ptrCheck(bool(*this));
    }

    Pointer ptr() const
    {
        check();
        return m_ptr;
    }

    void ptr(ConstHandleReference h)
    {
        m_ptr = h.ptr();
    }

    Reference ref() const
    {
        return *ptr();
    }
public:
    constexpr
    Ptr(): m_ptr(nullptr)
    {}

    constexpr
    Ptr(std::nullptr_t): m_ptr(nullptr)
    {}

    explicit Ptr(ConstHandleReference h)
    {
        ptr(h);
    }

    Ptr(const Ptr &) = default;
    explicit Ptr(Ptr &&) = default;

    template<
        typename O,
        typename = typename std::enable_if<std::is_convertible<O *, Pointer>::value>::type>
	Ptr(const Ptr<O> &other): m_ptr(other.m_ptr)
    {}

    Ptr &operator= (const Ptr &) = default;
    Ptr &operator= (Ptr &&) = default;

    Ptr &operator= (ConstHandleReference h)
    {
        ptr(h);
        return *this;
    }

    explicit operator bool () const
    {
        return nullptr != m_ptr;
    }

    Pointer operator-> () const
    {
        return ptr();
    }

    Reference operator* () const
    {
        return ref();
    }

    friend
    bool operator== (const Ptr &p, const std::nullptr_t &)
    {
        return p.m_ptr == nullptr;
    }

    friend
    bool operator== (const std::nullptr_t &, const Ptr &p)
    {
        return nullptr == p.m_ptr;
    }

    friend
    bool operator!= (const Ptr &p, const std::nullptr_t &)
    {
        return !(p == nullptr);
    }

    friend
    bool operator!= (const std::nullptr_t &, const Ptr &p)
    {
        return !(nullptr == p);
    }
};
/*----------------------------------------------------------------------------*/

#endif /* Ptr_h */
