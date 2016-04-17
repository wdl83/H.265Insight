/* STDC++ */
#include <iterator>
#include <sstream>
#include <iomanip>
/* HEVC */
#include <Decoder/Extractor.h>
#include <Decoder/State.h>

namespace HEVC { namespace Decoder {
/*----------------------------------------------------------------------------*/
typedef Syntax::StreamNalUnit SNU;
typedef Syntax::NalUnit NU;
typedef Syntax::NalUnitHeader NUH;

VLA<uint8_t> Extractor::parse(std::istream &input)
{
    /* start code prefix 24/32 bits (reverse order) */
    static const uint8_t scp24[] = {1, 0, 0};
    static const uint8_t scp32[] = {1, 0, 0, 0};

    std::istream_iterator<uint8_t> begin(input), end;
    VLA<uint8_t> output;

    output.reserve(4 * 1024);

    /* search for byte stream NAL Unit start code prefix */
    while(
            !isMatchingAt(output.rbegin(), output.rend(), scp24)
            && !isMatchingAt(output.rbegin(), output.rend(), scp32))
    {
        /* expected syntax element: leading_zero_8bits */
        output.pushBack(*begin);

        if(0 != output.back() && 1 != output.back())
        {
            /* undo unexpected only */
            input.unget();
            runtime_assert(false);
        }
        ++begin;
    }

    /* until next NAL Unit start code prefix is encountered (or end of file),
     * treat all bytes as NAL Unit payload */

    while(begin != end)
    {
        /* NAL Unit payload including syntax element: trailing_zero_8bits */
        output.pushBack(*begin);

        const auto scp24Matched =
            isMatchingAt(output.rbegin(), output.rend(), scp24);
        /* do not search for scp32 if scp24 present */
        const auto scp32Matched =
            scp24Matched
            ? false
            : isMatchingAt(output.rbegin(), output.rend(), scp32);

        if(scp24Matched || scp32Matched)
        {
            const auto scpSize = scp24Matched ? sizeof(scp24) : sizeof(scp32);

            /* revert consumed start code prefix */
            const std::streampos absPos = input.tellg();
            const std::streamoff offset = absPos - static_cast<std::streampos>(scpSize);

            runtime_assert(absPos > static_cast<std::streampos>(sizeof(scpSize)));
            input.seekg(offset, std::ios_base::beg);
            /* truncate next NAL Unit scp from current tail */
            output.resize(output.size() - scpSize);
            break;
        }
        ++begin;
    }

    return output;
}
/*----------------------------------------------------------------------------*/
void Extractor::exec(std::istream &bitstream)
{
    Decoder::State decoder{PictureOutput::Disabled};
    CmdQueue cmdQueue;
    SNUQueue snuQueue;
    SNUQueue cvs;
    size_t cvsSizeInBytes = 0;

    while(!bitstream.eof())
    {
        const auto p = bitstream.tellg();
        auto snuPayload = parse(bitstream);

        /* parse only */
        {
            /* use "const L-value &" as payload (no ownership transfer) */
            snuQueue.push(makeHandle<SNU>(snuPayload));

            decoder.exec(CmdId::ParseSNU, snuQueue);
        }

        const NalUnitType nut =
            *toNalUnit(*decoder.snu).getSubtree<NUH>()->get<NUH::NalUnitType>();

        /* release all resources consumed by decoder while parsing current SNU */
        destruct(decoder.snu);

        if(isIDR(nut) || isBLA(nut) || isCRA(nut))
        {
            std::unique_lock<std::mutex> lock(m_mutex);

            m_queue.push({cvsSizeInBytes, std::move(cvs)});
            m_queueSizeInBytes += cvsSizeInBytes;
            cvsSizeInBytes = 0;
            m_popCond.notify_one();

            if(State::StopReq == m_state)
            {
                bitstream.seekg(p);
                break;
            }

            while(m_queueSizeInBytes > m_queueMaxSizeInBytes)
            {
                m_pushCond.wait(lock);
            }
        }

        cvsSizeInBytes += snuPayload.size();
        /* use "R-value &" as payload (ownership transfer) */
        cvs.push(makeHandle<SNU>(std::move(snuPayload)));
    }

    {
        std::unique_lock<std::mutex> lock(m_mutex);

        m_state = State::Stopped;
        m_popCond.notify_one();
    }
}
/*----------------------------------------------------------------------------*/
SNUQueue Extractor::extractCVS()
{
    std::unique_lock<std::mutex> lock(m_mutex);

    while(m_queue.empty() && State::Stopped != m_state)
    {
        m_popCond.wait(lock);
    }

    if(!m_queue.empty())
    {
        auto payload = std::move(m_queue.front());

        m_queue.pop();
        m_queueSizeInBytes -= payload.sizeInBytes;
        m_pushCond.notify_one();
        return std::move(payload.queue);
    }
    else
    {
        return SNUQueue{};
    }
}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Decoder */
