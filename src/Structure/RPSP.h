#ifndef HEVC_Structure_RPSP_h
#define HEVC_Structure_RPSP_h

/* STDC++ */
#include <limits>
/* HEVC */
#include <HEVC.h>
#include <Syntax/Fwd.h>

namespace HEVC { namespace Structure {
/*----------------------------------------------------------------------------*/
/* Reference Picture Set Parameters */
class RPSP
{
public:
    template <typename T>
    using List = std::array<T, Limits::MaxDpbSize::value>;

    struct Entry
    {
        List<int> deltaPocS0;
        List<int> deltaPocS1;
        List<bool> usedByCurrPicS0;
        List<bool> usedByCurrPicS1;
        int numNegativePics;
        int numPositivePics;

        Entry()
        {
            std::fill(std::begin(deltaPocS0), std::end(deltaPocS0), 0);
            std::fill(std::begin(deltaPocS1), std::end(deltaPocS1), 0);
            std::fill(std::begin(usedByCurrPicS0), std::end(usedByCurrPicS0), false);
            std::fill(std::begin(usedByCurrPicS1), std::end(usedByCurrPicS1), false);
            numNegativePics = 0;
            numPositivePics = 0;
        }

        int numDeltaPocs() const
        {
            return numNegativePics + numPositivePics;
        }
    };
private:
    std::array<Entry, Limits::NumShortTermRefPicSets::num> m_entry;

    Entry &operator[] (int i)
    {
        return m_entry[i];
    }

public:
    List<int> pocLsbLt;
    List<int> deltaPocMsbCycleLt;
    List<bool> usedByCurrPicLt;
    int numPocTotalCurr;

    RPSP():
        numPocTotalCurr(0)
    {}

    RPSP(const RPSP &) = delete;
    RPSP &operator= (const RPSP &) = delete;

    void derive(const Syntax::ShortTermRefPicSet &);
    void derive(const Syntax::SequenceParameterSet &);
    void derive(
            const Syntax::SequenceParameterSet &,
            const Syntax::SliceSegmentHeader &);

    const Entry &entry(int i) const
    {
        return m_entry[i];
    }
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Structure */

#endif /* HEVC_Structure_RPSP_h */
