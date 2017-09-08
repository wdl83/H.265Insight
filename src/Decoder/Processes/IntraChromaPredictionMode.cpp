#include <Decoder/Processes/IntraChromaPredictionMode.h>
#include <Structure/Picture.h>
#include <Syntax/CodingUnit.h>

namespace HEVC { namespace Decoder { namespace Processes {
/*----------------------------------------------------------------------------*/
/* ITU-T H.265 v4 (12/2016)
 * Table 8-3 */
static
const std::array<int8_t, EnumRange<IntraPredictionMode>::length()> to422 =
{
    {
        0, 1, 2, 2, 2, 2, 3, 5, 7, 8, 10, 12, 13, 15, 17, 18, 19, 20,
        21, 22, 23, 23, 24, 24, 25, 25, 26, 27, 27, 28, 28, 29, 29, 30, 31
    }
};

CuIntraPredMode IntraChromaPredictionMode::exec(
        State &, Ptr<const Structure::Picture> picture,
        const Syntax::CodingUnit &cu)
{
    /* ITU-T H.265 v4 (12/2016)
     * 8.4.3 "Derivation process for chroma intra prediction mode" */
    using namespace Syntax;

    typedef CodingUnit CU;

    const auto chromaFormatIdc = picture->chromaFormatIdc;
    const auto is422 = ChromaFormatIdc::f422 == chromaFormatIdc;
    const auto is444 = ChromaFormatIdc::f444 == chromaFormatIdc;

    runtime_assert(ChromaFormatIdc::fSCP != chromaFormatIdc);

    const auto size = cu.get<CU::Size>()->inUnits();
    const PartitionMode partMode = *cu.get<CU::PartModePseudo>();
    const auto partNum = is444 && partMode == PartitionMode::PART_NxN ? 4 : 1;
    CuIntraPredMode mode = *cu.get<CU::IntraPredModeY>();

    for(auto i = 0; i < partNum; ++i)
    {
        const PelCoord offset
        {
            0 == i % 2 ? 0_pel : toPel(size - 1_log2),
            0 == i / 2 ? 0_pel : toPel(size - 1_log2)
        };

        const auto intraChromaPredMode =
            (*cu.get<CU::IntraChromaPredMode>())[makeTuple(offset, size)];

        /* Table 8-2 */
        switch(intraChromaPredMode)
        {
            default:
                {
                    runtime_assert(false);
                    break;
                }
            case 0:
                {
                    mode[i] =
                        IntraPredictionMode::Planar == mode[i]
                        ? IntraPredictionMode::Angular34
                        : IntraPredictionMode::Planar;
                    break;
                }
            case 1:
                {
                    mode[i] =
                        IntraPredictionMode::Angular26 == mode[i]
                        ? IntraPredictionMode::Angular34
                        : IntraPredictionMode::Angular26;
                    break;
                }
            case 2:
                {
                    mode[i] =
                        IntraPredictionMode::Angular10 == mode[i]
                        ? IntraPredictionMode::Angular34
                        : IntraPredictionMode::Angular10;
                    break;
                }
            case 3:
                {
                    mode[i] =
                        IntraPredictionMode::Dc == mode[i]
                        ? IntraPredictionMode::Angular34
                        : IntraPredictionMode::Dc;
                    break;
                }
            case 4:
                {
                    // modeIdx == IntraPredModeY
                    break;
                }
        }

        if(is422)
        {
            mode[i] = static_cast<IntraPredictionMode>(to422[int(mode[i])]);
        }

        log(LogId::Prediction, "Chroma ", cu.get<CU::Coord>()->inUnits(), ' ', getName(mode[i]), '\n');
    }

    return 1 == partNum ? CuIntraPredMode{mode[0]}: mode;
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Decoder::Processses */
