// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_CPP_COMPILER_CLANG_H
#define _FOG_CORE_CPP_COMPILER_CLANG_H

// ============================================================================
// [Fog::Core::C++ - Clang - Version]
// ============================================================================

#define FOG_CC_CLANG __clang__
#define FOG_CC_CLANG_VERSION_EQ(_Major_, _Minor_, _Patch_) \
  ( (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__) >= _Major_ * 10000 + _Minor_ * 100 + _Patch_)
#define FOG_CC_CLANG_VERSION_GE(_Major_, _Minor_, _Patch_) \
  ( (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__) >= _Major_ * 10000 + _Minor_ * 100 + _Patch_)

// ============================================================================
// [Fog::Core::C++ - Clang - Alignment]
// ============================================================================

#define FOG_ALIGN_BEGIN(_N_)
#define FOG_ALIGN_END(_N_) __attribute__((aligned(_N_)))
#define FOG_ALIGNED_TYPE(_Type_, _N_) __attribute__((aligned(_N_))) _Type_
#define FOG_ALIGNED_VAR(_Type_, _Name_, _N_) _Type_ __attribute__((aligned(_N_))) _Name_

// ============================================================================
// [Fog::Core::C++ - Clang - Attributes]
// ============================================================================

#define FOG_NO_RETURN __attribute__((noreturn))
#define FOG_DEPRECATED __attribute__((deprecated))
#define FOG_RESTRICT __restrict__

// ============================================================================
// [Fog::Core::C++ - Clang - Calling Conventions]
// ============================================================================

#if defined(FOG_ARCH_X86)
# define FOG_FASTCALL __attribute__((regparm(3)))
# define FOG_STDCALL __attribute__((stdcall))
# define FOG_CDECL __attribute__((cdecl))
#else
# define FOG_FASTCALL
# define FOG_STDCALL
# define FOG_CDECL
#endif

// ============================================================================
// [Fog::Core::C++ - Clang - Inline / No-Inline]
// ============================================================================

#define FOG_INLINE inline __attribute__((always_inline))
#define FOG_NO_INLINE __attribute__((noinline))

// ============================================================================
// [Fog::Core::C++ - Clang - Likely / Unlikely]
// ============================================================================

#define FOG_LIKELY(_Exp_) __builtin_expect(!!(_Exp_), 1)
#define FOG_UNLIKELY(_Exp_) __builtin_expect(!!(_Exp_), 0)

// ============================================================================
// [Fog::Core::C++ - Clang - Native Types]
// ============================================================================

#define FOG_CC_HAS_NATIVE_CHAR_TYPE
#define FOG_CC_HAS_NATIVE_WCHAR_TYPE

// ============================================================================
// [Fog::Core::C++ - Clang - Macros]
// ============================================================================

#define FOG_MACRO_BEGIN ({
#define FOG_MACRO_END })

// ============================================================================
// [Fog::Core::C++ - Clang - No-Throw]
// ============================================================================

#define FOG_NOTHROW throw()

// ============================================================================
// [Fog::Core::C++ - Clang - Unused]
// ============================================================================

#define FOG_UNUSED(a) (void)(a)

// ============================================================================
// [Fog::Core::C++ - Clang - Templates]
// ============================================================================

# define FOG_STATIC_T
# define FOG_STATIC_INLINE_T FOG_INLINE

// ============================================================================
// [Fog::Core::C++ - Clang - Features]
// ============================================================================

// C++0x features status:
//   http://clang.llvm.org/cxx_status.html

#define FOG_CC_HAS_PARTIAL_TEMPLATE_SPECIALIZATION

#if FOG_CC_CLANG_VERSION_GE(2, 9, 0)
# define FOG_CC_HAS_DECLTYPE
# define FOG_CC_HAS_DELETE_FUNCTION
# define FOG_CC_HAS_RVALUE
# define FOG_CC_HAS_STATIC_ASSERT
#endif

#if FOG_CC_CLANG_VERSION_GE(3, 0, 0)
# define FOG_CC_HAS_DEFAULT_FUNCTION
# define FOG_CC_HAS_NULLPTR
# define FOG_CC_HAS_OVERRIDE
#endif

// ============================================================================
// [Fog::Core::C++ - Clang - Visibility]
// ============================================================================

#define FOG_NO_EXPORT

#if defined(FOG_OS_WINDOWS)
# define FOG_DLL_IMPORT __declspec(dllimport)
# define FOG_DLL_EXPORT __declspec(dllexport)
#elif defined(__clang__)
# define FOG_DLL_IMPORT
# define FOG_DLL_EXPORT __attribute__((visibility("default")))
#else
# define FOG_DLL_IMPORT
# define FOG_DLL_EXPORT
#endif

// Variables.
#define FOG_CVAR_EXTERN_BASE(_Api_) extern "C" _Api_
#define FOG_CVAR_DECLARE_BASE(_Api_)

// C API.
#define FOG_CAPI_EXTERN_BASE(_Api_) extern "C" _Api_
#define FOG_CAPI_DECLARE_BASE(_Api_) extern "C"

// [Guard]
#endif // _FOG_CORE_CPP_COMPILER_CLANG_H
