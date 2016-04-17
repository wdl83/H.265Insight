#ifndef HEVC_Structure_CPB_h
#define HEVC_Structure_CPB_h

/* HEVC */
#include <HEVC.h>
#include <Structure/Fwd.h>
#include <Structure/AccessUnit.h>
#include <Structure/DPB.h>
#include <Syntax/Fwd.h>

namespace HEVC  { namespace Structure {
/*----------------------------------------------------------------------------*/
/* 04/2013, 3 "Definitions" */

/* 3.1 access unit:
 *
 * AccessUnit == NALUnit[0..n] ~ Picture
 * set of NAL units that are associated with each other according to a specified
 * classification rule, are cosecutive in decoding order,
 * and contain exactly one coded picture */

/* 3.28 coded video sequence (CVS):
 *
 * CVS == AccessUnit[0..m]
 * CVS: A sequence of access units that consists, in decoding order,
 * of an IRAP access unit with NoRaslOutputFlag equal to 1,
 * followed by zero or more access units that are not IRAP access units
 * with NoRaslOutputFlag equal to 1, including all subsequent access units
 * up to but not including any subsequent access unit that is an IRAP access unit
 * with NoRaslOutputFlag equal to 1.
 *
 * NOTE – An IRAP access unit may be an IDR access unit, a BLA access unit,
 * or a CRA access unit. The value of NoRaslOutputFlag is equal to 1
 * for each IDR access unit, each BLA access unit, and each CRA access unit
 * that is the first access unit in the bitstream in decoding order,
 * is the first access unit that follows an end of sequence NAL unit
 * in decoding order, or has HandleCraAsBlaFlag equal to 1 */

/* 3.95 picture order count:
 * TODO: define POC type
 * POC == int?
 * A variable that is associated with each picture, uniquely identifies
 * the associated picture among all pictures in the CVS, and,
 * when the associated picture is to be output from the decoded picture buffer,
 * indicates the position of the associated picture in output order relative to
 * the output order positions of the other pictures in the same CVS that are
 * to be output from the decoded picture buffer. */

/* CRA - Clean Random Access
 * IDR - Instantaneous Decoding Refresh
 * IRAP - Intra Random Access Point
 * RADL - Random Access Decodable Leading (Picture)
 * RASL - Random Access Skipped Leading (Picture)
 * STSA - Step-wise Temporal Sub-layer Access
 * TSA - Temporal Sub-layer Access
 * VLC - Video Coding Layer */

/*----------------------------------------------------------------------------*/
class CPB
{
    friend class DPB;
protected:
    Handle<AccessUnit> m_au;
public:
    CPB()
    {}

    CPB(const CPB &) = delete;
    CPB &operator= (const CPB &) = delete;

    bool empty() const
    {
        return !m_au;
    }

    void add(Handle<AccessUnit> &&handle)
    {
        runtime_assert(!m_au);
        m_au = std::move(handle);
    }

    void clear()
    {
        destruct(m_au);
    }

    Ptr<AccessUnit> au() const
    {
        return Ptr<AccessUnit>{m_au};
    }

    Ptr<Picture> picture() const
    {
        return au()->picture();
    }

    void moveTo(DPB &dpb)
    {
        dpb.emplaceBack(std::move(m_au));
    }
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Structure */

#endif /* HEVC_Structure_CPB_h */
