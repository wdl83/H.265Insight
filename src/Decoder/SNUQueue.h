#ifndef HEVC_Decoder_SNUQueue_h
#define HEVC_Decoder_SNUQueue_h

/* STDC++ */
#include <queue>
/* HEVC */
#include <HEVC.h>
#include <Syntax/Fwd.h>

namespace HEVC { namespace Decoder {
/*----------------------------------------------------------------------------*/
class SNUQueue
{
    typedef Syntax::StreamNalUnit SNU;
    typedef std::queue<Handle<SNU>> Queue;

    Queue m_queue;
public:
    SNUQueue()
    {}

    SNUQueue(const SNUQueue &) = delete;
    SNUQueue(SNUQueue &&) = default;
    SNUQueue &operator= (const SNUQueue &) = delete;
    SNUQueue &operator= (SNUQueue &&) = default;

    const Handle<SNU> &front() const
    {
        return m_queue.front();
    }

    const Handle<SNU> &back() const
    {
        return m_queue.back();
    }

    void push(Handle<SNU> &&snu)
    {
        m_queue.push(std::move(snu));
    }

    Handle<SNU> pop()
    {
        Handle<SNU> top = std::move(m_queue.front());
        m_queue.pop();
        return top;
    }

    bool empty() const
    {
        return m_queue.empty();
    }

    auto size() const -> decltype(m_queue.size())
    {
        return m_queue.size();
    }
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Decoder */

#endif /* HEVC_Decoder_SNUQueue_h */
