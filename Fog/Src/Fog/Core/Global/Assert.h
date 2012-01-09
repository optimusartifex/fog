// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_GLOBAL_ASSERT_H
#define _FOG_CORE_GLOBAL_ASSERT_H

// [Dependencies]
#include <Fog/Core/C++/Base.h>

//! @addtogroup Fog_Core_Global
//! @{

FOG_CAPI_EXTERN void FOG_NO_RETURN fog_assert_message(const char* file, int line, const char* msg);
FOG_CAPI_EXTERN void FOG_NO_RETURN fog_assert_format(const char* file, int line, const char* fmt, ...);

//! @def FOG_ASSERT
//!
//! @brief Run-time assertion used when debugging.

//! @def FOG_ASSERT_NOT_REACHED
//!
//! @brief Run-time assertion used in code that should be never reached.

//! @def FOG_ASSERT_X
//!
//! @brief Run-time assertion used when debugging with custom message.

// ============================================================================
// [FOG_ASSERT - DEBUG]
// ============================================================================

#if defined(FOG_DEBUG)

# define FOG_ASSERT(_Expression_) \
  FOG_MACRO_BEGIN \
    if (!(_Expression_)) \
      fog_assert_message(__FILE__, __LINE__, #_Expression_); \
  FOG_MACRO_END

# define FOG_ASSERT_ALIGNED(_Ptr_, _Alignment_) \
  FOG_MACRO_BEGIN \
    if (((intptr_t)(_Ptr_) & ((_Alignment_) - 1)) != 0) \
      fog_assert_format(__FILE__, __LINE__, "Pointer not aligned to %d bytes.", (int)(_Alignment_)); \
  FOG_MACRO_END

# define FOG_ASSERT_NOT_REACHED() \
  FOG_MACRO_BEGIN \
    fog_assert_message(__FILE__, __LINE__, "Assert Not Reached."); \
  FOG_MACRO_END

# define FOG_ASSERT_X(_Expression_, _Message_) \
  FOG_MACRO_BEGIN \
    if (!(_Expression_)) \
      fog_assert_message(__FILE__, __LINE__, _Message_); \
  FOG_MACRO_END

# define FOG_ASSUME(_Expression_) \
  FOG_MACRO_BEGIN \
    if (!(_Expression_)) \
      fog_assert_message(__FILE__, __LINE__, #_Expression_); \
  FOG_MACRO_END

// TODO: Declare and use.
// #define FOG_ASSERT_STATIC(_Expression_)

#else // !FOG_DEBUG

// ============================================================================
// [FOG_ASSERT - RELEASE]
// ============================================================================

# define FOG_ASSERT(_Expression_) FOG_NOP
# define FOG_ASSERT_ALIGNED(_Ptr_, _Alignment_) FOG_NOP
# define FOG_ASSERT_X(_Expression_, _Message_) FOG_NOP

# if defined(FOG_CC_MSC)

#  define FOG_ASSUME(_Expression_) __assume(_Expression_)
#  define FOG_ASSERT_NOT_REACHED() __assume(0)

# elif defined(FOG_CC_GNU) && FOG_CC_GNU_VERSION_GE(4, 5, 0)

#  define FOG_ASSUME(_Expression_) \
  FOG_MACRO_BEGIN \
    if (!(_Expression_)) __builtin_unreachable(); \
  FOG_MACRO_END

#  define FOG_ASSERT_NOT_REACHED() \
  __builtin_unreachable()

# else

#  define FOG_ASSUME(_Expression_) FOG_NOP
#  define FOG_ASSERT_NOT_REACHED(_Expression_) FOG_NOP

# endif

#endif // FOG_DEBUG

//! @}
  
// [Guard]
#endif // _FOG_CORE_GLOBAL_ASSERT_H
