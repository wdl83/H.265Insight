/* STDC++ */
#include <algorithm>
/* HEVC */
#include <Syntax/ScalingListData.h>

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
/* ScalingListData */
/*----------------------------------------------------------------------------*/
void ScalingListData::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder)
{
    using namespace ScalingListDataContent;
    using namespace Structure;

    auto scalingListPredModeFlag = embed<ScalingListPredModeFlag>(*this);
    auto scalingListPredMatrixIdDelta = embed<ScalingListPredMatrixIdDelta>(*this);
    auto scalingListDcCoefMinus8 = embed<ScalingListDcCoefMinus8>(*this);
    auto scalingListDeltaCoef = embed<ScalingListDeltaCoef>(*this);

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
            else
            {
                const ListIndex at{sizeId, matrixId};

                parse(streamAccessLayer, decoder, *scalingListPredModeFlag, at);

                if(!(*scalingListPredModeFlag))
                {
                    parse(streamAccessLayer, decoder, *scalingListPredMatrixIdDelta, at);

                    if(0 == *scalingListPredMatrixIdDelta)
                    {
                        if(sizeId > SizeId::Id1)
                        {
                            scalingListDcCoefMinus8->infer(at);
                        }
                    }
                    else
                    {
                        const ListIndex refAt
                        {
                            sizeId,
                            static_cast<MatrixId>(
                                    int(matrixId)
                                    - *scalingListPredMatrixIdDelta * (SizeId::Id3 == sizeId ? 3 : 1))
                        };

                        if( sizeId > SizeId::Id1)
                        {
                            scalingListDcCoefMinus8->infer(at, refAt);
                        }
                    }
                }
                else
                {
                    const int coefNum = std::min(64, 1 << (4 + (int(sizeId) << 1)));

                    if(SizeId::Id1 < sizeId)
                    {
                        parse(
                                streamAccessLayer, decoder,
                                *scalingListDcCoefMinus8, at);
                    }

                    for(auto i = 0; i < coefNum; ++i)
                    {
                        parse(
                                streamAccessLayer, decoder,
                                *scalingListDeltaCoef,
                                makeTuple(sizeId, matrixId, i));
                    }
                }
            }
        }
    }
}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */
