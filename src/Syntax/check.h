#ifndef HEVC_Syntax_check_h
#define HEVC_Syntax_check_h

#ifndef NO_DEBUG

#define syntaxCheck(cond)\
{\
    if(!(cond))\
    { \
        syntaxCheck_impl(__STRING(cond), __FILE__, __LINE__,  __PRETTY_FUNCTION__);\
    }\
}\

#else

#define syntaxCheck(cond)

#endif

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
void syntaxCheck_impl(const char *cond, const char *file, int line, const char *pfunc);
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */

#endif /* HEVC_Syntax_check_h */
