#ifndef CRCHasher_h
#define CRCHasher_h

#include <cstdint>
#include <array>
#include <algorithm>
#include <iterator>
#include <ostream>
#include <iomanip>

/*----------------------------------------------------------------------------*/
class CRCHasher
{
public:
    struct ValueType: public std::array<uint8_t, 16 / 8>
    {
        ValueType()
        {
            std::fill(std::begin(*this), std::end(*this), 0xFF);
        }

        uint16_t value() const
        {
            return *reinterpret_cast<const uint16_t * const>(data());
        }

        void value(uint16_t v)
        {
            *reinterpret_cast<uint16_t *>(data()) = v;
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
    CRCHasher()
    {}

    void calc(uint8_t data)
    {
        uint16_t &crc = *reinterpret_cast<uint16_t *>(m_value.data());

        for(auto i = 0; i < 8; ++i)
        {
            const auto v = (data >> (7 - i)) & 1;
            crc =
                (((crc << 1) + v) & 0xFFFF)
                ^ (((crc >> 15) & 1) * 0x1021);
        }
    };

    const ValueType &value()
    {
        return m_value;
    }
};
/*----------------------------------------------------------------------------*/


#endif /* CRCHasher_h */
