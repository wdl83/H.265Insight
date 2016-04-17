#ifndef HEVC_Decoder_ParserHooks_h
#define HEVC_Decoder_ParserHooks_h

#include <Syntax/ElementId.h>
#include <Syntax/Fwd.h>
#include <Decoder/Fwd.h>

namespace HEVC { namespace Decoder { namespace ParserHooks {
/*----------------------------------------------------------------------------*/
template <typename T>
inline
void onStart(const T &, State &)
{}

template <typename T>
inline
void onFinish(const T &, State &)
{}

void onStart(Syntax::SliceSegmentData &, State &);
void onStart(Syntax::CodingTreeUnit &, State &);
void onStart(Syntax::CodingUnit &, State &);
void onStart(Syntax::ResidualCoding &, State &);

void onFinish(Syntax::StreamNalUnit &, State &);
//void onFinish(Syntax::EndOfBitstreamRbsp &, State &);
//void onFinish(Syntax::EndOfSeqRbsp &, State &);
//void onFinish(Syntax::NalUnitHeader &, State &);
void onFinish(Syntax::NalUnit &, State &);
void onFinish(Syntax::ShortTermRefPicSet &, State &);
//void onFinish(Syntax::SliceSegmentLayerRbsp &, State &);
void onFinish(Syntax::SliceSegmentHeader &, State &);
void onFinish(Syntax::SliceSegmentHeaderContent::FirstSliceSegmentInPicFlag &, State &);
void onFinish(Syntax::SliceSegmentHeaderContent::SlicePicParameterSetId &, State &);
void onFinish(Syntax::SliceSegmentHeaderContent::PicOutputFlag &, State &);
void onFinish(Syntax::CodingTreeUnit &, State &);
void onFinish(Syntax::CodingUnit &, State &);
void onFinish(Syntax::PredictionUnit &, State &);
void onFinish(Syntax::TransformUnit &, State &);
void onFinish(Syntax::ResidualCoding &, State &);
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::ParserHooks */

#endif /* HEVC_Decoder_ParserHooks_h */
