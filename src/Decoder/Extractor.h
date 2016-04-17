#ifndef HEVC_Decoder_Extractor_h
#define HEVC_Decoder_Extractor_h

/* STDC++ */
#include <condition_variable>
#include <queue>
#include <mutex>
#include <thread>
/* HEVC */
#include <Decoder/SNUQueue.h>

namespace HEVC { namespace Decoder {
/*----------------------------------------------------------------------------*/
class Extractor
{
    enum class State
    {
        Running, StopReq, Stopped
    };

    struct Payload
    {
        size_t sizeInBytes;
        SNUQueue queue;

        Payload(size_t s, SNUQueue &&q):
            sizeInBytes{s},
            queue{std::move(q)}
        {}
    };

    std::queue<Payload> m_queue;
    const size_t m_queueMaxSizeInBytes;
    size_t m_queueSizeInBytes;
    std::condition_variable m_pushCond;
    std::condition_variable m_popCond;
    State m_state;
    mutable std::mutex m_mutex;

    VLA<uint8_t> parse(std::istream &);
    void exec(std::istream &);
public:
    std::thread thread;

    Extractor(std::istream &bitstream, size_t queueMaxSizeInBytes):
        m_queueMaxSizeInBytes{queueMaxSizeInBytes},
        m_queueSizeInBytes{0},
        m_state{State::Running},
        thread
        {
            [this, &bitstream]()
            {

                bitstream >> std::noskipws;
                exec(bitstream);
            }
        }
    {}

    ~Extractor()
    {
        while(!extractCVS().empty())
        {}

        join();
    }

    void join()
    {
        if(thread.joinable())
        {
            thread.join();
        }
    }

    void stop()
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if(State::Running == m_state)
        {
            m_state = State::StopReq;
        }
    }

    State state() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_state;
    }

    SNUQueue extractCVS();
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Decoder */

#endif /* HEVC_Decoder_Extractor_h */
