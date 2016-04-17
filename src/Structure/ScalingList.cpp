#include <Structure/ScalingList.h>
#include <Syntax/ScalingListData.h>
#include <sstream>

namespace HEVC { namespace Structure {
/*----------------------------------------------------------------------------*/
auto ScalingList::defaultCoeff(SizeId sizeId, MatrixId matrixId)
    -> const Coeff &
{
    /* 04/2013, 7.4.5 "Scaling list data semantics" */

    /* Table 7-5 */
    static const ScalingList::Coeff common4x4 =
    {
        16, 16, 16, 16,
        16, 16, 16, 16,
        16, 16, 16, 16,
        16, 16, 16, 16
    };

    /* Table 7-6 */
    static const ScalingList::Coeff intra8x8 =
    {
        /* i == [0..15] */
        16, 16, 16, 16, 16, 16, 16, 16,
        16, 16, 17, 16, 17, 16, 17, 18,
        /* i == [16..31] */
        17, 18, 18, 17, 18, 21, 19, 20,
        21, 20, 19, 21, 24, 22, 22, 24,
        /* i == [31..47] */
        24, 22, 22, 24, 25, 25, 27, 30,
        27, 25, 25, 29, 31, 35, 35, 31,
        /* i == [48..63] */
        29, 36, 41, 44, 41, 36, 47, 54,
        54, 47, 65, 70, 65, 88, 88, 115

    };

    /* Table 7-6 */
    static const ScalingList::Coeff inter8x8 =
    {
        /* i == [0..15] */
        16, 16, 16, 16, 16, 16, 16, 16,
        16, 16, 17, 17, 17, 17, 17, 18,
        /* i == [16..31] */
        18, 18, 18, 18, 18, 20, 20, 20,
        20, 20, 20, 20, 24, 24, 24, 24,
        /* i == [31..47] */
        24, 24, 24, 24, 25, 25, 25, 25,
        25, 25, 25, 28, 28, 28, 28, 28,
        /* i == [48..63] */
        28, 33, 33, 33, 33, 33, 41, 41,
        41, 41, 54, 54, 54, 71, 71, 91
    };

    if(SizeId::Id0 == sizeId)
    {
        return common4x4;
    }
    else
    {
        if(PredictionMode::Intra == toPredictionMode(matrixId))
        {
            return intra8x8;
        }
        else //if(PrefictionMode::Inter == getPredictionMode(sizeId, matrixId))
        {
            return inter8x8;
        }
    }
}
/*----------------------------------------------------------------------------*/
void ScalingList::derive(const Syntax::ScalingListData &sld)
{
    using namespace Syntax;

    typedef ScalingListData SLD;

    for(const auto sizeId : EnumRange<SizeId>())
    {
        for(const auto matrixId : EnumRange<MatrixId>())
        {
            if(
                    SizeId::Id3 == sizeId
                    && (
                        MatrixId::Id1 == matrixId
                        || MatrixId::Id2 == matrixId
                        || MatrixId::Id4 == matrixId
                        || MatrixId::Id5 == matrixId))
            {
                continue;
            }

            const auto at = makeTuple(sizeId, matrixId);

            if(!(*sld.get<SLD::ScalingListPredModeFlag>())[at])
            {
                const auto delta = (*sld.get<SLD::ScalingListPredMatrixIdDelta>())[at];

                if(0 == delta)
                {
                    (*this)[at] = ScalingList::defaultCoeff(sizeId, matrixId);
                }
                else
                {
                    const auto refAt = makeTuple(
                            sizeId,
                            static_cast<MatrixId>(
                                int(matrixId)
                                - delta * (SizeId::Id3 == sizeId ? 3 : 1)));

                    (*this)[at] = (*this)[refAt];
                }
            }
            else
            {
                int nextCoef = 8;
                const int coefNum = std::min(64, 1 << (4 + (int(sizeId) << 1)));

                if(SizeId::Id1 < sizeId)
                {
                    nextCoef = (*sld.get<SLD::ScalingListDcCoefMinus8>())[at] + 8;
                }

                for(auto i = 0; i < coefNum; ++i)
                {
                    const auto j = makeTuple(sizeId, matrixId, i);

                    nextCoef =
                        (nextCoef + (*sld.get<SLD::ScalingListDeltaCoef>())[j] + 256) % 256;
                    (*this)[makeTuple(sizeId, matrixId, i)] = nextCoef;
                }
            }
        }
    }
}
/*----------------------------------------------------------------------------*/
std::string ScalingList::toStr() const
{
    std::ostringstream oss;

    for(const auto sizeId : EnumRange<SizeId>())
    {
        const int sideLength = std::min(getMaxSideLength(), toSideLength(sizeId));

        for(const auto matrixId : EnumRange<MatrixId>())
        {
            oss << int(sizeId) << ":" << int(matrixId) << '\n';

            const auto &coeff = (*this)[makeTuple(sizeId, matrixId)];

            runtime_assert(int(coeff.size()) == sideLength * sideLength);

            for(int y = 0; y < sideLength; ++y)
            {
                for(int x = 0; x < sideLength; ++x)
                {
                    oss << coeff[y * sideLength + x] << ' ';
                }

                oss << '\n';
            }
        }
    }

    return oss.str();
}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Structure */
