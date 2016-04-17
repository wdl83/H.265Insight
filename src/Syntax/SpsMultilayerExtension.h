#ifndef HEVC_Syntax_SpsMultilayerExtension_h
#define HEVC_Syntax_SpsMultilayerExtension_h

/* HEVC */
#include <Syntax/Syntax.h>

namespace HEVC { namespace Syntax { namespace SpsMultilayerExtensionContent {
/*----------------------------------------------------------------------------*/
struct InterViewMvVertConstaintFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::inter_view_mv_vert_constraint_flag;
};
/*----------------------------------------------------------------------------*/
} /* spsMultilayerExtensionContent */
/*----------------------------------------------------------------------------*/
struct SpsMultilayerExtension:
    public EmbeddedAggregator<
        SpsMultilayerExtensionContent::InterViewMvVertConstaintFlag>
{
    typedef SpsMultilayerExtensionContent::InterViewMvVertConstaintFlag InterViewMvVertConstaintFlag;

    static const auto Id = ElementId::sps_multilayer_extension;

    void onParse(StreamAccessLayer &, Decoder::State &);
};
/*----------------------------------------------------------------------------*/
}} // HEVC::Syntax

#endif /* HEVC_Syntax_SpsMultilayerExtension_h */
