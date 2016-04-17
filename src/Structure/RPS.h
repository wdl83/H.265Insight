#ifndef HEVC_Structure_RPS_h
#define HEVC_Structure_RPS_h

/* STDC++ */
#include <ostream>
/* HEVC */
#include <HEVC.h>

namespace HEVC { namespace Structure {
/*----------------------------------------------------------------------------*/
class RPS
{
public:
    /* 04/2013, 8.3.2 "Decoding process for reference picture set"
     *
     * Note 3:
     * Curr[Before|After] - contain all reference pictures that may be
     * used for INTER prediction of the CURRENT picture and one or more
     * pictures that FOLLOW the CURRENT picture in decoding order.
     *
     * Foll - consist of all reference pictures that are NOT used for
     * INTER prediction of the CURRENT picture but may be used in
     * INTER prediction for one or more pictures that FOLLOW the CURRENT
     * picture in decoding order. */

    struct POC
    {
        template <typename T>
            using StList = std::array<T, Limits::NumShortTermRefPicSets::num>;

        template <typename T>
            using LtList = std::array<T, Limits::NumLongTermRefPicsSps::num>;

        typedef StList<PicOrderCntBase::ValueType> StPOC;
        typedef LtList<PicOrderCntBase::ValueType> LtPOC;
        typedef LtList<bool> LtDeltaMsbPresentFlag;

        StPOC stCurrBefore;
        StPOC stCurrAfter;
        StPOC stFoll;
        LtPOC ltCurr;
        LtPOC ltFoll;
        LtDeltaMsbPresentFlag currDeltaMsbPresentFlag;
        LtDeltaMsbPresentFlag follDeltaMsbPresentFlag;

        int numStCurrBefore;
        int numStCurrAfter;
        int numStFoll;
        int numLtCurr;
        int numLtFoll;

        POC():
            numStCurrBefore(0),
            numStCurrAfter(0),
            numStFoll(0),
            numLtCurr(0),
            numLtFoll(0)
        {
            std::fill(std::begin(stCurrBefore), std::end(stCurrBefore), 0);
            std::fill(std::begin(stCurrAfter), std::end(stCurrAfter), 0);
            std::fill(std::begin(stFoll), std::end(stFoll), 0);
            std::fill(std::begin(ltCurr), std::end(ltCurr), 0);
            std::fill(std::begin(ltFoll), std::end(ltFoll), 0);
            std::fill(std::begin(currDeltaMsbPresentFlag), std::end(currDeltaMsbPresentFlag), false);
            std::fill(std::begin(follDeltaMsbPresentFlag), std::end(follDeltaMsbPresentFlag), false);
        }

        void toStr(std::ostream &os) const
        {
            os << "numStCurrBefore " << numStCurrBefore;

            for(auto i : stCurrBefore)
            {
                os << ' ' << i;
            }

            os << "\nnumStCurrBefore " << numStCurrAfter;

            for(auto i : stCurrAfter)
            {
                os << ' ' << i;
            }

            os << "\nnumStFoll " << numStFoll;

            for(auto i : stFoll)
            {
                os << ' ' << i;
            }

            os << "\nnumLtCurr " << numLtCurr;

            for(auto i : ltCurr)
            {
                os << ' ' << i;
            }

            os << "\nnumLtFoll " << numLtFoll;

            for(auto i : ltFoll)
            {
                os << ' ' << i;
            }

            os << "\ncurrDeltaMsbPresentFlag";

            for(auto i : currDeltaMsbPresentFlag)
            {
                os << ' ' << i;
            }

            os << "\nfollDeltaMsbPresentFlag";

            for(auto i : follDeltaMsbPresentFlag)
            {
                os << ' ' << i;
            }
        }
    };

    typedef std::array<Optional<PicOrderCntVal>, Limits::NumShortTermRefPicSets::num> StList;
    typedef std::array<Optional<PicOrderCntVal>, Limits::NumLongTermRefPicsSps::num> LtList;

    StList stCurrBefore;
    StList stCurrAfter;
    StList stFoll;
    LtList ltCurr;
    LtList ltFoll;

    int numStCurrBefore;
    int numStCurrAfter;
    int numStFoll;
    int numLtCurr;
    int numLtFoll;

    RPS():
        numStCurrBefore(0),
        numStCurrAfter(0),
        numStFoll(0),
        numLtCurr(0),
        numLtFoll(0)
    {}
};
/*----------------------------------------------------------------------------*/
}} // namespace HEVC::Structure

#endif // HEVC_Structure_RPS_h
