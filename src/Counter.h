#ifndef HEVC_Counter_h
#define HEVC_Counter_h

/* C/C++ */
#include <type_traits>
/* */
#include <utils.h>

namespace HEVC {
/*----------------------------------------------------------------------------*/
template <
    typename Unsigned,
    uint8_t n,
    typename = typename std::enable_if<std::is_integral<Unsigned>::value>::type,
    typename = typename std::enable_if<std::is_unsigned<Unsigned>::value>::type>
class alignas(Unsigned) Counter
{
    static_assert(getSizeInBits<Unsigned>() >= n, "insufficient unsigned capacity");
private:
    typedef typename std::make_signed<Unsigned>::type Signed;
    Unsigned m_cntr : n;
public:
    constexpr Counter(): m_cntr(static_cast<Unsigned>(0))
    {}

    explicit constexpr Counter(Unsigned cntr): m_cntr(cntr)
    {}

    Counter &operator++ ()
    {
        ++m_cntr;
        return *this;
    }

    Counter& operator-- ()
    {
        --m_cntr;
        return *this;
    }

    Counter &operator+= (int i)
    {
        m_cntr += i;
        return *this;
    }

    Counter &operator-= (int i)
    {
        m_cntr -= i;
        return *this;
    }

    operator Unsigned () const
    {
        return m_cntr;
    }

    Signed operator- (const Counter &other) const
    {
        static_assert(getSizeInBits<Unsigned>() > n, "insufficient signed capacity");
        return static_cast<Signed>(m_cntr) - static_cast<Signed>(other.m_cntr);
    }
};
/*----------------------------------------------------------------------------*/
} /* HEVC */

#endif /* HEVC_Counter_h */
