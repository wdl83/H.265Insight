#ifndef MD5Hasher_h
#define MD5Hasher_h

#include <cstdint>
#include <array>
#include <algorithm>
#include <iterator>
#include <ostream>
#include <iomanip>
/* 3rdParty */
#include <ThirdParty/MD5/md5.h>

/*----------------------------------------------------------------------------*/
class MD5Hasher
{
public:
    struct ValueType: public std::array<uint8_t, 128 / 8>
    {
        ValueType()
        {
            std::fill(std::begin(*this), std::end(*this), 0);
        }

        void toStr(std::ostream &os) const
        {
            os << std::hex;

            for(auto i : *this)
            {
                os << int(i);
            }
        }
    };
private:
    md5_state_t m_state;
    ValueType m_value;
    bool m_done;
public:
    MD5Hasher()
    {
        clear();
    }

    void clear()
    {
        md5_init(&m_state);
        m_done = false;
    }

    void calc(uint8_t *msg, uint64_t length)
    {
        md5_append(&m_state, msg, length);
    }

    void calc(uint8_t msg)
    {
        md5_append(&m_state, &msg, 1);
    }

    const ValueType &value()
    {
        if(!m_done)
        {
            md5_finish(&m_state, m_value.data());
            m_done = true;
        }

        return m_value;
    }
};
/*----------------------------------------------------------------------------*/


#endif /* MD5Hasher_h */
