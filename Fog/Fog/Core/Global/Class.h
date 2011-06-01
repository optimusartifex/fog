// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_GLOBAL_CLASS_H
#define _FOG_CORE_GLOBAL_CLASS_H

// [Dependencies]
#include <Fog/Core/Config/Config.h>

//! @addtogroup Fog_Core_Global
//! @{

// ============================================================================
// [_FOG_CLASS...]
// ============================================================================

//! @brief Creates @c SelfType typedef inside class.
//!
//! @sa @c _FOG_CLASS_D
#define _FOG_CLASS_SELF(_SelfType_) \
  typedef _SelfType_ _Self;

//! @brief Creates @c _D typedef and @c _d member inside class
//!
//! @sa @c _FOG_CLASS_SELF, @c _FOG_CLASS_D_METHODS
#define _FOG_CLASS_D(_DType_) \
  typedef _DType_ _D; \
  \
  _DType_* _d;

//! @brief Use this macro to generate empty copy constructor and empty copy operator.
//! Use in 'private' section.
#define _FOG_CLASS_NO_COPY(_SelfType_) \
  FOG_INLINE FOG_NO_EXPORT _SelfType_(const _SelfType_& other); \
  FOG_INLINE FOG_NO_EXPORT _SelfType_& operator=(const _SelfType_& other);

//! @}

// [Guard]
#endif // _FOG_CORE_GLOBAL_CLASS_H
