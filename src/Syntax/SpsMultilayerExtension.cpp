#include <Syntax/SpsMultilayerExtension.h>


namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
void SpsMultilayerExtension::onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder)
{
    parse(streamAccessLayer, decoder, *embed<InterViewMvVertConstaintFlag>(*this));
}
/*----------------------------------------------------------------------------*/
}} // HEVC::Syntax
