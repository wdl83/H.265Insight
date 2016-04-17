/* HEVC */
#include <Syntax/CABAD/ArithmeticDecoder.h>
#include <StreamAccessLayer.h>
/* STDC++ */
#include <sstream>
#include <iomanip>


namespace HEVC { namespace Syntax { namespace CABAD {
/*----------------------------------------------------------------------------*/
namespace {
/*----------------------------------------------------------------------------*/
struct OnDecodeBin
{
    const ArithmeticDecoder &arithmeticDecoder;

    OnDecodeBin(const ArithmeticDecoder &arithmeticDecoder_):
        arithmeticDecoder(arithmeticDecoder_)
    {}

    ~OnDecodeBin()
    {
        log(
                LogId::ArithmeticDecoderState,
                std::hex, std::setw(4), std::setfill('0'),
                arithmeticDecoder.codeIRange(),
                ' ',
                std::hex, std::setw(4), std::setfill('0'),
                arithmeticDecoder.codeIOffset(),
                '\n');
    }
};
/*----------------------------------------------------------------------------*/
} /* namespace */
/*----------------------------------------------------------------------------*/
decltype(ArithmeticDecoder::m_rangeTabLPS) ArithmeticDecoder::m_rangeTabLPS =
{
    {
        {{128, 176, 208, 240}},
        {{128, 167, 197, 227}},
        {{128, 158, 187, 216}},
        {{123, 150, 178, 205}},
        {{116, 142, 169, 195}},
        {{111, 135, 160, 185}},
        {{105, 128, 152, 175}},
        {{100, 122, 144, 166}},
        {{ 95, 116, 137, 158}},
        {{ 90, 110, 130, 150}},
        {{ 85, 104, 123, 142}},
        {{ 81,  99, 117, 135}},
        {{ 77,  94, 111, 128}},
        {{ 73,  89, 105, 122}},
        {{ 69,  85, 100, 116}},
        {{ 66,  80,  95, 110}},
        {{ 62,  76,  90, 104}},
        {{ 59,  72,  86,  99}},
        {{ 56,  69,  81,  94}},
        {{ 53,  65,  77,  89}},
        {{ 51,  62,  73,  85}},
        {{ 48,  59,  69,  80}},
        {{ 46,  56,  66,  76}},
        {{ 43,  53,  63,  72}},
        {{ 41,  50,  59,  69}},
        {{ 39,  48,  56,  65}},
        {{ 37,  45,  54,  62}},
        {{ 35,  43,  51,  59}},
        {{ 33,  41,  48,  56}},
        {{ 32,  39,  46,  53}},
        {{ 30,  37,  43,  50}},
        {{ 29,  35,  41,  48}},
        {{ 27,  33,  39,  45}},
        {{ 26,  31,  37,  43}},
        {{ 24,  30,  35,  41}},
        {{ 23,  28,  33,  39}},
        {{ 22,  27,  32,  37}},
        {{ 21,  26,  30,  35}},
        {{ 20,  24,  29,  33}},
        {{ 19,  23,  27,  31}},
        {{ 18,  22,  26,  30}},
        {{ 17,  21,  25,  28}},
        {{ 16,  20,  23,  27}},
        {{ 15,  19,  22,  25}},
        {{ 14,  18,  21,  24}},
        {{ 14,  17,  20,  23}},
        {{ 13,  16,  19,  22}},
        {{ 12,  15,  18,  21}},
        {{ 12,  14,  17,  20}},
        {{ 11,  14,  16,  19}},
        {{ 11,  13,  15,  18}},
        {{ 10,  12,  15,  17}},
        {{ 10,  12,  14,  16}},
        {{  9,  11,  13,  15}},
        {{  9,  11,  12,  14}},
        {{  8,  10,  12,  14}},
        {{  8,   9,  11,  13}},
        {{  7,   9,  11,  12}},
        {{  7,   9,  10,  12}},
        {{  7,   8,  10,  11}},
        {{  6,   8,   9,  11}},
        {{  6,   7,   9,  10}},
        {{  6,   7,   8,   9}},
        {{  2,   2,   2,   2}}
    }
};
/*----------------------------------------------------------------------------*/
decltype(ArithmeticDecoder::m_transIdxLPS) ArithmeticDecoder::m_transIdxLPS =
{
    {
         0,  0,  1,  2,  2,  4,  4,  5,  6,  7,  8,  9,  9, 11, 11, 12,
        13, 13, 15, 15, 16, 16, 18, 18, 19, 19, 21, 21, 22, 22, 23, 24,
        24, 25, 26, 26, 27, 27, 28, 29, 29, 30, 30, 30, 31, 32, 32, 33,
        33, 33, 34, 34, 35, 35, 35, 36, 36, 36, 37, 37, 37, 38, 38, 63
    }
};
/*----------------------------------------------------------------------------*/
decltype(ArithmeticDecoder::m_transIdxMPS) ArithmeticDecoder::m_transIdxMPS =
{
    {
         1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16,
        17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32,
        33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
        49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 62, 63
    }
};
/*----------------------------------------------------------------------------*/
void ArithmeticDecoder::init(StreamAccessLayer &streamAccessLayer)
{
    /* 10v34, 9.3.2.5 "Initialization process for the arithmetic decoding engine"
     * This process is invoked before decoding the first coding tree block of a slice */

    m_codIRange = maxRange;
    m_codIOffset =  streamAccessLayer.getBits<uint32_t>(9);
    syntaxCheck(510 != m_codIOffset);
    syntaxCheck(511 != m_codIOffset);
}
/*----------------------------------------------------------------------------*/
void ArithmeticDecoder::align()
{
    m_codIRange = 256;
}
/*----------------------------------------------------------------------------*/
bool ArithmeticDecoder::decodeBin(StreamAccessLayer &streamAccessLayer)
{
    /* ISO/IEC 23008-2 (Draft 8)
     * 9.3.3.2 - Arithmetic decoding process (context independent)
     * 9.3.3.2.3 - Bypass decoding process for binary decisions */
    const OnDecodeBin onDecodeBin(*this);

    m_codIOffset <<= 1;
    m_codIOffset |= streamAccessLayer.getBit();

    if(m_codIOffset >= m_codIRange)
    {
        m_codIOffset -= m_codIRange;
        syntaxCheck(m_codIOffset < m_codIRange);
        return true;
    }
    else
    {
        syntaxCheck(m_codIOffset < m_codIRange);
        return false;
    }
}
/*----------------------------------------------------------------------------*/
bool ArithmeticDecoder::decodeBin(
        StreamAccessLayer &streamAccessLayer,
        Variable &variable)
{
    /* ISO/IEC 23008-2 (Draft 8)
     * 9.3.3.2 - Arithmetic decoding process (context dependent) */
    const OnDecodeBin onDecodeBin(*this);

    if(false == variable.getValMps() && 63 == variable.getPStateIdx())
    {
        /* 10v34, 9.3.4.3.5, "Decoding process for binary decisions before termination"
         *
         * SPECIAL CASE:
         *
         * end_of_slice_segment_flag,
         * end_of_sub_stream_one_bit,
         * pcm_flag */

        m_codIRange -= 2;

        if(m_codIOffset >= m_codIRange)
        {
            return true;
        }
        else
        {
            renormalize(streamAccessLayer);
            return false;
        }
    }
    else
    {
        const auto value = decodeBin(variable);
        updateState(variable, value);
        renormalize(streamAccessLayer);
        return value;
    }
}
/*----------------------------------------------------------------------------*/
bool ArithmeticDecoder::decodeBin(
        StreamAccessLayer &streamAccessLayer,
        Variable *variable)
{
    return
        nullptr == variable
        ? decodeBin(streamAccessLayer)
        : decodeBin(streamAccessLayer, *variable);
}
/*----------------------------------------------------------------------------*/
bool ArithmeticDecoder::decodeBin(const Variable &variable)
{
    /* 10v34, 9.3.4.3.2, "Arithmetic decoding process for a binary decision"
     * 9.3.4.3.2.1, "General" */

    const auto qRangeIdx = (m_codIRange >> 6u) & 3u;
    const auto rangeLPS = m_rangeTabLPS[variable.getPStateIdx()][qRangeIdx];

    m_codIRange -= rangeLPS;

    if(m_codIOffset >= m_codIRange)
    {
        m_codIOffset -= m_codIRange;
        m_codIRange = rangeLPS;

        return !variable.getValMps();
    }
    else
    {
        return variable.getValMps();
    }
}
/*----------------------------------------------------------------------------*/
void ArithmeticDecoder::updateState(Variable &variable, const bool value) const
{
    /* 10v34, 9.3.4.3.2.2 "State transition process" */

    if(value == variable.getValMps())
    {
        variable.setPStateIdx(m_transIdxMPS[variable.getPStateIdx()]);
    }
    else
    {
        if(0 == variable.getPStateIdx())
        {
            variable.setValMps(!variable.getValMps());
        }

        variable.setPStateIdx(m_transIdxLPS[variable.getPStateIdx()]);
    }
}
/*----------------------------------------------------------------------------*/
void ArithmeticDecoder::renormalize(StreamAccessLayer &streamAccessLayer)
{
    /* 10v34, 9.3.3.3.3, "Renormalization process in the arithmetic decoding engine" */

    while(m_codIRange < 256)
    {
        m_codIRange *= 2;
        m_codIOffset <<= 1;
        m_codIOffset |= streamAccessLayer.getBit();

        syntaxCheck(m_codIOffset < m_codIRange);
    }
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Syntax::CABAD */
