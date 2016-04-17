#ifndef HEVC_Syntax_CABAD_Variable_h
#define HEVC_Syntax_CABAD_Variable_h

/* STDC++ */
#include <cstdint>
#include <string>
#include <ostream>
#include <iomanip>
#include <sstream>
/* HEVC */
#include <HEVC.h>
#include <Syntax/CABAD/Init.h>

namespace HEVC { namespace Syntax { namespace CABAD {
/*----------------------------------------------------------------------------*/
class Variable
{
public:
    /* MPS - most probable symbol
     * LPS - least probable symbol */
    typedef bool Mps;
    /* pLPS - probability of LPS
     * {pLPS(pStateIdx) | 0 <= pState < 64}
     * pStateIdx == 0 corresponds to and LPS = 0.5
     * (probability decreases with increasing pStateIdx) */
    typedef uint8_t PStateIdx;
private:
    Mps m_valMps;
    PStateIdx m_pStateIdx;
public:
    Variable(): m_valMps(false), m_pStateIdx(0)
    {}

    Variable(Mps valMps, PStateIdx pStateIdx):
        m_valMps(valMps), m_pStateIdx(pStateIdx)
    {}

    Mps getValMps() const
    {
        return m_valMps;
    }

    void setValMps(Mps valMps)
    {
        m_valMps = valMps;
    }

    PStateIdx getPStateIdx() const
    {
        return m_pStateIdx;
    }

    void setPStateIdx(PStateIdx pStateIdx)
    {
        m_pStateIdx = pStateIdx;
    }

    void init(InitValue initValue, int sliceQpY)
    {
        /* slopeIdx, intersecIdx - 4bit values */
        const int slopeIdx = initValue >> 4;
        const int intersecIdx = initValue & 15;
        const int m = slopeIdx * 5 - 45;
        const int n = (intersecIdx << 3) - 16;
        const uint8_t preCtxState =
            clip3(1, 126, ((m * clip3(0, 51, sliceQpY)) >> 4) + n);

        setValMps(preCtxState <= 63 ? false : true);
        setPStateIdx(m_valMps ? preCtxState - 64 : 63 - preCtxState);
    }

    operator std::string () const
    {
        std::stringstream ss;
        ss << *this;

        return ss.str();
    }

    friend
    std::ostream &operator << (std::ostream &os, const Variable &variable)
    {
        /* Draft 10v25, 9.2.3.2, "Arithmetic decoding engine",
         * allowed ranges:
         * valMps [0:1]
         * pStateIdx [0:63] */
        os
            << std::setw(1) << variable.getValMps()
            << ':'
            << std::hex << std::setw(2) << std::setfill('0') << std::uppercase
            << int(variable.getPStateIdx());

        return os;
    }
};

struct NonAdaptiveVariable: public Variable
{
public:
    NonAdaptiveVariable(): Variable(0, 63)
    {}
};
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Syntax::CABAD */

#endif /* HEVC_Syntax_CABAD_Variable_h */
