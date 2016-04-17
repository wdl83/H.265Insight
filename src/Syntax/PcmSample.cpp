#include <Syntax/PcmSample.h>
#include <Structure/Picture.h>
#include <Decoder/State.h>

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
void PcmSample::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder)
{
    /* 04/2013, 7.3.8.7 "PCM sample syntax" */
    auto picture = decoder.picture();
    const auto chromaFormatIdc = picture->chromaFormatIdc;
    const auto bitDepthY = picture->bitDepth(Component::Luma);
    const auto bitDepthC = picture->bitDepth(Component::Chroma);
    const auto pcmBitDepthY = picture->bitDepthPCM(Component::Luma);
    const auto pcmBitDepthC = picture->bitDepthPCM(Component::Chroma);
    const auto bitDepthDiffY = bitDepthY - pcmBitDepthY;
    const auto bitDepthDiffC = bitDepthC - pcmBitDepthC;

    /* 04/2013, 7.4.9.7 "PCM sample semantics" */
    const auto extractY =
        [bitDepthDiffY, pcmBitDepthY](PcmSampleLuma::ValueType value)
        {
            return Sample(mask<decltype(value)>(value, pcmBitDepthY) << bitDepthDiffY);
        };

    const auto extractC =
        [bitDepthDiffC, pcmBitDepthC](PcmSampleChroma::ValueType value)
        {
            return Sample(mask<decltype(value)>(value, pcmBitDepthC) << bitDepthDiffC);
        };

    const LogId logs[] =
    {
        LogId::PcmSamplesY, LogId::PcmSamplesCb, LogId::PcmSamplesCr
    };

    const auto cuCoord = get<Coord>()->inUnits();
    const auto cuSize = get<Size>()->inUnits();

    using namespace Structure;

    auto pcmSampleLuma = embed<PcmSampleLuma>(*this, pcmBitDepthY);
    auto pcmSampleChroma = embed<PcmSampleChroma>(*this, pcmBitDepthC);

    for(const auto plane : EnumRange<Plane>())
    {
        if(!isPresent(plane, chromaFormatIdc))
        {
            continue;
        }

        const auto coord = scale(cuCoord, plane, chromaFormatIdc);
        const auto hSide = hScale(toPel(cuSize), plane, chromaFormatIdc);
        const auto vSide = vScale(toPel(cuSize), plane, chromaFormatIdc);
        auto &dst = picture->pelBuffer(PelLayerId::Reconstructed, plane);

        for(auto y = 0_pel; y < vSide; ++y)
        {
            for(auto x = 0_pel; x < hSide; ++x)
            {
                const PelCoord offset{x, y};
                const auto at = coord + offset;

                if(Plane::Y == plane)
                {
                    parse(streamAccessLayer, decoder, *pcmSampleLuma);
                    dst[at] = extractY(pcmSampleLuma->getValue());
                }
                else //if(Plane::Cb == plane || Plane::Cr == plane)
                {
                    parse(streamAccessLayer, decoder, *pcmSampleChroma);
                    dst[at] = extractC(pcmSampleChroma->getValue());
                }
            }
        }

        const auto toStr =
            [&dst, coord, hSide, vSide](std::ostream &oss)
            {
                oss << coord << '\n';
                dst.toStr(oss, {coord, hSide, vSide});
            };

        log(logs[int(plane)], toStr);
    }

    /* 04/2013,
     * 9.3 "CABAC parsing process for slice segment data",
     * 9.3.1 "General"
     *
     * "the decoding engine is initialized after the decoding of any
     * pcm_alignment_zero_bit and all pcm_sample_luma and pcm_sample_chroma
     * data as specified in clause 9.3.2.5 */
    decoder.picture()->getCabadState().arithmeticDecoder.init(streamAccessLayer);
}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */
