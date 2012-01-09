// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_GLOBAL_CLASS_H
#define _FOG_CORE_GLOBAL_CLASS_H

// [Dependencies]
#include <Fog/Core/C++/Base.h>

//! @addtogroup Fog_Core_Global
//! @{

// ============================================================================
// [_FOG_CLASS_D]
// ============================================================================

//! @brief Creates @c _D typedef and @c _d member inside class
#define _FOG_CLASS_D(_DType_) \
  typedef _DType_ _D; \
  \
  _DType_* _d;

// ============================================================================
// [_FOG_NO_COPY]
// ============================================================================

//! @def _FOG_NO_COPY
//!
//! @brief Use this macro to generate empty copy constructor and empty copy operator.
//!
//! Use in 'private' section.

#if defined(FOG_CC_HAS_DELETE_FUNCTION)
#define _FOG_NO_COPY(_SelfType_) \
  _SelfType_(const _SelfType_& other) = delete; \
  _SelfType_& operator=(const _SelfType_& other) = delete;
#else
#define _FOG_NO_COPY(_SelfType_) \
  FOG_INLINE _SelfType_(const _SelfType_& other); \
  FOG_INLINE _SelfType_& operator=(const _SelfType_& other);
#endif // FOG_CC_HAS_DELETE_FUNCTION

// ============================================================================
// [_FOG_CHAR]
// ============================================================================

#define _FOG_CHAR_T(_Type_) \
  template<typename CharT_Type_> \
  struct _Type_##T {};

#define _FOG_CHAR_A(_Type_) \
  template<> \
  struct _Type_##T<char> { typedef _Type_##A T; }; \
  template<> \
  struct _Type_##T< ::Fog::CharA > { typedef _Type_##A T; };

#define _FOG_CHAR_W(_Type_) \
  template<> \
  struct _Type_##T<uint16_t> { typedef _Type_##W T; }; \
  template<> \
  struct _Type_##T< ::Fog::CharW > { typedef _Type_##W T; };

// ============================================================================
// [_FOG_CHAR_TEMPLATE1]
// ============================================================================

#define _FOG_CHAR_TEMPLATE1_T(_Type_, _T1_, _A1_) \
  template<typename CharT, _T1_ _A1_> \
  struct _Type_##T {};

#define _FOG_CHAR_TEMPLATE1_A(_Type_, _T1_, _A1_) \
  template<_T1_ _A1_> \
  struct _Type_##T<char, _A1_> { typedef _Type_##A<_A1_> T; }; \
  template<_T1_ _A1_> \
  struct _Type_##T<CharA, _A1_> { typedef _Type_##A<_A1_> T; };

#define _FOG_CHAR_TEMPLATE1_W(_Type_, _T1_, _A1_) \
  template<_T1_ _A1_> \
  struct _Type_##T<uint16_t, _A1_> { typedef _Type_##W<_A1_> T; }; \
  template<_T1_ _A1_> \
  struct _Type_##T<CharW, _A1_> { typedef _Type_##W<_A1_> T; };

// ============================================================================
// [_FOG_NUM]
// ============================================================================

#define _FOG_NUM_T(_Type_) \
  template<typename NumT> \
  struct _Type_##T {};

#define _FOG_NUM_I(_Type_) \
  template<> \
  struct _Type_##T<int> { typedef _Type_##I T; };

#define _FOG_NUM_F(_Type_) \
  template<> \
  struct _Type_##T<float> { typedef _Type_##F T; };

#define _FOG_NUM_D(_Type_) \
  template<> \
  struct _Type_##T<double> { typedef _Type_##D T; };

// ============================================================================
// [_FOG_NUM_TEMPLATE1]
// ============================================================================

#define _FOG_NUM_TEMPLATE1_T(_Type_, _T1_, _A1_) \
  template<typename NumT, _T1_ _A1_> \
  struct _Type_##T {};

#define _FOG_NUM_TEMPLATE1_I(_Type_, _T1_, _A1_) \
  template<_T1_ _A1_> \
  struct _Type_##T<int, _A1_> { typedef _Type_##I<_A1_> T; };

#define _FOG_NUM_TEMPLATE1_F(_Type_, _T1_, _A1_) \
  template<_T1_ _A1_> \
  struct _Type_##T<float, _A1_> { typedef _Type_##F<_A1_> T; };

#define _FOG_NUM_TEMPLATE1_D(_Type_, _T1_, _A1_) \
  template<_T1_ _A1_> \
  struct _Type_##T<double, _A1_> { typedef _Type_##D<_A1_> T; };

//! @}

// [Guard]
#endif // _FOG_CORE_GLOBAL_CLASS_H
