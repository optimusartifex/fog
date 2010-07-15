// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

#ifndef _FOG_CORE_BUILD_STDCC_H
#define _FOG_CORE_BUILD_STDCC_H

#if !defined(FOG_IDE) && !defined(_FOG_BUILD_BUILD_H)
#error "Fog::Core::Build - StdCC.h can be only included by Fog/Core/Build.h"
#endif // _FOG_BUILD_BUILD_H

// ============================================================================
// [Fog::Core::Build - Standard Compiler Macros]
// ============================================================================

//! @addtogroup Fog_Core_Compiler
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

// ============================================================================
// [Fog::Core::Build - IDE (Macros for IDEs, for example NetBeans)]
// ============================================================================

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

// ============================================================================
// [Fog::Core::Build - Compiler - Clang]
// ============================================================================

#elif defined(__clang__)
#define FOG_CC_CLANG __clang__

// Standard attributes.
#define FOG_INLINE inline __attribute__((always_inline))
#define FOG_NO_INLINE __attribute__((noinline))
#define FOG_NO_RETURN __attribute__((noreturn))
#define FOG_DEPRECATED __attribute__((deprecated))

// Restrict support.
# define FOG_RESTRICT __restrict__

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
# define FOG_HIDDEN __attribute__((visibility("hidden")))
#else
# define FOG_HIDDEN
#endif

#if defined(FOG_OS_WINDOWS)
# define FOG_DLL_IMPORT __declspec(dllimport)
# define FOG_DLL_EXPORT __declspec(dllexport)
#else
# define FOG_DLL_IMPORT
# define FOG_DLL_EXPORT __attribute__((visibility("default")))
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
# define FOG_CVAR_EXTERN_BASE(api) extern "C"
# define FOG_CVAR_DECLARE_BASE(api)

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

// ============================================================================
// [Fog::Core::Build - Compiler - GNU C/C++]
// ============================================================================

#elif defined(__GNUC__)
#define FOG_CC_GNU __GNUC__

// Standard attributes.
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
# define FOG_HIDDEN __attribute__((visibility("hidden")))
#else
# define FOG_HIDDEN
#endif

#if defined(FOG_OS_WINDOWS)
# define FOG_DLL_IMPORT __declspec(dllimport)
# define FOG_DLL_EXPORT __declspec(dllexport)
#elif (__GNUC__ >= 4)
# define FOG_DLL_IMPORT
# define FOG_DLL_EXPORT __attribute__((visibility("default")))
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
# define FOG_CVAR_EXTERN_BASE(api) extern "C" api
# define FOG_CVAR_DECLARE_BASE(api)
#endif // __MINGW32__

// C API.
#define FOG_CAPI_EXTERN_BASE(api) extern "C" api
#define FOG_CAPI_DECLARE_BASE(api) extern "C"

// C API - static initializers we can hide them...
#define FOG_INIT_EXTERN_BASE(api) extern "C" FOG_HIDDEN
#define FOG_INIT_DECLARE_BASE(api) extern "C" FOG_HIDDEN

// API.
#if defined(Fog_EXPORTS)
# define FOG_API FOG_DLL_EXPORT
#else
# define FOG_API FOG_DLL_IMPORT
#endif // Fog_EXPORTS

// ============================================================================
// [Fog::Core::Build - Compiler - Borland C++ Builder or MSVC]
// ============================================================================

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

// ============================================================================
// [Fog::Core::Build - Unsupported Compiler]
// ============================================================================

// Unsupported compiler?
#else
# error "Unsupported compiler"
#endif

// ============================================================================
// [Fog::Core::Build - FOG_API]
// ============================================================================

#if defined(Fog_EXPORTS)
# define FOG_API FOG_DLL_EXPORT
#else
# define FOG_API FOG_DLL_IMPORT
#endif

#define FOG_CVAR_EXTERN FOG_CVAR_EXTERN_BASE(FOG_API)
#define FOG_CVAR_DECLARE FOG_CVAR_DECLARE_BASE(FOG_API)

#define FOG_CAPI_EXTERN FOG_CAPI_EXTERN_BASE(FOG_API)
#define FOG_CAPI_DECLARE FOG_CAPI_DECLARE_BASE(FOG_API)

#define FOG_INIT_EXTERN FOG_INIT_EXTERN_BASE(FOG_API)
#define FOG_INIT_DECLARE FOG_INIT_DECLARE_BASE(FOG_API)

//! @}

#endif // _FOG_CORE_BUILD_STDCC_H
