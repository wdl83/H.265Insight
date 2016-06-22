#ifndef Syntax_CABAD_Debinarizer_h
#define Syntax_CABAD_Debinarizer_h
/* STDC++ */
#include <type_traits>
#include <cmath>
/* HEVC */
#include <Syntax/CABAD/BinString.h>
#include <Syntax/CABAD/State.h>

namespace HEVC { namespace Syntax { namespace CABAD {
/*----------------------------------------------------------------------------*/
typedef uint32_t BinValueType;

enum class DebinarizerId
{
    TruncatedRice = 0,
    ExpGolomb,
    LimitedExpGolomb,
    FixedLength,
    Special
};

inline
const std::string &getName(DebinarizerId id)
{
    static const std::string name[] =
    {
        "TR", "EGk", "LimitedEGk" "FL", "Special"
    };

    return name[int(id)];
}
/*----------------------------------------------------------------------------*/
/* Debinarizer */
/*----------------------------------------------------------------------------*/
class Debinarizer
{
protected:
    BinString m_binarization;
public:
    Debinarizer()
    {}

    BinString getBinString() const
    {
        return m_binarization;
    }
};

template <typename T>
using IsDebinarizer =
    typename std::enable_if<std::is_base_of<Debinarizer, T>::value>::type;
/*----------------------------------------------------------------------------*/
/* FixedLength */
/*----------------------------------------------------------------------------*/
class FixedLengthDebinarizer: public Debinarizer
{
public:
    static const auto debinarizerId = DebinarizerId::FixedLength;
    int m_cMax;

    FixedLengthDebinarizer(int cMax = 0): m_cMax(cMax)
    {}

    template <typename ...ContextModel>
    static
    BinString getBinarization(
            StreamAccessLayer &streamAccessLayer,
            State &state,
            int cMax,
            ContextModel &...contextModel)
    {
        auto &arithmeticDecoder = state.arithmeticDecoder;
        const auto lengthInBits = std::ceil(std::log2(float(cMax + 1)));
        BinString binarization;

        for(int binIdx = 0; binIdx < lengthInBits; ++binIdx)
        {
            const auto binValue =
                arithmeticDecoder.decodeBin(
                        streamAccessLayer,
                        contextModel(state, binIdx)...);

            binarization.pushBack(binValue);
        }

        return binarization;
    }

    static
    BinValueType getBinarizationValue(BinString binarization)
    {
        const auto size = binarization.size();
        BinValueType value = 0;

        /* 0 == binIdx
         * relates to the MSB with increasing values of binIdx towards the LSB */
        for(int binIdx = 0; binIdx < size; ++binIdx)
        {
            if(binarization[binIdx])
            {
                value |= 1 << (size - 1 - binIdx);
            }
        }

        return value;
    }

    template <typename ...ContextModel>
    BinValueType getBinarizationValue(
            StreamAccessLayer &streamAccessLayer,
            State &state,
            ContextModel &... contexModel)
    {
        m_binarization =
            getBinarization(streamAccessLayer, state, m_cMax, contexModel...);
        return getBinarizationValue(m_binarization);
    }
};
/*----------------------------------------------------------------------------*/
/* TruncatedRice */
/*----------------------------------------------------------------------------*/
class TruncatedRiceDebinarizer: public Debinarizer
{
public:
    static const auto debinarizerId = DebinarizerId::TruncatedRice;
    int m_cRiceParam;
    int m_cTRMax;

    TruncatedRiceDebinarizer(int cRiceParam = 0, int cTRMax = 0):
        m_cRiceParam(cRiceParam), m_cTRMax(cTRMax)
    {}

    template <typename ...ContextModel>
    static
    BinString getBinarization(
            StreamAccessLayer &streamAccessLayer,
            State &state,
            int cRiceParam, int cTRMax,
            ContextModel &... contexModel)
    {
        /* 04/2013, 9.3.3.2 "Truncated Rice (TR) binarization process." */

        /* synVal == prefixVal << cRiceParam + suffixVal */

        auto &arithmeticDecoder = state.arithmeticDecoder;
        const int cMax = cTRMax >> cRiceParam;

        syntaxCheck(cTRMax == cMax << cRiceParam);

        BinString prefix;
        BinValueType prefixValue = 0;

        /*
         * prefixVal = synVal >> cRiceParam
         * if 0 == cRiceParam then prefixVal == synVal
         *
         * If prefixVal is less than cMax, the prefix bin string is a bit string of
         * length prefixVal + 1 indexed by binIdx.
         * The bins for binIdx less than prefixVal are equal to 1.
         * The bin with binIdx equal to prefixVal is equal to 0.
         * Otherwise, the bin string is bit string of length cMax with all
         * bins being equal to 1. */

        for(int binIdx = 0; binIdx < cMax; ++binIdx)
        {
            const auto binValue =
                arithmeticDecoder.decodeBin(
                        streamAccessLayer,
                        contexModel(state, binIdx)...);

            prefix.pushBack(binValue);

            if(false == binValue)
            {
                break;
            }
            else
            {
                ++prefixValue;
            }
        }

        BinString binarization(prefix);

        const bool suffixPresentConditionA = 0 < cRiceParam;

        /* When cMax > synVal suffix is present:
         *  cMax > synVal <=> cMax > (prefixVal << cRiceParam) + suffixVal
         *  for suffix to be present cMax > prefixVal */
        const bool suffixPresentConditionB = cMax > int(prefixValue);

        if(suffixPresentConditionA && suffixPresentConditionB)
        {
            BinString suffix =
                FixedLengthDebinarizer::getBinarization(
                        streamAccessLayer,
                        state,
                        (1 << cRiceParam) - 1);

            append(binarization, suffix);
        }

        return binarization;
    }

    static
    BinValueType getBinarizationValue(
            BinString binarization, int cRiceParam, int cTRMax)
    {
        const int cMax = cTRMax >> cRiceParam;

        BinValueType prefixValue = 0;
        int binIdx = 0;

        for(; binIdx < cMax; ++binIdx)
        {
            syntaxCheck(binarization.size() > binIdx);

            if(false == binarization[binIdx])
            {
                ++binIdx; /* go to next bin */
                break;
            }
            else
            {
                ++prefixValue;
            }
        }

        BinValueType suffixValue = 0;

        if(binarization.size() > binIdx)
        {
            suffixValue =
                FixedLengthDebinarizer::getBinarizationValue(
                        copy(binarization, binIdx, binarization.size()));
        }

        return (prefixValue << cRiceParam) + suffixValue;
    }

    template <typename ...ContextModel>
    BinValueType getBinarizationValue(
            StreamAccessLayer &streamAccessLayer,
            State &state,
            ContextModel &... contexModel)
    {
        m_binarization =
            getBinarization(streamAccessLayer, state, m_cRiceParam, m_cTRMax, contexModel...);
        return getBinarizationValue(m_binarization, m_cRiceParam, m_cTRMax);
    }
};
/*----------------------------------------------------------------------------*/
/* ExpGolomb */
/*----------------------------------------------------------------------------*/
class ExpGolombDebinarizer: public Debinarizer
{
public:
    static const auto debinarizerId = DebinarizerId::ExpGolomb;
    int m_codeOrder;

    ExpGolombDebinarizer(int codeOrder):
        m_codeOrder(codeOrder)
    {}

    template <typename ...ContextModel>
    static
    BinString getBinarization(
            StreamAccessLayer &streamAccessLayer,
            State &state,
            int codeOrder,
            ContextModel &... contextModel)
    {
        auto &arithmeticDecoder = state.arithmeticDecoder;
        BinString binarization;
        int binIdx = 0;

        while(true)
        {
            const auto binValue =
                arithmeticDecoder.decodeBin(
                        streamAccessLayer,
                        contextModel(state, binIdx)...);

            binarization.pushBack(binValue);
            ++binIdx;

            if(binValue)
            {
                ++codeOrder;
            }
            else
            {
                break;
            }
        }

        while(codeOrder)
        {
            const auto binValue =
                arithmeticDecoder.decodeBin(
                        streamAccessLayer,
                        contextModel(state, binIdx)...);

            binarization.pushBack(binValue);
            ++binIdx;
            --codeOrder;
        }

        return binarization;
    }

    static
    BinValueType getBinarizationValue(BinString binarization, int codeOrder)
    {
        int power = 0;
        int binIdx = 0;

        while(true)
        {
            syntaxCheck(binarization.size() > binIdx);
            const auto binValue = binarization[binIdx];
            ++binIdx;

            if(binValue)
            {
                power += 1 << codeOrder;
                ++codeOrder;
            }
            else
            {
                break;
            }
        }

        BinValueType residuum = 0;

        while(codeOrder)
        {
            syntaxCheck(binarization.size() > binIdx);
            const auto binValue = binarization[binIdx];

            if(binValue)
            {
                residuum |= 1 << (codeOrder - 1);
            }

            ++binIdx;
            --codeOrder;
        }

        return power + residuum;
    }

    template <typename ...ContextModel>
    BinValueType getBinarizationValue(
            StreamAccessLayer &streamAccessLayer,
            State &state,
            ContextModel &... contexModel)
    {
        m_binarization =
            getBinarization(streamAccessLayer, state, m_codeOrder, contexModel...);
        return getBinarizationValue(m_binarization, m_codeOrder);
    }
};
/*----------------------------------------------------------------------------*/
class LimitedExpGolomb
{
    /* 10/2014, 9.3.3.4 "Limited k-th order Exp-Golomb (EGk) binarization process" */
public:
    static const auto debinarizerId = DebinarizerId::LimitedExpGolomb;

    LimitedExpGolomb()
    {
    }

    template <typename ...ContextModel>
    static
    BinString getBinarization(
            StreamAccessLayer &streamAccessLayer,
            State &state,
            int codeOrder,
            int bitDepth,
            ContextModel &...contextModel)
    {
        auto &arithmeticDecoder = state.arithmeticDecoder;
        const auto transformRange = std::max(15_log2, Log2{bitDepth} + 6_log2);
        const auto maxPrefixExtLength = 28_log2 - transformRange;
        auto prefixLength = 0_log2;
        auto suffixLength = 0_log2;
        BinString prefix;

        while(true)
        {
            const auto binValue =
                arithmeticDecoder.decodeBin(
                        streamAccessLayer,
                        contextModel(state, prefix.size())...);

            prefix.pushBack(binValue);

            if(maxPrefixExtLength > prefixLength && binValue)
            {
                ++prefixLength;
            }
            else
            {
                break;
            }
        }

        if(maxPrefixExtLength == prefixLength)
        {
            suffixLength = transformRange;
        }
        else
        {
            suffixLength = prefixLength + Log2{codeOrder};
        }

        const auto suffix =
            FixedLengthDebinarizer::getBinarization(
                    streamAccessLayer,
                    state,
                    toInt(suffixLength) - 1,
                    contextModel...);

        return join(prefix, suffix);
    }

    static
    BinValueType getBinarizationValue(
            BinString binarization, int codeOrder, int bitDepth)
    {
        const auto transformRange = std::max(15_log2, Log2{bitDepth} + 6_log2);
        const auto maxPrefixExtLength = 28_log2 - transformRange;
        auto prefixLength = 0_log2;
        auto binIdx = 0;

        while(true)
        {
            const auto binValue = binarization[binIdx];

            if(maxPrefixExtLength > prefixLength && binValue)
            {
                ++binIdx;
                ++prefixLength;
            }
            else
            {
                break;
            }
        }
#if 0
        auto suffixLength = 0_log2;

        if(maxPrefixExtLength == prefixLength)
        {
            suffixLength = transformRange;
        }
        else
        {
            suffixLength = prefixLength + Log2{codeOrder};
        }
#endif

        int prefixValue = (toInt(prefixLength) - 1) << codeOrder;
        int suffixValue =
            FixedLengthDebinarizer::getBinarizationValue(
                    copy(binarization, binIdx, binarization.size()));

        return prefixValue + suffixValue;
    }
};
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Syntax::CABAD */

#endif /* Syntax_CABAD_Debinarizer_h */
