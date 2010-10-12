// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

#ifndef _FOG_CORE_BUILD_STDARCH_H
#define _FOG_CORE_BUILD_STDARCH_H

#if !defined(FOG_IDE) && !defined(_FOG_CORE_BUILD_H)
#error "Fog::Core::Build - StdArch.h can be only included by Fog/Core/Build.h"
#endif // _FOG_CORE_BUILD_H

// ============================================================================
// [Fog::Core::Build - Architecture]
// ============================================================================

//! @addtogroup Fog_Core_Compiler
//! @{

//! @def FOG_ARCH_X86
//! @brief x86 compatible, can be i386 or higher.

//! @def FOG_ARCH_X86_64
//! @brief based on 64 bit intel or 64 bit AMD processor.

//! @def FOG_ARCH_PPC
//! @brief 32 bit powerPC.

//! @def FOG_ARCH_BITS
//! @brief How bits has target CPU (32 or 64).
//!
//! Very nice method to check whether it's faster to use 64 bit arithmetics.

//! @def FOG_HARDCODE_MMX
//! @brief If defined, MMX assembly will be hardcoded into binaries and no lower optimizations are allowed.

//! @def FOG_HARDCODE_MMX2
//! @brief If defined, MMX2 assembly will be hardcoded into binaries and no lower optimizations are allowed.

//! @def FOG_HARDCODE_SSE
//! @brief If defined, SSE assembly will be hardcoded into binaries and no lower optimizations are allowed.

//! @def FOG_HARDCODE_SSE2
//! @brief If defined, SSE2 assembly will be hardcoded into binaries and no lower optimizations are allowed.

//! @def FOG_HARDCODE_SSE3
//! @brief If defined, SSE3 assembly will be hardcoded into binaries and no lower optimizations are allowed.

#if (defined(__i386__) || defined(_M_IX86) || defined(_WIN32)) && \
    !(defined(__x86_64__) || defined(_WIN64) || defined(_M_IA64) || defined(_M_X64))

# define FOG_ARCH_X86
# define FOG_ARCH_BITS 32

# if defined(__i686__)
#  define FOG_HARDCODE_MMX
# endif // __i686__

// MSVC defines this if compiler optimizes code for MMX or SSE
# if defined(_M_IX86_FP)
#  if _M_IX86_FP == 1
#   define FOG_HARDCODE_MMX
#   define FOG_HARDCODE_MMX2
#  elif _M_IX86_FP == 2
#   define FOG_HARDCODE_SSE
#   define FOG_HARDCODE_SSE2
#  endif
# endif // _M_IX86_FP

#elif (defined(__x86_64__) || defined(_WIN64) || defined(_M_IA64) || defined(_M_X64))

# define FOG_ARCH_X86_64
# define FOG_ARCH_BITS 64

// x86_64 uses always SSE/SSE2
# define FOG_HARDCODE_SSE
# define FOG_HARDCODE_SSE2

#elif defined(__powerpc__) || defined(__ppc__)

# define FOG_ARCH_PPC
# define FOG_ARCH_BITS 32

#elif defined(FOG_IDE)

# define FOG_ARCH_X86
# define FOG_ARCH_BITS 32

#else
// If you know your architecture and you want to try compile
// Fog library. Uncomment error and declare following macro:
// #define FOG_ARCH_BITS 32 or 64
# error "Unsupported target CPU"
#endif

#define FOG_BIG_ENDIAN 0
#define FOG_LITTLE_ENDIAN 1

#if FOG_ARCH_BITS == 32
# define FOG_ARCH_32_64(code32, code64) code32
#else
# define FOG_ARCH_32_64(code32, code64) code64
#endif // FOG_ARCH_BITS

//! @}

#endif // _FOG_CORE_BUILD_STDARCH_H
