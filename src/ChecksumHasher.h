#pragma once

#include <cstdint>
#include <array>
#include <algorithm>
#include <iterator>
#include <ostream>
#include <iomanip>

/*----------------------------------------------------------------------------*/
class ChecksumHasher
{
public:
    struct ValueType: public std::array<uint8_t, 32 / 8>
    {
        ValueType()
        {
            std::fill(std::begin(*this), std::end(*this), 0);
        }

        uint32_t value() const
        {
            return *reinterpret_cast<const uint32_t * const>(data());
        }

        void value(uint32_t v)
        {
            *reinterpret_cast<uint32_t *>(data()) = v;
        }

        void toStr(std::ostream &os) const
        {
            os << std::hex << std::setw(2) << std::setfill('0');

            for(auto i : *this)
            {
                os << int(i);
            }
        }

        std::string toStr() const
        {
            std::ostringstream oss;
            toStr(oss);
            return oss.str();
        }
    };
private:
    ValueType m_value;
public:
    ChecksumHasher()
    {}

    void calc(int x, int y, uint8_t data)
    {
        uint32_t &checksum = *reinterpret_cast<uint32_t *>(m_value.data());
        const auto xorMask = (x & 0xFF) ^ (y & 0xFF) ^ (x >> 8) ^ (y >> 8);
        checksum = (checksum + ((data & 0xFF) ^ xorMask)) & 0xFFFFFFFF;
    }

    const ValueType &value()
    {
        return m_value;
    }
};
/*----------------------------------------------------------------------------*/
