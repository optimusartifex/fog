// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

#ifndef _FOG_CORE_CPP_STDHEADERS_H
#define _FOG_CORE_CPP_STDHEADERS_H

#if !defined(FOG_IDE) && !defined(_FOG_CORE_CPP_BASE_H)
#error "Fog/Core/C++/StdHeaders.h can be only included by Fog/Core/C++/Base.h"
#endif // _FOG_CORE_CPP_BASE_H

// ============================================================================
// [Fog::Core::C++ - Standard Header Files]
// ============================================================================

// We need Win2000/XP+
#if defined(FOG_OS_WINDOWS)
# ifndef _WIN32_WINNT
#  define _WIN32_WINNT 0x500
# endif
# include <windows.h>
#endif

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(FOG_HAVE_STDARG_H)
# include <stdarg.h>
#endif // FOG_HAVE_STDARG_H

#if defined(FOG_HARDCODE_MMX)
# include <Fog/Core/C++/IntrinMMX.h>
#endif // FOG_HARDCODE_MMX

#if defined(FOG_HARDCODE_SSE)
# include <Fog/Core/C++/IntrinSSE.h>
#endif // FOG_HARDCODE_SSE

#if defined(FOG_HARDCODE_SSE2)
# include <Fog/Core/C++/IntrinSSE2.h>
#endif // FOG_HARDCODE_SSE2

#if defined(FOG_HARDCODE_SSE3)
# include <Fog/Core/C++/IntrinSSE3.h>
#endif // FOG_HARDCODE_SSE3

#if defined(FOG_HARDCODE_SSSE3)
# include <Fog/Core/C++/IntrinSSSE3.h>
#endif // FOG_HARDCODE_SSSE3

#endif // _FOG_CORE_CPP_STDHEADERS_H
