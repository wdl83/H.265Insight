#include <Syntax/MvdCoding.h>

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
/* MvdCoding */
/*----------------------------------------------------------------------------*/
void MvdCoding::onParse(
        StreamAccessLayer &streamAccessLayer, Decoder::State &decoder)
{
    auto absMvdGreater0Flag = embed<AbsMvdGreater0Flag>(*this);
    auto absMvdGreater1Flag = embed<AbsMvdGreater1Flag>(*this);
    auto absMvdMinus2 = embed<AbsMvdMinus2>(*this);
    auto mvdSignFlag = embed<MvdSignFlag>(*this);

    parse(streamAccessLayer, decoder, *absMvdGreater0Flag, Direction::H);
    parse(streamAccessLayer, decoder, *absMvdGreater0Flag, Direction::V);

    if((*absMvdGreater0Flag)[Direction::H])
    {
        parse(streamAccessLayer, decoder, *absMvdGreater1Flag, Direction::H);
    }

    if((*absMvdGreater0Flag)[Direction::V])
    {
        parse(streamAccessLayer, decoder, *absMvdGreater1Flag, Direction::V);
    }

    if((*absMvdGreater0Flag)[Direction::H])
    {
        if((*absMvdGreater1Flag)[Direction::H])
        {
            parse(streamAccessLayer, decoder, *absMvdMinus2, Direction::H);
        }

        parse(streamAccessLayer, decoder, *mvdSignFlag, Direction::H);
    }

    if((*absMvdGreater0Flag)[Direction::V])
    {
        if((*absMvdGreater1Flag)[Direction::V])
        {
            parse(streamAccessLayer, decoder, *absMvdMinus2, Direction::V);
        }

        parse(streamAccessLayer, decoder, *mvdSignFlag, Direction::V);
    }

    embed<LMvd>(*this, *absMvdGreater0Flag, *absMvdMinus2, *mvdSignFlag);
}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */
