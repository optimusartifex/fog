// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_ACC_ACCSSE2_H
#define _FOG_G2D_ACC_ACCSSE2_H

// [Dependencies]
#include <Fog/Core/Acc/AccSse2.h>

namespace Fog {
namespace Acc {

//! @addtogroup Fog_Core_Acc_Sse2
//! @{

// ============================================================================
// [Fog::Acc::SSE2 - Raster - Premultiply]
// ============================================================================

static FOG_INLINE void m128iPRGB32FromARGB32Lo_PBB(__m128i& dst0, const __m128i& x0)
{
  __m128i alpha0;

  Acc::m128iUnpackPI16FromPI8Lo(dst0, x0);
  Acc::m128iShufflePI16Lo<3, 3, 3, 3>(alpha0, dst0);
  Acc::m128iOr(dst0, dst0, FOG_XMM_GET_CONST_PI(00FF000000000000_00FF000000000000));
  Acc::m128iMulDiv255PI16(dst0, dst0, alpha0);
  Acc::m128iPackPU8FromPU16(dst0, dst0);
}

static FOG_INLINE void m128iPRGB32FromARGB32_PBW(__m128i& dst0, const __m128i& x0)
{
  __m128i alpha0;

  Acc::m128iShufflePI16<3, 3, 3, 3>(alpha0, x0);
  Acc::m128iOr(dst0, x0, FOG_XMM_GET_CONST_PI(00FF000000000000_00FF000000000000));
  Acc::m128iMulDiv255PI16(dst0, dst0, alpha0);
}

static FOG_INLINE void m128iPRGB32FromARGB32Lo_PBW(__m128i& dst0, const __m128i& x0)
{
  __m128i alpha0;

  Acc::m128iShufflePI16Lo<3, 3, 3, 3>(alpha0, x0);
  Acc::m128iOr(dst0, x0, FOG_XMM_GET_CONST_PI(00FF000000000000_00FF000000000000));
  Acc::m128iMulDiv255PI16(dst0, dst0, alpha0);
}

// ============================================================================
// [Fog::Acc::SSE2 - Raster - UnpackMask]
// ============================================================================

static FOG_INLINE void m128iUnpackMask2PI16(__m128i& dst0, const __m128i& x0)
{
  m128iUnpackPI32FromPI16Lo(dst0, x0, x0);
  m128iShufflePI32<1, 1, 0, 0>(dst0, dst0);
}

static FOG_INLINE void m128iUnpackMask4PI16(__m128i& dst0, __m128i& dst1, const __m128i& x0)
{
  m128iUnpackPI32FromPI16Lo(dst0, x0, x0);
  m128iShufflePI32<3, 3, 2, 2>(dst1, dst0);
  m128iShufflePI32<1, 1, 0, 0>(dst0, dst0);
}

//! @}

} // Acc namespace
} // Fog namespace

// [Guard]
#endif // _FOG_G2D_ACC_ACCSSE2_H
