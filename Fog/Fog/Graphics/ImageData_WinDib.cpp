// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

#include <Fog/Core/Build.h>
#if defined(FOG_OS_WINDOWS)

// [Dependencies]
#include <Fog/Core/Assert.h>
#include <Fog/Core/Memory.h>
#include <Fog/Graphics/Image.h>
#include <Fog/Graphics/ImageData_WinDib_p.h>
#include <Fog/Graphics/RasterEngine_p.h>

namespace Fog {

// ============================================================================
// [Fog::WinDibImageData]
// ============================================================================

WinDibImageData::WinDibImageData() :
  hBitmap(NULL)
{
  type = IMAGE_TYPE_WIN_DIB;
}

WinDibImageData::~WinDibImageData()
{
  WinDibImageData::destroy();
}

ImageData* WinDibImageData::clone() const
{
  FOG_ASSERT(width > 0 && height > 0);

  ImageData* newd = _createInstance();
  if (newd == NULL) return NULL;

  if (newd->create(width, height, format) != ERR_OK)
  {
    newd->deref();
    return NULL;
  }

  rasterFuncs.dib.vblit_rect[newd->format](
    newd->first, newd->stride,
    this->first, this->stride,
    newd->width, newd->height, NULL);
  newd->palette = this->palette;

  return newd;
}

err_t WinDibImageData::create(int w, int h, uint32_t format)
{
  FOG_ASSERT(refCount.get() == 1);

  WinDibImageData::destroy();

  this->data = NULL;
  hBitmap = _createDibSection(w, h, format, &this->data, &this->stride);

  if (hBitmap != NULL)
  {
    this->width = w;
    this->height = h;
    this->format = format;
    this->bytesPerPixel = Image::getBytesPerPixelFromFormat(format);
    this->first = data;

    return ERR_OK;
  }
  else
  {
    // Failed.
    this->width = 0;
    this->height = 0;
    
    this->format = IMAGE_FORMAT_NULL;
    this->bytesPerPixel = 0;

    this->data = NULL;
    this->first = NULL;
    this->stride = 0;

    this->palette.free();

    return ERR_IMAGE_UNSUPPORTED_FORMAT;
  }
}

err_t WinDibImageData::destroy()
{
  if (hBitmap)
  {
    DeleteObject(hBitmap);
    hBitmap = NULL;
  }

  return ERR_OK;
}

void* WinDibImageData::getHandle()
{
  return (void*)hBitmap;
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

  if (hDC)
  {
    DeleteDC(hDC);

    FOG_ASSERT(locked > 0);
    locked--;
  }
}

ImageData* WinDibImageData::_createInstance()
{
  WinDibImageData* d = 
    reinterpret_cast<WinDibImageData*>(Memory::alloc(sizeof(WinDibImageData)));
  if (d == NULL) return NULL;

  return fog_new_p(d) WinDibImageData();
}

HBITMAP WinDibImageData::_createDibSection(int w, int h, uint32_t format,
  uint8_t** bits, sysint_t* stride)
{
  HBITMAP hBitmap = NULL;

  switch (format)
  {
    // We can't distinguish between these two after the dib-section was created.
    // But I'm not sure if we can simply return ERR_IMAGE_NOT_SUPPORTED, because
    // Gdi+ can use ARGB32 non-premultiplied images.
    case IMAGE_FORMAT_PRGB32:
    case IMAGE_FORMAT_ARGB32:
    {
      BITMAPV4HEADER bmi;
      ZeroMemory(&bmi, sizeof(bmi));

      bmi.bV4Size = sizeof(bmi);
      bmi.bV4Width         = (int)w;
      bmi.bV4Height        = -((int)h);
      bmi.bV4Planes        = 1;
      bmi.bV4BitCount      = 32;
      bmi.bV4V4Compression = BI_RGB;

      bmi.bV4RedMask       = 0x00FF0000;
      bmi.bV4GreenMask     = 0x0000FF00;
      bmi.bV4BlueMask      = 0x000000FF;
      bmi.bV4AlphaMask     = 0xFF000000;

      hBitmap = CreateDIBSection(NULL, reinterpret_cast<BITMAPINFO*>(&bmi), DIB_RGB_COLORS, (void**)bits, NULL, 0);
      break;
    }

    case IMAGE_FORMAT_XRGB32:
    {
      BITMAPINFOHEADER bmi;

      ZeroMemory(&bmi, sizeof(bmi));
      bmi.biSize           = sizeof(bmi);
      bmi.biWidth          = (int)w;
      bmi.biHeight         = -((int)h);
      bmi.biPlanes         = 1;
      bmi.biBitCount       = 32;
      bmi.biCompression    = BI_RGB;

      hBitmap = CreateDIBSection(NULL, reinterpret_cast<BITMAPINFO*>(&bmi), DIB_RGB_COLORS, (void**)bits, NULL, 0);
      break;
    }

    // All others are unsupported format. IMAGE_FORMAT_I8 supports transparency
    // (this is impossible with GDI) and IMAGE_FORMAT_A8 is alpha-channel only
    // (there is nothing similar in GDI).
    case IMAGE_FORMAT_A8:
    case IMAGE_FORMAT_I8:
    default:
    {
      break;
    }
  }

  if (hBitmap)
  {
    DIBSECTION info;
    GetObjectW(hBitmap, sizeof(DIBSECTION), &info);
    *stride = info.dsBm.bmWidthBytes;
  }

  return hBitmap;
}

} // Fog namespace

#endif // FOG_OS_WINDOWS
