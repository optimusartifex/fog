// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_GLOBAL_TYPEVARIANT_H
#define _FOG_CORE_GLOBAL_TYPEVARIANT_H

// [Dependencies]
#include <Fog/Core/Global/Api.h>

// ============================================================================
// [FOG_TYPEVARIANT_DECLARE_F_D]
// ============================================================================

#define FOG_TYPEVARIANT_DECLARE_F_D(_Type_) \
  template<typename Number> \
  struct _Type_##T {}; \
  \
  template<> \
  struct _Type_##T<float> { typedef _Type_##F T; }; \
  \
  template<> \
  struct _Type_##T<double> { typedef _Type_##D T; };

// ============================================================================
// [FOG_TYPEVARIANT_DECLARE_F_D_I]
// ============================================================================

#define FOG_TYPEVARIANT_DECLARE_F_D_I(_Type_) \
  template<typename Number> \
  struct _Type_##T {}; \
  \
  template<> \
  struct _Type_##T<float> { typedef _Type_##F T; }; \
  \
  template<> \
  struct _Type_##T<double> { typedef _Type_##D T; }; \
  \
  template<> \
  struct _Type_##T<int> { typedef _Type_##I T; };

// [Guard]
#endif // _FOG_CORE_GLOBAL_TYPEVARIANT_H
