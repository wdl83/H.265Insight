#include <Decoder/Process.h>
#include <Decoder/State.h>
#include <Structure/AccessUnit.h>

namespace HEVC { namespace Decoder {
/*----------------------------------------------------------------------------*/
bool isMasked(const State &decoder, Processes::ProcessId id)
{
    return decoder.isMasked(id);
}
/*----------------------------------------------------------------------------*/
ProcessingTimes::~ProcessingTimes()
{
    auto &stats = m_decoder.currAU()->stats[m_processId];

    ++stats.count;
    stats.duration +=
        std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::high_resolution_clock::now() - m_begin);
}
/*----------------------------------------------------------------------------*/
}} // namespace HEVC::Decoder
