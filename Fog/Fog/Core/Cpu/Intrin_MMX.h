// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_CPU_INTRIN_MMX_H
#define _FOG_CORE_CPU_INTRIN_MMX_H

// [Dependencies]
#include <Fog/Core/Config/Config.h>
#include <mmintrin.h>

// ============================================================================
// [Fog::Core - MMX - Classes]
// ============================================================================

namespace Fog {

//! @addtogroup Fog_Core_Intrin
//! @{

//! @brief MMX data type that can be used to store / load data to / from MMX
//! register.
union FOG_ALIGNED_TYPE(mmx_t, 8)
{
  __m64     m64;
  int64_t   sq[1];
  uint64_t  uq[1];
  int32_t   sd[2];
  uint32_t  ud[2];
  int16_t   sw[4];
  uint16_t  uw[4];
  int8_t    sb[8];
  uint8_t   ub[8];
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::Core - MMX - Macros]
// ============================================================================

//! @addtogroup Fog_Core_Intrin
//! @{

// #define MMX_SHUFFLE(z, y, x, w) (((z) << 6) | ((y) << 4) | ((x) << 2) | (w))

// Fix for stupid msvc debugger feature that will assert you if you want to fill
// xmm register by ones using _mm_cmpeq_pi8() on currently uninitialized variable.
//
// In release mode it's ok so we add setzero() call only in debug builds.
#if defined(_MSC_VER) && (defined(DEBUG) || defined(_DEBUG) || defined(FOG_DEBUG))
# define _mm_ext_fill_si64(__var) \
  __var = _mm_setzero_si64(); \
  __var = _mm_cmpeq_pi8(__var, __var)
#else
# define _mm_ext_fill_si64(__var) \
  __var = _mm_cmpeq_pi8(__var, __var)
#endif

#if defined(__MINGW32__)
#endif // __MINGW32__

#define FOG_MMX_DECLARE_CONST_PI8_VAR(name, val0, val1, val2, val3, val4, val5, val6, val7) \
  FOG_ALIGNED_VAR(static const uint8_t, _sse_const_##name[8], 8) = \
  { \
    (uint8_t)(val7), \
    (uint8_t)(val6), \
    (uint8_t)(val5), \
    (uint8_t)(val4), \
    (uint8_t)(val3), \
    (uint8_t)(val2), \
    (uint8_t)(val1), \
    (uint8_t)(val0)  \
  }

#define FOG_MMX_DECLARE_CONST_PI8_SET(name, val0) \
  FOG_ALIGNED_VAR(static const uint8_t, _mmx_const_##name[8], 8) = \
  { \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0)  \
  }

#define FOG_MMX_DECLARE_CONST_PI16_VAR(name, val0, val1, val2, val3) \
  FOG_ALIGNED_VAR(static const uint16_t, _mmx_const_##name[4], 8) = \
  { \
    (uint16_t)(val3), \
    (uint16_t)(val2), \
    (uint16_t)(val1), \
    (uint16_t)(val0)  \
  }

#define FOG_MMX_DECLARE_CONST_PI16_SET(name, val0) \
  FOG_ALIGNED_VAR(static const uint16_t, _mmx_const_##name[4], 8) = \
  { \
    (uint16_t)(val0), \
    (uint16_t)(val0), \
    (uint16_t)(val0), \
    (uint16_t)(val0)  \
  }

#define FOG_MMX_DECLARE_CONST_PI32_VAR(name, val0, val1) \
  FOG_ALIGNED_VAR(static const uint32_t, _mmx_const_##name[2], 8) = \
  { \
    (uint32_t)(val1), \
    (uint32_t)(val0)  \
  }

#define FOG_MMX_DECLARE_CONST_PI32_SET(name, val0) \
  FOG_ALIGNED_VAR(static const uint32_t, _mmx_const_##name[2], 8) = \
  { \
    (uint32_t)(val0), \
    (uint32_t)(val0)  \
  }

#define FOG_MMX_DECLARE_CONST_PI64(name, val0, val1) \
  FOG_ALIGNED_VAR(static const uint64_t, _mmx_const_##name[1], 8) = \
  { \
    (uint64_t)(val0)  \
  }

#define FOG_MMX_DECLARE_CONST_PF_SET(name, val0) \
  FOG_ALIGNED_VAR(static const float, _mmx_const_##name[2], 8) = \
  { \
    (float)(val0), \
    (float)(val0)  \
  }

#define FOG_MMX_DECLARE_CONST_PF_VAR(name, val0, val1) \
  FOG_ALIGNED_VAR(static const float, _mmx_const_##name[2], 8) = \
  { \
    (float)(val1), \
    (float)(val0)  \
  }

#define FOG_MMX_GET_CONST(name) (*(const __m64*)_mmx_const_##name)

//! @}

// [Guard]
#endif // _FOG_CORE_CPU_INTRIN_MMX_H
