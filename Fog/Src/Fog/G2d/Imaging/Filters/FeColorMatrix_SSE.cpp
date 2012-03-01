// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Acc/AccSse.h>
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Tools/Cpu.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Imaging/Filters/FeColorMatrix.h>
#include <Fog/G2d/Source/Color.h>

namespace Fog {

// ============================================================================
// [Fog::FeColorMatrix - Add]
// ============================================================================

static err_t FOG_CDECL FeColorMatrix_addMatrix_SSE(FeColorMatrix* dst, const FeColorMatrix* a, const FeColorMatrix* b)
{
  float *dm = dst->m;
  const float *am = a->m;
  const float *bm = b->m;

  __m128f xmm0a, xmm1a, xmm2a, xmm3a;
  __m128f xmm0b, xmm1b, xmm2b, xmm3b;

  Acc::m128fLoad16uLoHi(xmm0a, am +  0);
  Acc::m128fLoad16uLoHi(xmm1a, am +  4);
  Acc::m128fLoad16uLoHi(xmm2a, am +  8);
  Acc::m128fLoad16uLoHi(xmm3a, am + 12);

  Acc::m128fLoad16uLoHi(xmm0b, bm +  0);
  Acc::m128fLoad16uLoHi(xmm1b, bm +  4);
  Acc::m128fLoad16uLoHi(xmm2b, bm +  8);
  Acc::m128fLoad16uLoHi(xmm3b, bm + 12);

  Acc::m128fAddPS(xmm0a, xmm0a, xmm0b);
  Acc::m128fAddPS(xmm1a, xmm1a, xmm1b);

  Acc::m128fStore16uLoHi(dm +  0, xmm0a);
  Acc::m128fStore16uLoHi(dm +  4, xmm1a);

  Acc::m128fLoad16uLoHi(xmm0a, am + 16);
  Acc::m128fLoad16uLoHi(xmm1a, am + 20);

  Acc::m128fAddPS(xmm2a, xmm2a, xmm2b);
  Acc::m128fAddPS(xmm3a, xmm3a, xmm3b);

  Acc::m128fLoad16uLoHi(xmm0b, bm + 16);
  Acc::m128fLoad16uLoHi(xmm1b, bm + 20);

  Acc::m128fStore16uLoHi(dm +  8, xmm2a);
  Acc::m128fStore16uLoHi(dm + 12, xmm3a);

  Acc::m128fLoad4(xmm2a, am + 24);
  Acc::m128fLoad4(xmm2b, bm + 24);

  Acc::m128fAddPS(xmm0a, xmm0a, xmm0b);
  Acc::m128fAddPS(xmm1a, xmm1a, xmm1b);

  Acc::m128fStore16uLoHi(dm + 16, xmm0a);

  Acc::m128fAddSS(xmm2a, xmm2a, xmm2b);
  Acc::m128fStore16uLoHi(dm + 20, xmm1a);
  Acc::m128fStore4(dm + 24, xmm2a);

  return ERR_OK;
}

// ============================================================================
// [Fog::FeColorMatrix - Subtract]
// ============================================================================

static err_t FOG_CDECL FeColorMatrix_subtractMatrix_SSE(FeColorMatrix* dst, const FeColorMatrix* a, const FeColorMatrix* b)
{
  float *dm = dst->m;
  const float *am = a->m;
  const float *bm = b->m;

  __m128f xmm0a, xmm1a, xmm2a, xmm3a;
  __m128f xmm0b, xmm1b, xmm2b, xmm3b;

  Acc::m128fLoad16uLoHi(xmm0a, am +  0);
  Acc::m128fLoad16uLoHi(xmm1a, am +  4);
  Acc::m128fLoad16uLoHi(xmm2a, am +  8);
  Acc::m128fLoad16uLoHi(xmm3a, am + 12);

  Acc::m128fLoad16uLoHi(xmm0b, bm +  0);
  Acc::m128fLoad16uLoHi(xmm1b, bm +  4);
  Acc::m128fLoad16uLoHi(xmm2b, bm +  8);
  Acc::m128fLoad16uLoHi(xmm3b, bm + 12);

  Acc::m128fSubPS(xmm0a, xmm0a, xmm0b);
  Acc::m128fSubPS(xmm1a, xmm1a, xmm1b);

  Acc::m128fStore16uLoHi(dm +  0, xmm0a);
  Acc::m128fStore16uLoHi(dm +  4, xmm1a);

  Acc::m128fLoad16uLoHi(xmm0a, am + 16);
  Acc::m128fLoad16uLoHi(xmm1a, am + 20);

  Acc::m128fSubPS(xmm2a, xmm2a, xmm2b);
  Acc::m128fSubPS(xmm3a, xmm3a, xmm3b);

  Acc::m128fLoad16uLoHi(xmm0b, bm + 16);
  Acc::m128fLoad16uLoHi(xmm1b, bm + 20);

  Acc::m128fStore16uLoHi(dm +  8, xmm2a);
  Acc::m128fStore16uLoHi(dm + 12, xmm3a);

  Acc::m128fLoad4(xmm2a, am + 24);
  Acc::m128fLoad4(xmm2b, bm + 24);

  Acc::m128fSubPS(xmm0a, xmm0a, xmm0b);
  Acc::m128fSubPS(xmm1a, xmm1a, xmm1b);

  Acc::m128fStore16uLoHi(dm + 16, xmm0a);

  Acc::m128fSubSS(xmm2a, xmm2a, xmm2b);
  Acc::m128fStore16uLoHi(dm + 20, xmm1a);
  Acc::m128fStore4(dm + 24, xmm2a);

  return ERR_OK;
}

// ============================================================================
// [Fog::FeColorMatrix - Map]
// ============================================================================

static void FOG_CDECL FeColorMatrix_mapArgbF_SSE(const FeColorMatrix* self, ArgbF* dst, const ArgbF* src)
{
  const float* m = self->m;

  __m128f xmmR, xmmG, xmmB, xmmA;
  __m128f xmm0, xmm1, xmm2, xmm3;

  Acc::m128fLoad16uLoHi(xmm0, m +  0);
  Acc::m128fLoad16uLoHi(xmm1, m +  5);
  Acc::m128fLoad16uLoHi(xmm2, m + 10);
  Acc::m128fLoad16uLoHi(xmm3, m + 15);

  Acc::m128fLoad4(xmmA, &src->a);
  Acc::m128fLoad4(xmmR, &src->r);
  Acc::m128fLoad4(xmmG, &src->g);
  Acc::m128fLoad4(xmmB, &src->b);

  Acc::m128fShuffle<0, 0, 0, 0>(xmmR, xmmR, xmmR);
  Acc::m128fShuffle<0, 0, 0, 0>(xmmG, xmmG, xmmG);

  Acc::m128fMulPS(xmm0, xmm0, xmmR);
  Acc::m128fMulPS(xmm1, xmm1, xmmG);

  Acc::m128fShuffle<0, 0, 0, 0>(xmmB, xmmB, xmmB);
  Acc::m128fShuffle<0, 0, 0, 0>(xmmA, xmmA, xmmA);

  Acc::m128fMulPS(xmm2, xmm2, xmmB);
  Acc::m128fMulPS(xmm3, xmm3, xmmA);

  Acc::m128fAddPS(xmm0, xmm0, xmm1);
  Acc::m128fLoad16uLoHi(xmm1, m + 20);

  Acc::m128fAddPS(xmm2, xmm2, xmm3);
  Acc::m128fAddPS(xmm0, xmm0, xmm1);

  Acc::m128fAddPS(xmm0, xmm0, xmm2);

  // Shuffle to ARGB (We have RGBA at the moment).
  Acc::m128fShuffle<0, 3, 2, 1>(xmm0, xmm0, xmm0);

  Acc::m128fStore16uLoHi(dst, xmm0);
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void FeColorMatrix_init_SSE(void)
{
  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  fog_api.fecolormatrix_addMatrix = FeColorMatrix_addMatrix_SSE;
  fog_api.fecolormatrix_subtractMatrix = FeColorMatrix_subtractMatrix_SSE;
  fog_api.fecolormatrix_mapArgbF = FeColorMatrix_mapArgbF_SSE;
}

} // Fog namespace
