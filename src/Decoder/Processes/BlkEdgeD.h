#ifndef HEVC_Decoder_Processes_BlkEdgeD_h
#define HEVC_Decoder_Processes_BlkEdgeD_h

/* HEVC */
#include <HEVC.h>

namespace HEVC { namespace Decoder {
/*----------------------------------------------------------------------------*/
struct BlkEdgeD
{
    typedef uint16_t Underlying;

    struct Bits
    {
        Underlying dE : 2;
        Underlying dEp : 1;
        Underlying tcPrime : 5;
        Underlying dEq : 1;
        Underlying betaPrime : 7;
    };

    union
    {
        Bits bits;
        Underlying value;
    };

    BlkEdgeD(): value(0)
    {}

    BlkEdgeD(int dE, bool dEp, bool dEq, int tcPrime, int betaPrime):
        value(0)
    {
        bits.dE = dE;
        bits.dEp = dEp;
        bits.tcPrime = tcPrime;
        bits.dEq = dEq;
        bits.betaPrime = betaPrime;
    }

    int dE() const
    {
        return bits.dE;
    }

    bool dEp() const
    {
        return bits.dEp;
    }

    bool dEq() const
    {
        return bits.dEq;
    }

    int tc(int bitDepthY) const
    {
        return bits.tcPrime * (1 << (bitDepthY - 8));
    }

    int beta(int bitDepthY) const
    {
        return bits.betaPrime * (1 << (bitDepthY - 8));
    }

    void toStr(std::ostream &os) const
    {
        os
            << "dE " << dE()
            << " dEp " << dEp()
            << " dEq "<< dEq()
            << " tc' " << bits.tcPrime
            << " b' " << bits.betaPrime;
    }
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Decoder */

#endif /* HEVC_Decoder_Processes_BlkEdgeD_h */
