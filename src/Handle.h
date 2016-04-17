#ifndef Handle_h
#define Handle_h

/* STDC++ */
#include <cstddef>
#include <utility>
/* */
#include <runtime_assert.h>

/*----------------------------------------------------------------------------*/
/* Handle assumes object ownership */
template <typename T>
class Handle
{
public:
    typedef T ValueType;
    typedef ValueType * Pointer;
    typedef ValueType & Reference;
    typedef const ValueType * ConstPointer;
    typedef const ValueType & ConstReference;
private:
    Pointer m_ptr;
protected:
    Handle(Pointer ptr): m_ptr(ptr)
    {}
public:
    Handle(): m_ptr(nullptr)
    {}

    ~Handle()
    {
        destruct(*this);
    }

    template <typename ...A_n>
    void construct(A_n &&...a_n)
    {
        ptrCheck(nullptr == m_ptr);
        m_ptr = new ValueType(std::forward<A_n>(a_n)...);
    }

    friend
    void destruct(Handle &h)
    {
        if(h.m_ptr)
        {
            delete h.m_ptr;
            h.m_ptr = nullptr;
        }
    }

    Handle(const Handle &) = delete;

    Handle(Handle &&other):
        Handle()
    {
        std::swap(m_ptr, other.m_ptr);
    }

    Handle &operator= (const Handle &) = delete;

    Handle &operator= (Handle &&other)
    {
        destruct(*this);
        std::swap(m_ptr, other.m_ptr);
        return *this;
    }

    void check() const
    {
        ptrCheck(bool(*this));
    }

    Pointer ptr() const
    {
        check();
        return m_ptr;
    }

    Reference ref() const
    {
        return *ptr();
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
    bool operator== (const Handle &h, const std::nullptr_t &)
    {
        return h.m_ptr == nullptr;
    }

    friend
    bool operator== (const std::nullptr_t &, const Handle &h)
    {
        return nullptr == h.m_ptr;
    }

    friend
    bool operator!= (const Handle &h, const std::nullptr_t &)
    {
        return !(h == nullptr);
    }

    friend
    bool operator!= (const std::nullptr_t &, const Handle &h)
    {
        return !(nullptr == h);
    }
};

template <typename T, typename ...A_n>
inline
Handle<T> makeHandle(A_n &&... a_n)
{
    Handle<T> h;

    h.construct(std::forward<A_n>(a_n)...);
    return h;
}
/*----------------------------------------------------------------------------*/

#endif /* Handle_h */
