#ifndef HEVC_Decoder_Processes_FractionalSamplesBlock_h
#define HEVC_Decoder_Processes_FractionalSamplesBlock_h

/* STDC++ */
#include <array>
#include <iomanip>
/* HEVC */
#include <HEVC.h>

namespace HEVC { namespace Decoder {
/*----------------------------------------------------------------------------*/
/* r - ratio, o - vertical/horizontal offset */
template <int r, int n, int left, int right, int top, int bottom>
class FractionalSamplesBlock
{
public:
    static const Log2 ratio;
    static const Pel side;
    static const Pel leftOffset;
    static const Pel rightOffset;
    static const Pel topOffset;
    static const Pel bottomOffset;
    typedef PelCoord Pos;
private:
    /* side value is set for worst case:
     * Luma
     * left: 3 * 4 = 12, right: 5 * 4 = 20
     * top: 3 * 4 = 12, bottom: 5 * 4 = 20
     * Chroma
     * left: 1 * 8 = 8, right: 3 * 8 = 24
     * top: 1 * 8 = 8, bottom: 3 * 8 = 24
     */
    static const auto width = left + n + right;
    static const auto height = top + n + bottom;
    std::array<int, width * height> m_sample;

    static int calcOffset(Pos at)
    {
        at += Pos{leftOffset, topOffset};

        bdryCheck(0_pel <= at.x());
        bdryCheck(0_pel <= at.y());
        bdryCheck(Pel{width} > at.x());
        bdryCheck(Pel{height} > at.y());
        return toUnderlying(at.y() * Pel{width} + at.x());
    }
public:
    FractionalSamplesBlock()
    {
        /* zero-ing is not required for correct decoding, this is done only
         * for analysis files to be consistent */
        clear();
    }

    int operator[] (Pos at) const
    {
        return m_sample[calcOffset(at)];
    }

    int &operator[] (Pos at)
    {
        return m_sample[calcOffset(at)];
    }

    void clear()
    {
        std::fill(std::begin(m_sample), std::end(m_sample), 0);
    }

    void toStr(std::ostream &os) const
    {
        for(auto y = 0; y < height; ++y)
        {
            if(top == y || top + n == y)
            {
                for(auto i = 0; i < width + width * 8 + 3; ++i) os << '-';
                os << '\n';
            }

            for(auto x = 0; x < width; ++x)
            {
                if(left == x || left + n == x) os << '|' << ' ';
                os
                    << std::hex << std::setw(8) << std::setfill('0')
                    << m_sample[y * width + x]
                    << (width - 1 == x ? '\n' : ' ');
            }
        }
    }
};

template <int r, int n, int left, int right, int top, int bottom>
const Log2 FractionalSamplesBlock<r, n, left, right, top, bottom>::ratio = Log2{r};
template <int r, int n, int left, int right, int top, int bottom>
const Pel FractionalSamplesBlock<r, n, left, right, top, bottom>::side = Pel{n};
template <int r, int n, int left, int right, int top, int bottom>
const Pel FractionalSamplesBlock<r, n, left, right, top, bottom>::leftOffset = Pel{left};
template <int r, int n, int left, int right, int top, int bottom>
const Pel FractionalSamplesBlock<r, n, left, right, top, bottom>::rightOffset = Pel{right};
template <int r, int n, int left, int right, int top, int bottom>
const Pel FractionalSamplesBlock<r, n, left, right, top, bottom>::topOffset = Pel{top};
template <int r, int n, int left, int right, int top, int bottom>
const Pel FractionalSamplesBlock<r, n, left, right, top, bottom>::bottomOffset = Pel{bottom};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Decoder */

#endif /* HEVC_Decoder_Processes_FractionalSamplesBlock_h */
