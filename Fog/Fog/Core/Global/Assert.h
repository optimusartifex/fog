// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_GLOBAL_ASSERT_H
#define _FOG_CORE_GLOBAL_ASSERT_H

// [Dependencies]
#include <Fog/Core/Config/Config.h>
#include <Fog/Core/Global/Debug.h>

//! @addtogroup Fog_Core_Global
//! @{

// ============================================================================
// [FOG_ASSERT - DEBUG]
// ============================================================================

#if defined(FOG_DEBUG)

#define FOG_ASSERT(_Expression_) \
  FOG_MACRO_BEGIN \
    if (!(_Expression_)) \
      ::Fog::Debug::assertMessage(__FILE__, __LINE__, #_Expression_); \
  FOG_MACRO_END

#define FOG_ASSERT_X(_Expression_, _Message_) \
  FOG_MACRO_BEGIN \
    if (!(_Expression_)) \
      ::Fog::Debug::assertFormat(__FILE__, __LINE__, _Message_); \
  FOG_MACRO_END

#define FOG_ASSERT_NOT_REACHED() \
  FOG_MACRO_BEGIN \
    ::Fog::Debug::assertMessage(__FILE__, __LINE__, "Assert Not Reached."); \
  FOG_MACRO_END

#define FOG_ASSUME(_Expression_) \
  FOG_MACRO_BEGIN \
    if (!(_Expression_)) \
      ::Fog::Debug::assertMessage(__FILE__, __LINE__, #_Expression_); \
  FOG_MACRO_END

#else // !FOG_DEBUG

// ============================================================================
// [FOG_ASSERT - RELEASE]
// ============================================================================

#define FOG_ASSERT(_Expression_) FOG_NOP
#define FOG_ASSERT_X(_Expression_, _Message_) FOG_NOP

#if defined(FOG_CC_MSC)

#define FOG_ASSUME(_Expression_) __assume(_Expression_)
#define FOG_ASSERT_NOT_REACHED() __assume(0)

#elif defined(FOG_CC_GNU) && FOG_CC_GNU_VERSION_GE(4, 5, 0)

#define FOG_ASSUME(_Expression_) \
  FOG_MACRO_BEGIN \
    if (!(_Expression_)) __builtin_unreachable(); \
  FOG_MACRO_END

#define FOG_ASSERT_NOT_REACHED() \
  __builtin_unreachable()

#else

#define FOG_ASSUME(_Expression_) FOG_NOP
#define FOG_ASSERT_NOT_REACHED(_Expression_) FOG_NOP

#endif

#endif // FOG_DEBUG

//! @}

// [Guard]
#endif // _FOG_CORE_GLOBAL_ASSERT_H
