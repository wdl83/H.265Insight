#ifndef HEVC_Decoder_Processes_MvDerivation_h
#define HEVC_Decoder_Processes_MvDerivation_h

/* HEVC */
#include <HEVC.h>
#include <Decoder/Process.h>
#include <Syntax/Fwd.h>
#include <Structure/Fwd.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
/* 04/2013, 8.5.3.2 "Derivation process for motion vector components and reference indices" */
/*----------------------------------------------------------------------------*/
struct MvDerivationLuma
{
    static const auto id = ProcessId::MvDerivationLuma;

    // mvL, refIdxL, predFlagL
    Tuple<Pair<MotionVector, RefList>, Pair<PicOrderCntVal, RefList>, Pair<bool, RefList>> exec(
            State &, Ptr<const Structure::Picture>,
            const Syntax::PredictionUnit &);
};
/*----------------------------------------------------------------------------*/
/* 04/2013, 8.5.3.2.9 "Derivation process for chroma motion vectors" */
/*----------------------------------------------------------------------------*/
struct MvDeriveChroma
{
    static const auto id = ProcessId::MvDerivationChroma;

    Pair<MotionVector, RefList> exec(
            State &, Ptr<const Structure::Picture>,
            Pair<MotionVector, RefList>);
};
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */

#endif /* HEVC_Decoder_Processes_MvDerivation_h */
