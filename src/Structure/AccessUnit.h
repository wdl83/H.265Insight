#ifndef HEVC_Structure_AccessUnit_h
#define HEVC_Structure_AccessUnit_h

/* STDC++ */
#include <array>
#include <chrono>
#include <ostream>
/* HEVC */
#include <HEVC.h>
#include <Syntax/StreamNalUnit.h>
#include <Structure/Picture.h>
#include <Decoder/Fwd.h>
#include <Decoder/Processes/ProcessId.h>
#include <log.h>

namespace HEVC { namespace Structure {
/*----------------------------------------------------------------------------*/
class AccessUnit
{
    /* 04/2013, 7.4.2.2 " NAL unit header semantics"
     *
     * All coded slice segment NAL units of an access unit shall have
     * the same value of nal_unit_type. A picture or an access unit is also
     * referred to as having a nal_unit_type equal to the nal_unit_type
     * of the coded slice segment NAL units of the picture or access unit */
public:
    typedef VLA<Handle<Syntax::StreamNalUnit>> StreamNalList;

    struct Cntr
    {
        /* Stream NAL Unit */
        int snu;

        Cntr(): snu(0)
        {}
    };
private:
    Decoder::State &m_decoder;
    Handle<Picture> m_picture;
    bool m_inDPB;
public:
    int temporalId;
    StreamNalList list;
    Cntr cntr;

    struct ProcessingStats
    {
        std::chrono::microseconds duration;
        uint64_t count;

        ProcessingStats():
            duration{0},
            count{0}
        {}
    };

    struct Stats
    {
        /* total time required to decode current access unit */
        std::chrono::high_resolution_clock::time_point beginTime, endTime;

        std::array<
            ProcessingStats,
            EnumRange<Decoder::Processes::ProcessId>::length()> processing;

        const ProcessingStats &operator[] (Decoder::Processes::ProcessId id) const
        {
            return processing[int(id)];
        }

        ProcessingStats &operator[] (Decoder::Processes::ProcessId id)
        {
            return processing[int(id)];
        }

        std::chrono::microseconds duration() const
        {
            return
                std::chrono::duration_cast<std::chrono::microseconds>(
                        endTime - beginTime);
        }

        Stats():
            beginTime{std::chrono::high_resolution_clock::now()},
            endTime{beginTime}
        {}
    };

    Stats stats;

    AccessUnit(Decoder::State &decoder, int tId):
        m_decoder{decoder},
        m_inDPB{false},
        temporalId{tId}
    {}

    AccessUnit(const AccessUnit &) = delete;
    AccessUnit &operator= (const AccessUnit &) = delete;

    void add(Handle<Syntax::StreamNalUnit> &&handle)
    {
        list.pushBack(std::move(handle));
        ++cntr.snu;
    }

    void picture(
            NalUnitType nalUnitType,
            Ptr<const Syntax::VideoParameterSet> vps,
            Ptr<const Syntax::SequenceParameterSet> sps,
            Ptr<const Syntax::PictureParameterSet> pps,
            bool noRaslOutputFlag,
            int64_t decodingNo)
    {
        m_picture.construct(vps, sps, pps, nalUnitType, noRaslOutputFlag, decodingNo);
    }

    Ptr<Picture> picture() const
    {
        return !m_picture ? nullptr : Ptr<Picture>{m_picture};
    }

    void onDecodeFinish(PictureOutput);
    void storePicture(PictureOutput = PictureOutput::Disabled, const std::string & = {}) const;
    void onDPB();

    bool inDPB() const
    {
        return m_inDPB;
    }

    void toStr(std::ostream &) const;
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Structure */

#endif /* HEVC_Structure_AccessUnit_h */
