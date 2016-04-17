#ifndef HEVC_Decoder_Processes_IntraAdjSamples_h
#define HEVC_Decoder_Processes_IntraAdjSamples_h

/* STDC++ */
#include <ostream>
#include <algorithm>
/* HEVC */
#include <HEVC.h>


namespace HEVC { namespace Decoder {
/*----------------------------------------------------------------------------*/
class IntraAdjSamples
{
public:
    typedef Optional<int> AdjSample;
    typedef VLA<AdjSample> Container;
private:
    Pel m_sideLength;
    VLA<AdjSample> m_s;
public:
    IntraAdjSamples():
        m_sideLength(0_pel)
    {}

    IntraAdjSamples(Pel sideLength)
    {
        init(sideLength);
    }
    /* x = -1, y = <-1, nTbS * 2 - 1>
     * y = -1, x = <-1, nTbS * 2 - 1>
     * sideLength = nTbS * 2 */
    void init(Pel sideLength)
    {
        m_sideLength = sideLength;
        m_s.resize(toUnderlying(sideLength) * 2 + 1);
    }

    /* == nTbS * 2 */
    Pel getSideEnd() const
    {
        return m_sideLength;
    }

    AdjSample operator[] (PelCoord offset) const
    {
        return m_s[calcOffset(offset.x(), offset.y())];
    }

    AdjSample &operator[](PelCoord offset)
    {
        return m_s[calcOffset(offset.x(), offset.y())];
    }

    bool areAllAvailable() const
    {
        const auto yes =
            std::all_of(
                    m_s.begin(), m_s.end(),
                    [](const AdjSample &i){return i.isConstructed();});

        return yes;
    }

    bool areAllUnAvailable() const
    {
        const auto no =
            std::any_of(
                    m_s.begin(), m_s.end(),
                    [](const AdjSample &i){return i.isConstructed();});

        return !no;
    }

    template <typename F>
    F vForEach(Pel vOffset = -1_pel, F f = F()) const
    {
        for(auto y = vOffset; y < getSideEnd(); ++y)
        {
            f({-1_pel, y}, m_s[calcOffset(-1_pel, y)]);
        }

        return f;
    }

    template <typename F>
    F vForEach(Pel vOffset = -1_pel, F f = F())
    {
        for(auto y = vOffset; y < getSideEnd(); ++y)
        {
            f({-1_pel, y}, m_s[calcOffset(-1_pel, y)]);
        }

        return f;
    }

    template <typename F>
    F hForEach(Pel hOffset = -1_pel, F f = F()) const
    {
        for(auto x = hOffset; x < getSideEnd(); ++x)
        {
            f({x, -1_pel}, m_s[calcOffset(x, -1_pel)]);
        }

        return f;
    }

    template <typename F>
    F hForEach(Pel hOffset = -1_pel, F f = F())
    {
        for(auto x = hOffset; x < getSideEnd(); ++x)
        {
            f({x, -1_pel}, m_s[calcOffset(x, -1_pel)]);
        }

        return f;
    }

    template <typename F>
    F forEach(F f) const
    {
        return hForEach(0_pel, vForEach(-1_pel, f));
    }

    template <typename F>
    F forEach(F f)
    {
        return hForEach(0_pel, vForEach(-1_pel, f));
    }

    template <typename F>
    void writeTo(std::ostream &os, F f = F()) const
    {
        const auto hF =
            [&](PelCoord at, AdjSample i)
            {
                if(i)
                {
                    f(os, *i);
                }
                else
                {
                    os << '?';
                }
                os << (getSideEnd() - 1_pel == at.x() ? '\n' : ' ');
            };

        const auto vF =
            [&](PelCoord, AdjSample i)
            {
                if(i)
                {
                    f(os, *i);
                }
                else
                {
                    os << '?';
                }

                os <<  '\n';
            };

        hForEach(-1_pel, hF);
        vForEach(0_pel, vF);
    }
protected:
    Container::SizeType calcOffset(Pel x, Pel y) const
    {
        bdryCheck(-1_pel == x || -1_pel == y);

        const auto offset =
             -1_pel == x && -1_pel != y
             ? (m_sideLength - 1_pel) - y
             : (
                     -1_pel != x && -1_pel == y
                     ? m_sideLength + 1_pel + x
                     : m_sideLength /* {-1_pel, -1_pel} */ );

        bdryCheck(0_pel <= offset);
        bdryCheck(m_s.size() > Container::SizeType(toUnderlying(offset)));
        return toUnderlying(offset);
    }
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Decoder */

#endif /* HEVC_Decoder_Processes_IntraAdjSamples_h */
