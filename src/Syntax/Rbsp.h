#ifndef HEVC_Syntax_Rbsp_h
#define HEVC_Syntax_Rbsp_h

/* HEVC */
#include <Syntax/Syntax.h>

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
bool moreRbspData(const StreamAccessLayer &streamAccessLayer);
bool moreRbspTrailingData(const StreamAccessLayer &streamAccessLayer);
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */

#endif /* HEVC_Syntax_Rbsp_h */
