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

// ============================================================================
// [Fog::Core::C++ - Borland - Alignment]
// ============================================================================

#define FOG_ALIGN_BEGIN(_N_)
#define FOG_ALIGN_END(_N_)
#define FOG_ALIGNED_TYPE(_Type_, _N_) _Type_
#define FOG_ALIGNED_VAR(_Type_, _Name_, _N_) _Type_ _Name_

// ============================================================================
// [Fog::Core::C++ - Borland - Attributes]
// ============================================================================

#define FOG_NO_RETURN
#define FOG_DEPRECATED
#define FOG_RESTRICT

// ============================================================================
// [Fog::Core::C++ - Borland - Calling Conventions]
// ============================================================================

#define FOG_FASTCALL
#define FOG_STDCALL
#define FOG_CDECL

// ============================================================================
// [Fog::Core::C++ - Borland - Inline / No-Inline]
// ============================================================================

#define FOG_INLINE inline
#define FOG_NO_INLINE

// ============================================================================
// [Fog::Core::C++ - Borland - Likely / Unlikely]
// ============================================================================

#define FOG_LIKELY(_Exp_) (_Exp_)
#define FOG_UNLIKELY(_Exp_) (_Exp_)

// ============================================================================
// [Fog::Core::C++ - Borland - Native Types]
// ============================================================================

#define FOG_CC_HAS_NATIVE_CHAR_TYPE
#define FOG_CC_HAS_NATIVE_WCHAR_TYPE

// ============================================================================
// [Fog::Core::C++ - Borland - Macros]
// ============================================================================

#define FOG_MACRO_BEGIN do {
#define FOG_MACRO_END } while(0)

// ============================================================================
// [Fog::Core::C++ - Borland - No-Throw]
// ============================================================================

#define FOG_NOTHROW throw()

// ============================================================================
// [Fog::Core::C++ - Borland - Unused]
// ============================================================================

#define FOG_UNUSED(a) (void)(a)

// ============================================================================
// [Fog::Core::C++ - Borland - Templates]
// ============================================================================

# define FOG_STATIC_T
# define FOG_STATIC_INLINE_T inline

// ============================================================================
// [Fog::Core::C++ - Borland - Features]
// ============================================================================

#define FOG_CC_HAS_PARTIAL_TEMPLATE_SPECIALIZATION

// ============================================================================
// [Fog::Core::C++ - Borland - Visibility]
// ============================================================================

#define FOG_NO_EXPORT

#define FOG_DLL_IMPORT
#define FOG_DLL_EXPORT

#define FOG_CVAR_EXTERN_BASE(_Api_) extern "C"
#define FOG_CVAR_DECLARE_BASE(_Api_) extern "C"

#define FOG_CAPI_EXTERN_BASE(_Api_) extern "C"
#define FOG_CAPI_DECLARE_BASE(_Api_) extern "C"

// [Guard]
#endif // _FOG_CORE_CPP_COMPILER_IDE_H
