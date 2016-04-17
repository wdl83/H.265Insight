#ifndef HEVC_Syntax_NalUnitHeader_h
#define HEVC_Syntax_NalUnitHeader_h

#include <Syntax/Syntax.h>

namespace HEVC { namespace Syntax { namespace NalUnitHeaderContent {
/*----------------------------------------------------------------------------*/
class ForbiddenZeroBit:
    public Embedded,
    public VLD::FixedPattern<0>
{
public:
    static const auto Id = ElementId::forbidden_zero_bit;
};
/*----------------------------------------------------------------------------*/
class NalUnitType:
    public Embedded,
    public VLD::FixedUInt<6, uint8_t>
{
public:
    static const auto Id = ElementId::nal_unit_type;

    operator HEVC::NalUnitType () const
    {
        return static_cast<HEVC::NalUnitType>(getValue());
    }

    void log() const
    {
        HEVC::log(LogId::NalUnits, getName(HEVC::NalUnitType(*this)), '\n');
    }
};
/*----------------------------------------------------------------------------*/
class NuhLayerId:
    public Embedded,
    public VLD::FixedUInt<6, uint8_t>
{
public:
    static const auto Id = ElementId::nuh_layer_id;
};
/*----------------------------------------------------------------------------*/
class NuhTemporalIdPlus1:
    public Embedded,
    public VLD::FixedUInt<3, uint8_t>
{
public:
    static const auto Id = ElementId::nuh_temporal_id_plus1;
};
/*----------------------------------------------------------------------------*/
class TemporalId:
    public Embedded
{
public:
    typedef NuhTemporalIdPlus1::ValueType ValueType;
private:
    ValueType m_temporalId;
public:
    static const auto Id = ElementId::TemporalId;

    TemporalId(const NuhTemporalIdPlus1 &nuhTemporalIdPlus1):
        m_temporalId(nuhTemporalIdPlus1.getValue() - ValueType{1})
    {}

    operator ValueType () const
    {
        return m_temporalId;
    }
};
/*----------------------------------------------------------------------------*/
} /* namespace NalUnitHeaderContent */

/*----------------------------------------------------------------------------*/
class NalUnitHeader:
    public EmbeddedAggregator<
        NalUnitHeaderContent::ForbiddenZeroBit,
        NalUnitHeaderContent::NalUnitType,
        NalUnitHeaderContent::NuhTemporalIdPlus1,
        NalUnitHeaderContent::NuhLayerId,
        NalUnitHeaderContent::TemporalId>
{
public:
    static const auto Id = ElementId::nal_unit_header;

    typedef NalUnitHeaderContent::ForbiddenZeroBit ForbiddenZeroBit;
    typedef NalUnitHeaderContent::NalUnitType NalUnitType;
    typedef NalUnitHeaderContent::NuhTemporalIdPlus1 NuhTemporalIdPlus1;
    typedef NalUnitHeaderContent::NuhLayerId NuhLayerId;
    typedef NalUnitHeaderContent::TemporalId TemporalId;

    void onParse(StreamAccessLayer &, Decoder::State &);
};
/*----------------------------------------------------------------------------*/
}}

#endif /* HEVC_Syntax_NalUnitHeader_h */
