// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

#ifndef _FOG_CORE_CPP_COMPILER_MSC_H
#define _FOG_CORE_CPP_COMPILER_MSC_H

// ============================================================================
// [Fog::Core::Config - C++ Compiler - MSC/Borland]
// ============================================================================

#if defined(__BORLANDC__)
// The naming convention for the __BORLANDC__ identifier is as follows:
// 0x0520 C++ Builder 1
// 0x0530 C++ Builder 3
// 0x0540 C++ Builder 4
// 0x0550 C++ Builder 5
// 0x0560 C++ Builder 6
# define FOG_CC_BORLAND __BORLANDC__
#else
// The naming convenion for Microsoft compiler is:
// 1000 - Visual C++ 4
// 1100 - Visual C++ 5
// 1200 - Visual C++ 6
// 1300 - Visual C++ .NET
// 1310 - Visual C++ .NET 2003
// 1400 - Visual C++ 2005
// 1500 - Visual C++ 2008
# define FOG_CC_MSC _MSC_VER
#endif

// Standard attributes.
#if defined(FOG_CC_MSC)
# define FOG_INLINE __forceinline
# define FOG_NO_INLINE
# define FOG_NO_RETURN __declspec(noreturn)
# define FOG_DEPRECATED __declspec(deprecated)
#else // BORLAND
# define FOG_INLINE inline
# define FOG_NO_INLINE
# define FOG_NO_RETURN
# define FOG_DEPRECATED __declspec(deprecated)
#endif

// Restrict support.
#if defined(FOG_CC_MSC)
# define FOG_RESTRICT __restrict
#else // BORLAND
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
#if defined(FOG_CC_MSC)
# if _MSC_VER >= 1300
#  define FOG_CC_HAVE_PARTIAL_TEMPLATE_SPECIALIZATION
# endif
#else
// TODO: Has Borland compiler partial template specialization, which version?
#endif

// MSC treats char, signed char, and unsigned char as different types.
#define FOG_CC_HAVE_NATIVE_CHAR_TYPE

// MSC must be configured to treat wchar_t and unsigned short as different types.
#if defined(_NATIVE_WCHAR_T_DEFINED)
# define FOG_CC_HAVE_NATIVE_WCHAR_TYPE
#endif // _NATIVE_WCHAR_T_DEFINED

// Visibility.
#define FOG_NO_EXPORT
#define FOG_DLL_IMPORT __declspec(dllimport)
#define FOG_DLL_EXPORT __declspec(dllexport)

// Likely / Unlikely.
#define FOG_LIKELY(exp) (exp)
#define FOG_UNLIKELY(exp) (exp)

// Nothrow.
#define FOG_NOTHROW throw()

// Unused.
#define FOG_UNUSED(a) (void)(a)

// Static-template.
# define FOG_STATIC_T
# define FOG_STATIC_INLINE_T FOG_INLINE

// Align.
#if defined(FOG_CC_MSC)
# define FOG_ALIGN_BEGIN(_N_) __declspec(align(_N_))
# define FOG_ALIGN_END(_N_)
# define FOG_ALIGNED_TYPE(_Type_, _N_) __declspec(align(_N_)) _Type_
# define FOG_ALIGNED_VAR(_Type_, _Name_, _N_) __declspec(align(_N_)) _Type_ _Name_
#else // BORLAND
# define FOG_ALIGN_BEGIN(_N_)
# define FOG_ALIGN_END(_N_)
# define FOG_ALIGNED_TYPE(_Type_, _N_) _Type_
# define FOG_ALIGNED_VAR(_Type_, _Name_, _N_) _Type_ _Name_
#endif

// Macro begin / end.
#define FOG_MACRO_BEGIN do {
#define FOG_MACRO_END } while (0)

// Variables.
#define FOG_CVAR_EXTERN_BASE(api) extern "C" api
#define FOG_CVAR_DECLARE_BASE(api) api

// C API.
#define FOG_CAPI_EXTERN_BASE(api) extern "C" api
#define FOG_CAPI_DECLARE_BASE(api) extern "C" api

// C API - static initializers we can hide them...
#define FOG_INIT_EXTERN_BASE(api) extern "C" FOG_NO_EXPORT
#define FOG_INIT_DECLARE_BASE(api) extern "C" FOG_NO_EXPORT

// Disable some warnings
#if defined(FOG_CC_MSC)
# pragma warning(disable:4146) // Unary minus operator applied to unsigned type, result still unsigned.
# pragma warning(disable:4251) // Struct '...' needs to have dll-interface to be used by clients of struct '...'.
# pragma warning(disable:4800) // Performance warning: casting to bool.
# pragma warning(disable:4275) // Non dll-interface struct X used as base for dll-interface struct Y.
# pragma warning(disable:4102) // Unreferenced label (we are using this is event mechanism).
#else // FOG_CC_BORLAND
# pragma warn -8008 // Condition is always true.
# pragma warn -8057 // Parameter is never used.
# pragma warn -8059 // Structure packing was changed.
# pragma warn -8066 // Unrecheable code.
#endif // FOG_CC_MSC

#endif // _FOG_CORE_CPP_COMPILER_MSC_H
