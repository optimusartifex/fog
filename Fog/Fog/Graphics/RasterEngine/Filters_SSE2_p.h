// [Fog-Graphics]
//
// [License]
// MIT, See COPYING file in package

// For some IDEs to enable code-assist.
#include <Fog/Core/Build.h>

#if defined(FOG_IDE)
#include <Fog/Graphics/RasterEngine/Defs_SSE2_p.h>
#include <Fog/Graphics/RasterEngine/Dib_SSE2_p.h>
#endif // FOG_IDE

//----------------------------------------------------------------------------
// Linear (stack) blur and recursive blur algorithms were ported from AntiGrain.
//----------------------------------------------------------------------------
//
// Anti-Grain Geometry - Version 2.4
// Copyright (C) 2002-2005 Maxim Shemanarev (http://www.antigrain.com)
//
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//----------------------------------------------------------------------------
// Contact: mcseem@antigrain.com
//          mcseemagg@yahoo.com
//          http://www.antigrain.com
//----------------------------------------------------------------------------
//
// The Stack Blur Algorithm was invented by Mario Klingemann, 
// mario@quasimondo.com and described here:
// http://incubator.quasimondo.com/processing/fast_blur_deluxe.php
// (search phrase "Stackblur: Fast But Goodlooking"). 
// The major improvement is that there's no more division table
// that was very expensive to create for large blur radii. Insted, 
// for 8-bit per channel and radius not exceeding 254 the division is 
// replaced by multiplication and shift. 
//
//----------------------------------------------------------------------------

namespace Fog {
namespace RasterEngine {

// ============================================================================
// [Fog::RasterEngine::SSE2 - Filter]
// ============================================================================

//! @internal
struct FOG_HIDDEN FilterSSE2
{
  // --------------------------------------------------------------------------
  // [Helpers]
  // --------------------------------------------------------------------------

  // Get reciprocal for 16-bit value @a val.
  static FOG_INLINE int getReciprocal(int val)
  {
    return (65536 + val - 1) / val;
  }

  // --------------------------------------------------------------------------
  // [ColorMatrix]
  // --------------------------------------------------------------------------

  static FOG_INLINE __m128 _loadColorMatrixRow(const float* src)
  {
    __m128 t0 = _mm_loadu_ps(src);
    t0 = _mm_shuffle_ps(t0, t0, _MM_SHUFFLE(3, 0, 1, 2));
    return t0;
  }

  static void FOG_FASTCALL color_matrix_prgb32(
    const ColorMatrix* cm,
    uint8_t* dst, const uint8_t* src, int w)
  {
    __m128 cf0 = _mm_set1_ps(0.0f);
    __m128 cf255 = _mm_set1_ps(255.0f);
    __m128 cf1div255mul = _mm_set_ps(0.0f, 1.0f / 255.0f, 1.0f / 255.0f, 1.0f / 255.0f);
    __m128 cf1div255add = _mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f);

    __m128 pr = _loadColorMatrixRow(cm->m[0]);
    __m128 pg = _loadColorMatrixRow(cm->m[1]);
    __m128 pb = _loadColorMatrixRow(cm->m[2]);
    __m128 pa = _loadColorMatrixRow(cm->m[3]);
    __m128 pt = _mm_mul_ps(_loadColorMatrixRow(cm->m[4]), cf255);

    while ((w -= 2) >= 0)
    {
      __m128i pix0, pix1;
      __m128 pixf0, pixf1;
      __m128 pixt0, pixt1;
      __m128 result0, result1;

      sse2_load8(pix0, src);

      {
        __m128i xmmz = _mm_setzero_si128();
        pix0 = _mm_unpacklo_epi8(pix0, xmmz);
        pix1 = _mm_shuffle_epi32(pix0, _MM_SHUFFLE(1, 0, 3, 2));

        pix0 = _mm_unpacklo_epi16(pix0, xmmz);
        pix1 = _mm_unpacklo_epi16(pix1, xmmz);
      }

      pixf0 = _mm_cvtepi32_ps(pix0);
      pixf1 = _mm_cvtepi32_ps(pix1);

      {
        __m128 demul0 = _mm_load_ss(&raster_demultiply_reciprocal_table_f[src[ARGB32_ABYTE]]);
        __m128 demul1 = _mm_load_ss(&raster_demultiply_reciprocal_table_f[src[ARGB32_ABYTE+4]]);

        // demul |= [1.0f, 0x0, 0x0, 0x0]
        demul0 = _mm_castsi128_ps(_mm_or_si128(_mm_castps_si128(demul0), FOG_SSE_GET_CONST_PI(3F80000000000000_0000000000000000)));
        demul1 = _mm_castsi128_ps(_mm_or_si128(_mm_castps_si128(demul1), FOG_SSE_GET_CONST_PI(3F80000000000000_0000000000000000)));

        demul0 = _mm_shuffle_ps(demul0, demul0, _MM_SHUFFLE(3, 0, 0, 0));
        demul1 = _mm_shuffle_ps(demul1, demul1, _MM_SHUFFLE(3, 0, 0, 0));

        pixf0 = _mm_mul_ps(pixf0, demul0);
        pixf1 = _mm_mul_ps(pixf1, demul1);
      }

      result0 = pt;
      result1 = pt;

      pixt0 = _mm_shuffle_epi32_f(pixf0, _MM_SHUFFLE(2, 2, 2, 2));
      pixt1 = _mm_shuffle_epi32_f(pixf1, _MM_SHUFFLE(2, 2, 2, 2));
      pixt0 = _mm_mul_ps(pixt0, pr);
      pixt1 = _mm_mul_ps(pixt1, pr);
      result0 = _mm_add_ps(result0, pixt0);
      result1 = _mm_add_ps(result1, pixt1);

      pixt0 = _mm_shuffle_epi32_f(pixf0, _MM_SHUFFLE(1, 1, 1, 1));
      pixt1 = _mm_shuffle_epi32_f(pixf1, _MM_SHUFFLE(1, 1, 1, 1));
      pixt0 = _mm_mul_ps(pixt0, pg);
      pixt1 = _mm_mul_ps(pixt1, pg);
      result0 = _mm_add_ps(result0, pixt0);
      result1 = _mm_add_ps(result1, pixt1);

      pixt0 = _mm_shuffle_epi32_f(pixf0, _MM_SHUFFLE(0, 0, 0, 0));
      pixt1 = _mm_shuffle_epi32_f(pixf1, _MM_SHUFFLE(0, 0, 0, 0));
      pixt0 = _mm_mul_ps(pixt0, pb);
      pixt1 = _mm_mul_ps(pixt1, pb);
      result0 = _mm_add_ps(result0, pixt0);
      result1 = _mm_add_ps(result1, pixt1);

      pixt0 = _mm_shuffle_epi32_f(pixf0, _MM_SHUFFLE(3, 3, 3, 3));
      pixt1 = _mm_shuffle_epi32_f(pixf1, _MM_SHUFFLE(3, 3, 3, 3));
      pixt0 = _mm_mul_ps(pixt0, pa);
      pixt1 = _mm_mul_ps(pixt1, pa);
      result0 = _mm_add_ps(result0, pixt0);
      result1 = _mm_add_ps(result1, pixt1);

      result0 = _mm_max_ps(result0, cf0);
      result1 = _mm_max_ps(result1, cf0);
      result0 = _mm_min_ps(result0, cf255);
      result1 = _mm_min_ps(result1, cf255);

      pixf0 = _mm_shuffle_epi32_f(result0, _MM_SHUFFLE(3, 3, 3, 3));
      pixf1 = _mm_shuffle_epi32_f(result1, _MM_SHUFFLE(3, 3, 3, 3));

      pixf0 = _mm_mul_ps(pixf0, cf1div255mul);
      pixf1 = _mm_mul_ps(pixf1, cf1div255mul);
      pixf0 = _mm_add_ps(pixf0, cf1div255add);
      pixf1 = _mm_add_ps(pixf1, cf1div255add);

      result0 = _mm_mul_ps(result0, pixf0);
      result1 = _mm_mul_ps(result1, pixf1);

      pix0 = _mm_cvtps_epi32(result0);
      pix1 = _mm_cvtps_epi32(result1);
      pix0 = _mm_packs_epi32(pix0, pix0);
      pix1 = _mm_packs_epi32(pix1, pix1);
      pix0 = _mm_packus_epi16(pix0, pix1);
      pix0 = _mm_shuffle_epi32(pix0, _MM_SHUFFLE(2, 0, 2, 0));

      sse2_store8(dst, pix0);
      dst += 8;
      src += 8;
    }
    w += 2;

    if (w & 1)
    {
      __m128i pix;
      __m128 pixf;
      __m128 pixt;
      __m128 demul;
      __m128 result;

      sse2_load4(pix, src);
      sse2_unpack_to_float(pixf, pix);

      demul = _mm_load_ss(&raster_demultiply_reciprocal_table_f[src[ARGB32_ABYTE]]);
      // demul |= [1.0f, 0x0, 0x0, 0x0]
      demul = _mm_castsi128_ps(_mm_or_si128(_mm_castps_si128(demul), FOG_SSE_GET_CONST_PI(3F80000000000000_0000000000000000)));
      demul = _mm_shuffle_ps(demul, demul, _MM_SHUFFLE(3, 0, 0, 0));
      pixf = _mm_mul_ps(pixf, demul);

      result = pt;

      pixt = _mm_shuffle_epi32_f(pixf, _MM_SHUFFLE(2, 2, 2, 2));
      pixt = _mm_mul_ps(pixt, pr);
      result = _mm_add_ps(result, pixt);

      pixt = _mm_shuffle_epi32_f(pixf, _MM_SHUFFLE(1, 1, 1, 1));
      pixt = _mm_mul_ps(pixt, pg);
      result = _mm_add_ps(result, pixt);

      pixt = _mm_shuffle_epi32_f(pixf, _MM_SHUFFLE(0, 0, 0, 0));
      pixt = _mm_mul_ps(pixt, pb);
      result = _mm_add_ps(result, pixt);

      pixt = _mm_shuffle_epi32_f(pixf, _MM_SHUFFLE(3, 3, 3, 3));
      pixt = _mm_mul_ps(pixt, pa);
      result = _mm_add_ps(result, pixt);

      result = _mm_max_ps(result, cf0);
      result = _mm_min_ps(result, cf255);

      pixf = _mm_shuffle_epi32_f(result, _MM_SHUFFLE(3, 3, 3, 3));

      pixf = _mm_mul_ps(pixf, cf1div255mul);
      pixf = _mm_add_ps(pixf, cf1div255add);
      result = _mm_mul_ps(result, pixf);

      sse2_pack_from_float(pix, result);
      sse2_store4(dst, pix);
    }
  }

  static void FOG_FASTCALL color_matrix_argb32(
    const ColorMatrix* cm,
    uint8_t* dst, const uint8_t* src, int w)
  {
    __m128 cf0 = _mm_set1_ps(0.0f);
    __m128 cf255 = _mm_set1_ps(255.0f);

    __m128 pr = _loadColorMatrixRow(cm->m[0]);
    __m128 pg = _loadColorMatrixRow(cm->m[1]);
    __m128 pb = _loadColorMatrixRow(cm->m[2]);
    __m128 pa = _loadColorMatrixRow(cm->m[3]);
    __m128 pt = _mm_mul_ps(_loadColorMatrixRow(cm->m[4]), cf255);

    while ((w -= 2) >= 0)
    {
      __m128i pix0, pix1;
      __m128 pixf0, pixf1;
      __m128 pixt0, pixt1;
      __m128 result0, result1;

      sse2_load8(pix0, src);

      {
        __m128i xmmz = _mm_setzero_si128();
        pix0 = _mm_unpacklo_epi8(pix0, xmmz);
        pix1 = _mm_shuffle_epi32(pix0, _MM_SHUFFLE(1, 0, 3, 2));

        pix0 = _mm_unpacklo_epi16(pix0, xmmz);
        pix1 = _mm_unpacklo_epi16(pix1, xmmz);
      }

      pixf0 = _mm_cvtepi32_ps(pix0);
      pixf1 = _mm_cvtepi32_ps(pix1);

      result0 = pt;
      result1 = pt;

      pixt0 = _mm_shuffle_epi32_f(pixf0, _MM_SHUFFLE(2, 2, 2, 2));
      pixt1 = _mm_shuffle_epi32_f(pixf1, _MM_SHUFFLE(2, 2, 2, 2));
      pixt0 = _mm_mul_ps(pixt0, pr);
      pixt1 = _mm_mul_ps(pixt1, pr);
      result0 = _mm_add_ps(result0, pixt0);
      result1 = _mm_add_ps(result1, pixt1);

      pixt0 = _mm_shuffle_epi32_f(pixf0, _MM_SHUFFLE(1, 1, 1, 1));
      pixt1 = _mm_shuffle_epi32_f(pixf1, _MM_SHUFFLE(1, 1, 1, 1));
      pixt0 = _mm_mul_ps(pixt0, pg);
      pixt1 = _mm_mul_ps(pixt1, pg);
      result0 = _mm_add_ps(result0, pixt0);
      result1 = _mm_add_ps(result1, pixt1);

      pixt0 = _mm_shuffle_epi32_f(pixf0, _MM_SHUFFLE(0, 0, 0, 0));
      pixt1 = _mm_shuffle_epi32_f(pixf1, _MM_SHUFFLE(0, 0, 0, 0));
      pixt0 = _mm_mul_ps(pixt0, pb);
      pixt1 = _mm_mul_ps(pixt1, pb);
      result0 = _mm_add_ps(result0, pixt0);
      result1 = _mm_add_ps(result1, pixt1);

      pixt0 = _mm_shuffle_epi32_f(pixf0, _MM_SHUFFLE(3, 3, 3, 3));
      pixt1 = _mm_shuffle_epi32_f(pixf1, _MM_SHUFFLE(3, 3, 3, 3));
      pixt0 = _mm_mul_ps(pixt0, pa);
      pixt1 = _mm_mul_ps(pixt1, pa);
      result0 = _mm_add_ps(result0, pixt0);
      result1 = _mm_add_ps(result1, pixt1);

      result0 = _mm_max_ps(result0, cf0);
      result1 = _mm_max_ps(result1, cf0);
      result0 = _mm_min_ps(result0, cf255);
      result1 = _mm_min_ps(result1, cf255);

      pix0 = _mm_cvtps_epi32(result0);
      pix1 = _mm_cvtps_epi32(result1);
      pix0 = _mm_packs_epi32(pix0, pix0);
      pix1 = _mm_packs_epi32(pix1, pix1);
      pix0 = _mm_packus_epi16(pix0, pix1);
      pix0 = _mm_shuffle_epi32(pix0, _MM_SHUFFLE(2, 0, 2, 0));

      sse2_store8(dst, pix0);

      dst += 8;
      src += 8;
    }
    w += 2;

    if (w & 1)
    {
      __m128i pix;
      __m128 pixf;
      __m128 pixt;
      __m128 result;

      sse2_load4(pix, src);
      sse2_unpack_to_float(pixf, pix);

      result = pt;

      pixt = _mm_shuffle_epi32_f(pixf, _MM_SHUFFLE(2, 2, 2, 2));
      pixt = _mm_mul_ps(pixt, pr);
      result = _mm_add_ps(result, pixt);

      pixt = _mm_shuffle_epi32_f(pixf, _MM_SHUFFLE(1, 1, 1, 1));
      pixt = _mm_mul_ps(pixt, pg);
      result = _mm_add_ps(result, pixt);

      pixt = _mm_shuffle_epi32_f(pixf, _MM_SHUFFLE(0, 0, 0, 0));
      pixt = _mm_mul_ps(pixt, pb);
      result = _mm_add_ps(result, pixt);

      pixt = _mm_shuffle_epi32_f(pixf, _MM_SHUFFLE(3, 3, 3, 3));
      pixt = _mm_mul_ps(pixt, pa);
      result = _mm_add_ps(result, pixt);

      result = _mm_max_ps(result, cf0);
      result = _mm_min_ps(result, cf255);

      sse2_pack_from_float(pix, result);
      sse2_store4(dst, pix);
    }
  }

  static void FOG_FASTCALL color_matrix_xrgb32(
    const ColorMatrix* cm,
    uint8_t* dst, const uint8_t* src, int w)
  {
    __m128 cf0 = _mm_set1_ps(0.0f);
    __m128 cf255 = _mm_set1_ps(255.0f);

    __m128 pr = _loadColorMatrixRow(cm->m[0]);
    __m128 pg = _loadColorMatrixRow(cm->m[1]);
    __m128 pb = _loadColorMatrixRow(cm->m[2]);
    __m128 pa = _loadColorMatrixRow(cm->m[3]);
    __m128 pt = _mm_mul_ps(_loadColorMatrixRow(cm->m[4]), cf255);

    pt = _mm_add_ps(pt, _mm_mul_ps(pa, cf255));

    while ((w -=2) >= 0)
    {
      __m128i pix0, pix1;
      __m128 pixf0, pixf1;
      __m128 pixt0, pixt1;
      __m128 result0, result1;

      sse2_load8(pix0, src);
      pix0 = _mm_or_si128(pix0, FOG_SSE_GET_CONST_PI(FF000000FF000000_FF000000FF000000));

      {
        __m128i xmmz = _mm_setzero_si128();
        pix0 = _mm_unpacklo_epi8(pix0, xmmz);
        pix1 = _mm_shuffle_epi32(pix0, _MM_SHUFFLE(1, 0, 3, 2));

        pix0 = _mm_unpacklo_epi16(pix0, xmmz);
        pix1 = _mm_unpacklo_epi16(pix1, xmmz);
      }

      pixf0 = _mm_cvtepi32_ps(pix0);
      pixf1 = _mm_cvtepi32_ps(pix1);

      result0 = pt;
      result1 = pt;

      pixt0 = _mm_shuffle_epi32_f(pixf0, _MM_SHUFFLE(2, 2, 2, 2));
      pixt1 = _mm_shuffle_epi32_f(pixf1, _MM_SHUFFLE(2, 2, 2, 2));
      pixt0 = _mm_mul_ps(pixt0, pr);
      pixt1 = _mm_mul_ps(pixt1, pr);
      result0 = _mm_add_ps(result0, pixt0);
      result1 = _mm_add_ps(result1, pixt1);

      pixt0 = _mm_shuffle_epi32_f(pixf0, _MM_SHUFFLE(1, 1, 1, 1));
      pixt1 = _mm_shuffle_epi32_f(pixf1, _MM_SHUFFLE(1, 1, 1, 1));
      pixt0 = _mm_mul_ps(pixt0, pg);
      pixt1 = _mm_mul_ps(pixt1, pg);
      result0 = _mm_add_ps(result0, pixt0);
      result1 = _mm_add_ps(result1, pixt1);

      pixt0 = _mm_shuffle_epi32_f(pixf0, _MM_SHUFFLE(0, 0, 0, 0));
      pixt1 = _mm_shuffle_epi32_f(pixf1, _MM_SHUFFLE(0, 0, 0, 0));
      pixt0 = _mm_mul_ps(pixt0, pb);
      pixt1 = _mm_mul_ps(pixt1, pb);
      result0 = _mm_add_ps(result0, pixt0);
      result1 = _mm_add_ps(result1, pixt1);

      result0 = _mm_max_ps(result0, cf0);
      result1 = _mm_max_ps(result1, cf0);
      result0 = _mm_min_ps(result0, cf255);
      result1 = _mm_min_ps(result1, cf255);

      pix0 = _mm_cvtps_epi32(result0);
      pix1 = _mm_cvtps_epi32(result1);
      pix0 = _mm_packs_epi32(pix0, pix0);
      pix1 = _mm_packs_epi32(pix1, pix1);
      pix0 = _mm_packus_epi16(pix0, pix1);
      pix0 = _mm_shuffle_epi32(pix0, _MM_SHUFFLE(2, 0, 2, 0));

      sse2_store8(dst, pix0);

      dst += 8;
      src += 8;
    }
    w += 2;

    if (w & 1)
    {
      __m128i pix;
      __m128 pixf;
      __m128 pixt;
      __m128 result;

      sse2_load4(pix, src);
      pix = _mm_or_si128(pix, FOG_SSE_GET_CONST_PI(FF000000FF000000_FF000000FF000000));
      sse2_unpack_to_float(pixf, pix);

      result = pt;

      pixt = _mm_shuffle_epi32_f(pixf, _MM_SHUFFLE(2, 2, 2, 2));
      pixt = _mm_mul_ps(pixt, pr);
      result = _mm_add_ps(result, pixt);

      pixt = _mm_shuffle_epi32_f(pixf, _MM_SHUFFLE(1, 1, 1, 1));
      pixt = _mm_mul_ps(pixt, pg);
      result = _mm_add_ps(result, pixt);

      pixt = _mm_shuffle_epi32_f(pixf, _MM_SHUFFLE(0, 0, 0, 0));
      pixt = _mm_mul_ps(pixt, pb);
      result = _mm_add_ps(result, pixt);

      result = _mm_max_ps(result, cf0);
      result = _mm_min_ps(result, cf255);

      sse2_pack_from_float(pix, result);
      sse2_store4(dst, pix);
    }
  }

  static void FOG_FASTCALL color_matrix_a8(
    const ColorMatrix* cm,
    uint8_t* dst, const uint8_t* src, int w)
  {
    __m128 cf0 = _mm_set_ps(0.0f, 0.0f, 0.0f, 0.0f);
    __m128 cf255 = _mm_set1_ps(255.0f);

    __m128 pa = _mm_set1_ps(cm->m[3][3]);
    __m128 pt = _mm_set1_ps(cm->m[4][3]);

    pt = _mm_mul_ps(pa, cf255);

    while ((w -= 4) >= 0)
    {
      __m128i pix;
      __m128 pixf;

      sse2_load4(pix, src);
      sse2_unpack_to_float(pixf, pix);

      pixf = _mm_mul_ps(pixf, pa);
      pixf = _mm_add_ps(pixf, pt);

      pixf = _mm_max_ps(pixf, cf0);
      pixf = _mm_min_ps(pixf, cf255);

      sse2_pack_from_float(pix, pixf);
      sse2_store4(dst, pix);

      dst += 4;
      src += 4;
    }
    w += 4;

    while (w)
    {
      __m128i pix;
      __m128 pixf;

      pix = _mm_cvtsi32_si128(src[0]);
      sse2_unpack_to_float(pixf, pix);

      pixf = _mm_mul_ss(pixf, pa);
      pixf = _mm_add_ss(pixf, pt);

      pixf = _mm_max_ss(pixf, cf0);
      pixf = _mm_min_ss(pixf, cf255);

      sse2_pack_from_float(pix, pixf);
      dst[0] = (uint8_t)_mm_cvtsi128_si32(pix);

      dst += 1;
      src += 1;
      w--;
    }
  }

  // --------------------------------------------------------------------------
  // [Fog::RasterEngine::SSE2 - BoxBlur]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL box_blur_h_argb32(
    const BlurParams* params,
    uint8_t* dst, sysint_t dstStride,
    const uint8_t* src, sysint_t srcStride,
    int width, int height, int offset)
  {
    int radius = Math::min<int>((int)params->vRadius, 254);

    if (radius == 0 || width < 2)
    {
      rasterFuncs.filter.copy_area[IMAGE_FORMAT_ARGB32](NULL, dst, dstStride, src, srcStride, width, height, offset);
      return;
    }

    if (radius > 254) radius = 254;

    int dym1 = width;
    int dym2 = height;
    int max = dym1 - 1;
    sysint_t end = max * 4;

    uint8_t* dstCur;
    const uint8_t* srcCur;

    int pos1;
    int pos2;
    int xp, i;

    uint size = (uint)radius * 2 + 1;

    __m128i mmMul = _mm_cvtsi32_si128(getReciprocal(size));
    __m128i mmShr = _mm_cvtsi32_si128(16);
    sse2_expand_pixel_lo_1x4B(mmMul, mmMul);

    uint32_t stack[512];
    uint32_t* stackEnd = stack + size;
    uint32_t* stackCur;

    uint32_t borderExtend = params->borderExtend;
    uint32_t borderColor = params->borderColor;

    uint32_t lBorderColor = borderColor;
    uint32_t rBorderColor = borderColor;

    for (pos2 = 0; pos2 < dym2; pos2++)
    {
      uint32_t pix32;

      __m128i pix;
      __m128i sum = _mm_setzero_si128();

      srcCur = src;

      if (borderExtend == IMAGE_FILTER_EXTEND_PAD)
      {
        lBorderColor = READ_32(srcCur);
        rBorderColor = READ_32(srcCur + end);
      }

      pix32 = lBorderColor;
      sse2_unpack_1x1D(pix, pix32);

      stackCur = stack;

      for (i = 0; i < radius; i++)
      {
        stackCur[0] = pix32;
        stackCur++;

        sum = _mm_add_epi32(sum, pix);
      }

      pix32 = READ_32(srcCur);
      stackCur[0] = pix32;
      stackCur++;

      sse2_unpack_1x1D(pix, pix32);
      sum = _mm_add_epi32(sum, pix);

      for (i = 1; i <= radius; i++)
      {
        if (i <= max)
        {
          srcCur += 4;
          pix32 = READ_32(srcCur);
        }
        else
        {
          pix32 = rBorderColor;
        }

        stackCur[0] = pix32;
        stackCur++;

        sse2_unpack_1x1D(pix, pix32);
        sum = _mm_add_epi32(sum, pix);
      }

      xp = Math::min<int>(radius, max);

      srcCur = src + xp * 4;
      dstCur = dst;

      stackCur = stack;

      for (pos1 = 0; pos1 < dym1; pos1++)
      {
        sse2_mul_const_4D(pix, sum, mmMul);
        pix = _mm_srl_epi32(pix, mmShr);
        sse2_pack_1x1D(pix, pix);
        sse2_store4(dstCur, pix);

        dstCur += 4;

        pix32 = stackCur[0];

        sse2_unpack_1x1D(pix, pix32);
        sum = _mm_sub_epi32(sum, pix);

        if (xp < max)
        {
          ++xp;
          srcCur += 4;
          pix32 = READ_32(srcCur);
        }
        else
        {
          pix32 = rBorderColor;
        }

        stackCur[0] = pix32;

        sse2_unpack_1x1D(pix, pix32);
        sum = _mm_add_epi32(sum, pix);

        stackCur += 1;
        if (stackCur == stackEnd) stackCur = stack;
      }

      src += srcStride;
      dst += dstStride;
    }
  }

  static void FOG_FASTCALL box_blur_h_xrgb32(
    const BlurParams* params,
    uint8_t* dst, sysint_t dstStride,
    const uint8_t* src, sysint_t srcStride,
    int width, int height, int offset)
  {
    int radius = Math::min<int>((int)params->vRadius, 254);

    if (radius == 0 || width < 2)
    {
      rasterFuncs.filter.copy_area[IMAGE_FORMAT_ARGB32](NULL, dst, dstStride, src, srcStride, width, height, offset);
      return;
    }

    if (radius > 254) radius = 254;

    int dym1 = width;
    int dym2 = height;
    int max = dym1 - 1;
    sysint_t end = max * 4;

    uint8_t* dstCur;
    const uint8_t* srcCur;

    int pos1;
    int pos2;
    int xp, i;

    uint size = (uint)radius * 2 + 1;

    __m128i mmMul = _mm_cvtsi32_si128(getReciprocal(size));
    __m128i mmShr = _mm_cvtsi32_si128(16);
    sse2_expand_pixel_lo_1x4B(mmMul, mmMul);

    uint32_t stack[512];
    uint32_t* stackEnd = stack + size;
    uint32_t* stackCur;

    uint32_t borderExtend = params->borderExtend;
    uint32_t borderColor = params->borderColor;

    uint32_t lBorderColor = borderColor;
    uint32_t rBorderColor = borderColor;

    for (pos2 = 0; pos2 < dym2; pos2++)
    {
      uint32_t pix32;

      __m128i pix;
      __m128i sum = _mm_setzero_si128();

      srcCur = src;

      if (borderExtend == IMAGE_FILTER_EXTEND_PAD)
      {
        lBorderColor = READ_32(srcCur);
        rBorderColor = READ_32(srcCur + end);
      }

      pix32 = lBorderColor;
      sse2_unpack_1x1D(pix, pix32);

      stackCur = stack;

      for (i = 0; i < radius; i++)
      {
        stackCur[0] = pix32;
        stackCur++;

        sum = _mm_add_epi32(sum, pix);
      }

      pix32 = READ_32(srcCur);
      stackCur[0] = pix32;
      stackCur++;

      sse2_unpack_1x1D(pix, pix32);
      sum = _mm_add_epi32(sum, pix);

      for (i = 1; i <= radius; i++)
      {
        if (i <= max)
        {
          srcCur += 4;
          pix32 = READ_32(srcCur);
        }
        else
        {
          pix32 = rBorderColor;
        }

        stackCur[0] = pix32;
        stackCur++;

        sse2_unpack_1x1D(pix, pix32);
        sum = _mm_add_epi32(sum, pix);
      }

      xp = Math::min<int>(radius, max);

      srcCur = src + xp * 4;
      dstCur = dst;

      stackCur = stack;

      for (pos1 = 0; pos1 < dym1; pos1++)
      {
        sse2_mul_const_4D(pix, sum, mmMul);
        pix = _mm_srl_epi32(pix, mmShr);
        sse2_pack_1x1D(pix, pix);
        pix = _mm_or_si128(pix, FOG_SSE_GET_CONST_PI(FF000000FF000000_FF000000FF000000));
        sse2_store4(dstCur, pix);

        dstCur += 4;

        pix32 = stackCur[0];

        sse2_unpack_1x1D(pix, pix32);
        sum = _mm_sub_epi32(sum, pix);

        if (xp < max)
        {
          ++xp;
          srcCur += 4;
          pix32 = READ_32(srcCur);
        }
        else
        {
          pix32 = rBorderColor;
        }

        stackCur[0] = pix32;

        sse2_unpack_1x1D(pix, pix32);
        sum = _mm_add_epi32(sum, pix);

        stackCur += 1;
        if (stackCur == stackEnd) stackCur = stack;
      }

      src += srcStride;
      dst += dstStride;
    }
  }

  static void FOG_FASTCALL box_blur_v_argb32(
    const BlurParams* params,
    uint8_t* dst, sysint_t dstStride,
    const uint8_t* src, sysint_t srcStride,
    int width, int height, int offset)
  {
    int radius = Math::min<int>((int)params->vRadius, 254);

    if (radius == 0 || height < 2)
    {
      rasterFuncs.filter.copy_area[IMAGE_FORMAT_ARGB32](NULL, dst, dstStride, src, srcStride, width, height, offset);
      return;
    }

    if (radius > 254) radius = 254;

    int dym1 = height;
    int dym2 = width;
    int max = dym1 - 1;
    sysint_t end = max * srcStride;

    uint8_t* dstCur;
    const uint8_t* srcCur;

    int pos1;
    int pos2;
    int xp, i;

    uint size = (uint)radius * 2 + 1;

    __m128i mmMul = _mm_cvtsi32_si128(getReciprocal(size));
    __m128i mmShr = _mm_cvtsi32_si128(16);
    sse2_expand_pixel_lo_1x4B(mmMul, mmMul);

    uint32_t stack[512];
    uint32_t* stackEnd = stack + size;
    uint32_t* stackCur;

    uint32_t borderExtend = params->borderExtend;
    uint32_t borderColor = params->borderColor;

    uint32_t lBorderColor = borderColor;
    uint32_t rBorderColor = borderColor;

    for (pos2 = 0; pos2 < dym2; pos2++)
    {
      uint32_t pix32;

      __m128i pix;
      __m128i sum = _mm_setzero_si128();

      srcCur = src;

      if (borderExtend == IMAGE_FILTER_EXTEND_PAD)
      {
        lBorderColor = READ_32(srcCur);
        rBorderColor = READ_32(srcCur + end);
      }

      pix32 = lBorderColor;
      sse2_unpack_1x1D(pix, pix32);

      stackCur = stack;

      for (i = 0; i < radius; i++)
      {
        stackCur[0] = pix32;
        stackCur++;

        sum = _mm_add_epi32(sum, pix);
      }

      pix32 = READ_32(srcCur);
      stackCur[0] = pix32;
      stackCur++;

      sse2_unpack_1x1D(pix, pix32);
      sum = _mm_add_epi32(sum, pix);

      for (i = 1; i <= radius; i++)
      {
        if (i <= max)
        {
          srcCur += srcStride;
          pix32 = READ_32(srcCur);
        }
        else
        {
          pix32 = rBorderColor;
        }

        stackCur[0] = pix32;
        stackCur++;

        sse2_unpack_1x1D(pix, pix32);
        sum = _mm_add_epi32(sum, pix);
      }

      xp = Math::min<int>(radius, max);

      srcCur = src + xp * srcStride;
      dstCur = dst;

      stackCur = stack;

      for (pos1 = 0; pos1 < dym1; pos1++)
      {
        sse2_mul_const_4D(pix, sum, mmMul);
        pix = _mm_srl_epi32(pix, mmShr);
        sse2_pack_1x1D(pix, pix);
        sse2_store4(dstCur, pix);

        dstCur += dstStride;

        pix32 = stackCur[0];

        sse2_unpack_1x1D(pix, pix32);
        sum = _mm_sub_epi32(sum, pix);

        if (xp < max)
        {
          ++xp;
          srcCur += srcStride;
          pix32 = READ_32(srcCur);
        }
        else
        {
          pix32 = rBorderColor;
        }

        stackCur[0] = pix32;

        sse2_unpack_1x1D(pix, pix32);
        sum = _mm_add_epi32(sum, pix);

        stackCur += 1;
        if (stackCur == stackEnd) stackCur = stack;
      }

      src += 4;
      dst += 4;
    }
  }

  static void FOG_FASTCALL box_blur_v_xrgb32(
    const BlurParams* params,
    uint8_t* dst, sysint_t dstStride,
    const uint8_t* src, sysint_t srcStride,
    int width, int height, int offset)
  {
    int radius = Math::min<int>((int)params->vRadius, 254);

    if (radius == 0 || height < 2)
    {
      rasterFuncs.filter.copy_area[IMAGE_FORMAT_ARGB32](NULL, dst, dstStride, src, srcStride, width, height, offset);
      return;
    }

    if (radius > 254) radius = 254;

    int dym1 = height;
    int dym2 = width;
    int max = dym1 - 1;
    sysint_t end = max * srcStride;

    uint8_t* dstCur;
    const uint8_t* srcCur;

    int pos1;
    int pos2;
    int xp, i;

    uint size = (uint)radius * 2 + 1;

    __m128i mmMul = _mm_cvtsi32_si128(getReciprocal(size));
    __m128i mmShr = _mm_cvtsi32_si128(16);
    sse2_expand_pixel_lo_1x4B(mmMul, mmMul);

    uint32_t stack[512];
    uint32_t* stackEnd = stack + size;
    uint32_t* stackCur;

    uint32_t borderExtend = params->borderExtend;
    uint32_t borderColor = params->borderColor;

    uint32_t lBorderColor = borderColor;
    uint32_t rBorderColor = borderColor;

    for (pos2 = 0; pos2 < dym2; pos2++)
    {
      uint32_t pix32;

      __m128i pix;
      __m128i sum = _mm_setzero_si128();

      srcCur = src;

      if (borderExtend == IMAGE_FILTER_EXTEND_PAD)
      {
        lBorderColor = READ_32(srcCur);
        rBorderColor = READ_32(srcCur + end);
      }

      pix32 = lBorderColor;
      sse2_unpack_1x1D(pix, pix32);

      stackCur = stack;

      for (i = 0; i < radius; i++)
      {
        stackCur[0] = pix32;
        stackCur++;

        sum = _mm_add_epi32(sum, pix);
      }

      pix32 = READ_32(srcCur);
      stackCur[0] = pix32;
      stackCur++;

      sse2_unpack_1x1D(pix, pix32);
      sum = _mm_add_epi32(sum, pix);

      for (i = 1; i <= radius; i++)
      {
        if (i <= max)
        {
          srcCur += srcStride;
          pix32 = READ_32(srcCur);
        }
        else
        {
          pix32 = rBorderColor;
        }

        stackCur[0] = pix32;
        stackCur++;

        sse2_unpack_1x1D(pix, pix32);
        sum = _mm_add_epi32(sum, pix);
      }

      xp = Math::min<int>(radius, max);

      srcCur = src + xp * srcStride;
      dstCur = dst;

      stackCur = stack;

      for (pos1 = 0; pos1 < dym1; pos1++)
      {
        sse2_mul_const_4D(pix, sum, mmMul);
        pix = _mm_srl_epi32(pix, mmShr);
        sse2_pack_1x1D(pix, pix);
        pix = _mm_or_si128(pix, FOG_SSE_GET_CONST_PI(FF000000FF000000_FF000000FF000000));
        sse2_store4(dstCur, pix);

        dstCur += dstStride;

        pix32 = stackCur[0];

        sse2_unpack_1x1D(pix, pix32);
        sum = _mm_sub_epi32(sum, pix);

        if (xp < max)
        {
          ++xp;
          srcCur += srcStride;
          pix32 = READ_32(srcCur);
        }
        else
        {
          pix32 = rBorderColor;
        }

        stackCur[0] = pix32;

        sse2_unpack_1x1D(pix, pix32);
        sum = _mm_add_epi32(sum, pix);

        stackCur += 1;
        if (stackCur == stackEnd) stackCur = stack;
      }

      src += 4;
      dst += 4;
    }
  }

  // --------------------------------------------------------------------------
  // [Fog::RasterEngine::SSE2 - LinearBlur]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL linear_blur_h_argb32(
    const BlurParams* params,
    uint8_t* dst, sysint_t dstStride,
    const uint8_t* src, sysint_t srcStride,
    int width, int height, int offset)
  {
    int radius = Math::min<int>((int)params->vRadius, 254);

    if (radius == 0 || width < 2)
    {
      rasterFuncs.filter.copy_area[IMAGE_FORMAT_ARGB32](NULL, dst, dstStride, src, srcStride, width, height, offset);
      return;
    }

    if (radius > 254) radius = 254;

    int dym1 = width;
    int dym2 = height;
    int max = dym1 - 1;
    sysint_t end = max * 4;

    uint8_t* dstCur;
    const uint8_t* srcCur;

    int pos1;
    int pos2;
    int xp, i;

    __m128i mmMul = _mm_cvtsi32_si128(raster_linear_blur8_mul[radius]);
    __m128i mmShr = _mm_cvtsi32_si128(raster_linear_blur8_shr[radius]);
    sse2_expand_pixel_lo_1x4B(mmMul, mmMul);

    uint32_t stack[512];
    uint32_t* stackEnd = stack + (radius * 2 + 1);
    uint32_t* stackLeft;
    uint32_t* stackRight;

    uint32_t borderExtend = params->borderExtend;
    uint32_t borderColor = params->borderColor;

    uint32_t lBorderColor = borderColor;
    uint32_t rBorderColor = borderColor;

    for (pos2 = 0; pos2 < dym2; pos2++)
    {
      uint32_t pix32;

      __m128i pix;
      __m128i sum    = _mm_setzero_si128();
      __m128i sumIn  = _mm_setzero_si128();
      __m128i sumOut = _mm_setzero_si128();
      __m128i mi;
      __m128i m1;

      srcCur = src;

      if (borderExtend == IMAGE_FILTER_EXTEND_PAD)
      {
        lBorderColor = READ_32(srcCur);
        rBorderColor = READ_32(srcCur + end);
      }

      pix32 = lBorderColor;
      sse2_unpack_1x1D(pix, pix32);
      mi = pix;

      stackLeft = stack;

      for (i = 0; i < radius; i++)
      {
        stackLeft[0] = pix32;
        stackLeft++;

        sum    = _mm_add_epi32(sum   , mi);
        sumOut = _mm_add_epi32(sumOut, pix);
        mi     = _mm_add_epi32(mi    , pix);
      }

      pix32 = READ_32(srcCur);
      sse2_unpack_1x1D(pix, pix32);

      stackLeft[0] = pix32;
      stackLeft++;

      sse2_expand_mask_1x1D(mi, radius + 1);
      sse2_expand_mask_1x1D(m1, 1);

      sumOut = _mm_add_epi32(sumOut, pix);

      sse2_mul_const_4D(pix, pix, mi);
      sum = _mm_add_epi32(sum, pix);

      for (i = 1; i <= radius; i++)
      {
        if (i <= max)
        {
          srcCur += 4;
          pix32 = READ_32(srcCur);
        }
        else
        {
          pix32 = rBorderColor;
        }

        stackLeft[0] = pix32;
        stackLeft++;

        sse2_unpack_1x1D(pix, pix32);

        mi = _mm_sub_epi32(mi, m1);
        sumIn = _mm_add_epi32(sumIn, pix);
        pix = _mm_mullo_epi16(pix, mi);
        sum = _mm_add_epi32(sum, pix);
      }

      xp = Math::min<int>(radius, max);

      srcCur = src + xp * 4;
      dstCur = dst;

      stackLeft = stack;
      stackRight = stack + radius;

      for (pos1 = 0; pos1 < dym1; pos1++)
      {
        sse2_mul_const_4D(pix, sum, mmMul);
        pix = _mm_srl_epi32(pix, mmShr);
        sse2_pack_1x1D(pix, pix);
        sse2_store4(dstCur, pix);

        dstCur += 4;

        sum = _mm_sub_epi32(sum, sumOut);

        sse2_unpack_1x1D(pix, stackLeft[0]);
        sumOut = _mm_sub_epi32(sumOut, pix);

        if (xp < max)
        {
          ++xp;
          srcCur += 4;
          pix32 = READ_32(srcCur);
        }
        else
        {
          pix32 = rBorderColor;
        }

        stackLeft[0] = pix32;
        sse2_unpack_1x1D(pix, pix32);

        sumIn = _mm_add_epi32(sumIn, pix);
        sum = _mm_add_epi32(sum, sumIn);

        stackLeft += 1;
        stackRight += 1;

        if (stackLeft == stackEnd) stackLeft = stack;
        if (stackRight == stackEnd) stackRight = stack;

        sse2_unpack_1x1D(pix, stackRight[0]);

        sumOut = _mm_add_epi32(sumOut, pix);
        sumIn = _mm_sub_epi32(sumIn, pix);
      }

      src += srcStride;
      dst += dstStride;
    }
  }

  static void FOG_FASTCALL linear_blur_v_argb32(
    const BlurParams* params,
    uint8_t* dst, sysint_t dstStride,
    const uint8_t* src, sysint_t srcStride,
    int width, int height, int offset)
  {
    int radius = Math::min<int>((int)params->vRadius, 254);

    if (radius == 0 || height < 2)
    {
      rasterFuncs.filter.copy_area[IMAGE_FORMAT_ARGB32](NULL, dst, dstStride, src, srcStride, width, height, offset);
      return;
    }

    if (radius > 254) radius = 254;

    int dym1 = height;
    int dym2 = width;
    int max = dym1 - 1;
    sysint_t end = max * srcStride;

    uint8_t* dstCur;
    const uint8_t* srcCur;

    int pos1;
    int pos2;
    int xp, i;

    __m128i mmMul = _mm_cvtsi32_si128(raster_linear_blur8_mul[radius]);
    __m128i mmShr = _mm_cvtsi32_si128(raster_linear_blur8_shr[radius]);
    sse2_expand_pixel_lo_1x4B(mmMul, mmMul);

    uint32_t stack[512];
    uint32_t* stackEnd = stack + (radius * 2 + 1);
    uint32_t* stackLeft;
    uint32_t* stackRight;

    uint32_t borderExtend = params->borderExtend;
    uint32_t borderColor = params->borderColor;

    uint32_t lBorderColor = borderColor;
    uint32_t rBorderColor = borderColor;

    for (pos2 = 0; pos2 < dym2; pos2++)
    {
      uint32_t pix32;

      __m128i pix;
      __m128i sum    = _mm_setzero_si128();
      __m128i sumIn  = _mm_setzero_si128();
      __m128i sumOut = _mm_setzero_si128();
      __m128i mi;
      __m128i m1;

      srcCur = src;

      if (borderExtend == IMAGE_FILTER_EXTEND_PAD)
      {
        lBorderColor = READ_32(srcCur);
        rBorderColor = READ_32(srcCur + end);
      }

      pix32 = lBorderColor;
      sse2_unpack_1x1D(pix, pix32);
      mi = pix;

      stackLeft = stack;

      for (i = 0; i < radius; i++)
      {
        stackLeft[0] = pix32;
        stackLeft++;

        sum    = _mm_add_epi32(sum   , mi);
        sumOut = _mm_add_epi32(sumOut, pix);
        mi     = _mm_add_epi32(mi    , pix);
      }

      pix32 = READ_32(srcCur);
      sse2_unpack_1x1D(pix, pix32);

      stackLeft[0] = pix32;
      stackLeft++;

      sse2_expand_mask_1x1D(mi, radius + 1);
      sse2_expand_mask_1x1D(m1, 1);

      sumOut = _mm_add_epi32(sumOut, pix);

      sse2_mul_const_4D(pix, pix, mi);
      sum = _mm_add_epi32(sum, pix);

      for (i = 1; i <= radius; i++)
      {
        if (i <= max)
        {
          srcCur += srcStride;
          pix32 = READ_32(srcCur);
        }
        else
        {
          pix32 = rBorderColor;
        }

        stackLeft[0] = pix32;
        stackLeft++;

        sse2_unpack_1x1D(pix, pix32);

        mi = _mm_sub_epi32(mi, m1);
        sumIn = _mm_add_epi32(sumIn, pix);
        pix = _mm_mullo_epi16(pix, mi);
        sum = _mm_add_epi32(sum, pix);
      }

      xp = Math::min<int>(radius, max);

      srcCur = src + xp * srcStride;
      dstCur = dst;

      stackLeft = stack;
      stackRight = stack + radius;

      for (pos1 = 0; pos1 < dym1; pos1++)
      {
        sse2_mul_const_4D(pix, sum, mmMul);
        pix = _mm_srl_epi32(pix, mmShr);
        sse2_pack_1x1D(pix, pix);
        sse2_store4(dstCur, pix);

        dstCur += dstStride;

        sum = _mm_sub_epi32(sum, sumOut);

        sse2_unpack_1x1D(pix, stackLeft[0]);
        sumOut = _mm_sub_epi32(sumOut, pix);

        if (xp < max)
        {
          ++xp;
          srcCur += srcStride;
          pix32 = READ_32(srcCur);
        }
        else
        {
          pix32 = rBorderColor;
        }

        stackLeft[0] = pix32;
        sse2_unpack_1x1D(pix, pix32);

        sumIn = _mm_add_epi32(sumIn, pix);
        sum = _mm_add_epi32(sum, sumIn);

        stackLeft += 1;
        stackRight += 1;

        if (stackLeft == stackEnd) stackLeft = stack;
        if (stackRight == stackEnd) stackRight = stack;

        sse2_unpack_1x1D(pix, stackRight[0]);

        sumOut = _mm_add_epi32(sumOut, pix);
        sumIn = _mm_sub_epi32(sumIn, pix);
      }

      src += 4;
      dst += 4;
    }
  }

  // --------------------------------------------------------------------------
  // [SymmetricConvolveFloat]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL symmetric_convolve_float_h_argb32(
    const FloatSymmetricConvolveParams* params,
    uint8_t* dst, sysint_t dstStride,
    const uint8_t* src, sysint_t srcStride,
    int width, int height, int offset)
  {
    const float* kernel = params->hMatrix.getData();
    int size = (int)params->hMatrix.getLength();

    if (size == 0 || width < 2)
    {
      if (dst != src) rasterFuncs.filter.copy_area[IMAGE_FORMAT_ARGB32](NULL, dst, dstStride, src, srcStride, width, height, offset);
      return;
    }

    int dym1 = width;
    int dym2 = height;
    int max = dym1 - 1;
    sysint_t end = max * 4;

    uint8_t* dstCur;
    const uint8_t* srcCur;

    int pos1;
    int pos2;
    int xp, i;

    int sizeHalf = size >> 1;

    LocalBuffer<256*sizeof(uint32_t)> stackBuffer;
    uint32_t* stack = (uint32_t*)stackBuffer.alloc(size * sizeof(uint32_t));
    uint32_t* stackEnd = stack + size;
    uint32_t* stackCur;

    if (!stack) return;

    uint32_t borderExtend = params->borderExtend;
    uint32_t lBorderColor = params->borderColor;
    uint32_t rBorderColor = params->borderColor;

    __m128 const1 = _mm_set1_ps(1.0f);
    __m128 const0 = _mm_set1_ps(0.0f);
    __m128 const255 = _mm_set1_ps(255.0f);

    for (pos2 = 0; pos2 < dym2; pos2++)
    {
      uint32_t pix0;

      dstCur = dst;
      srcCur = src;
      stackCur = stack;

      if (borderExtend == IMAGE_FILTER_EXTEND_PAD)
      {
        lBorderColor = READ_32(srcCur);
        rBorderColor = READ_32(srcCur + end);
      }

      pix0 = lBorderColor;
      xp = 0;

      for (i = 0; i < sizeHalf; i++)
      {
        stackCur[0] = pix0;
        stackCur++;
      }

      for (i = sizeHalf; i < size; i++)
      {
        if (xp < dym1)
        {
          pix0 = READ_32(srcCur);
          srcCur += 4;
          xp++;
        }
        else
        {
          pix0 = rBorderColor;
        }

        stackCur[0] = pix0;
        stackCur++;
      }

      stackCur = stack;

      for (pos1 = 0; pos1 < dym1; pos1++)
      {
        __m128i pixi;
        __m128 z = const1;

        for (i = 0; i < size; i++)
        {
          __m128 k = _mm_set1_ps(kernel[i]);
          __m128 pixf;

          sse2_load4(pixi, (uint8_t*)stackCur);
          sse2_unpack_to_float(pixf, pixi);

          pixf = _mm_mul_ps(pixf, k);
          z = _mm_add_ps(z, pixf);

          stackCur++;
          if (stackCur == stackEnd) stackCur = stack;
        }

        z = _mm_max_ps(z, const0);
        z = _mm_min_ps(z, const255);

        sse2_pack_from_float(pixi, z);
        sse2_store4(dstCur, pixi);
        dstCur += 4;

        if (xp < dym1)
        {
          sse2_load4(pixi, srcCur);
          sse2_store4((uint8_t*)stackCur, pixi);
          srcCur += 4;
          xp++;
        }
        else
        {
          stackCur[0] = rBorderColor;
        }

        stackCur++;
        if (stackCur == stackEnd) stackCur = stack;
      }

      src += srcStride;
      dst += dstStride;
    }
  }

  static void FOG_FASTCALL symmetric_convolve_float_v_argb32(
    const FloatSymmetricConvolveParams* params,
    uint8_t* dst, sysint_t dstStride,
    const uint8_t* src, sysint_t srcStride,
    int width, int height, int offset)
  {
    const float* kernel = params->vMatrix.getData();
    int size = (int)params->vMatrix.getLength();

    if (size == 0 || height < 2)
    {
      if (dst != src) rasterFuncs.filter.copy_area[IMAGE_FORMAT_ARGB32](NULL, dst, dstStride, src, srcStride, width, height, offset);
      return;
    }

    int dym1 = height;
    int dym2 = width;
    int max = dym1 - 1;
    sysint_t end = max * srcStride;

    uint8_t* dstCur;
    const uint8_t* srcCur;

    int pos1;
    int pos2;
    int xp, i;

    int sizeHalf = size >> 1;

    LocalBuffer<256*sizeof(uint32_t)> stackBuffer;
    uint32_t* stack = (uint32_t*)stackBuffer.alloc(size * sizeof(uint32_t));
    uint32_t* stackEnd = stack + size;
    uint32_t* stackCur;

    if (!stack) return;

    uint32_t borderExtend = params->borderExtend;
    uint32_t lBorderColor = params->borderColor;
    uint32_t rBorderColor = params->borderColor;

    __m128 const1 = _mm_set1_ps(1.0f);
    __m128 const0 = _mm_set1_ps(0.0f);
    __m128 const255 = _mm_set1_ps(255.0f);

    for (pos2 = 0; pos2 < dym2; pos2++)
    {
      uint32_t pix0;

      dstCur = dst;
      srcCur = src;
      stackCur = stack;

      if (borderExtend == IMAGE_FILTER_EXTEND_PAD)
      {
        lBorderColor = READ_32(srcCur);
        rBorderColor = READ_32(srcCur + end);
      }

      pix0 = lBorderColor;
      xp = 0;

      for (i = 0; i < sizeHalf; i++)
      {
        stackCur[0] = pix0;
        stackCur++;
      }

      for (i = sizeHalf; i < size; i++)
      {
        if (xp < dym1)
        {
          pix0 = READ_32(srcCur);
          srcCur += srcStride;
          xp++;
        }
        else
        {
          pix0 = rBorderColor;
        }

        stackCur[0] = pix0;
        stackCur++;
      }

      stackCur = stack;

      for (pos1 = 0; pos1 < dym1; pos1++)
      {
        __m128i pixi;
        __m128 z = const1;

        for (i = 0; i < size; i++)
        {
          __m128 k = _mm_set1_ps(kernel[i]);
          __m128 pixf;

          sse2_load4(pixi, (uint8_t*)stackCur);
          sse2_unpack_to_float(pixf, pixi);

          pixf = _mm_mul_ps(pixf, k);
          z = _mm_add_ps(z, pixf);

          stackCur++;
          if (stackCur == stackEnd) stackCur = stack;
        }

        z = _mm_max_ps(z, const0);
        z = _mm_min_ps(z, const255);

        sse2_pack_from_float(pixi, z);
        sse2_store4(dstCur, pixi);
        dstCur += dstStride;

        if (xp < dym1)
        {
          sse2_load4(pixi, srcCur);
          sse2_store4((uint8_t*)stackCur, pixi);
          srcCur += srcStride;
          xp++;
        }
        else
        {
          stackCur[0] = rBorderColor;
        }

        stackCur++;
        if (stackCur == stackEnd) stackCur = stack;
      }

      src += 4;
      dst += 4;
    }
  }
};

} // RasterEngine namespace
} // Fog namespace
