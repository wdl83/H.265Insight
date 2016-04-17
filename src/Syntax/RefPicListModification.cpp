#include <Syntax/RefPicListModification.h>
#include <Syntax/SliceSegmentHeader.h>
#include <Structure/RPSP.h>

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
void RefPicListModification::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder,
        const SliceSegmentHeader &sh,
        const Structure::RPSP &rpsp)
{
    typedef SliceSegmentHeader SSH;

    auto refPicListModificationFlagL0 = embed<RefPicListModificationFlagL0>(*this);
    auto listEntryL0 = embed<ListEntryL0>(*this);
    auto refPicListModificationFlagL1 = embed<RefPicListModificationFlagL1>(*this);
    auto listEntryL1 = embed<ListEntryL1>(*this);

    const auto numPocTotalCurr = rpsp.numPocTotalCurr;
    syntaxCheck(numPocTotalCurr);

    parse(streamAccessLayer, decoder, *refPicListModificationFlagL0);

    if(*refPicListModificationFlagL0)
    {
        const auto numRefIdxL0ActiveMinus1 = sh.get<SSH::NumRefIdxL0ActiveMinus1>();

        for (auto i = 0; i <= *numRefIdxL0ActiveMinus1; ++i)
        {
            parse(streamAccessLayer, decoder, *listEntryL0, numPocTotalCurr, i);
        }
    }

    if(isB(*sh.get<SSH::SliceType>()))
    {
        parse(streamAccessLayer, decoder, *refPicListModificationFlagL1);

        if (*refPicListModificationFlagL1)
        {
            const auto numRefIdxL1ActiveMinus1 = sh.get<SSH::NumRefIdxL1ActiveMinus1>();

            for (auto i = 0; i <= *numRefIdxL1ActiveMinus1; ++i)
            {
                parse(streamAccessLayer, decoder, *listEntryL1, numPocTotalCurr, i);
            }
        }
    }
}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */

