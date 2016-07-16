#ifndef HEVC_Syntax_ResidualCoding_h
#define HEVC_Syntax_ResidualCoding_h
/* HEVC */
#include <Syntax/Syntax.h>
#include <BitMatrix.h>

namespace HEVC { namespace Syntax { namespace ResidualCodingContent {
/*----------------------------------------------------------------------------*/
typedef EmbeddedCoord<Pel, ElementId::ResidualCodingCoord> Coord;
typedef EmbeddedUnit<Log2, ElementId::ResidualCodingSize> Size;
/*----------------------------------------------------------------------------*/
class CIdx: public Embedded
{
private:
    Plane m_cIdx;
public:
    static const auto Id = ElementId::ResidualCodingCIdx;

    CIdx(Plane cIdx): m_cIdx(cIdx)
    {}

    operator Plane () const
    {
        return m_cIdx;
    }

    operator Component () const
    {
        return Plane::Y == *this ? Component::Luma : Component::Chroma;
    }
};
/*----------------------------------------------------------------------------*/
class ScanIdx: public Embedded
{
private:
    HEVC::ScanIdx m_scanIdx;
public:
    static const auto Id = ElementId::scanIdx;

    ScanIdx(
            PelCoord, Log2, Plane,
            ChromaFormatIdc,
            PredictionMode,
            const CodingUnit &,
            const ResidualCoding &rc);

    operator HEVC::ScanIdx () const
    {
        return m_scanIdx;
    }
};
/*----------------------------------------------------------------------------*/
class TransformSkipFlag:
    public Embedded,
    public CABAD::FixedLength
{
public:
    static const auto Id = ElementId::transform_skip_flag;

    TransformSkipFlag():
        /* 04/2013, 9.3.3 "Binarization process", Table 9-32 */
        CABAD::FixedLength{1}
    {}

    explicit operator bool () const
    {
        return getValue();
    }

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
            Plane plane)
    {
        const auto contextModel =
            [plane](CABAD::State &state, int binIdx) -> CABAD::Variable &
            {
                /* 04/2013, 9.3.4.2
                 * "Derivation process for ctxTable, ctxIdx and bypassFlag", Table 9-37 */
                syntaxCheck(0 == binIdx);
                return
                    Plane::Y == plane
                    ? state.getVariable(CABAD::CtxId::transform_skip_flag_luma, 0)
                    : state.getVariable(CABAD::CtxId::transform_skip_flag_chroma, 0);
            };

        getFrom(streamAccessLayer, decoder, cabadState(decoder), *this, contextModel);
    }
};
/*----------------------------------------------------------------------------*/
struct ExplicitRdpcmFlag:
    public Embedded,
    public CABAD::FixedLength
{
    static const auto Id = ElementId::explicit_rdpcm_flag;

    ExplicitRdpcmFlag():
        /* 10/2014, 9.3.3 "Binarization process", Table 9-38 */
        CABAD::FixedLength{1}
    {
        /* 10/2014, 7.4.9.11 "Residual coding semantics" */
        setValue(0);
    }

    explicit operator bool () const
    {
        return getValue();
    }

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
            Plane plane)
    {
        const auto contextModel =
            [plane](CABAD::State &state, int binIdx) -> CABAD::Variable &
            {
                /* 10/2014, 9.3.4.2
                 * "Derivation process for ctxTable, ctxIdx and bypassFlag", Table 9-43 */
                syntaxCheck(0 == binIdx);
                return
                    Plane::Y == plane
                    ? state.getVariable(CABAD::CtxId::explicit_rdpcm_flag_luma, 0)
                    : state.getVariable(CABAD::CtxId::explicit_rdpcm_flag_chroma, 0);
            };

        getFrom(streamAccessLayer, decoder, cabadState(decoder), *this, contextModel);
    }
};
/*----------------------------------------------------------------------------*/
struct ExplicitRdpcmDirFlag:
    public Embedded,
    public CABAD::FixedLength
{
    static const auto Id = ElementId::explicit_rdpcm_dir_flag;

    ExplicitRdpcmDirFlag():
        /* 10/2014, 9.3.3 "Binarization process", Table 9-38 */
        CABAD::FixedLength{1}
    {}

    explicit operator bool () const
    {
        return getValue();
    }

    Direction inUnits() const
    {
        return *this ? Direction::V : Direction::H;
    }

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
            Plane plane)
    {
        const auto contextModel =
            [plane](CABAD::State &state, int binIdx) -> CABAD::Variable &
            {
                /* 10/2014, 9.3.4.2
                 * "Derivation process for ctxTable, ctxIdx and bypassFlag", Table 9-43 */
                syntaxCheck(0 == binIdx);
                return
                    Plane::Y == plane
                    ? state.getVariable(CABAD::CtxId::explicit_rdpcm_dir_flag_luma, 0)
                    : state.getVariable(CABAD::CtxId::explicit_rdpcm_dir_flag_chroma, 0);
            };

        getFrom(streamAccessLayer, decoder, cabadState(decoder), *this, contextModel);
    }
};
/*----------------------------------------------------------------------------*/
class LastSigCoeffXPrefix:
    public Embedded,
    public CABAD::TruncatedRice
{
public:
    static const auto Id = ElementId::last_sig_coeff_x_prefix;

    LastSigCoeffXPrefix(const Size &size):
        /* 04/2013, 9.3.3 "Binarization process", Table 9-32 */
        CABAD::TruncatedRice{0, toUnderlying(size.inUnits() * 2 - 1_log2)}
    {}

    operator int () const
    {
        return getValue();
    }

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
            Log2 rcSize, Plane plane)
    {
        const auto isLuma = Plane::Y == plane;
        const auto size = toUnderlying(rcSize);
        const auto contextModel =
            [size, isLuma](CABAD::State &state, int binIdx) -> CABAD::Variable &
            {
                const auto ctxOffset = isLuma ? 3 * (size - 2) + ((size - 1) >> 2) : 15;
                const auto ctxShift = isLuma ? (size + 1) >> 2 : size - 2;
                const auto i = (binIdx >> ctxShift) + ctxOffset;
                return state.getVariable(CABAD::CtxId::last_sig_coeff_x_prefix, i);
            };

        getFrom(streamAccessLayer, decoder, cabadState(decoder), *this, contextModel);
    }
};
/*----------------------------------------------------------------------------*/
class LastSigCoeffYPrefix:
    public Embedded,
    public CABAD::TruncatedRice
{
public:
    static const auto Id = ElementId::last_sig_coeff_y_prefix;

    LastSigCoeffYPrefix(const Size &size):
        /* 04/2013, 9.3.3 "Binarization process", Table 9-32 */
        CABAD::TruncatedRice{0, toUnderlying(size.inUnits() * 2 - 1_log2)}
    {}

    operator int () const
    {
        return getValue();
    }

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
            Log2 rcSize, Plane plane)
    {
        const auto isLuma = Plane::Y == plane;
        const auto size = toUnderlying(rcSize);
        const auto contextModel =
            [size, isLuma](CABAD::State &state, int binIdx) -> CABAD::Variable &
            {
                const auto ctxOffset = isLuma ? 3 * (size - 2) + ((size - 1) >> 2) : 15;
                const auto ctxShift = isLuma ? (size + 1) >> 2 : size - 2;
                const auto i = (binIdx >> ctxShift) + ctxOffset;
                return state.getVariable(CABAD::CtxId::last_sig_coeff_y_prefix, i);
            };

        getFrom(streamAccessLayer, decoder, cabadState(decoder), *this, contextModel);
    }
};
/*----------------------------------------------------------------------------*/
class LastSigCoeffXSuffix:
    public Embedded,
    public CABAD::FixedLength
{
public:
    static const auto Id = ElementId::last_sig_coeff_x_suffix;

    LastSigCoeffXSuffix(const LastSigCoeffXPrefix &lastSigCoeffXPrefix):
        /* 04/2013, 9.3.3 "Binarization process", Table 9-32 */
        CABAD::FixedLength{1 << ((lastSigCoeffXPrefix / 2 - 1) - 1)}
    {}

    operator int () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class LastSigCoeffYSuffix:
    public Embedded,
    public CABAD::FixedLength
{
public:
    static const auto Id = ElementId::last_sig_coeff_y_suffix;

    LastSigCoeffYSuffix(const LastSigCoeffYPrefix &lastSigCoeffYPrefix):
        /* 04/2013, 9.3.3 "Binarization process", Table 9-32 */
        CABAD::FixedLength{1 << ((lastSigCoeffYPrefix / 2 - 1) - 1)}
    {}

    operator int () const
    {
        return getValue();
    }
};
/*----------------------------------------------------------------------------*/
class LastSignificantCoeff
{
protected:
    Pel::StoragePrimitive m_lastSignificantCoeff;
public:
    LastSignificantCoeff(): m_lastSignificantCoeff{-1}
    {}

    operator Pel::StoragePrimitive () const
    {
        syntaxCheck(-1 != m_lastSignificantCoeff);
        return m_lastSignificantCoeff;
    }

    Pel inUnits() const
    {
        syntaxCheck(-1 != m_lastSignificantCoeff);
        return Pel{m_lastSignificantCoeff};
    }
protected:
    void set(const LastSigCoeffXPrefix &lastSigCoeffXPrefix)
    {
        m_lastSignificantCoeff = lastSigCoeffXPrefix;
    }

    void set(
            const LastSigCoeffXPrefix &lastSigCoeffXPrefix,
            const LastSigCoeffXSuffix &lastSigCoeffXSuffix)
    {
        m_lastSignificantCoeff =
            (1 << (lastSigCoeffXPrefix / 2 - 1))
            * (2 + (lastSigCoeffXPrefix % 2))
            + lastSigCoeffXSuffix;
    }

    void set(const LastSigCoeffYPrefix &lastSigCoeffYPrefix)
    {
        m_lastSignificantCoeff = lastSigCoeffYPrefix;
    }

    void set(
            const LastSigCoeffYPrefix &lastSigCoeffYPrefix,
            const LastSigCoeffYSuffix &lastSigCoeffYSuffix)
    {
        m_lastSignificantCoeff =
            (1 << (lastSigCoeffYPrefix / 2 - 1))
            * (2 + (lastSigCoeffYPrefix % 2))
            + lastSigCoeffYSuffix;
    }
};
/*----------------------------------------------------------------------------*/
class LastSignificantCoeffX:
    public Embedded,
    public LastSignificantCoeff
{
public:
    static const auto Id = ElementId::LastSignificantCoeffX;

    LastSignificantCoeffX(const ResidualCoding &);
};
/*----------------------------------------------------------------------------*/
class LastSignificantCoeffY:
    public Embedded,
    public LastSignificantCoeff
{
public:
    static const auto Id = ElementId::LastSignificantCoeffY;

    LastSignificantCoeffY(const ResidualCoding &);
};
/*----------------------------------------------------------------------------*/
class CodedSubBlockFlag:
    public Embedded,
    public CABAD::FixedLength
{
    /* max transform block size: 32x32
     * coded subblock size: 4x4 */
    static const auto ratio = 3;

    typedef BitMatrix<1 << ratio, 1 << ratio> Flags;

    static
    Flags::Pos toPos(SubCbCoord coord)
    {
        return {toUnderlying(coord.x()), toUnderlying(coord.y())};
    }

    Flags m_flags;
public:
    static const auto Id = ElementId::coded_sub_block_flag;

    CodedSubBlockFlag(
            const LastSignificantCoeffX &x, const LastSignificantCoeffY &y):
        /* 04/2013, 9.3.3 "Binarization process", Table 9-32 */
        CABAD::FixedLength{1}
    {
        /* inferred */
        m_flags[{0, 0}] = true;
        m_flags[toPos(toSubCb({x.inUnits(), y.inUnits()}))] = true;
    }

    bool operator[] (SubCbCoord coord) const
    {
        return m_flags[toPos(coord)];
    }

    void onParse(
            StreamAccessLayer &, Decoder::State &decoder,
            Plane, Log2, SubCbCoord);
};
/*----------------------------------------------------------------------------*/
class SigCoeffFlag:
    public Embedded,
    public CABAD::FixedLength
{
private:
    typedef BitMatrix<
        Limits::TrafoSize::max,
        Limits::TrafoSize::max> Flags;

    static
    Flags::Pos toPos(PelCoord coord)
    {
        return {toUnderlying(coord.x()), toUnderlying(coord.y())};
    }

    Flags m_flags;
public:
    static const auto Id = ElementId::sig_coeff_flag;

    SigCoeffFlag(
            const LastSignificantCoeffX &x, const LastSignificantCoeffY &y):
        /* 04/2013, 9.3.3 "Binarization process", Table 9-32 */
        CABAD::FixedLength{1}
    {
        // inferred
        m_flags[toPos({x.inUnits(), y.inUnits()})] = true;
    }

    bool operator[] (PelCoord at) const
    {
        return m_flags[toPos(at)];
    }

    auto operator[] (PelCoord at) -> decltype(m_flags[toPos(at)])
    {
        return m_flags[toPos(at)];
    }

    void onParse(
            StreamAccessLayer &, Decoder::State &decoder,
            const CodingUnitContent::CuTransquantBypassFlag &,
            const TransformSkipFlag &,
            const CodedSubBlockFlag &, Plane, ScanIdx, Log2, PelCoord);
};
/*----------------------------------------------------------------------------*/
class CoeffAbsLevelGreater1Flag:
    public Embedded,
    public CABAD::FixedLength
{
    typedef BitMatrix<
        Limits::TrafoSize::max,
        Limits::TrafoSize::max> Flags;

    static
    Flags::Pos toPos(PelCoord coord)
    {
        return {toUnderlying(coord.x()), toUnderlying(coord.y())};
    }

    Flags m_flags;
public:
    struct Ctx
    {
        int16_t ctxSet;
        int16_t greater1Ctx;
        bool greater1Flag;

        Ctx(): ctxSet(0), greater1Ctx(0), greater1Flag(false)
        {}
    };

    mutable Ctx currCtx;

    static const auto Id = ElementId::coeff_abs_level_greater1_flag;

    CoeffAbsLevelGreater1Flag():
        /* 04/2013, 9.3.3 "Binarization process", Table 9-32 */
        CABAD::FixedLength{1}
    {}

    bool operator[] (PelCoord at) const
    {
        return m_flags[toPos(at)];
    }

    void onParse(
            StreamAccessLayer &, Decoder::State &decoder,
            Plane, PelCoord, bool, bool);
};
/*----------------------------------------------------------------------------*/
class CoeffAbsLevelGreater2Flag:
    public Embedded,
    public CABAD::FixedLength
{
    typedef BitMatrix<
        Limits::TrafoSize::max,
        Limits::TrafoSize::max> Flags;

    static
    Flags::Pos toPos(PelCoord coord)
    {
        return {toUnderlying(coord.x()), toUnderlying(coord.y())};
    }

    Flags m_flags;
public:
    static const auto Id = ElementId::coeff_abs_level_greater2_flag;

    CoeffAbsLevelGreater2Flag():
        /* 04/2013, 9.3.3 "Binarization process", Table 9-32 */
        CABAD::FixedLength{1}
    {}

    bool operator[] (PelCoord at) const
    {
        return m_flags[toPos(at)];
    }

    void onParse(
            StreamAccessLayer &, Decoder::State &decoder,
            int, Plane, PelCoord);
};
/*----------------------------------------------------------------------------*/
class CoeffSignFlag:
    public Embedded,
    public CABAD::FixedLength
{
private:
    typedef BitMatrix<
        Limits::TrafoSize::max,
        Limits::TrafoSize::max> Flags;

    static
    Flags::Pos toPos(PelCoord coord)
    {
        return {toUnderlying(coord.x()), toUnderlying(coord.y())};
    }

    Flags m_flags;
public:
    static const auto Id = ElementId::coeff_sign_flag;

    CoeffSignFlag():
        /* 04/2013, 9.3.3 "Binarization process", Table 9-32 */
        CABAD::FixedLength{1}
    {}

    bool operator[] (PelCoord at) const
    {
        return m_flags[toPos(at)];
    }

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
            PelCoord at)
    {
        getFrom(streamAccessLayer, decoder, cabadState(decoder), *this);
        m_flags[toPos(at)] = getValue();
    }
};
/*----------------------------------------------------------------------------*/
class CoeffAbsLevelRemainingDebinarizer: public CABAD::Debinarizer
{
protected:
    /*
     * Allowed ranges:
     * range(baseLevel) == [1, 3]
     * range(TransCoeffLevel) = [-32768, 32767]
     * TransCoeffLevel == +/- coeff_abs_level_remaining + baseLevel
     * range(coeff_abs_level_remaining) == [0, 32767]
     * range(cAbsLevel) == range(coeff_abs_level_remaining + baseLevel) */
    struct Ctx
    {
        uint32_t cLastAbsLevel : 24;
        /* allowed range [0, 4] */
        uint32_t cLastRiceParam : 8;

        Ctx(): cLastAbsLevel(0), cLastRiceParam(0)
        {}

        void clear(int initRiceValue)
        {
            cLastAbsLevel = 0;
            cLastRiceParam = initRiceValue;
        }
    };

    static_assert(
            sizeof(Ctx) == sizeof(uint32_t),
            "CoeffAbsLevelRemaining: unexpected Ctx size.");

    Ctx m_currCtx;
public:
    static
    CABAD::BinValueType getBinarizationValue(
            CABAD::BinString prefix, CABAD::BinString suffix,
            int cRiceParam, int cTRMax,
            bool extendedPrecisionProcessingFlag,
            int bitDepth)
    {
        /* 04/2013, 9.3.3.9, "Binarization process for coeff_abs_level_remaining" */
        const auto prefixValue =
            CABAD::TruncatedRiceDebinarizer::getBinarizationValue(
                    prefix, cRiceParam, cTRMax);

        if(CABAD::BinString({1, 1, 1, 1}) == prefix)
        {
            const auto codeOrder = cRiceParam + 1;
            const auto suffixValue =
                extendedPrecisionProcessingFlag
                ? CABAD::LimitedExpGolomb::getBinarizationValue(
                        suffix, codeOrder, bitDepth)
                : CABAD::ExpGolombDebinarizer::getBinarizationValue(
                        suffix, codeOrder);

            return prefixValue + suffixValue;
        }

        return prefixValue;
    }

    struct Binarization
    {
        CABAD::BinString prefix;
        CABAD::BinString suffix;
        int cRiceParam;
        int cTRMax;

        Binarization(
                CABAD::BinString prefix_, CABAD::BinString suffix_,
                int cRiceParam_, int cTRMax_):
            prefix(prefix_), suffix(suffix_),
            cRiceParam(cRiceParam_), cTRMax(cTRMax_)
        {}
    };

    static
    Binarization getBinarization(
            StreamAccessLayer &streamAccessLayer, CABAD::State &state,
            int cLastAbsLevel, int cLastRiceParam,
            bool extendedPrecisionProcessingFlag,
            bool persistentRiceAdaptationEnabledFlag,
            int bitDepth)
    {

        /* 04/2013, 9.3.3.9, "Binarization process for coeff_abs_level_remaining" */
        const auto delta = cLastAbsLevel > (3 * (1 << cLastRiceParam)) ? 1 : 0;
        const auto cRiceParam =
            persistentRiceAdaptationEnabledFlag
            ? cLastRiceParam + delta
            : std::min(cLastRiceParam + delta, 4);
        const auto cTRMax = 4 << cRiceParam;

        const auto prefix =
            CABAD::TruncatedRiceDebinarizer::getBinarization(
                    streamAccessLayer, state,
                    cRiceParam, cTRMax);

        if(CABAD::BinString({1, 1, 1, 1}) == prefix)
        {
            /* suffix is present */
            const auto codeOrder = cRiceParam + 1;

            const auto suffix =
                extendedPrecisionProcessingFlag
                ? CABAD::LimitedExpGolomb::getBinarization(
                        streamAccessLayer, state,
                        codeOrder, bitDepth)
                : CABAD::ExpGolombDebinarizer::getBinarization(
                        streamAccessLayer, state,
                        codeOrder);

            return {prefix, suffix, cRiceParam, cTRMax};
        }

        return {prefix, CABAD::BinString{}, cRiceParam, cTRMax};
    }

    CABAD::BinValueType getBinarizationValue(
            StreamAccessLayer &streamAccessLayer, CABAD::State &state,
            bool extendedPrecisionProcessingFlag,
            bool persistentRiceAdaptationEnabledFlag,
            int bitDepth)
    {
        const auto binarization =
            getBinarization(
                    streamAccessLayer, state,
                    m_currCtx.cLastAbsLevel, m_currCtx.cLastRiceParam,
                    extendedPrecisionProcessingFlag,
                    persistentRiceAdaptationEnabledFlag,
                    bitDepth);

        m_binarization = join(binarization.prefix, binarization.suffix);
        m_currCtx.cLastRiceParam = binarization.cRiceParam;
        return
            getBinarizationValue(
                    binarization.prefix, binarization.suffix,
                    binarization.cRiceParam, binarization.cTRMax,
                    extendedPrecisionProcessingFlag,
                    bitDepth);
    }
};
/*----------------------------------------------------------------------------*/
class CoeffAbsLevelRemaining:
    public Embedded,
    public CABAD::ContextAdaptive<CoeffAbsLevelRemainingDebinarizer>
{
    typedef uint16_t AbsLevel;
public:
    static const auto Id = ElementId::coeff_abs_level_remaining;

    int absLevel() const
    {
        return getValue();
    }

    void onParse(
            StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
            int baseLevel,
            bool isFirstInSubBlock,
            bool cuTransquantBypassFlag,
            bool transformSkipFlag,
            bool extendedPrecisionProcessingFlag,
            bool persistentRiceAdaptationEnabledFlag,
            Plane plane,
            int bitDepth)
    {
        /* 04/2014, 9.3.3.10 "Binarization process for coeff_abs_level_remaining" */
        auto &statCoeff = cabadState(decoder).statCoeff;

        const auto sbType =
            !transformSkipFlag && !cuTransquantBypassFlag
            ? 2 * (Plane::Y == plane ? 1 : 0)
            : 2 * (Plane::Y == plane ? 1 : 0) + 1;

        const auto initRiceValue =
            persistentRiceAdaptationEnabledFlag
            ? statCoeff[sbType] / 4
            : 0;

        if(isFirstInSubBlock)
        {
            m_currCtx.clear(initRiceValue);
        }

        getFrom(
                streamAccessLayer, decoder, cabadState(decoder), *this,
                extendedPrecisionProcessingFlag,
                persistentRiceAdaptationEnabledFlag,
                bitDepth);

        m_currCtx.cLastAbsLevel = baseLevel + getValue();

        if(persistentRiceAdaptationEnabledFlag && isFirstInSubBlock)
        {
            if(int(getValue()) >= (3 << (statCoeff[sbType] / 4)))
            {
                ++statCoeff[sbType];
            }
            else if(
                    2 * int(getValue()) < (1 << (statCoeff[sbType] / 4))
                    && 0 < statCoeff[sbType])
            {
                --statCoeff[sbType];
            }
        }
    }
};
/*----------------------------------------------------------------------------*/
} /* ResidualCodingContent */

class ResidualCoding:
    public EmbeddedAggregator<
        ResidualCodingContent::Coord,
        ResidualCodingContent::Size,
        ResidualCodingContent::CIdx,
        ResidualCodingContent::ScanIdx,
        ResidualCodingContent::TransformSkipFlag,
        ResidualCodingContent::ExplicitRdpcmFlag,
        ResidualCodingContent::ExplicitRdpcmDirFlag,
        ResidualCodingContent::LastSigCoeffXPrefix,
        ResidualCodingContent::LastSigCoeffYPrefix,
        ResidualCodingContent::LastSigCoeffXSuffix,
        ResidualCodingContent::LastSigCoeffYSuffix,
        ResidualCodingContent::CodedSubBlockFlag,
        ResidualCodingContent::SigCoeffFlag,
        ResidualCodingContent::CoeffAbsLevelGreater1Flag,
        ResidualCodingContent::CoeffAbsLevelGreater2Flag,
        ResidualCodingContent::CoeffSignFlag,
        ResidualCodingContent::CoeffAbsLevelRemainingDebinarizer,
        ResidualCodingContent::CoeffAbsLevelRemaining,
        ResidualCodingContent::LastSignificantCoeff,
        ResidualCodingContent::LastSignificantCoeffX,
        ResidualCodingContent::LastSignificantCoeffY>
{
public:
    static const auto Id = ElementId::residual_coding;

    typedef ResidualCodingContent::Coord Coord;
    typedef ResidualCodingContent::Size Size;
    typedef ResidualCodingContent::CIdx CIdx;
    typedef ResidualCodingContent::ScanIdx ScanIdx;
    typedef ResidualCodingContent::TransformSkipFlag TransformSkipFlag;
    typedef ResidualCodingContent::ExplicitRdpcmFlag ExplicitRdpcmFlag;
    typedef ResidualCodingContent::ExplicitRdpcmDirFlag ExplicitRdpcmDirFlag;
    typedef ResidualCodingContent::LastSigCoeffXPrefix LastSigCoeffXPrefix;
    typedef ResidualCodingContent::LastSigCoeffYPrefix LastSigCoeffYPrefix;
    typedef ResidualCodingContent::LastSigCoeffXSuffix LastSigCoeffXSuffix;
    typedef ResidualCodingContent::LastSigCoeffYSuffix LastSigCoeffYSuffix;
    typedef ResidualCodingContent::CodedSubBlockFlag CodedSubBlockFlag;
    typedef ResidualCodingContent::SigCoeffFlag SigCoeffFlag;
    typedef ResidualCodingContent::CoeffAbsLevelGreater1Flag CoeffAbsLevelGreater1Flag;
    typedef ResidualCodingContent::CoeffAbsLevelGreater2Flag CoeffAbsLevelGreater2Flag;
    typedef ResidualCodingContent::CoeffSignFlag CoeffSignFlag;
    typedef ResidualCodingContent::CoeffAbsLevelRemainingDebinarizer CoeffAbsLevelRemainingDebinarizer;
    typedef ResidualCodingContent::CoeffAbsLevelRemaining CoeffAbsLevelRemaining;
    typedef ResidualCodingContent::LastSignificantCoeff LastSignificantCoeff;
    typedef ResidualCodingContent::LastSignificantCoeffX LastSignificantCoeffX;
    typedef ResidualCodingContent::LastSignificantCoeffY LastSignificantCoeffY;

    ResidualCoding(PelCoord rcCoord, Log2 rcSize, Plane plane)
    {
        embed<Coord>(*this, rcCoord);
        embed<Size>(*this, rcSize);
        embed<CIdx>(*this, plane);
    }

    void onParse(StreamAccessLayer &, Decoder::State &, const CodingUnit &);
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */

#endif /* HEVC_Syntax_ResidualCoding_h */
