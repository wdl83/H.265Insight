#ifndef HEVC_log_h
#define HEVC_log_h

/* STDC++ */
#include <algorithm>
#include <array>
#include <cstdint>
#include <ostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <initializer_list>
#include <string>
#include <utility>
/* HEVC */
#include <EnumUtils.h>
#include <Tuple.h>
#include <runtime_assert.h>

namespace HEVC {
/*----------------------------------------------------------------------------*/
enum class LogId
{
    Begin,
    Syntax = Begin,
    NalUnits,
    SEI,
    Sequence,
    Picture,
    Slice,
    CodingTreeUnit,
    Debug,
    ArithmeticDecoderState,
    QuantizationYpred,
    QuantizationY,
    QuantizationCb,
    QuantizationCr,
    Prediction,
    TransformCoeffLevels,
    Headers,
    ReferencePictureSetParam,
    ScalingList,
    ScalingFactor,
    PictureOrderCount,
    ReferencePictureSet,
    ReferencePictureList,
    PredWeightTable,
    PcmSamplesY,
    PcmSamplesCb,
    PcmSamplesCr,
    ScaledTransformCoeffsY,
    ScaledTransformCoeffsCb,
    ScaledTransformCoeffsCr,
    TransformedSamplesY,
    TransformedSamplesCb,
    TransformedSamplesCr,
    IntraAdjSamplesY,
    IntraAdjSamplesCb,
    IntraAdjSamplesCr,
    IntraAdjSubstitutedSamplesY,
    IntraAdjSubstitutedSamplesCb,
    IntraAdjSubstitutedSamplesCr,
    IntraAdjFilteredSamplesY,
    IntraAdjFilteredSamplesCb,
    IntraAdjFilteredSamplesCr,
    IntraRefSamplesY,
    IntraRefSamplesCb,
    IntraRefSamplesCr,
    IntraAngularRefSamplesY,
    IntraAngularRefSamplesCb,
    IntraAngularRefSamplesCr,
    IntraPredictedSamplesY,
    IntraPredictedSamplesCb,
    IntraPredictedSamplesCr,
    MotionVectors,
    MvMergeMode,
    MvSpatialMergeCandidate,
    MvCombinedBiPredMergeCandidate,
    MvZeroMergeCandidate,
    MvPrediction,
    MvPredictorCandidate,
    MvTemporalPrediction,
    MvCollocated,
    InterFractionalSamplesInterpolationY,
    InterFractionalSamplesInterpolationCb,
    InterFractionalSamplesInterpolationCr,
    InterPredictedSamplesY,
    InterPredictedSamplesCb,
    InterPredictedSamplesCr,
    InterWeightedSamplesPredictionY,
    InterWeightedSamplesPredictionCb,
    InterWeightedSamplesPredictionCr,
    ResidualsY,
    ResidualsCb,
    ResidualsCr,
    CrossComponentPredictionCb,
    CrossComponentPredictionCr,
    ReconstructedY,
    ReconstructedCb,
    ReconstructedCr,
    DeblockTransformEdges,
    DeblockPredictionEdges,
    DeblockEdges,
    DeblockBS,
    DeblockD,
    DeblockedY,
    DeblockedCb,
    DeblockedCr,
    SaoCtbMaskLuma,
    SaoCtbMaskChroma,
    DecodedY,
    DecodedCb,
    DecodedCr,
    DecodedPictureBuffer,
    ParserStats,
    ProcessStats,
    End,
    All = End
};

const std::string &toStr(LogId);
/*----------------------------------------------------------------------------*/
const std::string allLogsIndicator = "all";
bool enableLog(const std::string &name);
bool isEnabled(LogId id);
std::ostream &toStream(LogId id);
/*----------------------------------------------------------------------------*/
template <typename, typename>
struct IsOp;

template <typename Op, typename R, typename ...A_n>
struct IsOp<Op, R(A_n...)>
{
    template <typename T>
    static
    auto match(T*) ->
    typename std::is_same<decltype(std::declval<T>().operator()(std::declval<A_n>()...)), R>::type;

    template <typename>
    static
    std::false_type match(...);

    typedef decltype(match<Op>(nullptr)) Type;
    static constexpr bool value = Type::value;
};

template <typename Op>
void logSerialize(
        std::ostream &os,
        Op op,
        typename std::enable_if<IsOp<Op, void(std::ostream &)>::value>::type * = nullptr)
{
    op(os);
}

template <typename T>
void logSerialize(
        std::ostream &os,
        const T &v,
        typename std::enable_if<!IsOp<T, void(std::ostream &)>::value>::type * = nullptr)
{
    os << v;
}

inline
void logSerialize(std::ostream &os, uint8_t v)
{
    os << int(v);
}

inline
void logSerialize(std::ostream &os, int8_t v)
{
    os << int(v);
}

template <size_t n>
inline
void logSerialize(std::ostream &os, const std::array<bool, n> &a)
{
    for(const auto i : a)
    {
        os << (i ? '1' : '0');
    }
}
/*----------------------------------------------------------------------------*/
inline
void logImpl(std::ostream &)
{}

template <typename T, typename... A_n>
void logImpl(std::ostream &os, const T &v, const A_n &... arg_n)
{
    logSerialize(os, v);
    logImpl(os, arg_n...);
}
/*----------------------------------------------------------------------------*/
template <typename... A_n>
void log(LogId id, const A_n &... arg_n)
{
    if(LogId::All == id)
    {
        std::ostringstream oss;

        logImpl(oss, arg_n...);

        const auto str = oss.str();

        for(auto i : EnumRange<LogId>())
        {
            if(isEnabled(i))
            {
                toStream(i) << str;
            }
        }
    }
    else
    {
        if(isEnabled(id))
        {
            logImpl(toStream(id), arg_n...);
        }
    }
}
/*----------------------------------------------------------------------------*/
template <typename... A_n>
void log(const std::vector<LogId> &list, const A_n &... arg_n)
{
    if(
            std::any_of(
                list.cbegin(), list.cend(),
                [](LogId logId){return isEnabled(logId);}))
    {
        std::ostringstream oss;

        logImpl(oss, arg_n...);

        const auto str = oss.str();

        for(const auto i : list)
        {
            if(isEnabled(i))
            {
                toStream(i) << str;
            }
        }
    }
}
/*----------------------------------------------------------------------------*/
void flush(LogId id);
/*----------------------------------------------------------------------------*/
template <char f, int n, typename ...T_s>
struct Align: public Tuple<const T_s &...>
{
    Align(const T_s &... a_n):
        Tuple<const T_s &...>{a_n...}
    {}
};

struct ApplyLogImpl
{
    std::ostream &m_os;

    ApplyLogImpl(std::ostream &os): m_os{os}
    {}

    template <typename T>
    void operator() (const T &v) const
    {
        logImpl(m_os, v);
    }
};

template <char f, int n, typename ...A_n>
std::ostream &operator<< (std::ostream &os, const Align<f, n, A_n...> &a)
{
    const auto begin = os.tellp();

    apply(a, ApplyLogImpl{os});

    const auto end = os.tellp();

    const int d = end - begin;

    std::fill_n(std::ostreambuf_iterator<char>(os), n > d ? n - d : 0, f);
    return os;
}

template <char f = ' ', int n = 50, typename ...A_n>
Align<f, n, A_n...> align(const A_n &... a_n)
{
    return Align<f, n, A_n...>{a_n...};
}
/*----------------------------------------------------------------------------*/
} /* HEVC */

#endif /* HEVC_log_h */
