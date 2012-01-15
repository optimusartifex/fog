// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTEROPS_C_FILTERBLUR_P_H
#define _FOG_G2D_PAINTING_RASTEROPS_C_FILTERBLUR_P_H

// [Dependencies]
#include <Fog/G2d/Painting/RasterOps_C/FilterBase_p.h>

namespace Fog {
namespace RasterOps_C {

// ============================================================================
// [Fog::RasterOps_C - Filter - Blur - Base]
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

// How many pixels to process horizontally in BlurV. The problem here is that
// when using the standard way (1 pixel per run) then there is unpredicted
// random memory access, which is costly. So we process several pixels in the
// same memory location to reduce the effect of random memory access. Higher
// value means more memory used by blur stack, but should increase performance.
enum { BLUR_RECT_V_HLINE_COUNT = 16 };

// ============================================================================
// [Fog::RasterOps_C - Filter - Base - Component - PRGB32]
// ============================================================================

struct FOG_NO_EXPORT FBlurComponent_PRGB32
{
  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    a = 0;
    r = 0;
    g = 0;
    b = 0;
  }
  
  // --------------------------------------------------------------------------
  // [Set]
  // --------------------------------------------------------------------------

  FOG_INLINE void set(const FBlurComponent_PRGB32& other)
  {
    a = other.a;
    r = other.r;
    g = other.g;
    b = other.b;
  }

  // --------------------------------------------------------------------------
  // [Ops]
  // --------------------------------------------------------------------------

  FOG_INLINE void add(uint32_t pix0)
  {
    a += (pix0 >> 24);
    r += (pix0 >> 16) & 0xFF;
    g += (pix0 >>  8) & 0xFF;
    b += (pix0      ) & 0xFF;
  }

  FOG_INLINE void sub(uint32_t pix0)
  {
    a -= (pix0 >> 24);
    r -= (pix0 >> 16) & 0xFF;
    g -= (pix0 >>  8) & 0xFF;
    b -= (pix0      ) & 0xFF;
  }

  FOG_INLINE void addN(uint32_t pix0, uint32_t mul)
  {
    a += ((pix0 >> 24)       ) * mul;
    r += ((pix0 >> 16) & 0xFF) * mul;
    g += ((pix0 >>  8) & 0xFF) * mul;
    b += ((pix0      ) & 0xFF) * mul;
  }

  FOG_INLINE void add(const FBlurComponent_PRGB32& other)
  {
    a += other.a;
    r += other.r;
    g += other.g;
    b += other.b;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint32_t a;
  uint32_t r;
  uint32_t g;
  uint32_t b;
};

// ============================================================================
// [Fog::RasterOps_C - Filter - Base - Accessor - PRGB32]
// ============================================================================

struct FOG_NO_EXPORT FBlurAccessor_PRGB32
{
  // --------------------------------------------------------------------------
  // [Defs]
  // --------------------------------------------------------------------------

  typedef uint32_t Pixel;
  typedef FBlurComponent_PRGB32 Component;

  enum { PIXEL_BPP = 4 };
  enum { STACK_BPP = 4 };

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  static FOG_INLINE void fetchSolid(Pixel& dst, const RasterSolid& src)
  {
    dst = src.prgb32.p32;
  }

  static FOG_INLINE void fetchPixel(Pixel& dst, const void* src)
  {
    Face::p32Load4a(dst, src);
  }
  
  static FOG_INLINE void fetchStack(Pixel& dst, const void* src)
  {
    Face::p32Load4a(dst, src);
  }

  static FOG_INLINE void storePixel(void* dst, const Pixel& src)
  {
    Face::p32Store4a(dst, src);
  }

  static FOG_INLINE void storeStack(void* dst, const Pixel& src)
  {
    Face::p32Store4a(dst, src);
  }
  
  static FOG_INLINE void storeMulShr(void* dst, const Component& src, uint32_t mul, uint32_t shift)
  {
    uint32_t pix = _FOG_FACE_COMBINE_4(
      ((src.a * mul) >> shift) << 24,
      ((src.r * mul) >> shift) << 16,
      ((src.g * mul) >> shift) <<  8,
      ((src.b * mul) >> shift)      );
    storePixel(dst, pix);
  }

  static FOG_INLINE void unpack(Component& dst, const Pixel& src)
  {
    dst.a = (src >> 24);
    dst.r = (src >> 16) & 0xFF;
    dst.g = (src >>  8) & 0xFF;
    dst.b = (src      ) & 0xFF;
  }
};

// ============================================================================
// [Fog::RasterOps_C - Filter - Base - Component - XRGB32]
// ============================================================================

struct FOG_NO_EXPORT FBlurComponent_XRGB32
{
  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    r = 0;
    g = 0;
    b = 0;
  }
  
  // --------------------------------------------------------------------------
  // [Set]
  // --------------------------------------------------------------------------

  FOG_INLINE void set(const FBlurComponent_XRGB32& other)
  {
    r = other.r;
    g = other.g;
    b = other.b;
  }

  // --------------------------------------------------------------------------
  // [Ops]
  // --------------------------------------------------------------------------

  FOG_INLINE void add(uint32_t pix0)
  {
    r += (pix0 >> 16) & 0xFF;
    g += (pix0 >>  8) & 0xFF;
    b += (pix0      ) & 0xFF;
  }

  FOG_INLINE void sub(uint32_t pix0)
  {
    r -= (pix0 >> 16) & 0xFF;
    g -= (pix0 >>  8) & 0xFF;
    b -= (pix0      ) & 0xFF;
  }

  FOG_INLINE void addN(uint32_t pix0, uint32_t mul)
  {
    r += ((pix0 >> 16) & 0xFF) * mul;
    g += ((pix0 >>  8) & 0xFF) * mul;
    b += ((pix0      ) & 0xFF) * mul;
  }

  FOG_INLINE void add(const FBlurComponent_XRGB32& other)
  {
    r += other.r;
    g += other.g;
    b += other.b;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint32_t r;
  uint32_t g;
  uint32_t b;
};

// ============================================================================
// [Fog::RasterOps_C - Filter - Base - Accessor - XRGB32]
// ============================================================================

struct FOG_NO_EXPORT FBlurAccessor_XRGB32
{
  // --------------------------------------------------------------------------
  // [Defs]
  // --------------------------------------------------------------------------

  typedef uint32_t Pixel;
  typedef FBlurComponent_XRGB32 Component;

  enum { PIXEL_BPP = 4 };
  enum { STACK_BPP = 4 };

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  static FOG_INLINE void fetchSolid(Pixel& dst, const RasterSolid& src)
  {
    dst = src.prgb32.p32;
  }

  static FOG_INLINE void fetchPixel(Pixel& dst, const void* src)
  {
    Face::p32Load4a(dst, src);
  }

  static FOG_INLINE void fetchStack(Pixel& dst, const void* src)
  {
    Face::p32Load4a(dst, src);
  }

  static FOG_INLINE void storePixel(void* dst, const Pixel& src)
  {
    Face::p32Store4a(dst, src);
  }
  
  static FOG_INLINE void storeStack(void* dst, const Pixel& src)
  {
    Face::p32Store4a(dst, src);
  }

  static FOG_INLINE void storeMulShr(void* dst, const Component& src, uint32_t mul, uint32_t shift)
  {
    uint32_t pix = _FOG_FACE_COMBINE_4(
      0xFF000000,
      ((src.r * mul) >> shift) << 16,
      ((src.g * mul) >> shift) <<  8,
      ((src.b * mul) >> shift)      );
    storePixel(dst, pix);
  }

  static FOG_INLINE void unpack(Component& dst, const Pixel& src)
  {
    dst.r = (src >> 16) & 0xFF;
    dst.g = (src >>  8) & 0xFF;
    dst.b = (src      ) & 0xFF;
  }
};

// ============================================================================
// [Fog::RasterOps_C - Filter - Base - Component - RGB24]
// ============================================================================

struct FOG_NO_EXPORT FBlurComponent_RGB24
{
  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    r = 0;
    g = 0;
    b = 0;
  }
  
  // --------------------------------------------------------------------------
  // [Set]
  // --------------------------------------------------------------------------

  FOG_INLINE void set(const FBlurComponent_RGB24& other)
  {
    r = other.r;
    g = other.g;
    b = other.b;
  }

  // --------------------------------------------------------------------------
  // [Ops]
  // --------------------------------------------------------------------------

  FOG_INLINE void add(uint32_t pix0)
  {
    r += (pix0 >> 16) & 0xFF;
    g += (pix0 >>  8) & 0xFF;
    b += (pix0      ) & 0xFF;
  }

  FOG_INLINE void sub(uint32_t pix0)
  {
    r -= (pix0 >> 16) & 0xFF;
    g -= (pix0 >>  8) & 0xFF;
    b -= (pix0      ) & 0xFF;
  }

  FOG_INLINE void addN(uint32_t pix0, uint32_t mul)
  {
    r += ((pix0 >> 16) & 0xFF) * mul;
    g += ((pix0 >>  8) & 0xFF) * mul;
    b += ((pix0      ) & 0xFF) * mul;
  }

  FOG_INLINE void add(const FBlurComponent_RGB24& other)
  {
    r += other.r;
    g += other.g;
    b += other.b;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint32_t r;
  uint32_t g;
  uint32_t b;
};

// ============================================================================
// [Fog::RasterOps_C - Filter - Base - Accessor - RGB24]
// ============================================================================

struct FOG_NO_EXPORT FBlurAccessor_RGB24
{
  // --------------------------------------------------------------------------
  // [Defs]
  // --------------------------------------------------------------------------

  typedef uint32_t Pixel;
  typedef FBlurComponent_RGB24 Component;

  enum { PIXEL_BPP = 3 };
  enum { STACK_BPP = 4 };

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  static FOG_INLINE void fetchSolid(Pixel& dst, const RasterSolid& src)
  {
    dst = src.prgb32.p32;
  }

  static FOG_INLINE void fetchPixel(Pixel& dst, const void* src)
  {
    Face::p32Load3b(dst, src);
  }

  static FOG_INLINE void fetchStack(Pixel& dst, const void* src)
  {
    Face::p32Load4a(dst, src);
  }

  static FOG_INLINE void storePixel(void* dst, const Pixel& src)
  {
    Face::p32Store3b(dst, src);
  }
  
  static FOG_INLINE void storeStack(void* dst, const Pixel& src)
  {
    Face::p32Store4a(dst, src);
  }

  static FOG_INLINE void storeMulShr(void* dst, const Component& src, uint32_t mul, uint32_t shift)
  {
    static_cast<uint8_t*>(dst)[PIXEL_RGB24_POS_R] = ((src.r * mul) >> shift);
    static_cast<uint8_t*>(dst)[PIXEL_RGB24_POS_G] = ((src.g * mul) >> shift);
    static_cast<uint8_t*>(dst)[PIXEL_RGB24_POS_B] = ((src.b * mul) >> shift);
  }

  static FOG_INLINE void unpack(Component& dst, const Pixel& src)
  {
    dst.r = (src >> 16) & 0xFF;
    dst.g = (src >>  8) & 0xFF;
    dst.b = (src      ) & 0xFF;
  }
};

// ============================================================================
// [Fog::RasterOps_C - Filter - Base - Component - A8]
// ============================================================================

struct FOG_NO_EXPORT FBlurComponent_A8
{
  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    a = 0;
  }
  
  // --------------------------------------------------------------------------
  // [Set]
  // --------------------------------------------------------------------------

  FOG_INLINE void set(const FBlurComponent_A8& other)
  {
    a = other.a;
  }

  // --------------------------------------------------------------------------
  // [Ops]
  // --------------------------------------------------------------------------

  FOG_INLINE void add(uint8_t pix0)
  {
    a = uint32_t(pix0);
  }

  FOG_INLINE void sub(uint8_t pix0)
  {
    a -= uint32_t(pix0);
  }

  FOG_INLINE void addN(uint8_t pix0, uint32_t mul)
  {
    a += uint32_t(pix0) * mul;
  }

  FOG_INLINE void add(const FBlurComponent_A8& other)
  {
    a += other.a;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint32_t a;
};

// ============================================================================
// [Fog::RasterOps_C - Filter - Base - Accessor - A8]
// ============================================================================

struct FOG_NO_EXPORT FBlurAccessor_A8
{
  // --------------------------------------------------------------------------
  // [Defs]
  // --------------------------------------------------------------------------

  typedef uint8_t Pixel;
  typedef FBlurComponent_A8 Component;

  enum { PIXEL_BPP = 1 };
  enum { STACK_BPP = 1 };

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  static FOG_INLINE void fetchSolid(Pixel& dst, const RasterSolid& src)
  {
    dst = src.prgb32.a;
  }

  static FOG_INLINE void fetchPixel(Pixel& dst, const void* src)
  {
    Face::p8Load1b(dst, src);
  }

  static FOG_INLINE void fetchStack(Pixel& dst, const void* src)
  {
    Face::p8Load1b(dst, src);
  }

  static FOG_INLINE void storePixel(void* dst, const Pixel& src)
  {
    Face::p8Store1b(dst, src);
  }
  
  static FOG_INLINE void storeStack(void* dst, const Pixel& src)
  {
    Face::p8Store1b(dst, src);
  }

  static FOG_INLINE void storeMulShr(void* dst, const Component& src, uint32_t mul, uint32_t shift)
  {
    Face::p32Store1b(dst, (src.a * mul) >> shift);
  }

  static FOG_INLINE void unpack(Component& dst, const Pixel& src)
  {
    dst.a = uint32_t(src);
  }
};

// ============================================================================
// [Fog::RasterOps_C - Filter - Blur]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT FBlur
{
  // ==========================================================================
  // [Blur - Helpers]
  // ==========================================================================

  //! @brief Get reciprocal for 16-bit value @a val.
  //!
  //! Used to scale SUM of A, R, G, B pixels into the final pixels, which is
  //! stored to the destination buffer.
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

    int kernelRadius;
    int kernelSize;
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
      tmpStride = srcRect->w * dstDesc.getBytesPerPixel();
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

    kernelRadius = ctx->blur.hRadius;
    kernelSize = kernelRadius * 2 + 1;

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
    conv.runSize     = (i > kernelRadius) ? Math::min(i - kernelRadius, srcRect->w) : 1;
    conv.runOffset   = (srcRect->x + kernelRadius + 1) * (int)srcDesc.getBytesPerPixel();

    conv.aTableSize  = kernelSize;
    conv.bTableSize  = Math::min<int>(srcRect->w - conv.runSize, kernelRadius);

    conv.kernelRadius= kernelRadius;
    conv.kernelSize  = kernelSize;

    conv.srcFirstOffset = 0;
    conv.srcLastOffset  = (srcSize->w - 1) * (int)srcDesc.getBytesPerPixel();

    if (memBuffer->alloc(
      (conv.aTableSize + conv.bTableSize) * sizeof(ssize_t) + 
      kernelSize * dstDesc.getBytesPerPixel()) == NULL)
    {
      goto _End;
    }

    conv.aTableData = reinterpret_cast<ssize_t*>(memBuffer->getMem());
    conv.bTableData = conv.aTableData + conv.aTableSize;
    conv.stack = reinterpret_cast<uint8_t*>(conv.bTableData + conv.bTableSize);

    if (FOG_IS_NULL(conv.aTableData))
      goto _End;

    doFillBorderTables(&conv, srcRect->x - kernelRadius, 0, srcSize->w - 1, srcDesc.getBytesPerPixel());
  
    FOG_ASSERT(conv.aTableSize + conv.aBorderLeadSize + conv.aBorderTailSize == kernelSize);
    FOG_ASSERT(conv.bTableSize + conv.bBorderTailSize + conv.runSize == srcRect->w);

    ctx->blur.hConvolve(&conv);

    // ------------------------------------------------------------------------
    // [Vertical]
    // ------------------------------------------------------------------------

    kernelRadius = ctx->blur.vRadius;
    kernelSize = kernelRadius * 2 + 1;

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
    conv.runSize     = (i > kernelRadius) ? Math::min(i - kernelRadius, srcRect->h) : 1;
    conv.runOffset   = (tmpExtendTop + kernelRadius + 1) * conv.srcStride;

    conv.aTableSize  = kernelSize;
    conv.bTableSize  = Math::min<int>(srcRect->h - conv.runSize, kernelRadius);

    conv.kernelRadius= kernelRadius;
    conv.kernelSize  = kernelSize;

    conv.srcFirstOffset = 0;
    conv.srcLastOffset  = (srcRect->h - 1 + tmpExtendTop + tmpExtendBottom) * conv.srcStride;

    if (memBuffer->alloc(
      (conv.aTableSize + conv.bTableSize) * sizeof(ssize_t) + 
      kernelSize * BLUR_RECT_V_HLINE_COUNT * dstDesc.getBytesPerPixel()) == NULL)
    {
      goto _End;
    }

    conv.aTableData = reinterpret_cast<ssize_t*>(memBuffer->getMem());
    conv.bTableData = conv.aTableData + conv.aTableSize;
    conv.stack = reinterpret_cast<uint8_t*>(conv.bTableData + conv.bTableSize);

    doFillBorderTables(&conv, tmpExtendTop - kernelRadius, 0, srcRect->h - 1 + tmpExtendTop + tmpExtendBottom, conv.srcStride);

    FOG_ASSERT(conv.aTableSize + conv.aBorderLeadSize + conv.aBorderTailSize == kernelSize);
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

  template<typename Accessor>
  static void FOG_FASTCALL do_rect_box_h(
    RasterConvolve* ctx)
  {
    uint8_t* dst = ctx->dstData;
    uint8_t* src = ctx->srcData;

    ssize_t dstStride = ctx->dstStride;
    ssize_t srcStride = ctx->srcStride;

    uint runHeight = ctx->rowSize;
    uint runSize = ctx->runSize;

    uint32_t sumMul = getReciprocal(ctx->kernelSize);
    uint32_t sumShr = 16;

    ssize_t* aTableData = ctx->aTableData;
    ssize_t* bTableData = ctx->bTableData;
    uint8_t* stackBuffer = ctx->stack;
    uint8_t* stackEnd = stackBuffer + ctx->kernelSize * Accessor::STACK_BPP;
    
    uint aBorderLeadSize = ctx->aBorderLeadSize;
    uint aBorderTailSize = ctx->aBorderTailSize;
    uint bBorderTailSize = ctx->bBorderTailSize;

    uint aTableSize = ctx->aTableSize;
    uint bTableSize = ctx->bTableSize;

    uint i, r;
    for (r = 0; r < runHeight; r++)
    {
      uint8_t* dstPtr = dst;
      uint8_t* srcPtr = src;
      uint8_t* stackPtr = stackBuffer;

      typename Accessor::Component sum0;
      typename Accessor::Pixel pix0;

      sum0.reset();

#if defined(FOG_DEBUG)
      // NOTE: Code which uses these variables need to be #ifdefed too,
      // because assertions can be also enabled in release (not normal,
      // but possible to catch different types of bugs). So please put
      // assertions related to bounds checking to #ifdef block.
      uint8_t* srcEnd = src + srcStride;
      uint8_t* dstEnd = dst + dstStride;
#endif // FOG_DEBUG

      i = aBorderLeadSize;
      if (i != 0)
      {
        Accessor::fetchSolid(pix0, ctx->extendColor);
        if (ctx->extendType == FE_EXTEND_PAD)
          Accessor::fetchPixel(pix0, srcPtr + ctx->srcFirstOffset);

        sum0.addN(pix0, i);

        do {
          FOG_ASSERT(stackPtr < stackEnd);

          Accessor::storeStack(stackPtr, pix0);
          stackPtr += Accessor::STACK_BPP;
        } while (--i);
      }

      for (i = 0; i < aTableSize; i++)
      {
#if defined(FOG_DEBUG)
        FOG_ASSERT(srcPtr + aTableData[i] >= src && srcPtr + aTableData[i] < srcEnd);
#endif // FOG_DEBUG
        FOG_ASSERT(stackPtr < stackEnd);

        Accessor::fetchPixel(pix0, srcPtr + aTableData[i]);
        Accessor::storeStack(stackPtr, pix0);

        stackPtr += Accessor::STACK_BPP;
        sum0.add(pix0);
      }

      i = aBorderTailSize;
      if (i != 0)
      {
        Accessor::fetchSolid(pix0, ctx->extendColor);
        if (ctx->extendType == FE_EXTEND_PAD)
          Accessor::fetchPixel(pix0, srcPtr + ctx->srcLastOffset);

        sum0.addN(pix0, i);

        do {
          FOG_ASSERT(stackPtr < stackEnd);

          Accessor::storeStack(stackPtr, pix0);
          stackPtr += Accessor::STACK_BPP;
        } while (--i);
      }

      FOG_ASSERT(stackPtr == stackEnd);
      FOG_ASSERT(runSize != 0);

      stackPtr = stackBuffer;
      srcPtr += ctx->runOffset;

      i = runSize;
      goto _First;

      do {
#if defined(FOG_DEBUG)
        FOG_ASSERT(srcPtr < srcEnd);
#endif // FOG_DEBUG
        FOG_ASSERT(stackPtr < stackEnd);

        Accessor::fetchStack(pix0, stackPtr);
        sum0.sub(pix0);

        Accessor::fetchPixel(pix0, srcPtr);
        Accessor::storeStack(stackPtr, pix0);
        sum0.add(pix0);

        srcPtr += Accessor::PIXEL_BPP;
        stackPtr += Accessor::STACK_BPP;

        if (stackPtr == stackEnd)
          stackPtr = stackBuffer;

_First:
#if defined(FOG_DEBUG)
        FOG_ASSERT(dstPtr < dstEnd);
#endif // FOG_DEBUG

        Accessor::storeMulShr(dstPtr, sum0, sumMul, sumShr);
        dstPtr += Accessor::PIXEL_BPP;
      } while (--i);

      // From here it's not needed to store pixels to the stack, because
      // 'bTableSize + bTableTailSize' shouldn't be larger than kernelSize.

      srcPtr = src;
      for (i = 0; i < bTableSize; i++)
      {
#if defined(FOG_DEBUG)
        FOG_ASSERT(srcPtr + bTableData[i] >= src && srcPtr + bTableData[i] < srcEnd);
#endif // FOG_DEBUG
        FOG_ASSERT(stackPtr < stackEnd);

        Accessor::fetchStack(pix0, stackPtr);
        sum0.sub(pix0);

        Accessor::fetchPixel(pix0, srcPtr + bTableData[i]);
        // Doesn't need to store into stack.
        // Accessor::storeStack(stackPtr, pix0);
        sum0.add(pix0);

        stackPtr += Accessor::STACK_BPP;
        if (stackPtr == stackEnd)
          stackPtr = stackBuffer;

#if defined(FOG_DEBUG)
        FOG_ASSERT(dstPtr < dstEnd);
#endif // FOG_DEBUG

        Accessor::storeMulShr(dstPtr, sum0, sumMul, sumShr);
        dstPtr += Accessor::PIXEL_BPP;
      }

      i = bBorderTailSize;
      if (i != 0)
      {
        typename Accessor::Pixel pixB;
        Accessor::fetchSolid(pixB, ctx->extendColor);

        if (ctx->extendType == FE_EXTEND_PAD)
          Accessor::fetchPixel(pixB, srcPtr + ctx->srcLastOffset);

        typename Accessor::Component compB;
        Accessor::unpack(compB, pixB);

        do {
          FOG_ASSERT(stackPtr < stackEnd);

          Accessor::fetchStack(pix0, stackPtr);
          sum0.sub(pix0);

          // Doesn't need to store into stack.
          // Accessor::storeStack(stackPtr, pixB);
          sum0.add(compB);

          stackPtr += Accessor::STACK_BPP;
          if (stackPtr == stackEnd)
            stackPtr = stackBuffer;

#if defined(FOG_DEBUG)
          FOG_ASSERT(dstPtr < dstEnd);
#endif // FOG_DEBUG

          Accessor::storeMulShr(dstPtr, sum0, sumMul, sumShr);
          dstPtr += Accessor::PIXEL_BPP;
        } while (--i);
      }

      dst += dstStride;
      src += srcStride;
    }
  }

  template<typename Accessor>
  static void FOG_FASTCALL do_rect_box_v(
    RasterConvolve* ctx)
  {
    uint8_t* dst = ctx->dstData;
    uint8_t* src = ctx->srcData;

    ssize_t dstStride = ctx->dstStride;
    ssize_t srcStride = ctx->srcStride;

    uint runWidth = ctx->rowSize;
    uint runSize = ctx->runSize;

    uint32_t sumMul = getReciprocal(ctx->kernelSize);
    uint32_t sumShr = 16;

    ssize_t* aTableData = ctx->aTableData;
    ssize_t* bTableData = ctx->bTableData;
    uint8_t* stackBuffer = ctx->stack;
    
    uint aBorderLeadSize = ctx->aBorderLeadSize;
    uint aBorderTailSize = ctx->aBorderTailSize;
    uint bBorderTailSize = ctx->bBorderTailSize;

    uint aTableSize = ctx->aTableSize;
    uint bTableSize = ctx->bTableSize;

    uint i, r = 0;

    do {
      uint8_t* dstPtr = dst;
      uint8_t* srcPtr = src;
      uint8_t* stackPtr = stackBuffer;

      typename Accessor::Component sum[BLUR_RECT_V_HLINE_COUNT];
      typename Accessor::Pixel pix0;

      uint xLength = Math::min<uint>(runWidth - r, BLUR_RECT_V_HLINE_COUNT);
      uint x;

      uint8_t* stackEnd = stackBuffer + ctx->kernelSize * xLength * Accessor::STACK_BPP;

      i = aBorderLeadSize;
      if (i != 0)
      {
        if (ctx->extendType == FE_EXTEND_COLOR)
        {
          Accessor::fetchSolid(pix0, ctx->extendColor);
          sum[0].reset();
          sum[0].addN(pix0, i);

          for (x = 1; x < xLength; x++)
          {
            sum[x].set(sum[0]);
          }

          i *= xLength;
          do {
            FOG_ASSERT(stackPtr < stackEnd);

            Accessor::storeStack(stackPtr, pix0);
            stackPtr += Accessor::STACK_BPP;
          } while (--i);
        }
        else
        {
          uint8_t* srcBase = srcPtr + ctx->srcFirstOffset;
          uint8_t* stackBase = stackPtr;

          for (x = 0; x < xLength; x++)
          {
            FOG_ASSERT(stackPtr < stackEnd);

            Accessor::fetchPixel(pix0, srcBase + x * Accessor::PIXEL_BPP);
            sum[x].reset();
            sum[x].addN(pix0, i);

            Accessor::storeStack(stackPtr, pix0);
            stackPtr += Accessor::STACK_BPP;
          }

          while (--i)
          {
            for (x = 0; x < xLength; x++)
            {
              FOG_ASSERT(stackPtr < stackEnd);

              Accessor::fetchStack(pix0, stackBase + x * Accessor::STACK_BPP);
              Accessor::storeStack(stackPtr, pix0);
              stackPtr += Accessor::STACK_BPP;
            }
          }
        }
      }
      else
      {
        for (x = 0; x < xLength; x++)
        {
          sum[x].reset();
        }
      }

      for (i = 0; i < aTableSize; i++)
      {
        uint8_t* srcTab = srcPtr + aTableData[i];

        for (x = 0; x < xLength; x++)
        {
          FOG_ASSERT(stackPtr < stackEnd);

          Accessor::fetchPixel(pix0, srcTab + x * Accessor::PIXEL_BPP);
          Accessor::storeStack(stackPtr, pix0);

          stackPtr += Accessor::STACK_BPP;
          sum[x].add(pix0);
        }
      }

      i = aBorderTailSize;
      if (i != 0)
      {
        if (ctx->extendType == FE_EXTEND_COLOR)
        {
          Accessor::fetchSolid(pix0, ctx->extendColor);

          for (x = 0; x < xLength; x++)
          {
            sum[x].addN(pix0, i);
          }

          i *= xLength;
          do {
            FOG_ASSERT(stackPtr < stackEnd);

            Accessor::storeStack(stackPtr, pix0);
            stackPtr += Accessor::STACK_BPP;
          } while (--i);
        }
        else
        {
          uint8_t* srcBase = srcPtr + ctx->srcLastOffset;
          uint8_t* stackBase = stackPtr;

          for (x = 0; x < xLength; x++)
          {
            FOG_ASSERT(stackPtr < stackEnd);

            Accessor::fetchPixel(pix0, srcBase + x * Accessor::PIXEL_BPP);
            sum[x].addN(pix0, i);

            Accessor::storeStack(stackPtr, pix0);
            stackPtr += Accessor::STACK_BPP;
          }

          while (--i)
          {
            for (x = 0; x < xLength; x++)
            {
              FOG_ASSERT(stackPtr < stackEnd);

              Accessor::fetchStack(pix0, stackBase + x * Accessor::STACK_BPP);
              Accessor::storeStack(stackPtr, pix0);
              stackPtr += Accessor::STACK_BPP;
            }
          }
        }
      }

      FOG_ASSERT(stackPtr == stackEnd);
      FOG_ASSERT(runSize != 0);

      stackPtr = stackBuffer;
      srcPtr += ctx->runOffset;

      for (x = 0; x < xLength; x++)
      {
        Accessor::storeMulShr(dstPtr + x * Accessor::PIXEL_BPP, sum[x], sumMul, sumShr);
      }

      dstPtr += dstStride;
      i = runSize;

      while (--i)
      {
        for (x = 0; x < xLength; x++)
        {
          FOG_ASSERT(stackPtr < stackEnd);

          Accessor::fetchStack(pix0, stackPtr);
          sum[x].sub(pix0);

          Accessor::fetchPixel(pix0, srcPtr + x * Accessor::PIXEL_BPP);
          Accessor::storeStack(stackPtr, pix0);
          sum[x].add(pix0);

          stackPtr += Accessor::STACK_BPP;

          Accessor::storeMulShr(dstPtr + x * Accessor::PIXEL_BPP, sum[x], sumMul, sumShr);
        }

        if (stackPtr == stackEnd)
          stackPtr = stackBuffer;

        srcPtr += srcStride;
        dstPtr += dstStride;
      }
      
      // From here it's not needed to store pixels to the stack, because
      // 'bTableSize + bTableTailSize' shouldn't be larger than kernelSize.

      srcPtr = src;
      for (i = 0; i < bTableSize; i++)
      {
        uint8_t* srcBase = srcPtr + bTableData[i];

        for (x = 0; x < xLength; x++)
        {
          FOG_ASSERT(stackPtr < stackEnd);

          Accessor::fetchStack(pix0, stackPtr);
          sum[x].sub(pix0);

          Accessor::fetchPixel(pix0, srcBase + x * Accessor::PIXEL_BPP);
          // Doesn't need to store into stack.
          // Accessor::storeStack(stackPtr, pix0);
          sum[x].add(pix0);

          stackPtr += Accessor::STACK_BPP;
            
          Accessor::storeMulShr(dstPtr + x * Accessor::PIXEL_BPP, sum[x], sumMul, sumShr);
        }

        if (stackPtr == stackEnd)
          stackPtr = stackBuffer;

        dstPtr += dstStride;
      }
      
      i = bBorderTailSize;
      if (i != 0)
      {
        if (ctx->extendType == FE_EXTEND_COLOR)
        {
          typename Accessor::Pixel pixB;
          Accessor::fetchSolid(pixB, ctx->extendColor);

          typename Accessor::Component compB;
          Accessor::unpack(compB, pixB);

          do {
            for (x = 0; x < xLength; x++)
            {
              FOG_ASSERT(stackPtr < stackEnd);

              Accessor::fetchStack(pix0, stackPtr);
              sum[x].sub(pix0);

              // Doesn't need to store into stack.
              // Accessor::storeStack(stackPtr, pixB);
              sum[x].add(compB);

              stackPtr += Accessor::STACK_BPP;
              Accessor::storeMulShr(dstPtr + x * Accessor::PIXEL_BPP, sum[x], sumMul, sumShr);
            }

            if (stackPtr == stackEnd)
              stackPtr = stackBuffer;

            dstPtr += dstStride;
          } while (--i);
        }
        else
        {
          uint8_t* srcBase = srcPtr + ctx->srcLastOffset;
          
          do {
            for (x = 0; x < xLength; x++)
            {
              FOG_ASSERT(stackPtr < stackEnd);

              Accessor::fetchStack(pix0, stackPtr);
              sum[x].sub(pix0);

              Accessor::fetchPixel(pix0, srcBase + x * Accessor::PIXEL_BPP);
              // Doesn't need to store into stack.
              // Accessor::storeStack(stackPtr, pix0);
              sum[x].add(pix0);

              stackPtr += Accessor::STACK_BPP;
              Accessor::storeMulShr(dstPtr + x * Accessor::PIXEL_BPP, sum[x], sumMul, sumShr);
            }

            if (stackPtr == stackEnd)
              stackPtr = stackBuffer;

            dstPtr += dstStride;
          } while (--i);
        }
      }

      dst += xLength * Accessor::PIXEL_BPP;
      src += xLength * Accessor::PIXEL_BPP;

      r += xLength;
    } while (r < runWidth);
  }

  // ==========================================================================
  // [Blur - DoRect (Exp)]
  // ==========================================================================
};

} // Render namespace
} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTEROPS_C_FILTERBLUR_P_H
