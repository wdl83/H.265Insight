#ifndef Syntax_AccessUnitDelimiterRbsp_h
#define Syntax_AccessUnitDelimiterRbsp_h

#include <Syntax/Syntax.h>
#include <Syntax/Aggregator.h>
#include <Syntax/VLD/Descriptors.h>
#include <Decoder/Fwd.h>

namespace HEVC { namespace Syntax { namespace AccessUnitDelimiterRbspContent {
/*----------------------------------------------------------------------------*/
class PicType:
    public Embedded,
    public VLD::FixedUInt<3, uint8_t>
{
public:
    static const auto Id = ElementId::pic_type;
};
/*----------------------------------------------------------------------------*/
} /* namespace AccessUnitDelimiterRbspContent */

/*----------------------------------------------------------------------------*/
class AccessUnitDelimiterRbsp:
    public EmbeddedAggregator<AccessUnitDelimiterRbspContent::PicType>,
    public SubtreeAggregator<RbspTrailingBits>
{
public:
    static const auto Id = ElementId::access_unit_delimiter_rbsp;

    typedef AccessUnitDelimiterRbspContent::PicType PicType;

    void onParse(StreamAccessLayer &, Decoder::State &);
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */

#endif /*  Syntax_AccessUnitDelimiterRbsp_h */
