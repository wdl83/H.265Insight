#ifndef HEVC_h
#define HEVC_h
/* STDC++ */
#include <vector>
/* HEVC */
#include <HEVCDef.h>
#include <units.h>
#include <Coord.h>
#include <Rect.h>
#include <Range.h>
#include <Handle.h>
#include <Ptr.h>
#include <Pair.h>
#include <VLA.h>
#include <VLM.h>
#include <BitArray.h>
#include <VariadicTraits.h>
#include <Optional.h>

namespace HEVC {
/*----------------------------------------------------------------------------*/
template <typename U, typename S>
inline Coord<S> toUnderlying(Coord<LinearArithmetic<U, S>> coord)
{
    return Coord<S>(toUnderlying(coord.x()), toUnderlying(coord.y()));
}

enum class PelCategory
{
    Pel,
    SubPel
};

typedef UnitTraits<PelCategory, PelCategory::Pel> PelUnit;
typedef UnitTraits<PelCategory, PelCategory::SubPel> SubPelUnit;

typedef LinearArithmetic<PelUnit, int> Pel;
typedef LinearArithmetic<SubPelUnit, int> SubPel;

typedef Coord<Pel> PelCoord;
typedef Rect<Pel> PelRect;

typedef Coord<SubPel> SubPelCoord;
typedef Rect<SubPel> SubPelRect;

constexpr Pel operator"" _pel (unsigned long long value)
{
    return Pel(Pel::ValueType(value));
}

constexpr SubPel operator"" _sub_pel (unsigned long long value)
{
    return SubPel(SubPel::ValueType(value));
}

inline
SubPel toSubPel(Pel value, Log2 ratio)
{
    return SubPel(toUnderlying(value) << toUnderlying(ratio));
}

inline
SubPelCoord toSubPel(PelCoord coord, Log2 ratio)
{
    return SubPelCoord(toSubPel(coord.x(), ratio), toSubPel(coord.y(), ratio));
}

inline
Pel toPel(SubPel value, Log2 ratio)
{
    return Pel(toUnderlying(value) >> toUnderlying(ratio));
}

inline
PelCoord toPel(SubPelCoord coord, Log2 ratio)
{
    return PelCoord(toPel(coord.x(), ratio), toPel(coord.y(), ratio));
}

inline
Pel toPel(Log2 value)
{
    return Pel(Pel::ValueType(1) << toUnderlying(value));
}

inline
Pel operator>> (Pel x, Log2 l)
{
    return Pel{toUnderlying(x) >> toUnderlying(l)};
}

inline
Pel operator<< (Pel x, Log2 l)
{
    return Pel{toUnderlying(x) << toUnderlying(l)};
}

inline
SubPel fraction(SubPel value, Log2 ratio)
{
    return SubPel{toUnderlying(value) & makeMask<SubPel::ValueType>(toUnderlying(ratio))};
}

inline
Coord<SubPel> fraction(Coord<SubPel> coord, Log2 ratio)
{
    return {fraction(coord.x(), ratio), fraction(coord.y(), ratio)};
}
/*------------------------------------------------------------------------------
 * 10v34, 3 "Definitions"
 *
 * block:
 * An MxN (M-column by N-row) array of samples,
 * or an MxN array of transform coefficients.
    if (m_rect.topLeft() == toCtu*
 * partitioning: The division of a set into subsets such that each element of
 * the set is in exactly one of the subsets.
 *
 * (CTB)
 * coding tree block: An NxN block of samples for some value of N such that
 * the division of a component into coding tree blocks is a partitioning.
 *
 * (CTU)
 * coding tree unit: A coding tree block of luma samples,
 * two corresponding coding tree blocks of chroma samples of a picture
 * that has three sample arrays, or a coding tree block of samples of
 * a monochrome picture or a picture that is coded using
 * three separate colour planes and syntax structures used to code the samples.
 *
 * (CB)
 * coding block: An NxN block of samples for some value of N such that
 * the division of a coding tree block into coding blocks is a partitioning.
 *
 * (CU)
 * coding unit: A coding block of luma samples,
 * two corresponding coding blocks of chroma samples of a picture
 * that has three sample arrays, or a coding block of samples of
 * a monochrome picture or a picture that is coded using
 * three separate colour planes and syntax structures used to code the samples.
 *
 * (PB)
 * prediction block: A rectangular MxN block of samples on which
 * the same prediction is applied
 *
 * (PU)
 * prediction unit: A prediction block of luma samples,
 * two corresponding prediction blocks of chroma samples of a
 * picture that has three sample arrays, or a prediction block of samples of
 * a monochrome picture or a picture that is coded using
 * three separate colour planes and syntax structures used to
 * predict the prediction block samples.
 *
 * (TB)
 * transform block: A rectangular MxN block of samples on whic
 * the same transform is applied.
 *
 * (TU)
 * transform unit: A transform block of luma samples of size
 * 8x8, 16x16, or 32x32 or four transform blocks of luma samples of size 4x4,
 * two corresponding transform blocks of chroma samples of
 * a picture that has three sample arrays, or a transform block of
 * luma samples of size 8x8, 16x16, or 32x32 or four transform blocks of
 * luma samples of size 4x4 of a monochrome picture or
 * a picture that is coded using three separate colour planes
 * and syntax structures used to transform the transform block samples.
 *----------------------------------------------------------------------------*/

enum class CodingBlocks
{
    Ctb,
    Cb,
    MinCb,
    /* subblock */
    SubCb,
    /* minimum transform block */
    MinTb,
};

typedef UnitTraits<CodingBlocks, CodingBlocks::Ctb> CtbUnit;
typedef UnitTraits<CodingBlocks, CodingBlocks::Cb> CbUnit;
typedef UnitTraits<CodingBlocks, CodingBlocks::MinCb> MinCbUnit;
typedef UnitTraits<CodingBlocks, CodingBlocks::SubCb> SubCbUnit;
typedef UnitTraits<CodingBlocks, CodingBlocks::MinTb> MinTbUnit;

typedef LinearArithmetic<CtbUnit, int> Ctb;
typedef LinearArithmetic<CbUnit, int> Cb;
typedef LinearArithmetic<MinCbUnit, int> MinCb;
typedef LinearArithmetic<SubCbUnit, int> SubCb;
typedef LinearArithmetic<MinTbUnit, int> MinTb;

typedef Coord<Ctb> CtbCoord;
typedef Coord<Cb> CbCoord;
typedef Coord<MinCb> MinCbCoord;
typedef Coord<SubCb> SubCbCoord;
typedef Coord<MinTb> MinTbCoord;

constexpr Ctb operator"" _ctb (unsigned long long value)
{
    return Ctb(static_cast<Ctb::ValueType>(value));
}

constexpr Cb operator"" _cb (unsigned long long value)
{
    return Cb(static_cast<Cb::ValueType>(value));
}

constexpr MinCb operator"" _min_cb (unsigned long long value)
{
    return MinCb(static_cast<MinCb::ValueType>(value));
}

constexpr SubCb operator"" _sub_cb (unsigned long long value)
{
    return SubCb(static_cast<SubCb::ValueType>(value));
}

constexpr MinTb operator"" _min_tb (unsigned long long value)
{
    return MinTb(static_cast<MinTb::ValueType>(value));
}
/*----------------------------------------------------------------------------*/
/* Ctb */
/*----------------------------------------------------------------------------*/
inline
Pel toPel(Ctb value, Log2 size)
{
    return Pel{toUnderlying(value) << toUnderlying(size)};
}

inline
Ctb toCtb(Pel value, Log2 size)
{
    return Ctb{toUnderlying(value) >> toUnderlying(size)};
}

inline
Ctb toCtb(MinCb value, Log2 size)
{
    return Ctb{toUnderlying(value) >> toUnderlying(size)};
}

inline
PelCoord toPel(CtbCoord coord, Log2 size)
{
    return PelCoord{toPel(coord.x(), size), toPel(coord.y(), size)};
}

inline
CtbCoord toCtb(PelCoord coord, Log2 size)
{
    return CtbCoord{toCtb(coord.x(), size), toCtb(coord.y(), size)};
}
/*----------------------------------------------------------------------------*/
/* MinCb */
/*----------------------------------------------------------------------------*/
inline
Pel toPel(MinCb value, Log2 size)
{
    return Pel{toUnderlying(value) << toUnderlying(size)};
}

inline
MinCb toMinCb(Pel value, Log2 size)
{
    return MinCb{toUnderlying(value) >> toUnderlying(size)};
}

inline
PelCoord toPel(MinCbCoord coord, Log2 size)
{
    return PelCoord{toPel(coord.x(), size), toPel(coord.y(), size)};
}

inline
MinCbCoord toMinCb(PelCoord coord, Log2 size)
{
    return MinCbCoord{toMinCb(coord.x(), size), toMinCb(coord.y(), size)};
}
/*----------------------------------------------------------------------------*/
/* SubCb */
/*----------------------------------------------------------------------------*/
inline
Pel toPel(SubCb value, Log2 size = 2_log2)
{
    return Pel{toUnderlying(value) << toUnderlying(size)};
}

inline
SubCb toSubCb(Pel value, Log2 size = 2_log2)
{
    return SubCb{toUnderlying(value) >> toUnderlying(size)};
}

inline
PelCoord toPel(SubCbCoord coord, Log2 size = 2_log2)
{
    return PelCoord{toPel(coord.x(), size), toPel(coord.y(), size)};
}

inline
SubCbCoord toSubCb(PelCoord coord, Log2 size = 2_log2)
{
    return SubCbCoord{toSubCb(coord.x(), size), toSubCb(coord.y(), size)};
}
/*----------------------------------------------------------------------------*/
/* MinTb */
/*----------------------------------------------------------------------------*/
inline
Pel toPel(MinTb value, Log2 size)
{
    return Pel{toUnderlying(value) << toUnderlying(size)};
}

inline
MinTb toMinTb(Pel value, Log2 size)
{
    return MinTb{toUnderlying(value) >> toUnderlying(size)};
}

inline
PelCoord toPel(MinTbCoord coord, Log2 size)
{
    return PelCoord{toPel(coord.x(), size), toPel(coord.y(), size)};
}

inline
MinTbCoord toMinTb(PelCoord coord, Log2 size)
{
    return MinTbCoord{toMinTb(coord.x(), size), toMinTb(coord.y(), size)};
}
/*----------------------------------------------------------------------------*/
enum class CodingGroups
{
    MinQpGrp
};

typedef UnitTraits<CodingGroups, CodingGroups::MinQpGrp> MinQpGrpUnit;
typedef LinearArithmetic<MinQpGrpUnit, int> MinQpGrp;
typedef Coord<MinQpGrp> MinQpGrpCoord;


constexpr
MinQpGrp operator"" _min_qpgrp (unsigned long long value)
{
    return MinQpGrp(static_cast<MinQpGrp::ValueType>(value));
}

inline
Pel toPel(MinQpGrp value, Log2 size)
{
    return Pel{toUnderlying(value) << toUnderlying(size)};
}

inline
MinQpGrp toMinQpGrp(Pel value, Log2 size)
{
    return MinQpGrp{toUnderlying(value) >> toUnderlying(size)};
}

inline
PelCoord toPel(MinQpGrpCoord coord, Log2 size)
{
    return PelCoord{toPel(coord.x(), size), toPel(coord.y(), size)};
}

inline
MinQpGrpCoord toMinQpGrp(PelCoord coord, Log2 size)
{
    return MinQpGrpCoord{toMinQpGrp(coord.x(), size), toMinQpGrp(coord.y(), size)};
}
/*----------------------------------------------------------------------------*/
template <typename U>
inline
U toAddrScanZ(Pos pos)
{
    return
        U(
                Pos::TopLeft == pos
                ? 0
                : (
                    Pos::TopRight == pos
                    ? 1
                    : (
                        Pos::BottomLeft == pos
                        ? 2
                        : /* Pos::BottomRight */ 3)));
}
/*----------------------------------------------------------------------------*/
template <typename U>
inline
U toAddrScanZ(Coord<U> at, U dim, U minDim, U base)
{
    bdryCheck(U(0) < minDim);
    bdryCheck(dim >= minDim);
    bdryCheck(dim > at.x());
    bdryCheck(dim > at.y());

    if(dim == minDim)
    {
        return base;
    }
    else
    {
        const auto dimDiv2 = dim / 2;
        const auto quarterSize = toPower(dimDiv2 / minDim, 2);
        const auto hPos = at.x() < dimDiv2 ? HPos::Left : HPos::Right;
        const auto vPos = at.y() < dimDiv2 ? VPos::Top : VPos::Bottom;
        const auto pos = hPos | vPos;
        const auto offset = quarterSize * toAddrScanZ<U>(pos);
        const auto addr = base + offset;

        return toAddrScanZ({at.x() % dimDiv2, at.y() % dimDiv2}, dimDiv2, minDim, addr);
    }
}

template <typename U>
inline
U toAddrScanZ(Coord<U> at, U dim, U minDim)
{
    return toAddrScanZ(at, dim, minDim, U(0));
}
/*----------------------------------------------------------------------------*/
template <typename U>
inline
Coord<U> toCoordScanZ(U addr, U dim, U minDim, Coord<U> base)
{
    bdryCheck(U(0) < minDim);
    bdryCheck(dim >= minDim);
    bdryCheck(toPower(dim / minDim, 2) > addr);

    if(dim == minDim)
    {
        return base;
    }
    else
    {
        const auto dimDiv2 = dim / 2;
        const auto quarterSize = toPower(dimDiv2 / minDim, 2);

        const Coord<U> offset[] =
        {
            {U(0), U(0)},
            {dimDiv2, U(0)},
            {U(0), dimDiv2},
            {dimDiv2, dimDiv2}
        };

        const int quarterNo = toUnderlying(addr / quarterSize);
        const auto at = base + offset[quarterNo];

        return toCoordScanZ(addr % quarterSize, dimDiv2, minDim, at);
    }
}

template <typename U>
inline
Coord<U> toCoordScanZ(U addr, U dim, U minDim)
{
    return toCoordScanZ(addr, dim, minDim, {U(0), U(0)});
}
/*----------------------------------------------------------------------------*/
template <typename U>
inline
U toAddrScanD(Coord<U> at, U dim)
{
    /*
     * Diagonal up-right scan pattern
     *   1   2   3   4   5 ... [m]
     * ---------------------------
     * | 0 | 2 | 5 | 9 | E | ...
     * ---------------------------
     * | 1 | 4 | 8 | D | ...
     * ---------------------------
     * | 3 | 7 | C | ...
     * ---------------------------
     * | 6 | B | ...
     * ---------------------------
     * | A | ...
     * ---------------------------
     * | F | ...
     * ---------------------------
     * | .
     * | .
     * | .
     * ---------------------------
     *
     * For any square matrix in diagonal up-right scan:
     *
     * (x, y) - position
     * i - addr (in diagonal up-right scan) of (x, y)
     * x + y + 1 == m
     *
     * N >= m : i == (m^2 + m) / 2 - (y + 1)
     * N < m  : i == -N^2  + 2Nm - (m^2 - m) / 2 - (y + 1)
     *
     * */

    const auto N = dim;
    const auto x = at.x();
    const auto y = at.y();
    const auto m = x + y + U(1);

    if(N >= m)
    {
        /* up-left + diagonal */
        return (m * m + m) / U(2) - (y + U(1));
    }
    else // N < m
    {
        /* bottom-right */
        return -(N * N) + U(2) * N * m - (m * m - m)/ U(2)  - (y + U(1));
    }
}

template <typename U>
inline
Coord<U> toCoordScanD(U addr, U dim)
{
    const auto N = dim;

    bdryCheck(N * N > addr);
    bdryCheck(U(0) <= addr);

    U offset(0);
    U x(0), y(0);

    /* for each diagonal m */
    for(U m = U(1); m < U(2) * N; ++m)
    {
        /* diagonal capacity */
        const U c = N >= m ? m : U(2) * N - m;
        /* diagonal boundary addresses */
        const U begin = offset;
        const U end = offset + c;

        if(addr >= end)
        {
            offset = end;
            continue;
        }

        for(; offset < end; ++offset)
        {
            if(addr == offset)
            {
                if(N >= m)
                {
                    /* up-left + diagonal */
                    x = offset - begin;
                    y = m - U(1) - x;
                    goto done;
                }
                else
                {
                    /* bottom */
                    y = N - U(1) - (offset - begin);
                    x = m - U(1) - y;
                    goto done;
                }
            }
        }
    }
done:
    return {x, y};
}
/*----------------------------------------------------------------------------*/
template <typename U>
inline
U toAddrScanH(Coord<U> at, U dim /* N */)
{
    /*
     * Horizontal scan pattern
     * -----------------------------------------------------------
     * | 0N+0    | 0N+1    | 0N+2    | 0N+3    | ... | 0N+N-1    |
     * -----------------------------------------------------------
     * | 1N+0    | 1N+1    | 1N+2    | 1N+3    | ... | 1N+N-1    |
     * ---------------------------   -----------------------------
     * | 2N+0    | 2N+1    | 2N+2    | 2N+3    | ... | 2N+N-1    |
     * -----------------------------------------------------------
     * | 3N+0    | 3N+1    | 3N+2    | 3N+3    | ... | 3N+N-1    |
     * -----------------------------------------------------------
     * | .
     * | .
     * | .
     * -----------------------------------------------------------
     * | N^2-N+0 | N^2-N+1 | N^2-N+2 | N^2-N+3 | ... | N^2-N+N-1 |
     * -----------------------------------------------------------
     * */
    return dim * at.y() + at.x();
}

template <typename U>
inline
Coord<U> toCoordScanH(U addr, U dim /* N */)
{
    return {addr % dim, addr / dim};
}
/*----------------------------------------------------------------------------*/
template <typename U>
inline
U toAddrScanV(Coord<U> at, U dim /* N */)
{
    /*
     * Vertical scan pattern
     * -------------------------------------------------------
     * | 0N+0   | 1N+0   | 2N+0   | 3N+0   | ... | N^2-N+N-1 |
     * -------------------------------------------------------
     * | 0N+1   | 1N+1   | 2N+1   | 3N+1   | ... | N^2-N+N-1 |
     * -------------------------------------------------------
     * | 0N+2   | 1N+2   | 2N+2   | 3N+2   | ... | N^2-N+N-1 |
     * -------------------------------------------------------
     * | 0N+3   | 1N+3   | 2N+3   | 3N+3   | ... | N^2-N+N-1 |
     * -------------------------------------------------------
     * | .
     * | .
     * | .
     * -------------------------------------------------------
     * | 0N+N-1 | 1N+N-1 | 2N+N-1 | 3N+N-1 | ... | N^2-N+N-1 |
     * -------------------------------------------------------
     * */
    return dim * at.x() + at.y();
}

template <typename U>
inline
Coord<U> toCoordScanV(U addr, U dim /* N */)
{
    return {addr / dim, addr % dim};
}
/*----------------------------------------------------------------------------*/
template <typename U>
inline
U toAddr(ScanIdx scanIdx, Coord<U> at, U dim)
{
    if(ScanIdx::Horizontal == scanIdx)
    {
        return toAddrScanH(at, dim);
    }
    else if(ScanIdx::Vertical == scanIdx)
    {
        return toAddrScanV(at, dim);
    }
    else // if(ScanIdx::Diagonal == scanIdx)
    {
        return toAddrScanD(at, dim);
    }
}

template <typename U>
inline
Coord<U> toCoord(ScanIdx scanIdx, U addr, U dim)
{
    if(ScanIdx::Horizontal == scanIdx)
    {
        return toCoordScanH(addr, dim);
    }
    else if(ScanIdx::Vertical == scanIdx)
    {
        return toCoordScanV(addr, dim);
    }
    else // if(ScanIdx::Diagonal == scanIdx)
    {
        return toCoordScanD(addr, dim);
    }
}
/*----------------------------------------------------------------------------*/
inline
PelCoord toPelCoord(ScanIdx scanIdx, Log2 size, Pel addr)
{
    return toCoord(scanIdx, addr, toPel(size));
}

inline
PelCoord toPelCoord(ScanIdx scanIdx, Log2 size, int index)
{
    return toPelCoord(scanIdx, size, Pel(index));
}
/*----------------------------------------------------------------------------*/
inline
Pel hScale(Pel value, Component component, ChromaFormatIdc chromaFormatIdc)
{
    return
       Component::Luma == component
        ? value
        : (
                ChromaFormatIdc::f400 == chromaFormatIdc
                ? 0_pel
                : (
                    ChromaFormatIdc::f420 == chromaFormatIdc
                    || ChromaFormatIdc::f422 == chromaFormatIdc
                    ? value >> 1
                    : value /* f444 */));
}

inline
Pel hScale(Pel value, Plane plane, ChromaFormatIdc chromaFormatIdc)
{
    return hScale(value, toComponent(plane), chromaFormatIdc);
}

inline
Pel hInvScale(Pel value, Component component, ChromaFormatIdc chromaFormatIdc)
{
    return
        Component::Luma == component
        ? value
        : (
                ChromaFormatIdc::f400 == chromaFormatIdc
                ? 0_pel
                : (
                    ChromaFormatIdc::f420 == chromaFormatIdc
                    || ChromaFormatIdc::f422 == chromaFormatIdc
                    ? value << 1
                    : value /* f444 */));
}

inline
Pel hInvScale(Pel value, Plane plane, ChromaFormatIdc chromaFormatIdc)
{
    return hInvScale(value, toComponent(plane), chromaFormatIdc);
}
/*----------------------------------------------------------------------------*/
inline
Pel vScale(Pel value, Component component, ChromaFormatIdc chromaFormatIdc)
{
    return
        Component::Luma == component
        ? value
        : (
                ChromaFormatIdc::f400 == chromaFormatIdc
                ? 0_pel
                : (
                    ChromaFormatIdc::f420 == chromaFormatIdc
                    ? value >> 1
                    : value /* f422/f444 */));
}

inline
Pel vScale(Pel value, Plane plane, ChromaFormatIdc chromaFormatIdc)
{
    return vScale(value, toComponent(plane), chromaFormatIdc);
}

inline
Pel vInvScale(Pel value, Component component, ChromaFormatIdc chromaFormatIdc)
{
    return
        Component::Luma == component
        ? value
        : (
                ChromaFormatIdc::f400 == chromaFormatIdc
                ? 0_pel
                : (
                    ChromaFormatIdc::f420 == chromaFormatIdc
                    ? value << 1
                    : value /* f422/f444 */));
}

inline
Pel vInvScale(Pel value, Plane plane, ChromaFormatIdc chromaFormatIdc)
{
    return vInvScale(value, toComponent(plane), chromaFormatIdc);
}
/*----------------------------------------------------------------------------*/
/* Log2 support */
inline
Log2 hScale(Log2 value, Component component, ChromaFormatIdc chromaFormatIdc)
{
    return
        Component::Luma == component
        ? value
        : (
                ChromaFormatIdc::f400 == chromaFormatIdc
                ? 0_log2
                : (
                    ChromaFormatIdc::f420 == chromaFormatIdc
                    || ChromaFormatIdc::f422 == chromaFormatIdc
                    ? value - 1_log2
                    : value /* f444 */));
}

inline
Log2 hScale(Log2 value, Plane plane, ChromaFormatIdc chromaFormatIdc)
{
    return hScale(value, toComponent(plane), chromaFormatIdc);
}


inline
Log2 hInvScale(Log2 value, Component component, ChromaFormatIdc chromaFormatIdc)
{
    return
        Component::Luma == component
        ? value
        : (
                ChromaFormatIdc::f400 == chromaFormatIdc
                ? 0_log2
                : (
                    ChromaFormatIdc::f420 == chromaFormatIdc
                    || ChromaFormatIdc::f422 == chromaFormatIdc
                    ? value + 1_log2
                    : value /* f444 */));
}

inline
Log2 hInvScale(Log2 value, Plane plane, ChromaFormatIdc chromaFormatIdc)
{
    return hInvScale(value, toComponent(plane), chromaFormatIdc);
}
/*----------------------------------------------------------------------------*/
inline
Log2 vScale(Log2 value, Component component, ChromaFormatIdc chromaFormatIdc)
{
    return
        Component::Luma == component
        ? value
        : (
                ChromaFormatIdc::f400 == chromaFormatIdc
                ? 0_log2
                : (
                    ChromaFormatIdc::f420 == chromaFormatIdc
                    ? value - 1_log2
                    : value /* f422/f444 */));
}

inline
Log2 vScale(Log2 value, Plane plane, ChromaFormatIdc chromaFormatIdc)
{
    return vScale(value, toComponent(plane), chromaFormatIdc);
}

inline
Log2 vInvScale(Log2 value, Component component, ChromaFormatIdc chromaFormatIdc)
{
    return
        Component::Luma == component
        ? value
        : (
                ChromaFormatIdc::f400 == chromaFormatIdc
                ? 0_log2
                : (
                    ChromaFormatIdc::f420 == chromaFormatIdc
                    ? value + 1_log2
                    : value /* f422/f444 */));
}

inline
Log2 vInvScale(Log2 value, Plane plane, ChromaFormatIdc chromaFormatIdc)
{
    return vInvScale(value, toComponent(plane), chromaFormatIdc);
}
/*----------------------------------------------------------------------------*/
template <typename U>
inline
Coord<U> scale(Coord<U> coord, Component component, ChromaFormatIdc chromaFormatIdc)
{
    return
    {
        hScale(coord.x(), component, chromaFormatIdc),
        vScale(coord.y(), component, chromaFormatIdc)
    };
}

template <typename U>
inline
Coord<U> scale(Coord<U> coord, Plane plane, ChromaFormatIdc chromaFormatIdc)
{
    return
    {
        hScale(coord.x(), plane, chromaFormatIdc),
        vScale(coord.y(), plane, chromaFormatIdc)
    };
}

template <typename U>
inline
Coord<U> invScale(Coord<U> coord, Component component, ChromaFormatIdc chromaFormatIdc)
{
    return
    {
        hInvScale(coord.x(), component, chromaFormatIdc),
        vInvScale(coord.y(), component, chromaFormatIdc)
    };
}

template <typename U>
inline
Coord<U> invScale(Coord<U> coord, Plane plane, ChromaFormatIdc chromaFormatIdc)
{
    return
    {
        hInvScale(coord.x(), plane, chromaFormatIdc),
        vInvScale(coord.y(), plane, chromaFormatIdc)
    };
}

template <typename U>
inline
Rect<U> scale(const Rect<U> &bdry, Component component, ChromaFormatIdc chromaFormatIdc)
{
    return
    {
        scale(bdry.topLeft(), component, chromaFormatIdc),
        hScale(bdry.getWidth(), component, chromaFormatIdc),
        vScale(bdry.getHeight(), component, chromaFormatIdc)
    };
}

template <typename U>
inline
Rect<U> scale(const Rect<U> &bdry, Plane plane, ChromaFormatIdc chromaFormatIdc)
{
    return
    {
        scale(bdry.topLeft(), plane, chromaFormatIdc),
        hScale(bdry.getWidth(), plane, chromaFormatIdc),
        vScale(bdry.getHeight(), plane, chromaFormatIdc)
    };
}

template <typename U>
inline
Rect<U> invScale(const Rect<U> &bdry, Component component, ChromaFormatIdc chromaFormatIdc)
{
    return
    {
        invScale(bdry.topLeft(), component, chromaFormatIdc),
        hInvScale(bdry.getWidth(), component, chromaFormatIdc),
        vInvScale(bdry.getHeight(), component, chromaFormatIdc)
    };
}

template <typename U>
inline
Rect<U> invScale(const Rect<U> &bdry, Plane plane, ChromaFormatIdc chromaFormatIdc)
{
    return
    {
        invScale(bdry.topLeft(), plane, chromaFormatIdc),
        hInvScale(bdry.getWidth(), plane, chromaFormatIdc),
        vInvScale(bdry.getHeight(), plane, chromaFormatIdc)
    };
}
/*----------------------------------------------------------------------------*/
template <typename PatternType, int patternLength, typename Iterator>
bool isMatchingAt(
        Iterator begin, const Iterator end,
        const PatternType (&pattern)[patternLength])
{
    for(
            int index = 0;
            begin != end && index < patternLength;
            ++index, ++begin)
    {
        if(pattern[index] == *begin)
        {
            if(index + 1 == patternLength)
            {
                return true;
            }
        }
        else
        {
            break;
        }
    }
    return false;
}
/*----------------------------------------------------------------------------*/
/* Definitions for INTER decoding */
/*----------------------------------------------------------------------------*/
typedef SubPelCoord MotionVector;

struct MvScaleCoeff
{
    int td;
    int tb;
    int tx;
    int distScaleFactor;

    MvScaleCoeff(PicOrderCntVal diffTd, PicOrderCntVal diffTb):
        // (8-183)
        td{clip3(-128, 127, diffTd.value)},
        // (8-184)
        tb{clip3(-128, 127, diffTb.value)},
        // (8-180)
        tx{(16384 + (abs(td) >> 1)) / td},
        // (8-166)
        distScaleFactor{clip3(-4096, 4095, (tb * tx + 32) >> 6)}
    {}

    MotionVector scale(MotionVector mv) const
    {
        // (8-167)
        auto calc =
            [this](MotionVector::ValueType i)
            {
                const auto signValue = sign(distScaleFactor * toUnderlying(i));
                const auto absValue = (abs(distScaleFactor * toUnderlying(i)) + 127) >> 8;
                const auto clipped = clip3(-32768, 32767, signValue * absValue);

                return SubPel(clipped);
            };

        return {calc(mv.x()), calc(mv.y())};
    }

    void toStr(std::ostream &os) const
    {
        os
            << "td " << td
            << " tb " << tb
            << " tx " << tx
            << " dsf " << distScaleFactor;
    }
};
/*----------------------------------------------------------------------------*/
template <typename U>
struct Addr
{
    U inTs;
    U inRs;

    explicit Addr(U addrInTs, U addrInRs): inTs{addrInTs}, inRs{addrInRs}
    {}

    void toStr(std::ostream &os) const
    {
        os << '[' << inTs << ' ' << inRs << ']';
    }
};

typedef Addr<Ctb> CtbAddr;
/*----------------------------------------------------------------------------*/
enum class PelLayerId
{
    Begin,
    /* Intra or final Inter */
    Prediction = Begin,
    Residual,
    /* before in-loop filters */
    Reconstructed,
    /* after Deblocking filter but before SAO filter */
    Deblocked,
    /* final (after SAO filter, if not present Decoded == Deblocked) */
    Decoded,
    End
};

inline
const std::string &getName(PelLayerId id)
{
    static const std::string name[] =
    {
        "predicted",
        "residuals",
        "reconstructed",
        "deblocked",
        "decoded"
    };

    return name[int(id)];
}
/*----------------------------------------------------------------------------*/
class CuIntraPredMode
{
public:
    typedef std::array<IntraPredictionMode, 4> List;
private:
    List m_mode;
public:
    CuIntraPredMode(IntraPredictionMode mode)
    {
        std::fill(std::begin(m_mode), std::end(m_mode), mode);
    }

    CuIntraPredMode(): CuIntraPredMode(IntraPredictionMode::Undefined)
    {}

    CuIntraPredMode(const List &mode): m_mode(mode)
    {}

    IntraPredictionMode operator[] (int i) const
    {
        bdryCheck(int(m_mode.size()) > i);
        return m_mode[i];
    }

    IntraPredictionMode &operator[] (int i)
    {
        bdryCheck(int(m_mode.size()) > i);
        return m_mode[i];
    }
};
/*----------------------------------------------------------------------------*/
template <typename T>
inline
std::ostream &pelFmt(std::ostream &os, T v)
{
    os << std::hex << std::setw(4) << std::setfill('0') << v;
    return os;
}
/*----------------------------------------------------------------------------*/
} /* HEVC */

#endif /* HEVC_h */
