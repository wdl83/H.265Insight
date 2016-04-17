#include <Decoder/ParserHooks.h>
#include <Decoder/State.h>
#include <Syntax/NalUnitHeader.h>

namespace HEVC { namespace Decoder { namespace ParserHooks {
/*----------------------------------------------------------------------------*/
void onStart(Syntax::SliceSegmentData &ssd, State &decoder)
{
    decoder.onStart(ssd);
}

void onStart(Syntax::CodingTreeUnit &ctu, State &decoder)
{
    decoder.onStart(ctu);
}

void onStart(Syntax::CodingUnit &cu, State &decoder)
{
    decoder.onStart(cu);
}

void onStart(Syntax::ResidualCoding &rc, State &decoder)
{
    decoder.onStart(rc);
}
/*----------------------------------------------------------------------------*/
void onFinish(Syntax::StreamNalUnit &snu, State &decoder)
{
    decoder.onFinish(snu);
}
#if 0
void onFinish(Syntax::EndOfBitstreamRbsp &eob, State &decoder)
{
    decoder.onFinish(eob);
}

void onFinish(Syntax::EndOfSeqRbsp &eos, State &decoder)
{
    decoder.onFinish(eos);
}

void onFinish(Syntax::NalUnitHeader &nuh, State &decoder)
{
    decoder.onFinish(nuh);
}
#endif

void onFinish(Syntax::NalUnit &nu, State &decoder)
{
    decoder.onFinish(nu);
}

void onFinish(Syntax::ShortTermRefPicSet &stRPS, State &decoder)
{
    decoder.onFinish(stRPS);
}
#if 0
void onFinish(Syntax::SliceSegmentLayerRbsp &sslRBSP, State &decoder)
{
    decoder.onFinish(sslRBSP);
}
#endif

void onFinish(Syntax::SliceSegmentHeader &ssh, State &decoder)
{
    decoder.onFinish(ssh);
}

void onFinish(Syntax::SliceSegmentHeaderContent::FirstSliceSegmentInPicFlag &flag, State &decoder)
{
    decoder.onFinish(flag);
}

void onFinish(Syntax::SliceSegmentHeaderContent::SlicePicParameterSetId &id, State &decoder)
{
    decoder.onFinish(id);
}

void onFinish(Syntax::SliceSegmentHeaderContent::PicOutputFlag &flag, State &decoder)
{
    decoder.onFinish(flag);
}

void onFinish(Syntax::CodingTreeUnit &ctu, State &decoder)
{
    decoder.onFinish(ctu);
}

void onFinish(Syntax::CodingUnit &cu, State &decoder)
{
    decoder.onFinish(cu);
}

void onFinish(Syntax::PredictionUnit &pu, State &decoder)
{
    decoder.onFinish(pu);
}

void onFinish(Syntax::TransformUnit &tu, State &decoder)
{
    decoder.onFinish(tu);
}

void onFinish(Syntax::ResidualCoding &rc, State &decoder)
{
    decoder.onFinish(rc);
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::ParserHooks */
