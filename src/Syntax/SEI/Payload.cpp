#include <Syntax/SEI/Payload.h>
#include <Decoder/State.h>

namespace HEVC { namespace Syntax { namespace SEI {
/*----------------------------------------------------------------------------*/
namespace {
/*----------------------------------------------------------------------------*/
bool moreDataInPayload(
        StreamAccessLayer &streamAccessLayer,
        StreamAccessLayer::ConstIterator begin,
        int payloadSize)
{
    const auto diff = streamAccessLayer.curr() - begin;

    if(
            streamAccessLayer.isByteAligned()
            && 8 * payloadSize == diff)
    {
        return false;
    }

    return true;
}

bool payloadExtensionPresent(
        StreamAccessLayer &streamAccessLayer,
        StreamAccessLayer::ConstIterator begin,
        int payloadSize)
{
    const auto diff = streamAccessLayer.curr() - begin;

    if(8 * payloadSize - 1 != diff)
    {
        return true;
    }

    return false;
}

void discard(
        StreamAccessLayer &streamAccessLayer,
        int /*payloadType*/, int payloadSize)
{
    while(payloadSize)
    {
        streamAccessLayer.getByte();
        --payloadSize;
    }
#if 0
    std::cout
        << "SEI payload discarded type: " << std::dec << payloadType
        << " size: 0x" << std::hex << payloadSize << '\n';
#endif
}
/*----------------------------------------------------------------------------*/
} /* namespace */
/*----------------------------------------------------------------------------*/
void Payload::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
        NalUnitType nalUnitType, int payloadType, int payloadSize)
{
    embed<PayloadType>(*this, static_cast<PayloadId>(payloadType));

    const auto toStr =
        [nalUnitType, payloadType, payloadSize](std::ostream &oss)
        {
            oss
                << getName(nalUnitType) << ' '
                << std::dec << payloadType << " 0x"
                << std::hex << payloadSize << '\n';
        };

    log(LogId::SEI, toStr);

    const auto begin = streamAccessLayer.curr();

    if(NalUnitType::PREFIX_SEI_NUT == nalUnitType)
    {
        if(0 == payloadType)
        {
            //buffering_period(payloadSize)
            discard(streamAccessLayer, payloadType, payloadSize);
        }
        else if(1 == payloadType)
        {
            //pic_timing(payloadSize)
            discard(streamAccessLayer, payloadType, payloadSize);
        }
        else if(2 == payloadType)
        {
            //pan_scan_rect(payloadSize)
            discard(streamAccessLayer, payloadType, payloadSize);
        }
        else if(3 == payloadType)
        {
            //filler_payload(payloadSize)
            discard(streamAccessLayer, payloadType, payloadSize);
        }
        else if(4 == payloadType)
        {
            //user_data_registered_itu_t_t35(payloadSize)
            discard(streamAccessLayer, payloadType, payloadSize);
        }
        else if(5 == payloadType)
        {
            //user_data_unregistered(payloadSize)
            discard(streamAccessLayer, payloadType, payloadSize);
        }
        else if(6 == payloadType)
        {
            //recovery_point(payloadSize)
            discard(streamAccessLayer, payloadType, payloadSize);
        }
        else if(9 == payloadType)
        {
            //scene_info(payloadSize)
            discard(streamAccessLayer, payloadType, payloadSize);
        }
        else if(15 == payloadType)
        {
            //picture_snapshot(payloadSize)
            discard(streamAccessLayer, payloadType, payloadSize);
        }
        else if(16 == payloadType)
        {
            //progressive_refinement_segment_start(payloadSize)
            discard(streamAccessLayer, payloadType, payloadSize);
        }
        else if(17 == payloadType)
        {
            //progressive_refinement_segment_end(payloadSize)
            discard(streamAccessLayer, payloadType, payloadSize);
        }
        else if(19 == payloadType)
        {
            //film_grain_characteristics(payloadSize)
            discard(streamAccessLayer, payloadType, payloadSize);
        }
        else if(22 == payloadType)
        {
            //post_filter_hint(payloadSize)
            discard(streamAccessLayer, payloadType, payloadSize);
        }
        else if(23 == payloadType)
        {
            //tone_mapping_info(payloadSize)
            discard(streamAccessLayer, payloadType, payloadSize);
        }
        else if(45 == payloadType)
        {
            //frame_packing_arrangement(payloadSize)
            discard(streamAccessLayer, payloadType, payloadSize);
        }
        else if(47 == payloadType)
        {
            //display_orientation(payloadSize)
            discard(streamAccessLayer, payloadType, payloadSize);
        }
        else if(128 == payloadType)
        {
            //structure_of_pictures_info(payloadSize)
            discard(streamAccessLayer, payloadType, payloadSize);
        }
        else if(129 == payloadType)
        {
            //active_parameter_sets(payloadSize)
            discard(streamAccessLayer, payloadType, payloadSize);
        }
        else if(130 == payloadType)
        {
            //decoding_unit_info(payloadSize)
            discard(streamAccessLayer, payloadType, payloadSize);
        }
        else if(131 == payloadType)
        {
            //temporal_sub_layer_zero_index(payloadSize)
            discard(streamAccessLayer, payloadType, payloadSize);
        }
        else if(133 == payloadType)
        {
            //scalable_nesting(payloadSize)
            discard(streamAccessLayer, payloadType, payloadSize);
        }
        else if(134 == payloadType)
        {
            //region_refresh_info(payloadSize)
            discard(streamAccessLayer, payloadType, payloadSize);
        }
        else
        {
            //reserved_sei_message(payloadSize)
            discard(streamAccessLayer, payloadType, payloadSize);
        }
    }
    else // if(NalUnitType::SUFFIX_SEI_NUT == nalUnitType)
    {
        if(3 == payloadType)
        {
            //filler_payload(payloadSize)
            discard(streamAccessLayer, payloadType, payloadSize);
        }
        else if(4 == payloadType)
        {
            //user_data_registered_itu_t_t35(payloadSize)
            discard(streamAccessLayer, payloadType, payloadSize);
        }
        else if(5 == payloadType)
        {
            //user_data_unregistered(payloadSize)
            discard(streamAccessLayer, payloadType, payloadSize);
        }
        else if(17 == payloadType)
        {
            //progressive_refinement_segment_end(payloadSize)
            discard(streamAccessLayer, payloadType, payloadSize);
        }
        else if(22 == payloadType)
        {
            //post_filter_hint(payloadSize)
            discard(streamAccessLayer, payloadType, payloadSize);
        }
        else if(int(PayloadId::decoded_picture_hash) == payloadType)
        {
            if(decoder.cpb.picture())
            {
                parse(streamAccessLayer, decoder, *embedSubtree<DecodedPictureHash>(*this));
            }
            else
            {
                discard(streamAccessLayer, payloadType, payloadSize);
            }
        }
        else
        {
            //reserved_sei_message(payloadSize)
            discard(streamAccessLayer, payloadType, payloadSize);
        }
    }

    if(moreDataInPayload(streamAccessLayer, begin, payloadSize))
    {
        if(payloadExtensionPresent(streamAccessLayer, begin, payloadSize))
        {
            parse(streamAccessLayer, decoder, *embed<ReservedPayloadExtensionData>(*this));
        }

        parse(streamAccessLayer, decoder, *embed<PayloadBitEqualToOne>(*this));

        if(!streamAccessLayer.isByteAligned())
        {
            auto paylaodBitEqualToZero = embed<PaylaodBitEqualToZero>(*this);

            while(!streamAccessLayer.isByteAligned())
            {
                parse(streamAccessLayer, decoder, *paylaodBitEqualToZero);
            }
        }
    }
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Syntax::SEI */
