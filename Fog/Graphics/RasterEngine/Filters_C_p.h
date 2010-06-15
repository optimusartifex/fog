// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// For some IDEs to enable code-assist.
#include <Fog/Core/Build.h>

#if defined(FOG_IDE)
#include <Fog/Graphics/RasterEngine/Defs_C_p.h>
#include <Fog/Graphics/RasterEngine/Composite_C_p.h>
#include <Fog/Graphics/RasterEngine/Dib_C_p.h>
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
// [Fog::RasterEngine::C - Filter]
// ============================================================================

//! @internal
struct FOG_HIDDEN FilterC
{
  // Get reciprocal for 16-bit value @a val.
  static FOG_INLINE int getReciprocal(int val)
  {
    return (65536 + val - 1) / val;
  }

  // --------------------------------------------------------------------------
  // [ColorLut]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL color_lut_prgb32(
    const ColorLutData* lut,
    uint8_t* dst, const uint8_t* src, int w)
  {
    rasterFuncs.dib.convert[IMAGE_FORMAT_ARGB32][IMAGE_FORMAT_PRGB32](dst, src, w, NULL);
    rasterFuncs.filter.color_lut[IMAGE_FORMAT_ARGB32](lut, dst, dst, w);
    rasterFuncs.dib.convert[IMAGE_FORMAT_PRGB32][IMAGE_FORMAT_ARGB32](dst, dst, w, NULL);
  }

  static void FOG_FASTCALL color_lut_argb32(
    const ColorLutData* lut,
    uint8_t* dst, const uint8_t* src, int w)
  {
    const uint8_t* rLut = lut->r;
    const uint8_t* gLut = lut->g;
    const uint8_t* bLut = lut->b;
    const uint8_t* aLut = lut->a;

    do {
      dst[ARGB32_RBYTE] = rLut[src[ARGB32_RBYTE]];
      dst[ARGB32_GBYTE] = gLut[src[ARGB32_GBYTE]];
      dst[ARGB32_BBYTE] = bLut[src[ARGB32_BBYTE]];
      dst[ARGB32_ABYTE] = aLut[src[ARGB32_ABYTE]];

      dst += 4;
      src += 4;
    } while (--w);
  }

  static void FOG_FASTCALL color_lut_xrgb32(
    const ColorLutData* lut,
    uint8_t* dst, const uint8_t* src, int w)
  {
    const uint8_t* rLut = lut->r;
    const uint8_t* gLut = lut->g;
    const uint8_t* bLut = lut->b;

    do {
      dst[ARGB32_RBYTE] = rLut[src[ARGB32_RBYTE]];
      dst[ARGB32_GBYTE] = gLut[src[ARGB32_GBYTE]];
      dst[ARGB32_BBYTE] = bLut[src[ARGB32_BBYTE]];
      dst[ARGB32_ABYTE] = 0xFF;

      dst += 4;
      src += 4;
    } while (--w);
  }

  static void FOG_FASTCALL color_lut_a8(
    const ColorLutData* lut,
    uint8_t* dst, const uint8_t* src, int w)
  {
    const uint8_t* aLut = lut->a;

    do {
      dst[0] = aLut[src[0]];

      dst += 1;
      src += 1;
    } while (--w);
  }

  // --------------------------------------------------------------------------
  // [ColorMatrix]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL color_matrix_prgb32(
    const ColorMatrix* cm,
    uint8_t* dst, const uint8_t* src, int w)
  {
    float dr = cm->m[4][0] * 255.0f + 0.5f;
    float dg = cm->m[4][1] * 255.0f + 0.5f;
    float db = cm->m[4][2] * 255.0f + 0.5f;
    float da = cm->m[4][3] * 255.0f + 0.5f;

    do {
      int pa = src[ARGB32_ABYTE];
      float demul = raster_demultiply_reciprocal_table_f[pa];

      float fr = (float)src[ARGB32_RBYTE] * demul;
      float fg = (float)src[ARGB32_GBYTE] * demul;
      float fb = (float)src[ARGB32_BBYTE] * demul;
      float fa = (float)pa;

      int tr = (int)(fr * cm->m[0][0] + fg * cm->m[1][0] + fb * cm->m[2][0] + fa * cm->m[3][0] + dr);
      int tg = (int)(fr * cm->m[0][1] + fg * cm->m[1][1] + fb * cm->m[2][1] + fa * cm->m[3][1] + dg);
      int tb = (int)(fr * cm->m[0][2] + fg * cm->m[1][2] + fb * cm->m[2][2] + fa * cm->m[3][2] + db);
      int ta = (int)(fr * cm->m[0][3] + fg * cm->m[1][3] + fb * cm->m[2][3] + fa * cm->m[3][3] + da);

      tr = Math::bound<int>(tr, 0, 255);
      tg = Math::bound<int>(tg, 0, 255);
      tb = Math::bound<int>(tb, 0, 255);
      ta = Math::bound<int>(ta, 0, 255);

      ((uint32_t*)dst)[0] = ColorUtil::premultiply(ta, tr, tg, tb);
      
      dst += 4;
      src += 4;
    } while (--w);
  }

  static void FOG_FASTCALL color_matrix_argb32(
    const ColorMatrix* cm,
    uint8_t* dst, const uint8_t* src, int w)
  {
    float dr = cm->m[4][0] * 255.0f + 0.5f;
    float dg = cm->m[4][1] * 255.0f + 0.5f;
    float db = cm->m[4][2] * 255.0f + 0.5f;
    float da = cm->m[4][3] * 255.0f + 0.5f;

    do {
      float fr = (float)src[ARGB32_RBYTE];
      float fg = (float)src[ARGB32_GBYTE];
      float fb = (float)src[ARGB32_BBYTE];
      float fa = (float)src[ARGB32_ABYTE];

      int tr = (int)(fr * cm->m[0][0] + fg * cm->m[1][0] + fb * cm->m[2][0] + fa * cm->m[3][0] + dr);
      int tg = (int)(fr * cm->m[0][1] + fg * cm->m[1][1] + fb * cm->m[2][1] + fa * cm->m[3][1] + dg);
      int tb = (int)(fr * cm->m[0][2] + fg * cm->m[1][2] + fb * cm->m[2][2] + fa * cm->m[3][2] + db);
      int ta = (int)(fr * cm->m[0][3] + fg * cm->m[1][3] + fb * cm->m[2][3] + fa * cm->m[3][3] + da);

      tr = Math::bound<int>(tr, 0, 255);
      tg = Math::bound<int>(tg, 0, 255);
      tb = Math::bound<int>(tb, 0, 255);
      ta = Math::bound<int>(ta, 0, 255);

      ((uint32_t*)dst)[0] = Argb::make(ta, tr, tg, tb);

      dst += 4;
      src += 4;
    } while (--w);
  }

  static void FOG_FASTCALL color_matrix_xrgb32(
    const ColorMatrix* cm,
    uint8_t* dst, const uint8_t* src, int w)
  {
    float dr = (cm->m[3][0] + cm->m[4][0]) * 255.0f + 0.5f;
    float dg = (cm->m[3][1] + cm->m[4][1]) * 255.0f + 0.5f;
    float db = (cm->m[3][2] + cm->m[4][2]) * 255.0f + 0.5f;

    do {
      float fr = (float)src[ARGB32_RBYTE];
      float fg = (float)src[ARGB32_GBYTE];
      float fb = (float)src[ARGB32_BBYTE];

      int tr = (int)(fr * cm->m[0][0] + fg * cm->m[1][0] + fb * cm->m[2][0] + dr);
      int tg = (int)(fr * cm->m[0][1] + fg * cm->m[1][1] + fb * cm->m[2][1] + dg);
      int tb = (int)(fr * cm->m[0][2] + fg * cm->m[1][2] + fb * cm->m[2][2] + db);

      tr = Math::bound<int>(tr, 0, 255);
      tg = Math::bound<int>(tg, 0, 255);
      tb = Math::bound<int>(tb, 0, 255);

      ((uint32_t*)dst)[0] = Argb::make(0xFF, tr, tg, tb);

      dst += 4;
      src += 4;
    } while (--w);
  }

  static void FOG_FASTCALL color_matrix_a8(
    const ColorMatrix* cm,
    uint8_t* dst, const uint8_t* src, int w)
  {
    float da = cm->m[4][3] * 255.0f + 0.5f;
    float xa = cm->m[3][3];

    do {
      int ta = (int)((float)src[0] * xa + da);
      ta = Math::bound<int>(ta, 0, 255);
      dst[0] = ta;

      dst += 1;
      src += 1;
    } while (--w);
  }

  // --------------------------------------------------------------------------
  // [CopyArea]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL copy_area_32(
    const void* context,
    uint8_t* dst, sysint_t dstStride,
    const uint8_t* src, sysint_t srcStride,
    int width, int height, int offset)
  {
    if (dst == src) return;

    int y = height;
    int size = width * 4;

    if (offset == -1)
    {
      dst += offset * dstStride;
      src += offset * srcStride;

      y = 1;
    }

    while (y)
    {
      Memory::copy(dst, src, size);

      dst += dstStride;
      src += srcStride;
      y--;
    }
  }

  static void FOG_FASTCALL copy_area_24(
    const void* context,
    uint8_t* dst, sysint_t dstStride,
    const uint8_t* src, sysint_t srcStride,
    int width, int height, int offset)
  {
    if (dst == src) return;

    int y = height;
    int size = width * 3;

    if (offset == -1)
    {
      dst += offset * dstStride;
      src += offset * srcStride;

      y = 1;
    }

    while (y)
    {
      Memory::copy(dst, src, size);

      dst += dstStride;
      src += srcStride;
      y--;
    }
  }

  static void FOG_FASTCALL copy_area_8(
    const void* context,
    uint8_t* dst, sysint_t dstStride,
    const uint8_t* src, sysint_t srcStride,
    int width, int height, int offset)
  {
    if (dst == src) return;

    int y = height;
    int size = width;

    if (offset == -1)
    {
      dst += offset * dstStride;
      src += offset * srcStride;

      y = 1;
    }

    while (y)
    {
      Memory::copy(dst, src, size);

      dst += dstStride;
      src += srcStride;
      y--;
    }
  }

  // --------------------------------------------------------------------------
  // [BoxBlur]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL box_blur_h_argb32(
    const BlurParams* params,
    uint8_t* dst, sysint_t dstStride,
    const uint8_t* src, sysint_t srcStride,
    int width, int height, int offset)
  {
    int radius = Math::min<int>((int)params->hRadius, 254);

    if (radius == 0 || width < 2)
    {
      rasterFuncs.filter.copy_area[IMAGE_FORMAT_ARGB32](NULL, dst, dstStride, src, srcStride, width, height, offset);
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

    uint size = radius * 2 + 1;

    uint sumMul = getReciprocal(size);
    uint sumShr = 16;

    uint32_t stack[512];
    uint32_t* stackEnd = stack + size;
    uint32_t* stackCur;

    uint32_t borderExtend = params->borderExtend;
    uint32_t lBorderColor = params->borderColor;
    uint32_t rBorderColor = params->borderColor;

    for (pos2 = 0; pos2 < dym2; pos2++)
    {
      uint32_t pix0;
      uint32_t pixR;
      uint32_t pixG;
      uint32_t pixB;
      uint32_t pixA;

      uint32_t sumR = 0;
      uint32_t sumG = 0;
      uint32_t sumB = 0;
      uint32_t sumA = 0;

      srcCur = src;

      if (borderExtend == IMAGE_FILTER_EXTEND_PAD)
      {
        lBorderColor = READ_32(srcCur);
        rBorderColor = READ_32(srcCur + end);
      }

      pix0 = lBorderColor;
      pixR = (pix0 >> 16) & 0xFF;
      pixG = (pix0 >>  8) & 0xFF;
      pixB = (pix0      ) & 0xFF;
      pixA = (pix0 >> 24);

      stackCur = stack;

      for (i = 0; i < radius; i++)
      {
        stackCur[0] = pix0;
        stackCur++;

        sumR += pixR;
        sumG += pixG;
        sumB += pixB;
        sumA += pixA;
      }

      pix0 = READ_32(srcCur);
      stackCur[0] = pix0;
      stackCur++;

      pixR = (pix0 >> 16) & 0xFF;
      pixG = (pix0 >>  8) & 0xFF;
      pixB = (pix0      ) & 0xFF;
      pixA = (pix0 >> 24);

      sumR += pixR;
      sumG += pixG;
      sumB += pixB;
      sumA += pixA;

      for (i = 1; i <= radius; i++)
      {
        if (i <= max)
        {
          srcCur += 4;
          pix0 = READ_32(srcCur);
        }
        else
        {
          pix0 = rBorderColor;
        }

        stackCur[0] = pix0;
        stackCur++;

        pixR = (pix0 >> 16) & 0xFF;
        pixG = (pix0 >>  8) & 0xFF;
        pixB = (pix0      ) & 0xFF;
        pixA = (pix0 >> 24);

        sumR += pixR;
        sumG += pixG;
        sumB += pixB;
        sumA += pixA;
      }

      xp = Math::min<int>(radius, max);

      srcCur = src + xp * 4;
      dstCur = dst;

      stackCur = stack;

      for (pos1 = 0; pos1 < dym1; pos1++)
      {
        ((uint32_t*)dstCur)[0] =
          (((sumR * sumMul) >> sumShr) << 16) |
          (((sumG * sumMul) >> sumShr) <<  8) |
          (((sumB * sumMul) >> sumShr)      ) |
          (((sumA * sumMul) >> sumShr) << 24) ;
        dstCur += 4;

        pix0 = stackCur[0];

        pixR = (pix0 >> 16) & 0xFF;
        pixG = (pix0 >>  8) & 0xFF;
        pixB = (pix0      ) & 0xFF;
        pixA = (pix0 >> 24);

        sumR -= pixR;
        sumG -= pixG;
        sumB -= pixB;
        sumA -= pixA;

        if (xp < max)
        {
          ++xp;
          srcCur += 4;
          pix0 = READ_32(srcCur);
        }
        else
        {
          pix0 = rBorderColor;
        }

        stackCur[0] = pix0;

        pixR    = (pix0 >> 16) & 0xFF;
        pixG    = (pix0 >>  8) & 0xFF;
        pixB    = (pix0      ) & 0xFF;
        pixA    = (pix0 >> 24);

        sumR += pixR;
        sumG += pixG;
        sumB += pixB;
        sumA += pixA;

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

    int dym1 = height;
    int dym2 = width;
    int max = dym1 - 1;
    sysint_t end = max * srcStride;

    uint8_t* dstCur;
    const uint8_t* srcCur;

    int pos1;
    int pos2;
    int xp, i;

    uint size = radius * 2 + 1;

    uint sumMul = getReciprocal(size);
    uint sumShr = 16;

    uint32_t stack[512];
    uint32_t* stackEnd = stack + size;
    uint32_t* stackCur;

    uint32_t borderExtend = params->borderExtend;
    uint32_t lBorderColor = params->borderColor;
    uint32_t rBorderColor = params->borderColor;

    for (pos2 = 0; pos2 < dym2; pos2++)
    {
      uint32_t pix0;
      uint32_t pixR;
      uint32_t pixG;
      uint32_t pixB;
      uint32_t pixA;

      uint32_t sumR = 0;
      uint32_t sumG = 0;
      uint32_t sumB = 0;
      uint32_t sumA = 0;

      srcCur = src;

      if (borderExtend == IMAGE_FILTER_EXTEND_PAD)
      {
        lBorderColor = READ_32(srcCur);
        rBorderColor = READ_32(srcCur + end);
      }

      pix0 = lBorderColor;
      pixR = (pix0 >> 16) & 0xFF;
      pixG = (pix0 >>  8) & 0xFF;
      pixB = (pix0      ) & 0xFF;
      pixA = (pix0 >> 24);

      stackCur = stack;

      for (i = 0; i < radius; i++)
      {
        stackCur[0] = pix0;
        stackCur++;

        sumR += pixR;
        sumG += pixG;
        sumB += pixB;
        sumA += pixA;
      }

      pix0 = READ_32(srcCur);
      stackCur[0] = pix0;
      stackCur++;

      pixR = (pix0 >> 16) & 0xFF;
      pixG = (pix0 >>  8) & 0xFF;
      pixB = (pix0      ) & 0xFF;
      pixA = (pix0 >> 24);

      sumR += pixR;
      sumG += pixG;
      sumB += pixB;
      sumA += pixA;

      for (i = 1; i <= radius; i++)
      {
        if (i <= max)
        {
          srcCur += srcStride;
          pix0 = READ_32(srcCur);
        }
        else
        {
          pix0 = rBorderColor;
        }

        stackCur[0] = pix0;
        stackCur++;

        pixR = (pix0 >> 16) & 0xFF;
        pixG = (pix0 >>  8) & 0xFF;
        pixB = (pix0      ) & 0xFF;
        pixA = (pix0 >> 24);

        sumR += pixR;
        sumG += pixG;
        sumB += pixB;
        sumA += pixA;
      }

      xp = Math::min<int>(radius, max);

      srcCur = src + xp * srcStride;
      dstCur = dst;

      stackCur = stack;

      for (pos1 = 0; pos1 < dym1; pos1++)
      {
        ((uint32_t*)dstCur)[0] =
          (((sumR * sumMul) >> sumShr) << 16) |
          (((sumG * sumMul) >> sumShr) <<  8) |
          (((sumB * sumMul) >> sumShr)      ) |
          (((sumA * sumMul) >> sumShr) << 24) ;
        dstCur += dstStride;

        pix0 = stackCur[0];

        pixR = (pix0 >> 16) & 0xFF;
        pixG = (pix0 >>  8) & 0xFF;
        pixB = (pix0      ) & 0xFF;
        pixA = (pix0 >> 24);

        sumR -= pixR;
        sumG -= pixG;
        sumB -= pixB;
        sumA -= pixA;

        if (xp < max)
        {
          ++xp;
          srcCur += srcStride;
          pix0 = READ_32(srcCur);
        }
        else
        {
          pix0 = rBorderColor;
        }

        stackCur[0] = pix0;

        pixR    = (pix0 >> 16) & 0xFF;
        pixG    = (pix0 >>  8) & 0xFF;
        pixB    = (pix0      ) & 0xFF;
        pixA    = (pix0 >> 24);

        sumR += pixR;
        sumG += pixG;
        sumB += pixB;
        sumA += pixA;

        stackCur += 1;
        if (stackCur == stackEnd) stackCur = stack;
      }

      src += 4;
      dst += 4;
    }
  }

  static void FOG_FASTCALL box_blur_h_xrgb32(
    const BlurParams* params,
    uint8_t* dst, sysint_t dstStride,
    const uint8_t* src, sysint_t srcStride,
    int width, int height, int offset)
  {
    int radius = Math::min<int>((int)params->hRadius, 254);

    if (radius == 0 || width < 2)
    {
      rasterFuncs.filter.copy_area[IMAGE_FORMAT_XRGB32](NULL, dst, dstStride, src, srcStride, width, height, offset);
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

    uint size = radius * 2 + 1;

    uint sumMul = getReciprocal(size);
    uint sumShr = 16;

    uint32_t stack[512];
    uint32_t* stackEnd = stack + size;
    uint32_t* stackCur;

    uint32_t borderExtend = params->borderExtend;
    uint32_t lBorderColor = params->borderColor;
    uint32_t rBorderColor = params->borderColor;

    for (pos2 = 0; pos2 < dym2; pos2++)
    {
      uint32_t pix0;
      uint32_t pixR;
      uint32_t pixG;
      uint32_t pixB;

      uint32_t sumR = 0;
      uint32_t sumG = 0;
      uint32_t sumB = 0;

      srcCur = src;

      if (borderExtend == IMAGE_FILTER_EXTEND_PAD)
      {
        lBorderColor = READ_32(srcCur);
        rBorderColor = READ_32(srcCur + end);
      }

      pix0 = lBorderColor;
      pixR = (pix0 >> 16) & 0xFF;
      pixG = (pix0 >>  8) & 0xFF;
      pixB = (pix0      ) & 0xFF;

      stackCur = stack;

      for (i = 0; i < radius; i++)
      {
        stackCur[0] = pix0;
        stackCur++;

        sumR += pixR;
        sumG += pixG;
        sumB += pixB;
      }

      pix0 = READ_32(srcCur);
      stackCur[0] = pix0;
      stackCur++;

      pixR = (pix0 >> 16) & 0xFF;
      pixG = (pix0 >>  8) & 0xFF;
      pixB = (pix0      ) & 0xFF;

      sumR += pixR;
      sumG += pixG;
      sumB += pixB;

      for (i = 1; i <= radius; i++)
      {
        if (i <= max)
        {
          srcCur += 4;
          pix0 = READ_32(srcCur);
        }
        else
        {
          pix0 = rBorderColor;
        }

        stackCur[0] = pix0;
        stackCur++;

        pixR = (pix0 >> 16) & 0xFF;
        pixG = (pix0 >>  8) & 0xFF;
        pixB = (pix0      ) & 0xFF;

        sumR += pixR;
        sumG += pixG;
        sumB += pixB;
      }

      xp = Math::min<int>(radius, max);

      srcCur = src + xp * 4;
      dstCur = dst;

      stackCur = stack;

      for (pos1 = 0; pos1 < dym1; pos1++)
      {
        ((uint32_t*)dstCur)[0] =
          (((sumR * sumMul) >> sumShr) << 16) |
          (((sumG * sumMul) >> sumShr) <<  8) |
          (((sumB * sumMul) >> sumShr)      ) |
          0xFF000000;
        dstCur += 4;

        pix0 = stackCur[0];

        pixR = (pix0 >> 16) & 0xFF;
        pixG = (pix0 >>  8) & 0xFF;
        pixB = (pix0      ) & 0xFF;

        sumR -= pixR;
        sumG -= pixG;
        sumB -= pixB;

        if (xp < max)
        {
          ++xp;
          srcCur += 4;
          pix0 = READ_32(srcCur);
        }
        else
        {
          pix0 = rBorderColor;
        }

        stackCur[0] = pix0;

        pixR    = (pix0 >> 16) & 0xFF;
        pixG    = (pix0 >>  8) & 0xFF;
        pixB    = (pix0      ) & 0xFF;

        sumR += pixR;
        sumG += pixG;
        sumB += pixB;

        stackCur += 1;
        if (stackCur == stackEnd) stackCur = stack;
      }

      src += srcStride;
      dst += dstStride;
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
      rasterFuncs.filter.copy_area[IMAGE_FORMAT_XRGB32](NULL, dst, dstStride, src, srcStride, width, height, offset);
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

    uint size = radius * 2 + 1;

    uint sumMul = getReciprocal(size);
    uint sumShr = 16;

    uint32_t stack[512];
    uint32_t* stackEnd = stack + size;
    uint32_t* stackCur;

    uint32_t borderExtend = params->borderExtend;
    uint32_t lBorderColor = params->borderColor;
    uint32_t rBorderColor = params->borderColor;

    for (pos2 = 0; pos2 < dym2; pos2++)
    {
      uint32_t pix0;
      uint32_t pixR;
      uint32_t pixG;
      uint32_t pixB;

      uint32_t sumR = 0;
      uint32_t sumG = 0;
      uint32_t sumB = 0;

      srcCur = src;

      if (borderExtend == IMAGE_FILTER_EXTEND_PAD)
      {
        lBorderColor = READ_32(srcCur);
        rBorderColor = READ_32(srcCur + end);
      }

      pix0 = lBorderColor;
      pixR = (pix0 >> 16) & 0xFF;
      pixG = (pix0 >>  8) & 0xFF;
      pixB = (pix0      ) & 0xFF;

      stackCur = stack;

      for (i = 0; i < radius; i++)
      {
        stackCur[0] = pix0;
        stackCur++;

        sumR += pixR;
        sumG += pixG;
        sumB += pixB;
      }

      pix0 = READ_32(srcCur);
      stackCur[0] = pix0;
      stackCur++;

      pixR = (pix0 >> 16) & 0xFF;
      pixG = (pix0 >>  8) & 0xFF;
      pixB = (pix0      ) & 0xFF;

      sumR += pixR;
      sumG += pixG;
      sumB += pixB;

      for (i = 1; i <= radius; i++)
      {
        if (i <= max)
        {
          srcCur += srcStride;
          pix0 = READ_32(srcCur);
        }
        else
        {
          pix0 = rBorderColor;
        }

        stackCur[0] = pix0;
        stackCur++;

        pixR = (pix0 >> 16) & 0xFF;
        pixG = (pix0 >>  8) & 0xFF;
        pixB = (pix0      ) & 0xFF;

        sumR += pixR;
        sumG += pixG;
        sumB += pixB;
      }

      xp = Math::min<int>(radius, max);

      srcCur = src + xp * srcStride;
      dstCur = dst;

      stackCur = stack;

      for (pos1 = 0; pos1 < dym1; pos1++)
      {
        ((uint32_t*)dstCur)[0] =
          (((sumR * sumMul) >> sumShr) << 16) |
          (((sumG * sumMul) >> sumShr) <<  8) |
          (((sumB * sumMul) >> sumShr)      ) |
          0xFF000000;
        dstCur += dstStride;

        pix0 = stackCur[0];

        pixR = (pix0 >> 16) & 0xFF;
        pixG = (pix0 >>  8) & 0xFF;
        pixB = (pix0      ) & 0xFF;

        sumR -= pixR;
        sumG -= pixG;
        sumB -= pixB;

        if (xp < max)
        {
          ++xp;
          srcCur += srcStride;
          pix0 = READ_32(srcCur);
        }
        else
        {
          pix0 = rBorderColor;
        }

        stackCur[0] = pix0;

        pixR    = (pix0 >> 16) & 0xFF;
        pixG    = (pix0 >>  8) & 0xFF;
        pixB    = (pix0      ) & 0xFF;

        sumR += pixR;
        sumG += pixG;
        sumB += pixB;

        stackCur += 1;
        if (stackCur == stackEnd) stackCur = stack;
      }

      src += 4;
      dst += 4;
    }
  }

  static void FOG_FASTCALL box_blur_h_a8(
    const BlurParams* params,
    uint8_t* dst, sysint_t dstStride,
    const uint8_t* src, sysint_t srcStride,
    int width, int height, int offset)
  {
    int radius = Math::min<int>((int)params->hRadius, 254);

    if (radius == 0 || width < 2)
    {
      rasterFuncs.filter.copy_area[IMAGE_FORMAT_A8](NULL, dst, dstStride, src, srcStride, width, height, offset);
      return;
    }

    int dym1 = width;
    int dym2 = height;
    int max = dym1 - 1;
    sysint_t end = max;

    uint8_t* dstCur;
    const uint8_t* srcCur;

    int pos1;
    int pos2;
    int xp, i;

    uint size = radius * 2 + 1;

    uint sumMul = getReciprocal(size);
    uint sumShr = 16;

    uint8_t stack[512];
    uint8_t* stackEnd = stack + size;
    uint8_t* stackCur;

    uint32_t borderExtend = params->borderExtend;
    uint32_t lBorderColor = params->borderColor;
    uint32_t rBorderColor = params->borderColor;

    for (pos2 = 0; pos2 < dym2; pos2++)
    {
      uint32_t pix;
      uint32_t sum = 0;

      srcCur = src;

      if (borderExtend == IMAGE_FILTER_EXTEND_PAD)
      {
        lBorderColor = srcCur[0];
        rBorderColor = srcCur[end];
      }

      pix = lBorderColor;

      stackCur = stack;

      for (i = 0; i < radius; i++)
      {
        stackCur[0] = pix;
        stackCur++;

        sum += pix;
      }

      pix = srcCur[0];
      stackCur[0] = pix;
      stackCur++;

      sum += pix;

      for (i = 1; i <= radius; i++)
      {
        if (i <= max)
        {
          srcCur += 1;
          pix = srcCur[0];
        }
        else
        {
          pix = rBorderColor;
        }

        stackCur[0] = pix;
        stackCur++;

        sum += pix;
      }

      xp = Math::min<int>(radius, max);

      srcCur = src + xp;
      dstCur = dst;

      stackCur = stack;

      for (pos1 = 0; pos1 < dym1; pos1++)
      {
        dstCur[0] = (sum * sumMul) >> sumShr;
        dstCur += 1;

        pix = stackCur[0];
        sum -= pix;

        if (xp < max)
        {
          ++xp;
          srcCur += 1;
          pix = srcCur[0];
        }
        else
        {
          pix = rBorderColor;
        }

        stackCur[0] = pix;
        sum += pix;

        stackCur += 1;
        if (stackCur == stackEnd) stackCur = stack;
      }

      src += srcStride;
      dst += dstStride;
    }
  }

  static void FOG_FASTCALL box_blur_v_a8(
    const BlurParams* params,
    uint8_t* dst, sysint_t dstStride,
    const uint8_t* src, sysint_t srcStride,
    int width, int height, int offset)
  {
    int radius = Math::min<int>((int)params->vRadius, 254);

    if (radius == 0 || height < 2)
    {
      rasterFuncs.filter.copy_area[IMAGE_FORMAT_A8](NULL, dst, dstStride, src, srcStride, width, height, offset);
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

    uint size = radius * 2 + 1;

    uint sumMul = getReciprocal(size);
    uint sumShr = 16;

    uint8_t stack[512];
    uint8_t* stackEnd = stack + size;
    uint8_t* stackCur;

    uint32_t borderExtend = params->borderExtend;
    uint32_t lBorderColor = params->borderColor;
    uint32_t rBorderColor = params->borderColor;

    for (pos2 = 0; pos2 < dym2; pos2++)
    {
      uint32_t pix;
      uint32_t sum = 0;

      srcCur = src;

      if (borderExtend == IMAGE_FILTER_EXTEND_PAD)
      {
        lBorderColor = srcCur[0];
        rBorderColor = srcCur[end];
      }

      pix = lBorderColor;
      stackCur = stack;

      for (i = 0; i < radius; i++)
      {
        stackCur[0] = pix;
        stackCur++;

        sum += pix;
      }

      pix = srcCur[0];
      stackCur[0] = pix;
      stackCur++;

      sum += pix;

      for (i = 1; i <= radius; i++)
      {
        if (i <= max)
        {
          srcCur += srcStride;
          pix = srcCur[0];
        }
        else
        {
          pix = rBorderColor;
        }

        stackCur[0] = pix;
        stackCur++;

        sum += pix;
      }

      xp = Math::min<int>(radius, max);

      srcCur = src + xp * srcStride;
      dstCur = dst;

      stackCur = stack;

      for (pos1 = 0; pos1 < dym1; pos1++)
      {
        dstCur[0] = (sum * sumMul) >> sumShr;
        dstCur += dstStride;

        pix = stackCur[0];
        sum -= pix;

        if (xp < max)
        {
          ++xp;
          srcCur += srcStride;
          pix = srcCur[0];
        }
        else
        {
          pix = rBorderColor;
        }

        stackCur[0] = pix;
        sum += pix;

        stackCur += 1;
        if (stackCur == stackEnd) stackCur = stack;
      }

      src += 1;
      dst += 1;
    }
  }

  // --------------------------------------------------------------------------
  // [LinearBlur]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL linear_blur_h_argb32(
    const BlurParams* params,
    uint8_t* dst, sysint_t dstStride,
    const uint8_t* src, sysint_t srcStride,
    int width, int height, int offset)
  {
    int radius = Math::min<int>((int)params->hRadius, 254);

    if (radius == 0 || width < 2)
    {
      rasterFuncs.filter.copy_area[IMAGE_FORMAT_ARGB32](NULL, dst, dstStride, src, srcStride, width, height, offset);
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

    uint sumMul = raster_linear_blur8_mul[radius];
    uint sumShr = raster_linear_blur8_shr[radius];

    uint32_t stack[512];
    uint32_t* stackEnd = stack + (radius * 2 + 1);
    uint32_t* stackLeft;
    uint32_t* stackRight;

    uint32_t borderExtend = params->borderExtend;
    uint32_t lBorderColor = params->borderColor;
    uint32_t rBorderColor = params->borderColor;

    for (pos2 = 0; pos2 < dym2; pos2++)
    {
      uint32_t pix0;
      uint32_t pixR;
      uint32_t pixG;
      uint32_t pixB;
      uint32_t pixA;

      uint32_t sumR = 0;
      uint32_t sumG = 0;
      uint32_t sumB = 0;
      uint32_t sumA = 0;

      uint32_t sumInR = 0;
      uint32_t sumInG = 0;
      uint32_t sumInB = 0;
      uint32_t sumInA = 0;

      uint32_t sumOutR = 0;
      uint32_t sumOutG = 0;
      uint32_t sumOutB = 0;
      uint32_t sumOutA = 0;

      srcCur = src;

      if (borderExtend == IMAGE_FILTER_EXTEND_PAD)
      {
        lBorderColor = READ_32(srcCur);
        rBorderColor = READ_32(srcCur + end);
      }

      pix0 = lBorderColor;
      pixR = (pix0 >> 16) & 0xFF;
      pixG = (pix0 >>  8) & 0xFF;
      pixB = (pix0      ) & 0xFF;
      pixA = (pix0 >> 24);

      stackLeft = stack;

      for (i = 0; i < radius; i++)
      {
        stackLeft[0] = pix0;
        stackLeft++;

        sumR            += pixR * (i + 1);
        sumG            += pixG * (i + 1);
        sumB            += pixB * (i + 1);
        sumA            += pixA * (i + 1);

        sumOutR         += pixR;
        sumOutG         += pixG;
        sumOutB         += pixB;
        sumOutA         += pixA;
      }

      pix0 = READ_32(srcCur);
      stackLeft[0] = pix0;
      stackLeft++;

      pixR = (pix0 >> 16) & 0xFF;
      pixG = (pix0 >>  8) & 0xFF;
      pixB = (pix0      ) & 0xFF;
      pixA = (pix0 >> 24);

      sumR            += pixR * (radius + 1);
      sumG            += pixG * (radius + 1);
      sumB            += pixB * (radius + 1);
      sumA            += pixA * (radius + 1);

      sumOutR         += pixR;
      sumOutG         += pixG;
      sumOutB         += pixB;
      sumOutA         += pixA;

      for (i = 1; i <= radius; i++)
      {
        if (i <= max)
        {
          srcCur += 4;
          pix0 = READ_32(srcCur);
        }
        else
        {
          pix0 = rBorderColor;
        }

        pixR = (pix0 >> 16) & 0xFF;
        pixG = (pix0 >>  8) & 0xFF;
        pixB = (pix0      ) & 0xFF;
        pixA = (pix0 >> 24);

        stackLeft[0] = pix0;
        stackLeft++;

        sumR            += pixR * (radius + 1 - i);
        sumG            += pixG * (radius + 1 - i);
        sumB            += pixB * (radius + 1 - i);
        sumA            += pixA * (radius + 1 - i);

        sumInR          += pixR;
        sumInG          += pixG;
        sumInB          += pixB;
        sumInA          += pixA;
      }

      xp = Math::min<int>(radius, max);

      srcCur = src + xp * 4;
      dstCur = dst;

      stackLeft = stack;
      stackRight = stack + radius;

      for (pos1 = 0; pos1 < dym1; pos1++)
      {
        ((uint32_t*)dstCur)[0] =
          (((sumR * sumMul) >> sumShr) << 16) |
          (((sumG * sumMul) >> sumShr) <<  8) |
          (((sumB * sumMul) >> sumShr)      ) |
          (((sumA * sumMul) >> sumShr) << 24) ;
        dstCur += 4;

        sumR -= sumOutR;
        sumG -= sumOutG;
        sumB -= sumOutB;
        sumA -= sumOutA;

        pix0 = stackLeft[0];

        sumOutR -= (pix0 >> 16) & 0xFF;
        sumOutG -= (pix0 >>  8) & 0xFF;
        sumOutB -= (pix0      ) & 0xFF;
        sumOutA -= (pix0 >> 24);

        if (xp < max)
        {
          ++xp;
          srcCur += 4;
          pix0 = READ_32(srcCur);
        }
        else
        {
          pix0 = rBorderColor;
        }

        pixR    = (pix0 >> 16) & 0xFF;
        pixG    = (pix0 >>  8) & 0xFF;
        pixB    = (pix0      ) & 0xFF;
        pixA    = (pix0 >> 24);

        stackLeft[0] = pix0;

        sumInR += pixR;
        sumInG += pixG;
        sumInB += pixB;
        sumInA += pixA;

        sumR   += sumInR;
        sumG   += sumInG;
        sumB   += sumInB;
        sumA   += sumInA;

        stackLeft += 1;
        stackRight += 1;

        if (stackLeft == stackEnd) stackLeft = stack;
        if (stackRight == stackEnd) stackRight = stack;

        pix0    = stackRight[0];
        pixR    = (pix0 >> 16) & 0xFF;
        pixG    = (pix0 >>  8) & 0xFF;
        pixB    = (pix0      ) & 0xFF;
        pixA    = (pix0 >> 24);

        sumOutR += pixR;
        sumOutG += pixG;
        sumOutB += pixB;
        sumOutA += pixA;

        sumInR  -= pixR;
        sumInG  -= pixG;
        sumInB  -= pixB;
        sumInA  -= pixA;
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

    int dym1 = height;
    int dym2 = width;
    int max = dym1 - 1;
    sysint_t end = max * srcStride;

    uint8_t* dstCur;
    const uint8_t* srcCur;

    int pos1;
    int pos2;
    int xp, i;

    uint sumMul = raster_linear_blur8_mul[radius];
    uint sumShr = raster_linear_blur8_shr[radius];

    uint32_t stack[512];
    uint32_t* stackEnd = stack + (radius * 2 + 1);
    uint32_t* stackLeft;
    uint32_t* stackRight;

    uint32_t borderExtend = params->borderExtend;
    uint32_t lBorderColor = params->borderColor;
    uint32_t rBorderColor = params->borderColor;

    for (pos2 = 0; pos2 < dym2; pos2++)
    {
      uint32_t pix0;
      uint32_t pixR;
      uint32_t pixG;
      uint32_t pixB;
      uint32_t pixA;

      uint32_t sumR = 0;
      uint32_t sumG = 0;
      uint32_t sumB = 0;
      uint32_t sumA = 0;

      uint32_t sumInR = 0;
      uint32_t sumInG = 0;
      uint32_t sumInB = 0;
      uint32_t sumInA = 0;

      uint32_t sumOutR = 0;
      uint32_t sumOutG = 0;
      uint32_t sumOutB = 0;
      uint32_t sumOutA = 0;

      srcCur = src;

      if (borderExtend == IMAGE_FILTER_EXTEND_PAD)
      {
        lBorderColor = READ_32(srcCur);
        rBorderColor = READ_32(srcCur + end);
      }

      pix0 = lBorderColor;
      pixR = (pix0 >> 16) & 0xFF;
      pixG = (pix0 >>  8) & 0xFF;
      pixB = (pix0      ) & 0xFF;
      pixA = (pix0 >> 24);

      stackLeft = stack;

      for (i = 0; i < radius; i++)
      {
        stackLeft[0] = pix0;
        stackLeft++;

        sumR            += pixR * (i + 1);
        sumG            += pixG * (i + 1);
        sumB            += pixB * (i + 1);
        sumA            += pixA * (i + 1);

        sumOutR         += pixR;
        sumOutG         += pixG;
        sumOutB         += pixB;
        sumOutA         += pixA;
      }

      pix0 = READ_32(srcCur);
      stackLeft[0] = pix0;
      stackLeft++;

      pixR = (pix0 >> 16) & 0xFF;
      pixG = (pix0 >>  8) & 0xFF;
      pixB = (pix0      ) & 0xFF;
      pixA = (pix0 >> 24);

      sumR            += pixR * (radius + 1);
      sumG            += pixG * (radius + 1);
      sumB            += pixB * (radius + 1);
      sumA            += pixA * (radius + 1);

      sumOutR         += pixR;
      sumOutG         += pixG;
      sumOutB         += pixB;
      sumOutA         += pixA;

      for (i = 1; i <= radius; i++)
      {
        if (i <= max)
        {
          srcCur += srcStride;
          pix0 = READ_32(srcCur);
        }
        else
        {
          pix0 = rBorderColor;
        }

        pixR = (pix0 >> 16) & 0xFF;
        pixG = (pix0 >>  8) & 0xFF;
        pixB = (pix0      ) & 0xFF;
        pixA = (pix0 >> 24);

        stackLeft[0] = pix0;
        stackLeft++;

        sumR            += pixR * (radius + 1 - i);
        sumG            += pixG * (radius + 1 - i);
        sumB            += pixB * (radius + 1 - i);
        sumA            += pixA * (radius + 1 - i);

        sumInR          += pixR;
        sumInG          += pixG;
        sumInB          += pixB;
        sumInA          += pixA;
      }

      xp = Math::min<int>(radius, max);

      srcCur = src + xp * srcStride;
      dstCur = dst;

      stackLeft = stack;
      stackRight = stack + radius;

      for (pos1 = 0; pos1 < dym1; pos1++)
      {
        ((uint32_t*)dstCur)[0] =
          (((sumR * sumMul) >> sumShr) << 16) |
          (((sumG * sumMul) >> sumShr) <<  8) |
          (((sumB * sumMul) >> sumShr)      ) |
          (((sumA * sumMul) >> sumShr) << 24) ;
        dstCur += dstStride;

        sumR -= sumOutR;
        sumG -= sumOutG;
        sumB -= sumOutB;
        sumA -= sumOutA;

        pix0 = stackLeft[0];

        sumOutR -= (pix0 >> 16) & 0xFF;
        sumOutG -= (pix0 >>  8) & 0xFF;
        sumOutB -= (pix0      ) & 0xFF;
        sumOutA -= (pix0 >> 24);

        if (xp < max)
        {
          srcCur += srcStride;
          ++xp;
          pix0 = ((const uint32_t*)srcCur)[0];
        }
        else
        {
          pix0 = rBorderColor;
        }

        pixR    = (pix0 >> 16) & 0xFF;
        pixG    = (pix0 >>  8) & 0xFF;
        pixB    = (pix0      ) & 0xFF;
        pixA    = (pix0 >> 24);

        stackLeft[0] = pix0;

        sumInR += pixR;
        sumInG += pixG;
        sumInB += pixB;
        sumInA += pixA;

        sumR   += sumInR;
        sumG   += sumInG;
        sumB   += sumInB;
        sumA   += sumInA;

        stackLeft += 1;
        stackRight += 1;

        if (stackLeft == stackEnd) stackLeft = stack;
        if (stackRight == stackEnd) stackRight = stack;

        pix0    = stackRight[0];
        pixR    = (pix0 >> 16) & 0xFF;
        pixG    = (pix0 >>  8) & 0xFF;
        pixB    = (pix0      ) & 0xFF;
        pixA    = (pix0 >> 24);

        sumOutR += pixR;
        sumOutG += pixG;
        sumOutB += pixB;
        sumOutA += pixA;

        sumInR  -= pixR;
        sumInG  -= pixG;
        sumInB  -= pixB;
        sumInA  -= pixA;
      }

      src += 4;
      dst += 4;
    }
  }

  static void FOG_FASTCALL linear_blur_h_a8(
    const BlurParams* params,
    uint8_t* dst, sysint_t dstStride,
    const uint8_t* src, sysint_t srcStride,
    int width, int height, int offset)
  {
    int radius = Math::min<int>((int)params->hRadius, 254);

    if (radius == 0 || width < 2)
    {
      rasterFuncs.filter.copy_area[IMAGE_FORMAT_A8](NULL, dst, dstStride, src, srcStride, width, height, offset);
      return;
    }

    int dym1 = width;
    int dym2 = height;
    int max = dym1 - 1;
    sysint_t end = max;

    uint8_t* dstCur;
    const uint8_t* srcCur;

    int pos1;
    int pos2;
    int xp, i;

    uint sumMul = raster_linear_blur8_mul[radius];
    uint sumShr = raster_linear_blur8_shr[radius];

    uint8_t stack[512];
    uint8_t* stackEnd = stack + (radius * 2 + 1);
    uint8_t* stackLeft;
    uint8_t* stackRight;

    uint32_t borderExtend = params->borderExtend;
    uint32_t lBorderColor = params->borderColor;
    uint32_t rBorderColor = params->borderColor;

    for (pos2 = 0; pos2 < dym2; pos2++)
    {
      uint32_t pix;
      uint32_t sum = 0;
      uint32_t sumIn = 0;
      uint32_t sumOut = 0;

      srcCur = src;

      if (borderExtend == IMAGE_FILTER_EXTEND_PAD)
      {
        lBorderColor = srcCur[0];
        rBorderColor = srcCur[end];
      }

      pix = lBorderColor;

      stackLeft = stack;

      for (i = 0; i < radius; i++)
      {
        stackLeft[0] = pix;
        stackLeft += 1;

        sum            += pix * (i + 1);
        sumOut         += pix;
      }

      pix = srcCur[0];

      stackLeft[0] = pix;
      stackLeft += 1;

      sum            += pix * (radius + 1);
      sumOut         += pix;

      for (i = 1; i <= radius; i++)
      {
        if (i <= max)
        {
          srcCur += 1;
          pix = srcCur[0];
        }
        else
        {
          pix = rBorderColor;
        }

        stackLeft[0] = pix;
        stackLeft += 1;

        sum            += pix * (radius + 1 - i);
        sumIn          += pix;
      }

      xp = Math::min<int>(radius, max);

      srcCur = src + xp;
      dstCur = dst;

      stackLeft = stack;
      stackRight = stack + radius;

      for (pos1 = 0; pos1 < dym1; pos1++)
      {
        dstCur[0] = ((sum * sumMul) >> sumShr);
        dstCur += 1;

        sum -= sumOut;
        sumOut -= stackLeft[0];

        if (xp < max)
        {
          srcCur += 1;
          ++xp;

          pix = srcCur[0];
        }
        else
        {
          pix = rBorderColor;
        }

        stackLeft[0] = pix;

        sumIn += pix;
        sum   += sumIn;

        stackLeft += 1;
        stackRight += 1;

        if (stackLeft == stackEnd) stackLeft = stack;
        if (stackRight == stackEnd) stackRight = stack;

        pix    = stackRight[0];

        sumOut += pix;
        sumIn  -= pix;
      }

      src += srcStride;
      dst += dstStride;
    }
  }

  static void FOG_FASTCALL linear_blur_v_a8(
    const BlurParams* params,
    uint8_t* dst, sysint_t dstStride,
    const uint8_t* src, sysint_t srcStride,
    int width, int height, int offset)
  {
    int radius = Math::min<int>((int)params->vRadius, 254);

    if (radius == 0 || height < 2)
    {
      rasterFuncs.filter.copy_area[IMAGE_FORMAT_A8](NULL, dst, dstStride, src, srcStride, width, height, offset);
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

    uint sumMul = raster_linear_blur8_mul[radius];
    uint sumShr = raster_linear_blur8_shr[radius];

    uint8_t stack[512];
    uint8_t* stackEnd = stack + (radius * 2 + 1);
    uint8_t* stackLeft;
    uint8_t* stackRight;

    uint32_t borderExtend = params->borderExtend;
    uint32_t borderColor = params->borderColor;

    uint32_t lBorderColor = params->borderColor;
    uint32_t rBorderColor = params->borderColor;

    for (pos2 = 0; pos2 < dym2; pos2++)
    {
      uint32_t pix;
      uint32_t sum = 0;
      uint32_t sumIn = 0;
      uint32_t sumOut = 0;

      srcCur = src;

      if (borderExtend == IMAGE_FILTER_EXTEND_PAD)
      {
        lBorderColor = srcCur[0];
        rBorderColor = srcCur[end];
      }
      else
      {
        lBorderColor = (borderColor >> 24) & 0xFF;
        rBorderColor = lBorderColor;
      }

      pix = lBorderColor;
      stackLeft = stack;

      for (i = 0; i <= radius; i++)
      {
        stackLeft[0] = pix;
        stackLeft += 1;

        sum            += pix * (i + 1);
        sumOut         += pix;
      }

      pix = srcCur[0];

      stackLeft[0] = pix;
      stackLeft += 1;

      sum            += pix * (radius + 1);
      sumOut         += pix;

      for (i = 1; i < radius; i++)
      {
        if (i <= max)
        {
          srcCur += srcStride;
          pix = srcCur[0];
        }
        else
        {
          pix = rBorderColor;
        }

        stackLeft[0] = pix;
        stackLeft += 1;

        sum            += pix * (radius + 1 - i);
        sumIn          += pix;
      }

      xp = Math::min<int>(radius, max);

      srcCur = src + xp * srcStride;
      dstCur = dst;

      stackLeft = stack;
      stackRight = stack + radius;

      for (pos1 = 0; pos1 < dym1; pos1++)
      {
        dstCur[0] = ((sum * sumMul) >> sumShr);
        dstCur += dstStride;

        sum -= sumOut;
        sumOut -= stackLeft[0];

        if (xp < max)
        {
          srcCur += srcStride;
          ++xp;

          pix = srcCur[0];
        }
        else
        {
          pix = rBorderColor;
        }

        stackLeft[0] = pix;

        sumIn += pix;
        sum   += sumIn;

        stackLeft += 1;
        stackRight += 1;

        if (stackLeft == stackEnd) stackLeft = stack;
        if (stackRight == stackEnd) stackRight = stack;

        pix    = stackRight[0];
        sumOut += pix;
        sumIn  -= pix;
      }

      src += 1;
      dst += 1;
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
      rasterFuncs.filter.copy_area[IMAGE_FORMAT_ARGB32](NULL, dst, dstStride, src, srcStride, width, height, offset);
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
        float cr = 0.5f;
        float cg = 0.5f;
        float cb = 0.5f;
        float ca = 0.5f;

        for (i = 0; i < size; i++)
        {
          float k = kernel[i];
          pix0 = stackCur[0];

          cr += (float)((int)(pix0 >> 16) & 0xFF) * k;
          cg += (float)((int)(pix0 >>  8) & 0xFF) * k;
          cb += (float)((int)(pix0      ) & 0xFF) * k;
          ca += (float)((int)(pix0 >> 24)       ) * k;

          stackCur++;
          if (stackCur == stackEnd) stackCur = stack;
        }

        ((uint32_t*)dstCur)[0] =
          ((uint32_t)Math::boundToByte((int)cr) << 16) |
          ((uint32_t)Math::boundToByte((int)cg) <<  8) |
          ((uint32_t)Math::boundToByte((int)cb)      ) |
          ((uint32_t)Math::boundToByte((int)ca) << 24) ;
        dstCur += 4;

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
      rasterFuncs.filter.copy_area[IMAGE_FORMAT_ARGB32](NULL, dst, dstStride, src, srcStride, width, height, offset);
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
        float cr = 0.5f;
        float cg = 0.5f;
        float cb = 0.5f;
        float ca = 0.5f;

        for (i = 0; i < size; i++)
        {
          float k = kernel[i];
          pix0 = stackCur[0];

          cr += (float)((int)(pix0 >> 16) & 0xFF) * k;
          cg += (float)((int)(pix0 >>  8) & 0xFF) * k;
          cb += (float)((int)(pix0      ) & 0xFF) * k;
          ca += (float)((int)(pix0 >> 24)       ) * k;

          stackCur++;
          if (stackCur == stackEnd) stackCur = stack;
        }

        ((uint32_t*)dstCur)[0] =
          ((uint32_t)Math::boundToByte((int)cr) << 16) |
          ((uint32_t)Math::boundToByte((int)cg) <<  8) |
          ((uint32_t)Math::boundToByte((int)cb)      ) |
          ((uint32_t)Math::boundToByte((int)ca) << 24) ;
        dstCur += dstStride;

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
        if (stackCur == stackEnd) stackCur = stack;
      }

      src += 4;
      dst += 4;
    }
  }

  static void FOG_FASTCALL symmetric_convolve_float_h_a8(
    const FloatSymmetricConvolveParams* params,
    uint8_t* dst, sysint_t dstStride,
    const uint8_t* src, sysint_t srcStride,
    int width, int height, int offset)
  {
    const float* kernel = params->hMatrix.getData();
    int size = (int)params->hMatrix.getLength();

    if (size == 0 || width < 2)
    {
      rasterFuncs.filter.copy_area[IMAGE_FORMAT_ARGB32](NULL, dst, dstStride, src, srcStride, width, height, offset);
      return;
    }

    int dym1 = width;
    int dym2 = height;
    int max = dym1 - 1;
    sysint_t end = max;

    uint8_t* dstCur;
    const uint8_t* srcCur;

    int pos1;
    int pos2;
    int xp, i;

    int sizeHalf = size >> 1;

    LocalBuffer<256*sizeof(uint32_t)> stackBuffer;
    uint8_t* stack = (uint8_t*)stackBuffer.alloc(size * sizeof(uint8_t));
    uint8_t* stackEnd = stack + size;
    uint8_t* stackCur;

    if (!stack) return;

    uint32_t borderExtend = params->borderExtend;
    uint8_t lBorderColor = params->borderColor;
    uint8_t rBorderColor = params->borderColor;

    for (pos2 = 0; pos2 < dym2; pos2++)
    {
      uint8_t pix0;

      dstCur = dst;
      srcCur = src;
      stackCur = stack;

      if (borderExtend == IMAGE_FILTER_EXTEND_PAD)
      {
        lBorderColor = srcCur[0];
        rBorderColor = srcCur[end];
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
          pix0 = srcCur[0];
          srcCur += 1;
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
        float ca = 0.5f;

        for (i = 0; i < size; i++)
        {
          float k = kernel[i];
          pix0 = stackCur[0];

          ca += (float)((int)pix0) * k;

          stackCur++;
          if (stackCur == stackEnd) stackCur = stack;
        }

        dstCur[0] = (uint8_t)Math::boundToByte((int)ca);
        dstCur += 1;

        if (xp < dym1)
        {
          stackCur[0] = srcCur[0];
          srcCur += 1;
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

  static void FOG_FASTCALL symmetric_convolve_float_v_a8(
    const FloatSymmetricConvolveParams* params,
    uint8_t* dst, sysint_t dstStride,
    const uint8_t* src, sysint_t srcStride,
    int width, int height, int offset)
  {
    const float* kernel = params->vMatrix.getData();
    int size = (int)params->vMatrix.getLength();

    if (size == 0 || height < 2)
    {
      rasterFuncs.filter.copy_area[IMAGE_FORMAT_ARGB32](NULL, dst, dstStride, src, srcStride, width, height, offset);
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
    uint8_t* stack = (uint8_t*)stackBuffer.alloc(size * sizeof(uint8_t));
    uint8_t* stackEnd = stack + size;
    uint8_t* stackCur;

    if (!stack) return;

    uint32_t borderExtend = params->borderExtend;
    uint8_t lBorderColor = params->borderColor;
    uint8_t rBorderColor = params->borderColor;

    for (pos2 = 0; pos2 < dym2; pos2++)
    {
      uint8_t pix0;

      dstCur = dst;
      srcCur = src;
      stackCur = stack;

      if (borderExtend == IMAGE_FILTER_EXTEND_PAD)
      {
        lBorderColor = srcCur[0];
        rBorderColor = srcCur[end];
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
          pix0 = srcCur[0];
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
        float ca = 0.5f;

        for (i = 0; i < size; i++)
        {
          float k = kernel[i];
          pix0 = stackCur[0];

          ca += (float)((int)pix0) * k;

          stackCur++;
          if (stackCur == stackEnd) stackCur = stack;
        }

        dstCur[0] = (uint8_t)Math::boundToByte((int)ca);
        dstCur += dstStride;

        if (xp < dym1)
        {
          stackCur[0] = srcCur[0];
          srcCur += 1;
          xp++;
        }
        else
        {
          stackCur[0] = rBorderColor;
        }

        stackCur++;
        if (stackCur == stackEnd) stackCur = stack;
      }

      src += 1;
      dst += 1;
    }
  }
};

} // RasterEngine namespace
} // Fog namespace
