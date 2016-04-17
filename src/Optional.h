#ifndef HEVC_Optional_h
#define HEVC_Optional_h

/* STDC++ */
#include <new>
#include <memory>
#include <cstring>
#include <ostream>
#include <utility>
/* HEVC */
#include <runtime_assert.h>

namespace HEVC {
/*----------------------------------------------------------------------------*/
template <typename T> class Optional
{
public:
    typedef T Type;
    typedef T* TypePtr;
    typedef T& TypeRef;
    typedef const T* ConstTypePtr;
    typedef const T& ConstTypeRef;
private:
    struct Storage
    {
        unsigned char m_bytes[sizeof(Type)];
    };

    Storage m_storage;
    bool m_constructed;
private:
    void const *getStorageAddr() const
    {
        return &m_storage;
    }

    void *getStorageAddr()
    {
        return &m_storage;
    }

    void check() const
    {
        runtime_assert(isConstructed());
    }
public:
    Optional(): m_constructed(false)
    {
        std::memset(getStorageAddr(), 0, sizeof(Storage));
    }

    Optional(const Optional &) = default;
    Optional(Optional &&) = default;
    Optional &operator= (const Optional &) = default;

    Optional &operator= (ConstTypeRef value)
    {
        if(isConstructed())
        {
            ref() = value;
        }
        else
        {
            construct(value);
        }

        return *this;
    }

    template <typename ...A_n>
    Optional(A_n &&...a_n)
    {
        construct(std::forward<A_n>(a_n)...);
    }

    ~Optional()
    {
        if(isConstructed())
        {
            destruct();
        }
    }

    bool isConstructed() const
    {
        return m_constructed;
    }

    template <typename ...A_n>
    void construct(A_n &&...a_n)
    {
        new (getStorageAddr()) Type(std::forward<A_n>(a_n)...);
        m_constructed = true;
    }

    ConstTypePtr ptr() const
    {
        check();
        return static_cast<ConstTypePtr>(getStorageAddr());
    }

    TypePtr ptr()
    {
        check();
        return static_cast<TypePtr>(getStorageAddr());
    }

    ConstTypeRef ref() const
    {
        return *ptr();
    }

    TypeRef ref()
    {
        return *ptr();
    }

    void destruct()
    {
        check();
        ptr()->~Type();
        m_constructed = false;
        std::memset(getStorageAddr(), 0, sizeof(Storage));
    }

    explicit operator bool () const
    {
        return isConstructed();
    }

    ConstTypePtr operator-> () const
    {
        return ptr();
    }

    TypePtr operator-> ()
    {
        return ptr();
    }

    ConstTypeRef operator* () const
    {
        return ref();
    }

    TypeRef operator* ()
    {
        return ref();
    }

    friend
    std::ostream &operator<< (std::ostream &os, const Optional &value)
    {
        if(value)
        {
            os << *value;
        }
        else
        {
            os << '?';
        }

        return os;
    }
};
/*----------------------------------------------------------------------------*/
} /* HEVC */

#endif /* HEVC_Optional_h */
