// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Guard]
#include <Fog/Core/Config/Config.h>
#if defined(FOG_OS_WINDOWS)

// [Dependencies]
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Memory/Memory.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/WinImage_p.h>
#include <Fog/G2d/Render/RenderApi_p.h>
#include <Fog/G2d/Render/RenderConstants_p.h>

namespace Fog {

// ============================================================================
// [Fog::WinDibImageData]
// ============================================================================

#include <Fog/Core/Pack/PackByte.h>
struct BitfieldsBitmapHeader
{
  BITMAPINFOHEADER bmi;
  DWORD rMask;
  DWORD gMask;
  DWORD bMask;
};
#include <Fog/Core/Pack/PackRestore.h>

WinDibImageData::WinDibImageData(const SizeI& size, uint32_t format, uint8_t* bits, sysint_t stride, HBITMAP hBitmap)
{
  this->type = IMAGE_BUFFER_WIN_DIB;
  this->flags = NO_FLAGS;
  this->format = format;
  this->bytesPerPixel = ImageFormatDescription::getByFormat(format).getBytesPerPixel();
  this->size = size;
  this->data = bits;
  this->first = bits;
  this->stride = stride;
  this->hBitmap = hBitmap;
}

WinDibImageData::~WinDibImageData()
{
  if (hBitmap) DeleteObject(hBitmap);
}

ImageData* WinDibImageData::clone() const
{
  FOG_ASSERT(size.w > 0 && size.h > 0);

  ImageData* newd;
  err_t err = _createDibImage(size, format, &newd);
  if (FOG_IS_ERROR(err)) return NULL;

  RenderVBlitRectFn blit = _g2d_render.getCopyRectFn(format);
  blit(newd->first, newd->stride,
       this->first, this->stride,
       newd->size.w, newd->size.h, NULL);

  newd->palette = this->palette;
  newd->paletteModified(Range(0, newd->palette.getLength()));

  return newd;
}

void* WinDibImageData::getHandle()
{
  return (void*)hBitmap;
}

void WinDibImageData::paletteModified(const Range& range)
{
  if (format == IMAGE_FORMAT_I8)
  {
    // TODO: WinDIB Palette support
  }
}

HDC WinDibImageData::getDC()
{
  FOG_ASSERT(refCount.get() == 1);

  HDC hDC;
  HGDIOBJ hOldObj;

  if (hBitmap == NULL) return (HDC)NULL;

  hDC = CreateCompatibleDC(NULL);
  if (hDC == NULL) goto failed;

  hOldObj = SelectObject(hDC, hBitmap);
  if (hOldObj == (HGDIOBJ)GDI_ERROR)
  {
    DeleteDC(hDC);
    hDC = NULL;
    goto failed;
  }
  
  locked++;

failed:
  return hDC;
}

void WinDibImageData::releaseDC(HDC hDC)
{
  FOG_ASSERT(refCount.get() == 1);

  if (hDC == NULL) return;
  DeleteDC(hDC);

  FOG_ASSERT(locked > 0);
  locked--;
}

err_t WinDibImageData::_createDibSection(const SizeI& size, uint32_t format, HBITMAP* dst, uint8_t** bits, sysint_t* stride)
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
      bmi.bV4Width         = size.w;
      bmi.bV4Height        = -size.h;
      bmi.bV4Planes        = 1;
      bmi.bV4BitCount      = 32;
      bmi.bV4V4Compression = BI_RGB;

      bmi.bV4RedMask       = 0x00FF0000;
      bmi.bV4GreenMask     = 0x0000FF00;
      bmi.bV4BlueMask      = 0x000000FF;
      bmi.bV4AlphaMask     = 0xFF000000;

      hBitmap = CreateDIBSection(NULL, reinterpret_cast<BITMAPINFO*>(&bmi), DIB_RGB_COLORS, (void**)bits, NULL, 0);
      if (hBitmap == NULL) err = ERR_RT_OUT_OF_MEMORY;
      break;
    }

    case IMAGE_FORMAT_XRGB32:
    case IMAGE_FORMAT_RGB24:
    {
      BITMAPINFOHEADER bmi;
      ZeroMemory(&bmi, sizeof(bmi));

      bmi.biSize           = sizeof(bmi);
      bmi.biWidth          = size.w;
      bmi.biHeight         = -size.h;
      bmi.biPlanes         = 1;
      bmi.biBitCount       = ImageFormatDescription::getByFormat(format).getDepth();
      bmi.biCompression    = BI_RGB;

      hBitmap = CreateDIBSection(NULL, reinterpret_cast<BITMAPINFO*>(&bmi), DIB_RGB_COLORS, (void**)bits, NULL, 0);
      if (hBitmap == NULL) err = ERR_RT_OUT_OF_MEMORY;
      break;
    }

    // TODO: ImageI8 windows support.
    case IMAGE_FORMAT_I8:

    // Other formats are not supported.
    default:
    {
      err = ERR_IMAGE_UNSUPPORTED_FORMAT;
      break;
    }
  }

  *dst = hBitmap;
  if (hBitmap)
  {
    DIBSECTION info;
    GetObjectW(hBitmap, sizeof(DIBSECTION), &info);

    *stride = info.dsBm.bmWidthBytes;
  }

  return err;
}

err_t WinDibImageData::_createDibImage(const SizeI& size, uint32_t format, ImageData** dst)
{
  uint8_t* bits;
  sysint_t stride;
  HBITMAP hBitmap;
  WinDibImageData* d;

  FOG_RETURN_ON_ERROR(_createDibSection(size, format, &hBitmap, &bits, &stride));
  d = reinterpret_cast<WinDibImageData*>(Memory::alloc(sizeof(WinDibImageData)));

  if (d == NULL)
  {
    DeleteObject(hBitmap);
    return ERR_RT_OUT_OF_MEMORY;
  }

  *dst = fog_new_p(d) WinDibImageData(size, format, bits, stride, hBitmap);
  return ERR_OK;
}

} // Fog namespace

// [Guard]
#endif // FOG_OS_WINDOWS
