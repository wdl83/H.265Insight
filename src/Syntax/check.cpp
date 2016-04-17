#include <Syntax/check.h>
#include <runtime_assert.h>

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/

void syntaxCheck_impl(const char *cond, const char *file, int line, const char *pfunc)
{
    runtime_assert_impl(cond, file, line, pfunc);
}
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */
