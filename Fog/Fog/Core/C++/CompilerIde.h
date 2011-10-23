// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_CPP_COMPILER_IDE_H
#define _FOG_CORE_CPP_COMPILER_IDE_H

// ============================================================================
// [Fog::Core::C++ - IDE Support (Compiler Agnostic)]
// ============================================================================

// Here we generate macros thats friendly for IDEs. In compilation time they are
// never used. Adding FOG_IDE as defined macro to your IDE can help with using
// code-assist.

#define FOG_CC_IDE

// Standard attributes.
#define FOG_INLINE inline
#define FOG_NO_INLINE
#define FOG_NO_RETURN
#define FOG_DEPRECATED

// Restrict support.
#define FOG_RESTRICT

// 32-bit x86 calling conventions.
#define FOG_FASTCALL
#define FOG_STDCALL
#define FOG_CDECL

// Features setup.
#define FOG_CC_HAVE_PARTIAL_TEMPLATE_SPECIALIZATION
#define FOG_CC_HAVE_NATIVE_CHAR_TYPE

// Visibility.
#define FOG_NO_EXPORT
#define FOG_DLL_IMPORT
#define FOG_DLL_EXPORT

// Likely / Unlikely.
#define FOG_LIKELY(_Exp_) (_Exp_)
#define FOG_UNLIKELY(_Exp_) (_Exp_)

// Nothrow.
#define FOG_NOTHROW throw()

// Unused.
#define FOG_UNUSED(a) (void)(a)

// Static-template.
# define FOG_STATIC_T
# define FOG_STATIC_INLINE_T inline

// Align.
#define FOG_ALIGN_BEGIN(_N_)
#define FOG_ALIGN_END(_N_)
#define FOG_ALIGNED_TYPE(_Type_, _N_) _Type_
#define FOG_ALIGNED_VAR(_Type_, _Name_, _N_) _Type_ _Name_

// Macro begin / end.
#define FOG_MACRO_BEGIN do {
#define FOG_MACRO_END } while(0)

// Variables.
#define FOG_CVAR_EXTERN_BASE(_Api_) extern "C"
#define FOG_CVAR_DECLARE_BASE(_Api_) extern "C"

// C API.
#define FOG_CAPI_EXTERN_BASE(_Api_) extern "C"
#define FOG_CAPI_DECLARE_BASE(_Api_) extern "C"

// API.
#define FOG_API
#define FOG_NO_EXPORT

// [Guard]
#endif // _FOG_CORE_CPP_COMPILER_IDE_H
