// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

#ifndef _FOG_CORE_CPP_COMPILER_GCC_H
#define _FOG_CORE_CPP_COMPILER_GCC_H

// ============================================================================
// [Fog::Core::Config - C++ Compiler - GNU C/C++]
// ============================================================================

#if defined(__clang__)
# define FOG_CC_CLANG __clang__
#else
# define FOG_CC_GNU __GNUC__
# define FOG_CC_GNU_VERSION_EQ(_Major_, _Minor_, _Patch_) \
  ( (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__) >= _Major_ * 10000 + _Minor_ * 100 + _Patch_)
# define FOG_CC_GNU_VERSION_GE(_Major_, _Minor_, _Patch_) \
  ( (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__) >= _Major_ * 10000 + _Minor_ * 100 + _Patch_)
#endif

// Standard attributes.
#define FOG_INLINE inline __attribute__((always_inline))
#define FOG_NO_INLINE __attribute__((noinline))
#define FOG_NO_RETURN __attribute__((noreturn))
#define FOG_DEPRECATED __attribute__((deprecated))

// Restrict support.
#if (defined(FOG_CC_GNU) && __GNUC__ >= 3) || \
    (defined(__clang__))
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
#define FOG_CC_HAVE_NATIVE_CHAR_TYPE
#define FOG_CC_HAVE_NATIVE_WCHAR_TYPE

// Visibility.
//
// Following code is better than the code we use, but in this case GCC shows
// incredible number of warnings. So we assume that the default visibility is
// "hidden" and Fog-API visibility is "default".
//
// #if __GNUC__ >= 4
// # define FOG_NO_EXPORT __attribute__((visibility("hidden")))
// #else
// # define FOG_NO_EXPORT
// #endif
#define FOG_NO_EXPORT

#if defined(FOG_OS_WINDOWS)
# define FOG_DLL_IMPORT __declspec(dllimport)
# define FOG_DLL_EXPORT __declspec(dllexport)
#elif (defined(__GNUC__) && __GNUC__ >= 4) || \
      (defined(__clang__))
# define FOG_DLL_IMPORT
# define FOG_DLL_EXPORT __attribute__((visibility("default")))
#else
# define FOG_DLL_IMPORT
# define FOG_DLL_EXPORT
#endif

// Likely / Unlikely.
#define FOG_LIKELY(exp) __builtin_expect(!!(exp), 1)
#define FOG_UNLIKELY(exp) __builtin_expect(!!(exp), 0)

// Nothrow.
#define FOG_NOTHROW throw()

// Unused.
#define FOG_UNUSED(a) (void)(a)

// Static-template.
# define FOG_STATIC_T
# define FOG_STATIC_INLINE_T FOG_INLINE

// Align.
#define FOG_ALIGN_BEGIN(_N_)
#define FOG_ALIGN_END(_N_) __attribute__((aligned(_N_)))
#define FOG_ALIGNED_TYPE(_Type_, _N_) __attribute__((aligned(_N_))) _Type_
#define FOG_ALIGNED_VAR(_Type_, _Name_, _N_) _Type_ __attribute__((aligned(_N_))) _Name_

// Macro begin / end.
#define FOG_MACRO_BEGIN ({
#define FOG_MACRO_END })

// Variables.
#if defined(__MINGW32__)
# define FOG_CVAR_EXTERN_BASE(api) extern "C" api
# define FOG_CVAR_DECLARE_BASE(api)
#else
# define FOG_CVAR_EXTERN_BASE(api) extern "C" api
# define FOG_CVAR_DECLARE_BASE(api)
#endif // __MINGW32__

// C API.
#define FOG_CAPI_EXTERN_BASE(api) extern "C" api
#define FOG_CAPI_DECLARE_BASE(api) extern "C"

// C API - static initializers we can hide them...
#define FOG_INIT_EXTERN_BASE(api) extern "C" FOG_NO_EXPORT
#define FOG_INIT_DECLARE_BASE(api) extern "C" FOG_NO_EXPORT

#endif // _FOG_CORE_CPP_COMPILER_GCC_H
