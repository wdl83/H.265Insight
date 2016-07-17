#ifndef HEVC_Structure_Tile_h
#define HEVC_Structure_Tile_h

/* STDC++ */
#include <algorithm>
/* HEVC */
#include <HEVC.h>
#include <Structure/Fwd.h>
#include <Structure/Slice.h>

namespace HEVC { namespace Structure {
/*----------------------------------------------------------------------------*/
class Tile
{
public:
    typedef VLA<Ptr<Slice>> SliceList;
private:
    int m_id;
    Range<Ctb> m_addrInTs;
    SliceList m_slice;
public:
    Tile(int id_, Range<Ctb> addrInTs):
        m_id{id_},
        m_addrInTs{addrInTs}
    {}

    Tile(const Tile &) = delete;
    Tile(Tile &&) = default;
    Tile &operator= (const Tile &) = delete;

    int id() const
    {
        return m_id;
    }

    Range<Ctb> bdryInTs() const
    {
        return m_addrInTs;
    }

    bool encloses(Ctb addrInTs) const
    {
        return m_addrInTs.encloses(addrInTs);
    }

    const SliceList &sliceList() const
    {
        return m_slice;
    }

    SliceList::ConstIterator findSlice(Ctb addrInTs) const
    {
        if(m_slice.empty())
        {
            return m_slice.end();
        }

        auto i =
            std::upper_bound(
                    std::begin(m_slice), std::end(m_slice),
                    addrInTs,
                    [](Ctb addr, Ptr<Slice> s){return s->addr().inTs > addr;});
        return i - 1;
    }

    Ptr<Slice> slice(Ctb addrInTs) const
    {
        bdryCheck(encloses(addrInTs));
        const auto i = findSlice(addrInTs);
        bdryCheck(i != std::end(m_slice));
        return *i;
    }

    void add(Ptr<Slice> slice)
    {
        bdryCheck(bdryInTs().end() > slice->addr().inTs);
        m_slice.pushBack(slice);
    }
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Structure */

#endif /* HEVC_Structure_Tile_h */
