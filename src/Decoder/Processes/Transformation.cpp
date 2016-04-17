#include <Decoder/Processes/Transformation.h>
#include <Decoder/State.h>
#include <Decoder/Processes/TransformCoeffsScaling.h>
#include <Syntax/SequenceParameterSet.h>
#include <Syntax/SpsRangeExtension.h>
#include <Syntax/CodingUnit.h>
#include <Syntax/ResidualCoding.h>
#include <Structure/Picture.h>
/* STDC++ */
#include <array>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
namespace {
/*----------------------------------------------------------------------------*/
typedef int8_t Coeff;

template <size_t n>
using CoeffMatrix = std::array<std::array<Coeff, n>, n>;

typedef CoeffMatrix<2> Matrix2x2;
typedef CoeffMatrix<4> Matrix4x4;
typedef CoeffMatrix<8> Matrix8x8;
typedef CoeffMatrix<16> Matrix16x16;
typedef CoeffMatrix<32> Matrix32x32;

template <typename T, size_t n>
using Array = std::array<T, n>;

template <size_t n>
using CoeffArray = Array<int, n>;
/*----------------------------------------------------------------------------*/
void iDST(CoeffArray<4> &coeff)
{
    /* 04/2013, 8.6.4.2 "Transformation process", (8-277)
     * a  b  c  d
     * c  c  0 -c
     * d -a -c  b
     * b -d  c -a
     * (transposed)
     * a  c  d  b
     * b  c -a -d
     * c  0 -c  c
     * d -c  b -a */
#if 0
    static const Matrix4x4 m
    {{
        {{29,  74,  84,  55}},
        {{55,  74, -29, -84}},
        {{74,   0, -74,  74}},
        {{84, -74,  55, -29}}
    }};
#endif

    const Coeff a = 29;
    const Coeff b = 55;
    const Coeff c = 74;
    const Coeff d = 84;

    const auto t1 = c * coeff[1];
    const auto r0 = a * coeff[0] + t1 + d * coeff[2] + b * coeff[3];
    const auto r1 = b * coeff[0] + t1 - a * coeff[2] - d * coeff[3];
    const auto r2 = c * (coeff[0] - coeff[2] + coeff[3]);
    const auto r3 = d * coeff[0] - t1 + b * coeff[2] - a * coeff[3];

    coeff[0] = r0;
    coeff[1] = r1;
    coeff[2] = r2;
    coeff[3] = r3;
}
/*----------------------------------------------------------------------------*/
/* generic iDCT O = N^2 */
template <size_t n>
inline
void iDCT(const CoeffMatrix<n> &m, CoeffArray<n> &c)
{
    CoeffArray<n> r;

    for(auto y = 0; y < int(n); ++y)
    {
        auto sum = 0;

        for(auto x = 0; x < int(n); ++x)
        {
            sum += m[y][x] * c[x];
        }

        r[y] = sum;
    }

    for(auto y = 0; y < int(n); ++y)
    {
        c[y] = r[y];
    }
}
/*----------------------------------------------------------------------------*/
/* optimized 4x4 iDCT which uses odd-even decomposition */
inline
void iDCT(CoeffArray<4> &coeff)
{
    /* 04/2013, 8.6.4.2 "Transformation process", (8-279, 8-280, 8-281, 8-282)
     *  a  b  a  c
     *  a  c -a -b
     *  a -c -a  b
     *  a -b  a -c
     * y even x even
     *  a  a
     *  a -a
     * y odd x even
     *  a -a
     *  a  a
     * y odd x odd
     *  c -b
     * -b -c
     * y even x odd
     *  b  c
     * -c  b */
#if 0
    static const Matrix2x2 even =
    {{
          {{64,  64}},
          {{64, -64}}
    }};

    static const Matrix2x2 odd =
    {{
          {{ 36, -83}},
          {{-83, -36}}
    }};
#endif

    const Coeff a = 64;
    const Coeff b = 83;
    const Coeff c = 36;

    const auto r0 = a * coeff[0] + a * coeff[2];
    const auto r2 = a * coeff[0] - a * coeff[2];
    const auto r1 = c * coeff[1] - b * coeff[3];
    const auto r3 = -b * coeff[1] - c * coeff[3];

    coeff[0] = r0 - r3;
    coeff[1] = r2 + r1;
    coeff[2] = r2 - r1;
    coeff[3] = r0 + r3;
}
/*----------------------------------------------------------------------------*/
inline
void iDCT(CoeffArray<8> &c)
{
    const auto n = 8;
    /* 04/2013, 8.6.4.2 "Transformation process", (8-279, 8-280, 8-281, 8-282)
     *  a  d  b  e  a  f  c  g
     *  a  e  c -g -a -d -b -f
     *  a  f -c -d -a  g  b  e
     *  a  g -b -f  a  e -c -d
     *  a -g -b  f  a -e -c  d
     *  a -f -c  d -a -g  b -e
     *  a -e  c  g -a  d -b  f
     *  a -d  b -e  a -f  c -g
     * y even x even
     *  a  b  a  c
     *  a -c -a  b
     *  a -b  a -c
     *  a  c -a -b
     * y odd x even
     *  a  c -a -b
     *  a -b  a -c
     *  a -c -a  b
     *  a  b  a  c
     * y odd x odd
     *  e -g -d -f
     *  g -f  e -d
     * -f  d -g -e
     * -d -e -f -g
     * y even x odd
     *  d  e  f  g
     *  f -d  g  e
     * -g  f -e  d
     * -e  g  d  f */

    CoeffArray<4> cEven{{c[0], c[2], c[4], c[6]}};
    CoeffArray<4> cOdd{{c[1], c[3], c[5], c[7]}};
#if 0
    static const CoeffMatrix<4> even
    {{
         {{64,  83,  64,  36}},
         {{64, -36, -64,  83}},
         {{64, -83,  64, -36}},
         {{64,  36, -64, -83}}
    }};
#endif
    static const CoeffMatrix<4> odd
    {{
        {{75, -18, -89, -50}},
        {{18, -50,  75, -89}},
        {{-50,  89, -18, -75}},
        {{-89, -75, -50, -18}}
    }};

    //iDCT(even, cEven);
    iDCT(cEven);
    iDCT(odd, cOdd);

    std::swap(cEven[1], cEven[2]);
    std::swap(cEven[2], cEven[3]);

    for(auto i = 0; i < n; ++i)
    {
        c[i] =
            0 == i % 2
            ? cEven[i >> 1] - cOdd[(n - 1 - i) >> 1]
            : cEven[(n - 1 - i) >> 1] + cOdd[i >> 1];
    }
}
/*----------------------------------------------------------------------------*/
inline
void iDCT(CoeffArray<16> &c)
{
    const auto n = 16;
    /* 04/2013, 8.6.4.2 "Transformation process", (8-279, 8-280, 8-281, 8-282)
     *  a  h  d  i  b  j  e  k  a  l  f  m  c  n  g  o
     *  a  i  e  l  c  o -g -m -a -j -d -h -b -k -f -n
     *  a  j  f  o -c -k -d -i -a -n  g  l  b  h  e  m
     *  a  k  g -m -b -i -f  o  a  h  e  n -c -j -d -l
     *  a  l -g -j -b -n  f  h  a -o -e -i -c  m  d  k
     *  a  m -f -h -c  l  d  n -a -i -g  k  b  o -e -j
     *  a  n -e -k  c  h  g -j -a  m  d  o -b -l  f  i
     *  a  o -d -n  b  m -e -l  a  k -f -j  c  i -g -h
     *  a -o -d  n  b -m -e  l  a -k -f  j  c -i -g  h
     *  a -n -e  k  c -h  g  j -a -m  d -o -b  l  f -i
     *  a -m -f  h -c -l  d -n -a  i -g -k  b -o -e  j
     *  a -l -g  j -b  n  f -h  a  o -e  i -c -m  d -k
     *  a -k  g  m -b  i -f -o  a -h  e -n -c  j -d  l
     *  a -j  f -o -c  k -d  i -a  n  g -l  b -h  e -m
     *  a -i  e -l  c -o -g  m -a  j -d  h -b  k -f  n
     *  a -h  d -i  b -j  e -k  a -l  f -m  c -n  g -o
     *  y even x even
     *  a  d  b  e  a  f  c  g
     *  a  f -c -d -a  g  b  e
     *  a -g -b  f  a -e -c  d
     *  a -e  c  g -a  d -b  f
     *  a -d  b -e  a -f  c -g
     *  a -f -c  d -a -g  b -e
     *  a  g -b -f  a  e -c -d
     *  a  e  c -g -a -d -b -f
     * y odd x even
     *  a  e  c -g -a -d -b -f
     *  a  g -b -f  a  e -c -d
     *  a -f -c  d -a -g  b -e
     *  a -d  b -e  a -f  c -g
     *  a -e  c  g -a  d -b  f
     *  a -g -b  f  a -e -c  d
     *  a  f -c -d -a  g  b  e
     *  a  d  b  e  a  f  c  g
     * y odd x odd
     *  i  l  o -m -j -h -k -n
     *  k -m -i  o  h  n -j -l
     *  m -h  l  n -i  k  o -j
     *  o -n  m -l  k -j  i -h
     * -n  k -h  j -m -o  l -i
     * -l  j  n -h  o  i -m -k
     * -j -o  k  i  n -l -h -m
     * -h -i -j -k -l -m -n -o
     * y even x odd
     *  h  i  j  k  l  m  n  o
     *  j  o -k -i -n  l  h  m
     *  l -j -n  h -o -i  m  k
     *  n -k  h -j  m  o -l  i
     * -o  n -m  l -k  j -i  h
     * -m  h -l -n  i -k -o  j
     * -k  m  i -o -h -n  j  l
     * -i -l -o  m  j  h  k  n */

    CoeffArray<8> cEven{{c[0], c[2], c[4], c[6], c[8], c[10], c[12], c[14]}};
    CoeffArray<8> cOdd{{c[1], c[3], c[5], c[7], c[9], c[11], c[13], c[15]}};
#if 0
    static const CoeffMatrix<8> even
    {{
        {{64,  89,  83,  75,  64,  50,  36,  18}},
        {{64,  50, -36, -89, -64,  18,  83,  75}},
        {{64, -18, -83,  50,  64, -75, -36,  89}},
        {{64, -75,  36,  18, -64,  89, -83,  50}},
        {{64, -89,  83, -75,  64, -50,  36, -18}},
        {{64, -50, -36,  89, -64, -18,  83, -75}},
        {{64,  18, -83, -50,  64,  75, -36, -89}},
        {{64,  75,  36, -18, -64, -89, -83, -50}}
    }};
#endif
    static const CoeffMatrix<8> odd
    {{
         {{ 87,  57,   9, -43, -80, -90, -70, -25}},
         {{ 70, -43, -87,   9,  90,  25, -80, -57}},
         {{ 43, -90,  57,  25, -87,  70,   9, -80}},
         {{  9, -25,  43, -57,  70, -80,  87, -90}},
         {{-25,  70, -90,  80, -43,  -9,  57, -87}},
         {{-57,  80,  25, -90,   9,  87, -43, -70}},
         {{-80,  -9,  70,  87,  25, -57, -90, -43}},
         {{-90, -87, -80, -70, -57, -43, -25,  -9}}
    }};

    //iDCT(even, cEven);
    iDCT(cEven);
    iDCT(odd, cOdd);

    std::swap(cEven[1], cEven[7]);
    std::swap(cEven[3], cEven[6]);
    std::swap(cEven[1], cEven[4]);
    std::swap(cEven[1], cEven[2]);

    for(auto i = 0; i < n; ++i)
    {
        c[i] =
            0 == i % 2
            ? cEven[i >> 1] - cOdd[(n - 1 - i) >> 1]
            : cEven[(n - 1 - i) >> 1] + cOdd[i >> 1];
    }
}
/*----------------------------------------------------------------------------*/
inline
void iDCT(CoeffArray<32> &c)
{
    const auto n = 32;
    /* 04/2013, 8.6.4.2 "Transformation process", (8-279, 8-280, 8-281, 8-282)
     *  a  p  h  q  d  r  i  s  b  t  j  u  e  v  k  w  a  x  l  y  f  z  m  A  c  B  n  C  g  D  o  E
     *  a  q  i  t  e  w  l  z  c  C  o -E -g -B -m -y -a -v -j -s -d -p -h -r -b -u -k -x -f -A -n -D
     *  a  r  j  w  f  B  o -D -c -y -k -t -d -p -i -u -a -z -n -E  g  A  l  v  b  q  h  s  e  x  m  C
     *  a  s  k  z  g -D -m -w -b -p -i -v -f -C  o  A  a  t  h  r  e  y  n -E -c -x -j -q -d -u -l -B
     *  a  t  l  C -g -y -j -p -b -x -n  D  f  u  h  s  a  B -o -z -e -q -i -w -c  E  m  v  d  r  k  A
     *  a  u  m -E -f -t -h -v -c  D  l  s  d  w  n -C -a -r -i -x -g  B  k  q  b  y  o -A -e -p -j -z
     *  a  v  n -B -e -p -k -C  c  u  h  w  g -A -j -q -a -D  m  t  d  x  o -z -b -r -l -E  f  s  i  y
     *  a  w  o -y -d -u -n  A  b  s  m -C -e -q -l  E  a  p  k  D -f -r -j -B  c  t  i  z -g -v -h -x
     *  a  x -o -v -d -z  n  t  b  B -m -r -e -D  l  p  a -E -k -q -f  C  j  s  c -A -i -u -g  y  h  w
     *  a  y -n -s -e -E  k  r  c -z -h -x  g  t  j  D -a -q -m  A  d  w -o -u -b -C  l  p  f -B -i -v
     *  a  z -m -p -f  A  h  y -c -q -l  B  d  x -n -r -a  C  i  w -g -s -k  D  b  v -o -t -e  E  j  u
     *  a  A -l -r -g  v  j -E -b -w  n  q  f -z -h -B  a  s  o -u -e  D  i  x -c -p -m  y  d  C -k -t
     *  a  B -k -u  g  q  m -x -b  E  i  y -f -r -o  t  a -A -h -C  e  v -n -p -c  w  j -D -d -z  l  s
     *  a  C -j -x  f  s -o -q -c  v  k -A -d -E  i  z -a -u  n  p  g -t -l  y  b -D -h -B  e  w -m -r
     *  a  D -i -A  e  x -l -u  c  r -o -p -g  s  m -v -a  y  j -B -d  E  h  C -b -z  k  w -f -t  n  q
     *  a  E -h -D  d  C -i -B  b  A -j -z  e  y -k -x  a  w -l -v  f  u -m -t  c  s -n -r  g  q -o -p
     *  a -E -h  D  d -C -i  B  b -A -j  z  e -y -k  x  a -w -l  v  f -u -m  t  c -s -n  r  g -q -o  p
     *  a -D -i  A  e -x -l  u  c -r -o  p -g -s  m  v -a -y  j  B -d -E  h -C -b  z  k -w -f  t  n -q
     *  a -C -j  x  f -s -o  q -c -v  k  A -d  E  i -z -a  u  n -p  g  t -l -y  b  D -h  B  e -w -m  r
     *  a -B -k  u  g -q  m  x -b -E  i -y -f  r -o -t  a  A -h  C  e -v -n  p -c -w  j  D -d  z  l -s
     *  a -A -l  r -g -v  j  E -b  w  n -q  f  z -h  B  a -s  o  u -e -D  i -x -c  p -m -y  d -C -k  t
     *  a -z -m  p -f -A  h -y -c  q -l -B  d -x -n  r -a -C  i -w -g  s -k -D  b -v -o  t -e -E  j -u
     *  a -y -n  s -e  E  k -r  c  z -h  x  g -t  j -D -a  q -m -A  d -w -o  u -b  C  l -p  f  B -i  v
     *  a -x -o  v -d  z  n -t  b -B -m  r -e  D  l -p  a  E -k  q -f -C  j -s  c  A -i  u -g -y  h -w
     *  a -w  o  y -d  u -n -A  b -s  m  C -e  q -l -E  a -p  k -D -f  r -j  B  c -t  i -z -g  v -h  x
     *  a -v  n  B -e  p -k  C  c -u  h -w  g  A -j  q -a  D  m -t  d -x  o  z -b  r -l  E  f -s  i -y
     *  a -u  m  E -f  t -h  v -c -D  l -s  d -w  n  C -a  r -i  x -g -B  k -q  b -y  o  A -e  p -j  z
     *  a -t  l -C -g  y -j  p -b  x -n -D  f -u  h -s  a -B -o  z -e  q -i  w -c -E  m -v  d -r  k -A
     *  a -s  k -z  g  D -m  w -b  p -i  v -f  C  o -A  a -t  h -r  e -y  n  E -c  x -j  q -d  u -l  B
     *  a -r  j -w  f -B  o  D -c  y -k  t -d  p -i  u -a  z -n  E  g -A  l -v  b -q  h -s  e -x  m -C
     *  a -q  i -t  e -w  l -z  c -C  o  E -g  B -m  y -a  v -j  s -d  p -h  r -b  u -k  x -f  A -n  D
     *  a -p  h -q  d -r  i -s  b -t  j -u  e -v  k -w  a -x  l -y  f -z  m -A  c -B  n -C  g -D  o -E
     * y even x even
     *  a  h  d  i  b  j  e  k  a  l  f  m  c  n  g  o
     *  a  j  f  o -c -k -d -i -a -n  g  l  b  h  e  m
     *  a  l -g -j -b -n  f  h  a -o -e -i -c  m  d  k
     *  a  n -e -k  c  h  g -j -a  m  d  o -b -l  f  i
     *  a -o -d  n  b -m -e  l  a -k -f  j  c -i -g  h
     *  a -m -f  h -c -l  d -n -a  i -g -k  b -o -e  j
     *  a -k  g  m -b  i -f -o  a -h  e -n -c  j -d  l
     *  a -i  e -l  c -o -g  m -a  j -d  h -b  k -f  n
     *  a -h  d -i  b -j  e -k  a -l  f -m  c -n  g -o
     *  a -j  f -o -c  k -d  i -a  n  g -l  b -h  e -m
     *  a -l -g  j -b  n  f -h  a  o -e  i -c -m  d -k
     *  a -n -e  k  c -h  g  j -a -m  d -o -b  l  f -i
     *  a  o -d -n  b  m -e -l  a  k -f -j  c  i -g -h
     *  a  m -f -h -c  l  d  n -a -i -g  k  b  o -e -j
     *  a  k  g -m -b -i -f  o  a  h  e  n -c -j -d -l
     *  a  i  e  l  c  o -g -m -a -j -d -h -b -k -f -n
     * y odd x even
     *  a  i  e  l  c  o -g -m -a -j -d -h -b -k -f -n
     *  a  k  g -m -b -i -f  o  a  h  e  n -c -j -d -l
     *  a  m -f -h -c  l  d  n -a -i -g  k  b  o -e -j
     *  a  o -d -n  b  m -e -l  a  k -f -j  c  i -g -h
     *  a -n -e  k  c -h  g  j -a -m  d -o -b  l  f -i
     *  a -l -g  j -b  n  f -h  a  o -e  i -c -m  d -k
     *  a -j  f -o -c  k -d  i -a  n  g -l  b -h  e -m
     *  a -h  d -i  b -j  e -k  a -l  f -m  c -n  g -o
     *  a -i  e -l  c -o -g  m -a  j -d  h -b  k -f  n
     *  a -k  g  m -b  i -f -o  a -h  e -n -c  j -d  l
     *  a -m -f  h -c -l  d -n -a  i -g -k  b -o -e  j
     *  a -o -d  n  b -m -e  l  a -k -f  j  c -i -g  h
     *  a  n -e -k  c  h  g -j -a  m  d  o -b -l  f  i
     *  a  l -g -j -b -n  f  h  a -o -e -i -c  m  d  k
     *  a  j  f  o -c -k -d -i -a -n  g  l  b  h  e  m
     *  a  h  d  i  b  j  e  k  a  l  f  m  c  n  g  o
     * y odd x odd
     *  q  t  w  z  C -E -B -y -v -s -p -r -u -x -A -D
     *  s  z -D -w -p -v -C  A  t  r  y -E -x -q -u -B
     *  u -E -t -v  D  s  w -C -r -x  B  q  y -A -p -z
     *  w -y -u  A  s -C -q  E  p  D -r -B  t  z -v -x
     *  y -s -E  r -z -x  t  D -q  A  w -u -C  p -B -v
     *  A -r  v -E -w  q -z -B  s -u  D  x -p  y  C -t
     *  C -x  s -q  v -A -E  z -u  p -t  y -D -B  w -r
     *  E -D  C -B  A -z  y -x  w -v  u -t  s -r  q -p
     * -D  A -x  u -r  p -s  v -y  B -E -C  z -w  t -q
     * -B  u -q  x -E -y  r -t  A  C -v  p -w  D  z -s
     * -z  p -A -y  q -B -x  r -C -w  s -D -v  t -E -u
     * -x  v  z -t -B  r  D -p  E  q -C -s  A  u -y -w
     * -v  B  p  C -u -w  A  q  D -t -x  z  r  E -s -y
     * -t -C  y  p  x -D -u -s -B  z  q  w -E -v -r -A
     * -r -w -B  D  y  t  p  u  z  E -A -v -q -s -x -C
     * -p -q -r -s -t -u -v -w -x -y -z -A -B -C -D -E
     * y even x odd
     *  p  q  r  s  t  u  v  w  x  y  z  A  B  C  D  E
     *  r  w  B -D -y -t -p -u -z -E  A  v  q  s  x  C
     *  t  C -y -p -x  D  u  s  B -z -q -w  E  v  r  A
     *  v -B -p -C  u  w -A -q -D  t  x -z -r -E  s  y
     *  x -v -z  t  B -r -D  p -E -q  C  s -A -u  y  w
     *  z -p  A  y -q  B  x -r  C  w -s  D  v -t  E  u
     *  B -u  q -x  E  y -r  t -A -C  v -p  w -D -z  s
     *  D -A  x -u  r -p  s -v  y -B  E  C -z  w -t  q
     * -E  D -C  B -A  z -y  x -w  v -u  t -s  r -q  p
     * -C  x -s  q -v  A  E -z  u -p  t -y  D  B -w  r
     * -A  r -v  E  w -q  z  B -s  u -D -x  p -y -C  t
     * -y  s  E -r  z  x -t -D  q -A -w  u  C -p  B  v
     * -w  y  u -A -s  C  q -E -p -D  r  B -t -z  v  x
     * -u  E  t  v -D -s -w  C  r  x -B -q -y  A  p  z
     * -s -z  D  w  p  v  C -A -t -r -y  E  x  q  u  B
     * -q -t -w -z -C  E  B  y  v  s  p  r  u  x  A  D */
    CoeffArray<16> cEven
    {{
        c[0], c[2], c[4], c[6], c[8], c[10], c[12], c[14],
        c[16], c[18], c[20], c[22], c[24], c[26], c[28], c[30]
    }};

    CoeffArray<16> cOdd
    {{
        c[1], c[3], c[5], c[7], c[9], c[11], c[13], c[15],
        c[17], c[19], c[21], c[23], c[25], c[27], c[29], c[31]
    }};
#if 0
    static const CoeffMatrix<16> even
    {{
        {{64,  90,  89,  87,  83,  80,  75,  70,  64,  57,  50,  43,  36,  25,  18,   9}},
        {{64,  80,  50,   9, -36, -70, -89, -87, -64, -25,  18,  57,  83,  90,  75,  43}},
        {{64,  57, -18, -80, -83, -25,  50,  90,  64,  -9, -75, -87, -36,  43,  89,  70}},
        {{64,  25, -75, -70,  36,  90,  18, -80, -64,  43,  89,   9, -83, -57,  50,  87}},
        {{64,  -9, -89,  25,  83, -43, -75,  57,  64, -70, -50,  80,  36, -87, -18,  90}},
        {{64, -43, -50,  90, -36, -57,  89, -25, -64,  87, -18, -70,  83,  -9, -75,  80}},
        {{64, -70,  18,  43, -83,  87, -50,  -9,  64, -90,  75, -25, -36,  80, -89,  57}},
        {{64, -87,  75, -57,  36,  -9, -18,  43, -64,  80, -89,  90, -83,  70, -50,  25}},
        {{64, -90,  89, -87,  83, -80,  75, -70,  64, -57,  50, -43,  36, -25,  18,  -9}},
        {{64, -80,  50,  -9, -36,  70, -89,  87, -64,  25,  18, -57,  83, -90,  75, -43}},
        {{64, -57, -18,  80, -83,  25,  50, -90,  64,   9, -75,  87, -36, -43,  89, -70}},
        {{64, -25, -75,  70,  36, -90,  18,  80, -64, -43,  89,  -9, -83,  57,  50, -87}},
        {{64,   9, -89, -25,  83,  43, -75, -57,  64,  70, -50, -80,  36,  87, -18, -90}},
        {{64,  43, -50, -90, -36,  57,  89,  25, -64, -87, -18,  70,  83,   9, -75, -80}},
        {{64,  70,  18, -43, -83, -87, -50,   9,  64,  90,  75,  25, -36, -80, -89, -57}},
        {{64,  87,  75,  57,  36,   9, -18, -43, -64, -80, -89, -90, -83, -70, -50, -25}}
    }};
#endif
    static const CoeffMatrix<16> odd
    {{
        {{ 90,  82,  67,  46,  22,  -4, -31, -54, -73, -85, -90, -88, -78, -61, -38, -13}},
        {{ 85,  46, -13, -67, -90, -73, -22,  38,  82,  88,  54,  -4, -61, -90, -78, -31}},
        {{ 78,  -4, -82, -73,  13,  85,  67, -22, -88, -61,  31,  90,  54, -38, -90, -46}},
        {{ 67, -54, -78,  38,  85, -22, -90,   4,  90,  13, -88, -31,  82,  46, -73, -61}},
        {{ 54, -85,  -4,  88, -46, -61,  82,  13, -90,  38,  67, -78, -22,  90, -31, -73}},
        {{ 38, -88,  73,  -4, -67,  90, -46, -31,  85, -78,  13,  61, -90,  54,  22, -82}},
        {{ 22, -61,  85, -90,  73, -38,  -4,  46, -78,  90, -82,  54, -13, -31,  67, -88}},
        {{  4, -13,  22, -31,  38, -46,  54, -61,  67, -73,  78, -82,  85, -88,  90, -90}},
        {{-13,  38, -61,  78, -88,  90, -85,  73, -54,  31,  -4, -22,  46, -67,  82, -90}},
        {{-31,  78, -90,  61,  -4, -54,  88, -82,  38,  22, -73,  90, -67,  13,  46, -85}},
        {{-46,  90, -38, -54,  90, -31, -61,  88, -22, -67,  85, -13, -73,  82,  -4, -78}},
        {{-61,  73,  46, -82, -31,  88,  13, -90,   4,  90, -22, -85,  38,  78, -54, -67}},
        {{-73,  31,  90,  22, -78, -67,  38,  90,  13, -82, -61,  46,  88,   4, -85, -54}},
        {{-82, -22,  54,  90,  61, -13, -78, -85, -31,  46,  90,  67,  -4, -73, -88, -38}},
        {{-88, -67, -31,  13,  54,  82,  90,  78,  46,   4, -38, -73, -90, -85, -61, -22}},
        {{-90, -90, -88, -85, -82, -78, -73, -67, -61, -54, -46, -38, -31, -22, -13,  -4}}
     }};

    //iDCT(even, cEven);
    iDCT(cEven);
    iDCT(odd, cOdd);

    std::swap(cEven[1], cEven[15]);
    std::swap(cEven[3], cEven[14]);
    std::swap(cEven[5], cEven[13]);
    std::swap(cEven[7], cEven[12]);
    std::swap(cEven[9], cEven[11]);
    std::swap(cEven[9], cEven[10]);
    std::swap(cEven[5], cEven[9]);
    std::swap(cEven[1], cEven[8]);
    std::swap(cEven[3], cEven[7]);
    std::swap(cEven[3], cEven[6]);
    std::swap(cEven[1], cEven[4]);
    std::swap(cEven[1], cEven[2]);

    for(auto i = 0; i < n; ++i)
    {
        c[i] =
            0 == i % 2
            ? cEven[i >> 1] - cOdd[(n - 1 - i) >> 1]
            : cEven[(n - 1 - i) >> 1] + cOdd[i >> 1];
    }
}
/*----------------------------------------------------------------------------*/
template <size_t n>
void iDCT(
        Structure::PelBuffer &buf, PelCoord base,
        int,
        int bdShift, int bdOffset,
        int min, int max)
{
    Array<CoeffArray<n>, n> column;

    auto ySrc = base.y();

    for(auto y = 0; y < int(n); ++y, ++ySrc)
    {
        auto xSrc = base.x();

        for(auto x = 0; x < int(n); ++x, ++xSrc)
        {
            column[x][y] = buf[{xSrc, ySrc}];
        }
    }

    for(auto i = 0; i < int(n); ++i)
    {
        /* TODO: odd-even can be calculated by different threads */
        iDCT(column[i]);
    }

    Array<CoeffArray<n>, n> row;

    for(auto y = 0; y < int(n); ++y)
    {
        for(auto x = 0; x < int(n); ++x)
        {
            row[y][x] = clip3(min, max, (column[x][y] + 64) >> 7);
        }
    }

    for(auto i = 0; i < int(n); ++i)
    {
        /* TODO: odd-even can be calculated by different threads */
        iDCT(row[i]);
    }

    auto yDst = base.y();

    for(auto y = 0; y < int(n); ++y, ++yDst)
    {
        auto xDst = base.x();

        for(auto x = 0; x < int(n); ++x, ++xDst)
        {
            buf[{xDst, yDst}] = clip3(min, max, (row[y][x] + bdOffset) >> bdShift);
        }
    }
}
/*----------------------------------------------------------------------------*/
void iDST(
        Structure::PelBuffer &buf, PelCoord base,
        int,
        int bdShift, int bdOffset,
        int min, int max)
{
    const auto n = 4;
    Array<CoeffArray<n>, n> column;

    auto ySrc = base.y();

    for(auto y = 0; y < int(n); ++y, ++ySrc)
    {
        auto xSrc = base.x();

        for(auto x = 0; x < int(n); ++x, ++xSrc)
        {
            column[x][y] = buf[{xSrc, ySrc}];
        }
    }

    for(auto i = 0; i < int(n); ++i)
    {
        /* TODO: odd-even can be calculated by different threads */
        iDST(column[i]);
    }

    Array<CoeffArray<n>, n> row;

    for(auto y = 0; y < int(n); ++y)
    {
        for(auto x = 0; x < int(n); ++x)
        {
            row[y][x] = clip3(min, max, (column[x][y] + 64) >> 7);
        }
    }

    for(auto i = 0; i < int(n); ++i)
    {
        /* TODO: odd-even can be calculated by different threads */
        iDST(row[i]);
    }

    auto yDst = base.y();

    for(auto y = 0; y < int(n); ++y, ++yDst)
    {
        auto xDst = base.x();

        for(auto x = 0; x < int(n); ++x, ++xDst)
        {
            buf[{xDst, yDst}] = clip3(min, max, (row[y][x] + bdOffset) >> bdShift);
        }
    }
}
#if 0
/*----------------------------------------------------------------------------*/
static const Matrix4x4 coeffIntraY4x4 =
{
    /* 04/2013, 8.6.4.2 "Transformation process", (8-277) */
    {
        {{29, 55, 74, 84}},
        {{74, 74, 0, -74}},
        {{84, -29, -74, 55}},
        {{55, -84, 74, -29}}
    }
};

static const Matrix4x4 coeff4x4 =
{
    /* 04/2013, 8.6.4.2 "Transformation process", (8-279, 8-280, 8-281, 8-282) */
    {
        {{64,  64,  64,  64}},
        {{83,  36, -36, -83}},
        {{64, -64, -64,  64}},
        {{36, -83,  83, -36}}
    }
};

static const Matrix8x8 coeff8x8 =
{
    /* 04/2013, 8.6.4.2 "Transformation process", (8-279, 8-280, 8-281, 8-282) */
    {
        {{64,  64,  64,  64,  64,  64,  64,  64}},
        {{89,  75,  50,  18, -18, -50, -75, -89}},
        {{83,  36, -36, -83, -83, -36,  36,  83}},
        {{75, -18, -89, -50,  50,  89,  18, -75}},
        {{64, -64, -64,  64,  64, -64, -64,  64}},
        {{50, -89,  18,  75, -75, -18,  89, -50}},
        {{36, -83,  83, -36, -36,  83, -83,  36}},
        {{18, -50,  75, -89,  89, -75,  50, -18}}
    }
};

static const Matrix16x16 coeff16x16 =
{
    /* 04/2013, 8.6.4.2 "Transformation process", (8-279, 8-280, 8-281, 8-282) */
    {
        {{64,  64,  64,  64,  64,  64,  64,  64,  64,  64,  64,  64,  64,  64,  64,  64}},
        {{90,  87,  80,  70,  57,  43,  25,   9,  -9, -25, -43, -57, -70, -80, -87, -90}},
        {{89,  75,  50,  18, -18, -50, -75, -89, -89, -75, -50, -18,  18,  50,  75,  89}},
        {{87,  57,   9, -43, -80, -90, -70, -25,  25,  70,  90,  80,  43,  -9, -57, -87}},
        {{83,  36, -36, -83, -83, -36,  36,  83,  83,  36, -36, -83, -83, -36,  36,  83}},
        {{80,   9, -70, -87, -25,  57,  90,  43, -43, -90, -57,  25,  87,  70,  -9, -80}},
        {{75, -18, -89, -50,  50,  89,  18, -75, -75,  18,  89,  50, -50, -89, -18,  75}},
        {{70, -43, -87,   9,  90,  25, -80, -57,  57,  80, -25, -90,  -9,  87,  43, -70}},
        {{64, -64, -64,  64,  64, -64, -64,  64,  64, -64, -64,  64,  64, -64, -64,  64}},
        {{57, -80, -25,  90,  -9, -87,  43,  70, -70, -43,  87,   9, -90,  25,  80, -57}},
        {{50, -89,  18,  75, -75, -18,  89, -50, -50,  89, -18, -75,  75,  18, -89,  50}},
        {{43, -90,  57,  25, -87,  70,   9, -80,  80,  -9, -70,  87, -25, -57,  90, -43}},
        {{36, -83,  83, -36, -36,  83, -83,  36,  36, -83,  83, -36, -36,  83, -83,  36}},
        {{25, -70,  90, -80,  43,   9, -57,  87, -87,  57,  -9, -43,  80, -90,  70, -25}},
        {{18, -50,  75, -89,  89, -75,  50, -18, -18,  50, -75,  89, -89,  75, -50,  18}},
        {{ 9, -25,  43, -57,  70, -80,  87, -90,  90, -87,  80, -70,  57, -43,  25,  -9}}
    }
};

static const Matrix32x32 coeff32x32 =
{{
    /* 04/2013, 8.6.4.2 "Transformation process", (8-279, 8-280, 8-281, 8-282) */
    {{64,  64,  64,  64,  64,  64,  64,  64,  64,  64,  64,  64,  64,  64,  64,  64,  64,  64,  64,  64,  64,  64,  64,  64,  64,  64,  64,  64,  64,  64,  64,  64}},
    {{90,  90,  88,  85,  82,  78,  73,  67,  61,  54,  46,  38,  31,  22,  13,   4,  -4, -13, -22, -31, -38, -46, -54, -61, -67, -73, -78, -82, -85, -88, -90, -90}},
    {{90,  87,  80,  70,  57,  43,  25,   9,  -9, -25, -43, -57, -70, -80, -87, -90, -90, -87, -80, -70, -57, -43, -25,  -9,   9,  25,  43,  57,  70,  80,  87,  90}},
    {{90,  82,  67,  46,  22,  -4, -31, -54, -73, -85, -90, -88, -78, -61, -38, -13,  13,  38,  61,  78,  88,  90,  85,  73,  54,  31,   4, -22, -46, -67, -82, -90}},
    {{89,  75,  50,  18, -18, -50, -75, -89, -89, -75, -50, -18,  18,  50,  75,  89,  89,  75,  50,  18, -18, -50, -75, -89, -89, -75, -50, -18,  18,  50,  75,  89}},
    {{88,  67,  31, -13, -54, -82, -90, -78, -46,  -4,  38,  73,  90,  85,  61,  22, -22, -61, -85, -90, -73, -38,   4,  46,  78,  90,  82,  54,  13, -31, -67, -88}},
    {{87,  57,   9, -43, -80, -90, -70, -25,  25,  70,  90,  80,  43,  -9, -57, -87, -87, -57,  -9,  43,  80,  90,  70,  25, -25, -70, -90, -80, -43,   9,  57,  87}},
    {{85,  46, -13, -67, -90, -73, -22,  38,  82,  88,  54,  -4, -61, -90, -78, -31,  31,  78,  90,  61,   4, -54, -88, -82, -38,  22,  73,  90,  67,  13, -46, -85}},
    {{83,  36, -36, -83, -83, -36,  36,  83,  83,  36, -36, -83, -83, -36,  36,  83,  83,  36, -36, -83, -83, -36,  36,  83,  83,  36, -36, -83, -83, -36,  36,  83}},
    {{82,  22, -54, -90, -61,  13,  78,  85,  31, -46, -90, -67,   4,  73,  88,  38, -38, -88, -73,  -4,  67,  90,  46, -31, -85, -78, -13,  61,  90,  54, -22, -82}},
    {{80,   9, -70, -87, -25,  57,  90,  43, -43, -90, -57,  25,  87,  70,  -9, -80, -80,  -9,  70,  87,  25, -57, -90, -43,  43,  90,  57, -25, -87, -70,   9,  80}},
    {{78,  -4, -82, -73,  13,  85,  67, -22, -88, -61,  31,  90,  54, -38, -90, -46,  46,  90,  38, -54, -90, -31,  61,  88,  22, -67, -85, -13,  73,  82,   4, -78}},
    {{75, -18, -89, -50,  50,  89,  18, -75, -75,  18,  89,  50, -50, -89, -18,  75,  75, -18, -89, -50,  50,  89,  18, -75, -75,  18,  89,  50, -50, -89, -18,  75}},
    {{73, -31, -90, -22,  78,  67, -38, -90, -13,  82,  61, -46, -88,  -4,  85,  54, -54, -85,   4,  88,  46, -61, -82,  13,  90,  38, -67, -78,  22,  90,  31, -73}},
    {{70, -43, -87,   9,  90,  25, -80, -57,  57,  80, -25, -90,  -9,  87,  43, -70, -70,  43,  87,  -9, -90, -25,  80,  57, -57, -80,  25,  90,   9, -87, -43,  70}},
    {{67, -54, -78,  38,  85, -22, -90,   4,  90,  13, -88, -31,  82,  46, -73, -61,  61,  73, -46, -82,  31,  88, -13, -90,  -4,  90,  22, -85, -38,  78,  54, -67}},
    {{64, -64, -64,  64,  64, -64, -64,  64,  64, -64, -64,  64,  64, -64, -64,  64,  64, -64, -64,  64,  64, -64, -64,  64,  64, -64, -64,  64,  64, -64, -64,  64}},
    {{61, -73, -46,  82,  31, -88, -13,  90,  -4, -90,  22,  85, -38, -78,  54,  67, -67, -54,  78,  38, -85, -22,  90,   4, -90,  13,  88, -31, -82,  46,  73, -61}},
    {{57, -80, -25,  90,  -9, -87,  43,  70, -70, -43,  87,   9, -90,  25,  80, -57, -57,  80,  25, -90,   9,  87, -43, -70,  70,  43, -87,  -9,  90, -25, -80,  57}},
    {{54, -85,  -4,  88, -46, -61,  82,  13, -90,  38,  67, -78, -22,  90, -31, -73,  73,  31, -90,  22,  78, -67, -38,  90, -13, -82,  61,  46, -88,   4,  85, -54}},
    {{50, -89,  18,  75, -75, -18,  89, -50, -50,  89, -18, -75,  75,  18, -89,  50,  50, -89,  18,  75, -75, -18,  89, -50, -50,  89, -18, -75,  75,  18, -89,  50}},
    {{46, -90,  38,  54, -90,  31,  61, -88,  22,  67, -85,  13,  73, -82,   4,  78, -78,  -4,  82, -73, -13,  85, -67, -22,  88, -61, -31,  90, -54, -38,  90, -46}},
    {{43, -90,  57,  25, -87,  70,   9, -80,  80,  -9, -70,  87, -25, -57,  90, -43, -43,  90, -57, -25,  87, -70, -9,   80, -80,   9,  70, -87,  25,  57, -90,  43}},
    {{38, -88,  73,  -4, -67,  90, -46, -31,  85, -78,  13,  61, -90,  54,  22, -82,  82, -22, -54,  90, -61, -13,  78, -85,  31,  46, -90,  67,   4, -73,  88, -38}},
    {{36, -83,  83, -36, -36,  83, -83,  36,  36, -83,  83, -36, -36,  83, -83,  36,  36, -83,  83, -36, -36,  83, -83,  36,  36, -83,  83, -36, -36,  83, -83,  36}},
    {{31, -78,  90, -61,   4,  54, -88,  82, -38, -22,  73, -90,  67, -13, -46,  85, -85,  46,  13, -67,  90, -73,  22,  38, -82,  88, -54,  -4,  61, -90,  78, -31}},
    {{25, -70,  90, -80,  43,   9, -57,  87, -87,  57,  -9, -43,  80, -90,  70, -25, -25,  70, -90,  80, -43,  -9,  57, -87,  87, -57,   9,  43, -80,  90, -70,  25}},
    {{22, -61,  85, -90,  73, -38,  -4,  46, -78,  90, -82,  54, -13, -31,  67, -88,  88, -67,  31,  13, -54,  82, -90,  78, -46,   4,  38, -73,  90, -85,  61, -22}},
    {{18, -50,  75, -89,  89, -75,  50, -18, -18,  50, -75,  89, -89,  75, -50,  18,  18, -50,  75, -89,  89, -75,  50, -18, -18,  50, -75,  89, -89,  75, -50,  18}},
    {{13, -38,  61, -78,  88, -90,  85, -73,  54, -31,   4,  22, -46,  67, -82,  90, -90,  82, -67,  46, -22,  -4,  31, -54,  73, -85,  90, -88,  78, -61,  38, -13}},
    {{ 9, -25,  43, -57,  70, -80,  87, -90,  90, -87,  80, -70,  57, -43,  25,  -9,  -9,  25, -43,  57, -70,  80, -87,  90, -90,  87, -80,  70, -57,  43, -25,   9}},
    {{ 4, -13,  22, -31,  38, -46,  54, -61,  67, -73,  78, -82,  85, -88,  90, -90,  90, -90,  88, -85,  82, -78,  73, -67,  61, -54,  46, -38,  31, -22,  13,  -4}}
}};
/*----------------------------------------------------------------------------*/
template <typename T, size_t n>
void hTransform(
        Structure::PelBuffer &buf, PelCoord base,
        int bitDepth,
        int bdShift, int bdOffset,
        int min, int max,
        const std::array<std::array<T, n>, n> &m)
{
    /* 04/2013, 8.6.4.2 "Transformation process", (8-276) */

    for(auto y = 0; y < int(n); ++y)
    {
        std::array<Sample, n> row;
        const auto ySrc = base.y() + Pel{y};

        for(int i = 0; i < int(n); ++i)
        {
            int unClipped = 0;
            auto xSrc = base.x();

            for(int j = 0; j < int(n); ++j, ++xSrc)
            {
                unClipped += m[j][i] * buf[{xSrc, ySrc}];
            }

            row[i] = clip3(min, max, (unClipped + bdOffset) >> bdShift);
        }

        for(auto i = 0; i < int(n); ++i)
        {
            buf[{base.x() + Pel{i}, base.y() + Pel{y}}] = row[i];
        }
    }
}

template <typename T, size_t n>
void vTransform(
        Structure::PelBuffer &buf, PelCoord base,
        int min, int max,
        const std::array<std::array<T, n>, n> &m)
{
    for(auto x = 0; x < int(n); ++x)
    {
        std::array<Sample, n> column;
        const auto xSrc = base.x() + Pel{x};

        for(int i = 0; i < int(n); ++i)
        {
            int unClipped = 0;
            auto ySrc = base.y();

            for(int j = 0; j < int(n); ++j, ++ySrc)
            {
                unClipped += m[j][i] * buf[{xSrc, ySrc}];
            }

            /* 04/2013, 8.6.4.1 "Transformation process for scaled transform coefficients"
             * 2 (clip) */
            column[i] = clip3(min, max, (unClipped + 64) >> 7);
        }

        for(auto i = 0; i < int(n); ++i)
        {
            buf[{base.x() + Pel{x}, base.y() + Pel{i}}] = column[i];
        }
    }
}
/*----------------------------------------------------------------------------*/
template <typename T, size_t n>
void transform(
        Structure::PelBuffer &buf,
        Plane plane, PelCoord base,
        int bitDepth,
        int bdShift, int bdOffset,
        int min, int max,
        const std::array<std::array<T, n>, n> &m)
{
    /* 04/2013, 8.6.4.1 "Transformation process for scaled transform coefficients" */

    // 1 (vertical transform)
    vTransform(buf, base, min, max, m);
    // 3 (horizontal transform)
    hTransform(buf, base, bitDepth, bdShift, bdOffset, min, max, m);
}
#endif
/*----------------------------------------------------------------------------*/
} /* namespace */

void Transformation::exec(
        State &,
        Ptr<Structure::Picture> picture,
        const Syntax::CodingUnit &cu,
        const Syntax::ResidualCoding &rc)
{
    using namespace Syntax;

    typedef SequenceParameterSet SPS;
    typedef SpsRangeExtension SPSRE;

    const auto spsre = picture->spsre;
    const auto extendedPrecisionProcessingFlag =
        spsre && bool(*spsre->get<SPSRE::ExtendedPrecisionProcessingFlag>());
    const auto rcCoord = rc.get<ResidualCoding::Coord>()->inUnits();
    const auto rcSize = rc.get<ResidualCoding::Size>()->inUnits();
    const Plane plane = *rc.get<ResidualCoding::CIdx>();
    const auto coord = scale(rcCoord, plane, picture->chromaFormatIdc);
    const auto cuPredMode = cu.get<CodingUnit::CuPredMode>();
    const auto bitDepth = picture->bitDepth(plane);
    auto &buf = picture->pelBuffer(PelLayerId::Residual, plane);

    const auto min = minCoeff(extendedPrecisionProcessingFlag, bitDepth);
    const auto max = maxCoeff(extendedPrecisionProcessingFlag, bitDepth);

    /* 04/2013, 8.6.2 "Scaling and transformation process", (8-268) */
    /* 10/2014, 8.6.2 "Scaling and transformation process", (8-265) */
    const auto bdShift = std::max(20 - bitDepth, extendedPrecisionProcessingFlag ? 11 : 0);
    const auto bdOffset = 1 << (bdShift - 1);
#if 0
    /* trType derivation */
    if(
            PredictionMode::Intra == *cuPredMode
            && 2_log2 == rcSize
            && Plane::Y == plane)
    {
        transform(buf, plane, coord, bitDepth, bdShift, bdOffset, min, max, coeffIntraY4x4);
    }
    else if(2_log2 == rcSize)
    {
        transform(buf, plane, coord, bitDepth, bdShift, bdOffset, min, max, coeff4x4);
    }
    else if(3_log2 == rcSize)
    {
        transform(buf, plane, coord, bitDepth, bdShift, bdOffset, min, max, coeff8x8);
    }
    else if(4_log2 == rcSize)
    {
        transform(buf, plane, coord, bitDepth, bdShift, bdOffset, min, max, coeff16x16);
    }
    else if(5_log2 == rcSize)
    {
        transform(buf, plane, coord, bitDepth, bdShift, bdOffset, min, max, coeff32x32);
    }
#endif
    /* trType derivation */
    if(
            PredictionMode::Intra == *cuPredMode
            && 2_log2 == rcSize
            && Plane::Y == plane)
    {
        iDST(buf, coord, bitDepth, bdShift, bdOffset, min, max);
    }
    else if(2_log2 == rcSize)
    {
        iDCT<4>(buf, coord, bitDepth, bdShift, bdOffset, min, max);
    }
    else if(3_log2 == rcSize)
    {
        iDCT<8>(buf, coord, bitDepth, bdShift, bdOffset, min, max);
    }
    else if(4_log2 == rcSize)
    {
        iDCT<16>(buf, coord, bitDepth, bdShift, bdOffset, min, max);
    }
    else if(5_log2 == rcSize)
    {
        iDCT<32>(buf, coord, bitDepth, bdShift, bdOffset, min, max);
    }

    const auto toStr =
        [coord, rcSize, &buf](std::ostream &oss)
        {
            const auto side = toPel(rcSize);

            oss << coord << '\n';

            for(auto y = 0_pel; y < side; ++y)
            {
                for(auto x = 0_pel; x < side; ++x)
                {
                    pelFmt(oss, buf[coord + PelCoord{x, y}]);
                    oss << ' ';
                }

                oss << '\n';
            }
        };

    const LogId logId[] =
    {
        LogId::TransformedSamplesY,
        LogId::TransformedSamplesCb,
        LogId::TransformedSamplesCr
    };

    log(logId[int(plane)], toStr);
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processses */
