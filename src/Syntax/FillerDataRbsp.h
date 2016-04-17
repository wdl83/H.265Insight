#ifndef SyntaxFillerDataRbsp_h
#define SyntaxFillerDataRbsp_h

#include <Syntax/Syntax.h>
#include <Syntax/RbspTrailingBits.h>

namespace HEVC { namespace Syntax { namespace FillerDataRbspContent {
/*----------------------------------------------------------------------------*/
class FfByte:
    public Embedded,
    public VLD::FixedPattern<1, 1, 1, 1, 1, 1, 1, 1 /* 0xFF */>
{
public:
    static const auto Id = ElementId::ff_byte;

    enum
    {
        Value = 0xFF
    };
};
/*----------------------------------------------------------------------------*/
} /* namespace FillerDataRbspContent */

/*----------------------------------------------------------------------------*/
class FillerDataRbsp:
    public EmbeddedAggregator<
        FillerDataRbspContent::FfByte>,
    public SubtreeAggregator<
        RbspTrailingBits>
{
public:
    static const auto Id = ElementId::filler_data_rbsp;

    typedef FillerDataRbspContent::FfByte FfByte;

    void onParse(StreamAccessLayer &, Decoder::State &);
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */

#endif /*  SyntaxFillerDataRbsp_h */
