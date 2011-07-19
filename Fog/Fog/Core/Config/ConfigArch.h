// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

#ifndef _FOG_CORE_CONFIG_CONFIGARCH_H
#define _FOG_CORE_CONFIG_CONFIGARCH_H

#if !defined(FOG_IDE) && !defined(_FOG_CORE_CONFIG_CONFIG_H)
#error "Fog::Core::Config - ConfigArch.h can be only included by Fog/Core/Config/Config.h"
#endif // _FOG_CORE_CONFIG_CONFIG_H

// ============================================================================
// [Fog::Core::Config - Architecture]
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
//! Very nice method to check whether it's faster to use 64-bit arithmetics.

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

// ============================================================================
// [Fog::Core::Config - Architecture - X86_64]
// ============================================================================

#if defined(_M_X64    ) || \
    defined(_M_AMD64  ) || \
    defined(__x86_64__)

# define FOG_ARCH_X86_64
# define FOG_ARCH_BITS 64

// x86_64 uses always SSE/SSE2
# define FOG_HARDCODE_SSE
# define FOG_HARDCODE_SSE2

// ============================================================================
// [Fog::Core::Config - Architecture - X86_32]
// ============================================================================

#elif defined(_M_IX86 ) || \
      defined(__i386__)

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

// ============================================================================
// [Fog::Core::Config - Architecture - Itanium]
// ============================================================================

#elif defined(_M_IA64)

#define FOG_ARCH_ITANIUM
#define FOG_ARCH_BITS 64

// ============================================================================
// [Fog::Core::Config - Architecture - PowerPC]
// ============================================================================

#elif defined(_M_PPC     ) || \
      defined(_M_MPPC    ) || \
      defined(__ppc__    ) || \
      defined(__powerpc__)

# define FOG_ARCH_PPC
# define FOG_ARCH_BITS 32

// ============================================================================
// [Fog::Core::Build - Hack for IDE]
// ============================================================================

#elif defined(FOG_IDE)

# define FOG_ARCH_X86
# define FOG_ARCH_BITS 32

#else
// If you know your architecture and you want to try compile
// Fog library. Uncomment error and declare following macro:
// #define FOG_ARCH_BITS 32 or 64
# error "Unsupported CPU architecture"
#endif

#define FOG_BIG_ENDIAN 0
#define FOG_LITTLE_ENDIAN 1

#if FOG_ARCH_BITS == 32
# define FOG_ARCH_32_64(code32, code64) code32
#else
# define FOG_ARCH_32_64(code32, code64) code64
#endif // FOG_ARCH_BITS

//! @}

#endif // _FOG_CORE_CONFIG_CONFIGARCH_H
