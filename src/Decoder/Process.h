#ifndef HEVC_Decoder_Process_h
#define HEVC_Decoder_Process_h

/* STDC++ */
#include <chrono>
#include <utility>
/* HEVC */
#include <Decoder/Fwd.h>
#include <Decoder/Processes/ProcessId.h>
#include <Structure/Fwd.h>
#include <Optional.h>

namespace HEVC { namespace Decoder {
/*----------------------------------------------------------------------------*/
bool isMasked(const State &, Processes::ProcessId);

class ProcessingTimes
{
    State &m_decoder;
    Processes::ProcessId m_processId;
    std::chrono::high_resolution_clock::time_point m_begin;
public:
    ProcessingTimes(State &decoder,  Processes::ProcessId processId):
        m_decoder{decoder},
        m_processId{processId},
        m_begin{std::chrono::high_resolution_clock::now()}
    {}

    ~ProcessingTimes();
};

template <typename P, typename ...A_n>
void process(State &decoder, P &&p, A_n &&... a_n)
{
    if(!isMasked(decoder, P::id))
    {
        ProcessingTimes collector{decoder, P::id};

        p.exec(decoder, std::forward<A_n>(a_n)...);
    }
}

template <typename P, typename ...A_n>
auto subprocess(State &decoder, P &&p, A_n &&... a_n)
    -> decltype(p.exec(decoder, std::forward<A_n>(a_n)...))
{
    ProcessingTimes collector{decoder, P::id};

    return p.exec(decoder, std::forward<A_n>(a_n)...);
}
/*----------------------------------------------------------------------------*/
}} /* namespace HEVC::Decoder */

#endif /* HEVC_Decoder_Process_h */
