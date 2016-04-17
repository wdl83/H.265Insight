#ifndef HEVC_Structure_CtbEdgeFlags_h
#define HEVC_Structure_CtbEdgeFlags_h

/* HEVC */
#include <HEVC.h>
#include <BitMatrix.h>

namespace HEVC { namespace Structure {
/*----------------------------------------------------------------------------*/
class CtbEdgeFlags
{
public:
    static const auto ratio = 2;
    static const auto side = Limits::CodingBlock::max >> ratio;

    typedef BitMatrix<side, side> FlagMatrix;
    typedef FlagMatrix::Pos Pos;
private:
    FlagMatrix m_flags;
public:
    CtbEdgeFlags(): m_flags(false)
    {}

    auto operator[] (Pos at) const -> decltype(m_flags[at])
    {
        return m_flags[at];
    }

    auto operator[] (Pos at) -> decltype(m_flags[at])
    {
        return m_flags[at];
    }

    void toStr(std::ostream &os, Log2 size, EdgeType) const
    {
        const auto s = toUnderlying(toPel(size - Log2{ratio}));
        m_flags.toStr(os, s, s, '+', '.');
    }

    CtbEdgeFlags &operator|= (const CtbEdgeFlags &other)
    {
        m_flags |= other.m_flags;
        return *this;
    }

    friend
    CtbEdgeFlags operator| (const CtbEdgeFlags &x, const CtbEdgeFlags &y)
    {
        return CtbEdgeFlags(x) |= y;
    }
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Structure */

#endif /* HEVC_Structure_CtbEdgeFlags_h */
