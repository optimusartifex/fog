// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

#ifndef _FOG_CORE_CPP_COMPILER_H
#define _FOG_CORE_CPP_COMPILER_H

#if !defined(FOG_IDE) && !defined(_FOG_CORE_CPP_BASE_H)
#error "Fog::Core::C++ - Compiler.h can be only included by Fog/Core/C++/Base.h"
#endif // _FOG_CORE_CPP_BASE_H

// [Dependencies]
#include <Fog/Core/C++/StdArch.h>
#include <Fog/Core/C++/StdOS.h>

// ============================================================================
// [Fog::Core::Config - C++ Compiler]
// ============================================================================

//! @addtogroup Fog_Core_Cpp_Compiler
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

//! @def FOG_RESTRICT
//! @brief Restrict operator (for compilers which not supports it).

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

//! @def FOG_NO_EXPORT
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

//! @}

#if defined(FOG_IDE)
# include "CompilerIde.h"
#elif defined(__BORLANDC__)
# include "CompilerBorland.h"
#elif defined(__clang__)
# include "CompilerClang.h"
#elif defined(__GNUC__)
# include "CompilerGnu.h"
#elif defined(_MSC_VER)
# include "CompilerMsc.h"
#else
# error "Fog::Core::C++ - Unsupported C++ Compiler, please fill a bug report."
#endif

// Define the C++ specific macros so those macros can be used in #if ... defs.
// If we don't do that then all preprocessor macros which depends on the C++
// compiler version won't work.

#if !defined(FOG_CC_GNU_VERSION_EQ)
# define FOG_CC_GNU_VERSION_EQ(_Major_, _Minor_, _Patch_) 0
# define FOG_CC_GNU_VERSION_GE(_Major_, _Minor_, _Patch_) 0
#endif // FOG_CC_GNU_VERSION_EQ

#if !defined(FOG_CC_CLANG_VERSION_EQ)
# define FOG_CC_CLANG_VERSION_EQ(_Major_, _Minor_, _Patch_) 0
# define FOG_CC_CLANG_VERSION_GE(_Major_, _Minor_, _Patch_) 0
#endif // FOG_CC_CLANG_VERSION_EQ

// Define the override attribute if C++ compiler can't use it.
#if !defined(FOG_CC_HAS_OVERRIDE)
# define override
#endif // FOG_CC_HAS_OVERRIDE

// ============================================================================
// [Fog::Core::Build - FOG_API]
// ============================================================================

#if !defined(FOG_API)
# if defined(FOG_BUILD_STATIC)
#  define FOG_API
# elif defined(Fog_EXPORTS)
#  define FOG_API FOG_DLL_EXPORT
# else
#  define FOG_API FOG_DLL_IMPORT
# endif
#endif

#define FOG_CVAR_EXTERN FOG_CVAR_EXTERN_BASE(FOG_API)
#define FOG_CVAR_DECLARE FOG_CVAR_DECLARE_BASE(FOG_API)

#define FOG_CAPI_EXTERN FOG_CAPI_EXTERN_BASE(FOG_API)
#define FOG_CAPI_DECLARE FOG_CAPI_DECLARE_BASE(FOG_API)

//! @}

#endif // _FOG_CORE_CPP_COMPILER_H
