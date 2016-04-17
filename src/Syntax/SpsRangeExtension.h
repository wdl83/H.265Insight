#ifndef HEVC_Syntax_SpsRangeExtension_h
#define HEVC_Syntax_SpsRangeExtension_h

/* HEVC */
#include <Syntax/Syntax.h>

namespace HEVC { namespace Syntax { namespace SpsRangeExtensionContent {
/*----------------------------------------------------------------------------*/
struct TransformSkipRotationEnabledFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::transform_skip_rotation_enabled_flag;

    TransformSkipRotationEnabledFlag()
    {
        /* 10/2014, 7.4.3.2.2 "Sequence parameter set range extension semantics" */
        setValue(0);
    }

    explicit operator bool() const
    {
        return getValue();
    }
};

struct TransformSkipContextEnabledFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::transform_skip_context_enabled_flag;

    TransformSkipContextEnabledFlag()
    {
        /* 10/2014, 7.4.3.2.2 "Sequence parameter set range extension semantics" */
        setValue(0);
    }

    explicit operator bool() const
    {
        return getValue();
    }
};

struct ImplicitRdpcmEnabledFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::implicit_rdpcm_enabled_flag;

    ImplicitRdpcmEnabledFlag()
    {
        /* 10/2014, 7.4.3.2.2 "Sequence parameter set range extension semantics" */
        setValue(0);
    }

    explicit operator bool () const
    {
        return getValue();
    }
};

struct ExplicitRdpcmEnabledFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::explicit_rdpcm_enabled_flag;

    ExplicitRdpcmEnabledFlag()
    {
        /* 10/2014, 7.4.3.2.2 "Sequence parameter set range extension semantics" */
        setValue(0);
    }

    explicit operator bool () const
    {
        return getValue();
    }
};

struct ExtendedPrecisionProcessingFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::extended_precision_processing_flag;

    ExtendedPrecisionProcessingFlag()
    {
        /* 10/2014, 7.4.3.2.2 "Sequence parameter set range extension semantics" */
        setValue(0);
    }

    explicit operator bool() const
    {
        return getValue();
    }
};

struct IntraSmoothingDisabledFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::intra_smoothing_disabled_flag;

    IntraSmoothingDisabledFlag()
    {
        /* 10/2014, 7.4.3.2.2 "Sequence parameter set range extension semantics" */
        setValue(0);
    }

    explicit operator bool() const
    {
        return getValue();
    }
};

struct HighPrecisionOffsetsEnabledFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::high_precision_offsets_enabled_flag;

    HighPrecisionOffsetsEnabledFlag()
    {
        /* 10/2014, 7.4.3.2.2 "Sequence parameter set range extension semantics" */
        setValue(0);
    }

    explicit operator bool() const
    {
        return getValue();
    }
};

struct PersistentRiceAdaptationEnabledFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::persistent_rice_adaptation_enabled_flag;

    PersistentRiceAdaptationEnabledFlag()
    {
        /* 10/2014, 7.4.3.2.2 "Sequence parameter set range extension semantics" */
        setValue(0);
    }

    explicit operator bool () const
    {
        return getValue();
    }
};

struct CabacBypassAlignmentEnabledFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::cabac_bypass_alignment_enabled_flag;

    CabacBypassAlignmentEnabledFlag()
    {
        /* 10/2014, 7.4.3.2.2 "Sequence parameter set range extension semantics" */
        setValue(0);
    }

    explicit operator bool () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
} /* SpsRangeExtensionContent */

struct SpsRangeExtension:
    public EmbeddedAggregator<
        SpsRangeExtensionContent::TransformSkipRotationEnabledFlag,
        SpsRangeExtensionContent::TransformSkipContextEnabledFlag,
        SpsRangeExtensionContent::ImplicitRdpcmEnabledFlag,
        SpsRangeExtensionContent::ExplicitRdpcmEnabledFlag,
        SpsRangeExtensionContent::ExtendedPrecisionProcessingFlag,
        SpsRangeExtensionContent::IntraSmoothingDisabledFlag,
        SpsRangeExtensionContent::HighPrecisionOffsetsEnabledFlag,
        SpsRangeExtensionContent::PersistentRiceAdaptationEnabledFlag,
        SpsRangeExtensionContent::CabacBypassAlignmentEnabledFlag>
{
    typedef SpsRangeExtensionContent::TransformSkipRotationEnabledFlag TransformSkipRotationEnabledFlag;
    typedef SpsRangeExtensionContent::TransformSkipContextEnabledFlag TransformSkipContextEnabledFlag;
    typedef SpsRangeExtensionContent::ImplicitRdpcmEnabledFlag ImplicitRdpcmEnabledFlag;
    typedef SpsRangeExtensionContent::ExplicitRdpcmEnabledFlag ExplicitRdpcmEnabledFlag;
    typedef SpsRangeExtensionContent::ExtendedPrecisionProcessingFlag ExtendedPrecisionProcessingFlag;
    typedef SpsRangeExtensionContent::IntraSmoothingDisabledFlag IntraSmoothingDisabledFlag;
    typedef SpsRangeExtensionContent::HighPrecisionOffsetsEnabledFlag HighPrecisionOffsetsEnabledFlag;
    typedef SpsRangeExtensionContent::PersistentRiceAdaptationEnabledFlag PersistentRiceAdaptationEnabledFlag;
    typedef SpsRangeExtensionContent::CabacBypassAlignmentEnabledFlag CabacBypassAlignmentEnabledFlag;

    static const auto Id = ElementId::sps_range_extension;

    void onParse(StreamAccessLayer &, Decoder::State &);
};
/*----------------------------------------------------------------------------*/
}} // HEVC::Syntax

#endif /* HEVC_Syntax_SpsRangeExtension_h */
