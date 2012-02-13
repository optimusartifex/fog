// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_CPP_COMPILER_BORLAND_H
#define _FOG_CORE_CPP_COMPILER_BORLAND_H

// ============================================================================
// [Fog::Core::C++ - Borland/CodeGear - Version]
// ============================================================================

// 0x0520 C++ Builder 1
// 0x0530 C++ Builder 3
// 0x0540 C++ Builder 4
// 0x0550 C++ Builder 5
// 0x0560 C++ Builder 6
// 0x0570 BDS 2006
// 0x0590 C++ Builder 2007
// 0x0591 C++ Builder 2007 Update 1
// 0x0592 RAD Studio 2007
// 0x0593 RAD Studio 2007 Update 1
// 0x0610 C++ Builder 2009 Update 1
// 0x0620 C++ Builder 2010 Update 1
// 0x0621 C++ Builder 2010 Update 2
// 0x0630 C++ Builder XE
// 0x0631 C++ Builder XE Update 1
// 0x0640 C++ Builder XE2

#if defined(__CODEGEARC__)
# define FOG_CC_BORLAND __CODEGEARC__
#else
# define FOG_CC_BORLAND __BORLANDC__
#endif

// ============================================================================
// [Fog::Core::C++ - Borland/CodeGear - Alignment]
// ============================================================================

#define FOG_ALIGN_BEGIN(_N_)
#define FOG_ALIGN_END(_N_)
#define FOG_ALIGNED_TYPE(_Type_, _N_) _Type_
#define FOG_ALIGNED_VAR(_Type_, _Name_, _N_) _Type_ _Name_

// ============================================================================
// [Fog::Core::C++ - Borland/CodeGear - Attributes]
// ============================================================================

#if FOG_CC_BORLAND >= 0x0610
# define FOG_NO_RETURN __declspec(noreturn)
#else
# define FOG_NO_RETURN
#endif

#define FOG_DEPRECATED __declspec(deprecated)
#define FOG_RESTRICT

// ============================================================================
// [Fog::Core::C++ - Borland/CodeGear - Calling Conventions]
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
// [Fog::Core::C++ - Borland/CodeGear - Inline / No-Inline]
// ============================================================================

#define FOG_INLINE inline
#define FOG_NO_INLINE

// ============================================================================
// [Fog::Core::C++ - Borland/CodeGear - Likely / Unlikely]
// ============================================================================

#define FOG_LIKELY(_Exp_) (_Exp_)
#define FOG_UNLIKELY(_Exp_) (_Exp_)

// ============================================================================
// [Fog::Core::C++ - Borland/CodeGear - Native Types]
// ============================================================================

// TODO: Borland Compiler - Is char native type?
#define FOG_CC_HAS_NATIVE_CHAR_TYPE
// TODO: Borland Compiler - Is wchar_t native type?
#define FOG_CC_HAS_NATIVE_WCHAR_TYPE

// ============================================================================
// [Fog::Core::C++ - Borland/CodeGear - Macros]
// ============================================================================

#define FOG_MACRO_BEGIN do {
#define FOG_MACRO_END } while (0)

// ============================================================================
// [Fog::Core::C++ - Borland/CodeGear - No-Throw]
// ============================================================================

#define FOG_NOTHROW throw()

// ============================================================================
// [Fog::Core::C++ - Borland/CodeGear - Unused]
// ============================================================================

#define FOG_UNUSED(a) (void)(a)

// ============================================================================
// [Fog::Core::C++ - Borland/CodeGear - Templates]
// ============================================================================

# define FOG_STATIC_T
# define FOG_STATIC_INLINE_T FOG_INLINE

// ============================================================================
// [Fog::Core::C++ - Borland/CodeGear - Features]
// ============================================================================

// C++0x features status:
//   http://docs.codegear.com/products/rad_studio/delphiAndcpp2009/HelpUpdate2/EN/html/devcommon/whatsnewtiburon_xml.html

// Features setup.
// TODO: Borland Compiler - Has BCC partial template specialization, which version?

#if FOG_CC_BORLAND >= 0x0610
# define FOG_CC_HAS_DECLTYPE
# define FOG_CC_HAS_PARTIAL_TEMPLATE_SPECIALIZATION
# define FOG_CC_HAS_RVALUE
# define FOG_CC_HAS_STATIC_ASSERT
#endif // FOG_CC_BORLAND >= 0x0613

// ============================================================================
// [Fog::Core::C++ - Borland/CodeGear - Visibility]
// ============================================================================

#define FOG_NO_EXPORT

#define FOG_DLL_IMPORT __declspec(dllimport)
#define FOG_DLL_EXPORT __declspec(dllexport)

#define FOG_CVAR_EXTERN_BASE(_Api_) extern "C" _Api_
#define FOG_CVAR_DECLARE_BASE(_Api_) _Api_

#define FOG_CAPI_EXTERN_BASE(_Api_) extern "C" _Api_
#define FOG_CAPI_DECLARE_BASE(_Api_) extern "C" _Api_

// ============================================================================
// [Fog::Core::C++ - Borland/CodeGear - Disable Warnings]
// ============================================================================

#pragma warn -8008 // Condition is always true.
#pragma warn -8057 // Parameter is never used.
#pragma warn -8059 // Structure packing was changed.
#pragma warn -8066 // Unrecheable code.

// [Guard]
#endif // _FOG_CORE_CPP_COMPILER_BORLAND_H
