#include <Syntax/ProfileTierLevel.h>

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
void ProfileTierLevel::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
        bool profilePresentFlag,
        int maxNumSubLayersMinus1)
{
    auto generalProfileSpace = embed<GeneralProfileSpace>(*this);
    auto generalTierFlag = embed<GeneralTierFlag>(*this);
    auto generalProfileIdc = embed<GeneralProfileIdc>(*this);
    auto generalProfileCompatibilityFlag = embed<GeneralProfileCompatibilityFlag>(*this);
    auto generalProgressiveSourceFlag = embed<GeneralProgressiveSourceFlag>(*this);
    auto generalInterlacedSourceFlag = embed<GeneralInterlacedSourceFlag>(*this);
    auto generalNonPackedConstraintFlag = embed<GeneralNonPackedConstraintFlag>(*this);
    auto generalFrameOnlyConstraintFlag = embed<GeneralFrameOnlyConstraintFlag>(*this);
    auto generalLevelIdc = embed<GeneralLevelIdc>(*this);

    if(profilePresentFlag)
    {
        parse(streamAccessLayer, decoder, *generalProfileSpace);
        parse(streamAccessLayer, decoder, *generalTierFlag);
        parse(streamAccessLayer, decoder, *generalProfileIdc);

        for(auto i = 0; i < GeneralProfileCompatibilityFlag::num; ++i)
        {
            parse(streamAccessLayer, decoder, *generalProfileCompatibilityFlag, i);
        }

        parse(streamAccessLayer, decoder, *generalProgressiveSourceFlag);
        parse(streamAccessLayer, decoder, *generalInterlacedSourceFlag);
        parse(streamAccessLayer, decoder, *generalNonPackedConstraintFlag);
        parse(streamAccessLayer, decoder, *generalFrameOnlyConstraintFlag);

        if(
                4 == generalProfileIdc->inUnits()
                || 5 == generalProfileIdc->inUnits()
                || 6 == generalProfileIdc->inUnits()
                || 7 == generalProfileIdc->inUnits()
                || 8 == generalProfileIdc->inUnits()
                || 9 == generalProfileIdc->inUnits()
                || 10 == generalProfileIdc->inUnits()
                || (*generalProfileCompatibilityFlag)[4]
                || (*generalProfileCompatibilityFlag)[5]
                || (*generalProfileCompatibilityFlag)[6]
                || (*generalProfileCompatibilityFlag)[7]
                || (*generalProfileCompatibilityFlag)[8]
                || (*generalProfileCompatibilityFlag)[9]
                || (*generalProfileCompatibilityFlag)[10])
        {
            parse(streamAccessLayer, decoder, *embed<GeneralMax12BitConstraintFlag>(*this));
            parse(streamAccessLayer, decoder, *embed<GeneralMax10BitConstraintFlag>(*this));
            parse(streamAccessLayer, decoder, *embed<GeneralMax8BitConstraintFlag>(*this));
            parse(streamAccessLayer, decoder, *embed<GeneralMax422ChromaConstraintFlag>(*this));
            parse(streamAccessLayer, decoder, *embed<GeneralMax420ChromaConstraintFlag>(*this));
            parse(streamAccessLayer, decoder, *embed<GeneralMaxMonochromeConstraintFlag>(*this));
            parse(streamAccessLayer, decoder, *embed<GeneralIntraConstraintFlag>(*this));
            parse(streamAccessLayer, decoder, *embed<GeneralOnePictureOnlyConstraintFlag>(*this));
            parse(streamAccessLayer, decoder, *embed<GeneralLowerBitRateConstraintFlag>(*this));

            if(
                    5 == generalProfileIdc->inUnits()
                    || 9 == generalProfileIdc->inUnits()
                    || 10 == generalProfileIdc->inUnits()
                    || (*generalProfileCompatibilityFlag)[5]
                    || (*generalProfileCompatibilityFlag)[9]
                    || (*generalProfileCompatibilityFlag)[10])
            {
                parse(streamAccessLayer, decoder, *embed<GeneralMax14BitConstraintFlag>(*this));
                parse(streamAccessLayer, decoder, *embed<GeneralReservedZero33Bits>(*this));
            }
            else
            {
                parse(streamAccessLayer, decoder, *embed<GeneralReservedZero34Bits>(*this));
            }
        }
        else
        {
            parse(streamAccessLayer, decoder, *embed<GeneralReservedZero43Bits>(*this));
        }

        if(
                (1 <= generalProfileIdc->inUnits() && 5 >= generalProfileIdc->inUnits())
                || 9 == generalProfileIdc->inUnits()
                || (*generalProfileCompatibilityFlag)[1]
                || (*generalProfileCompatibilityFlag)[2]
                || (*generalProfileCompatibilityFlag)[3]
                || (*generalProfileCompatibilityFlag)[4]
                || (*generalProfileCompatibilityFlag)[5]
                || (*generalProfileCompatibilityFlag)[9])
        {
            parse(streamAccessLayer, decoder, *embed<GeneralInbldFlag>(*this));
        }
        else
        {
            parse(streamAccessLayer, decoder, *embed<GeneralReservedZeroBit>(*this));
        }
    }

    parse(streamAccessLayer, decoder, *generalLevelIdc);

    if(0 < maxNumSubLayersMinus1)
    {
        auto subLayerProfilePresentFlag = embed<SubLayerProfilePresentFlag>(*this);
        auto subLayerLevelPresentFlag = embed<SubLayerLevelPresentFlag>(*this);

        for(auto i = 0; i < maxNumSubLayersMinus1; ++i)
        {
            parse(streamAccessLayer, decoder, *subLayerProfilePresentFlag, i);
            parse(streamAccessLayer, decoder, *subLayerLevelPresentFlag, i);
        }

        if(8 > maxNumSubLayersMinus1)
        {
            auto reservedZero2Bits = embed<ReservedZero2Bits>(*this);

            for(auto i = maxNumSubLayersMinus1; 8 > i; ++i)
            {
                parse(streamAccessLayer, decoder, *reservedZero2Bits);
            }
        }

        auto subLayerProfileSpace = embed<SubLayerProfileSpace>(*this);
        auto subLayerTierFlag = embed<SubLayerTierFlag>(*this);
        auto subLayerProfileIdc = embed<SubLayerProfileIdc>(*this);
        auto subLayerProfileCompatibilityFlag = embed<SubLayerProfileCompatibilityFlag>(*this);
        auto subLayerProgressiveSourceFlag = embed<SubLayerProgressiveSourceFlag>(*this);
        auto subLayerInterlacedSourceFlag = embed<SubLayerInterlacedSourceFlag>(*this);
        auto subLayerNonPackedConstraintFlag = embed<SubLayerNonPackedConstraintFlag>(*this);
        auto subLayerFrameOnlyConstraintFlag = embed<SubLayerFrameOnlyConstraintFlag>(*this);
        auto subLayerMax12BitConstraintFlag = embed<SubLayerMax12BitConstraintFlag>(*this);
        auto subLayerMax10BitConstraintFlag = embed<SubLayerMax10BitConstraintFlag>(*this);
        auto subLayerMax8BitConstraintFlag = embed<SubLayerMax8BitConstraintFlag>(*this);
        auto subLayerMax422ChromaConstraintFlag = embed<SubLayerMax422ChromaConstraintFlag>(*this);
        auto subLayerMax420ChromaConstraintFlag = embed<SubLayerMax420ChromaConstraintFlag>(*this);
        auto subLayerMaxMonochromeConstraintFlag = embed<SubLayerMaxMonochromeConstraintFlag>(*this);
        auto subLayerIntraConstraintFlag = embed<SubLayerIntraConstraintFlag>(*this);
        auto subLayerOnePictureOnlyConstraintFlag = embed<SubLayerOnePictureOnlyConstraintFlag>(*this);
        auto subLayerLowerBitRateConstraintFlag = embed<SubLayerLowerBitRateConstraintFlag>(*this);
        auto subLayerMax14BitConstraintFlag = embed<SubLayerMax14BitConstraintFlag>(*this);
        auto subLayerReservedZero33Bits = embed<SubLayerReservedZero33Bits>(*this);
        auto subLayerReservedZero34Bits = embed<SubLayerReservedZero34Bits>(*this);
        auto subLayerReservedZero43Bits = embed<SubLayerReservedZero43Bits>(*this);
        auto subLayerInbldFlag = embed<SubLayerInbldFlag>(*this);
        auto subLayerReservedZeroBit = embed<SubLayerReservedZeroBit>(*this);
        auto subLayerLevelIdc = embed<SubLayerLevelIdc>(*this);

        for(auto i = 0; i < maxNumSubLayersMinus1; ++i)
        {
            if((*subLayerProfilePresentFlag)[i])
            {
                parse(streamAccessLayer, decoder, *subLayerProfileSpace, i);
                parse(streamAccessLayer, decoder, *subLayerTierFlag, i);
                parse(streamAccessLayer, decoder, *subLayerProfileIdc, i);

                for(auto j = 0; j < GeneralProfileCompatibilityFlag::num; ++j)
                {
                    parse(streamAccessLayer, decoder, *subLayerProfileCompatibilityFlag, i, j);
                }

                parse(streamAccessLayer, decoder, *subLayerProgressiveSourceFlag, i);
                parse(streamAccessLayer, decoder, *subLayerInterlacedSourceFlag, i);
                parse(streamAccessLayer, decoder, *subLayerNonPackedConstraintFlag, i);
                parse(streamAccessLayer, decoder, *subLayerFrameOnlyConstraintFlag, i);

                if(
                        4 == (*subLayerProfileIdc)[i]
                        || 5 == (*subLayerProfileIdc)[i]
                        || 6 == (*subLayerProfileIdc)[i]
                        || 7 == (*subLayerProfileIdc)[i]
                        || 8 == (*subLayerProfileIdc)[i]
                        || 9 == (*subLayerProfileIdc)[i]
                        || 10 == (*subLayerProfileIdc)[i]
                        || (*subLayerProfileCompatibilityFlag)[std::make_pair(i, 4)]
                        || (*subLayerProfileCompatibilityFlag)[std::make_pair(i, 5)]
                        || (*subLayerProfileCompatibilityFlag)[std::make_pair(i, 6)]
                        || (*subLayerProfileCompatibilityFlag)[std::make_pair(i, 7)]
                        || (*subLayerProfileCompatibilityFlag)[std::make_pair(i, 8)]
                        || (*subLayerProfileCompatibilityFlag)[std::make_pair(i, 9)]
                        || (*subLayerProfileCompatibilityFlag)[std::make_pair(i, 10)])
                {
                    parse(streamAccessLayer, decoder, *subLayerMax12BitConstraintFlag, i);
                    parse(streamAccessLayer, decoder, *subLayerMax10BitConstraintFlag, i);
                    parse(streamAccessLayer, decoder, *subLayerMax8BitConstraintFlag, i);
                    parse(streamAccessLayer, decoder, *subLayerMax422ChromaConstraintFlag, i);
                    parse(streamAccessLayer, decoder, *subLayerMax420ChromaConstraintFlag, i);
                    parse(streamAccessLayer, decoder, *subLayerMaxMonochromeConstraintFlag, i);
                    parse(streamAccessLayer, decoder, *subLayerIntraConstraintFlag, i);
                    parse(streamAccessLayer, decoder, *subLayerOnePictureOnlyConstraintFlag, i);
                    parse(streamAccessLayer, decoder, *subLayerLowerBitRateConstraintFlag, i);

                    if(
                            5 == (*subLayerProfileIdc)[i]
                            || (*subLayerProfileCompatibilityFlag)[std::make_pair(i, 5)])
                    {
                        parse(streamAccessLayer, decoder, *subLayerMax14BitConstraintFlag);
                        parse(streamAccessLayer, decoder, *subLayerReservedZero33Bits);
                    }
                    else
                    {
                        parse(streamAccessLayer, decoder, *subLayerReservedZero34Bits);
                    }
                }
                else
                {
                    parse(streamAccessLayer, decoder, *subLayerReservedZero43Bits);
                }

                if(
                        (1 <= (*subLayerProfileIdc)[i] && 5 >= (*subLayerProfileIdc)[i])
                        || 9 == (*subLayerProfileIdc)[i]
                        || (*subLayerProfileCompatibilityFlag)[std::make_pair(i, 1)]
                        || (*subLayerProfileCompatibilityFlag)[std::make_pair(i, 2)]
                        || (*subLayerProfileCompatibilityFlag)[std::make_pair(i, 3)]
                        || (*subLayerProfileCompatibilityFlag)[std::make_pair(i, 4)]
                        || (*subLayerProfileCompatibilityFlag)[std::make_pair(i, 5)]
                        || (*subLayerProfileCompatibilityFlag)[std::make_pair(i, 9)])
                {
                    parse(streamAccessLayer, decoder, *subLayerInbldFlag, i);
                }
                else
                {
                    parse(streamAccessLayer, decoder, *subLayerReservedZeroBit);
                }
            }

            if((*subLayerLevelPresentFlag)[i])
            {
                parse(streamAccessLayer, decoder, *subLayerLevelIdc, i);
            }
        }
    }
}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */
