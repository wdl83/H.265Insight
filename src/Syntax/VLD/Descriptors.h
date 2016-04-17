#ifndef HEVC_Syntax_VLD_Descriptors_h
#define HEVC_Syntax_VLD_Descriptors_h
/* STDC++ */
#include <initializer_list>
#include <type_traits>
#include <ostream>
#include <iomanip>
#include <iterator>
/* HEVC */
#include <Syntax/Syntax.h>
#include <Syntax/Descriptor.h>
#include <Syntax/check.h>
#include <StreamAccessLayer.h>

namespace HEVC { namespace Syntax { namespace VLD {
/*----------------------------------------------------------------------------*/
struct TagVLD
{};

class ExpGolombCode: public Descriptor
{
public:
    typedef uint32_t CodeType;

    CodeType getCode(StreamAccessLayer &streamAccessLayer) const
    {
        /* 10v34, 9.2, "Parsing process for 0-th order Exp-Golomb codes" */
        const auto leadingZerosNum = getLeadingZerosNum(streamAccessLayer);

        if(0 < leadingZerosNum)
        {
            const auto prefix = CodeType(1) << leadingZerosNum;
            const auto suffix = streamAccessLayer.getBits<CodeType>(leadingZerosNum);
            const auto code = prefix - CodeType(1) + suffix;

            return code;
        }

        return 0;
    }
private:
    int getLeadingZerosNum(StreamAccessLayer &streamAccessLayer) const
    {
        /* 10v34, 9.2, "Parsing process for 0-th order Exp-Golomb codes" */
        int leadingZeroBits = 0;

        while(true)
        {
            if(!streamAccessLayer.isEndOfStream())
            {
                if(streamAccessLayer.getBit())
                {
                    break;
                }
                else
                {
                    ++leadingZeroBits;
                }
            }
            else
            {
                syntaxCheck(false);
            }
        }

        return leadingZeroBits;
    }
};
/*----------------------------------------------------------------------------*/
/* se(v) */
class IntExpGolombCoded:
    public TagVLD,
    public ExpGolombCode,
    public StoreByValue<int32_t>
{
public:
    static const auto descriptorId = DescriptorId::SignedExpGolombCoded;

    IntExpGolombCoded():
        StoreByValue(0)
    {}

    void onGetFrom(StreamAccessLayer &streamAccessLayer)
    {
        /* 10v34, 9.2.2, "Mapping process for signed Exp-Golomb codes" */
        const CodeType code = getCode(streamAccessLayer);

        setValue(std::ceil(static_cast<float>(code) / 2) * (0 == code % 2 ? -1 : 1));
    }
};
/*----------------------------------------------------------------------------*/
/* ue(v) */
class UIntExpGolombCoded:
    public TagVLD,
    public ExpGolombCode,
    public StoreByValue<uint32_t>
{
public:
    static const auto descriptorId = DescriptorId::UnsignedExpGolombCoded;

    UIntExpGolombCoded():
        StoreByValue(0)
    {}

    void onGetFrom(StreamAccessLayer &streamAccessLayer)
    {
        /* (Draft 7) ISO/IEC 23008-2 : 201x, 9.1 */
        setValue(getCode(streamAccessLayer));
    }
};
/*----------------------------------------------------------------------------*/
/* f(n) */
template <bool ...bit>
class FixedPattern:
    public TagVLD,
    public Descriptor
{
public:
    static const auto descriptorId = DescriptorId::FixedPattern;

    static const std::array<bool, sizeof...(bit)> pattern;

    static const std::array<bool, sizeof...(bit)> &getValue()
    {
        return pattern;
    }

    void onGetFrom(StreamAccessLayer &streamAccessLayer)
    {
        auto i = std::begin(pattern);
        const auto end = std::end(pattern);

        for(; i != end; ++i)
        {
            if(*i != streamAccessLayer.getBit())
            {
                syntaxCheck(false);
            }
        }
    }
};

template <bool ...bit>
const std::array<bool, sizeof...(bit)> FixedPattern<bit...>::pattern = {{bit...}};
/*----------------------------------------------------------------------------*/
/* i(n) */
template <int NumberOfBits, typename StoragePrimitive> class FixedInt:
    public TagVLD,
    public Descriptor,
    public StoreByValue<StoragePrimitive>
{
public:
    static const auto descriptorId = DescriptorId::FixedSigned;
    using StoreByValue<StoragePrimitive>::getValue;
    using StoreByValue<StoragePrimitive>::setValue;
    using typename StoreByValue<StoragePrimitive>::ValueType;

    FixedInt():
        StoreByValue<StoragePrimitive>(0)
    {
        static_assert(
                getSizeInBits<StoragePrimitive>() >= NumberOfBits,
                "FixedInt, StoragePrimitive insufficient capacity.");

        static_assert(
                std::is_signed<StoragePrimitive>::value,
                "FixedInt, StoragePrimitive must be signed.");
    }

    void onGetFrom(StreamAccessLayer &streamAccessLayer)
    {
        typedef typename std::make_unsigned<ValueType>::type UnsignedValueType;
        UnsignedValueType value = 0;

        for(auto bitIndex = 0; bitIndex < NumberOfBits; ++bitIndex)
        {
            if(streamAccessLayer.getBit())
            {
                value |= UnsignedValueType(1) << bitIndex;
            }
        }

        const auto valuesMaxNum = UnsignedValueType(1) << NumberOfBits;

        setValue(value < valuesMaxNum / 2 ? value : value - (valuesMaxNum + 1));
    }
};
/*----------------------------------------------------------------------------*/
/* u(n) */
template <int NumberOfBits, typename StoragePrimitive> class FixedUInt:
    public TagVLD,
    public Descriptor,
    public StoreByValue<StoragePrimitive>
{
public:
    static const auto lengthInBits = NumberOfBits;
    static const auto descriptorId = DescriptorId::FixedUnsigned;
    using StoreByValue<StoragePrimitive>::getValue;
    using StoreByValue<StoragePrimitive>::setValue;
    using typename StoreByValue<StoragePrimitive>::ValueType;
public:
    FixedUInt():
        StoreByValue<StoragePrimitive>(0)
    {
        static_assert(
                getSizeInBits<StoragePrimitive>() >= NumberOfBits,
                "FixedUInt, StoragePrimitive insufficient capacity.");

        static_assert(
                std::is_unsigned<StoragePrimitive>::value,
                "FixedUInt, StoragePrimitive must be unsigned.");
    }

    void onGetFrom(StreamAccessLayer &streamAccessLayer)
    {
        ValueType value = 0;

        for(auto bitIndex = 0; bitIndex < NumberOfBits; ++bitIndex)
        {
            if(streamAccessLayer.getBit())
            {
                const int shiftBy = NumberOfBits - 1 - bitIndex;

                value |= ValueType(1) << shiftBy;
            }
        }

        setValue(value);
    }
};
/*----------------------------------------------------------------------------*/
/* b(8) */
class Bits8:
    public TagVLD,
    public Descriptor,
    public StoreByValue<uint8_t>
{
public:
    static const auto descriptorId = DescriptorId::Byte;

    Bits8():
        StoreByValue(0)
    {}

    void onGetFrom(StreamAccessLayer &streamAccessLayer)
    {
        setValue(streamAccessLayer.getByte());
    }
};
/*----------------------------------------------------------------------------*/
/* i(v) */
class Int:
    public TagVLD,
    public Descriptor,
    public StoreByValue<int32_t>,
    public VariableBitLength
{
public:
    static const auto descriptorId = DescriptorId::Signed;

    Int():
        StoreByValue(0)
    {}

    void onGetFrom(StreamAccessLayer &streamAccessLayer)
    {
        /* (Draft 7) ISO/IEC 23008-2 : 201x, 7.2 */
        setValue( streamAccessLayer.getBits<ValueType>(getLengthInBits()));
    }
};
/*----------------------------------------------------------------------------*/
/* u(v) */
class UInt:
    public TagVLD,
    public Descriptor,
    public StoreByValue<uint32_t>,
    public VariableBitLength
{
public:
    static const auto descriptorId = DescriptorId::Unsigned;

    UInt():
    StoreByValue(0)
    {}

    UInt(int lengthInBits):
        StoreByValue(0),
        VariableBitLength(lengthInBits)
    {}

    void onGetFrom(StreamAccessLayer &streamAccessLayer)
    {
        /* (Draft 7) ISO/IEC 23008-2 : 201x, 7.2 */
        if(0 == getLengthInBits())
        {
            setValue(0);
        }
        else
        {
            setValue(streamAccessLayer.getBits<ValueType>(getLengthInBits()));
        }
    }
};
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Syntax::VLD */

#endif /* HEVC_Syntax_VLD_Descriptors_h */
