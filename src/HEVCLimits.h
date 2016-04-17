#ifndef HEVCLimits_h
#define HEVCLimits_h

namespace HEVC { namespace Limits {
/*----------------------------------------------------------------------------*/
struct CodedSubBlock
{
    static const auto size = 4;
};

struct CodingBlock
{
    static const int min = 8;
    static const int max = 64;
    static const int log2MaxSize = 6;
};

struct TrafoSize
{
    static const int log2MinSize = 2;
    static const int min = 4;
    static const int max = 32;
};

struct RefListIdx
{
    static const int min = 0;
    static const int max = 15;
    static const int num = 16;
};

struct NumShortTermRefPicSets
{
    /* 04/2013, 7.4.3.2 "Sequence parameter set RBSP semnatics" */
    static const int min = 0;
    static const int max = 64;
    /* see Note 5 */
    static const int num = 65;
};

struct NumLongTermRefPicsSps
{
    /* 04/2013, 7.4.3.2 "Sequence parameter set RBSP semnatics" */
    static const int min = 0;
    static const int max = 32;
    static const int num = 33;
};

struct NumLongTerm
{
    // TODO
    static const auto num = 16;
};

struct MaxDpbSize
{
    static const auto value = 16;
};

struct NumPocTotalCurr
{
    static const auto min = 0;
    static const auto max = 15;
    static const auto num = max + 1;
};

struct VpsNumLayerSets
{
    static const auto min = 0;
    static const auto max = 1023;
    static const auto num = 1024;
};

struct VpsMaxSubLayers
{
    static const auto num = 7;
};

struct VpsNumHrdParameters
{
    static const auto min = 0;
    static const auto max = 1023;
    static const auto num = 1024;
};

struct SpsMaxSubLayers
{
    static const auto num = 7;
};

struct CpbCnt
{
    static const auto min = 0;
    static const auto max = 32;
    static const auto num = 33;
};

struct MaxPicWidthInMinCtb
{
    /* 04/2013, A.4.1 "General tier and level limits"
     * Table A.1 (Level 6.2)
     * sqrt(MaxLumaPs * 8) / 32 */
    static const auto value = 2048;
};

struct MaxPicHeightInMinCtb
{
    /* 04/2013, A.4.1 "General tier and level limits"
     * Table A.1 (Level 6.2)
     * sqrt(MaxLumaPs * 8) / 32 */
    static const auto value = 2048;
};

struct ChromaQpOffsetListLenght
{
    static const auto value = 6;
};
/*----------------------------------------------------------------------------*/
}} /* HEVC::Limits */

#endif /* HEVCLimits_h */
