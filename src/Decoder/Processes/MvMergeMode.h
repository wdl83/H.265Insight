#ifndef HEVC_Decoder_Processes_MvMergeMode_h
#define HEVC_Decoder_Processes_MvMergeMode_h

/* STDC++ */
#include <array>
#include <sstream>
/* HEVC */
#include <HEVC.h>
#include <Decoder/Fwd.h>
#include <Decoder/Processes/ProcessId.h>
#include <Syntax/Fwd.h>
#include <Structure/Fwd.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
/* 04/2013, 8.5.3.2.1 "Derivation process for luma motion vectors for merge mode" */
/*----------------------------------------------------------------------------*/
struct MvMergeCandList
{
    static const auto size = 6;

    template <typename T>
    using List = std::array<T, size>;

    List<Pair<PicOrderCntVal, RefList>> refIdxL;
    List<Pair<bool, RefList>> predFlagL;
    List<Pair<MotionVector, RefList>> mvL;

    int numCurrMergeCand;

    MvMergeCandList():
        numCurrMergeCand(0)
    {
        std::fill(
                std::begin(refIdxL), std::end(refIdxL),
                Pair<PicOrderCntVal, RefList>{PicOrderCntVal{-1}, PicOrderCntVal{-1}});
        std::fill(
                std::begin(predFlagL), std::end(predFlagL),
                Pair<bool, RefList>{false, false});
    }

    operator std::string () const
    {
        std::ostringstream oss;

        for(auto i : EnumRange<RefList>())
        {
            for(auto n = 0; n < numCurrMergeCand; ++n)
            {
                oss
                    << 'L' << int(i) << ' '
                    << mvL[n][i] << ' '
                    << refIdxL[n][i] << ' '
                    << predFlagL[n][i] << '\n';
            }
        }

        return oss.str();
    }
};

struct MvMergeMode
{
    static const auto id = ProcessId::MvMergeMode;

    // mvL, refIdxL, predFlagL
    Tuple<Pair<MotionVector, RefList>, Pair<PicOrderCntVal, RefList>, Pair<bool, RefList>> exec(
            State &, Ptr<const Structure::Picture>,
            const Syntax::PredictionUnit &);
};
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */

#endif /* HEVC_Decoder_Processes_MvMergeMode_h */
