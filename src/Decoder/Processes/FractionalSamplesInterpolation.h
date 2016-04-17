#ifndef HEVC_Decoder_Processes_FractionalSamplesInterpolation_h
#define HEVC_Decoder_Processes_FractionalSamplesInterpolation_h

/* HEVC */
#include <HEVC.h>
#include <Decoder/Fwd.h>
#include <Decoder/Processes/ProcessId.h>
#include <Decoder/Processes/FractionalSamplesBlock.h>
#include <Syntax/Fwd.h>
#include <Structure/Fwd.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
class PredSampleLx
{
    Pel m_width;
    Pel m_height;
    VLA<int> m_samples;

    int calcOffset(PelCoord at) const
    {
        return toUnderlying(at.y() * m_width + at.x());
    }
public:
    typedef int ValueType;

    PredSampleLx()
    {}

    PredSampleLx(Pel w, Pel h):
        m_width{w}, m_height{h},
        m_samples(toUnderlying(w * h), 0)
    {}

    ValueType operator[] (PelCoord at) const
    {
        return m_samples[calcOffset(at)];
    }

    ValueType &operator[] (PelCoord at)
    {
        return m_samples[calcOffset(at)];
    }
};

struct FractionalSamplesInterpolation
{
    static const auto id = ProcessId::FractionalSamplesInterpolation;
    static const auto lumaSide = 16;
    static const auto chromaSide = 8;
    typedef FractionalSamplesBlock<2, lumaSide, 3, 5, 3, 5> BlkLuma;
    typedef FractionalSamplesBlock<3, chromaSide, 1, 3, 1, 3> BlkChroma;

    PredSampleLx exec(
            State &, Ptr<Structure::Picture>,
            Ptr<const Structure::Picture>,
            const Syntax::PredictionUnit &,
            Plane plane, Range<Pel> h, Range<Pel> v,
            RefList);
};
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */

#endif /* HEVC_Decoder_Processes_FractionalSamplesInterpolation_h */
