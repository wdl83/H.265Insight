#ifndef HEVC_Syntax_PpsRangeExtension_h
#define HEVC_Syntax_PpsRangeExtension_h

/* STDC++ */
#include <array>
/* HEVC */
#include <Syntax/Syntax.h>

namespace HEVC { namespace Syntax { namespace PpsRangeExtensionContent {
/*----------------------------------------------------------------------------*/
struct MaxTransformSkipBlockSizeMinus2:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::log2_max_transform_skip_block_size_minus2;

    MaxTransformSkipBlockSizeMinus2()
    {
        /* 10/2014, 7.4.3.3.2 "Picture parameter set range extension semantics" */
        setValue(0);
    }

    Log2 inUnits() const
    {
        return Log2(getValue());
    }
};

struct CrossComponentPredictionEnabledFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::cross_component_prediction_enabled_flag;

    CrossComponentPredictionEnabledFlag()
    {
        /* 10/2014, 7.4.3.3.2 "Picture parameter set range extension semantics" */
        setValue(0);
    }

    explicit operator bool () const
    {
        return getValue();
    }
};

struct ChromaQpOffsetListEnabledFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::chroma_qp_offset_list_enabled_flag;

    explicit operator bool () const
    {
        return getValue();
    }
};

struct DiffCuChromaQpOffsetDepth:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::diff_cu_chroma_qp_offset_depth;

    Log2 inUnits() const
    {
        return Log2(getValue());
    }
};

struct ChromaQpOffsetListLenMinus1:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::chroma_qp_offset_list_len_minus1;

    int inUnits() const
    {
        return getValue();
    }
};

class ChromaQpOffsetList
{
public:
    static const auto length = Limits::ChromaQpOffsetListLenght::value;
protected:
    std::array<int8_t, length> m_list;
public:
    ChromaQpOffsetList()
    {
        std::fill(std::begin(m_list), std::end(m_list), 0);
    }

    int8_t operator[] (int i) const
    {
        return m_list[i];
    }
};

struct CbQpOffsetList:
    public ChromaQpOffsetList,
    public Embedded,
    public VLD::IntExpGolombCoded
{
    static const auto Id = ElementId::cb_qp_offset_list;

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list[i] = getValue();
    }
};

struct CrQpOffsetList:
    public ChromaQpOffsetList,
    public Embedded,
    public VLD::IntExpGolombCoded
{
    static const auto Id = ElementId::cr_qp_offset_list;

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list[i] = getValue();
    }
};

struct SaoOffsetScaleLuma:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::log2_sao_offset_scale_luma;

    SaoOffsetScaleLuma()
    {
        /* 10/2014, 7.4.3.3.2 "Picture parameter set range extension semantics" */
        setValue(0);
    }

    Log2 inUnits() const
    {
        return Log2(getValue());
    }
};

struct SaoOffsetScaleChroma:
    public Embedded,
    public VLD::UIntExpGolombCoded
{
    static const auto Id = ElementId::log2_sao_offset_scale_chroma;

    SaoOffsetScaleChroma()
    {
        /* 10/2014, 7.4.3.3.2 "Picture parameter set range extension semantics" */
        setValue(0);
    }

    Log2 inUnits() const
    {
        return Log2(getValue());
    }
};
/*----------------------------------------------------------------------------*/
} /* PpsRangeExentionContent */

/*----------------------------------------------------------------------------*/
struct PpsRangeExtension:
    public EmbeddedAggregator<
        PpsRangeExtensionContent::MaxTransformSkipBlockSizeMinus2,
        PpsRangeExtensionContent::CrossComponentPredictionEnabledFlag,
        PpsRangeExtensionContent::ChromaQpOffsetListEnabledFlag,
        PpsRangeExtensionContent::DiffCuChromaQpOffsetDepth,
        PpsRangeExtensionContent::ChromaQpOffsetListLenMinus1,
        PpsRangeExtensionContent::CbQpOffsetList,
        PpsRangeExtensionContent::CrQpOffsetList,
        PpsRangeExtensionContent::SaoOffsetScaleLuma,
        PpsRangeExtensionContent::SaoOffsetScaleChroma>
{
    typedef PpsRangeExtensionContent::MaxTransformSkipBlockSizeMinus2 MaxTransformSkipBlockSizeMinus2;
    typedef PpsRangeExtensionContent::CrossComponentPredictionEnabledFlag CrossComponentPredictionEnabledFlag;
    typedef PpsRangeExtensionContent::ChromaQpOffsetListEnabledFlag ChromaQpOffsetListEnabledFlag;
    typedef PpsRangeExtensionContent::DiffCuChromaQpOffsetDepth DiffCuChromaQpOffsetDepth;
    typedef PpsRangeExtensionContent::ChromaQpOffsetListLenMinus1 ChromaQpOffsetListLenMinus1;
    typedef PpsRangeExtensionContent::CbQpOffsetList CbQpOffsetList;
    typedef PpsRangeExtensionContent::CrQpOffsetList CrQpOffsetList;
    typedef PpsRangeExtensionContent::SaoOffsetScaleLuma SaoOffsetScaleLuma;
    typedef PpsRangeExtensionContent::SaoOffsetScaleChroma SaoOffsetScaleChroma;

    static const auto Id = ElementId::pps_range_extension;

    void onParse(StreamAccessLayer &, Decoder::State &, const PictureParameterSet &);
};
/*----------------------------------------------------------------------------*/
}} // HEVC::Syntax

#endif /* HEVC_Syntax_PpsRangeExtension_h */
