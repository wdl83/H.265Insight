#ifndef HEVC_Decoder_Processes_MvSpatialMergeCandidate_h
#define HEVC_Decoder_Processes_MvSpatialMergeCandidate_h

/* STDC++ */
#include <algorithm>
/* HEVC */
#include <HEVC.h>
#include <Decoder/Fwd.h>
#include <Decoder/Processes/ProcessId.h>
#include <Syntax/Fwd.h>
#include <Structure/Fwd.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
/* 04/2013, 8.5.3.2.2 "Derivation process for spatial merging candidates" */
/*----------------------------------------------------------------------------*/

struct MvSpatialMergeCand
{
    enum Location
    {
        A0, A1, B0, B1, B2, Num
    };

    template <typename T>
    using List = std::array<T, Num>;

    List<bool> availableFlag;
    List<Pair<PicOrderCntVal, RefList>> refIdxL;
    List<Pair<bool, RefList>> predFlagL;
    List<Pair<MotionVector, RefList>> mvL;

    MvSpatialMergeCand()
    {
        std::fill(std::begin(availableFlag), std::end(availableFlag), false);
        std::fill(
                std::begin(refIdxL), std::end(refIdxL),
                Pair<PicOrderCntVal, RefList>{PicOrderCntVal{-1}, PicOrderCntVal{-1}});
        std::fill(
                std::begin(predFlagL), std::end(predFlagL),
                Pair<bool, RefList>{false, false});
    }
};

struct MvSpatialMergeCandidate
{
    static const auto id = ProcessId::MvSpatialMergeCandidate;

    MvSpatialMergeCand exec(
            State &, Ptr<const Structure::Picture>,
            Ptr<const Syntax::CodingUnit>,
            PelCoord, Pel, Pel, int);
};
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */

#endif /* HEVC_Decoder_Processes_MvSpatialMergeCandidate_h */
