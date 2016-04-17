#include <Decoder/Processes/InterPrediction.h>
#include <Decoder/Processes/MvDerivation.h>
#include <Decoder/Processes/InterSamplesPrediction.h>
#include <Syntax/PredictionUnit.h>
#include <log.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
void InterPrediction::exec(
        State &decoder,
        Ptr<Structure::Picture> picture,
        Syntax::PredictionUnit &pu)
{
    using namespace Syntax;

    typedef PredictionUnit PU;

    /* 8.5.3.2 Derivation process for motion vector components and reference indices */
    Pair<MotionVector, RefList> mvL;
    Pair<PicOrderCntVal, RefList> refIdxL;
    Pair<bool, RefList> predFlagL;

    tie(mvL, refIdxL, predFlagL) = subprocess(decoder, MvDerivationLuma(), picture, pu);
    const auto mvCL = subprocess(decoder, MvDeriveChroma(), picture, mvL);

    embed<PU::MvLX>(pu, mvL);
    embed<PU::RefIdxLX>(pu, refIdxL);
    embed<PU::PredFlagLX>(pu, predFlagL);
    embed<PU::MvCLX>(pu, mvCL);

    /* 8.5.3.3 Decoding process for inter prediction samples */
    process(decoder, InterSamplesPrediction(), picture, pu);
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processes */
