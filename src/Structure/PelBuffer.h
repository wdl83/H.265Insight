#ifndef HEVC_Structure_PelBuffer_h
#define HEVC_Structure_PelBuffer_h

/* STDC++ */
#include <cstddef>
#include <cstdint>
#include <utility>
#include <ostream>
#include <array>
/* HEVC */
#include <HEVC.h>

namespace HEVC { namespace Structure {
/*----------------------------------------------------------------------------*/
class PelBuffer
{
public:
    typedef VLM<Sample> Container;
private:
    typedef Container::Pos Pos;
    Container m_samples;
public:
    PelBuffer()
    {}

    void resize(Pel w, Pel h)
    {
        m_samples.resize(
                Container::SizeType(toUnderlying(w)),
                Container::SizeType(toUnderlying(h)),
                Sample(0));
    }

    decltype(m_samples.capacity()) capacity() const
    {
        return m_samples.capacity();
    }

    template <size_t n>
    std::array<Sample, n> vStripe(PelCoord at) const
    {
        const Pos base(toUnderlying(at.x()), toUnderlying(at.y()));
        std::array<Sample, n> r;

        for(size_t i = 0; i < n; ++i)
        {
            r[i] = m_samples[base + Pos(0, i)];
        }

        return r;
    }

    template <size_t n>
    void vStripe(PelCoord at, const std::array<Sample, n> &s)
    {
        const Pos base(toUnderlying(at.x()), toUnderlying(at.y()));
        for(size_t i = 0; i < n; ++i)
        {
            m_samples[base + Pos(0, i)] = s[i];
        }
    }

    template <size_t n>
    std::array<Sample, n> hStripe(PelCoord at) const
    {
        const Pos base(toUnderlying(at.x()), toUnderlying(at.y()));
        std::array<Sample, n> r;

        for(size_t i = 0; i < n; ++i)
        {
            r[i] = m_samples[base + Pos(i, 0)];
        }

        return r;
    }

    template <size_t n>
    void hStripe(PelCoord at, const std::array<Sample, n> &s)
    {
        const Pos base(toUnderlying(at.x()), toUnderlying(at.y()));

        for(size_t i = 0; i < n; ++i)
        {
            m_samples[base + Pos(i, 0)] = s[i];
        }
    }

    Sample operator[] (PelCoord at) const
    {
        return m_samples[Pos(toUnderlying(at.x()), toUnderlying(at.y()))];
    }

    Sample &operator[] (PelCoord at)
    {
        return m_samples[Pos(toUnderlying(at.x()), toUnderlying(at.y()))];
    }

    void toStr(std::ostream &os, PelRect bdry) const
    {
        const auto yBegin = bdry.topLeft().y();
        const auto yEnd = yBegin + bdry.height();
        const auto xBegin = bdry.topLeft().x();
        const auto xEnd = xBegin + bdry.width();

        for(auto y = yBegin; y < yEnd; ++y)
        {
            for(auto x = xBegin; x < xEnd; ++x)
            {
                pelFmt(os, (*this)[{x, y}]);
                os << (xEnd - 1_pel == x ? '\n' : ' ');
            }
        }
    }

    template <typename F>
    void writeTo(std::ostream &os, F f = F()) const
    {
        for(const auto i : m_samples)
        {
            f(os, i);
        }
    }
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Structure */

#endif /* HEVC_Structure_PelBuffer_h */
