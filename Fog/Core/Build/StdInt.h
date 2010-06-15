// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

#ifndef _FOG_CORE_BUILD_STDINT_H
#define _FOG_CORE_BUILD_STDINT_H

#if !defined(FOG_IDE) && !defined(_FOG_BUILD_BUILD_H)
#error "Fog::Core::Build - StdInt.h can be only included by Fog/Core/Build.h"
#endif // _FOG_BUILD_BUILD_H

// ============================================================================
// [Fog::Core::Build - System Integer Types]
// ============================================================================

#if defined(FOG_HAVE_LIMITS_H)
# include <limits.h>
#endif // FOG_HAVE_LIMITS_H

#if defined(FOG_HAVE_STDINT_H)
#include <stdint.h>
#else
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

#if defined(FOG_CC_MSVC)
// Detect 64-bit portability issues
typedef int __w64 sysint_t;
typedef unsigned int __w64 sysuint_t;
#else
typedef int32_t sysint_t;
typedef uint32_t sysuint_t;
#endif

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

#endif // _FOG_CORE_BUILD_STDINT_H
