#ifndef HEVC_Syntax_CABAD_Init_h
#define HEVC_Syntax_CABAD_Init_h

/* STDC++ */
#include <cstdint>
#include <string>
#include <vector>
#include <map>
/* HEVC */
#include <Syntax/check.h>
#include <Syntax/CABAD/CtxId.h>

namespace HEVC { namespace Syntax { namespace CABAD {
/*----------------------------------------------------------------------------*/
enum InitType
{
    InitType_Undefined = -1,
    InitType_0 = 0,
    InitType_1,
    InitType_2,
    InitType_Num
};

const std::string &getName(InitType initType);

typedef uint32_t InitValue;
typedef std::vector<InitValue> InitValueTable;

class InitParam
{
private:
    const InitType m_initType;
    const InitValueTable m_initValueTable;
public:
    InitParam(
            InitType initType,
            const std::initializer_list<InitValue> &initValueTable):
        m_initType(initType),
        m_initValueTable(initValueTable)
    {}

    InitType getInitType() const
    {
        return m_initType;
    }

    const InitValueTable &getInitValueTable() const
    {
        return m_initValueTable;
    }
};

typedef std::vector<InitParam> InitParamTable;
typedef std::map<CtxId, InitParamTable> InitParamTables;

const InitParamTables &getInitParamTables();
const InitParamTable &getInitParamTable(CtxId id);
const InitValueTable &getInitValueTable(CtxId id, InitType initType);
bool isInitTypeSupported(CtxId id, InitType initType);
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Syntax::CABAD */

#endif /* HEVC_Syntax_CABAD_Init_h */
