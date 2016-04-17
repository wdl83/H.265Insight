#ifndef HEVC_Structure_Slice_h
#define HEVC_Structure_Slice_h

/* STDC++ */
#include <algorithm>
/* HEVC */
#include <HEVC.h>
#include <Syntax/Fwd.h>
#include <Structure/Fwd.h>
#include <Structure/RPL.h>

namespace HEVC { namespace Structure {
/*----------------------------------------------------------------------------*/
class Slice
{
public:
    typedef Syntax::SliceSegmentHeader SSH;
    typedef Syntax::SliceSegmentData SSD;
    typedef Tuple<CtbAddr, Ptr<SSH>, Ptr<SSD>> Segment;
    typedef VLA<Segment> SegmentList;
private:
    CtbAddr m_addr;
    SegmentList m_segment;
public:
    RPL rpl;

    Slice(CtbAddr ctbAddr):
        m_addr(ctbAddr)
    {}

    Slice(const Slice &) = delete;
    Slice(Slice &&) = default;
    Slice &operator= (const Slice &) = delete;

    CtbAddr addr() const
    {
        return m_addr;
    }

    void add(CtbAddr addr, Ptr<SSH> ssh, Ptr<SSD> ssd)
    {
        m_segment.emplaceBack(addr, ssh, ssd);
    }

    Ptr<SSH> header() const
    {
        return m_segment.front().get<Ptr<SSH>>();
    }

    const SegmentList &segmentList() const
    {
        return m_segment;
    }

    SegmentList::ConstIterator findSegment(Ctb addrInTs) const
    {
        if(m_segment.empty())
        {
            return m_segment.end();
        }

        auto i =
            std::upper_bound(
                    std::begin(m_segment), std::end(m_segment),
                    addrInTs,
                    [](Ctb addr, const Segment &s){return  s.get<CtbAddr>().inTs > addr;});

        return i - 1;
    }

    const Segment &segment(Ctb addrInTs) const
    {
        const auto i = findSegment(addrInTs);
        runtime_assert(i != std::end(m_segment));
        return *i;
    }
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Structure */

#endif /* HEVC_Structure_Slice_h */
