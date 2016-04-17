#include <Syntax/SpsRangeExtension.h>

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
void SpsRangeExtension::onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder)
{
    parse(streamAccessLayer, decoder, *embed<TransformSkipRotationEnabledFlag>(*this));
    parse(streamAccessLayer, decoder, *embed<TransformSkipContextEnabledFlag>(*this));
    parse(streamAccessLayer, decoder, *embed<ImplicitRdpcmEnabledFlag>(*this));
    parse(streamAccessLayer, decoder, *embed<ExplicitRdpcmEnabledFlag>(*this));
    parse(streamAccessLayer, decoder, *embed<ExtendedPrecisionProcessingFlag>(*this));
    parse(streamAccessLayer, decoder, *embed<IntraSmoothingDisabledFlag>(*this));
    parse(streamAccessLayer, decoder, *embed<HighPrecisionOffsetsEnabledFlag>(*this));
    parse(streamAccessLayer, decoder, *embed<PersistentRiceAdaptationEnabledFlag>(*this));
    parse(streamAccessLayer, decoder, *embed<CabacBypassAlignmentEnabledFlag>(*this));
}
/*----------------------------------------------------------------------------*/
}} // HEVC::Syntax
