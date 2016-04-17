#ifndef HEVC_Syntax_CABAD_State_h
#define HEVC_Syntax_CABAD_State_h

/* STDC++ */
#include <vector>
#include <array>
#include <memory>
/* HEVC */
#include <Fwd.h>
#include <Syntax/CABAD/CtxId.h>
#include <Syntax/CABAD/Init.h>
#include <Syntax/CABAD/Variable.h>
#include <Syntax/CABAD/ArithmeticDecoder.h>

namespace HEVC { namespace Syntax { namespace CABAD {
/*----------------------------------------------------------------------------*/
class State
{
public:
    typedef std::vector<Variable> CtxTable;
    typedef std::array<int, 4> StatCoeff;

    InitType initType;
    int sliceQpY;
    ArithmeticDecoder arithmeticDecoder;
    StatCoeff statCoeff;
private:
    class CtxTables
    {
    private:
        std::array<CtxTable, EnumRange<CtxId>::length()> m_tables;
    public:
        void clear()
        {
            for(auto &i : m_tables)
            {
                i.clear();
            }
        }

        const CtxTable &operator[] (CtxId id) const
        {
            return m_tables[int(id)];
        }

        CtxTable &operator[] (CtxId id)
        {
            return m_tables[int(id)];
        }
    };

    CtxTables m_ctxTables;
public:
    State();
    State(const State &other);
    State &operator= (const State &other);

    void init( StreamAccessLayer &, InitType, int);

    Variable &getVariable(CtxId id, int ctxInc)
    {
        auto &ctxTable = getCtxTable(id);

        syntaxCheck(int(ctxTable.size()) > ctxInc);
        return ctxTable[ctxInc];
    }
private:
    void initCtxTable(CtxId id);

    CtxTable &getCtxTable(CtxId id)
    {
        if(m_ctxTables[id].empty())
        {
            initCtxTable(id);
        }

        return m_ctxTables[id];
    }
};
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Syntax::CABAD */

#endif /* HEVC_Syntax_CABAD_State_h */
