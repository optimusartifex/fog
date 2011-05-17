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
  template<typename NumT> \
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
  template<typename NumT> \
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

// ============================================================================
// [FOG_TYPEVARIANT_DECLARE_TEMPLATE1_F_D]
// ============================================================================

#define FOG_TYPEVARIANT_DECLARE_TEMPLATE1_F_D(_Type_, _T1_, _A1_) \
  template<typename NumT, _T1_ _A1_> \
  struct _Type_##T {}; \
  \
  template<_T1_ _A1_> \
  struct _Type_##T<float, _A1_> { typedef _Type_##F<_A1_> T; }; \
  \
  template<_T1_ _A1_> \
  struct _Type_##T<double, _A1_> { typedef _Type_##D<_A1_> T; };

#define FOG_DECLARE_TYPEINFO_TEMPLATE1(__symbol__, T1, A1, __typeinfo__) \
namespace Fog { \
template <T1 A1> \
struct TypeInfo < __symbol__<A1> > : public TypeInfo_Wrapper< __symbol__<A1>, __typeinfo__ > {}; \
}


// [Guard]
#endif // _FOG_CORE_GLOBAL_TYPEVARIANT_H
