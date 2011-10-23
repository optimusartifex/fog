// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_CPP_COMPILER_MSC_H
#define _FOG_CORE_CPP_COMPILER_MSC_H

// ============================================================================
// [Fog::Core::C++ - MSC]
// ============================================================================

// 1000 - Visual C++ 4
// 1100 - Visual C++ 5
// 1200 - Visual C++ 6
// 1300 - Visual C++ .NET
// 1310 - Visual C++ .NET 2003
// 1400 - Visual C++ 2005
// 1500 - Visual C++ 2008
// 1600 - Visual C++ 2010
#define FOG_CC_MSC _MSC_VER

// Standard attributes.
#define FOG_INLINE __forceinline
#define FOG_NO_INLINE
#define FOG_NO_RETURN __declspec(noreturn)
#define FOG_DEPRECATED __declspec(deprecated)

// Restrict support.
#define FOG_RESTRICT __restrict

// 32-bit x86 calling conventions.
#if defined(FOG_ARCH_X86)
# define FOG_FASTCALL __fastcall
# define FOG_STDCALL __stdcall
# define FOG_CDECL __cdecl
#else
# define FOG_FASTCALL
# define FOG_STDCALL
# define FOG_CDECL
#endif

// Features setup.
#if _MSC_VER >= 1300
# define FOG_CC_HAVE_PARTIAL_TEMPLATE_SPECIALIZATION
#endif // _MSC_VER >= 1300

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
#define FOG_LIKELY(_Exp_) (_Exp_)
#define FOG_UNLIKELY(_Exp_) (_Exp_)

// Nothrow.
#define FOG_NOTHROW throw()

// Unused.
#define FOG_UNUSED(a) (void)(a)

// Static-template.
# define FOG_STATIC_T
# define FOG_STATIC_INLINE_T FOG_INLINE

// Align.
#define FOG_ALIGN_BEGIN(_N_) __declspec(align(_N_))
#define FOG_ALIGN_END(_N_)
#define FOG_ALIGNED_TYPE(_Type_, _N_) __declspec(align(_N_)) _Type_
#define FOG_ALIGNED_VAR(_Type_, _Name_, _N_) __declspec(align(_N_)) _Type_ _Name_

// Macro begin / end.
#define FOG_MACRO_BEGIN do {
#define FOG_MACRO_END } while (0)

// Variables.
#define FOG_CVAR_EXTERN_BASE(_Api_) extern "C" _Api_
#define FOG_CVAR_DECLARE_BASE(_Api_) _Api_

// C API.
#define FOG_CAPI_EXTERN_BASE(_Api_) extern "C" _Api_
#define FOG_CAPI_DECLARE_BASE(_Api_) extern "C" _Api_

// C API - static initializers we can hide them...
#define FOG_INIT_EXTERN_BASE(_Api_) extern "C" FOG_NO_EXPORT
#define FOG_INIT_DECLARE_BASE(_Api_) extern "C" FOG_NO_EXPORT

// Disable warnings.
#pragma warning(disable:4146) // Unary minus operator applied to unsigned type, result still unsigned.
#pragma warning(disable:4251) // Struct '...' needs to have dll-interface to be used by clients of struct '...'.
#pragma warning(disable:4800) // Performance warning: casting to bool.
#pragma warning(disable:4275) // Non dll-interface struct X used as base for dll-interface struct Y.
#pragma warning(disable:4102) // Unreferenced label (we are using this is event mechanism).

// [Guard]
#endif // _FOG_CORE_CPP_COMPILER_MSC_H
