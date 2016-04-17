#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <runtime_assert.h>
#include <TerminationTools.h>
#include <log.h>

/*----------------------------------------------------------------------------*/
namespace {

void assert_impl(const char *cond, const char *file, int line, const char *pfunc)
{
    std::fprintf(stderr, "\n%s:%d: %s: assertion \'%s\' failed\n", file, line, pfunc, cond);
    HEVC::flush(HEVC::LogId::All);
    std::abort();
}

} /* namespace */

void runtime_assert_impl(const char *cond, const char *file, int line, const char *pfunc)
{
    assert_impl(cond, file, line, pfunc);
}
/*----------------------------------------------------------------------------*/
#ifdef PTR_CHECK

void ptrCheck_impl(const char *cond, const char *file, int line, const char *pfunc)
{
    assert_impl(cond, file, line, pfunc);
}

#endif
/*----------------------------------------------------------------------------*/
#ifdef BDRY_CHECK

void bdryCheck_impl(const char *cond, const char *file, int line, const char *pfunc)
{
    assert_impl(cond, file, line, pfunc);
}

#endif
/*----------------------------------------------------------------------------*/
#ifdef NUMERIC_CHECK

void numericCheck_impl(const char *cond, const char *file, int line, const char *pfunc)
{
    assert_impl(cond, file, line, pfunc);
}

#endif
/*----------------------------------------------------------------------------*/
