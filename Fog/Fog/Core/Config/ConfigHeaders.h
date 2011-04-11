// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

#ifndef _FOG_CORE_CONFIG_CONFIGHEADERS_H
#define _FOG_CORE_CONFIG_CONFIGHEADERS_H

#if !defined(FOG_IDE) && !defined(_FOG_CORE_CONFIG_CONFIG_H)
#error "Fog::Core::Config - ConfigHeaders.h can be only included by Fog/Core/Config/Config.h"
#endif // _FOG_CORE_CONFIG_CONFIG_H

// ============================================================================
// [Fog::Core::Build - Standard Header Files]
// ============================================================================

#if defined(FOG_OS_WINDOWS)
# define _WIN32_WINNT 0x500 // We need Win2000/XP+
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
# include <Fog/Core/Cpu/Intrin_MMX.h>
#endif // FOG_HARDCODE_MMX

#if defined(FOG_HARDCODE_SSE)
# include <Fog/Core/Cpu/Intrin_SSE.h>
#endif // FOG_HARDCODE_SSE

#if defined(FOG_HARDCODE_SSE2)
# include <Fog/Core/Cpu/Intrin_SSE2.h>
#endif // FOG_HARDCODE_SSE2

// ============================================================================
// [Fog::Core::Build - strcasecmp / strncasecmp]
// ============================================================================

// Sorry for undocumented porting things here, but I will place these functions
// here instead of creating #ifdefs in code.
#if defined(FOG_OS_WINDOWS)
// unix like
# define strncasecmp _strnicmp
# define strcasecmp _stricmp
#endif // FOG_OS_WINDOWS

#endif // _FOG_CORE_CONFIG_CONFIGHEADERS_H
