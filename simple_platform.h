#ifndef __CSIMPLE_PLATFORM_H
#define __CSIMPLE_PLATFORM_H

#if defined(_MSC_VER) || defined(WIN32) || defined(_WIN32)
#define _MSWINDOWS_
#include <sdkddkver.h>
#if _WIN32_WINNT >= 0x0600 
#define _AFTER_VISTA
#endif
#endif

#endif

