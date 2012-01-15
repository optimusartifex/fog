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
//   1. 'aBorderLeadSize' - Process the leading border pixels, used only by 
//      FE_EXTEND_COLOR or FE_EXTEND_PAD).
//      - No data is write at this step.
//
//   2. 'aTableSize' - Process the pixels which offset is stored in aTableData.
//      - No data is write at this step, ATable is used for indices to pixels.
//
//   3. 'aBorderTailSize' - Process the tailing border pixels, used only by
//      FE_EXTEND_COLOR or FE_EXTEND_PAD as a special case where it's needed
//      to process pixels from the second border.
//      - No data is write at this step.
//
//   4. 'runSize - Standard run-loop, which should do the most of the work.
//      - One pixel is read and stored per loop iteration.
//      - No checking for borders, generally the fastest loop.
//
//   5. 'bTableSize' - Process the pixels which offset is stored in bTableData,
//      - One pixel is read and stored per loop iteration.
//
//   6. 'bBorderTailSize' - Process the tailing border pixels.
//      - No data is read at this step.
//
// The naming convention is strict. For initial processing the prefix 'a' is
// used (aBorderSize, aTableSize, ...). For final processing the prefix 'b' is
// used (bBorderSize, bTableSize, ...).
//
// For SSE2 version please see RasterOps_SSE2 directory.

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

    ctx->blur.extendColor.reset();
    ctx->blur.extendType = feData->_extendType;

    if (ctx->blur.extendType == FE_EXTEND_COLOR)
    {
      if (ImageFormatDescription::getByFormat(dstFormat).is16Bpc())
      {
        Argb64 argb64 = feData->_extendColor().getArgb64();
        Face::p64PRGB64FromARGB64(ctx->blur.extendColor.prgb64.p64, argb64.p64);
      }
      else
      {
        Argb32 argb32 = feData->_extendColor().getArgb32();
        Face::p32PRGB32FromARGB32(ctx->blur.extendColor.prgb32.p32, argb32.p32);
      }
    }

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
    FOG_ASSERT(srcRect->x >= 0);
    FOG_ASSERT(srcRect->y >= 0);
    FOG_ASSERT(srcRect->x + srcRect->w <= srcSize->w);
    FOG_ASSERT(srcRect->y + srcRect->h <= srcSize->h);

    err_t err = ERR_RT_OUT_OF_MEMORY;
    
    MemBufferTmp<1024> memBufferTmp;
    MemBuffer* memBuffer = &memBufferTmp;

    if (ctx->memBuffer)
      memBuffer = ctx->memBuffer;

    RasterConvolve conv;
    conv.filterCtx = ctx;

    int radius, size;
    int i;

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

    // Move to closer location.
    conv.extendType = ctx->blur.extendType;
    conv.extendColor.prgb64.p64 = ctx->blur.extendColor.prgb64.p64;

    // ------------------------------------------------------------------------
    // [Horizontal]
    // ------------------------------------------------------------------------

    radius = ctx->blur.hRadius;
    size = radius * 2 + 1;

    if (tmpBuffer)
    {
      conv.dstData = tmpBuffer;
      conv.dstStride = tmpStride;
    }
    else
    {
      conv.dstData = dst + dstPos->y * dstStride +
                           dstPos->x * (int)dstDesc.getBytesPerPixel();
      conv.dstStride = dstStride;
    }

    conv.srcData = const_cast<uint8_t*>(src) + (srcRect->y - tmpExtendTop) * srcStride;
    conv.srcStride = srcStride;

    i = Math::max(srcSize->w - srcRect->x, 0);
    conv.rowSize     = srcRect->h + tmpExtendTop + tmpExtendBottom;
    conv.runSize     = (i > radius) ? Math::min(i - radius, srcRect->w) : 1;
    conv.runOffset   = (srcRect->x + radius + 1) * (int)srcDesc.getBytesPerPixel();

    conv.aTableSize  = size;
    conv.bTableSize  = Math::min<int>(srcRect->w - conv.runSize, radius);

    conv.kernelRadius= radius;
    conv.kernelSize  = size;

    conv.srcFirstOffset = 0;
    conv.srcLastOffset  = (srcSize->w - 1) * (int)srcDesc.getBytesPerPixel();

    conv.aTableData = static_cast<ssize_t*>(
      ctx->memBuffer->alloc((conv.aTableSize + conv.bTableSize) * sizeof(ssize_t)));
    conv.bTableData = conv.aTableData + conv.aTableSize;

    if (FOG_IS_NULL(conv.aTableData))
      goto _End;

    doFillBorderTables(&conv, srcRect->x - radius, 0, srcSize->w - 1, srcDesc.getBytesPerPixel());
  
    FOG_ASSERT(conv.aTableSize + conv.aBorderLeadSize + conv.aBorderTailSize == size);
    FOG_ASSERT(conv.bTableSize + conv.bBorderTailSize + conv.runSize == srcRect->w);

    ctx->blur.hConvolve(&conv);

    // ------------------------------------------------------------------------
    // [Vertical]
    // ------------------------------------------------------------------------

    radius = ctx->blur.vRadius;
    size = radius * 2 + 1;

    conv.dstData = dst + dstPos->y * dstStride +
                         dstPos->x * (int)dstDesc.getBytesPerPixel();
    conv.dstStride = dstStride;

    if (tmpBuffer)
    {
      conv.srcData = tmpBuffer;
      conv.srcStride = tmpStride;
    }
    else
    {
      conv.srcData = const_cast<uint8_t*>(dst) + dstPos->y * dstStride + dstPos->x * (int)dstDesc.getBytesPerPixel();
      conv.srcStride = dstStride;
    }

    i = Math::max(srcSize->h - srcRect->y, 0);
    conv.rowSize     = srcRect->w;
    conv.runSize     = (i > radius) ? Math::min(i - radius, srcRect->h) : 1;
    conv.runOffset   = (tmpExtendTop + radius + 1) * conv.srcStride;

    conv.aTableSize  = size;
    conv.bTableSize  = Math::min<int>(srcRect->h - conv.runSize, radius);

    conv.kernelRadius= radius;
    conv.kernelSize  = size;

    conv.srcFirstOffset = 0;
    conv.srcLastOffset  = (srcRect->h - 1 + tmpExtendTop + tmpExtendBottom) * conv.srcStride;

    conv.aTableData = static_cast<ssize_t*>(
      ctx->memBuffer->alloc((conv.aTableSize + conv.bTableSize) * sizeof(ssize_t)));
    conv.bTableData = conv.aTableData + conv.aTableSize;

    if (FOG_IS_NULL(conv.aTableData))
      goto _End;

    doFillBorderTables(&conv, tmpExtendTop - radius, 0, srcRect->h - 1 + tmpExtendTop + tmpExtendBottom, conv.srcStride);

    FOG_ASSERT(conv.aTableSize + conv.aBorderLeadSize + conv.aBorderTailSize == size);
    FOG_ASSERT(conv.bTableSize + conv.bBorderTailSize + conv.runSize == srcRect->h);

    ctx->blur.vConvolve(&conv);

    err = ERR_OK;

_End:
    if (tmpBuffer != NULL)
      MemMgr::free(tmpBuffer);

    return err;
  }
  
  static void FOG_FASTCALL doFillBorderTables(
    RasterConvolve* ctx, int t, int tMin, int tMax, ssize_t tMul)
  {
    uint i;
    int tRepeat = tMax - tMin + 1;

    ctx->aBorderLeadSize = 0;
    ctx->aBorderTailSize = 0;
    ctx->bBorderTailSize = 0;

    switch (ctx->extendType)
    {
      case FE_EXTEND_COLOR:
      case FE_EXTEND_PAD:
      {
        // Catch each pixel which is outside of the raster and setup sizes of
        // lead and tail borders. We need to decrease size of aTableSize every
        // time we set the border lead/tail size.
        if (t < tMin)
        {
          ctx->aBorderLeadSize = tMin - t;
          ctx->aTableSize -= ctx->aBorderLeadSize;
          t = tMin;
        }
        
        if (t + int(ctx->aTableSize) > tMax + 1)
        {
          ctx->aBorderTailSize = (t + ctx->aTableSize) - (tMax + 1);
          ctx->aTableSize -= ctx->aBorderTailSize;

          ctx->bBorderTailSize = ctx->bTableSize;
          ctx->bTableSize = 0;
        }
        else 
        {
          int m = t + int(ctx->kernelSize) + int(ctx->runSize);
          if (m > tMax)
          {
            ctx->bBorderTailSize = Math::min<int>(m - tMax, int(ctx->bTableSize));
            ctx->bTableSize -= ctx->bBorderTailSize;
          }
        }

        // Now it's safe to continue using Repeat mode.
        goto _Repeat;
      }

      case FE_EXTEND_REPEAT:
      {
        // Repeat 't'.
        t -= tMin;
        t %= tRepeat;
        if (t < 0)
          t += tRepeat;
        t += tMin;

_Repeat:
        for (i = 0; i < ctx->aTableSize; i++)
        {
          ctx->aTableData[i] = t * tMul;

          if (++t > tMax)
            t = tMin;
        }

        t += ctx->runSize;
        if (t >= tMax)
          t -= tRepeat;

        for (i = 0; i < ctx->bTableSize; i++)
        {
          ctx->bTableData[i] = t * tMul;

          if (++t > tMax)
            t = tMin;
        }
        break;
      }

      case FE_EXTEND_REFLECT:
      {
        // Reflect 't'.
        int tRepeat2 = tRepeat * 2;
        
        t -= tMin;
        t %= tRepeat2;
        if (t < 0)
          t += tRepeat2;

        for (i = 0; i < ctx->aTableSize; i++)
        {
          ctx->aTableData[i] = ((t < tRepeat ? t : tRepeat2 - t) + tMin) * tMul;
          if (++t > tRepeat2)
            t = 0;
        }

        t += ctx->runSize;
        t %= tRepeat2;

        for (i = 0; i < ctx->bTableSize; i++)
        {
          ctx->bTableData[i] = ((t < tRepeat ? t : tRepeat2 - t) + tMin) * tMul;
          if (++t > tRepeat2)
            t = 0;
        }
        break;
      }
    }
  }

  // ==========================================================================
  // [Blur - DoRect - Box]
  // ==========================================================================

  static void FOG_FASTCALL do_rect_box_h_prgb32(
    RasterConvolve* ctx)
  {
    uint8_t* dst = ctx->dstData;
    uint8_t* src = ctx->srcData;

    ssize_t dstStride = ctx->dstStride;
    ssize_t srcStride = ctx->srcStride;

    uint height = ctx->rowSize;
    uint runSize = ctx->runSize;

    uint32_t sumMul = getReciprocal(ctx->kernelSize);
    uint8_t sumShr = 16;

    uint32_t stackBuffer[512];
    uint32_t* stackEnd = stackBuffer + ctx->kernelSize;

    ssize_t* aTableData = ctx->aTableData;
    ssize_t* bTableData = ctx->bTableData;
    
    uint aBorderLeadSize = ctx->aBorderLeadSize;
    uint aBorderTailSize = ctx->aBorderTailSize;
    uint bBorderTailSize = ctx->bBorderTailSize;

    uint aTableSize = ctx->aTableSize;
    uint bTableSize = ctx->bTableSize;

    uint i, y;
    for (y = 0; y < height; y++)
    {
      uint8_t* dstPtr = dst;
      uint8_t* srcPtr = src;
      uint32_t* stackPtr = stackBuffer;

#if defined(FOG_DEBUG)
      // NOTE: Code which uses these variables need to be #ifdefed too,
      // because assertions can be also enabled in release (not normal,
      // but possible to catch different types of bugs). So please put
      // assertions related to bounds checking to #ifdef block.
      uint8_t* srcEnd = src + srcStride;
      uint8_t* dstEnd = dst + dstStride;
#endif // FOG_DEBUG

      uint32_t pix0;
      uint32_t sumA = 0;
      uint32_t sumR = 0;
      uint32_t sumG = 0;
      uint32_t sumB = 0;

      i = aBorderLeadSize;
      if (i != 0)
      {
        pix0 = ctx->extendColor.prgb32.p32;
        if (ctx->extendType == FE_EXTEND_PAD)
          Face::p32Load4a(pix0, srcPtr + ctx->srcFirstOffset);

        sumA += ((pix0 >> 24)       ) * i;
        sumR += ((pix0 >> 16) & 0xFF) * i;
        sumG += ((pix0 >>  8) & 0xFF) * i;
        sumB += ((pix0      ) & 0xFF) * i;

        do {
          FOG_ASSERT(stackPtr < stackEnd);
          stackPtr[0] = pix0;
          stackPtr++;
        } while (--i);
      }

      for (i = 0; i < aTableSize; i++)
      {
#if defined(FOG_DEBUG)
        FOG_ASSERT(srcPtr + aTableData[i] >= src && srcPtr + aTableData[i] < srcEnd);
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

      i = aBorderTailSize;
      if (i != 0)
      {
        pix0 = ctx->extendColor.prgb32.p32;
        if (ctx->extendType == FE_EXTEND_PAD)
          Face::p32Load4a(pix0, srcPtr + ctx->srcLastOffset);

        sumA += ((pix0 >> 24)       ) * i;
        sumR += ((pix0 >> 16) & 0xFF) * i;
        sumG += ((pix0 >>  8) & 0xFF) * i;
        sumB += ((pix0      ) & 0xFF) * i;

        do {
          FOG_ASSERT(stackPtr < stackEnd);
          stackPtr[0] = pix0;
          stackPtr++;
        } while (--i);
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
        FOG_ASSERT(srcPtr + bTableData[i] >= src && srcPtr + bTableData[i] < srcEnd);
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
      
      i = bBorderTailSize;
      if (i != 0)
      {
        uint32_t pixZ = ctx->extendColor.prgb32.p32;
        if (ctx->extendType == FE_EXTEND_PAD)
          Face::p32Load4a(pixZ, srcPtr + ctx->srcLastOffset);

        uint32_t pixA = ((pixZ >> 24)       );
        uint32_t pixR = ((pixZ >> 16) & 0xFF);
        uint32_t pixG = ((pixZ >>  8) & 0xFF);
        uint32_t pixB = ((pixZ      ) & 0xFF);

        do {
          Face::p32Load4a(pix0, stackPtr);
          sumA -= (pix0 >> 24);
          sumR -= (pix0 >> 16) & 0xFF;
          sumG -= (pix0 >>  8) & 0xFF;
          sumB -= (pix0      ) & 0xFF;

          FOG_ASSERT(stackPtr < stackEnd);
          stackPtr[0] = pixZ;
          if (++stackPtr == stackEnd)
            stackPtr = stackBuffer;

          sumA += pixA;
          sumR += pixR;
          sumG += pixG;
          sumB += pixB;

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
      }

      dst += dstStride;
      src += srcStride;
    }
  }

  static void FOG_FASTCALL do_rect_box_v_prgb32(
    RasterConvolve* ctx)
  {
    uint8_t* dst = ctx->dstData;
    uint8_t* src = ctx->srcData;

    ssize_t dstStride = ctx->dstStride;
    ssize_t srcStride = ctx->srcStride;

    uint width = ctx->rowSize;
    uint runSize = ctx->runSize;

    uint32_t sumMul = getReciprocal(ctx->kernelSize);
    uint8_t sumShr = 16;

    uint32_t stackBuffer[512];
    uint32_t* stackEnd = stackBuffer + ctx->kernelSize;

    ssize_t* aTableData = ctx->aTableData;
    ssize_t* bTableData = ctx->bTableData;
    
    uint aBorderLeadSize = ctx->aBorderLeadSize;
    uint aBorderTailSize = ctx->aBorderTailSize;
    uint bBorderTailSize = ctx->bBorderTailSize;

    uint aTableSize = ctx->aTableSize;
    uint bTableSize = ctx->bTableSize;

    uint i, x;
    for (x = 0; x < width; x++)
    {
      uint8_t* dstPtr = dst;
      uint8_t* srcPtr = src;
      uint32_t* stackPtr = stackBuffer;

      uint32_t pix0;
      uint32_t sumA = 0;
      uint32_t sumR = 0;
      uint32_t sumG = 0;
      uint32_t sumB = 0;

      i = aBorderLeadSize;
      if (i != 0)
      {
        pix0 = ctx->extendColor.prgb32.p32;
        if (ctx->extendType == FE_EXTEND_PAD)
          Face::p32Load4a(pix0, srcPtr + ctx->srcFirstOffset);

        sumA += ((pix0 >> 24)       ) * i;
        sumR += ((pix0 >> 16) & 0xFF) * i;
        sumG += ((pix0 >>  8) & 0xFF) * i;
        sumB += ((pix0      ) & 0xFF) * i;

        do {
          FOG_ASSERT(stackPtr < stackEnd);
          stackPtr[0] = pix0;
          stackPtr++;
        } while (--i);
      }

      for (i = 0; i < aTableSize; i++)
      {
        Face::p32Load4a(pix0, srcPtr + aTableData[i]);

        FOG_ASSERT(stackPtr < stackEnd);
        stackPtr[0] = pix0;
        stackPtr++;

        sumA += (pix0 >> 24);
        sumR += (pix0 >> 16) & 0xFF;
        sumG += (pix0 >>  8) & 0xFF;
        sumB += (pix0      ) & 0xFF;
      }

      i = aBorderTailSize;
      if (i != 0)
      {
        pix0 = ctx->extendColor.prgb32.p32;
        if (ctx->extendType == FE_EXTEND_PAD)
          Face::p32Load4a(pix0, srcPtr + ctx->srcLastOffset);

        sumA += ((pix0 >> 24)       ) * i;
        sumR += ((pix0 >> 16) & 0xFF) * i;
        sumG += ((pix0 >>  8) & 0xFF) * i;
        sumB += ((pix0      ) & 0xFF) * i;

        do {
          FOG_ASSERT(stackPtr < stackEnd);
          stackPtr[0] = pix0;
          stackPtr++;
        } while (--i);
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

        Face::p32Load4a(pix0, srcPtr);
        srcPtr += srcStride;

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

        Face::p32Store4a(dstPtr, pix0);
        dstPtr += dstStride;
      } while (--i);

      srcPtr = src;
      for (i = 0; i < bTableSize; i++)
      {
        Face::p32Load4a(pix0, stackPtr);
        sumA -= (pix0 >> 24);
        sumR -= (pix0 >> 16) & 0xFF;
        sumG -= (pix0 >>  8) & 0xFF;
        sumB -= (pix0      ) & 0xFF;

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

        Face::p32Store4a(dstPtr, pix0);
        dstPtr += dstStride;
      }
      
      i = bBorderTailSize;
      if (i != 0)
      {
        uint32_t pixZ = ctx->extendColor.prgb32.p32;
        if (ctx->extendType == FE_EXTEND_PAD)
          Face::p32Load4a(pixZ, srcPtr + ctx->srcLastOffset);

        uint32_t pixA = ((pixZ >> 24)       );
        uint32_t pixR = ((pixZ >> 16) & 0xFF);
        uint32_t pixG = ((pixZ >>  8) & 0xFF);
        uint32_t pixB = ((pixZ      ) & 0xFF);

        do {
          Face::p32Load4a(pix0, stackPtr);
          sumA -= (pix0 >> 24);
          sumR -= (pix0 >> 16) & 0xFF;
          sumG -= (pix0 >>  8) & 0xFF;
          sumB -= (pix0      ) & 0xFF;

          FOG_ASSERT(stackPtr < stackEnd);
          stackPtr[0] = pixZ;
          if (++stackPtr == stackEnd)
            stackPtr = stackBuffer;

          sumA += pixA;
          sumR += pixR;
          sumG += pixG;
          sumB += pixB;

          pix0 = 
            (((sumA * sumMul) >> sumShr) << 24) |
            (((sumR * sumMul) >> sumShr) << 16) |
            (((sumG * sumMul) >> sumShr) <<  8) |
            (((sumB * sumMul) >> sumShr)      ) ;

          Face::p32Store4a(dstPtr, pix0);
          dstPtr += dstStride;
        } while (--i);
      }

      dst += 4;
      src += 4;
    }
  }

  // ==========================================================================
  // [Blur - DoRect (Exp)]
  // ==========================================================================
};

} // Render namespace
} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTEROPS_C_FILTERBLUR_P_H
