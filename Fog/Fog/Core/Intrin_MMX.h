// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_INTRIN_MMX_H
#define _FOG_CORE_INTRIN_MMX_H

// [Dependencies]
#include <Fog/Build/Build.h>
#include <mmintrin.h>

// #ifndef _MM_SHUFFLE
// #define _MM_SHUFFLE(z, y, x, w) (((z) << 6) | ((y) << 4) | ((x) << 2) | (w))
// #endif // _MM_SHUFFLE

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

//! @addtogroup Fog_Core
//! @{

namespace Fog {

//! @brief MMX data type that can be used to store / load data to / from
//! mm register.
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

}

//! @}

// [Guard]
#endif // _FOG_CORE_INTRIN_MMX_H
