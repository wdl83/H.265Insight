#ifndef HEVCDef_h
#define HEVCDef_h

/* STDC++ */
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <limits>
#include <ostream>
#include <type_traits>
/* HEVC */
#include <HEVCLimits.h>
#include <PicOrderCnt.h>
#include <EnumUtils.h>
#include <Tuple.h>
#include <utils.h>
#include <runtime_assert.h>

namespace HEVC {
/*----------------------------------------------------------------------------*/
typedef int32_t Sample;

/* 04/2013
 * 3, "Definitions"
 *
 * access unit: A set of NAL units that are associated with each other
 * according to a specified classification rule, are consecutive in
 * decoding order, and contain exactly one coded picture.
 * NOTE – In addition to containing the VCL NAL units of the coded picture,
 * an access unit may also contain non-VCL NAL units.
 * The decoding of an access unit always results in a decoded picture. */

enum class NalUnitType
{
    /* 04/2013,
     * 7.4.2.2 "NAL unit header semantics", Table 7-1 */
    Begin = 0,
    TRAIL_N = Begin,
    TRAIL_R = 1,
    TSA_N = 2,
    TSA_R = 3,
    STSA_N = 4,
    STSA_R = 5,
    RADL_N = 6,
    RADL_R = 7,
    RASL_N = 8,
    RASL_R = 9,
    RSV_VCL_N10 = 10,
    RSV_VCL_N12 = 12,
    RSV_VCL_N14 = 14,
    RSV_VCL_R11 = 11,
    RSV_VCL_R13 = 13,
    RSV_VCL_R15 = 15,
    BLA_W_LP = 16,
    BLA_W_RADL = 17,
    BLA_N_LP = 18,
    IDR_W_RADL = 19,
    IDR_N_LP = 20,
    CRA_NUT = 21,
    RSV_IRAP_VCL22 = 22,
    RSV_IRAP_VCL23 = 23,
    RSV_VCL24 = 24,
    RSV_VCL25 = 25,
    RSV_VCL26 = 26,
    RSV_VCL27 = 27,
    RSV_VCL28 = 28,
    RSV_VCL29 = 29,
    RSV_VCL30 = 30,
    RSV_VCL31 = 31,
    VPS_NUT = 32,
    SPS_NUT = 33,
    PPS_NUT = 34,
    AUD_NUT = 35,
    EOS_NUT = 36,
    EOB_NUT = 37,
    FD_NUT = 38,
    PREFIX_SEI_NUT = 39,
    SUFFIX_SEI_NUT = 40,
    RSV_NVCL41 = 41,
    RSV_NVCL42 = 42,
    RSV_NVCL43 = 43,
    RSV_NVCL44 = 44,
    RSV_NVCL45 = 45,
    RSV_NVCL46 = 46,
    RSV_NVCL47 = 47,
    UNSPEC48 = 48,
    UNSPEC49 = 49,
    UNSPEC50 = 50,
    UNSPEC51 = 51,
    UNSPEC52 = 52,
    UNSPEC53 = 53,
    UNSPEC54 = 54,
    UNSPEC55 = 55,
    UNSPEC56 = 56,
    UNSPEC57 = 57,
    UNSPEC58 = 58,
    UNSPEC59 = 59,
    UNSPEC60 = 60,
    UNSPEC61 = 61,
    UNSPEC62 = 62,
    UNSPEC63 = 63,

    End = 64,
    Undefined = End
};

const std::string &getName(NalUnitType nalUnitType);

constexpr
inline
bool isVCL(NalUnitType type)
{
    return NalUnitType::RSV_VCL31 >= type;
}

constexpr
inline
bool isNonVCL(NalUnitType type)
{
    return !isVCL(type);
}

constexpr
inline
bool isRSV(NalUnitType type)
{
    return
        type >= NalUnitType::RSV_VCL_N10 && type <= NalUnitType::RSV_VCL_R15
        || type >= NalUnitType::RSV_IRAP_VCL22 && type <= NalUnitType::RSV_VCL31
        || type >= NalUnitType::RSV_NVCL41 && type <= NalUnitType::RSV_NVCL47;
}

constexpr
inline
bool isUNSPEC(NalUnitType type)
{
    return type > NalUnitType::RSV_NVCL47;
}

constexpr
inline
bool isPartOfContext(NalUnitType type)
{
    /* TODO: at some point maybe also SEI should be added here */
    return
        NalUnitType::VPS_NUT == type
        || NalUnitType::SPS_NUT == type
        || NalUnitType::PPS_NUT == type;
}

constexpr
inline
bool isIRAP(NalUnitType type)
{
    return
        type >= NalUnitType::BLA_W_LP
        && type <= NalUnitType::RSV_IRAP_VCL23;
}

constexpr
inline
bool isIDR(NalUnitType type)
{
    return
        NalUnitType::IDR_N_LP == type
        || NalUnitType::IDR_W_RADL == type;
}

constexpr
inline
bool isRASL(NalUnitType type)
{
    return
        NalUnitType::RASL_R == type
        || NalUnitType::RASL_N == type;
}

constexpr
inline
bool isRADL(NalUnitType type)
{
    return
        NalUnitType::RADL_R == type
        || NalUnitType::RADL_N == type;
}

constexpr
inline
bool isBLA(NalUnitType type)
{
    return
        type >= NalUnitType::BLA_W_LP
        && type <= NalUnitType::BLA_N_LP;
}

constexpr
inline
bool isCRA(NalUnitType type)
{
    return NalUnitType::CRA_NUT == type;
}

constexpr
inline
bool isSubLayerNonReference(NalUnitType type)
{
    return
        NalUnitType::TRAIL_N == type
        || NalUnitType::TSA_N == type
        || NalUnitType::STSA_N == type
        || NalUnitType::RADL_N == type
        || NalUnitType::RASL_N == type
        || NalUnitType::RSV_VCL_N10 == type
        || NalUnitType::RSV_VCL_N12 == type
        || NalUnitType::RSV_VCL_N14 == type;
}

constexpr
inline
bool isSubLayerReference(NalUnitType type)
{
    return !isSubLayerNonReference(type);
}

constexpr
inline
bool isAUD(NalUnitType type)
{
    return NalUnitType::AUD_NUT == type;
}

constexpr
inline
bool isEOS(NalUnitType type)
{
    return NalUnitType::EOS_NUT == type;
}

constexpr
inline
bool isEOB(NalUnitType type)
{
    return NalUnitType::EOB_NUT == type;
}

constexpr
inline
bool isFD(NalUnitType type)
{
    return NalUnitType::FD_NUT == type;
}

constexpr
inline
bool canStartAccessUnit(NalUnitType type)
{
    return NalUnitType::AUD_NUT == type
        || NalUnitType::VPS_NUT == type
        || NalUnitType::SPS_NUT == type
        || NalUnitType::PPS_NUT == type
        || NalUnitType::PREFIX_SEI_NUT == type
        || (type >= NalUnitType::RSV_NVCL41 && type <= NalUnitType::RSV_NVCL44)
        || (type >= NalUnitType::UNSPEC48 && type <= NalUnitType::UNSPEC55);
}
/*----------------------------------------------------------------------------*/
/* 04/2013
 * 6.3.1, "Partitioning of pictures into slices, slice segments, and tiles"
 *
 * A slice is a sequence of one or more slice segments starting with
 * an independent slice segment and containing all subsequent dependent
 * slice segments (if any) that precede the next independent slice segment
 * (if any) within the same access unit.
 */

enum class SliceType
{
    B = 0,
    P,
    I
};

inline
const std::string &getName(SliceType type)
{
    static const std::string name[] =
    {
        {"B"}, {"P"}, {"I"}
    };

    return name[int(type)];
}

constexpr
inline
bool isB(SliceType type)
{
    return SliceType::B == type;
}

constexpr
inline
bool isP(SliceType type)
{
    return SliceType::P == type;
}

constexpr
inline
bool isI(SliceType type)
{
    return SliceType::I == type;
}

constexpr
inline
bool isI(NalUnitType nalUnitType)
{
    return
        isBLA(nalUnitType)
        || isIDR(nalUnitType)
        || isCRA(nalUnitType)
        || isIRAP(nalUnitType);
}
/*----------------------------------------------------------------------------*/
enum class Component
{
    Luma = 0,
    Begin = Luma,
    Chroma,
    End
};

inline
const std::string &getName(Component component)
{
    static const std::string name[] =
    {
        {"Luma"}, {"Chroma"}
    };

    return name[int(component)];
}
/*----------------------------------------------------------------------------*/
enum class Chroma
{
    Begin,
    Cb = Begin,
    Cr,
    End
};


inline
const std::string &getName(Chroma chroma)
{
    static const std::string name[] =
    {
        {"Cb"}, {"Cr"}
    };

    return name[int(chroma)];
}
/*----------------------------------------------------------------------------*/
enum class Plane
{
    Y = 0,
    Begin = Y,
    Cb,
    Cr,
    End
};

inline
Component toComponent(Plane plane)
{
    return Plane::Y == plane ? Component::Luma : Component::Chroma;
}

inline
const std::string &getName(Plane plane)
{
    static const std::string name[] =
    {
        {"Y"}, {"Cb"}, {"Cr"}
    };

    return name[int(plane)];
}

inline
Plane toPlane(Chroma chroma)
{
    return static_cast<Plane>(int(chroma) + 1);
}
/*----------------------------------------------------------------------------*/
enum class ChromaFormatIdc
{
    Begin,
    f400 = Begin, /* 0 == separate_colour_plane_flag, 0 == ChromaArrayType */
    f420, /* 0 == separate_colour_plane_flag, 1 == ChromaArrayType */
    f422, /* 0 == separate_colour_plane_flag, 2 == ChromaArrayType */
    f444, /* 0 == separate_colour_plane_flag, 3 == ChromaArrayType */
    fSCP, /* 1 == separate_colour_plane_flag, 0 == ChromaArrayType */
    End,
    Undefined = End
};

inline
const std::string &getName(ChromaFormatIdc chromaFormatIdc)
{
    static const std::string name[] =
    {
        "400", "420", "422", "444", "SCP", "undefined"
    };

    return name[int(chromaFormatIdc)];
}

inline
bool isPresent(Component component, ChromaFormatIdc chromaFormatIdc)
{
    return !(Component::Chroma == component && ChromaFormatIdc::f400 == chromaFormatIdc);
}

inline
bool isPresent(Plane plane, ChromaFormatIdc chromaFormatIdc)
{
    return !(Plane::Y != plane && ChromaFormatIdc::f400 == chromaFormatIdc);
}
/*----------------------------------------------------------------------------*/
inline
int sign(int x)
{
    return 0 < x ? 1 : ( 0 > x ? -1 : 0);
}
/*----------------------------------------------------------------------------*/
template <typename R, typename T>
int clip3(R min, R max, T v)
{
    return v < min ? min : (v > max ? max : v);
}
/*----------------------------------------------------------------------------*/
template <typename T>
inline
int clip1(int bitDepth, T v)
{
    return clip3(0, (1 << bitDepth) - 1, v);
}
/*----------------------------------------------------------------------------*/
template <typename T>
inline
T abs(T x)
{
    return x >= T(0) ? x : -x;
}
/*----------------------------------------------------------------------------*/
template <typename T>
inline
int sign(T x)
{
    return x > T(0) ? 1 : (x < T(0) ? -1 : 0);
}
/*----------------------------------------------------------------------------*/
enum class PredictionMode
{
    Intra = 0,
    Inter,
    Skip
};

inline
const std::string &getName(PredictionMode i)
{
    static const std::string name[] =
    {
        {"Intra"}, {"Inter"}, {"Skip"}
    };

    return name[int(i)];
}

inline
bool isIntra(PredictionMode mode)
{
    return PredictionMode::Intra == mode;
}

inline
bool isInter(PredictionMode mode)
{
    return PredictionMode::Inter == mode;
}

inline
bool isSkip(PredictionMode mode)
{
    return PredictionMode::Skip == mode;
}
/*----------------------------------------------------------------------------*/
/* 04/2013,
 * 7.4.5 "Scaling list data semantics", Table 7-3 */
enum class SizeId
{
    Id0 = 0, /* 4x4 */
    Begin = Id0,
    Id1, /* 8x8 */
    Id2, /* 16x16 */
    Id3, /* 32x32 */
    End
};
/*----------------------------------------------------------------------------*/
inline
const std::string &getName(SizeId i)
{
    static const std::string name[] =
    {
        {"4x4"}, {"8x8"}, {"16x16"}, {"32x32"}
    };

    return name[castToUnderlying(i)];
}
/*----------------------------------------------------------------------------*/
/* 04/2013, 7.4.5 "Scaling list data semantics", Table 7-4
 * 10/2014, 7.4.5 "Scaling list data semantics", Table 7-4 */
enum class MatrixId
{
    Begin,
    /* Intra && Y && sizeId == [0..3] */
    Id0 = Begin,
    /* Intra && Cb && sizeId == [0..3] */
    Id1,
    /* Intra && Cr && sizeId == [0..3] */
    Id2,
    /* Inter && Y && sizeId = [0..3] */
    Id3,
    /* Inter && Cb && sizeId = [0..3] */
    Id4,
    /* Inter && Cr && sizeId = [0..3] */
    Id5,
    End
};
/*----------------------------------------------------------------------------*/
/* 10/2014, 7.4.5 "Scaling list data semantics", Table 7-4 */
inline
PredictionMode toPredictionMode(MatrixId id)
{
    return
        MatrixId::Id0 == id || MatrixId::Id1 == id || MatrixId::Id2 == id
        ? PredictionMode::Intra
        : PredictionMode::Inter;
}
/*----------------------------------------------------------------------------*/
inline
MatrixId toMatrixId(Plane plane, PredictionMode predictionMode)
{
    typedef std::array<MatrixId, EnumRange<Plane>::length()> PlaneToMatrixId;

    static const std::array<PlaneToMatrixId, 2 /* Intra + Inter */> toMatrixId =
    {{
         /* Intra */
         {{MatrixId::Id0, MatrixId::Id1, MatrixId::Id2}},
         /* Inter */
         {{MatrixId::Id3, MatrixId::Id4, MatrixId::Id5}}
     }};

    return toMatrixId[int(predictionMode)][int(plane)];
}
/*----------------------------------------------------------------------------*/
inline
int toSideLength(SizeId sizeId)
{
    static const std::array<int, 4> sideLength =
    {
        {
            4, 8, 16, 32
        }
    };

    return sideLength[castToUnderlying(sizeId)];
}

inline
SizeId toSizeId(int sideLength)
{
    if(4 == sideLength)
    {
        return SizeId::Id0;
    }
    else if(8 == sideLength)
    {
        return SizeId::Id1;
    }
    else if(16 == sideLength)
    {
        return SizeId::Id2;
    }
    else //if(32 == sideLength)
    {
        return SizeId::Id3;
    }
}
/*----------------------------------------------------------------------------*/
enum class IntraPredictionMode
{
    Planar = 0, Begin = Planar,
    Dc,
    Angular2, BeginAngular = Angular2,
    Angular3, Angular4, Angular5,
    Angular6, Angular7, Angular8, Angular9,
    Angular10, Angular11, Angular12, Angular13,
    Angular14, Angular15, Angular16, Angular17,
    Angular18, Angular19, Angular20, Angular21,
    Angular22, Angular23, Angular24, Angular25,
    Angular26, Angular27, Angular28, Angular29,
    Angular30, Angular31, Angular32, Angular33,
    Angular34,
    EndAngular,
    End = EndAngular,
    Undefined = End
};

inline
int toPredAngle(IntraPredictionMode predModeIntra)
{
    typedef EnumRange<
        IntraPredictionMode,
        IntraPredictionMode::BeginAngular,
        IntraPredictionMode::EndAngular> AngularRange;

    bdryCheck(AngularRange::encloses(predModeIntra));

    /* 04/2013,
     * "Specification of intra prediction mode in range of
     * INTRA_ANGULAR2.. INTRA_ANGULAR34", Table 8-4 */
    static const std::array<int, AngularRange::length()> angle =
    {
        {
            32, 26, 21, 17, 13, 9, 5, 2, 0,
            -2, -5, -9, -13, -17, -21, -26,
            -32,
            -26, -21, -17, -13, -9, -5, -2,
            0, 2, 5, 9, 13, 17, 21, 26, 32
        }
    };

    const auto offset = int(predModeIntra) - int(IntraPredictionMode::BeginAngular);

    return angle[offset];
}

inline
int toInvAngle(IntraPredictionMode predModeIntra)
{
    typedef EnumRange<
        IntraPredictionMode,
        IntraPredictionMode::Angular11,
        IntraPredictionMode::Angular26> InvAngularRange;

    bdryCheck(InvAngularRange::encloses(predModeIntra));

    /* 04/2013,
     * "Specification of intra prediction mode in range of
     * INTRA_ANGULAR2.. INTRA_ANGULAR34", Table 8-5 */
    static const std::array<int, InvAngularRange::length()> invAngle =
    {
        {
            -4096, -1638, -910, -630, -482, -390, -315,
            -256,
            -315, -390, -482, -630, -910, -1638, -4096
        }
    };

    const auto offset = int(predModeIntra) - int(IntraPredictionMode::Angular11);

    return invAngle[offset];
}

inline
const std::string &getName(IntraPredictionMode i)
{
    static const std::string name[] =
    {
        {"Planar"},
        {"Dc"},
        {"Angular2"}, {"Angular3"}, {"Angular4"}, {"Angular5"},
        {"Angular6"}, {"Angular7"}, {"Angular8"}, {"Angular9"},
        {"Angular10"}, {"Angular11"}, {"Angular12"}, {"Angular13"},
        {"Angular14"}, {"Angular15"}, {"Angular16"}, {"Angular17"},
        {"Angular18"}, {"Angular19"}, {"Angular20"}, {"Angular21"},
        {"Angular22"}, {"Angular23"}, {"Angular24"}, {"Angular25"},
        {"Angular26"}, {"Angular27"}, {"Angular28"}, {"Angular29"},
        {"Angular30"}, {"Angular31"}, {"Angular32"}, {"Angular33"},
        {"Angular34"},
        {"undefined"}
    };

    return name[int(i)];
}

inline
std::ostream &operator<< (std::ostream &os, IntraPredictionMode i)
{
    os << getName(i);
    return os;
}
/*----------------------------------------------------------------------------*/
enum class PartitionMode
{
    /* 2Nx2N (Intra/Inter)
     * -----------
     * |         |
     * |         |
     * |         |
     * |         |
     * ----------- */
    PART_2Nx2N = 0,

    /* 2NxN (Inter-only)
     * -----------
     * |         |
     * |         |
     * -----------
     * |         |
     * |         |
     * ----------- */
    PART_2NxN,

    /* Nx2N (Inter-only)
     * -----------
     * |    |    |
     * |    |    |
     * |    |    |
     * |    |    |
     * ----------- */
    PART_Nx2N,

    /* NxN (Intra/Inter)
     * -----------
     * |    |    |
     * |    |    |
     * -----------
     * |    |    |
     * |    |    |
     * ----------- */
    PART_NxN,

    /* 2NxnU (Inter-only)
     * -----------
     * |         |
     * -----------
     * |         |
     * |         |
     * |         |
     * ----------- */
    PART_2NxnU,

    /* 2NxnD (Inter-only)
     * -----------
     * |         |
     * |         |
     * |         |
     * -----------
     * |         |
     * ----------- */
    PART_2NxnD,

    /* nLx2N (Inter-only)
     * -----------
     * |  |      |
     * |  |      |
     * |  |      |
     * |  |      |
     * ----------- */
    PART_nLx2N,

    /* nRx2N (Inter-only)
     * -----------
     * |      |  |
     * |      |  |
     * |      |  |
     * |      |  |
     * ----------- */
    PART_nRx2N
};

inline
const std::string &getName(PartitionMode mode)
{
    static const std::string name[] =
    {
        {"PART_2Nx2N"},
        {"PART_2NxN"},
        {"PART_Nx2N"},
        {"PART_NxN"},
        {"PART_2NxnU"},
        {"PART_2NxnD"},
        {"PART_nLx2N"},
        {"PART_nRx2N"}
    };

    return name[int(mode)];
}
/*----------------------------------------------------------------------------*/
enum class RefList
{
    L0, Begin = L0,
    L1,
    End
};

inline
const std::string &getName(RefList l)
{
    static const std::string name[] =
    {
        "L0", "L1"
    };

    return name[int(l)];
}

enum class Neighbour
{
    A, Begin = A,
    B,
    End
};

enum class RefPicType
{
    Unused = 0,
    ShortTerm = 1,
    LongTerm = 2
};

inline
const std::string &getName(RefPicType type)
{
    static const std::string name[] =
    {
        "unused", "short_term", "long_term"
    };

    return name[int(type)];
}

inline
bool isUnused(RefPicType type)
{
    return RefPicType::Unused == type;
}

inline
bool isShortTerm(RefPicType type)
{
    return RefPicType::ShortTerm == type;
}

inline
bool isLongTerm(RefPicType type)
{
    return RefPicType::LongTerm == type;
}
/*----------------------------------------------------------------------------*/
enum class InterPredIdc
{
    PRED_L0 = 0,
    PRED_L1,
    PRED_BI
};
/*----------------------------------------------------------------------------*/
/* SAO */
/*----------------------------------------------------------------------------*/
enum class SaoType
{
    Undefined = -1,
    NotApplied = 0,
    BandOffset,
    /* see SaoEdgeOffsetClass definition */
    EdgeOffset
};

inline
const std::string &getName(SaoType type)
{
    static const std::string name[] =
    {
        "NotApplied", "BandOffset", "EdgeOffset"
    };

    return name[castToUnderlying(type)];
}

enum class SaoEdgeOffsetClass
{
    /* C - current (read-write), A/B - adjacent (read-only) */

    /* -------------
     * | A | C | B |
     * ------------- */
    Degree0 = 0,

    /* -----
     * | A |
     * -----
     * | C |
     * -----
     * | B |
     * ----- */
    Degree90,

    /* -----
     * | A |
     * ---------
     *     | C |
     *     ---------
     *         | B |
     *         ----- */
    Degree135,

    /*         -----
     *         | A |
     *     ---------
     *     | C |
     * ---------
     * | B |
     * ----- */
    Degree45
};

inline
const std::string &getName(SaoEdgeOffsetClass type)
{
    static const std::string name[] =
    {
        "0deg", "90deg", "135deg", "45deg"
    };

    return name[int(type)];
}
/*----------------------------------------------------------------------------*/
enum class ScanIdx
{
    Diagonal = 0,
    Horizontal,
    Vertical
};

inline
const std::string &getName(ScanIdx scanIdx)
{
    static const std::string names[] =
    {
        {"Diagonal"}, {"Horizontal"}, {"Vertical"}
    };

    return names[int(scanIdx)];
}

inline
std::ostream &operator<< (std::ostream &os, ScanIdx scanIdx)
{
    os << getName(scanIdx);
    return os;
}
/*----------------------------------------------------------------------------*/
enum class Direction
{
    H, Begin = H,
    V,
    End
};

inline
const std::string &getName(Direction dir)
{
    static const std::string name[] =
    {
        "Horizontal",
        "Vertical"
    };

    return name[int(dir)];
}

enum class EdgeType
{
    V, H
};

inline
bool isV(EdgeType type)
{
    return EdgeType::V == type;
}

inline
bool isH(EdgeType type)
{
    return EdgeType::H == type;
}

inline
const std::string &getName(EdgeType type)
{
    static const std::string name[] =
    {
        "V", "H"
    };

    return name[int(type)];
}

namespace Syntax { namespace CABAD {
/*----------------------------------------------------------------------------*/

enum class RestorePoint
{
    /* dependent slice segment */
    DSS = 0, Begin = DSS,
    /* wavefront parallel processing */
    WPP,
    End
};

inline
const std::string &getName(RestorePoint point)
{
    static const std::string name[] =
    {
        {"DSS"}, {"WPP"}
    };

    return name[int(point)];
}

/*----------------------------------------------------------------------------*/
}} /* Syntax::CABAD */
/*----------------------------------------------------------------------------*/
enum class QP
{
    Begin,
    Y = Begin,
    PrimeY,
    PrimeCb,
    PrimeCr,
    End
};

inline
int qPiToQpC(int qPi, ChromaFormatIdc chromaFormatIdc)
{
    const auto is420 = ChromaFormatIdc::f420 == chromaFormatIdc;

    if(!is420)
    {
        return std::min(qPi, 51);
    }
    else if(30 > qPi)
    {
        return qPi;
    }
    else if(30 <= qPi && 44 > qPi)
    {
        static const int offset[] =
        {
            1, 1, 1, 1, 1, /* 30,31,32,33,34 */
            2, 2, /* 35,36 */
            3, 3, /* 37,38 */
            4, 4, /* 39,40 */
            5, 5, /* 41,42 */
            6 /* 43 */
        };

        return qPi - offset[qPi - 30];
    }
    else
    {
        return qPi - 6;
    }
}
/*----------------------------------------------------------------------------*/
/* 10/2014, E.3 "Vui semantics",
 * Table E.1 "Interpretation of sample aspect ratio indicatior" */
enum class AspectRatioIdc
{
    Unspecified,
    r1_1,
    r12_11,
    r10_11,
    r16_11,
    r40_33,
    r24_11,
    r20_11,
    r32_11,
    r80_33,
    r18_11,
    r15_11,
    r64_33,
    r160_99,
    r4_3,
    r3_2,
    r2_1,
    /* 17..254 Reserved */
    EXTENDED_SAR = 255
};

/* epp: Extended Precision Processing
 * hpo: High Precision Offsets */
inline
int minCoeff(bool epp, int bitDepth)
{
    return -(1 << (epp ? std::max(15, bitDepth + 6) : 15));
}

inline
int maxCoeff(bool epp, int bitDepth)
{
    return (1 << (epp ? std::max(15, bitDepth + 6) : 15)) - 1;
}

inline
int weightedPredictionOffsetBdShift(bool hpo, int bitDepth)
{
    return hpo ? 0 : bitDepth - 8;
}

inline
int weightedPredictionOffsetHalfRange(bool hpo, int bitDepth)
{
    return 1 << (hpo ? bitDepth - 1 : 7);
}
/*----------------------------------------------------------------------------*/
enum class PictureOutput
{
    Begin,
    Disabled = Begin,
    Continuous,
    Discrete,
    End
};
/*----------------------------------------------------------------------------*/
} /* HEVC */

#endif /* HEVCDef_h */
