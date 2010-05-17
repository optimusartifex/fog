// [Fog Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_BUILD_BUILD_H
#define _FOG_BUILD_BUILD_H

// [Dependencies]
#include <Fog/Build/Config.h>

#include <new>

//! @defgroup Group_Macros Macros
//! @brief Global Fog macros.

// ============================================================================
// [Operating System]
// ============================================================================

//! @defgroup Group_Macros_System System macros
//! @ingroup Group_Macros
//!
//! Operating system checking at compile time. If your application directly
//! depends to specific features of operating system, you can easily check
//! for it and include these features. These features are detected at compile
//! time.
//!
//! @{

//! @def FOG_OS_WINDOWS
//! @brief Operating system is windows.

#if !defined(FOG_OS_DEFINED) && \
    (defined(__WIN32) || defined(_WIN32) || defined(_WINDOWS))
# define FOG_OS_DEFINED
# define FOG_OS_WINDOWS
#endif // __WIN32 || _WIN32 || _WINDOWS

//! @def FOG_OS_POSIX
//! @brief Portable check if operating system is like unix (linux, freebsd, ...)
//!
//! @note Posix is always defined if Fog is not compiled for Windows. We treat
//! all other operating systems as posix and we use posix API a lot. Differences
//! are usually handled in places where needed (Linux/BSD/MacOSX specific code)

//! @def FOG_OS_LINUX
//! @brief Operating system is linux.

#if !defined(FOG_OS_DEFINED) && \
    (defined(linux) || defined(__linux) || defined(__linux__))
# define FOG_OS_DEFINED
# define FOG_OS_POSIX
# define FOG_OS_LINUX
#endif // linux || __linux || __linux__

//! @def FOG_OS_FREEBSD
//! @brief Operating system is freebsd.

#if !defined(FOG_OS_DEFINED) && defined(__FreeBSD__)
# define FOG_OS_DEFINED
# define FOG_OS_POSIX
# define FOG_OS_BSD
# define FOG_OS_FREEBSD
#endif // __FreeBSD__

//! @def FOG_OS_OPENBSD
//! @brief Operating system is openbsd.

#if !defined(FOG_OS_DEFINED) && defined(__OpenBSD__)
# define FOG_OS_DEFINED
# define FOG_OS_POSIX
# define FOG_OS_BSD
# define FOG_OS_OPENBSD
#endif // __OpenBSD__

//! @def FOG_OS_NETBSD
//! @brief Operating system is netbsd.

#if !defined(FOG_OS_DEFINED) && defined(__NetBSD__)
# define FOG_OS_DEFINED
# define FOG_OS_POSIX
# define FOG_OS_BSD
# define FOG_OS_NETBSD
#endif // __NetBSD__

//! @def FOG_OS_HPUX
//! @brief Operating system is netbsd.

#if !defined(FOG_OS_DEFINED) && defined(__HPUX__)
# define FOG_OS_DEFINED
# define FOG_OS_POSIX
# define FOG_OS_HPUX
#endif // __HPUX__

#if !defined(FOG_OS_DEFINED) && (defined(__DARWIN__) || defined(__APPLE__))
# define FOG_OS_DEFINED
# define FOG_OS_POSIX
# define FOG_OS_MAC
#endif // __DARWIN__ || __APPLE__

//! @}

// ============================================================================
// [Architecture]
// ============================================================================

//! @defgroup Group_Macros_Architecture Architecture macros
//! @ingroup Group_Macros
//!
//! Target architeture compile-time check. You can use it to check if we are 
//! in 32bit or 64bit mode or simple for x86 or x86_64. WDE uses different 
//! optimizations to be best for different architectures.
//!
//! You can also check only if architecture is 32bit or 64bit, see 
//! @c FOG_ARCH_BITS
//!
//! @note Only 32bit and 64bit architectures are supported.
//!
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

// ============================================================================
// [Compiler Options]
// ============================================================================

//! @defgroup Group_Macros_Compiler_Features Compiler features
//! @ingroup Group_Macros
//!
//! This is mainly for internal use, but simplifies some runtime checks to
//! this header. Normally you do not know how to use these macros.
//!
//! @{

//! @def FOG_INLINE
//! @brief Inline function, see C++ @c inline keyword. Core library can use 
//! compiler specific keywords for better inlining (for exampple __forceinline 
//! under MSVC)

//! @def FOG_NO_RETURN
//! @brief Function never returns, so warning will not be shown for no return value

//! @def FOG_DEPRECATED
//! @brief Usage of symbol is deprecated.
//!
//! If this attribute is used and you will try to use something that has
//! set this attribute, compiler warning will be showed.

//! @def FOG_FASTCALL
//! @brief Two first arguments of function will be registers, for small functions.
//!
//! @note Only valid for 32-bit x86 targets.

//! @def FOG_STDCALL
//! @brief No register arguments, just push and pop.
//!
//! @note Only valid for 32-bit x86 targets.

//! @def FOG_CDECL
//! @brief Standard C function declaration.
//!
//! @note Only valid for 32-bit x86 targets.

//! @def FOG_HIDDEN
//! @brief Symbol will not be exported to library / executable.
//!
//! You can control some symbols and disable them to link their names in
//! your library or application, Core uses this macros to decrease size of
//! library.

//! @def FOG_DLL_IMPORT
//! @brief Symbol will be imported from the dynamic linked library.

//! @def FOG_DLL_EXPORT
//! @brief Symbol will be exported to the dynamic linked library.

//! @def FOG_LIKELY(exp)
//! @brief Expression in macro is expected to be true.
//! @param exp Expression.

//! @def FOG_UNLIKELY(exp)
//! @brief Expression in macro is expected to be false.
//! @param exp Expression.

//! @def FOG_ALIGN_BEGIN(n)
//! @brief Aligns variable to @a n byte boundary (begin aligned variable declaration).

//! @def FOG_ALIGN_END(n)
//! @brief Aligns variable to @a n byte boundary (end of aligned variable declaration).

//! @def FOG_API
//! @brief Symbols with this attribute set on will be exported to dynamic symbols table.
//!
//! @note This attribute is set to @c FOG_DLL_EXPORT when library is building and
//! to @c FOG_DLL_IMPORT when library is including from another project.

// [Compiler - IDE]

// Here we generate macros thats friendly for IDEs. In compilation time they are
// never used. Adding FOG_IDE as defined macro to your IDE can help with code
// assist.
#if defined(FOG_IDE)

#define FOG_CC_IDE

#define FOG_INLINE inline
#define FOG_NO_INLINE
#define FOG_NO_RETURN
#define FOG_DEPRECATED
#define FOG_RESTRICT

#define FOG_FASTCALL
#define FOG_STDCALL
#define FOG_CDECL

#define FOG_HIDDEN

#define FOG_DLL_IMPORT
#define FOG_DLL_EXPORT

#define FOG_LIKELY(exp) (exp)
#define FOG_UNLIKELY(exp) (exp)

#define FOG_UNUSED(a)
#define FOG_ALIGN_BEGIN(__n__)
#define FOG_ALIGN_END(__n__)
#define FOG_ALIGNED_TYPE(__type__, __n__) __type__
#define FOG_ALIGNED_VAR(__type__, __name__ __n__) __type__ __name__

#define FOG_BEGIN_MACRO do {
#define FOG_END_MACRO } while(0)

#define FOG_API

#define FOG_CVAR_EXTERN_BASE(api) extern "C"
#define FOG_CVAR_DECLARE_BASE(api)

#define FOG_CAPI_EXTERN_BASE(api) extern "C"
#define FOG_CAPI_DECLARE_BASE(api) extern "C"

#define FOG_INIT_EXTERN_BASE(api) extern "C"
#define FOG_INIT_DECLARE_BASE(api) extern "C"

// [Compiler - GNU C/C++]

#elif defined(__GNUC__)
#define FOG_CC_GNU __GNUC__

// Standard attributes/
#define FOG_INLINE inline __attribute__((always_inline))
#define FOG_NO_INLINE __attribute__((noinline))
#define FOG_NO_RETURN __attribute__((noreturn))
#define FOG_DEPRECATED __attribute__((deprecated))

// Restrict support.
#if (__GNUC__ >= 3)
# define FOG_RESTRICT __restrict__
#else
# define FOG_RESTRICT
#endif

// 32-bit x86 calling conventions.
#ifdef FOG_ARCH_X86
# define FOG_FASTCALL __attribute__((regparm(3)))
# define FOG_STDCALL __attribute__((stdcall))
# define FOG_CDECL __attribute__((cdecl))
#else
# define FOG_FASTCALL
# define FOG_STDCALL
# define FOG_CDECL
#endif

// Features setup.
#define FOG_CC_HAVE_PARTIAL_TEMPLATE_SPECIALIZATION

// Visibility.
#if __GNUC__ >= 4
# define FOG_HIDDEN __attribute__((visibility("default")))
#else
# define FOG_HIDDEN
#endif

#if defined(FOG_OS_WINDOWS)
# define FOG_DLL_IMPORT __declspec(dllimport)
# define FOG_DLL_EXPORT __declspec(dllexport)
#elif (__GNUC__ >= 4)
# define FOG_DLL_IMPORT
# define FOG_DLL_EXPORT __attribute__((visibility ("default")))
#else
# define FOG_DLL_IMPORT
# define FOG_DLL_EXPORT
#endif

// Likely / unlikely.
#define FOG_LIKELY(exp) __builtin_expect(!!(exp), 1)
#define FOG_UNLIKELY(exp) __builtin_expect(!!(exp), 0)

// Unused.
#define FOG_UNUSED(a) (void)(a)

// Align.
#define FOG_ALIGN_BEGIN(__n__)
#define FOG_ALIGN_END(__n__) __attribute__((aligned(__n__)))
#define FOG_ALIGNED_TYPE(__type__, __n__) __attribute__((aligned(__n__))) __type__
#define FOG_ALIGNED_VAR(__type__, __name__, __n__) __type__ __attribute__((aligned(__n__))) __name__

// Macro begin / end.
#define FOG_BEGIN_MACRO ({
#define FOG_END_MACRO })

// Variables.
#if defined(__MINGW32__)
# define FOG_CVAR_EXTERN_BASE(api) extern "C" api
# define FOG_CVAR_DECLARE_BASE(api) 
#else
# define FOG_CVAR_EXTERN_BASE(api) extern "C"
# define FOG_CVAR_DECLARE_BASE(api) 
#endif // __MINGW32__

// C API.
#define FOG_CAPI_EXTERN_BASE(api) extern "C" api
#define FOG_CAPI_DECLARE_BASE(api) extern "C" api

// C API - static initializers we can hide them...
#define FOG_INIT_EXTERN_BASE(api) extern "C" FOG_HIDDEN
#define FOG_INIT_DECLARE_BASE(api) extern "C" FOG_HIDDEN

// API.
#if defined(Fog_EXPORTS)
# define FOG_API FOG_DLL_EXPORT
#else
# define FOG_API FOG_DLL_IMPORT
#endif // Fog_EXPORTS

// [Compiler - Borland C++ Builder or MSVC]

#elif defined(__BORLANDC__) || defined(_MSC_VER)

#if defined(__BORLANDC__)
// The naming convention for the __BORLANDC__ identifier is as follows:
// 0x0520 C++ Builder 1
// 0x0530 C++ Builder 3
// 0x0540 C++ Builder 4
// 0x0550 C++ Builder 5
// 0x0560 C++ Builder 6
# define FOG_CC_BORLAND __BORLANDC__
#else // _MSC_VER
// The naming convenion for Microsoft compiler is:
// 1000 - Visual C++ 4
// 1100 - Visual C++ 5
// 1200 - Visual C++ 6
// 1300 - Visual C++ .NET
// 1310 - Visual C++ .NET 2003
// 1400 - Visual C++ 2005
// 1500 - Visual C++ 2008
# define FOG_CC_MSVC _MSC_VER
#endif // __BORLANDC__

// Standard attributes.
#if defined(FOG_CC_MSVC)
# define FOG_INLINE __forceinline
# define FOG_NO_INLINE
# define FOG_NO_RETURN __declspec(noreturn)
# define FOG_DEPRECATED __declspec(deprecated)
# define FOG_RESTRICT __restrict
#else // BORLAND
# define FOG_INLINE inline
# define FOG_NO_INLINE
# define FOG_NO_RETURN
# define FOG_DEPRECATED __declspec(deprecated)
# define FOG_RESTRICT
#endif

// 32-bit x86 calling conventions.
#ifdef FOG_ARCH_X86
# define FOG_FASTCALL __fastcall
# define FOG_STDCALL __stdcall
# define FOG_CDECL __cdecl
#else
# define FOG_FASTCALL
# define FOG_STDCALL
# define FOG_CDECL
#endif

// Features setup.
#if defined(FOG_CC_MSVC)
# if _MSC_VER >= 1300
#  define FOG_CC_HAVE_PARTIAL_TEMPLATE_SPECIALIZATION
# endif
#else
// Has Borland compiler partial template specialization?
#endif

// Visibility.
#define FOG_HIDDEN

// API.
#define FOG_DLL_IMPORT __declspec(dllimport)
#define FOG_DLL_EXPORT __declspec(dllexport)

// Likely / unlikely.
#define FOG_LIKELY(exp) (exp)
#define FOG_UNLIKELY(exp) (exp)

// Unused.
#define FOG_UNUSED(a) (void)(a)

// Align.
#if defined(FOG_CC_MSVC)
# define FOG_ALIGN_BEGIN(__n__) __declspec(align(__n__))
# define FOG_ALIGN_END(__n__)
# define FOG_ALIGNED_TYPE(__type__, __n__) __declspec(align(__n__)) __type__
# define FOG_ALIGNED_VAR(__type__, __name__, __n__) __declspec(align(__n__)) __type__ __name__
#else // BORLAND
# define FOG_ALIGN_BEGIN(__n__)
# define FOG_ALIGN_END(__n__)
# define FOG_ALIGNED_TYPE(__type__, __n__) __type__
# define FOG_ALIGNED_VAR(__type__, __name__, __n__) __type__ __name__
#endif

// Macro begin / end.
#define FOG_BEGIN_MACRO do {
#define FOG_END_MACRO } while (0)

// Variables.
#define FOG_CVAR_EXTERN_BASE(api) extern "C" api
#define FOG_CVAR_DECLARE_BASE(api) api

// C API.
#define FOG_CAPI_EXTERN_BASE(api) extern "C" api
#define FOG_CAPI_DECLARE_BASE(api) extern "C" api

// C API - static initializers we can hide them...
#define FOG_INIT_EXTERN_BASE(api) extern "C" FOG_HIDDEN
#define FOG_INIT_DECLARE_BASE(api) extern "C" FOG_HIDDEN

// Disable some warnings
#if defined(FOG_CC_MSVC)
# pragma warning(disable:4146) // unary minus operator applied to unsigned type, result still unsigned
# pragma warning(disable:4251) // struct '...' needs to have dll-interface to be used by clients of struct '...'
# pragma warning(disable:4800) // performance warning: casting to bool
# pragma warning(disable:4275) // non dll-interface struct X used as base for dll-interface struct Y
# pragma warning(disable:4102) // unreferenced label (we are using this is event mechanism)
#else // FOG_CC_BORLAND
# pragma warn -8008 // condition is always true
# pragma warn -8057 // parameter is never used
# pragma warn -8059 // structure packing was changed
# pragma warn -8066 // unrecheable code
#endif // FOG_CC_MSVC

// Unsupported compiler?
#else
# error "Unsupported compiler"
#endif

#if !defined(FOG_GCC_FORCE_INLINE)
#define FOG_GCC_FORCE_INLINE
#endif // FOG_GCC_FORCE_INLINE

#if defined(Fog_EXPORTS)
# define FOG_API FOG_DLL_EXPORT
#else
# define FOG_API FOG_DLL_IMPORT
#endif

// Fog API Visibility.
#define FOG_CVAR_EXTERN FOG_CVAR_EXTERN_BASE(FOG_API)
#define FOG_CVAR_DECLARE FOG_CVAR_DECLARE_BASE(FOG_API)

#define FOG_CAPI_EXTERN FOG_CAPI_EXTERN_BASE(FOG_API)
#define FOG_CAPI_DECLARE FOG_CAPI_DECLARE_BASE(FOG_API)

#define FOG_INIT_EXTERN FOG_INIT_EXTERN_BASE(FOG_API)
#define FOG_INIT_DECLARE FOG_INIT_DECLARE_BASE(FOG_API)

//! @}

// ============================================================================
// [Headers]
// ============================================================================

#if defined(FOG_OS_WINDOWS)
# define _WIN32_WINNT 0x500 // We need Win2000/XP+
# include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(FOG_HAVE_STDARG_H)
# include <stdarg.h>
#endif // FOG_HAVE_STDARG_H

#if defined(FOG_HAVE_STDINT_H)
# include <stdint.h>
#endif // FOG_HAVE_STDINT_H

#if defined(FOG_HAVE_LIMITS_H)
# include <limits.h>
#endif // FOG_HAVE_LIMITS_H

// ============================================================================
// [System Integer Types]
// ============================================================================

// Standard C/C++ defs
#if !defined(FOG_HAVE_STDINT_H)

#if defined(_MSC_VER)

#if (_MSC_VER < 1300)
typedef char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#else
typedef __int8 int8_t;
typedef __int16 int16_t;
typedef __int32 int32_t;
typedef __int64 int64_t;
typedef unsigned __int8 uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;
#endif

#else // !defined(_MSC_VER)
// We are trying to be compatible with everything defined by standard compilers.
// So we will define some macros that says that these types were defined so
// standard compliant compiler should understand them.
#if !defined(__int8_t_defined)
# define __int8_t_defined
typedef signed char int8_t;
#endif // __int8_t_defined

#if !defined(__uint8_t_defined)
# define __uint8_t_defined
typedef unsigned char uint8_t;
#endif // __uint8_t_defined

#if !defined(__int16_t_defined)
# define __int16_t_defined
typedef short int16_t;
#endif // __int16_t_defined

#if !defined(__uint16_t_defined)
# define __uint16_t_defined
typedef unsigned short uint16_t;
#endif // __uint16_t_defined

#if !defined(__int32_t_defined)
# define __int32_t_defined
typedef int int32_t;
#endif // __int32_t_defined

#if !defined(__uint32_t_defined)
# define __uint32_t_defined
typedef unsigned int uint32_t;
#endif // __uint32_t_defined

#if FOG_SIZEOF_LONG == 8

# if !defined(__int64_t_defined)
#  define __int64_t_defined
typedef long int64_t;
# endif // __int64_t_defined

# if !defined(__uint64_t_defined)
#  define __uint64_t_defined
typedef unsigned long uint64_t;
# endif // __uint64_t_defined

#else

# if !defined(__int64_t_defined)
#  define __int64_t_defined
typedef long long int64_t;
# endif // __int64_t_defined

# if !defined(__uint64_t_defined)
#  define __uint64_t_defined
typedef unsigned long long uint64_t;
# endif // __uint64_t_defined

#endif

#endif // !_MSC_VER
#endif // FOG_HAVE_STDINT_H

// uchar, ushort, uint and ulong.
// - these typedefs should never conflict!
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

typedef int32_t err_t;

#if FOG_ARCH_BITS == 32
typedef int32_t sysint_t;
typedef uint32_t sysuint_t;
#else
typedef int64_t sysint_t;
typedef uint64_t sysuint_t;
#endif

// fixed point integer types.
typedef int32_t int16x16_t;
typedef int32_t int24x8_t;
typedef int64_t int48x16_t;

#if defined(FOG_OS_WINDOWS) && !defined(FOG_CC_GNU)
# define FOG_INT64_C(num) num##i64
# define FOG_UINT64_C(num) num##ui64
#else
# define FOG_INT64_C(num) num##LL
# define FOG_UINT64_C(num) num##ULL
#endif

#if !defined(INT8_MIN)
# define INT8_MIN (-128)
#endif

#if !defined(INT16_MIN)
# define INT16_MIN (-32767-1)
#endif

#if !defined(INT32_MIN)
# define INT32_MIN (-2147483647-1)
#endif

#if !defined(INT64_MIN)
# define INT64_MIN (FOG_INT64_C(-9223372036854775807)-FOG_INT64_C(1))
#endif

#if !defined(INT8_MAX)
# define INT8_MAX (127)
#endif

#if !defined(INT16_MAX)
# define INT16_MAX (32767)
#endif

#if !defined(INT32_MAX)
# define INT32_MAX (2147483647)
#endif

#if !defined(INT64_MAX)
# define INT64_MAX (FOG_INT64_C(9223372036854775807))
#endif

#if !defined(UINT8_MAX)
# define UINT8_MAX (255)
#endif

#if !defined(UINT16_MAX)
# define UINT16_MAX (65535)
#endif

#if !defined(UINT32_MAX)
# define UINT32_MAX (4294967295U)
#endif

#if !defined(UINT64_MAX)
# define UINT64_MAX (FOG_UINT64_C(18446744073709551615))
#endif

#if !defined(FLOAT_MAX)
# define FLOAT_MAX (3.4028234663852886e+38)
#endif

#if !defined(FLOAT_MIN)
# define FLOAT_MIN (-FLOAT_MAX)
#endif

#if !defined(SYSINT_MIN)
# if FOG_ARCH_BITS == 32
#  define SYSINT_MIN INT32_MIN
# else
#  define SYSINT_MIN INT64_MIN
# endif
#endif

#if !defined(SYSINT_MAX)
# if FOG_ARCH_BITS == 32
#  define SYSINT_MAX INT32_MAX
# else
#  define SYSINT_MAX INT64_MAX
# endif
#endif

#if !defined(SYSUINT_MAX)
# if FOG_ARCH_BITS == 32
#  define SYSUINT_MAX UINT32_MAX
# else
#  define SYSUINT_MAX UINT64_MAX
# endif
#endif

#if !defined(INT_MIN)
# define INT_MIN INT32_MIN
#endif

#if !defined(INT_MAX)
# define INT_MAX INT32_MAX
#endif

#if !defined(UINT_MIN)
# define UINT_MIN UINT32_MIN
#endif

#if !defined(UINT_MAX)
# define UINT_MAX UINT32_MAX
#endif

#if !defined(LONG_MIN)
# if FOG_SIZEOF_LONG == 4
#  define LONG_MIN INT32_MIN
# else
#  define LONG_MIN INT64_MIN
# endif
#endif

#if !defined(LONG_MAX)
# if FOG_SIZEOF_LONG == 4
#  define LONG_MAX INT32_MAX
# else
#  define LONG_MAX INT64_MAX
# endif
#endif

#if !defined(ULONG_MAX)
# if FOG_SIZEOF_LONG == 4
#  define ULONG_MAX UINT32_MAX
# else
#  define ULONG_MAX UINT64_MAX
# endif
#endif

#if !defined(LLONG_MIN)
# define LLONG_MIN INT64_MIN
#endif

#if !defined(LLONG_MAX)
# define LLONG_MAX INT64_MAX
#endif

#if !defined(ULLONG_MAX)
# define ULLONG_MAX UINT64_MAX
#endif

// ============================================================================
// [Core C++ Macros]
// ============================================================================

//! @brief Creates @c __selftype__ typedef in class thats for callbacks and references
//!
//! @sa @c FOG_DECLARE_D
#define FOG_DECLARE_SELFTYPE(selftype) \
  typedef selftype __selftype__;

//! @brief Creates @c __dtype__ typedef and @c d member in class
//!
//! @sa @c FOG_DECLARE_SELFTYPE, @c FOG_DECLARE_D_METHODS
#define FOG_DECLARE_D(dtype) \
  typedef dtype __dtype__; \
  \
  __dtype__* _d;

//! @brief Use this macro to generate empty copy constructor and empty copy operator.
//! Use in 'private' section.
#define FOG_DISABLE_COPY(selftype) \
  FOG_INLINE FOG_HIDDEN selftype(const selftype& other); \
  FOG_INLINE FOG_HIDDEN selftype& operator=(const selftype& other);

// Workaround for Borland compiler that don't know about variadic macros
#define FOG_MACRO1(a0) a0
#define FOG_MACRO2(a0, a1) a0, a1
#define FOG_MACRO3(a0, a1, a2) a0, a1, a2
#define FOG_MACRO4(a0, a1, a2, a3) a0, a1, a2, a3
#define FOG_MACRO5(a0, a1, a2, a3, a4) a0, a1, a2, a3, a4
#define FOG_MACRO6(a0, a1, a2, a3, a4, a5) a0, a1, a2, a3, a4, a5
#define FOG_MACRO7(a0, a1, a2, a3, a4, a5, a6) a0, a1, a2, a3, a4, a5, a6
#define FOG_MACRO8(a0, a1, a2, a3, a4, a5, a6, a7) a0, a1, a2, a3, a4, a5, a6, a7
#define FOG_MACRO9(a0, a1, a2, a3, a4, a5, a6, a7, a8) a0, a1, a2, a3, a4, a5, a6, a7, a8

//! @brief Some systems hasn't defined @c offsetof keyword, but FOG_OFFSET_OF 
//! is always defined and it's possible to use it to get offset from a class 
//! member (only single inheritance is allowed).
#define FOG_OFFSET_OF(struct, field) ((sysuint_t) ((const uchar*) &((const struct*)0x1)->field) - 1)

//! @brief Size of static table. Do not use this macro to get size of dynamic
//! table, you can't get it by this way!!!
//!
//! Use it for example in following way:
//! @code
//! uint table[] = { 1, 2, 3, 4, 5 };
//! uint i;
//!
//! for (i = 0; i != FOG_ARRAY_SIZE(table); i++)
//! {
//!   printf("table[%u]=%u\n", i, table[i]);
//! }
//! @endcode
#define FOG_ARRAY_SIZE(element) ( sizeof(element) / sizeof(element[0]) )

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
#define FOG_MAKE_UINT16_SEQ(byte0, byte1) \
  (uint16_t) ( ((uint16_t)(byte0) <<  0) | \
               ((uint16_t)(byte1) <<  8) )
#define FOG_MAKE_UINT32_SEQ(byte0, byte1, byte2, byte3) \
  (uint32_t) ( ((uint32_t)(byte0) <<  0) | \
               ((uint32_t)(byte1) <<  8) | \
               ((uint32_t)(byte2) << 16) | \
               ((uint32_t)(byte3) << 24) )
#define FOG_MAKE_UINT64_SEQ(byte0, byte1, byte2, byte3, byte4, byte5, byte6, byte7) \
  (uint64_t) ( ((uint64_t)(byte0) <<  0) | \
               ((uint64_t)(byte1) <<  8) | \
               ((uint64_t)(byte2) << 16) | \
               ((uint64_t)(byte3) << 24) | \
               ((uint64_t)(byte4) << 32) | \
               ((uint64_t)(byte5) << 40) | \
               ((uint64_t)(byte6) << 48) | \
               ((uint64_t)(byte7) << 56) )
#else
#define FOG_MAKE_UINT16_SEQ(byte0, byte1) \
  (uint16_t) ( ((uint16_t)(byte1) <<  0) | \
               ((uint16_t)(byte0) <<  8) )
#define FOG_MAKE_UINT32_SEQ(byte0, byte1, byte2, byte3) \
  (uint32_t) ( ((uint32_t)(byte3) <<  0) | \
               ((uint32_t)(byte2) <<  8) | \
               ((uint32_t)(byte1) << 16) | \
               ((uint32_t)(byte0) << 24) )
#define FOG_MAKE_UINT64_SEQ(byte0, byte1, byte2, byte3, byte4, byte5, byte6, byte7) \
  (uint64_t) ( ((uint64_t)(byte7) <<  0) | \
               ((uint64_t)(byte6) <<  8) | \
               ((uint64_t)(byte5) << 16) | \
               ((uint64_t)(byte4) << 24) | \
               ((uint64_t)(byte3) << 32) | \
               ((uint64_t)(byte2) << 40) | \
               ((uint64_t)(byte1) << 48) | \
               ((uint64_t)(byte0) << 56) )
#endif

#define FOG_RETURN_ON_ERROR(expression) \
  FOG_BEGIN_MACRO \
    err_t __err_inside = (expression); \
    if (__err_inside != ::Fog::ERR_OK) return __err_inside; \
  FOG_END_MACRO

// ============================================================================
// [Noop]
// ============================================================================

#define FOG_NOP ((void)0)

// ============================================================================
// [strcasecmp / strncasecmp]
// ============================================================================

// Porting to windows...
//
// Sorry for undocumented porting things here, but I will place these functions
// here instead of creating #ifdefs in code.
#if defined(FOG_OS_WINDOWS)
// unix like
# define strncasecmp _strnicmp
# define strcasecmp _stricmp
#endif // FOG_OS_WINDOWS

#if defined(FOG_HARDCODE_SSE)
# include <xmmintrin.h>
#endif // FOG_HARDCODE_SSE

#if defined(FOG_HARDCODE_SSE2)
# include <emmintrin.h>
#endif // FOG_HARDCODE_SSE2

// Defined also in Fog-Core/Application.h. It's defined here to prevent
// compilation errors when using FOG_CORE_MAIN() or FOG_GUI_MAIN() and
// this header file is not included.
FOG_API void fog_arguments_init(int argc, char* argv[]);

//! Usage:
//!
//! FOG_GUI_MAIN()
//! {
//!   // Your main() here ...
//!   return Fog::ExitSuccess;
//! }
#if defined(FOG_OS_WINDOWS)
#define FOG_GUI_MAIN() \
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
#define FOG_GUI_MAIN() \
static int _fog_main(int argc, char* argv[]); \
\
int main(int argc, char* argv[]) \
{ \
  fog_arguments_init(argc, argv); \
  \
  return _fog_main(argc, argv); \
} \
\
static int _fog_main(int argc, char* argv[])
#endif

// [Guard]
#endif // _FOG_BUILD_BUILD_H
