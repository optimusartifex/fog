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
#include <Fog/Core/OS/OSUtil.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/ImageFormatDescription.h>
#include <Fog/G2d/OS/OSUtil.h>
#include <Fog/G2d/Painting/RasterApi_p.h>

namespace Fog {

// ============================================================================
// [Fog::MacCGImageData]
// ============================================================================

struct FOG_NO_EXPORT MacCGImageData : public ImageData
{
  CGImageRef cgImage;
};

// ============================================================================
// [Fog::Image - MacCG]
// ============================================================================

static err_t FOG_CDECL Image_MacCG_create(ImageData** pd, const SizeI* size, uint32_t format);
static void  FOG_CDECL Image_MacCG_destroy(ImageData* d);
static void* FOG_CDECL Image_MacCG_getHandle(const ImageData* d);
static err_t FOG_CDECL Image_MacCG_updatePalette(ImageData* d, const Range* range);

static const void *Image_MacCG_getBytePointerCallback(void *info);
static void Image_MacCG_releaseInfoCallback(void *info);

static const ImageVTable Image_MacCG_vTable =
{
  Image_MacCG_create,
  Image_MacCG_destroy,
  Image_MacCG_getHandle,
  Image_MacCG_updatePalette
};

static const CGDataProviderDirectCallbacks Image_MacCG_cgDataProviderCallbacks =
{
	0,
  Image_MacCG_getBytePointerCallback,
	NULL,
	NULL,
	Image_MacCG_releaseInfoCallback
};

static err_t FOG_CDECL Image_MacCG_create(ImageData** pd, const SizeI* size, uint32_t format)
{
  const ImageFormatDescription& desc = ImageFormatDescription::getByFormat(format);
  ssize_t stride = (size_t)fog_api.image_getStrideFromWidth(size->w, desc.getDepth());

  if (stride == 0)
    return ERR_RT_INVALID_ARGUMENT;

  if ((uint)size->h > (SIZE_MAX - sizeof(ImageData) - 16) / stride)
    return ERR_RT_OUT_OF_MEMORY;

  uint32_t cgBPC;
  uint32_t cgBPP;
  CGBitmapInfo cgFmt;

  // NOTE: Mac doesn't contain support for 24-bit pixel format.
  switch (format)
  {
    case IMAGE_FORMAT_PRGB32:
      cgBPC = 8;
      cgBPP = 32;
      cgFmt = kCGBitmapByteOrder32Host | kCGImageAlphaPremultipliedFirst;
      break;
      
    case IMAGE_FORMAT_XRGB32:
      cgBPC = 8;
      cgBPP = 32;
      cgFmt = kCGBitmapByteOrder32Host | kCGImageAlphaNoneSkipFirst;
      break;

    case IMAGE_FORMAT_A8:
      cgBPC = 8;
      cgBPP = 8;
      cgFmt = kCGImageAlphaOnly;
      break;

    default:
      return ERR_IMAGE_INVALID_FORMAT;
  }

  size_t dSize = sizeof(MacCGImageData) + 16 + (size_t)stride * (uint)size->h;
  MacCGImageData* d = reinterpret_cast<MacCGImageData*>(MemMgr::alloc(dSize));

  if (FOG_IS_NULL(d))
    return ERR_RT_OUT_OF_MEMORY;

  d->reference.init(1);
  d->vType = VAR_TYPE_IMAGE | VAR_FLAG_NONE;
  d->locked = 0;

  d->vtable = &Image_MacCG_vTable;
  d->size = *size;
  d->format = format;
  d->type = IMAGE_TYPE_MAC_CG;
  d->adopted = 0;
  d->colorKey = IMAGE_COLOR_KEY_NONE;
  d->bytesPerPixel = desc.getBytesPerPixel();
  FOG_PADDING_ZERO_64(d->padding);

  d->data = (uint8_t*)( ((size_t)d + sizeof(ImageData) + 15) & ~(size_t)15 );
  d->first = d->data;
  d->stride = stride;

  CGColorSpaceRef cgColorSpace = CGColorSpaceCreateDeviceRGB();
  if (cgColorSpace == NULL)
  {
    MemMgr::free(d);
    return ERR_RT_OUT_OF_MEMORY;
  }
  
  CGDataProviderRef cgDataProvider = CGDataProviderCreateDirect(d, d->stride * d->size.h, &Image_MacCG_cgDataProviderCallbacks);
  if (cgDataProvider == NULL)
  {
    CGColorSpaceRelease(cgColorSpace);

    MemMgr::free(d);
    return ERR_RT_OUT_OF_MEMORY;
  }

  CGImageRef cgImage = CGImageCreate(
    size->w, size->h,                              // Size. 
    cgBPC, cgBPP, stride,                          // BPC, BPP, stride.
    cgColorSpace,                                  // Colorspace.
    cgFmt,                                         // Image format.
    cgDataProvider,                                // DataProvider - 'd'.
    NULL,                                          // Don't remap colors.
    true,                                          // Interpolate.
    kCGRenderingIntentDefault);                    // Default out-of-gamut handling.

  d->palette.init();
  d->cgImage = cgImage;

  CGColorSpaceRelease(cgColorSpace);
  CGDataProviderRelease(cgDataProvider);

  // In case that CGImageCreate failed, the 'd' pointer was already released,
  // because we provided releaseInfoCallback() to CGDataProvider, which was
  // released by CGDataProviderRelease() method.
  if (cgImage == NULL)
    return ERR_RT_OUT_OF_MEMORY;

  *pd = d;
  return ERR_OK;
}

// This function is called two times: The first time it's caled by Fog-Framework
// to release the Fog::ImageData() after the reference count was decreased to
// zero. In such case we clear cgImage from 'd' structure and we release it. 
// Second time it's called by CGDataProviderReleaseInfoCallback, because Mac
// released the CGDataProvider we created throuch Image_MacCG_create() method.
//
// The reason why it's so complicated is that the ImageCGRef created by Fog can
// be used by Mac after the last reference to Fog::ImageData was released. This
// means that when running on Mac, you can create CGImageRef using Fog::Image
// use it after the Fog::Image was released.
static void FOG_CDECL Image_MacCG_destroy(ImageData* _d)
{
  MacCGImageData* d = reinterpret_cast<MacCGImageData*>(_d);
  CGImageRef cgImage = d->cgImage;

  if (cgImage == NULL)
  {
    d->palette.destroy();
    MemMgr::free(d);
  }
  else
  {
    // Set d->cgImage to NULL so we know that the CGImageRef was released. In
    // such case the Image_MacGC_destroy() should be called second time by
    // Image_MacCG_releaseInfoCallback(). In such case we do the cleanup.
    d->cgImage = NULL;
    CGImageRelease(cgImage);
  }
}

static void* FOG_CDECL Image_MacCG_getHandle(const ImageData* _d)
{
  const MacCGImageData* d = reinterpret_cast<const MacCGImageData*>(_d);
  return (void*)d->cgImage;
}

static err_t FOG_CDECL Image_MacCG_updatePalette(ImageData* _d, const Range* range)
{
  // There is no indexed palette support on Mac. We safely ignore any call to
  // MacCGImageData::updatePalette().
  return ERR_OK;
}

static const void *Image_MacCG_getBytePointerCallback(void *info)
{
  MacCGImageData* d = reinterpret_cast<MacCGImageData*>(info);
  return d->data;
}

static void Image_MacCG_releaseInfoCallback(void *info)
{
  MacCGImageData* d = reinterpret_cast<MacCGImageData*>(info);
  d->release();
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Image_init_mac(void)
{
  // --------------------------------------------------------------------------
  // [VTable]
  // --------------------------------------------------------------------------

  fog_api.image_vTable[IMAGE_TYPE_MAC_CG] = &Image_MacCG_vTable;

  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

}

} // Fog namespace
