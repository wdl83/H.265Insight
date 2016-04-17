#ifndef HEVC_Decoder_Processes_IntraAngularPrediction_h
#define HEVC_Decoder_Processes_IntraAngularPrediction_h

/* HEVC */
#include <HEVC.h>
#include <Decoder/Fwd.h>
#include <Decoder/Processes/ProcessId.h>
#include <Decoder/Processes/IntraAdjSamples.h>
#include <Structure/Fwd.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
class ReferenceSamples
{
public:
    typedef HEVC::Sample Sample;
    typedef VLA<Sample> Container;
private:
    Range<Pel> m_range;
    Container m_refSamples;
private:
    Container::SizeType calcOffset(Pel at) const
    {
        runtime_assert(m_range.encloses(at));
        const Container::SizeType offset = toUnderlying(at - m_range.begin());

        return offset;
    }
public:
    ReferenceSamples(Range<Pel> range):
        m_range(range),
        m_refSamples(toUnderlying(m_range.length()))
    {}

    Sample operator[] (Pel at) const
    {
        return m_refSamples[calcOffset(at)];
    }

    Sample &operator[] (Pel at)
    {
        return m_refSamples[calcOffset(at)];
    }

    Range<Pel> getRange() const
    {
        return m_range;
    };

    Pel getLength() const
    {
        return m_range.length();
    }

    template <typename F>
    F forEach(Pel offset, F f = F()) const
    {
        for(auto x = offset; x < getRange().end(); ++x)
        {
            f(x, (*this)[x]);
        }

        return f;
    }

    template <typename F>
    void writeTo(std::ostream &os, F f = F()) const
    {
        const auto ff =
            [&](Pel at, Sample i)
            {
                if(0_pel == at)
                {
                    os << '|';
                    f(os, i);
                    os << '|';
                }
                else
                {
                    f(os, i);
                }

                os << (getRange().end() - 1_pel == at ? '\n' : ' ');
            };

        forEach(getRange().begin(), ff);
    }
};
/*----------------------------------------------------------------------------*/
struct IntraAngularPrediction
{
    static const auto id = ProcessId::IntraAngularPrediction;

    void exec(
            State &,
            Structure::PelBuffer &,
            PelCoord, Log2, Plane, int,
            IntraPredictionMode,
            bool,
            const IntraAdjSamples &);
};
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processses */

#endif /* HEVC_Decoder_Processes_IntraAngularPrediction_h */
