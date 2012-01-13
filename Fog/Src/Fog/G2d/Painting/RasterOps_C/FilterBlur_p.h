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
//
// Blurring and convolution based effects need to access the neighboring pixels.
// It's fine until you need to access pixels outside the image dimension, in
// this case there are several modes possible, each very specific when it comes
// to implementation. So instead of making big switch() in code, or instead of
// making several versions of blur there is a table where are stored all
// positions of pixels which are outside of the raster.
//
// So in general the blur effector contains five different loops, processed in
// the current order:
//
//   1. Process the initial border pixels (zero border is common).
//      - No data is write at this step.
//
//   2. Process the initial part of the blur.
//      - No data is write at this step, ATable is used for indices to pixels.
//
//   3. Run-loop.
//      - One pixel is loaded and one stored.
//
//   4. Process the tail part of the blur.
//      - BTable is used for indices to pixels.
//
//   5. Process the tail border piexls (zero border is common).
//      - No data is read at this step.
//
// The naming convention is strict. For initial processing the prefix 'a' is
// used (aBorderSize, aTableSize). For tail processing the prefix 'b' is used
// (bBorderSize, bTableSize).

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

  static err_t FOG_FASTCALL do_rect(
    RasterFilter* ctx,
    uint8_t*       dst, ssize_t dstStride, const SizeI* dstSize, const PointI* dstPos,
    const uint8_t* src, ssize_t srcStride, const SizeI* srcSize, const RectI* srcRect)
  {
    err_t err = ERR_RT_OUT_OF_MEMORY;

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

    int tmpExtendTop;
    int tmpExtendBottom;
    int tmpHeight;

    ssize_t tmpStride = 0;
    uint8_t* tmpBuffer = NULL;
  
    // ------------------------------------------------------------------------
    // [Base]
    // ------------------------------------------------------------------------

    conv.dstStride = dstStride;
    conv.srcStride = srcStride;

    tmpExtendTop = Math::min(ctx->blur.vRadius, srcRect->y);
    tmpExtendBottom = Math::min(ctx->blur.vRadius, srcSize->h - srcRect->y - srcRect->h);
    
    if ((tmpExtendTop | tmpExtendBottom) != 0)
    {
      tmpHeight = srcRect->h + tmpExtendTop + tmpExtendBottom;
      tmpStride = srcRect->w * 4;
      tmpBuffer = reinterpret_cast<uint8_t*>(MemMgr::alloc(tmpHeight * tmpStride));
      
      if (FOG_IS_NULL(tmpBuffer))
        goto _End;
    }

    // ------------------------------------------------------------------------
    // [Horizontal]
    // ------------------------------------------------------------------------

    radius = ctx->blur.hRadius;
    size = radius * 2 + 1;

    if (tmpBuffer)
      conv.dstData = tmpBuffer;
    else
      conv.dstData = dst + dstPos->y * dstStride +
                           dstPos->x * (int)dstDesc.getBytesPerPixel();

    conv.srcData = const_cast<uint8_t*>(src) + (srcRect->y - tmpExtendTop) * srcStride;

    conv.aBorderSize = 0;
    conv.bBorderSize = 0;

    i = srcSize->w - srcRect->x;
    conv.runSize     = (radius < i) ? Math::min(srcRect->w, i - radius) : 1;
    conv.runOffset   = (srcRect->x + radius + 1) * (int)srcDesc.getBytesPerPixel();

    conv.aTableSize  = size;
    conv.bTableSize  = srcRect->w - conv.runSize;

    conv.rowSize     = srcRect->h + tmpExtendTop + tmpExtendBottom;
    conv.kernelSize  = size;

    FOG_ASSERT(conv.aBorderSize + conv.aTableSize + 
               conv.bBorderSize + conv.bTableSize + conv.runSize == srcRect->w + size);

    conv.aTableData = static_cast<ssize_t*>(
      ctx->memBuffer->alloc((conv.aTableSize + conv.bTableSize) * sizeof(ssize_t)));
    conv.bTableData = conv.aTableData + conv.aTableSize;

    if (FOG_IS_NULL(conv.aTableData))
      goto _End;

    {
      int minX = 0;
      int maxX = srcSize->w - 1;

      p = srcRect->x - radius;
      for (i = 0; i < conv.aTableSize; i++)
      {
        conv.aTableData[i] = Math::bound<int>(p, minX, maxX) * (int)srcDesc.getBytesPerPixel();
        p++;
      }

      p += conv.runSize;
      for (i = 0; i < conv.bTableSize; i++)
      {
        conv.bTableData[i] = Math::bound<int>(p, minX, maxX) * (int)srcDesc.getBytesPerPixel();
        p++;
      }
    }

    ctx->blur.hConvolve(&conv);

    // ------------------------------------------------------------------------
    // [Vertical]
    // ------------------------------------------------------------------------

    radius = ctx->blur.vRadius;
    size = radius * 2 + 1;

    conv.dstData = dst + dstPos->y * dstStride +
                         dstPos->x * (int)dstDesc.getBytesPerPixel();

    if (tmpBuffer)
      conv.srcData = tmpBuffer + tmpExtendTop * tmpStride;
    else
      conv.srcData = const_cast<uint8_t*>(src) + srcRect->y * srcStride;

    conv.aBorderSize = 0;
    conv.bBorderSize = 0;

    i = srcSize->h - srcRect->y;
    conv.runSize     = (radius < i) ? Math::min(srcRect->h, i - radius) : 1;
    conv.runOffset   = (srcRect->y + radius + 1) * (int)srcDesc.getBytesPerPixel();

    conv.aTableSize  = size;
    conv.bTableSize  = srcRect->h - conv.runSize;

    conv.rowSize     = srcRect->w;
    conv.kernelSize  = size;

    FOG_ASSERT(conv.aBorderSize + conv.aTableSize + 
               conv.bBorderSize + conv.bTableSize + conv.runSize == srcRect->h + size);

    conv.aTableData = static_cast<ssize_t*>(
      ctx->memBuffer->alloc((conv.aTableSize + conv.bTableSize) * sizeof(ssize_t)));
    conv.bTableData = conv.aTableData + conv.aTableSize;

    if (FOG_IS_NULL(conv.aTableData))
      goto _End;

    {
      int minY = -tmpExtendTop;
      int maxY = srcSize->h - 1 + tmpExtendBottom;

      p = srcRect->y - radius;
      for (i = 0; i < conv.aTableSize; i++)
      {
        conv.aTableData[i] = Math::bound<int>(p, minY, maxY) * srcStride;
        p++;
      }

      p += conv.runSize;
      for (i = 0; i < conv.bTableSize; i++)
      {
        conv.bTableData[i] = Math::bound<int>(p, minY, maxY) * srcStride;
        p++;
      }
    }

    ctx->blur.vConvolve(&conv);

    err = ERR_OK;
    
_End:
    if (tmpBuffer != NULL)
      MemMgr::free(tmpBuffer);

    return err;
  }
  
  // ==========================================================================
  // [Blur - DoRect - Box]
  // ==========================================================================
  
  static void FOG_FASTCALL do_rect_box_h_prgb32(
    RasterConvolve* ctx)
  {
    uint height = ctx->rowSize;
    uint runSize = ctx->runSize;

    uint32_t sumMul = getReciprocal(ctx->kernelSize);
    uint8_t  sumShr = 16;

    uint32_t stackBuffer[512];
    uint32_t* stackEnd = stackBuffer + ctx->kernelSize;

    ssize_t* aTableData = ctx->aTableData;
    ssize_t* bTableData = ctx->bTableData;
    
    uint8_t* dst = ctx->dstData;
    uint8_t* src = ctx->srcData;

    ssize_t dstStride = ctx->dstStride;
    ssize_t srcStride = ctx->srcStride;

    uint aBorderSize = ctx->aBorderSize;
    uint bBorderSize = ctx->bBorderSize;

    uint aTableSize = ctx->aTableSize;
    uint bTableSize = ctx->bTableSize;

    uint i, y;
    for (y = 0; y < height; y++)
    {
      uint8_t* dstPtr = dst;
      uint8_t* srcPtr = src;
      uint32_t* stackPtr = stackBuffer;

#if defined(FOG_DEBUG)
      uint8_t* srcEnd = src + srcStride;
      uint8_t* dstEnd = dst + dstStride;
#endif // FOG_DEBUG

      uint32_t pix0;
      uint32_t sumA = 0;
      uint32_t sumR = 0;
      uint32_t sumG = 0;
      uint32_t sumB = 0;

      i = aBorderSize;
      if (i != 0)
      {
        pix0 = ctx->borderPixel.u32Lo;
        sumA += ((pix0 >> 24)       ) * aBorderSize;
        sumR += ((pix0 >> 16) & 0xFF) * aBorderSize;
        sumG += ((pix0 >>  8) & 0xFF) * aBorderSize;
        sumB += ((pix0      ) & 0xFF) * aBorderSize;

        do {
          FOG_ASSERT(stackPtr < stackEnd);
          stackPtr[0] = pix0;
          stackPtr++;
        } while (--i);
      }

      for (i = 0; i < aTableSize; i++)
      {
#if defined(FOG_DEBUG)
        FOG_ASSERT(srcPtr + aTableData[i] >= src &&
                   srcPtr + aTableData[i] < srcEnd);
#endif // FOG_DEBUG
        Face::p32Load4a(pix0, srcPtr + aTableData[i]);

        FOG_ASSERT(stackPtr < stackEnd);
        stackPtr[0] = pix0;
        stackPtr++;

        sumA += (pix0 >> 24);
        sumR += (pix0 >> 16) & 0xFF;
        sumG += (pix0 >>  8) & 0xFF;
        sumB += (pix0      ) & 0xFF;
      }

      FOG_ASSERT(stackPtr == stackEnd);
      stackPtr = stackBuffer;
      srcPtr += ctx->runOffset;

      i = runSize;
      FOG_ASSERT(i != 0);

      goto _First;
      do {
        Face::p32Load4a(pix0, stackPtr);
        sumA -= (pix0 >> 24);
        sumR -= (pix0 >> 16) & 0xFF;
        sumG -= (pix0 >>  8) & 0xFF;
        sumB -= (pix0      ) & 0xFF;

#if defined(FOG_DEBUG)
        FOG_ASSERT(srcPtr < srcEnd);
#endif // FOG_DEBUG
        Face::p32Load4a(pix0, srcPtr);
        srcPtr += 4;

        FOG_ASSERT(stackPtr < stackEnd);
        stackPtr[0] = pix0;
        if (++stackPtr == stackEnd)
          stackPtr = stackBuffer;

        sumA += (pix0 >> 24);
        sumR += (pix0 >> 16) & 0xFF;
        sumG += (pix0 >>  8) & 0xFF;
        sumB += (pix0      ) & 0xFF;

_First:
        pix0 = 
          (((sumA * sumMul) >> sumShr) << 24) |
          (((sumR * sumMul) >> sumShr) << 16) |
          (((sumG * sumMul) >> sumShr) <<  8) |
          (((sumB * sumMul) >> sumShr)      ) ;

#if defined(FOG_DEBUG)
        FOG_ASSERT(dstPtr < dstEnd);
#endif // FOG_DEBUG
        Face::p32Store4a(dstPtr, pix0);
        dstPtr += 4;
      } while (--i);

      srcPtr = src;
      for (i = 0; i < bTableSize; i++)
      {
        Face::p32Load4a(pix0, stackPtr);
        sumA -= (pix0 >> 24);
        sumR -= (pix0 >> 16) & 0xFF;
        sumG -= (pix0 >>  8) & 0xFF;
        sumB -= (pix0      ) & 0xFF;

#if defined(FOG_DEBUG)
        FOG_ASSERT(srcPtr + bTableData[i] >= src &&
                   srcPtr + bTableData[i] < srcEnd);
#endif // FOG_DEBUG
        Face::p32Load4a(pix0, srcPtr + bTableData[i]);

        FOG_ASSERT(stackPtr < stackEnd);
        stackPtr[0] = pix0;
        if (++stackPtr == stackEnd)
          stackPtr = stackBuffer;

        sumA += (pix0 >> 24);
        sumR += (pix0 >> 16) & 0xFF;
        sumG += (pix0 >>  8) & 0xFF;
        sumB += (pix0      ) & 0xFF;

        pix0 = 
          (((sumA * sumMul) >> sumShr) << 24) |
          (((sumR * sumMul) >> sumShr) << 16) |
          (((sumG * sumMul) >> sumShr) <<  8) |
          (((sumB * sumMul) >> sumShr)      ) ;

#if defined(FOG_DEBUG)
        FOG_ASSERT(dstPtr < dstEnd);
#endif // FOG_DEBUG
        Face::p32Store4a(dstPtr, pix0);
        dstPtr += 4;
      }
      
      i = bBorderSize;
      if (i != 0)
      {
      }

      dst += dstStride;
      src += srcStride;
    }
  }

  static void FOG_FASTCALL do_rect_box_v_prgb32(
    RasterConvolve* ctx)
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
