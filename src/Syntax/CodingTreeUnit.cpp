#include <Syntax/CodingTreeUnit.h>
#include <Syntax/SliceSegmentHeader.h>
#include <Syntax/SAO.h>
#include <Decoder/State.h>
#include <Structure/Picture.h>
#include <BitMatrix.h>
#include <log.h>

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
void CodingTreeUnit::toStr(std::ostream &os, Log2 ctuSize) const
{
    using namespace Limits;

    typedef CodingUnit CU;
    typedef TransformUnit TU;

    typedef BitMatrix<CodingBlock::max, CodingBlock::max> BdryMatrix;

    const auto vSet =
        [](BdryMatrix &bdry, PelCoord offset, Log2 size, Pel xOffset)
        {
            const auto side = toPel(size);

            for(auto y = 0_pel; y < side; ++y)
            {
                const auto xPos = toUnderlying(offset.x() + xOffset);
                const auto yPos = toUnderlying(offset.y() + y);

                bdry[{xPos, yPos}] = true;
            }
        };

    const auto hSet =
        [](BdryMatrix &bdry, PelCoord offset, Log2 size, Pel yOffset)
        {
            const auto side = toPel(size);

            for(auto x = 0_pel; x < side; ++x)
            {
                const auto xPos = toUnderlying(offset.x() + x);
                const auto yPos = toUnderlying(offset.y() + yOffset);

                bdry[{xPos, yPos}] = true;
            }
        };

    const auto ctuCoord = get<Coord>()->inUnits();
    const auto ctuSide = toPel(ctuSize);

    const auto cqtListInZ = getCodingQuadTreeListInZ();

    BdryMatrix cuBdry, tuBdry;

    for(auto i : cqtListInZ)
    {
        const auto cu = i->getCodingUnit();

        if(cu)
        {
            const auto cuCoord = cu->get<CU::Coord>()->inUnits();
            const auto cuSize = cu->get<CU::Size>()->inUnits();
            const auto cuOffset = cuCoord - ctuCoord;

            vSet(cuBdry, cuOffset, cuSize, 0_pel);
            hSet(cuBdry, cuOffset, cuSize, 0_pel);

            const auto ttList = cu->getTransformTreeListInZ();

            for(auto tt : ttList)
            {
                const auto tu = tt->getTransformUnit();

                if(tu)
                {
                    const auto tuCoord = tu->get<TU::Coord>()->inUnits();
                    const auto tuSize = tu->get<TU::Size>()->inUnits();

                    vSet(tuBdry, (tuCoord - cuCoord) + cuOffset, tuSize, 0_pel);
                    hSet(tuBdry, (tuCoord - cuCoord) + cuOffset, tuSize, 0_pel);
                }
            }

            if(ttList.empty())
            {
                vSet(tuBdry, cuOffset, cuSize, 0_pel);
                hSet(tuBdry, cuOffset, cuSize, 0_pel);
            }
        }
    }

    // RHS & Bottom (this would require increasing side by 1)
    //vSet(cuBdry, {0_pel, 0_pel}, ctuSize, ctuSide);
    //hSet(cuBdry, {0_pel, 0_pel}, ctuSize, ctuSide);
    //vSet(tuBdry, {0_pel, 0_pel}, ctuSize, ctuSide);
    //hSet(tuBdry, {0_pel, 0_pel}, ctuSize, ctuSide);

    os << "CU\n";
    cuBdry.toStr(os, toUnderlying(ctuSide), toUnderlying(ctuSide), '+', ' ');
    os << "TU\n";
    tuBdry.toStr(os, toUnderlying(ctuSide), toUnderlying(ctuSide), '+', ' ');

    os << "CBF Y\n";
    m_cbf[int(Plane::Y)].toStr(os);
    os << "\nCBF Cb\n";
    m_cbf[int(Plane::Cb)].toStr(os);
    os << "\nCBF Cr\n";
    m_cbf[int(Plane::Cr)].toStr(os);
}
/*----------------------------------------------------------------------------*/
void CodingTreeUnit::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
        const SliceSegmentHeader &sh)
{
    typedef SliceSegmentHeader SSH;

    const auto coord = get<Coord>()->inUnits();

    auto picture = decoder.picture();
    const auto sliceSaoLumaFlag = sh.get<SSH::SliceSaoLumaFlag>();
    const auto sliceSaoChromaFlag = sh.get<SSH::SliceSaoChromaFlag>();

    if(*sliceSaoLumaFlag || *sliceSaoChromaFlag)
    {
        auto sao = embedSubtree<SAO>(*this, coord);

        parse(streamAccessLayer, decoder, *sao);
    }

    auto cqt =
        embedCodingQuadTree(coord, PelCoord{0_pel, 0_pel}, picture->ctbSizeY, 0);

    parse(streamAccessLayer, decoder, *cqt, sh, *this);
}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */
