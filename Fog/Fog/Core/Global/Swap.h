// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_GLOBAL_SWAP_H
#define _FOG_CORE_GLOBAL_SWAP_H

// [Dependencies]
#include <Fog/Core/Config/Config.h>

namespace Fog {

//! @addtogroup Fog_Core_Global
//! @{

// ============================================================================
// [Fog::Swap]
// ============================================================================

//! @brief Swap @a a and @a b.
template <typename T>
FOG_INLINE void swap(T& a, T& b)
{
  T c(a);
  a = b;
  b = c;
}

// ============================================================================
// [Fog::Swap - DECLARE]
// ============================================================================

#define _FOG_SWAP_DECLARE(_Symbol_, _Code_) \
\
namespace Fog { \
  template<> \
  FOG_INLINE void swap(_Symbol_& a, _Symbol_& b) \
  { \
    _Code_ \
  }; \
}

// ============================================================================
// [Fog::Swap - D]
// ============================================================================

#define _FOG_SWAP_D(_Symbol_) \
\
namespace Fog { \
  template<> \
  FOG_INLINE void swap(\
    ::_Symbol_& a, \
    ::_Symbol_& b) \
  { \
    ::_Symbol_::_D* a_d = a._d; \
    ::_Symbol_::_D* b_d = b._d; \
    \
    a._d = b_d; \
    b._d = a_d; \
  }; \
}

#define _FOG_SWAP_D_TEMPLATE1(_Symbol_, T1, A1) \
\
namespace Fog { \
  template <T1 A1> \
  FOG_INLINE void swap(\
    typename ::_Symbol_<A1>& a, \
    typename ::_Symbol_<A1>& b) \
  { \
    typename _Symbol_<A1>::_D* a_d = a._d; \
    typename _Symbol_<A1>::_D* b_d = b._d; \
    \
    a._d = b_d; \
    b._d = a_d; \
  } \
}

#define _FOG_SWAP_D_TEMPLATE2(_Symbol_, T1, A1, T2, A2) \
\
namespace Fog { \
  template <T1 A1, T2 A2> \
  FOG_INLINE void swap(\
    typename ::_Symbol_<A1, A2>& a, \
    typename ::_Symbol_<A1, A2>& b) \
  { \
    typename ::_Symbol_<A1, A2>::_D* a_d = a._d; \
    typename ::_Symbol_<A1, A2>::_D* b_d = b._d; \
    \
    a._d = b_d; \
    b._d = a_d; \
  } \
}

#define _FOG_SWAP_D_TEMPLATE3(_Symbol_, T1, A1, T2, A2, T3, A3) \
\
namespace Fog { \
  template <T1 A1, T2 A2, T3 A3> \
  FOG_INLINE void swap(\
    typename ::_Symbol_<A1, A2, A3>& a, \
    typename ::_Symbol_<A1, A2, A3>& b) \
  { \
    typename ::_Symbol_<A1, A2, A3>::_D* a_d = a._d; \
    typename ::_Symbol_<A1, A2, A3>::_D* b_d = b._d; \
    \
    a._d = b_d; \
    b._d = a_d; \
  } \
}

#define _FOG_SWAP_D_TEMPLATE4(_Symbol_, T1, A1, T2, A2, T3, A3, T4, A4) \
\
namespace Fog { \
  template <T1 A1, T2 A2, T3 A3, T4 A4> \
  FOG_INLINE void swap(\
    typename ::_Symbol_<A1, A2, A3>& a, \
    typename ::_Symbol_<A1, A2, A3, A4>& b) \
  { \
    typename ::_Symbol_<A1, A2, A3, A4>::_D* a_d = a._d; \
    typename ::_Symbol_<A1, A2, A3, A4>::_D* b_d = b._d; \
    \
    a._d = b_d; \
    b._d = a_d; \
  } \
}

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_GLOBAL_SWAP_H
