// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_CPP_COMPILER_GNU_H
#define _FOG_CORE_CPP_COMPILER_GNU_H

// ============================================================================
// [Fog::Core::C++ - GNU - Version]
// ============================================================================

#define FOG_CC_GNU __GNUC__
#define FOG_CC_GNU_VERSION_EQ(_Major_, _Minor_, _Patch_) \
  ( (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__) >= _Major_ * 10000 + _Minor_ * 100 + _Patch_)
#define FOG_CC_GNU_VERSION_GE(_Major_, _Minor_, _Patch_) \
  ( (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__) >= _Major_ * 10000 + _Minor_ * 100 + _Patch_)

// ============================================================================
// [Fog::Core::C++ - GNU - Alignment]
// ============================================================================

#define FOG_ALIGN_BEGIN(_N_)
#define FOG_ALIGN_END(_N_) __attribute__((aligned(_N_)))
#define FOG_ALIGNED_TYPE(_Type_, _N_) __attribute__((aligned(_N_))) _Type_
#define FOG_ALIGNED_VAR(_Type_, _Name_, _N_) _Type_ __attribute__((aligned(_N_))) _Name_

// ============================================================================
// [Fog::Core::C++ - GNU - Attributes]
// ============================================================================

#define FOG_NO_RETURN __attribute__((noreturn))
#define FOG_DEPRECATED __attribute__((deprecated))

#if defined(__GNUC__) && __GNUC__ >= 3
# define FOG_RESTRICT __restrict__
#else
# define FOG_RESTRICT
#endif

// ============================================================================
// [Fog::Core::C++ - GNU - Calling Conventions]
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
// [Fog::Core::C++ - GNU - Inline / No-Inline]
// ============================================================================

#if defined(__MINGW32__)
// MinGW has very strange problem that it can't inline some really trivial
// functions. To workaround this bug we define FOG_INLINE to inline, but this
// means that many blitters won't be inlined, degrading performance a lot.
# define FOG_INLINE inline
#else
# define FOG_INLINE inline __attribute__((always_inline))
#endif

#define FOG_NO_INLINE __attribute__((noinline))

// ============================================================================
// [Fog::Core::C++ - GNU - Likely / Unlikely]
// ============================================================================

#define FOG_LIKELY(_Exp_) __builtin_expect(!!(_Exp_), 1)
#define FOG_UNLIKELY(_Exp_) __builtin_expect(!!(_Exp_), 0)

// ============================================================================
// [Fog::Core::C++ - GNU - Native Types]
// ============================================================================

#define FOG_CC_HAS_NATIVE_CHAR_TYPE
#define FOG_CC_HAS_NATIVE_WCHAR_TYPE

// ============================================================================
// [Fog::Core::C++ - GNU - Macros]
// ============================================================================

#define FOG_MACRO_BEGIN ({
#define FOG_MACRO_END })

// ============================================================================
// [Fog::Core::C++ - GNU - No-Throw]
// ============================================================================

#define FOG_NOTHROW throw()

// ============================================================================
// [Fog::Core::C++ - GNU - Unused]
// ============================================================================

#define FOG_UNUSED(a) (void)(a)

// ============================================================================
// [Fog::Core::C++ - GNU - Templates]
// ============================================================================

#define FOG_STATIC_T
#define FOG_STATIC_INLINE_T FOG_INLINE

// ============================================================================
// [Fog::Core::C++ - GNU - Features]
// ============================================================================

// C++0x features status:
//   http://gcc.gnu.org/projects/cxx0x.html

#define FOG_CC_HAS_PARTIAL_TEMPLATE_SPECIALIZATION

#if defined(__GXX_EXPERIMENTAL_CXX0X__)

# if FOG_CC_GNU_VERSION_GE(4, 3, 0)
#  define FOG_CC_HAS_DECLTYPE
#  define FOG_CC_HAS_RVALUE
#  define FOG_CC_HAS_STATIC_ASSERT
# endif // GCC >= 4.3.0

# if FOG_CC_GNU_VERSION_GE(4, 4, 0)
#  define FOG_CC_HAS_DEFAULT_FUNCTION
#  define FOG_CC_HAS_DELETE_FUNCTION
# endif // GCC >= 4.4.0

# if FOG_CC_GNU_VERSION_GE(4, 5, 0)
#  define FOG_CC_HAS_INITIALIZER_LIST
#  define FOG_CC_HAS_LAMBDA
# endif // GCC >= 4.5.0

# if FOG_CC_GNU_VERSION_GE(4, 6, 0)
#  define FOG_CC_HAS_NULLPTR
# endif // GCC >= 4.6.0

# if FOG_CC_GNU_VERSION_GE(4, 7, 0)
#  define FOG_CC_HAS_OVERRIDE
# endif // GCC >= 4.7.0

#endif // __GXX_EXPERIMENTAL_CXX0X__

// ============================================================================
// [Fog::Core::C++ - GNU - Visibility]
// ============================================================================

// Following code is better than the code we use, but in this case GCC shows
// incredible number of warnings. So we assume that the default visibility is
// "hidden" and Fog-API visibility is "default".
//
// #if __GNUC__ >= 4
// # define FOG_NO_EXPORT __attribute__((visibility("hidden")))
// #else
// # define FOG_NO_EXPORT
// #endif
#define FOG_NO_EXPORT

#if defined(FOG_OS_WINDOWS)
# define FOG_DLL_IMPORT __declspec(dllimport)
# define FOG_DLL_EXPORT __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
# define FOG_DLL_IMPORT
# define FOG_DLL_EXPORT __attribute__((visibility("default")))
#else
# define FOG_DLL_IMPORT
# define FOG_DLL_EXPORT
#endif

#define FOG_CVAR_EXTERN_BASE(_Api_) extern "C" _Api_
#define FOG_CVAR_DECLARE_BASE(_Api_)

#define FOG_CAPI_EXTERN_BASE(_Api_) extern "C" _Api_
#define FOG_CAPI_DECLARE_BASE(_Api_) extern "C"

// [Guard]
#endif // _FOG_CORE_CPP_COMPILER_GNU_H
