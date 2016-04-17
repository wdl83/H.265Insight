#ifndef runtime_assert_h
#define runtime_assert_h

#include <cstdarg>

void runtime_assert_impl(const char *cond, const char *file, int line, const char *pfunc);

#define runtime_assert(cond)\
{\
    if(!(cond))\
    { \
        runtime_assert_impl(__STRING(cond), __FILE__, __LINE__,  __PRETTY_FUNCTION__);\
    }\
}\
/*----------------------------------------------------------------------------*/
#ifdef PTR_CHECK

void ptrCheck_impl(const char *cond, const char *file, int line, const char *pfunc);

#define ptrCheck(cond)\
{\
    if(!(cond))\
    { \
        ptrCheck_impl(__STRING(cond), __FILE__, __LINE__,  __PRETTY_FUNCTION__);\
    }\
}\

#else

#define ptrCheck(cond)

#endif
/*----------------------------------------------------------------------------*/
#ifdef BDRY_CHECK

void bdryCheck_impl(const char *cond, const char *file, int line, const char *pfunc);

#define bdryCheck(cond)\
{\
    if(!(cond))\
    { \
        bdryCheck_impl(__STRING(cond), __FILE__, __LINE__,  __PRETTY_FUNCTION__);\
    }\
}\

#else

#define bdryCheck(cond)

#endif
/*----------------------------------------------------------------------------*/
#ifdef NUMERIC_CHECK

void numericCheck_impl(const char *cond, const char *file, int line, const char *pfunc);

#define numericCheck(cond)\
{\
    if(!(cond))\
    { \
        numericCheck_impl(__STRING(cond), __FILE__, __LINE__,  __PRETTY_FUNCTION__);\
    }\
}\

#else

#define numericCheck(cond)

#endif
/*----------------------------------------------------------------------------*/

#endif // runtime_assert_h
