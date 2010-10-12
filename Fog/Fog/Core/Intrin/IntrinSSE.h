// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_INTRIN_INTRINSSE_H
#define _FOG_CORE_INTRIN_INTRINSSE_H

// [Dependencies]
#include <Fog/Core/Build.h>
#include <xmmintrin.h>

// ============================================================================
// [Fog::Core - SSE - Macros]
// ============================================================================

//! @addtogroup Fog_Core_Intrin
//! @{

#define FOG_SSE_DECLARE_CONST_PS_VAR(name, val0, val1, val2, val3) \
  FOG_ALIGNED_VAR(static const float, _sse_const_##name[4], 16) = \
  { \
    (float)(val3), \
    (float)(val2), \
    (float)(val1), \
    (float)(val0)  \
  }

#define FOG_SSE_DECLARE_CONST_PS_SET(name, val0) \
  FOG_ALIGNED_VAR(static const float, _sse_const_##name[4], 16) = \
  { \
    (float)(val0), \
    (float)(val0), \
    (float)(val0), \
    (float)(val0)  \
  }

#define FOG_SSE_DECLARE_CONST_PI8_VAR(name, val0, val1, val2, val3, val4, val5, val6, val7, val8, val9, val10, val11, val12, val13, val14, val15) \
  FOG_ALIGNED_VAR(static const uint8_t, _sse_const_##name[16], 16) = \
  { \
    (uint8_t)(val15), \
    (uint8_t)(val14), \
    (uint8_t)(val13), \
    (uint8_t)(val12), \
    (uint8_t)(val11), \
    (uint8_t)(val10), \
    (uint8_t)(val9), \
    (uint8_t)(val8), \
    (uint8_t)(val7), \
    (uint8_t)(val6), \
    (uint8_t)(val5), \
    (uint8_t)(val4), \
    (uint8_t)(val3), \
    (uint8_t)(val2), \
    (uint8_t)(val1), \
    (uint8_t)(val0)  \
  }

#define FOG_SSE_DECLARE_CONST_PI8_SET(name, val0) \
  FOG_ALIGNED_VAR(static const uint8_t, _sse_const_##name[16], 16) = \
  { \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0)  \
  }

#define FOG_SSE_DECLARE_CONST_PI16_VAR(name, val0, val1, val2, val3, val4, val5, val6, val7) \
  FOG_ALIGNED_VAR(static const uint16_t, _sse_const_##name[8], 16) = \
  { \
    (uint16_t)(val7), \
    (uint16_t)(val6), \
    (uint16_t)(val5), \
    (uint16_t)(val4), \
    (uint16_t)(val3), \
    (uint16_t)(val2), \
    (uint16_t)(val1), \
    (uint16_t)(val0)  \
  }

#define FOG_SSE_DECLARE_CONST_PI16_SET(name, val0) \
  FOG_ALIGNED_VAR(static const uint16_t, _sse_const_##name[8], 16) = \
  { \
    (uint16_t)(val0), \
    (uint16_t)(val0), \
    (uint16_t)(val0), \
    (uint16_t)(val0), \
    (uint16_t)(val0), \
    (uint16_t)(val0), \
    (uint16_t)(val0), \
    (uint16_t)(val0)  \
  }

#define FOG_SSE_DECLARE_CONST_PI32_VAR(name, val0, val1, val2, val3) \
  FOG_ALIGNED_VAR(static const uint32_t, _sse_const_##name[4], 16) = \
  { \
    (uint32_t)(val3), \
    (uint32_t)(val2), \
    (uint32_t)(val1), \
    (uint32_t)(val0)  \
  }

#define FOG_SSE_DECLARE_CONST_PI32_SET(name, val0) \
  FOG_ALIGNED_VAR(static const uint32_t, _sse_const_##name[4], 16) = \
  { \
    (uint32_t)(val0), \
    (uint32_t)(val0), \
    (uint32_t)(val0), \
    (uint32_t)(val0)  \
  }

#define FOG_SSE_DECLARE_CONST_PI64_VAR(name, val0, val1) \
  FOG_ALIGNED_VAR(static const uint64_t, _sse_const_##name[2], 16) = \
  { \
    (uint64_t)(val1), \
    (uint64_t)(val0)  \
  }

#define FOG_SSE_DECLARE_CONST_PI64_SET(name, val0) \
  FOG_ALIGNED_VAR(static const uint64_t, _sse_const_##name[2], 16) = \
  { \
    (uint64_t)(val0), \
    (uint64_t)(val0)  \
  }

#define FOG_SSE_DECLARE_CONST_PD_VAR(name, val0, val1) \
  FOG_ALIGNED_VAR(static const double, _sse_const_##name[2], 16) = \
  { \
    val1, \
    val0  \
  }

#define FOG_SSE_DECLARE_CONST_PD_SET(name, val0) \
  FOG_ALIGNED_VAR(static const double, _sse_const_##name[2], 16) = \
  { \
    val0, \
    val0  \
  }

#define FOG_SSE_GET_CONST_SS(name) (*(const  float  *)_sse_const_##name)
#define FOG_SSE_GET_CONST_PS(name) (*(const __m128  *)_sse_const_##name)

#define FOG_SSE_GET_CONST_SD(name) (*(const double  *)_sse_const_##name)
#define FOG_SSE_GET_CONST_PD(name) (*(const __m128d *)_sse_const_##name)

#define FOG_SSE_GET_CONST_SI(name) (*(const int     *)_sse_const_##name)
#define FOG_SSE_GET_CONST_PI(name) (*(const __m128i *)_sse_const_##name)

//! @}

// [Guard]
#endif // _FOG_CORE_INTRIN_INTRINSSE_H
