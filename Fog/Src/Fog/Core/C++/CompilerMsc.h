// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_CPP_COMPILER_MSC_H
#define _FOG_CORE_CPP_COMPILER_MSC_H

// ============================================================================
// [Fog::Core::C++ - MSC - Version]
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

// ============================================================================
// [Fog::Core::C++ - MSC - Alignment]
// ============================================================================

#define FOG_ALIGN_BEGIN(_N_) __declspec(align(_N_))
#define FOG_ALIGN_END(_N_)
#define FOG_ALIGNED_TYPE(_Type_, _N_) __declspec(align(_N_)) _Type_
#define FOG_ALIGNED_VAR(_Type_, _Name_, _N_) __declspec(align(_N_)) _Type_ _Name_

// ============================================================================
// [Fog::Core::C++ - MSC - Attributes]
// ============================================================================

#define FOG_NO_RETURN __declspec(noreturn)
#define FOG_DEPRECATED __declspec(deprecated)
#define FOG_RESTRICT __restrict

// ============================================================================
// [Fog::Core::C++ - MSC - Calling Conventions]
// ============================================================================

#if defined(FOG_ARCH_X86)
# define FOG_FASTCALL __fastcall
# define FOG_STDCALL __stdcall
# define FOG_CDECL __cdecl
#else
# define FOG_FASTCALL
# define FOG_STDCALL
# define FOG_CDECL
#endif

// ============================================================================
// [Fog::Core::C++ - MSC - Inline / No-Inline]
// ============================================================================

#define FOG_INLINE __forceinline
#define FOG_NO_INLINE

// ============================================================================
// [Fog::Core::C++ - MSC - Likely / Unlikely]
// ============================================================================

#define FOG_LIKELY(_Exp_) (_Exp_)
#define FOG_UNLIKELY(_Exp_) (_Exp_)

// ============================================================================
// [Fog::Core::C++ - MSC - Native Types]
// ============================================================================

// MSC treats char, signed char, and unsigned char as different types.
#define FOG_CC_HAS_NATIVE_CHAR_TYPE

// MSC must be configured to treat wchar_t and unsigned short as different types.
#if defined(_NATIVE_WCHAR_T_DEFINED)
# define FOG_CC_HAS_NATIVE_WCHAR_TYPE
#endif // _NATIVE_WCHAR_T_DEFINED

// ============================================================================
// [Fog::Core::C++ - MSC - Macros]
// ============================================================================

#define FOG_MACRO_BEGIN do {
#define FOG_MACRO_END } while (0)

// ============================================================================
// [Fog::Core::C++ - MSC - No-Throw]
// ============================================================================

#define FOG_NOTHROW throw()

// ============================================================================
// [Fog::Core::C++ - MSC - Unused]
// ============================================================================

#define FOG_UNUSED(a) (void)(a)

// ============================================================================
// [Fog::Core::C++ - MSC - Templates]
// ============================================================================

# define FOG_STATIC_T
# define FOG_STATIC_INLINE_T FOG_INLINE

// ============================================================================
// [Fog::Core::C++ - MSC - Features]
// ============================================================================

// C++0x features status:
//   http://blogs.msdn.com/b/vcblog/archive/2010/04/06/c-0x-core-language-features-in-vc10-the-table.aspx
//   http://blogs.msdn.com/b/vcblog/archive/2011/09/12/10209291.aspx

#if FOG_CC_MSC >= 1300
# define FOG_CC_HAS_PARTIAL_TEMPLATE_SPECIALIZATION
#endif // FOG_CC_MSC >= 1300

#if FOG_CC_MSC >= 1400
# define FOG_CC_HAS_OVERRIDE
#endif // FOG_CC_MSC >= 1400

#if FOG_CC_MSC >= 1600
# define FOG_CC_HAS_DECLTYPE
# define FOG_CC_HAS_LAMBDA
# define FOG_CC_HAS_NULLPTR
# define FOG_CC_HAS_RVALUE
# define FOG_CC_HAS_STATIC_ASSERT
#endif // FOG_CC_MSC >= 1600

// ============================================================================
// [Fog::Core::C++ - MSC - Visibility]
// ============================================================================

#define FOG_NO_EXPORT
#define FOG_DLL_IMPORT __declspec(dllimport)
#define FOG_DLL_EXPORT __declspec(dllexport)

#define FOG_CVAR_EXTERN_BASE(_Api_) extern "C" _Api_
#define FOG_CVAR_DECLARE_BASE(_Api_) _Api_

#define FOG_CAPI_EXTERN_BASE(_Api_) extern "C" _Api_
#define FOG_CAPI_DECLARE_BASE(_Api_) extern "C" _Api_

#define FOG_INIT_EXTERN_BASE(_Api_) extern "C" FOG_NO_EXPORT
#define FOG_INIT_DECLARE_BASE(_Api_) extern "C" FOG_NO_EXPORT

// ============================================================================
// [Fog::Core::C++ - MSC - Disable Warnings]
// ============================================================================

#pragma warning(disable:4146) // Unary minus operator applied to unsigned type, result still unsigned.
#pragma warning(disable:4251) // Struct '...' needs to have dll-interface to be used by clients of struct '...'.
#pragma warning(disable:4800) // Performance warning: casting to bool.
#pragma warning(disable:4275) // Non dll-interface struct X used as base for dll-interface struct Y.
#pragma warning(disable:4102) // Unreferenced label (we are using this is event mechanism).

// [Guard]
#endif // _FOG_CORE_CPP_COMPILER_MSC_H
