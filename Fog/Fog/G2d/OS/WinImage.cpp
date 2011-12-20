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
#include <Fog/G2d/OS/OSUtil.h>
#include <Fog/G2d/Painting/RasterApi_p.h>

namespace Fog {

// ============================================================================
// [Fog::WinDibImageData]
// ============================================================================

struct FOG_NO_EXPORT WinDibImageData : public ImageData
{
  HBITMAP hBitmap;
};

// ============================================================================
// [Fog::Image - WinDib]
// ============================================================================

// The following code can be used to test the Fog::Image <=> HBITMAP conversion:
//
// HDC hdc = CreateCompatibleDC(NULL);
// HBITMAP hbmp = CreateCompatibleBitmap(hdc, 320, 200);
// RECT r = {0 , 0, 320, 200 };
//
// HGDIOBJ old = SelectObject(hdc, (HGDIOBJ)hbmp);
// FillRect(hdc, &r, (HBRUSH)GetStockObject(WHITE_BRUSH));
// SetBkMode(hdc, TRANSPARENT);
// TextOut(hdc, 0, 0, "Abcdefghijklmnop", 16);
// SelectObject(hdc, old);
// DeleteObject(hdc);
//
// Image i;
// i.fromHBITMAP(hbmp);
// DeleteObject((HGDIOBJ)hbmp);

static err_t Image_WinDib_createDibSection(HBITMAP* dst, uint8_t** bits, ssize_t* stride, const SizeI* size, uint32_t format)
{
  err_t err = ERR_OK;
  HBITMAP hBitmap = NULL;

  switch (format)
  {
    case IMAGE_FORMAT_PRGB32:
    {
      BITMAPV4HEADER bmi;
      ZeroMemory(&bmi, sizeof(bmi));

      bmi.bV4Size          = sizeof(bmi);
      bmi.bV4Width         = size->w;
      bmi.bV4Height        =-size->h;
      bmi.bV4Planes        = 1;
      bmi.bV4BitCount      = 32;
      bmi.bV4V4Compression = BI_RGB;

      bmi.bV4RedMask       = 0x00FF0000;
      bmi.bV4GreenMask     = 0x0000FF00;
      bmi.bV4BlueMask      = 0x000000FF;
      bmi.bV4AlphaMask     = 0xFF000000;

      hBitmap = ::CreateDIBSection(NULL, reinterpret_cast<BITMAPINFO*>(&bmi), DIB_RGB_COLORS, (void**)bits, NULL, 0);

      if (hBitmap == NULL)
        err = ERR_RT_OUT_OF_MEMORY;
      break;
    }

    case IMAGE_FORMAT_XRGB32:
    case IMAGE_FORMAT_RGB24:
    {
      BITMAPINFOHEADER bmi;
      ZeroMemory(&bmi, sizeof(bmi));

      bmi.biSize           = sizeof(bmi);
      bmi.biWidth          = size->w;
      bmi.biHeight         =-size->h;
      bmi.biPlanes         = 1;
      bmi.biBitCount       = ImageFormatDescription::getByFormat(format).getDepth();
      bmi.biCompression    = BI_RGB;

      hBitmap = ::CreateDIBSection(NULL, reinterpret_cast<BITMAPINFO*>(&bmi), DIB_RGB_COLORS, (void**)bits, NULL, 0);

      if (hBitmap == NULL)
        err = ERR_RT_OUT_OF_MEMORY;
      break;
    }

    case IMAGE_FORMAT_I8:
    {
      // TODO: IMAGE_FORMAT_I8 windows support.
    }

    // Other formats are not supported.
    default:
    {
      err = ERR_IMAGE_INVALID_FORMAT;
      break;
    }
  }

  *dst = hBitmap;
  if (hBitmap)
  {
    DIBSECTION info;
    ::GetObjectW(hBitmap, sizeof(DIBSECTION), &info);

    *stride = info.dsBm.bmWidthBytes;
  }

  return err;
}

static err_t FOG_CDECL Image_WinDib_create(ImageData** pd, const SizeI* size, uint32_t format);
static void  FOG_CDECL Image_WinDib_destroy(ImageData* d);
static void* FOG_CDECL Image_WinDib_getHandle(const ImageData* d);
static err_t FOG_CDECL Image_WinDib_updatePalette(ImageData* d, const Range* range);

static const ImageVTable Image_WinDib_vTable =
{
  Image_WinDib_create,
  Image_WinDib_destroy,
  Image_WinDib_getHandle,
  Image_WinDib_updatePalette
};

static err_t FOG_CDECL Image_WinDib_create(ImageData** pd, const SizeI* size, uint32_t format)
{
  HBITMAP hBitmap;
  uint8_t* bits;
  ssize_t stride;

  FOG_RETURN_ON_ERROR(Image_WinDib_createDibSection(&hBitmap, &bits, &stride, size, format));

  const ImageFormatDescription& desc = ImageFormatDescription::getByFormat(format);
  WinDibImageData* d = reinterpret_cast<WinDibImageData*>(MemMgr::alloc(sizeof(WinDibImageData)));

  if (FOG_IS_NULL(d))
  {
    ::DeleteObject((HGDIOBJ)hBitmap);
    return ERR_RT_OUT_OF_MEMORY;
  }

  d->reference.init(1);
  d->vType = VAR_TYPE_IMAGE | VAR_FLAG_NONE;
  d->locked = 0;

  d->vtable = &Image_WinDib_vTable;
  d->size = *size;
  d->format = format;
  d->type = IMAGE_TYPE_WIN_DIB;
  d->adopted = 0;
  d->colorKey = IMAGE_COLOR_KEY_NONE;
  d->bytesPerPixel = desc.getBytesPerPixel();
  FOG_PADDING_ZERO_64(d->padding);

  d->data = bits;
  d->first = bits;
  d->stride = stride;

  d->palette.init();
  d->hBitmap = hBitmap;

  *pd = d;
  return ERR_OK;
}

static void FOG_CDECL Image_WinDib_destroy(ImageData* _d)
{
  WinDibImageData* d = reinterpret_cast<WinDibImageData*>(_d);

  if (d->hBitmap)
    ::DeleteObject((HGDIOBJ)d->hBitmap);

  d->palette.destroy();
  MemMgr::free(d);
}

static void* FOG_CDECL Image_WinDib_getHandle(const ImageData* _d)
{
  const WinDibImageData* d = reinterpret_cast<const WinDibImageData*>(_d);
  return (void*)d->hBitmap;
}

static err_t FOG_CDECL Image_WinDib_updatePalette(ImageData* _d, const Range* range)
{
  WinDibImageData* d = reinterpret_cast<WinDibImageData*>(_d);

  if (d->format == IMAGE_FORMAT_I8)
  {
    // TODO: WinDib update palette.
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::Image - FromHBITMAP / ToHBITMAP]
// ============================================================================

static err_t FOG_CDECL Image_toHBITMAP(const Image* self, HBITMAP* dst)
{
  ImageData* d = self->_d;

  if (d->stride == 0)
  {
    *dst = NULL;
    return ERR_IMAGE_INVALID_SIZE;
  }

  // If the image format is PRGB32/XRGB32/RGB24 then it's easy, it's only
  // needed to create DIBSECTION and copy bits there. If the image format
  // is A8/I8 then we create 32-bit DIBSECTION and copy there the alphas,
  // this image will be still usable when using functions like AlphaBlend().

  uint8_t* dstBits = NULL;
  ssize_t dstStride = 0;

  uint8_t* srcBits = d->first;
  ssize_t srcStride = d->stride;

  uint32_t dstFormat = IMAGE_FORMAT_NULL;

  switch (d->format)
  {
    case IMAGE_FORMAT_PRGB32:
    case IMAGE_FORMAT_PRGB64:
      dstFormat = IMAGE_FORMAT_PRGB32;
      break;

    case IMAGE_FORMAT_XRGB32:
    case IMAGE_FORMAT_RGB24:
      dstFormat = d->format;
      break;

    // There is no such concept like Alpha-only image in Windows GDI. So we
    // treat this kind of image as premultiplied RGB. It's waste, but I have
    // no other idea.
    case IMAGE_FORMAT_A8:
    case IMAGE_FORMAT_A16:
      dstFormat = IMAGE_FORMAT_PRGB32;
      break;

    case IMAGE_FORMAT_I8:
      dstFormat = IMAGE_FORMAT_XRGB32;
      break;

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  HBITMAP hBitmap;
  FOG_RETURN_ON_ERROR(Image_WinDib_createDibSection(&hBitmap, &dstBits, &dstStride, &d->size, dstFormat));

  RasterVBlitLineFunc blitLine = _api_raster.getCompositeCore(dstFormat, COMPOSITE_SRC)->vblit_line[d->format];

  RasterClosure closure;
  closure.ditherOrigin.reset();
  closure.palette = d->palette->_d;
  closure.data = NULL;

  int w = d->size.w;
  int h = d->size.h;

  for (int y = 0; y < h; y++, dstBits += dstStride, srcBits += srcStride, closure.ditherOrigin.y++)
  {
    blitLine(dstBits, srcBits, w, &closure);
  }

  *dst = hBitmap;
  return ERR_OK;
}

static err_t FOG_CDECL Image_fromHBITMAP(Image* self, HBITMAP hBitmap)
{
  if (hBitmap == NULL)
  {
    self->reset();
    return ERR_RT_INVALID_ARGUMENT;
  }

  uint32_t format = IMAGE_FORMAT_NULL;
  DIBSECTION ds;

  // --------------------------------------------------------------------------
  // [DIBSECTION]
  // --------------------------------------------------------------------------

  if (GetObjectW(hBitmap, sizeof(DIBSECTION), &ds) != 0)
  {
    RasterVBlitLineFunc blitLine = NULL;

    // RGB24.
    if (ds.dsBm.bmBitsPixel == 24)
    {
      format = IMAGE_FORMAT_RGB24;
      blitLine = _api_raster.convert.copy[RASTER_COPY_24];
    }

    // RGB32.
    if (ds.dsBm.bmBitsPixel == 32 && ds.dsBitfields[0] == 0x00FF0000 &&
                                     ds.dsBitfields[1] == 0x0000FF00 &&
                                     ds.dsBitfields[2] == 0x000000FF)
    {
      format = IMAGE_FORMAT_XRGB32;
      blitLine = _api_raster.convert.copy[RASTER_COPY_32];
    }

    if (blitLine)
    {
      int w = (int)ds.dsBm.bmWidth;
      int h = Math::abs(ds.dsBm.bmHeight);

      FOG_RETURN_ON_ERROR(self->create(SizeI(w, h), format));
      ImageData* d = self->_d;

      uint8_t* dstPixels = d->first;
      ssize_t dstStride = d->stride;

      const uint8_t* srcPixels = (const uint8_t*)ds.dsBm.bmBits;
      ssize_t srcStride = ds.dsBm.bmWidthBytes;

      // Bottom-To-Top.
      if (ds.dsBm.bmHeight > 0)
      {
        srcPixels += (ssize_t)(h - 1) * srcStride;
        srcStride = -srcStride;
      }

      for (int y = ds.dsBm.bmHeight - 1; y >= 0; y--, dstPixels += dstStride, srcPixels += srcStride)
      {
        blitLine(dstPixels, srcPixels, w, NULL);
      }
      return ERR_OK;
    }
  }

  // --------------------------------------------------------------------------
  // [BITMAP]
  // --------------------------------------------------------------------------

  if (GetObjectW(hBitmap, sizeof(BITMAP), &ds.dsBm) != 0)
  {
    HDC hdc;
    HBITMAP hOldBitmap;
    BITMAPINFO di;

    switch (ds.dsBm.bmBitsPixel)
    {
      case  1:
      case  4:
      case  8:
      case 16:
      case 24: format = IMAGE_FORMAT_RGB24; break;
      case 32: format = IMAGE_FORMAT_XRGB32; break;

      default:
        return ERR_IMAGE_INVALID_FORMAT;
    }

    FOG_RETURN_ON_ERROR(self->create(SizeI((int)ds.dsBm.bmWidth, (int)ds.dsBm.bmHeight), format));
    ImageData* d = self->_d;

    uint8_t* dstPixels = d->first;
    ssize_t dstStride = d->stride;

    // DDB bitmap.
    if ((hdc = CreateCompatibleDC(NULL)) == NULL)
    {
      self->reset();
      return OSUtil::getErrFromOSLastError();
    }

    if ((hOldBitmap = (HBITMAP)SelectObject(hdc, (HGDIOBJ)hBitmap)) == NULL)
    {
      DeleteDC(hdc);

      self->reset();
      return OSUtil::getErrFromOSLastError();
    }

    ZeroMemory(&di, sizeof(BITMAPINFO));
    di.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    di.bmiHeader.biCompression = BI_RGB;
    di.bmiHeader.biWidth = ds.dsBm.bmWidth;
    di.bmiHeader.biHeight = ds.dsBm.bmHeight;
    di.bmiHeader.biBitCount = d->bytesPerPixel * 8;
    di.bmiHeader.biPlanes = 1;

    for (int y = ds.dsBm.bmHeight - 1; y >= 0; y--, dstPixels += dstStride)
    {
      GetDIBits(hdc, hBitmap, y, 1, (LPVOID)dstPixels, &di, DIB_RGB_COLORS);
    }

    SelectObject(hdc, (HGDIOBJ)hOldBitmap);
    DeleteDC(hdc);
    return ERR_OK;
  }

  self->reset();
  return ERR_RT_INVALID_ARGUMENT;
}

// ============================================================================
// [Fog::Image - GetDC / ReleaseDC]
// ============================================================================

static err_t FOG_CDECL Image_getDC(Image* self, HDC* _hDC)
{
  WinDibImageData* d = reinterpret_cast<WinDibImageData*>(self->_d);

  if (_hDC == NULL)
    return ERR_RT_INVALID_ARGUMENT;

  if (d->type != IMAGE_TYPE_WIN_DIB)
    return ERR_IMAGE_INVALID_TYPE;

  if (!d->isDetached())
  {
    FOG_RETURN_ON_ERROR(self->_detach());
    d = reinterpret_cast<WinDibImageData*>(self->_d);

    // Image::detach() must keep the current image format and type.
    FOG_ASSERT(d->type == IMAGE_TYPE_WIN_DIB);
  }

  FOG_ASSERT(d->hBitmap != NULL);

  HDC hDC = CreateCompatibleDC(NULL);
  if (hDC == NULL)
  {
    return OSUtil::getErrFromOSLastError();
  }

  HGDIOBJ hOldObj = SelectObject(hDC, d->hBitmap);
  if (hOldObj == (HGDIOBJ)GDI_ERROR)
  {
    DeleteDC(hDC);
    return OSUtil::getErrFromOSLastError();
  }

  d->locked++;
  *_hDC = hDC;

  return ERR_OK;
}

static err_t Image_releaseDC(Image* self, HDC hDC)
{
  WinDibImageData* d = reinterpret_cast<WinDibImageData*>(self->_d);

  if (hDC == NULL)
    return ERR_RT_INVALID_ARGUMENT;

  if (d->type != IMAGE_TYPE_WIN_DIB)
    return ERR_IMAGE_INVALID_TYPE;

  if (!DeleteDC(hDC))
    return OSUtil::getErrFromOSLastError();

  if (d->locked == 0)
  {
    // TODO: Logger
    return ERR_RT_INVALID_STATE;
  }
  else
  {
    d->locked--;
    return ERR_OK;
  }
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Image_init_win(void)
{
  // --------------------------------------------------------------------------
  // [VTable]
  // --------------------------------------------------------------------------

  fog_api.image_vTable[IMAGE_TYPE_WIN_DIB] = &Image_WinDib_vTable;

  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  fog_api.image_toHBITMAP = Image_toHBITMAP;
  fog_api.image_fromHBITMAP = Image_fromHBITMAP;
  fog_api.image_getDC = Image_getDC;
  fog_api.image_releaseDC = Image_releaseDC;
}

} // Fog namespace
