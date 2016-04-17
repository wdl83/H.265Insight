#ifndef HEVC_Syntax_PcmSample_h
#define HEVC_Syntax_PcmSample_h

#include <Syntax/Syntax.h>

namespace HEVC { namespace Syntax { namespace PcmSampleContent {
/*----------------------------------------------------------------------------*/
typedef EmbeddedCoord<Pel, ElementId::PcmSampleCoord> Coord;
typedef EmbeddedUnit<Log2, ElementId::PcmSampleSize> Size;
/*----------------------------------------------------------------------------*/
struct PcmSampleLuma:
    public Embedded,
    public VLD::UInt
{
    static const auto Id = ElementId::pcm_sample_luma;

    PcmSampleLuma(int bitDepth)
    {
        setLengthInBits(bitDepth);
    }
};
/*----------------------------------------------------------------------------*/
struct PcmSampleChroma:
    public Embedded,
    public VLD::UInt
{
    static const auto Id = ElementId::pcm_sample_chroma;

    PcmSampleChroma(int bitDepth)
    {
        setLengthInBits(bitDepth);
    }
};
/*----------------------------------------------------------------------------*/
} /* PcmSampleContent */

class PcmSample:
    public EmbeddedAggregator<
        PcmSampleContent::Coord,
        PcmSampleContent::Size,
        PcmSampleContent::PcmSampleLuma,
        PcmSampleContent::PcmSampleChroma>
{
public:
    static const auto Id = ElementId::pcm_sample;

    typedef PcmSampleContent::Coord Coord;
    typedef PcmSampleContent::Size Size;
    typedef PcmSampleContent::PcmSampleLuma PcmSampleLuma;
    typedef PcmSampleContent::PcmSampleChroma PcmSampleChroma;

    PcmSample(PelCoord pcmCoord, Log2 pcmSize)
    {
        embed<Coord>(*this, pcmCoord);
        embed<Size>(*this, pcmSize);
    }

    void onParse(StreamAccessLayer &, Decoder::State &);
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */

#endif /* HEVC_Syntax_PcmSample_h */
