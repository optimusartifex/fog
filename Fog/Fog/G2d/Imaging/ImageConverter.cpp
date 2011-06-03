// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Memory/Memory.h>
#include <Fog/G2d/Imaging/ImageConverter.h>
#include <Fog/G2d/Render/RenderApi_p.h>
#include <Fog/G2d/Render/RenderConstants_p.h>

namespace Fog {

// ============================================================================
// [Fog::ImageConverter - Helpers]
// ============================================================================

static ImageConverterData* ImageConverter_dalloc()
{
  ImageConverterData* d = reinterpret_cast<ImageConverterData*>(Memory::alloc(sizeof(ImageConverterData)));
  if (FOG_UNLIKELY(d == NULL)) return NULL;

  d->refCount.init(1);
  d->dstPalette->_d = NULL;
  d->srcPalette->_d = NULL;

  return d;
}

static void ImageConverter_dfree(ImageConverterData* d)
{
  if (d->dstPalette->_d) d->dstPalette.destroy();
  if (d->srcPalette->_d) d->srcPalette.destroy();

  Memory::free(d);
}

// ============================================================================
// [Fog::ImageConverter - Construction / Destruction]
// ============================================================================

ImageConverter::ImageConverter() :
  _d(_dnull)
{
  _d->refCount.inc();
}

ImageConverter::ImageConverter(const ImageConverter& other) :
  _d(other._d)
{
  _d->refCount.inc();
}

ImageConverter::ImageConverter(
  const ImageFormatDescription& dstFormatDescription,
  const ImageFormatDescription& srcFormatDescription,
  uint32_t dither,
  const ImagePalette* dstPalette,
  const ImagePalette* srcPalette)
{
  _d = NULL;
  create(dstFormatDescription, srcFormatDescription, dither, dstPalette, srcPalette);
}

ImageConverter::~ImageConverter()
{
  if (_d->refCount.deref()) ImageConverter_dfree(_d);
}

// ============================================================================
// [Fog::ImageConverter - Create]
// ============================================================================

err_t ImageConverter::create(
  const ImageFormatDescription& dstFormatDescription,
  const ImageFormatDescription& srcFormatDescription,
  uint32_t dither,
  const ImagePalette* dstPalette,
  const ImagePalette* srcPalette)
{
  err_t err = ERR_OK;

  const ImageFormatDescription& df = dstFormatDescription;
  const ImageFormatDescription& sf = srcFormatDescription;

  if (!df.isValid() || !sf.isValid())
  {
    err = ERR_RT_INVALID_ARGUMENT;
    goto _Fail;
  }

  if ((df.isIndexed() && dstPalette == NULL) || (sf.isIndexed() && srcPalette == NULL))
  {
    err = ERR_RT_INVALID_ARGUMENT;
    goto _Fail;
  }

  if (_d == NULL || !_d->refCount.deref())
  {
    _d = ImageConverter_dalloc();
    if (FOG_UNLIKELY(_d == NULL))
    {
      err = ERR_RT_OUT_OF_MEMORY;
      goto _Fail;
    }
  }
  else
  {
    // Destroy the associated objects.
    if (_d->dstPalette->_d) { _d->dstPalette.destroy(); _d->dstPalette->_d = NULL; }
    if (_d->srcPalette->_d) { _d->srcPalette.destroy(); _d->srcPalette->_d = NULL; }

    _d->refCount.init(1);
  }

  _d->blitFn = NULL;

  _d->dstFormatDescription = df;
  _d->srcFormatDescription = sf;

  _d->isDithered = dither;
  _d->isCopy = 0;
  _d->isBSwap = 0;

  if (df.getDepth() == sf.getDepth())
  {
    uint32_t depth = df.getDepth();
    uint32_t cantDither = 0;

    // Turn off dithering if the destination format has greater or equal bits
    // per component.
    if ((df.getASize() == 0) ||
        (df.getASize() >= sf.getASize())) cantDither++;
    if ((df.getRSize() == 0) ||
        (df.getRSize() >= sf.getRSize() &&
         df.getGSize() >= sf.getGSize() &&
         df.getBSize() >= sf.getBSize())) cantDither++;
    if (cantDither == 2) _d->isDithered = 0;

    if (df.isPremultiplied() == sf.isPremultiplied())
    {
      if (df.getAMask64() == sf.getAMask64() && df.getRMask64() == sf.getRMask64() &&
          df.getGMask64() == sf.getGMask64() && df.getBMask64() == sf.getBMask64())
      {
        // Formats are similar so fill the isCopy and isBSwap flags.
        bool isCopy = df.isByteSwapped() == sf.isByteSwapped();

        _d->isCopy = isCopy;
        _d->isBSwap = !isCopy;
      }
      // Special case, 24-bit or 32-bit BSwap.
      else if (df.hasAlignedComponents() && sf.hasAlignedComponents())
      {
        if ((depth == 24 && (df.getRMask32() == Memory::bswap32(sf.getRMask32()) >> 8) &&
                            (df.getGMask32() == Memory::bswap32(sf.getGMask32()) >> 8) &&
                            (df.getBMask32() == Memory::bswap32(sf.getBMask32()) >> 8)) ||
            (depth == 32 && (df.getAMask32() == Memory::bswap32(sf.getAMask32())     ) &&
                            (df.getRMask32() == Memory::bswap32(sf.getRMask32())     ) &&
                            (df.getGMask32() == Memory::bswap32(sf.getGMask32())     ) &&
                            (df.getBMask32() == Memory::bswap32(sf.getBMask32())     )) )
        {
          _d->isBSwap = true;
        }
      }
    }
  }

  // Should be already NULL.
  FOG_ASSERT(_d->dstPalette->_d == NULL);
  FOG_ASSERT(_d->srcPalette->_d == NULL);

  // Use the direct converter if available.
  if (df.getFormat() < IMAGE_FORMAT_COUNT && !df.isIndexed() &&
      sf.getFormat() < IMAGE_FORMAT_COUNT && !dither)
  {
    _d->blitFn = (ImageConverterBlitLineFn)_g2d_render.getCopyFullFn(
      df.getFormat(), sf.getFormat());
    if (_d->blitFn != NULL) return ERR_OK;
  }

  // Use the Fog::Render to create the converter for us.
  err = _g2d_render.initConverter(_d);
  if (err == ERR_OK) return ERR_OK;

_Fail:
  if (_d != NULL && _d->refCount.deref()) ImageConverter_dfree(_d);

  _d = _dnull;
  _d->refCount.inc();

  return err;
}

err_t ImageConverter::createDithered8(
  const ImageDither8Params& dstParams,
  const ImageFormatDescription& srcFormatDescription,
  const ImagePalette* dstPalette,
  const ImagePalette* srcPalette)
{
  // TODO: Not implemented.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::ImageConverter - Reset]
// ============================================================================

void ImageConverter::reset()
{
  _dnull->refCount.inc();
  ImageConverterData* old = atomicPtrXchg(&_d, _dnull.instancep());
  if (old->refCount.deref()) ImageConverter_dfree(old);
}

// ============================================================================
// [Fog::ImageConverter - Blit Span]
// ============================================================================

void ImageConverter::blitSpan(void* dst, const void* src, int w)
{
  FOG_ASSERT(isValid());

  ImageConverterClosure closure;
  setupClosure(&closure);

  _d->blitFn(reinterpret_cast<uint8_t*>(dst), reinterpret_cast<const uint8_t*>(src), w, &closure);
}

void ImageConverter::blitSpan(void* dst, const void* src, int w, const PointI& ditherOrigin)
{
  FOG_ASSERT(isValid());

  ImageConverterClosure closure;
  setupClosure(&closure, ditherOrigin);

    _d->blitFn(reinterpret_cast<uint8_t*>(dst), reinterpret_cast<const uint8_t*>(src), w, &closure);
}

// ============================================================================
// [Fog::ImageConverter - Blit Rect]
// ============================================================================

void ImageConverter::blitRect(void* dst, size_t dstStride, const void* src, size_t srcStride, int w, int h)
{
  FOG_ASSERT(isValid());

  ImageConverterClosure closure;
  setupClosure(&closure);

  uint8_t* dstCur = reinterpret_cast<uint8_t*>(dst);
  const uint8_t* srcCur = reinterpret_cast<const uint8_t*>(src);

  for (int y = 0; y < h; y++, dstCur += dstStride, srcCur += srcStride)
  {
    closure.ditherOrigin.y = y;
    _d->blitFn(dstCur, srcCur, w, &closure);
  }
}

void ImageConverter::blitRect(void* dst, size_t dstStride, const void* src, size_t srcStride, int w, int h, const PointI& ditherOrigin)
{
  FOG_ASSERT(isValid());

  ImageConverterClosure closure;
  setupClosure(&closure, ditherOrigin);

  uint8_t* dstCur = reinterpret_cast<uint8_t*>(dst);
  const uint8_t* srcCur = reinterpret_cast<const uint8_t*>(src);

  for (int y = 0; y < h; y++, dstCur += dstStride, srcCur += srcStride)
  {
    _d->blitFn(dstCur, srcCur, w, &closure);
    closure.ditherOrigin.y++;
  }
}

// ============================================================================
// [Fog::ImageConverter - Statics]
// ============================================================================

Static<ImageConverterData> ImageConverter::_dnull;

// ============================================================================
// [Fog::G2d - Library Initializers]
// ============================================================================

FOG_NO_EXPORT void _g2d_imageconverter_init(void)
{
  ImageConverterData* d = ImageConverter::_dnull.instancep();

  d->refCount.init(1);
  d->blitFn = NULL;
  d->dstFormatDescription.reset();
  d->srcFormatDescription.reset();
  d->dstPalette->_d = NULL;
  d->srcPalette->_d = NULL;
}

FOG_NO_EXPORT void _g2d_imageconverter_fini(void)
{
  ImageConverterData* d = ImageConverter::_dnull.instancep();

  d->refCount.dec();
}

} // Fog namespace
