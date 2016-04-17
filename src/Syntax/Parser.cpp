#include <Syntax/Parser.h>
#include <Structure/Picture.h>
#include <Decoder/State.h>

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
CABAD::State &cabadState(Decoder::State &decoder)
{
    return decoder.picture()->getCabadState();
}
/*----------------------------------------------------------------------------*/
ParserStats::~ParserStats()
{
    auto &stats = m_decoder.snu->stats[m_id];

    ++stats.count;
    stats.duration +=
        std::chrono::duration_cast<StreamNalUnit::ElementStats::Duration>(
                std::chrono::high_resolution_clock::now() - m_begin);
}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */
