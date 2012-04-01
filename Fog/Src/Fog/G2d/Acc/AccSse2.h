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

//! @addtogroup Fog_G2d_Acc_Sse2
//! @{

// ============================================================================
// [Fog::Acc::SSE2 - Raster - Premultiply]
// ============================================================================

static FOG_INLINE void m128iPRGB32FromARGB32Lo_PBB(__m128i& dst0, const __m128i& x0)
{
  __m128i alpha0;

  m128iUnpackPI16FromPI8Lo(dst0, x0);
  m128iShufflePI16Lo<3, 3, 3, 3>(alpha0, dst0);
  m128iOr(dst0, dst0, FOG_XMM_GET_CONST_PI(00FF000000000000_00FF000000000000));
  m128iMulDiv255PI16(dst0, dst0, alpha0);
  m128iPackPU8FromPU16(dst0, dst0);
}

static FOG_INLINE void m128iPRGB32FromARGB32_PBW(__m128i& dst0, const __m128i& x0)
{
  __m128i alpha0;

  m128iShufflePI16<3, 3, 3, 3>(alpha0, x0);
  m128iOr(dst0, x0, FOG_XMM_GET_CONST_PI(00FF000000000000_00FF000000000000));
  m128iMulDiv255PI16(dst0, dst0, alpha0);
}

static FOG_INLINE void m128iPRGB32FromARGB32Lo_PBW(__m128i& dst0, const __m128i& x0)
{
  __m128i alpha0;

  m128iShufflePI16Lo<3, 3, 3, 3>(alpha0, x0);
  m128iOr(dst0, x0, FOG_XMM_GET_CONST_PI(00FF000000000000_00FF000000000000));
  m128iMulDiv255PI16(dst0, dst0, alpha0);
}

// ============================================================================
// [Fog::Acc::SSE2 - Raster - UnpackMask]
// ============================================================================

static FOG_INLINE void m128iUnpackMask2PI16(__m128i& dst0, const __m128i& x0)
{
#if defined(FOG_HARDCODE_SSSE3)
  FOG_XMM_DECLARE_CONST_PI8_VAR(Tmp,
    0x03, 0x02, 0x03, 0x02,
    0x03, 0x02, 0x03, 0x02,
    0x01, 0x00, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x00);
  dst0 = _mm_shuffle_epi8(x0, FOG_XMM_GET_CONST_PI(Tmp));
#else
  m128iUnpackPI32FromPI16Lo(dst0, x0, x0);
  m128iShufflePI32<1, 1, 0, 0>(dst0, dst0);
#endif // FOG_HARDCODE_SSSE3
}

static FOG_INLINE void m128iUnpackMask4PI16(__m128i& dst0, __m128i& dst1, const __m128i& x0)
{
  m128iUnpackPI32FromPI16Lo(dst0, x0, x0);
  m128iShufflePI32<3, 3, 2, 2>(dst1, dst0);
  m128iShufflePI32<1, 1, 0, 0>(dst0, dst0);
}

// ============================================================================
// [Fog::Acc::SSE2 - Raster - UnpackMask]
// ============================================================================

static FOG_INLINE void m128iUnpackAlphaPI16FromARGB32_PI8(__m128i& dst0, __m128i& dst1, const __m128i& x0)
{
#if defined(FOG_HARDCODE_SSSE3)
  FOG_XMM_DECLARE_CONST_PI8_VAR(Tmp0,
    0x80, 0x07, 0x80, 0x07,
    0x80, 0x07, 0x80, 0x07,
    0x80, 0x03, 0x80, 0x03,
    0x80, 0x03, 0x80, 0x03);

  FOG_XMM_DECLARE_CONST_PI8_VAR(Tmp1,
    0x80, 0x0F, 0x80, 0x0F,
    0x80, 0x0F, 0x80, 0x0F,
    0x80, 0x03, 0x80, 0x03,
    0x80, 0x03, 0x80, 0x03);

  dst1 = x0;
  dst0 = _mm_shuffle_epi8(x0  , FOG_XMM_GET_CONST_PI(Tmp0));
  dst1 = _mm_shuffle_epi8(dst1, FOG_XMM_GET_CONST_PI(Tmp1));
#else
  m128iRShiftPU16<8>(dst0, x0);

  m128iUnpackPI32FromPI16Hi(dst1, dst0, dst0);
  m128iUnpackPI32FromPI16Lo(dst0, dst0, dst0);

  m128iShufflePI32<3, 3, 1, 1>(dst0, dst0);
  m128iShufflePI32<3, 3, 1, 1>(dst1, dst1);
#endif // FOG_HARDCODE_SSSE3
}

//! @}

} // Acc namespace
} // Fog namespace

// [Guard]
#endif // _FOG_G2D_ACC_ACCSSE2_H
