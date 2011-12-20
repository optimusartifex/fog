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
#include <Fog/G2d/Imaging/ImageConverter.h>
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
// [Fog::Image - FromCGImage / ToCGImage]
// ============================================================================

static err_t FOG_CDECL Image_toCGImage(const Image* self, CGImageRef* cgImage)
{
  if (cgImage == NULL)
    return ERR_RT_INVALID_ARGUMENT;

  ImageData* d = self->_d;
  *cgImage = NULL;

  if (d->size.w == 0 || d->size.h == 0)
    return ERR_RT_INVALID_ARGUMENT;
  
  SizeI size = d->size;

  uint32_t cgBPC;
  uint32_t cgBPP;
  uint32_t cgFmt;
  
  uint64_t cgAMask = 0;
  uint64_t cgRMask = 0;
  uint64_t cgGMask = 0;
  uint64_t cgBMask = 0;

  ssize_t cgStride = 0;

  switch (d->format)
  {
    // iOS doesn't support 16-bit BPC images.

    case IMAGE_FORMAT_PRGB32:
#if defined(FOG_OS_IOS)
    case IMAGE_FORMAT_PRGB64:
#endif // FOG_OS_IOS
      cgBPC = 8;
      cgBPP = 32;
      cgFmt = kCGBitmapByteOrder32Host | kCGImageAlphaPremultipliedFirst;

      cgAMask = 0xFF000000;
      cgRMask = 0x00FF0000;
      cgGMask = 0x0000FF00;
      cgBMask = 0x000000FF;

      cgStride = size.w * 4;
      break;

    case IMAGE_FORMAT_XRGB32:
    case IMAGE_FORMAT_RGB24:
#if defined(FOG_OS_IOS)
    case IMAGE_FORMAT_RGB48:
#endif // FOG_OS_IOS
    case IMAGE_FORMAT_I8:
      cgBPC = 8;
      cgBPP = 32;
      cgFmt = kCGBitmapByteOrder32Host | kCGImageAlphaNoneSkipFirst;
      
      cgAMask = 0x00000000;
      cgRMask = 0x00FF0000;
      cgGMask = 0x0000FF00;
      cgBMask = 0x000000FF;
      
      cgStride = size.w * 4;
      break;

    case IMAGE_FORMAT_A8:
    case IMAGE_FORMAT_A16:
      cgBPC = 8;
      cgBPP = 8;
      cgFmt = kCGImageAlphaOnly;
      
      cgAMask = 0x000000FF;
      cgRMask = 0x00000000;
      cgGMask = 0x00000000;
      cgBMask = 0x00000000;
      
      cgStride = (size.w + 3) & ~3;
      break;

#if !defined(FOG_OS_IOS)
    case IMAGE_FORMAT_PRGB64:
      cgBPC = 16;
      cgBPP = 64;
      cgFormat = kCGBitmapByteOrderDefault | kCGImageAlphaPremultipliedLast;

      cgAMask = FOG_UINT64_C(0x000000000000FFFF);
      cgRMask = FOG_UINT64_C(0xFFFF000000000000);
      cgGMask = FOG_UINT64_C(0x0000FFFF00000000);
      cgBMask = FOG_UINT64_C(0x00000000FFFF0000);
      
      cgStride = size.w * 8;
      break;

    case IMAGE_FORMAT_RGB48:
      cgBPC = 16;
      cgBPP = 64;
      cgFormat = kCGBitmapByteOrderDefault | kCGImageAlphaNoneSkipLast;
      
      cgAMask = FOG_UINT64_C(0x0000000000000000);
      cgRMask = FOG_UINT64_C(0xFFFF000000000000);
      cgGMask = FOG_UINT64_C(0x0000FFFF00000000);
      cgBMask = FOG_UINT64_C(0x00000000FFFF0000);

      cgStride = size.w * 8;
      break;
#endif // FOG_OS_IOS

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  ImageConverter converter;

  FOG_RETURN_ON_ERROR(converter.create(
    // Destination ImageFormatDescription.
    ImageFormatDescription::fromArgb(
      cgBPP,
      IMAGE_FD_IS_PREMULTIPLIED | IMAGE_FD_FILL_UNUSED_BITS,
      cgAMask, cgRMask, cgGMask, cgBMask
    ),
    // Source ImageFormatDescription (provided natively by Fog-Framework).
    ImageFormatDescription::getByFormat(d->format),
    false,       // Use Dithering?
    NULL,        // Destination Palette.
    &d->palette  // Source Palette (only used if source is IMAGE_FORMAT_I8).
  ));

  CGColorSpaceRef cgColorSpace = CGColorSpaceCreateDeviceRGB();
  if (cgColorSpace == NULL)
  {
    *cgImage = NULL;
    return ERR_RT_OUT_OF_MEMORY;
  }

  CFMutableDataRef cfDataRef = CFDataCreateMutable(NULL, CFIndex(cgStride * size.h));
  if (cfDataRef == NULL)
  {
    CFRelease(cgColorSpace);
    return ERR_RT_OUT_OF_MEMORY;
  }

  CGDataProviderRef cgDataProvider = CGDataProviderCreateWithCFData(cfDataRef);
  if (cgDataProvider == NULL)
  {
    CFRelease(cgColorSpace);
    CFRelease(cfDataRef);
    return ERR_RT_OUT_OF_MEMORY;
  }

  uint8_t* cgData = (uint8_t*)CFDataGetMutableBytePtr(cfDataRef);
  FOG_ASSERT(cgData != NULL);

  converter.blitRect(cgData, cgStride, d->first, d->stride, size.w, size.h);

  *cgImage = CGImageCreate(size.w, size.h, cgBPC, cgBPP, cgStride, 
    cgColorSpace, cgFmt, cgDataProvider, NULL, true, kCGRenderingIntentDefault);

  // We don't need to release cfDataRef, becuase CGDataProviderCreateWithCFData
  // takes ownership of the cfDataRef object.
  CFRelease(cgColorSpace);
  CFRelease(cgDataProvider);

  if (*cgImage == NULL)
    return ERR_RT_OUT_OF_MEMORY;
  else
    return ERR_OK;
}

static err_t FOG_CDECL Image_fromCGImage(Image* self, CGImageRef cgImage)
{
  if (FOG_IS_NULL(cgImage))
  {
    self->reset();
    return ERR_RT_INVALID_ARGUMENT;
  }

  // Instead of making extensive checks of cgImage we just create compatible
  // destination image where CGContext paint to, and draw the source image
  // onto it. This generally means that we should support any incoming cgImage.
  CGBitmapInfo info = CGImageGetBitmapInfo(cgImage);

  int w = (int)CGImageGetWidth(cgImage);
  int h = (int)CGImageGetHeight(cgImage);

  uint32_t cgBpc     = 0;
  uint32_t cgBpp     = 0;
  uint32_t cgFormat  = 0;
  uint32_t fogFormat = IMAGE_FORMAT_NULL;

  uint32_t alphaInfo = uint32_t(info) & kCGBitmapAlphaInfoMask;

  if (alphaInfo != kCGImageAlphaNone          &&
      alphaInfo != kCGImageAlphaNoneSkipLast  &&
      alphaInfo != kCGImageAlphaNoneSkipFirst )
  {
    // We discarded all AlphaMasks which describes image format without alpha,
    // so we are pretty sure that incoming cgImage has the alpha-channel. So
    // the possibility at now are PRGB32 or A8. At this time we don't support
    // other depths, because Fog::Image is not compatible to CGImageRef when
    // using more than 8-bits per component.
    if (alphaInfo == kCGImageAlphaOnly)
    {
      cgBpc = 8;
      cgBpp = 8;
      cgFormat = kCGImageAlphaOnly;
      fogFormat = IMAGE_FORMAT_A8;
    }
    else
    {
      cgBpc = 8;
      cgBpp = 32;
      cgFormat = kCGBitmapByteOrder32Host | kCGImageAlphaPremultipliedFirst;
      fogFormat = IMAGE_FORMAT_PRGB32;
    }
  }
  else
  {
    cgBpc = 8;
    cgBpp = 32;
    cgFormat = kCGBitmapByteOrder32Host | kCGImageAlphaNoneSkipFirst;
    fogFormat = IMAGE_FORMAT_XRGB32;
  }

  FOG_RETURN_ON_ERROR(self->create(SizeI(w, h), fogFormat));
  ImageData* d = self->_d;

  CGColorSpaceRef cgColorSpace = CGColorSpaceCreateDeviceRGB();
  if (cgColorSpace == NULL)
  {
    self->reset();
    return ERR_RT_OUT_OF_MEMORY;
  }

  CGContextRef ctx = CGBitmapContextCreate(d->first, d->size.w, d->size.h, cgBpc, d->stride, cgColorSpace, cgFormat);

  if (ctx != NULL)
  {
    CGContextSetBlendMode(ctx, kCGBlendModeCopy);
    CGContextDrawImage(ctx, CGRectMake(0.0f, 0.0f, float(w), float(h)), cgImage);
    CGContextRelease(ctx);
  }

  CFRelease(cgColorSpace);
  
  if (ctx == NULL)
  {
    self->reset();
    return ERR_RT_OUT_OF_MEMORY;
  }
  
  return ERR_OK;
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

  fog_api.image_toCGImage = Image_toCGImage;
  fog_api.image_fromCGImage = Image_fromCGImage;
}

} // Fog namespace
