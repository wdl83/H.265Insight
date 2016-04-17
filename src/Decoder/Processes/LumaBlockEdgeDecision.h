#ifndef HEVC_Decoder_Processes_LumaBlockEdgeDecision_h
#define HEVC_Decoder_Processes_LumaBlockEdgeDecision_h

/* HEVC */
#include <HEVC.h>
#include <Decoder/Fwd.h>
#include <Decoder/Processes/ProcessId.h>
#include <Decoder/Processes/BlkEdgeD.h>
#include <Syntax/Fwd.h>
#include <Structure/Fwd.h>
#include <Structure/CtbEdgeBS.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
struct LumaBlockEdgeDecision
{
    static const auto id = ProcessId::LumaBlockEdgeDecision;

    BlkEdgeD exec(
            State &, Ptr<const Structure::Picture>,
            PelCoord,
            EdgeType,
            Structure::CtbEdgeBS::BS);
};
/*----------------------------------------------------------------------------*/
// 04/2013, 8.7.2.5.3 "Decision process for luma block edges", Table 8-11
inline
int deriveBetaPrime(int q)
{
    static const uint8_t betaPrime[] =
    {
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  6,  7,  8,  9, 10, 11, 12, 13,
        14, 15, 16, 17, 18, 20, 22, 24, 26, 28, 30, 32,
        34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56,
        58, 60, 62, 64
    };

    bdryCheck(0 <= q && int(lengthOf(betaPrime)) > q);
    return betaPrime[q];
}

// 04/2013, 8.7.2.5.3 "Decision process for luma block edges", Table 8-11
inline
int deriveTcPrime(int q)
{
    static const uint8_t tcPrime[] =
    {
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  2,  2,  2,  2,  3,  3,  3,  3,  4,
         4,  4,  5,  5,  6,  6,  7,  8,  9, 10, 11, 13,
        14, 16, 18, 20, 22, 24
    };

    bdryCheck(0 <= q && int(lengthOf(tcPrime)) > q);
    return tcPrime[q];
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */

#endif /* HEVC_Decoder_Processes_LumaBlockEdgeDecision_h */
