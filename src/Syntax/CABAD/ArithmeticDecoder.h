#ifndef CABAD_ArithmeticDecoder_h
#define CABAD_ArithmeticDecoder_h
/* STDC++ */
#include <cstdint>
#include <array>
/* HEVC */
#include <HEVC.h>
#include <Fwd.h>
#include <Syntax/check.h>
#include <Syntax/CABAD/Variable.h>
#include <log.h>

namespace HEVC { namespace Syntax { namespace CABAD {
/*----------------------------------------------------------------------------*/
class ArithmeticDecoder
{
public:
    static const int maxRange = 510;
private:
    uint16_t m_codIRange;
    uint16_t m_codIOffset;
    static const std::array<std::array<uint8_t, 4>, 64> m_rangeTabLPS;
    static const std::array<uint8_t, 64> m_transIdxLPS;
    static const std::array<uint8_t, 64> m_transIdxMPS;
public:
    ArithmeticDecoder():
        m_codIRange(0), m_codIOffset(0)
    {}

    ArithmeticDecoder(const ArithmeticDecoder &) = default;
    ArithmeticDecoder(ArithmeticDecoder &&) = delete;
    ArithmeticDecoder &operator= (const ArithmeticDecoder &) = default;
    void init(StreamAccessLayer &);
    void align();
    bool decodeBin(StreamAccessLayer &);
    bool decodeBin(StreamAccessLayer &, Variable &);
    bool decodeBin(StreamAccessLayer &, Variable *);

    uint16_t codeIRange() const
    {
        return m_codIRange;
    }

    uint16_t codeIOffset() const
    {
        return m_codIOffset;
    }
protected:
    bool decodeBin(const Variable &);
    void updateState(Variable &, bool) const;
    void renormalize(StreamAccessLayer &);
};
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Syntax::CABAD */

#endif /* CABAD_ArithmeticDecoder_h */
