#ifndef HEVC_Decoder_Processes_CtbMask_h
#define HEVC_Decoder_Processes_CtbMask_h

/* HEVC */
#include <HEVC.h>
#include <BitMatrix.h>

namespace HEVC { namespace Decoder {
/*----------------------------------------------------------------------------*/
class CtbMask
{
public:
    static const auto side = Limits::CodingBlock::max;
    typedef BitMatrix<side, side> FlagMatrix;
    typedef FlagMatrix::Pos Pos;
private:
    FlagMatrix m_flags;
public:
    CtbMask(): m_flags(false)
    {}

    auto operator[] (Pos at) const -> decltype(m_flags[at])
    {
        return m_flags[at];
    }

    auto operator[] (Pos at) -> decltype(m_flags[at])
    {
        return m_flags[at];
    }

    void toStr(std::ostream &os, Log2 hSize, Log2 vSize) const
    {
        const auto hSide = toInt(hSize);
        const auto vSide = toInt(vSize);
        m_flags.toStr(os, hSide, vSide, 'x', ' ');
    }

    CtbMask &operator|= (const CtbMask &other)
    {
        m_flags |= other.m_flags;
        return *this;
    }

    friend CtbMask operator| (const CtbMask &x, const CtbMask &y)
    {
        return CtbMask(x) |= y;
    }
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Decoder */

#endif /* HEVC_Decoder_Processes_CtbMask_h */
