#include <Structure/DPB.h>
#include <Structure/AccessUnit.h>
#include <Structure/Picture.h>
#include <Syntax/SequenceParameterSet.h>
/* STDC++ */
#include <algorithm>
#include <iterator>

namespace HEVC { namespace Structure {
/*----------------------------------------------------------------------------*/
namespace {
/*----------------------------------------------------------------------------*/
bool isTemporalId0(const AccessUnit &au)
{
    using namespace Syntax;

    typedef SliceSegmentHeader SSH;
    typedef NalUnitHeader NUH;

    const auto nalUnitType = au.picture()->nalUnitType;
    const auto temporalId = au.temporalId;

    return
        0 == temporalId
        && !isRASL(nalUnitType)
        && !isRADL(nalUnitType)
        && !isSubLayerNonReference(nalUnitType);
}
/*----------------------------------------------------------------------------*/
} /* namespace */
/*----------------------------------------------------------------------------*/
void DPB::setup(const Syntax::SequenceParameterSet &sps)
{
    using namespace Syntax;

    typedef SequenceParameterSet SPS;

    const int highestTid = *sps.get<SPS::SpsMaxSubLayersMinus1>();

    m_maxNumReorderPics = (*sps.get<SPS::SpsMaxNumReorderPics>())[highestTid];
    m_maxLatencyIncreasePlus1 = (*sps.get<SPS::SpsMaxLatencyIncreasePlus1>())[highestTid];
    m_maxDecPicBufferingMinus1 = (*sps.get<SPS::SpsMaxDecPicBufferingMinus1>())[highestTid];

    log(LogId::DecodedPictureBuffer, "DBP setup\n");
}
/*----------------------------------------------------------------------------*/
void DPB::emplaceBack(Handle<AccessUnit> &&au)
{
    runtime_assert(au);

    if(isTemporalId0(*au))
    {
        m_prevTemporalId0 = au->picture()->order;
    }

    /* "For each picture in the DPB that is marked as "needed for output",
     * the associated variable PicLatencyCount is set equal to
     * PicLatencyCount + 1." */
    std::for_each(
            begin(), end(),
            [](Entry &entry)
            {
                if(entry.neededForOutput)
                {
                    ++entry.picLatencyCount;
                }
            });

    m_list.emplace_back(std::move(au), au->picture()->picOutputFlag);
    m_list.back().accessUnit->onDPB();

    /* "When one or more of the following conditions are true,
     * the "bumping" process specified in clause C.5.2.4 is invoked
     * repeatedly until none of the following conditions are true:" */
    while(
            reorderOverflow()
            || latencyOverflow())
    {
        bump();
    }
}
/*----------------------------------------------------------------------------*/
void DPB::erase(List::iterator i)
{
    log(
            LogId::DecodedPictureBuffer, "DPB erase ",
            std::distance(begin(), i),
            '\n',
            [this](std::ostream &oss){toStr(oss);});

    i->accessUnit->onDPB();
    m_list.erase(i);
}
/*----------------------------------------------------------------------------*/
void DPB::clear()
{
    log(LogId::DecodedPictureBuffer, "DPB clear\n");

    while(!m_list.empty())
    {
        erase(std::next(m_list.rbegin()).base());
    }
}
/*----------------------------------------------------------------------------*/
void DPB::flush()
{
    log(LogId::DecodedPictureBuffer, "DPB flush\n");

    const auto outputCount =
        [this]()
        {
            return
                std::count_if(
                        begin(), end(),
                        [](const Entry &e){return e.neededForOutput;});
        };

    eraseUnused();

    auto bumpGuard = 10 * capacity();

    while(outputCount())
    {
        runtime_assert(0 < bumpGuard);
        bump();
        --bumpGuard;
    }
}
/*----------------------------------------------------------------------------*/
bool DPB::reorderOverflow() const
{
    /* "The number of pictures in the DPB that are marked as "needed for output"
     * is greater than sps_max_num_reorder_pics[HighestTid]." */

    const auto n =
        std::count_if(
                begin(), end(),
                [](const Structure::DPB::Entry &entry)
                {
                    return entry.neededForOutput;
                });
    return n > m_maxNumReorderPics;
}
/*----------------------------------------------------------------------------*/
bool DPB::latencyOverflow() const
{
    /* "sps_max_latency_increase_plus1[HighestTid] is not equal to 0 and
     * there is at least one picture in the DPB that is marked as
     * "needed for output" for which the associated variable PicLatencyCount
     * is greater than or equal to SpsMaxLatencyPictures[HighestTid]." */

    return
        0 != m_maxLatencyIncreasePlus1
        &&
        end()
        !=
        std::find_if(
                begin(), end(),
                [this](const Entry &entry)
                {
                    return
                        entry.neededForOutput
                        && entry.picLatencyCount >= m_maxLatencyIncreasePlus1;
                });
}
/*----------------------------------------------------------------------------*/
bool DPB::bufferOverflow() const
{
    /* "The number of pictures in the DPB is greater than or equal to
     * sps_max_dec_pic_buffering_minus1[ HighestTid ] + 1." */

    return size() >= (m_maxDecPicBufferingMinus1 + 1);
}
/*----------------------------------------------------------------------------*/
void DPB::mark(PicOrderCntVal cnt, RefPicType type)
{
    auto p = picture(cnt);

    //runtime_assert(!isUnused(p->reference.get<RefPicType>()));
    p->reference.get<RefPicType>() = type;
}
/*----------------------------------------------------------------------------*/
void DPB::markAll(RefPicType type)
{
    for(auto &entry : *this)
    {
        entry.accessUnit->picture()->reference.get<RefPicType>() = type;
    }
}
/*----------------------------------------------------------------------------*/
Ptr<Picture> DPB::picture(PicOrderCntVal cnt) const
{
    const auto i =
        std::find_if(
                rbegin(), rend(),
                [cnt](const Entry &entry)
                {
                    return
                       entry.accessUnit
                        && entry.accessUnit->picture()->order.get<PicOrderCntVal>() == cnt;
                });

    return rend() != i ? i->accessUnit->picture() : nullptr;
}
/*----------------------------------------------------------------------------*/
Ptr<Picture> DPB::picture(PicOrderCntLsb cnt) const
{
    const auto i =
        std::find_if(
                rbegin(), rend(),
                [cnt](const Entry &entry)
                {
                    return
                        entry.accessUnit
                        && entry.accessUnit->picture()->order.get<PicOrderCntLsb>() == cnt;
                });

    return rend() != i ? i->accessUnit->picture() : nullptr;
}
/*----------------------------------------------------------------------------*/
bool DPB::isLongTerm(PicOrderCntVal cnt) const
{
    return HEVC::isLongTerm(picture(cnt)->reference.get<RefPicType>());
}
/*----------------------------------------------------------------------------*/
bool DPB::isShortTerm(PicOrderCntVal cnt) const
{
    return HEVC::isShortTerm(picture(cnt)->reference.get<RefPicType>());
}
/*----------------------------------------------------------------------------*/
void DPB::eraseUnused()
{
    log(LogId::DecodedPictureBuffer, "DBP erase unused\n");

    auto i = begin();

    while(end() != i)
    {
        if(
                !i->neededForOutput
                && isUnused(i->accessUnit->picture()->reference.get<RefPicType>()))
        {
            erase(i);
            i = begin();
        }
        else
        {
            ++i;
        }
    }
}
/*----------------------------------------------------------------------------*/
void DPB::bump()
{
    log(LogId::DecodedPictureBuffer, "DBP bump\n");

    auto i =
        std::find_if(
                begin(), end(),
                [](const Entry &x)
                {
                    return x.neededForOutput;
                });

    for(auto j = i; end() != j; ++j)
    {
        if(
                j->neededForOutput
                && (
                    j->accessUnit->picture()->order.get<PicOrderCntVal>()
                    < i->accessUnit->picture()->order.get<PicOrderCntVal>()))
        {
            i = j;
        }
    }

    if(end() != i)
    {
        /* TODO:
         * 1. cropp picture using conformance cropping window (active SPS)
         * 2. output cropped picture */
        i->neededForOutput = false;
        i->accessUnit->picture()->outputNo = m_outputCntr;
        i->accessUnit->storePicture(pictureOutput, "display_order_");
        ++m_outputCntr;

        if(isUnused(i->accessUnit->picture()->reference.get<RefPicType>()))
        {
            erase(i);
        }
    }
}
/*----------------------------------------------------------------------------*/
void DPB::toStr(std::ostream &os) const
{
    os
        << "capacity " << capacity()
        << " size " << size()
        << " buf_overflow " << bufferOverflow()
        << " latency_overflow " << latencyOverflow()
        << " reorder_overflow " << reorderOverflow() << '\n';

    for(const auto &entry : *this)
    {
        entry.accessUnit->toStr(os);
        os
            << " neededForOutput " << entry.neededForOutput
            << " picLatencyCount " << entry.picLatencyCount << '\n';
    }
}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Structure */
