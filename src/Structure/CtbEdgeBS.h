#ifndef HEVC_Structure_CtbEdgeBS_h
#define HEVC_Structure_CtbEdgeBS_h

/* STDC++ */
#include <array>
/* HEVC */
#include <HEVC.h>

namespace HEVC { namespace Structure {
/*----------------------------------------------------------------------------*/
class CtbEdgeBS
{
public:
    struct BS
    {
        friend std::ostream &operator<< (std::ostream &os, const BS &bs)
        {
            os << (bs.bits.pBypass ? 'B' : ' ');

            if(0 != bs.bits.bs)
            {
                os << int(bs.bits.bs);
            }
            else
            {
                os << '.';
            }

            os << (bs.bits.qBypass ? 'B' : ' ');

            return os;
        }

        static const int sizeInBits = 12;
        typedef uint16_t Underlying;

        struct Bits
        {
            /* 04/2013
             * (8-288)
             * qpL = (pQpY + qQpY + 1) >> 1
             * (8-258)
             * QpY = (QP % (52 + QpBdOffsetY)) - QpBdOffsetY
             * (7-5)
             * QpBdOffsetY = (bitDepthY - 8) * 6 */
            int8_t qpL : 8;
            uint8_t bs : 2;
            uint8_t pBypass : 1;
            uint8_t qBypass : 1;
        };

        union
        {
            Bits bits;
            Underlying value;
        };

        BS(): value(0)
        {}

        BS(int bs_, int pQpY, int qQpY, bool pBypass_, bool qBypass_):
            value(0)
        {
            bits.pBypass = pBypass_;
            bits.qBypass = qBypass_;
            bits.bs = bs_;
            bits.qpL = (pQpY + qQpY + 1) >> 1;
        }

        int bs() const
        {
            return bits.bs;
        }

        bool pBypass() const
        {
            return bits.pBypass;
        }

        bool qBypass() const
        {
            return bits.qBypass;
        }

        bool pqBypass() const
        {
            return bits.pBypass && bits.qBypass;
        }
    };

    /* allowed BS range: [0:2]
     * BS is calculated for each:
     * 4x8 pel segment (vertical edges)
     * 8x4 pel segment (horizontal edges) */
    static const auto ratio = 2;
    static const auto side = Limits::CodingBlock::max >> 2;

    typedef std::array<BS, side * side> Matrix;
    typedef Coord<int> Pos;
private:
    Matrix m_bs;

    Pos::ValueType calcOffset(Pos at) const
    {
        const auto i = at.y() * side + at.x();

        bdryCheck(Pos::ValueType(m_bs.size()) > i);
        return i;
    }
public:
    CtbEdgeBS()
    {}

    BS operator[] (Pos at) const
    {
        return m_bs[calcOffset(at)];
    }

    BS &operator[] (Pos at)
    {
        return m_bs[calcOffset(at)];
    }

    void writeTo(std::ostream &os, Log2 size, EdgeType edgeType) const
    {
        const auto w = toUnderlying(toPel(size - (isV(edgeType) ? 3_log2 : 2_log2)));
        const auto h = toUnderlying(toPel(size - (isH(edgeType) ? 3_log2 : 2_log2)));

        for(auto y = 0; y < h; ++y)
        {
            for(auto x = 0; x < w; ++x)
            {
                os << (*this)[{x, y}] << (w - 1 == x ? '\n' : ' ');
            }
        }
    }
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Structure */

#endif /* HEVC_Structure_CtbEdgeBS_h */
