#include <Syntax/Rbsp.h>
#include <StreamAccessLayer.h>

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
bool moreRbspData(const StreamAccessLayer &streamAccessLayer)
{
    if(!streamAccessLayer.isEndOfStream())
    {
        const auto rbspTailBitsNum = streamAccessLayer.getSizeInBits();

        /* search for rbsp_trailing_bits */

        bool rbspStopOneBitPresent = false;
        size_t alignmentZeroBitNum = 0;

        for(const auto bit : streamAccessLayer)
        {
            if(bit)
            {
                /* rbsp_stop_one_bit */
                rbspStopOneBitPresent = true;
                alignmentZeroBitNum = 0;
            }
            else
            {
                /* rbsp_alignment_zero_bit */
                if(rbspStopOneBitPresent)
                {
                    ++alignmentZeroBitNum;
                }
            }
        }

        if(rbspStopOneBitPresent)
        {
            return
                rbspTailBitsNum
                - (1 /* rbsp_stop_one_bit */ + alignmentZeroBitNum);
        }
        else
        {
            syntaxCheck(false);
        }
    }

    return false;
}
/*----------------------------------------------------------------------------*/
bool moreRbspTrailingData(const StreamAccessLayer &streamAccessLayer)
{
    if(streamAccessLayer.isEndOfStream())
    {
        return false;
    }

    return true;
}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */
