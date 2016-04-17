#ifndef HEVC_Decoder_Cmd_h
#define HEVC_Decoder_Cmd_h

#include <condition_variable>
#include <queue>
#include <mutex>

namespace HEVC { namespace Decoder {
/*----------------------------------------------------------------------------*/

enum class CmdId
{
    Undefined,
    ParseSNU,
    DecodeSNU,
    /* WARNING: Quit command is not acknowledged by decoder */
    Quit
};

struct Cmd
{
    const CmdId id;
    const int64_t seqNo;

    Cmd(CmdId id_, int64_t seqNo_):
        id{id_},
        seqNo{seqNo_}
    {}

    friend
    bool operator== (const Cmd &x, const Cmd &y)
    {
        return x.id == y.id && x.seqNo == y.seqNo;
    }

    friend
    bool operator!= (const Cmd &x, const Cmd &y)
    {
        return !(x == y);
    }
};

struct CmdAck
{
    const Cmd cmd;

    CmdAck(Cmd cmd_):
        cmd{cmd_}
    {}
};

class CmdQueue
{
    std::queue<Cmd> m_cmdQueue;
    std::queue<CmdAck> m_ackQueue;
    mutable std::mutex m_cmdMutex;
    std::mutex m_ackMutex;
    std::condition_variable m_cmdCond;
    std::condition_variable m_ackCond;
    int64_t seqNo;
public:
    CmdQueue():
        seqNo{0}
    {}

    bool emptyCmd() const
    {
        std::lock_guard<std::mutex> lock(m_cmdMutex);
        return m_cmdQueue.empty();
    }

    Cmd pushCmd(CmdId id)
    {
        std::lock_guard<std::mutex> lock(m_cmdMutex);
        m_cmdQueue.emplace(id, seqNo);
        m_cmdCond.notify_one();
        return Cmd{id, seqNo++};
    }

    void pushAck(CmdAck ack)
    {
        std::lock_guard<std::mutex> lock(m_ackMutex);
        m_ackQueue.push(ack);
        m_ackCond.notify_one();
    }

    Cmd popCmd()
    {
        std::unique_lock<std::mutex> lock(m_cmdMutex);

        while(m_cmdQueue.empty())
        {
            m_cmdCond.wait(lock);
        }

        auto cmd = m_cmdQueue.front();
        m_cmdQueue.pop();
        return cmd;
    }

    CmdAck popAck()
    {
        std::unique_lock<std::mutex> lock(m_ackMutex);

        while(m_ackQueue.empty())
        {
            m_ackCond.wait(lock);
        }

        auto ack = m_ackQueue.front();
        m_ackQueue.pop();
        return ack;
    }
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Decoder */

#endif /* HEVC_Decoder_Cmd_h */
