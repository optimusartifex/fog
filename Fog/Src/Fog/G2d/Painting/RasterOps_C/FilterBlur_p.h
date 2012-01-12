// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTEROPS_C_FILTERBLUR_P_H
#define _FOG_G2D_PAINTING_RASTEROPS_C_FILTERBLUR_P_H

// [Dependencies]
#include <Fog/G2d/Painting/RasterOps_C/BaseDefs_p.h>

namespace Fog {
namespace RasterOps_C {

// ============================================================================
// [Fog::RasterOps_C - Filter - Blur]
// ============================================================================

// The Stack Blur Algorithm was invented by Mario Klingemann,
// mario@quasimondo.com and described here:
//
//   http://incubator.quasimondo.com/processing/fast_blur_deluxe.php
//   (search phrase "Stackblur: Fast But Goodlooking").
//
// The major improvement is that there's no more division table that was very
// expensive to create for large blur radii. Insted, for 8-bit per channel and
// radius not exceeding 254 the division is replaced by multiplication and shift.

//! @internal
struct FOG_NO_EXPORT FBlur
{
  // ==========================================================================
  // [Blur - Helpers]
  // ==========================================================================

  // Get reciprocal for 16-bit value @a val.
  static FOG_INLINE int getReciprocal(int val)
  {
    return (val + 65535) / val;
  }

  // ==========================================================================
  // [Blur - Create]
  // ==========================================================================

  static err_t FOG_FASTCALL create(
    RasterFilter* ctx,
    const ImageFilter* filter,
    const ImageFilterScaleD* scale,
    MemBuffer* memBuffer,
    uint32_t dstFormat,
    uint32_t srcFormat)
  {
    if (dstFormat == IMAGE_FORMAT_I8 || srcFormat == IMAGE_FORMAT_I8)
      return ERR_IMAGE_INVALID_FORMAT;

    // TODO: We should allow mixing some basic formats in the future.
    if (dstFormat != srcFormat)
      return ERR_IMAGE_INVALID_FORMAT;

    const FeBlur* feData = reinterpret_cast<const FeBlur*>(filter->_d->getFeData());

    ctx->reference.init(1);
    ctx->destroy = destroy;

    ctx->doRect = do_rect;
    ctx->doLine = NULL;

    ctx->memBuffer = memBuffer;
    ctx->dstFormat = dstFormat;
    ctx->srcFormat = srcFormat;

    float hRadiusScale = 1.0f;
    float vRadiusScale = 1.0f;

    if (scale != NULL)
    {
      hRadiusScale = float(scale->_pt.x);
      vRadiusScale = float(scale->_pt.y);
    }

    ctx->blur.hRadius = Math::bound<int>(Math::abs(Math::iround(feData->_hRadius * hRadiusScale)), 0, 254);
    ctx->blur.vRadius = Math::bound<int>(Math::abs(Math::iround(feData->_vRadius * vRadiusScale)), 0, 254);

    if (scale != NULL && scale->isSwapped())
      swap(ctx->blur.hRadius, ctx->blur.vRadius);

    ctx->blur.hConvolve = _api_raster.filter.blur.box.convolve_h[srcFormat];
    ctx->blur.vConvolve = _api_raster.filter.blur.box.convolve_v[srcFormat];

    return ERR_OK;
  }

  // ==========================================================================
  // [Blur - Destroy]
  // ==========================================================================

  static void FOG_FASTCALL destroy(
    RasterFilter* ctx)
  {
    // Just be safe and detect possible NULL pointer dereference.
    ctx->destroy = NULL;
    ctx->doRect = NULL;
    ctx->doLine = NULL;
  }

  // ==========================================================================
  // [Blur - DoRect]
  // ==========================================================================

  static void FOG_FASTCALL do_rect(
    RasterFilter* ctx,
    uint8_t*       dst, ssize_t dstStride, const SizeI* dstSize, const PointI* dstPos,
    const uint8_t* src, ssize_t srcStride, const SizeI* srcSize, const RectI* srcRect)
  {
    MemBufferTmp<1024> memBufferTmp;
    MemBuffer* memBuffer = &memBufferTmp;

    if (ctx->memBuffer)
      memBuffer = ctx->memBuffer;

    RasterConvolve conv;
    conv.filterCtx = ctx;
    
    int radius, size;

    int i, p;

    const ImageFormatDescription& dstDesc = ImageFormatDescription::getByFormat(ctx->dstFormat);
    const ImageFormatDescription& srcDesc = ImageFormatDescription::getByFormat(ctx->srcFormat);

    dst += dstPos->y * dstStride + dstPos->x * dstDesc.getBytesPerPixel();
    src += srcRect->y * srcStride + srcRect->x * srcDesc.getBytesPerPixel();

    // ------------------------------------------------------------------------
    // [Horizontal]
    // ------------------------------------------------------------------------

    radius = ctx->blur.hRadius;
    size = radius * 2 + 1;

    conv.aTableSize = size;
    conv.bTableSize = Math::max<int>(srcRect->x + srcRect->w + radius - srcSize->w, 0);
    conv.runSize    = srcRect->w - conv.bTableSize; //Math::max<int>(srcSize->w - (srcRect->x + radius + 1), 0);
    conv.kernelSize = size;

    FOG_ASSERT(conv.aTableSize + conv.bTableSize + conv.runSize == srcRect->w + size);

    conv.aTableData = static_cast<ssize_t*>(
      ctx->memBuffer->alloc((conv.aTableSize + conv.bTableSize) * sizeof(ssize_t)));
    conv.bTableData = conv.aTableData + conv.aTableSize;

    // TODO: We should report this.
    if (FOG_IS_NULL(conv.aTableData))
      return;

    p = srcRect->x - radius;
    for (i = 0; i < conv.aTableSize; i++)
    {
      conv.aTableData[i] = (Math::bound<int>(p, 0, srcSize->w - 1) - srcRect->x) * (int)srcDesc.getBytesPerPixel();
      p++;
    }

    p += conv.runSize;
    for (i = 0; i < conv.bTableSize; i++)
    {
      conv.bTableData[i] = (Math::bound<int>(p, 0, srcSize->w - 1) - srcRect->x) * (int)srcDesc.getBytesPerPixel();
      p++;
    }

    ctx->blur.hConvolve(&conv, dst, dstStride, src, srcStride, srcRect->w, srcRect->h);

    // ------------------------------------------------------------------------
    // [Vertical]
    // ------------------------------------------------------------------------

    radius = ctx->blur.vRadius;
    size = radius * 2 + 1;

    conv.aTableSize = size;
    conv.bTableSize = Math::max<int>(srcRect->y + srcRect->h + radius - srcSize->h, 0);
    conv.runSize    = srcRect->h - conv.bTableSize;
    conv.kernelSize = size;

    FOG_ASSERT(conv.aTableSize + conv.bTableSize + conv.runSize == srcRect->h + size);

    conv.aTableData = static_cast<ssize_t*>(
      ctx->memBuffer->alloc((conv.aTableSize + conv.bTableSize) * sizeof(ssize_t)));
    conv.bTableData = conv.aTableData + conv.aTableSize;

    // TODO: We should report this.
    if (FOG_IS_NULL(conv.aTableData))
      return;

    p = srcRect->y - radius;
    for (i = 0; i < conv.aTableSize; i++)
    {
      conv.aTableData[i] = (Math::bound<int>(p, 0, srcSize->h - 1) - srcRect->y) * srcStride;
      p++;
    }

    p += conv.runSize;
    for (i = 0; i < conv.bTableSize; i++)
    {
      conv.bTableData[i] = (Math::bound<int>(p, 0, srcSize->h - 1) - srcRect->y) * srcStride;
      p++;
    }

    ctx->blur.vConvolve(&conv, dst, dstStride, src, srcStride, srcRect->w, srcRect->h);
  }
  
  // ==========================================================================
  // [Blur - DoRect - Box]
  // ==========================================================================
  
  static void FOG_FASTCALL do_rect_box_h_prgb32(
    RasterConvolve* ctx,
    uint8_t*        dst, ssize_t dstStride,
    const uint8_t*  src, ssize_t srcStride,
    int w, int h)
  {
    int i, y;
    int runSize = ctx->runSize;

    uint32_t sumMul = getReciprocal(ctx->kernelSize);
    uint8_t  sumShr = 16;

    uint32_t stackBuffer[512];
    uint32_t* stackEnd = stackBuffer + ctx->kernelSize;

    ssize_t* aTableData = ctx->aTableData;
    ssize_t* bTableData = ctx->bTableData;

    int aTableSize = ctx->aTableSize;
    int bTableSize = ctx->bTableSize;

    for (y = 0; y < h; y++)
    {
      uint8_t*       dstPtr = dst;
      const uint8_t* srcPtr = src;

      uint32_t* stackPtr = stackBuffer;

      uint32_t pix0;
      uint32_t sumA = 0;
      uint32_t sumR = 0;
      uint32_t sumG = 0;
      uint32_t sumB = 0;

      for (i = 0; i < aTableSize; i++)
      {
        Face::p32Load4a(pix0, srcPtr + aTableData[i]);

        stackPtr[0] = pix0;
        stackPtr++;

        sumA += (pix0 >> 24);
        sumR += (pix0 >> 16) & 0xFF;
        sumG += (pix0 >>  8) & 0xFF;
        sumB += (pix0      ) & 0xFF;
      }

      if (stackPtr == stackEnd)
        stackPtr = stackBuffer;

      srcPtr += (ctx->radius + 1) * 4;

      for (i = 0; i < runSize; i++)
      {
        pix0 = 
          (((sumA * sumMul) >> sumShr) << 24) |
          (((sumR * sumMul) >> sumShr) << 16) |
          (((sumG * sumMul) >> sumShr) <<  8) |
          (((sumB * sumMul) >> sumShr)      ) ;
        Face::p32Store4a(dstPtr, pix0);
      
        Face::p32Load4a(pix0, stackPtr);
        sumA -= (pix0 >> 24);
        sumR -= (pix0 >> 16) & 0xFF;
        sumG -= (pix0 >>  8) & 0xFF;
        sumB -= (pix0      ) & 0xFF;

        Face::p32Load4a(pix0, srcPtr);
        stackPtr[0] = pix0;
        if (++stackPtr == stackEnd)
          stackPtr = stackBuffer;

        sumA += (pix0 >> 24);
        sumR += (pix0 >> 16) & 0xFF;
        sumG += (pix0 >>  8) & 0xFF;
        sumB += (pix0      ) & 0xFF;

        dstPtr += 4;
        srcPtr += 4;
      }

      srcPtr = src;

      for (i = 0; i < bTableSize; i++)
      {
        pix0 = 
          (((sumA * sumMul) >> sumShr) << 24) |
          (((sumR * sumMul) >> sumShr) << 16) |
          (((sumG * sumMul) >> sumShr) <<  8) |
          (((sumB * sumMul) >> sumShr)      ) ;
        Face::p32Store4a(dstPtr, pix0);

        Face::p32Load4a(pix0, stackPtr);
        sumA -= (pix0 >> 24);
        sumR -= (pix0 >> 16) & 0xFF;
        sumG -= (pix0 >>  8) & 0xFF;
        sumB -= (pix0      ) & 0xFF;

        Face::p32Load4a(pix0, srcPtr + bTableData[i]);
        stackPtr[0] = pix0;
        if (++stackPtr == stackEnd)
          stackPtr = stackBuffer;

        sumA += (pix0 >> 24);
        sumR += (pix0 >> 16) & 0xFF;
        sumG += (pix0 >>  8) & 0xFF;
        sumB += (pix0      ) & 0xFF;

        dstPtr += 4;
      }

      dst += dstStride;
      src += srcStride;
    }
  }

  static void FOG_FASTCALL do_rect_box_v_prgb32(
    RasterConvolve* ctx,
    uint8_t*        dst, ssize_t dstStride,
    const uint8_t*  src, ssize_t srcStride,
    int w, int h)
  {
  }

  // ==========================================================================
  // [Blur - DoRect (Exp)]
  // ==========================================================================
};

} // Render namespace
} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTEROPS_C_FILTERBLUR_P_H