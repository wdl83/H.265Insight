#ifndef HEVC_Syntax_ProfileTierLevel_h
#define HEVC_Syntax_ProfileTierLevel_h

/* HEVC */
#include <Syntax/Syntax.h>
#include <BitArray.h>
/* STDC++ */
#include <array>

namespace HEVC { namespace Syntax { namespace ProfileTierLevelContent {
/*----------------------------------------------------------------------------*/
struct GeneralProfileSpace:
    public Embedded,
    public VLD::FixedUInt<2, uint8_t>
{
    static const auto Id = ElementId::general_profile_space;
};

struct GeneralTierFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::general_tier_flag;
};

struct GeneralProfileIdc:
    public Embedded,
	public VLD::FixedUInt<5, uint8_t>
{
    static const auto Id = ElementId::general_profile_idc;

    GeneralProfileIdc()
    {
        /* TODO: check */
        /* 10/2014, 7.4.4 "Profile, tier, and level semantics" */
        setValue(0);
    }

    int inUnits() const
    {
        return getValue();
    }
};

struct GeneralProfileCompatibilityFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto num = 32;
    static const auto Id = ElementId::general_profile_compatibility_flag;
private:
    BitArray<num> m_list;
public:
    GeneralProfileCompatibilityFlag():
        /* TODO: check */
        /* 10/2014, 7.4.4 "Profile, tier, and level semantics" */
        m_list{false}
    {}

    bool operator[] (int i) const
    {
        return m_list[i];
    }

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list[i] = getValue();
    }
};

struct GeneralProgressiveSourceFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::general_progressive_source_flag;
};

struct GeneralInterlacedSourceFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::general_interlaced_source_flag;
};

struct GeneralNonPackedConstraintFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::general_non_packed_constraint_flag;
};

struct GeneralFrameOnlyConstraintFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::general_frame_only_constraint_flag;
};

struct GeneralMax12BitConstraintFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::general_max_12bit_constraint_flag;
};

struct GeneralMax10BitConstraintFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::general_max_10bit_constraint_flag;
};

struct GeneralMax8BitConstraintFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::general_max_8bit_constraint_flag;
};

struct GeneralMax422ChromaConstraintFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::general_max_422chroma_constraint_flag;
};

struct GeneralMax420ChromaConstraintFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::general_max_420chroma_constraint_flag;
};

struct GeneralMaxMonochromeConstraintFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::general_max_monochrome_constraint_flag;
};

struct GeneralIntraConstraintFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::general_intra_constraint_flag;
};

struct GeneralOnePictureOnlyConstraintFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::general_one_picture_only_constraint_flag;
};

struct GeneralLowerBitRateConstraintFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::general_lower_bit_rate_constraint_flag;
};

struct GeneralReservedZero34Bits:
    public Embedded,
    public VLD::FixedUInt<34, uint64_t>
{
    static const auto Id = ElementId::general_reserved_zero_34bits;
};

struct GeneralReservedZero43Bits:
    public Embedded,
    public VLD::FixedUInt<43, uint64_t>
{
    static const auto Id = ElementId::general_reserved_zero_43bits;
};

struct GeneralInbldFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::general_inbld_flag;
};

struct GeneralReservedZeroBit:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::general_reserved_zero_bit;
};

struct GeneralLevelIdc:
    public Embedded,
    public VLD::FixedUInt<8, uint8_t>
{
    static const auto Id = ElementId::general_level_idc;
};

class SubLayerFlagList
{
protected:
    BitArray<Limits::VpsMaxSubLayers::num> m_list;
public:
    bool operator[] (int i) const
    {
        return m_list[i];
    }
};

template <typename T>
class SubLayerList
{
protected:
    std::array<T, Limits::VpsMaxSubLayers::num> m_list;
public:
    T operator[] (int i) const
    {
        return m_list[i];
    }
};

struct SubLayerProfilePresentFlag:
    public SubLayerFlagList,
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::sub_layer_profile_present_flag;

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list[i] = getValue();
    }
};

struct SubLayerLevelPresentFlag:
    public SubLayerFlagList,
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::sub_layer_level_present_flag;

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list[i] = getValue();
    }
};

struct ReservedZero2Bits:
    public Embedded,
    public VLD::FixedUInt<2, uint8_t>
{
    static const auto Id = ElementId::reserved_zero_2bits;
};

struct SubLayerProfileSpace:
    public SubLayerList<int8_t>,
    public Embedded,
    public VLD::FixedUInt<2, uint8_t>
{
    static const auto Id = ElementId::sub_layer_profile_space;

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list[i] = getValue();
    }
};

struct SubLayerTierFlag:
    public SubLayerFlagList,
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::sub_layer_tier_flag;

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list[i] = getValue();
    }
};

struct SubLayerProfileIdc:
    public SubLayerList<int8_t>,
    public Embedded,
    public VLD::FixedUInt<5, uint8_t>
{
    static const auto Id = ElementId::sub_layer_profile_idc;

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list[i] = getValue();
    }
};

struct SubLayerProfileCompatibilityFlag:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::sub_layer_profile_compatibility_flag;
private:
    BitArray<Limits::VpsMaxSubLayers::num * GeneralProfileCompatibilityFlag::num>
        m_list;
public:
    bool operator[] (std::pair<int, int> i) const
    {
        return m_list[std::get<0>(i) * GeneralProfileCompatibilityFlag::num + std::get<1>(i)];
    }

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i, int j)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list[i * GeneralProfileCompatibilityFlag::num + j] = getValue();
    }
};

struct SubLayerProgressiveSourceFlag:
    public SubLayerFlagList,
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::sub_layer_progressive_source_flag;

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list[i] = getValue();
    }
};

struct SubLayerInterlacedSourceFlag:
    public SubLayerFlagList,
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::sub_layer_interlaced_source_flag;

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list[i] = getValue();
    }
};

struct SubLayerNonPackedConstraintFlag:
    public SubLayerFlagList,
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::sub_layer_non_packed_constraint_flag;

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list[i] = getValue();
    }
};

struct SubLayerFrameOnlyConstraintFlag:
    public SubLayerFlagList,
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::sub_layer_frame_only_constraint_flag;

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list[i] = getValue();
    }
};

struct SubLayerMax12BitConstraintFlag:
    public SubLayerFlagList,
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::sub_layer_max_12bit_constraint_flag;

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list[i] = getValue();
    }
};

struct SubLayerMax10BitConstraintFlag:
    public SubLayerFlagList,
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::sub_layer_max_10bit_constraint_flag;

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list[i] = getValue();
    }
};

struct SubLayerMax8BitConstraintFlag:
    public SubLayerFlagList,
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::sub_layer_max_8bit_constraint_flag;

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list[i] = getValue();
    }
};

struct SubLayerMax422ChromaConstraintFlag:
    public SubLayerFlagList,
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::sub_layer_max_422chroma_constraint_flag;

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list[i] = getValue();
    }
};

struct SubLayerMax420ChromaConstraintFlag:
    public SubLayerFlagList,
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::sub_layer_max_420chroma_constraint_flag;

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list[i] = getValue();
    }
};

struct SubLayerMaxMonochromeConstraintFlag:
    public SubLayerFlagList,
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::sub_layer_max_monochrome_constraint_flag;

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list[i] = getValue();
    }
};

struct SubLayerIntraConstraintFlag:
    public SubLayerFlagList,
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::sub_layer_intra_constraint_flag;

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list[i] = getValue();
    }
};

struct SubLayerOnePictureOnlyConstraintFlag:
    public SubLayerFlagList,
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::sub_layer_one_picture_only_constraint_flag;

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list[i] = getValue();
    }
};

struct SubLayerLowerBitRateConstraintFlag:
    public SubLayerFlagList,
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::sub_layer_lower_bit_rate_constraint_flag;

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list[i] = getValue();
    }
};

struct SubLayerReservedZero34Bits:
    public Embedded,
    public VLD::FixedUInt<34, uint64_t>
{
    static const auto Id = ElementId::sub_layer_reserved_zero_34bits;
};

struct SubLayerReservedZero43Bits:
    public Embedded,
    public VLD::FixedUInt<43, uint64_t>
{
    static const auto Id = ElementId::sub_layer_reserved_zero_43bits;
};

struct SubLayerInbldFlag:
    public SubLayerFlagList,
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::sub_layer_inbld_flag;

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list[i] = getValue();
    }
};

struct SubLayerReservedZeroBit:
    public Embedded,
    public VLD::FixedUInt<1, uint8_t>
{
    static const auto Id = ElementId::sub_layer_reserved_zero_bit;
};

struct SubLayerLevelIdc:
    public SubLayerList<int>,
    public Embedded,
    public VLD::FixedUInt<8, uint8_t>
{
    static const auto Id = ElementId::sub_layer_level_idc;

    void onParse(StreamAccessLayer &streamAccessLayer, Decoder::State &decoder, int i)
    {
        getFrom(streamAccessLayer, decoder, *this);
        m_list[i] = getValue();
    }
};
/*----------------------------------------------------------------------------*/
} /* namespace ProfileTierLevelContent */

/*----------------------------------------------------------------------------*/
class ProfileTierLevel:
    public EmbeddedAggregator<
        ProfileTierLevelContent::GeneralProfileSpace,
        ProfileTierLevelContent::GeneralTierFlag,
        ProfileTierLevelContent::GeneralProfileIdc,
        ProfileTierLevelContent::GeneralProfileCompatibilityFlag,
        ProfileTierLevelContent::GeneralProgressiveSourceFlag,
        ProfileTierLevelContent::GeneralInterlacedSourceFlag,
        ProfileTierLevelContent::GeneralNonPackedConstraintFlag,
        ProfileTierLevelContent::GeneralFrameOnlyConstraintFlag,
        ProfileTierLevelContent::GeneralMax12BitConstraintFlag,
        ProfileTierLevelContent::GeneralMax10BitConstraintFlag,
        ProfileTierLevelContent::GeneralMax8BitConstraintFlag,
        ProfileTierLevelContent::GeneralMax422ChromaConstraintFlag,
        ProfileTierLevelContent::GeneralMax420ChromaConstraintFlag,
        ProfileTierLevelContent::GeneralMaxMonochromeConstraintFlag,
        ProfileTierLevelContent::GeneralIntraConstraintFlag,
        ProfileTierLevelContent::GeneralOnePictureOnlyConstraintFlag,
        ProfileTierLevelContent::GeneralLowerBitRateConstraintFlag,
        ProfileTierLevelContent::GeneralReservedZero34Bits,
        ProfileTierLevelContent::GeneralReservedZero43Bits,
        ProfileTierLevelContent::GeneralInbldFlag,
        ProfileTierLevelContent::GeneralReservedZeroBit,
        ProfileTierLevelContent::GeneralLevelIdc,
        ProfileTierLevelContent::SubLayerProfilePresentFlag,
        ProfileTierLevelContent::SubLayerLevelPresentFlag,
        ProfileTierLevelContent::ReservedZero2Bits,
        ProfileTierLevelContent::SubLayerProfileSpace,
        ProfileTierLevelContent::SubLayerTierFlag,
        ProfileTierLevelContent::SubLayerProfileIdc,
        ProfileTierLevelContent::SubLayerProfileCompatibilityFlag,
        ProfileTierLevelContent::SubLayerProgressiveSourceFlag,
        ProfileTierLevelContent::SubLayerInterlacedSourceFlag,
        ProfileTierLevelContent::SubLayerNonPackedConstraintFlag,
        ProfileTierLevelContent::SubLayerFrameOnlyConstraintFlag,
        ProfileTierLevelContent::SubLayerMax12BitConstraintFlag,
        ProfileTierLevelContent::SubLayerMax10BitConstraintFlag,
        ProfileTierLevelContent::SubLayerMax8BitConstraintFlag,
        ProfileTierLevelContent::SubLayerMax422ChromaConstraintFlag,
        ProfileTierLevelContent::SubLayerMax420ChromaConstraintFlag,
        ProfileTierLevelContent::SubLayerMaxMonochromeConstraintFlag,
        ProfileTierLevelContent::SubLayerIntraConstraintFlag,
        ProfileTierLevelContent::SubLayerOnePictureOnlyConstraintFlag,
        ProfileTierLevelContent::SubLayerLowerBitRateConstraintFlag,
        ProfileTierLevelContent::SubLayerReservedZero34Bits,
        ProfileTierLevelContent::SubLayerReservedZero43Bits,
        ProfileTierLevelContent::SubLayerInbldFlag,
        ProfileTierLevelContent::SubLayerReservedZeroBit,
        ProfileTierLevelContent::SubLayerLevelIdc>
{
public:
    static const auto Id = ElementId::profile_tier_level;

    typedef ProfileTierLevelContent::GeneralProfileSpace GeneralProfileSpace;
    typedef ProfileTierLevelContent::GeneralTierFlag GeneralTierFlag;
    typedef ProfileTierLevelContent::GeneralProfileIdc GeneralProfileIdc;
    typedef ProfileTierLevelContent::GeneralProfileCompatibilityFlag GeneralProfileCompatibilityFlag;
    typedef ProfileTierLevelContent::GeneralProgressiveSourceFlag GeneralProgressiveSourceFlag;
    typedef ProfileTierLevelContent::GeneralInterlacedSourceFlag GeneralInterlacedSourceFlag;
    typedef ProfileTierLevelContent::GeneralNonPackedConstraintFlag GeneralNonPackedConstraintFlag;
    typedef ProfileTierLevelContent::GeneralFrameOnlyConstraintFlag GeneralFrameOnlyConstraintFlag;
    typedef ProfileTierLevelContent::GeneralMax12BitConstraintFlag GeneralMax12BitConstraintFlag;
    typedef ProfileTierLevelContent::GeneralMax10BitConstraintFlag GeneralMax10BitConstraintFlag;
    typedef ProfileTierLevelContent::GeneralMax8BitConstraintFlag GeneralMax8BitConstraintFlag;
    typedef ProfileTierLevelContent::GeneralMax422ChromaConstraintFlag GeneralMax422ChromaConstraintFlag;
    typedef ProfileTierLevelContent::GeneralMax420ChromaConstraintFlag GeneralMax420ChromaConstraintFlag;
    typedef ProfileTierLevelContent::GeneralMaxMonochromeConstraintFlag GeneralMaxMonochromeConstraintFlag;
    typedef ProfileTierLevelContent::GeneralIntraConstraintFlag GeneralIntraConstraintFlag;
    typedef ProfileTierLevelContent::GeneralOnePictureOnlyConstraintFlag GeneralOnePictureOnlyConstraintFlag;
    typedef ProfileTierLevelContent::GeneralLowerBitRateConstraintFlag GeneralLowerBitRateConstraintFlag;
    typedef ProfileTierLevelContent::GeneralReservedZero34Bits GeneralReservedZero34Bits;
    typedef ProfileTierLevelContent::GeneralReservedZero43Bits GeneralReservedZero43Bits;
    typedef ProfileTierLevelContent::GeneralInbldFlag GeneralInbldFlag;
    typedef ProfileTierLevelContent::GeneralReservedZeroBit GeneralReservedZeroBit;
    typedef ProfileTierLevelContent::GeneralLevelIdc GeneralLevelIdc;
    typedef ProfileTierLevelContent::SubLayerProfilePresentFlag SubLayerProfilePresentFlag;
    typedef ProfileTierLevelContent::SubLayerLevelPresentFlag SubLayerLevelPresentFlag;
    typedef ProfileTierLevelContent::ReservedZero2Bits ReservedZero2Bits;
    typedef ProfileTierLevelContent::SubLayerProfileSpace SubLayerProfileSpace;
    typedef ProfileTierLevelContent::SubLayerTierFlag SubLayerTierFlag;
    typedef ProfileTierLevelContent::SubLayerProfileIdc SubLayerProfileIdc;
    typedef ProfileTierLevelContent::SubLayerProfileCompatibilityFlag SubLayerProfileCompatibilityFlag;
    typedef ProfileTierLevelContent::SubLayerProgressiveSourceFlag SubLayerProgressiveSourceFlag;
    typedef ProfileTierLevelContent::SubLayerInterlacedSourceFlag SubLayerInterlacedSourceFlag;
    typedef ProfileTierLevelContent::SubLayerNonPackedConstraintFlag SubLayerNonPackedConstraintFlag;
    typedef ProfileTierLevelContent::SubLayerFrameOnlyConstraintFlag SubLayerFrameOnlyConstraintFlag;
    typedef ProfileTierLevelContent::SubLayerMax12BitConstraintFlag SubLayerMax12BitConstraintFlag;
    typedef ProfileTierLevelContent::SubLayerMax10BitConstraintFlag SubLayerMax10BitConstraintFlag;
    typedef ProfileTierLevelContent::SubLayerMax8BitConstraintFlag SubLayerMax8BitConstraintFlag;
    typedef ProfileTierLevelContent::SubLayerMax422ChromaConstraintFlag SubLayerMax422ChromaConstraintFlag;
    typedef ProfileTierLevelContent::SubLayerMax420ChromaConstraintFlag SubLayerMax420ChromaConstraintFlag;
    typedef ProfileTierLevelContent::SubLayerMaxMonochromeConstraintFlag SubLayerMaxMonochromeConstraintFlag;
    typedef ProfileTierLevelContent::SubLayerIntraConstraintFlag SubLayerIntraConstraintFlag;
    typedef ProfileTierLevelContent::SubLayerOnePictureOnlyConstraintFlag SubLayerOnePictureOnlyConstraintFlag;
    typedef ProfileTierLevelContent::SubLayerLowerBitRateConstraintFlag SubLayerLowerBitRateConstraintFlag;
    typedef ProfileTierLevelContent::SubLayerReservedZero34Bits SubLayerReservedZero34Bits;
    typedef ProfileTierLevelContent::SubLayerReservedZero43Bits SubLayerReservedZero43Bits;
    typedef ProfileTierLevelContent::SubLayerInbldFlag SubLayerInbldFlag;
    typedef ProfileTierLevelContent::SubLayerReservedZeroBit SubLayerReservedZeroBit;
    typedef ProfileTierLevelContent::SubLayerLevelIdc SubLayerLevelIdc;

    void onParse(StreamAccessLayer &, Decoder::State &decoder, bool, int);
};
/*----------------------------------------------------------------------------*/
}} // HEVC::Syntax

#endif /* HEVC_Syntax_ProfileAndLevel_h */
