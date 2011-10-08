// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/G2d/Imaging/ImageConverter.h>
#include <Fog/G2d/Painting/RasterApi_p.h>
#include <Fog/G2d/Painting/RasterConstants_p.h>

namespace Fog {

// ============================================================================
// [Fog::ImageConverter - Globals]
// ============================================================================

static Static<ImageConverterData> ImageConverter_dNull;
static Static<ImageConverter> ImageConverter_oNull;

// ============================================================================
// [Fog::ImageConverter - Construction / Destruction]
// ============================================================================

static void FOG_CDECL ImageConverter_ctor(ImageConverter* self)
{
  self->_d = ImageConverter_dNull->addRef();
}

static void FOG_CDECL ImageConverter_ctorCopy(ImageConverter* self, const ImageConverter* other)
{
  self->_d = other->_d->addRef();
}

static void FOG_CDECL ImageConverter_ctorCreate(ImageConverter* self,
  const ImageFormatDescription* dstFormatDescription,
  const ImageFormatDescription* srcFormatDescription,
  uint32_t dither,
  const ImagePalette* dstPalette,
  const ImagePalette* srcPalette)
{
  self->_d = NULL;
  _api.imageconverter_create(self, dstFormatDescription, srcFormatDescription, dither, dstPalette, srcPalette);
}

static void FOG_CDECL ImageConverter_dtor(ImageConverter* self)
{
  self->_d->release();
}

// ============================================================================
// [Fog::ImageConverter - Create]
// ============================================================================

static err_t FOG_CDECL ImageConverter_create(ImageConverter* self,
  const ImageFormatDescription* dstFormatDescription,
  const ImageFormatDescription* srcFormatDescription,
  uint32_t dither,
  const ImagePalette* dstPalette,
  const ImagePalette* srcPalette)
{
  ImageConverterData* d = self->_d;
  err_t err = ERR_OK;

  const ImageFormatDescription* df = dstFormatDescription;
  const ImageFormatDescription* sf = srcFormatDescription;

  if (!df->isValid() || !sf->isValid())
  {
    err = ERR_RT_INVALID_ARGUMENT;
    goto _Fail;
  }

  if ((df->isIndexed() && dstPalette == NULL) || (sf->isIndexed() && srcPalette == NULL))
  {
    err = ERR_RT_INVALID_ARGUMENT;
    goto _Fail;
  }

  if (d == NULL || !d->reference.deref())
  {
    d = _api.imageconverter_dCreate();
    self->_d = d;

    if (FOG_IS_NULL(d))
    {
      err = ERR_RT_OUT_OF_MEMORY;
      goto _Fail;
    }
  }
  else
  {
    // Destroy the associated objects.
    if (d->dstPalette->_d)
    {
      d->dstPalette.destroy();
      d->dstPalette->_d = NULL;
    }

    if (d->srcPalette->_d)
    {
      d->srcPalette.destroy();
      d->srcPalette->_d = NULL;
    }

    d->reference.init(1);
  }

  d->blitFn = NULL;

  d->dstFormatDescription = *df;
  d->srcFormatDescription = *sf;

  d->isDithered = dither;
  d->isCopy = 0;
  d->isBSwap = 0;

  if (df->getDepth() == sf->getDepth())
  {
    uint32_t depth = df->getDepth();
    uint32_t cantDither = 0;

    // Turn off dithering if the destination format has greater or equal bits
    // per component.
    if ((df->getASize() == 0) ||
        (df->getASize() >= sf->getASize()))
    {
      cantDither++;
    }

    if ((df->getRSize() == 0) ||
        (df->getRSize() >= sf->getRSize() &&
         df->getGSize() >= sf->getGSize() &&
         df->getBSize() >= sf->getBSize()))
    {
      cantDither++;
    }

    if (cantDither == 2)
      d->isDithered = 0;

    if (df->isPremultiplied() == sf->isPremultiplied())
    {
      if (df->getAMask64() == sf->getAMask64() && df->getRMask64() == sf->getRMask64() &&
          df->getGMask64() == sf->getGMask64() && df->getBMask64() == sf->getBMask64())
      {
        // Formats are similar so fill the isCopy and isBSwap flags.
        bool isCopy = df->isByteSwapped() == sf->isByteSwapped();

        d->isCopy = isCopy;
        d->isBSwap = !isCopy;
      }
      // Special case, 24-bit or 32-bit BSwap.
      else if (df->hasAlignedComponents() && sf->hasAlignedComponents())
      {
        if ((depth == 24 && (df->getRMask32() == Memory::bswap32(sf->getRMask32()) >> 8) &&
                            (df->getGMask32() == Memory::bswap32(sf->getGMask32()) >> 8) &&
                            (df->getBMask32() == Memory::bswap32(sf->getBMask32()) >> 8)) ||
            (depth == 32 && (df->getAMask32() == Memory::bswap32(sf->getAMask32())     ) &&
                            (df->getRMask32() == Memory::bswap32(sf->getRMask32())     ) &&
                            (df->getGMask32() == Memory::bswap32(sf->getGMask32())     ) &&
                            (df->getBMask32() == Memory::bswap32(sf->getBMask32())     )) )
        {
          d->isBSwap = true;
        }
      }
    }
  }

  // Should be already NULL.
  FOG_ASSERT(d->dstPalette->_d == NULL);
  FOG_ASSERT(d->srcPalette->_d == NULL);

  // Use the direct converter if available.
  if (df->getFormat() < IMAGE_FORMAT_COUNT && !df->isIndexed() &&
      sf->getFormat() < IMAGE_FORMAT_COUNT && !dither)
  {
    d->blitFn = (ImageConverterBlitLineFunc)_api_raster.getCopyFullFunc(
      df->getFormat(), sf->getFormat());

    if (d->blitFn != NULL)
      return ERR_OK;
  }

  // Use the Fog::Raster to create the converter for us.
  err = _api_raster.initConverter(d);
  if (err == ERR_OK)
    return ERR_OK;

_Fail:
  if (d != NULL && d->reference.deref())
    _api.imageconverter_dFree(d);

  self->_d = ImageConverter_dNull->addRef();
  return err;
}

static err_t FOG_CDECL ImageConverter_createDithered8(ImageConverter* self,
  const ImageDither8Params* dstParams,
  const ImageFormatDescription* srcFormatDescription,
  const ImagePalette* dstPalette,
  const ImagePalette* srcPalette)
{
  // TODO: Not implemented.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::ImageConverter - Reset]
// ============================================================================

static void FOG_CDECL ImageConverter_reset(ImageConverter* self)
{
  atomicPtrXchg(&self->_d, ImageConverter_dNull->addRef())->release();
}

// ============================================================================
// [Fog::ImageConverter - Blit]
// ============================================================================

static void FOG_CDECL ImageConverter_blitLine(const ImageConverter* self,
  void* dst, const void* src,
  int w, const PointI* ditherOrigin)
{
  ImageConverterData* d = self->_d;
  if (d == &ImageConverter_dNull)
    return;

  ImageConverterClosure closure;

  if (ditherOrigin == NULL)
    closure.ditherOrigin.reset();
  else
    closure.ditherOrigin.set(*ditherOrigin);

  closure.palette = d->srcPalette->_d;
  closure.data = d;

  d->blitFn(reinterpret_cast<uint8_t*>(dst), reinterpret_cast<const uint8_t*>(src), w, &closure);
}

static void FOG_CDECL ImageConverter_blitRect(const ImageConverter* self,
  void* dst, size_t dstStride,
  const void* src, size_t srcStride,
  int w, int h, const PointI* ditherOrigin)
{
  ImageConverterData* d = self->_d;
  if (d == &ImageConverter_dNull)
    return;

  ImageConverterClosure closure;

  if (ditherOrigin == NULL)
    closure.ditherOrigin.reset();
  else
    closure.ditherOrigin.set(*ditherOrigin);

  closure.palette = d->srcPalette->_d;
  closure.data = d;

  uint8_t* dPtr = reinterpret_cast<uint8_t*>(dst);
  const uint8_t* sPtr = reinterpret_cast<const uint8_t*>(src);

  ImageConverterBlitLineFunc blitLine = d->blitFn;

  for (int y = 0; y < h; y++)
  {
    closure.ditherOrigin.y = y;
    blitLine(dPtr, sPtr, w, &closure);

    dPtr += dstStride;
    sPtr += srcStride;
  }
}

// ============================================================================
// [Fog::ImageConverter - ImageConverterData]
// ============================================================================

static ImageConverterData* FOG_CDECL ImageConverter_dCreate(void)
{
  ImageConverterData* d = reinterpret_cast<ImageConverterData*>(MemMgr::alloc(sizeof(ImageConverterData)));

  if (FOG_IS_NULL(d))
    return NULL;

  d->reference.init(1);
  d->blitFn = NULL;

  d->dstPalette->_d = NULL;
  d->srcPalette->_d = NULL;

  return d;
}

static void FOG_CDECL ImageConverter_dFree(ImageConverterData* d)
{
  if (d->dstPalette->_d)
    d->dstPalette.destroy();

  if (d->srcPalette->_d)
    d->srcPalette.destroy();

  MemMgr::free(d);
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void ImageConverter_init(void)
{
  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  _api.imageconverter_ctor = ImageConverter_ctor;
  _api.imageconverter_ctorCopy = ImageConverter_ctorCopy;
  _api.imageconverter_ctorCreate = ImageConverter_ctorCreate;
  _api.imageconverter_dtor = ImageConverter_dtor;

  _api.imageconverter_create = ImageConverter_create;
  _api.imageconverter_createDithered8 = ImageConverter_createDithered8;
  _api.imageconverter_reset = ImageConverter_reset;

  _api.imageconverter_blitLine = ImageConverter_blitLine;
  _api.imageconverter_blitRect = ImageConverter_blitRect;

  _api.imageconverter_dCreate = ImageConverter_dCreate;
  _api.imageconverter_dFree = ImageConverter_dFree;

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  ImageConverterData* d = &ImageConverter_dNull;

  d->reference.init(1);
  d->blitFn = NULL;
  d->dstFormatDescription.reset();
  d->srcFormatDescription.reset();
  d->dstPalette->_d = NULL;
  d->srcPalette->_d = NULL;

  _api.imageconverter_oNull = ImageConverter_oNull.initCustom1(d);
}

} // Fog namespace
