#ifndef HEVC_Structure_DPB_h
#define HEVC_Structure_DPB_h

/* STDC++ */
#include <ostream>
#include <vector>
/* HEVC */
#include <HEVC.h>
#include <Structure/Fwd.h>
#include <Structure/AccessUnit.h>
#include <Syntax/Fwd.h>

namespace HEVC { namespace Structure {
/*----------------------------------------------------------------------------*/
class DPB
{
public:
    struct Entry
    {
        Handle<AccessUnit> accessUnit;
        int picLatencyCount;
        bool neededForOutput;

        Entry():
            picLatencyCount{0},
            neededForOutput{false}
        {}

        Entry(Handle<AccessUnit> &&au, bool output):
            accessUnit(std::move(au)),
            picLatencyCount(0),
            neededForOutput(output)
        {}

        Ptr<AccessUnit> au() const
        {
            return Ptr<AccessUnit>{accessUnit};
        }
    };

    typedef std::vector<Entry> List;
private:
    List m_list;
    int m_maxNumReorderPics;
    int m_maxLatencyIncreasePlus1;
    int m_maxDecPicBufferingMinus1;
    PicOrderCntSet m_prevTemporalId0;
    int64_t m_outputCntr;
public:
    const PictureOutput pictureOutput;

    DPB(PictureOutput pictureOutput_):
        m_maxNumReorderPics(0),
        m_maxLatencyIncreasePlus1(0),
        m_maxDecPicBufferingMinus1(-1),
        m_outputCntr{0},
        pictureOutput{pictureOutput_}
    {
        //m_list.reserve(Limits::MaxDpbSizeTraits::value);
    }

    ~DPB()
    {
        runtime_assert(empty());
    }

    int capacity() const
    {
        return m_maxDecPicBufferingMinus1 + 1;
    }

    int size() const
    {
        return m_list.size();
    }

    List::const_iterator begin() const
    {
        return m_list.begin();
    }

    List::iterator begin()
    {
        return m_list.begin();
    }

    List::const_reverse_iterator rbegin() const
    {
        return m_list.rbegin();
    }

    List::reverse_iterator rbegin()
    {
        return m_list.rbegin();
    }

    List::const_iterator end() const
    {
        return m_list.end();
    }

    List::iterator end()
    {
        return m_list.end();
    }

    List::const_reverse_iterator rend() const
    {
        return m_list.rend();
    }

    List::reverse_iterator rend()
    {
        return m_list.rend();
    }

    void setup(const Syntax::SequenceParameterSet &);
    /* 04/2013, C.5.2.3 "Picture decoding, marking, additional bumping, and storage" */
    void emplaceBack(Handle<AccessUnit> &&);
    void erase(List::iterator);
    void clear();
    void flush();

    bool reorderOverflow() const;
    bool latencyOverflow() const;
    bool bufferOverflow() const;

    bool empty() const
    {
        return 0 == size();
    }

    void mark(PicOrderCntVal, RefPicType);
    void markAll(RefPicType);

    Ptr<Picture> picture(PicOrderCntVal) const;
    Ptr<Picture> picture(PicOrderCntLsb) const;

    PicOrderCntSet prevTemporalId0() const
    {
        return m_prevTemporalId0;
    }

    bool isLongTerm(PicOrderCntVal) const;
    bool isShortTerm(PicOrderCntVal) const;


    void eraseUnused();
    /* 04/2013, C.5.2.4 ""Bumping" process" */
    void bump();

    void toStr(std::ostream &) const;
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Structure */

#endif /* HEVC_Structure_DPB_h */
