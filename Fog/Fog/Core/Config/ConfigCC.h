// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

#ifndef _FOG_CORE_CONFIG_CONFIGCC_H
#define _FOG_CORE_CONFIG_CONFIGCC_H

#if !defined(FOG_IDE) && !defined(_FOG_CORE_CONFIG_CONFIG_H)
#error "Fog::Core::Config - ConfigCC.h can be only included by Fog/Core/Config/Config.h"
#endif // _FOG_CORE_CONFIG_CONFIG_H

// ============================================================================
// [Fog::Core::Config - C++ Compiler]
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
# include "ConfigCC_ide.h"
#elif defined(__GNUC__) || defined(__clang__)
# include "ConfigCC_gcc_compat.h"
#elif defined(_MSC_VER) || defined(__BORLANDC__)
# include "ConfigCC_msc_compat.h"
#else
# error "Fog::Core::ConfigCC - C++ Compiler - Unsupported"
#endif

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

#define FOG_INIT_EXTERN FOG_INIT_EXTERN_BASE(FOG_API)
#define FOG_INIT_DECLARE FOG_INIT_DECLARE_BASE(FOG_API)

//! @}

#endif // _FOG_CORE_CONFIG_CONFIGCC_H
