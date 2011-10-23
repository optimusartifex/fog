// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_CPP_COMPILER_BORLAND_H
#define _FOG_CORE_CPP_COMPILER_BORLAND_H

// ============================================================================
// [Fog::Core::C++ - Borland]
// ============================================================================

// 0x0520 C++ Builder 1
// 0x0530 C++ Builder 3
// 0x0540 C++ Builder 4
// 0x0550 C++ Builder 5
// 0x0560 C++ Builder 6
#define FOG_CC_BORLAND __BORLANDC__

// Standard attributes.
#define FOG_INLINE inline
#define FOG_NO_INLINE
#define FOG_NO_RETURN
#define FOG_DEPRECATED __declspec(deprecated)

// Restrict support.
#define FOG_RESTRICT

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
// TODO: Has Borland compiler partial template specialization, which version?

// TODO: Borland support: Is char native type?
#define FOG_CC_HAVE_NATIVE_CHAR_TYPE
// TODO: Borland support: Is wchar_t native type?
#define FOG_CC_HAVE_NATIVE_WCHAR_TYPE

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
#define FOG_ALIGN_BEGIN(_N_)
#define FOG_ALIGN_END(_N_)
#define FOG_ALIGNED_TYPE(_Type_, _N_) _Type_
#define FOG_ALIGNED_VAR(_Type_, _Name_, _N_) _Type_ _Name_

// Macro begin / end.
#define FOG_MACRO_BEGIN do {
#define FOG_MACRO_END } while (0)

// Variables.
#define FOG_CVAR_EXTERN_BASE(_Api_) extern "C" _Api_
#define FOG_CVAR_DECLARE_BASE(_Api_) _Api_

// C API.
#define FOG_CAPI_EXTERN_BASE(_Api_) extern "C" _Api_
#define FOG_CAPI_DECLARE_BASE(_Api_) extern "C" _Api_

// Disable warnings.
#pragma warn -8008 // Condition is always true.
#pragma warn -8057 // Parameter is never used.
#pragma warn -8059 // Structure packing was changed.
#pragma warn -8066 // Unrecheable code.

// [Guard]
#endif // _FOG_CORE_CPP_COMPILER_BORLAND_H
