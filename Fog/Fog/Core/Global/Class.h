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
// [_FOG_CLASS_SELF]
// ============================================================================

//! @brief Creates @c SelfType typedef inside class.
//!
//! @sa @c _FOG_CLASS_D
#define _FOG_CLASS_SELF(_SelfType_) \
  typedef _SelfType_ _Self;

// ============================================================================
// [_FOG_CLASS_D]
// ============================================================================

//! @brief Creates @c _D typedef and @c _d member inside class
//!
//! @sa @c _FOG_CLASS_SELF, @c _FOG_CLASS_D_METHODS
#define _FOG_CLASS_D(_DType_) \
  typedef _DType_ _D; \
  \
  _DType_* _d;

// ============================================================================
// [_FOG_CLASS_NO_COPY]
// ============================================================================

//! @brief Use this macro to generate empty copy constructor and empty copy operator.
//! Use in 'private' section.
#define _FOG_CLASS_NO_COPY(_SelfType_) \
  FOG_INLINE FOG_NO_EXPORT _SelfType_(const _SelfType_& other); \
  FOG_INLINE FOG_NO_EXPORT _SelfType_& operator=(const _SelfType_& other);

// ============================================================================
// [_FOG_CLASS_PRECISION_F_D]
// ============================================================================

#define FOG_CLASS_PRECISION_F_D(_Type_) \
  template<typename NumT> \
  struct _Type_##T {}; \
  \
  template<> \
  struct _Type_##T<float> { typedef _Type_##F T; }; \
  \
  template<> \
  struct _Type_##T<double> { typedef _Type_##D T; };

// ============================================================================
// [_FOG_CLASS_PRECISION_F_D_I]
// ============================================================================

#define FOG_CLASS_PRECISION_F_D_I(_Type_) \
  template<typename NumT> \
  struct _Type_##T {}; \
  \
  template<> \
  struct _Type_##T<int> { typedef _Type_##I T; }; \
  \
  template<> \
  struct _Type_##T<float> { typedef _Type_##F T; }; \
  \
  template<> \
  struct _Type_##T<double> { typedef _Type_##D T; };

// ============================================================================
// [_FOG_CLASS_PRECISION_TEMPLATE1_F_D]
// ============================================================================

#define FOG_CLASS_PRECISION_TEMPLATE1_F_D(_Type_, _T1_, _A1_) \
  template<typename NumT, _T1_ _A1_> \
  struct _Type_##T {}; \
  \
  template<_T1_ _A1_> \
  struct _Type_##T<float, _A1_> { typedef _Type_##F<_A1_> T; }; \
  \
  template<_T1_ _A1_> \
  struct _Type_##T<double, _A1_> { typedef _Type_##D<_A1_> T; };

//! @}

// [Guard]
#endif // _FOG_CORE_GLOBAL_CLASS_H
