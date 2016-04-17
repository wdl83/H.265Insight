#ifndef HEVC_Syntax_NalUnit_h
#define HEVC_Syntax_NalUnit_h

#include <Syntax/Syntax.h>
#include <Syntax/NalUnitHeader.h>
#include <Syntax/VideoParameterSet.h>
#include <Syntax/SequenceParameterSet.h>
#include <Syntax/PictureParameterSet.h>
#include <Syntax/AccessUnitDelimiterRbsp.h>
#include <Syntax/EndOfSeqRbsp.h>
#include <Syntax/EndOfBitstreamRbsp.h>
#include <Syntax/FillerDataRbsp.h>
#include <Syntax/SliceSegmentLayerRbsp.h>
#include <Syntax/SeiRbsp.h>

namespace HEVC { namespace Syntax { namespace NalUnitContent {
/*----------------------------------------------------------------------------*/
class NumBytesInRbsp:
    public Embedded
{
public:
    static const auto Id = ElementId::NumBytesInRBSP;
private:
    const size_t m_numBytesInRbsp;
public:
    NumBytesInRbsp(size_t numBytesInRbsp):
        m_numBytesInRbsp(numBytesInRbsp)
    {}

    operator size_t () const
    {
        return m_numBytesInRbsp;
    }
};
/*----------------------------------------------------------------------------*/
class RbspByte:
    public Embedded,
    public VLD::Bits8
{
    VLA<uint8_t> m_list;
public:
    static const auto Id = ElementId::rbsp_byte;

    RbspByte(size_t sizeInBytes)
    {
        m_list.reserve(sizeInBytes);
    }

    operator const VLA<uint8_t> &() const
    {
        return m_list;
    }

    bool empty() const
    {
        return m_list.empty();
    }

    friend
    bool operator== (const RbspByte &x, const RbspByte &y)
    {
        return x.m_list == y.m_list;
    }

    friend
    bool operator!= (const RbspByte &x, const RbspByte &y)
    {
        return !(x == y);
    }

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list.emplaceBack(getValue());
    }
};
/*----------------------------------------------------------------------------*/
class EmulationPreventionThreeByte:
    public Embedded,
    public VLD::FixedPattern<0, 0, 0, 0, 0, 0, 1, 1 /* 0x03 */>
{
public:
    static const auto Id = ElementId::emulation_prevention_three_byte;
};
/*----------------------------------------------------------------------------*/
} /* namespace NalUnitContent */
/*----------------------------------------------------------------------------*/
class NalUnit:
    public EmbeddedAggregator<
        NalUnitContent::EmulationPreventionThreeByte,
        NalUnitContent::RbspByte,
        NalUnitContent::NumBytesInRbsp>,
    public SubtreeAggregator<
        NalUnitHeader,
        VideoParameterSet,
        SequenceParameterSet,
        PictureParameterSet,
        AccessUnitDelimiterRbsp,
        EndOfSeqRbsp,
        EndOfBitstreamRbsp,
        FillerDataRbsp,
        SliceSegmentLayerRbsp,
        SeiRbsp>
{
private:
    size_t m_numBytesInNalUnit;
public:
    static const auto Id = ElementId::nal_unit;

    typedef NalUnitContent::EmulationPreventionThreeByte EmulationPreventionThreeByte;
    typedef NalUnitContent::RbspByte RbspByte;
    typedef NalUnitContent::NumBytesInRbsp NumBytesInRbsp;

    NalUnit(size_t numBytesInNalUnit):
        m_numBytesInNalUnit(numBytesInNalUnit)
    {}

    bool empty() const
    {
        return get<RbspByte>()->empty();
    }

    void onParse(StreamAccessLayer &, Decoder::State &);
    void parseRbsp(Decoder::State &);
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */

#endif /* HEVC_Syntax_NalUnit_t */
