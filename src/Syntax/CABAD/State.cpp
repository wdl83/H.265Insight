/* STDC++ */
#include <algorithm>
#include <sstream>
#include <iomanip>
/* HEVC */
#include <Syntax/CABAD/State.h>
#include <utils.h>
#include <log.h>

namespace HEVC { namespace Syntax { namespace CABAD {
/*----------------------------------------------------------------------------*/
/* State */
/*----------------------------------------------------------------------------*/
State::State():
    initType(InitType_Undefined),
    sliceQpY(0)
{
    std::fill(std::begin(statCoeff), std::end(statCoeff), 0);
}
/*----------------------------------------------------------------------------*/
State::State(const State &other):
    initType{other.initType},
    sliceQpY{other.sliceQpY},
    arithmeticDecoder{other.arithmeticDecoder},
    statCoeff(other.statCoeff),
    m_ctxTables(other.m_ctxTables)
{}
/*----------------------------------------------------------------------------*/
State & State::operator=(const State &other)
{
    initType = other.initType;
    sliceQpY = other.sliceQpY;
    arithmeticDecoder = other.arithmeticDecoder;
    statCoeff = other.statCoeff;
    m_ctxTables = other.m_ctxTables;
    return *this;
}
/*----------------------------------------------------------------------------*/
void State::init(StreamAccessLayer &streamAccessLayer, InitType initType_, int qpY)
{

    log(
            LogId::ArithmeticDecoderState,
            "CABAD::State::init(initType ", initType, " sliceQpY ", sliceQpY, ")\n");

    initType = initType_;
    sliceQpY = qpY;
    arithmeticDecoder.init(streamAccessLayer);
    std::fill(std::begin(statCoeff), std::end(statCoeff), 0);
    m_ctxTables.clear();
}
/*----------------------------------------------------------------------------*/
void State::initCtxTable(CtxId id)
{
    const auto &initValueTable = getInitValueTable(id, initType);
    auto ctxTable = CtxTable(initValueTable.size());

    for(size_t i = 0; i < ctxTable.size(); ++i)
    {
        ctxTable[i].init(initValueTable[i], sliceQpY);
    }

    m_ctxTables[id] = ctxTable;
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Syntax::CABAD */
