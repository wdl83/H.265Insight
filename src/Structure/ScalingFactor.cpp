#include <Structure/ScalingFactor.h>
#include <Structure/ScalingList.h>
#include <Syntax/ScalingListData.h>
#include <sstream>

namespace HEVC { namespace Structure {
/*----------------------------------------------------------------------------*/
void ScalingFactor::derive(
        const ScalingList &scalingList,
        Ptr<const Syntax::ScalingListData> scalingListData)
{
    using namespace Syntax;

    typedef ScalingListData SLD;

    const auto deriveDcCoeff =
        [scalingListData](SizeId sizeId, MatrixId matrixId)
        {
            runtime_assert(SizeId::Id2 == sizeId || SizeId::Id3 == sizeId);

            if(nullptr != scalingListData)
            {
                const auto scalingListDcCoefMinus8 =
                    scalingListData->get<SLD::ScalingListDcCoefMinus8>();

                return (*scalingListDcCoefMinus8)[makeTuple(sizeId, matrixId)] + 8;
            }
            else
            {
                return SLD::ScalingListDcCoefMinus8::InferredValue + 8;
            }
        };


    static const std::array<Log2, EnumRange<SizeId>::length()> toBlockSize =
    {
        {2_log2, 3_log2, 3_log2, 3_log2}
    };

    static const std::array<Factor::SizeType, EnumRange<SizeId>::length()> toStride =
    {
        {1, 1, 2, 4}
    };

    for(const auto sizeId : EnumRange<SizeId>())
    {
        const Factor::SizeType side = std::min(ScalingList::getMaxSideLength(), toSideLength(sizeId));
        const auto blockSize = toBlockSize[int(sizeId)];
        const auto stride = toStride[int(sizeId)];

        for(const auto matrixId : EnumRange<MatrixId>())
        {
            const auto at =
                makeTuple(
                        SizeId::Id3 == sizeId
                        && (
                            MatrixId::Id1 == matrixId
                            || MatrixId::Id2 == matrixId
                            || MatrixId::Id4 == matrixId
                            || MatrixId::Id5 == matrixId)
                        ? SizeId::Id2
                        : sizeId,
                        matrixId);

            const auto &coeff = scalingList[at];
            Factor f(toSideLength(sizeId), toSideLength(sizeId), 0);

            for(Factor::SizeType i = 0; i < side * side; ++i)
            {
                const auto value = coeff[i];
                const auto pos = toPelCoord(ScanIdx::Diagonal, blockSize, i);
                const Factor::Pos base(toUnderlying(pos.x()) * stride, toUnderlying(pos.y()) * stride);

                for(Factor::SizeType j = 0; j < stride; ++j)
                {
                    for(Factor::SizeType k = 0; k < stride; ++k)
                    {
                        f[{base.x() + k, base.y() + j}] = value;
                    }
                }
            }

            if(SizeId::Id2 == sizeId || SizeId::Id3 == sizeId)
            {
                f[{0, 0}] = deriveDcCoeff(at.get<SizeId>(), at.get<MatrixId>());
            }

            ref(makeTuple(sizeId, matrixId)) = f;
        }
    }
}
/*----------------------------------------------------------------------------*/
std::string ScalingFactor::toStr() const
{
    std::ostringstream oss;

    for(const auto sizeId : EnumRange<SizeId>())
    {
        const Factor::SizeType side = toSideLength(sizeId);

        for(const auto matrixId : EnumRange<MatrixId>())
        {
            oss << int(sizeId) << ':' << int(matrixId) << '\n';

            const auto &f = (*this)[makeTuple(sizeId, matrixId)];

            runtime_assert(f.size() == side * side);

            for(Factor::SizeType y = 0; y < side; ++y)
            {
                for(Factor::SizeType x = 0; x < side; ++x)
                {
                    oss << f[{x, y}] << ' ';
                }

                oss << '\n';
            }
        }
    }

    return oss.str();
}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Structure */
