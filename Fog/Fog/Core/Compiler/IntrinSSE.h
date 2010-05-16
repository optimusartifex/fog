// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_INTRIN_SSE_H
#define _FOG_CORE_INTRIN_SSE_H

// [Dependencies]
#include <Fog/Core/Build.h>
#include <xmmintrin.h>

// ============================================================================
// [Fog::Core - SSE - Macros]
// ============================================================================

//! @addtogroup Fog_Core_Intrinsics
//! @{

#define FOG_DECLARE_SSE_CONST_PS_VAR(name, val0, val1, val2, val3) \
  FOG_ALIGNED_VAR(static const float, _sse2_const_##name[4], 16) = \
  { \
    (float)(val3), \
    (float)(val2), \
    (float)(val1), \
    (float)(val0)  \
  }

#define FOG_DECLARE_SSE_CONST_PS_SET(name, val0) \
  FOG_ALIGNED_VAR(static const float, _sse2_const_##name[4], 16) = \
  { \
    (float)(val0), \
    (float)(val0), \
    (float)(val0), \
    (float)(val0)  \
  }

#define FOG_GET_SSE_CONST_PS(name) (*(const __m128 *)_sse2_const_##name)

//! @}

// [Guard]
#endif // _FOG_CORE_INTRIN_SSE_H
