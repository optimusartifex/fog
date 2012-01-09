// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Face/FaceSSE2.h>
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/MemBuffer.h>
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/OS/OSUtil.h>
#include <Fog/Core/Tools/Cpu.h>
#include <Fog/G2d/Painting/Rasterizer_p.h>
#include <Fog/G2d/Tools/Region.h>
#include <Fog/G2d/Tools/RegionTmp_p.h>
#include <Fog/G2d/Tools/RegionUtil_p.h>

namespace Fog {

// ============================================================================
// [Fog::Region - Translate (SSE2)]
// ============================================================================

static err_t FOG_CDECL Region_translate_SSE2(Region* dst, const Region* src, const PointI* pt)
{
  RegionData* d = dst->_d;
  RegionData* src_d = src->_d;

  int tx = pt->x;
  int ty = pt->y;

  if (src_d == fog_api.region_oInfinite->_d || (tx | ty) == 0)
  {
    return dst->setRegion(*src);
  }

  size_t length = src_d->length;
  if (length == 0)
  {
    dst->clear();
    return ERR_OK;
  }

  // If the translation cause arithmetic overflow or underflow then we first
  // clip the input region into safe boundary which might be translated.
  bool saturate = ((tx < 0) & (src_d->boundingBox.x0 < INT_MIN - tx)) |
                  ((ty < 0) & (src_d->boundingBox.y0 < INT_MIN - ty)) |
                  ((tx > 0) & (src_d->boundingBox.x1 > INT_MAX - tx)) |
                  ((ty > 0) & (src_d->boundingBox.y1 > INT_MAX - ty)) ;

  if (saturate)
  {
    BoxI clipBox(INT_MIN, INT_MIN, INT_MAX, INT_MAX);
    return fog_api.region_translateAndClip(dst, src, pt, &clipBox);
  }

  if (d->reference.get() != 1 || d->capacity < length)
  {
    d = fog_api.region_dCreate(length);
    if (FOG_IS_NULL(d))
      return ERR_RT_OUT_OF_MEMORY;
  }

  BoxI* dCur = d->data;
  BoxI* sCur = src_d->data;

  __m128i xmm0;
  __m128i xmmt;

  Face::m128iLoad8(xmmt, pt);
  Face::m128iLoad16uLoHi(xmm0, &src_d->boundingBox);
  Face::m128iShufflePI32<1, 0, 1, 0>(xmmt, xmmt);

  d->length = length;

  Face::m128iAddPI32(xmm0, xmm0, xmmt);
  Face::m128iStore16uLoHi(&d->boundingBox, xmm0);

  size_t i;

  if ((((size_t)dCur | (size_t)sCur) & 0xF) == 0)
  {
    for (i = length >> 2; i; i--, dCur += 4, sCur += 4)
    {
      __m128i xmm1, xmm2, xmm3;

      Face::m128iLoad16a(xmm0, &sCur[0]);
      Face::m128iLoad16a(xmm1, &sCur[1]);
      Face::m128iLoad16a(xmm2, &sCur[2]);
      Face::m128iLoad16a(xmm3, &sCur[3]);

      Face::m128iAddPI32(xmm0, xmm0, xmmt);
      Face::m128iAddPI32(xmm1, xmm1, xmmt);
      Face::m128iAddPI32(xmm2, xmm2, xmmt);
      Face::m128iAddPI32(xmm3, xmm3, xmmt);

      Face::m128iStore16a(&dCur[0], xmm0);
      Face::m128iStore16a(&dCur[1], xmm1);
      Face::m128iStore16a(&dCur[2], xmm2);
      Face::m128iStore16a(&dCur[3], xmm3);
    }

    for (i = length & 3; i; i--, dCur++, sCur++)
    {
      Face::m128iLoad16a(xmm0, &sCur[0]);
      Face::m128iAddPI32(xmm0, xmm0, xmmt);
      Face::m128iStore16a(&dCur[0], xmm0);
    }
  }
  else
  {
    for (i = length >> 2; i; i--, dCur += 4, sCur += 4)
    {
      __m128i xmm1, xmm2, xmm3;

      Face::m128iLoad16uLoHi(xmm0, &sCur[0]);
      Face::m128iLoad16uLoHi(xmm1, &sCur[1]);
      Face::m128iLoad16uLoHi(xmm2, &sCur[2]);
      Face::m128iLoad16uLoHi(xmm3, &sCur[3]);

      Face::m128iAddPI32(xmm0, xmm0, xmmt);
      Face::m128iAddPI32(xmm1, xmm1, xmmt);
      Face::m128iAddPI32(xmm2, xmm2, xmmt);
      Face::m128iAddPI32(xmm3, xmm3, xmmt);

      Face::m128iStore16uLoHi(&dCur[0], xmm0);
      Face::m128iStore16uLoHi(&dCur[1], xmm1);
      Face::m128iStore16uLoHi(&dCur[2], xmm2);
      Face::m128iStore16uLoHi(&dCur[3], xmm3);
    }

    for (i = length & 3; i; i--, dCur++, sCur++)
    {
      Face::m128iLoad16uLoHi(xmm0, &sCur[0]);
      Face::m128iAddPI32(xmm0, xmm0, xmmt);
      Face::m128iStore16uLoHi(&dCur[0], xmm0);
    }
  }

  if (d != dst->_d)
    atomicPtrXchg(&dst->_d, d)->release();
  return ERR_OK;
}

// ============================================================================
// [Fog::Region - Equality (SSE2)]
// ============================================================================

static bool FOG_CDECL Region_eq_SSE2(const Region* a, const Region* b)
{
  const RegionData* a_d = a->_d;
  const RegionData* b_d = b->_d;
  size_t length = a_d->length;

  if (a_d == b_d)
    return true;

  if (length != b_d->length)
    return false;

  const BoxI* a_data = a_d->data;
  const BoxI* b_data = b_d->data;

  size_t i;
  int mask;

  __m128i xmm_am;
  __m128i xmm_b0;

  Face::m128iLoad16uLoHi(xmm_am, &a_d->boundingBox);
  Face::m128iLoad16uLoHi(xmm_b0, &b_d->boundingBox);

  Face::m128iCmpEqPI32(xmm_am, xmm_am, xmm_b0);
  Face::m128iMoveMaskPI8(mask, xmm_am);
  if (mask != 0xFFFF)
    return false;

  // If both data pointers are aligned then we are safe to use MOVDQA
  // instruction. Otherwise slower variant of using MOVQ is used.
  if ((((size_t)a_data | (size_t)b_data) & 0xF) == 0)
  {
    for (i = length >> 2; i; i--, a_data += 4, b_data += 4)
    {
      __m128i xmm_a1, xmm_a2, xmm_a3;
      __m128i xmm_b1, xmm_b2, xmm_b3;

      Face::m128iLoad16a(xmm_am, &a_data[0]);
      Face::m128iLoad16a(xmm_a1, &a_data[1]);
      Face::m128iLoad16a(xmm_b0, &b_data[0]);
      Face::m128iLoad16a(xmm_b1, &b_data[1]);

      Face::m128iCmpEqPI32(xmm_am, xmm_am, xmm_b0);
      Face::m128iCmpEqPI32(xmm_a1, xmm_a1, xmm_b1);

      Face::m128iLoad16a(xmm_a2, &a_data[2]);
      Face::m128iLoad16a(xmm_a3, &a_data[3]);

      Face::m128iAnd(xmm_am, xmm_am, xmm_a1);

      Face::m128iLoad16a(xmm_b2, &b_data[2]);
      Face::m128iLoad16a(xmm_b3, &b_data[3]);

      Face::m128iCmpEqPI32(xmm_a2, xmm_a2, xmm_b2);
      Face::m128iCmpEqPI32(xmm_a3, xmm_a3, xmm_b3);

      Face::m128iAnd(xmm_am, xmm_am, xmm_a2);
      Face::m128iAnd(xmm_am, xmm_am, xmm_a3);

      Face::m128iMoveMaskPI8(mask, xmm_am);
      if (mask != 0xFFFF)
        return false;
    }

    for (i = length & 3; i; i--, a_data++, b_data++)
    {
      __m128i xmm_a0;

      Face::m128iLoad16a(xmm_a0, &a_data[0]);
      Face::m128iLoad16a(xmm_b0, &b_data[0]);

      Face::m128iCmpEqPI32(xmm_a0, xmm_a0, xmm_b0);
      Face::m128iAnd(xmm_am, xmm_am, xmm_a0);
    }
  }
  else
  {
    for (i = length >> 2; i; i--, a_data += 4, b_data += 4)
    {
      __m128i xmm_a1, xmm_a2, xmm_a3;
      __m128i xmm_b1, xmm_b2, xmm_b3;

      Face::m128iLoad16uLoHi(xmm_am, &a_data[0]);
      Face::m128iLoad16uLoHi(xmm_a1, &a_data[1]);
      Face::m128iLoad16uLoHi(xmm_b0, &b_data[0]);
      Face::m128iLoad16uLoHi(xmm_b1, &b_data[1]);

      Face::m128iCmpEqPI32(xmm_am, xmm_am, xmm_b0);
      Face::m128iCmpEqPI32(xmm_a1, xmm_a1, xmm_b1);

      Face::m128iLoad16uLoHi(xmm_a2, &a_data[2]);
      Face::m128iLoad16uLoHi(xmm_a3, &a_data[3]);

      Face::m128iAnd(xmm_am, xmm_am, xmm_a1);

      Face::m128iLoad16uLoHi(xmm_b2, &b_data[2]);
      Face::m128iLoad16uLoHi(xmm_b3, &b_data[3]);

      Face::m128iCmpEqPI32(xmm_a2, xmm_a2, xmm_b2);
      Face::m128iCmpEqPI32(xmm_a3, xmm_a3, xmm_b3);

      Face::m128iAnd(xmm_am, xmm_am, xmm_a2);
      Face::m128iAnd(xmm_am, xmm_am, xmm_a3);

      Face::m128iMoveMaskPI8(mask, xmm_am);
      if (mask != 0xFFFF)
        return false;
    }

    for (i = length & 3; i; i--, a_data++, b_data++)
    {
      __m128i xmm_a0;

      Face::m128iLoad16uLoHi(xmm_a0, &a_data[0]);
      Face::m128iLoad16uLoHi(xmm_b0, &b_data[0]);

      Face::m128iCmpEqPI32(xmm_a0, xmm_a0, xmm_b0);
      Face::m128iAnd(xmm_am, xmm_am, xmm_a0);
    }
  }

  Face::m128iMoveMaskPI8(mask, xmm_am);
  return mask == 0xFFFF;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Region_init_SSE2(void)
{
  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  fog_api.region_translate = Region_translate_SSE2;
  fog_api.region_eq = Region_eq_SSE2;
}

} // Fog namespace
