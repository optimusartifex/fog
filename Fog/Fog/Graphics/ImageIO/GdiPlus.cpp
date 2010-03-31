// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Build/Build.h>

#if defined(FOG_OS_WINDOWS)

#include <Fog/Core/Constants.h>
#include <Fog/Core/Library.h>
#include <Fog/Core/ManagedString.h>
#include <Fog/Core/Math.h>
#include <Fog/Core/Misc.h>
#include <Fog/Core/Static.h>
#include <Fog/Core/Stream.h>
#include <Fog/Core/String.h>
#include <Fog/Core/Strings.h>
#include <Fog/Core/WinIStream_p.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Image.h>
#include <Fog/Graphics/ImageIO.h>
#include <Fog/Graphics/ImageIO/GdiPlus_p.h>
#include <Fog/Graphics/RasterEngine_p.h>

namespace Fog {
namespace ImageIO {

// ===========================================================================
// [Fog::ImageIO::GdiPlus - Format - Helpers]
// ===========================================================================

static uint32_t GdiPlus_fogFormatFromGpFormat(GpPixelFormat gpFormat)
{
  switch (gpFormat)
  {
    case GpPixelFormat1bppIndexed:
    case GpPixelFormat4bppIndexed:
    case GpPixelFormat8bppIndexed:
      return PIXEL_FORMAT_I8;

    case GpPixelFormat16bppGrayScale:
      return PIXEL_FORMAT_I8;

    case GpPixelFormat16bppRGB555:
    case GpPixelFormat16bppRGB565:
      return PIXEL_FORMAT_XRGB32;

    case GpPixelFormat16bppARGB1555:
      return PIXEL_FORMAT_ARGB32;

    case GpPixelFormat24bppRGB:
    case GpPixelFormat32bppRGB:
      return PIXEL_FORMAT_XRGB32;

    case GpPixelFormat32bppARGB:
      return PIXEL_FORMAT_ARGB32;

    case GpPixelFormat32bppPARGB:
      return PIXEL_FORMAT_PRGB32;

    case GpPixelFormat48bppRGB:
      return PIXEL_FORMAT_XRGB32;

    case GpPixelFormat64bppARGB:
      return PIXEL_FORMAT_ARGB32;

    case GpPixelFormat64bppPARGB:
      return PIXEL_FORMAT_PRGB32;

    default:
      return PIXEL_FORMAT_NULL;
  }
}

static GpPixelFormat gdiPlus_gpFormatFromFogFormat(int fogFormat)
{
  switch (fogFormat)
  {
    case PIXEL_FORMAT_PRGB32: return GpPixelFormat32bppPARGB;
    case PIXEL_FORMAT_ARGB32: return GpPixelFormat32bppARGB;
    case PIXEL_FORMAT_XRGB32: return GpPixelFormat32bppRGB;
    case PIXEL_FORMAT_A8    : return GpPixelFormat8bppIndexed;
    case PIXEL_FORMAT_I8    : return GpPixelFormat8bppIndexed;

    default:
      return GpPixelFormatUndefined;
  }
}

// ===========================================================================
// [Fog::ImageIO::GdiPlus - Params - GUID]
// ===========================================================================

#define MY_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
  static const GUID name = { l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } }

MY_GUID(GpEncoderQuality, 0x1d5be4b5, 0xfa4a, 0x452d, 0x9c, 0xdd, 0x5d, 0xb3, 0x51, 0x05, 0xe7, 0xeb);

#undef MU_GUID

// ===========================================================================
// [Fog::ImageIO::GdiPlus - Params - Helpers]
// ===========================================================================

static void GdiPlus_clearCommonParameters(GdiPlusCommonParams* params, uint32_t fileType)
{
  memset(params, 0, sizeof(GdiPlusCommonParams));

  switch (fileType)
  {
    case IMAGE_IO_FILE_JPEG:
      params->jpeg.quality = 90;
      break;
    case IMAGE_IO_FILE_PNG:
      break;
    case IMAGE_IO_FILE_TIFF:
      break;
  }
}

static err_t GdiPlus_getCommonParameter(const GdiPlusCommonParams* params, uint32_t fileType, const ManagedString& name, Value& value)
{
  // This means to continue property processing calling superclass.
  err_t err = (err_t)0xFFFFFFFF;

  switch (fileType)
  {
    case IMAGE_IO_FILE_JPEG:
      if (name == fog_strings->getString(STR_GRAPHICS_quality))
      {
        return value.setInt32(params->jpeg.quality);
      }
      break;
    case IMAGE_IO_FILE_PNG:
      break;
    case IMAGE_IO_FILE_TIFF:
      break;
  }

  return err;
}

static err_t GdiPlus_setCommonParameter(GdiPlusCommonParams* params, uint32_t fileType, const ManagedString& name, const Value& value)
{
  // This means to continue property processing calling superclass.
  err_t err = (err_t)0xFFFFFFFF;

  switch (fileType)
  {
    case IMAGE_IO_FILE_JPEG:
      if (name == fog_strings->getString(STR_GRAPHICS_quality))
      {
        int i;
        if ((err = value.getInt32(&i)) == ERR_OK)
          params->jpeg.quality = Math::bound(i, 0, 100);
        return ERR_OK;
      }
      break;
    case IMAGE_IO_FILE_PNG:
      break;
    case IMAGE_IO_FILE_TIFF:
      break;
  }

  return err;
}

// ===========================================================================
// [Fog::ImageIO::GdiPlusLibrary]
// ===========================================================================

GdiPlusLibrary::GdiPlusLibrary() :
  err(0xFFFFFFFF),
  gdiplusToken(0)
{
}

GdiPlusLibrary::~GdiPlusLibrary()
{
  close();
}

err_t GdiPlusLibrary::prepare()
{
  if (err == 0xFFFFFFFF)
  {
    FOG_ONCE_LOCK();
    if (err == 0xFFFFFFFF) err = init();
    FOG_ONCE_UNLOCK();
  }

  return err;
}

err_t GdiPlusLibrary::init()
{
  static const char symbols[] =
    "GdiplusStartup\0"
    "GdiplusShutdown\0"
    "GdipLoadImageFromStream\0"
    "GdipSaveImageToStream\0"
    "GdipDisposeImage\0"
    "GdipGetImageType\0"
    "GdipGetImageWidth\0"
    "GdipGetImageHeight\0"
    "GdipGetImageFlags\0"
    "GdipGetImagePixelFormat\0"
    "GdipGetImageGraphicsContext\0"
    "GdipImageGetFrameCount\0"
    "GdipImageSelectActiveFrame\0"
    "GdipCreateBitmapFromScan0\0"
    "GdipSetCompositingMode\0"
    "GdipDrawImageI\0"
    "GdipFlush\0"
    "GdipDeleteGraphics\0"

    "GdipGetImageEncoders\0"
    "GdipGetImageEncodersSize\0"
    ;

  if (dll.open(Ascii8("gdiplus")) != ERR_OK)
  {
    // gdiplus.dll not found.
    return ERR_IMAGEIO_GDIPLUS_NOT_LOADED;
  }

  const char* badSymbol;
  if (dll.getSymbols(addr, symbols, FOG_ARRAY_SIZE(symbols), NUM_SYMBOLS, (char**)&badSymbol) != NUM_SYMBOLS)
  {
    // Some symbol failed to load? Inform about it.
    fog_debug("Fog::ImageIO::GdiPlusLibrary::init() - Can't load symbol '%s'.", badSymbol);
    dll.close();
    return ERR_IMAGEIO_GDIPLUS_NOT_LOADED;
  }

  // GdiPlus - Startup.
  GpGdiplusStartupInput startupInput;
  startupInput.GdiplusVersion = 1;
  startupInput.DebugEventCallback = NULL;
  startupInput.SuppressBackgroundThread = false;
  startupInput.SuppressExternalCodecs = false;

  GpStatus status = pGdiplusStartup(&gdiplusToken, &startupInput, NULL);
  if (status != GpOk)
  {
    fog_debug("Fog::ImageIO::GdiPlusLibrary::init() - GdiplusStartup() failed (%u).", status);
    dll.close();
    return ERR_IMAGEIO_GDIPLUS_NOT_LOADED;
  }

  return ERR_OK;
}

void GdiPlusLibrary::close()
{
  // GdiPlus - Shutdown.
  if (err == ERR_OK)
  {
    pGdiplusShutdown(gdiplusToken);
    gdiplusToken = 0;
  }

  dll.close();
  err = 0xFFFFFFFF;
}

static Static<GdiPlusLibrary> _gdiPlusLibrary;
static Atomic<sysint_t> _gdiPlusRefCount;

// ===========================================================================
// [Fog::ImageIO::GdiPlusProvider]
// ===========================================================================

static err_t getGdiPlusEncoderClsid(const WCHAR* mime, CLSID* clsid)
{
  GpStatus status;
  GpImageCodecInfo* codecs = NULL;

  UINT i;
  UINT codecsCount;
  UINT codecsDataSize;

  err_t err = ERR_OK;

  status = _gdiPlusLibrary->pGdipGetImageEncodersSize(&codecsCount, &codecsDataSize);
  if (status != GpOk)
  {
    err = ERR_IMAGEIO_GDIPLUS_ERROR;
    goto end;
  }

  codecs = reinterpret_cast<GpImageCodecInfo*>(Memory::alloc(codecsDataSize));
  if (codecs == NULL)
  {
    err = ERR_RT_OUT_OF_MEMORY;
    goto end;
  }

  status = _gdiPlusLibrary->pGdipGetImageEncoders(codecsCount, codecsDataSize, codecs);
  if (status != GpOk)
  {
    err = ERR_IMAGEIO_GDIPLUS_ERROR;
    goto end;
  }

  for (i = 0; i < codecsCount; i++)
  {
    if (wcscmp(codecs[i].MimeType, mime) == 0)
    {
      *clsid = codecs[i].Clsid;
      goto end;
    }
  }

  // Shouldn't happen.
  err = ERR_IMAGEIO_INTERNAL_ERROR;

end:
  if (codecs) Memory::free(codecs);
  return err;
}

GdiPlusProvider::GdiPlusProvider(uint32_t fileType)
{
  // Initialize GdiPlusLibrary.
  if (_gdiPlusRefCount.addXchg(1) == 0) _gdiPlusLibrary.init();

  const WCHAR* gdipMime = NULL;

  // File type.
  _fileType = fileType;

  // Supported devices.
  _deviceType = IMAGE_IO_DEVICE_BOTH;

  // Name of ImageIO Provider.
  switch (_fileType)
  {
    case IMAGE_IO_FILE_JPEG:
      _name = fog_strings->getString(STR_GRAPHICS_JPEG);
      _gdipMime = L"image/jpeg";
      break;
    case IMAGE_IO_FILE_PNG:
      _name = fog_strings->getString(STR_GRAPHICS_PNG);
      _gdipMime = L"image/png";
      break;
    case IMAGE_IO_FILE_TIFF:
      _name = fog_strings->getString(STR_GRAPHICS_TIFF);
      _gdipMime = L"image/tiff";
      break;
  }

  // All GDI+ providers have with "[GDI+]" suffix.
  _name.append(Ascii8("[GDI+]"));

  // Supported extensions.
  switch (_fileType)
  {
    case IMAGE_IO_FILE_JPEG:
      _imageExtensions.reserve(4);
      _imageExtensions.append(fog_strings->getString(STR_GRAPHICS_jpg));
      _imageExtensions.append(fog_strings->getString(STR_GRAPHICS_jpeg));
      _imageExtensions.append(fog_strings->getString(STR_GRAPHICS_jfi));
      _imageExtensions.append(fog_strings->getString(STR_GRAPHICS_jfif));
      break;
    case IMAGE_IO_FILE_PNG:
      _imageExtensions.reserve(1);
      _imageExtensions.append(fog_strings->getString(STR_GRAPHICS_png));
      break;
    case IMAGE_IO_FILE_TIFF:
      _imageExtensions.reserve(2);
      _imageExtensions.append(fog_strings->getString(STR_GRAPHICS_tif));
      _imageExtensions.append(fog_strings->getString(STR_GRAPHICS_tiff));
      break;
    default:
      FOG_ASSERT_NOT_REACHED();
  }
}

GdiPlusProvider::~GdiPlusProvider()
{
  // Shutdown GdiPlusLibrary.
  if (_gdiPlusRefCount.deref()) _gdiPlusLibrary.destroy();
}

uint32_t GdiPlusProvider::checkSignature(const void* mem, sysuint_t length) const
{
  // Note: GdiPlus proxy provider uses 14 as a base score. This
  // is by one less than all other providers based on external 
  // libraries (libpng, libjpeg, libtiff) and reason is that when
  // these external libraries are available they are used instead.
  if (!mem || length == 0) return 0;

  uint32_t score = 0;
  sysuint_t i;

  // Mime data.
  static const uint8_t mimeJPEG[2]    = { 0xFF, 0xD8 };
  static const uint8_t mimePNG[8]     = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
  static const uint8_t mimeTIFF_LE[4] = { 0x49, 0x49, 0x00, 0x42 };
  static const uint8_t mimeTIFF_BE[4] = { 0x4D, 0x4D, 0x42, 0x00 };

  // Mime check.
  switch (_fileType)
  {
    case IMAGE_IO_FILE_JPEG:
      i = Math::min<sysuint_t>(length, 2);
      if (memcmp(mem, mimeJPEG, i) == 0)
        score = Math::max<uint32_t>(score, 14 + ((uint32_t)i * 40));
      break;
    case IMAGE_IO_FILE_PNG:
      i = Math::min<sysuint_t>(length, 8);
      if (memcmp(mem, mimePNG, i) == 0)
        score = Math::max<uint32_t>(score, 14 + ((uint32_t)i * 10));
      break;
    case IMAGE_IO_FILE_TIFF:
      i = Math::min<sysuint_t>(length, 4);
      if (memcmp(mem, mimeTIFF_LE, i) == 0 || memcmp(mem, mimeTIFF_BE, i) == 0)
        score = Math::max<uint32_t>(score, 14 + ((uint32_t)i * 20));
      break;
    default:
      FOG_ASSERT_NOT_REACHED();
  }

  return score;
}

err_t GdiPlusProvider::createDevice(uint32_t deviceType, BaseDevice** device) const
{
  BaseDevice* d = NULL;

  err_t err = _gdiPlusLibrary->init();
  if (err) return err;

  switch (deviceType)
  {
    case IMAGE_IO_DEVICE_DECODER:
      d = new(std::nothrow) GdiPlusDecoderDevice(const_cast<GdiPlusProvider*>(this));
      break;
    case IMAGE_IO_DEVICE_ENCODER:
      d = new(std::nothrow) GdiPlusEncoderDevice(const_cast<GdiPlusProvider*>(this));
      break;
    default:
      return ERR_RT_INVALID_ARGUMENT;
  }

  if (!d) return ERR_RT_OUT_OF_MEMORY;

  *device = d;
  return ERR_OK;
}

// ===========================================================================
// [Fog::ImageIO::GdiPlusDecoderDevice - Construction / Destruction]
// ===========================================================================

GdiPlusDecoderDevice::GdiPlusDecoderDevice(Provider* provider) :
  DecoderDevice(provider),
  _istream(NULL),
  _gpImage(NULL)
{
  GdiPlus_clearCommonParameters(&_params, _fileType);
}

GdiPlusDecoderDevice::~GdiPlusDecoderDevice()
{
}

// ===========================================================================
// [Fog::ImageIO::GdiPlusDecoderDevice - AttachStream / DetachStream]
// ===========================================================================

void GdiPlusDecoderDevice::attachStream(Stream& stream)
{
  _istream = new(std::nothrow) WinIStream(stream);

  base::attachStream(stream);
}

void GdiPlusDecoderDevice::detachStream()
{
  if (_gpImage) 
  {
    _gdiPlusLibrary->pGdipDisposeImage(_gpImage);
    _gpImage = NULL;
  }

  if (_istream)
  {
    _istream->Release();
    _istream = NULL;
  }

  base::detachStream();
}

// ===========================================================================
// [Fog::ImageIO::GdiPlusDecoderDevice - Reset]
// ===========================================================================

void GdiPlusDecoderDevice::reset()
{
  GdiPlus_clearCommonParameters(&_params, _fileType);
  DecoderDevice::reset();
}

// ===========================================================================
// [Fog::ImageIO::GdiPlusDecoderDevice - ReadHeader]
// ===========================================================================

err_t GdiPlusDecoderDevice::readHeader()
{
  // Do not read header more than once.
  if (_headerResult) return _headerResult;

  if (_istream == NULL) return ERR_RT_INVALID_HANDLE;

  GpStatus status = _gdiPlusLibrary->pGdipLoadImageFromStream(_istream, &_gpImage);
  if (status != GpOk) return (_headerResult = ERR_IMAGEIO_GDIPLUS_ERROR);

  _gdiPlusLibrary->pGdipGetImageWidth(_gpImage, &_width);
  _gdiPlusLibrary->pGdipGetImageHeight(_gpImage, &_height);
  _planes = 1;

  GpPixelFormat pf;
  _gdiPlusLibrary->pGdipGetImagePixelFormat(_gpImage, &pf);

  _format = GdiPlus_fogFormatFromGpFormat(pf);
  _depth = Image::formatToDepth(_format);

  return ERR_OK;
}

// ===========================================================================
// [Fog::ImageIO::GdiPlusDecoderDevice - ReadImage]
// ===========================================================================

err_t GdiPlusDecoderDevice::readImage(Image& image)
{
  err_t err = ERR_OK;

  if (_istream == NULL) return ERR_RT_INVALID_HANDLE;

  GpBitmap* bm = NULL;
  GpGraphics* gr = NULL;
  GpStatus status;

  // Read image header.
  if (readHeader() != ERR_OK) return _headerResult;

  // Don't read image more than once.
  if (isReaderDone()) return (_readerResult = ERR_IMAGEIO_NO_MORE_FRAMES);

  // Create image.
  if ((err = image.create(_width, _height, _format)) != ERR_OK) return err;

  // Create GpBitmap that will share raster data with our image.
  status = _gdiPlusLibrary->pGdipCreateBitmapFromScan0(
    (INT)image.getWidth(),
    (INT)image.getHeight(), 
    (INT)image.getStride(),
    gdiPlus_gpFormatFromFogFormat(image.getFormat()),
    (BYTE*)image.getMData(),
    &bm);
  if (status != GpOk) { err = ERR_IMAGEIO_GDIPLUS_ERROR; goto end; }

  // Create GpGraphics context.
  status = _gdiPlusLibrary->pGdipGetImageGraphicsContext((GpImage*)bm, &gr);
  if (status != GpOk) { err = ERR_IMAGEIO_GDIPLUS_ERROR; goto end; }

  // Set compositing to source copy (we want alpha bits).
  status = _gdiPlusLibrary->pGdipSetCompositingMode(gr, GpCompositingModeSourceCopy);
  if (status != GpOk) { err = ERR_IMAGEIO_GDIPLUS_ERROR; goto end; }

  // Draw streamed image to GpGraphics context.
  status = _gdiPlusLibrary->pGdipDrawImageI(gr, _gpImage, 0, 0);
  if (status != GpOk) { err = ERR_IMAGEIO_GDIPLUS_ERROR; goto end; }

  // flush (this step is probably not necessary).
  status = _gdiPlusLibrary->pGdipFlush(gr, GpFlushIntentionSync);
  if (status != GpOk) { err = ERR_IMAGEIO_GDIPLUS_ERROR; goto end; }

end:
  // Delete created Gdi+ objects.
  if (gr) _gdiPlusLibrary->pGdipDeleteGraphics(gr);
  if (bm) _gdiPlusLibrary->pGdipDisposeImage((GpImage*)bm);

  if (err == ERR_OK) updateProgress(1.0);
  return (_readerResult = err);
}

// ===========================================================================
// [Fog::ImageIO::GdiPlusDecoderDevice - GetProperty / SetProperty]
// ===========================================================================

err_t GdiPlusDecoderDevice::getProperty(const ManagedString& name, Value& value) const
{
  err_t err = GdiPlus_getCommonParameter(&_params, _fileType, name, value);
  if (err != (err_t)0xFFFFFFFF) return err;

  return base::getProperty(name, value);
}

err_t GdiPlusDecoderDevice::setProperty(const ManagedString& name, const Value& value)
{
  err_t err = GdiPlus_setCommonParameter(&_params, _fileType, name, value);
  if (err != (err_t)0xFFFFFFFF) return err;

  return base::setProperty(name, value);
}

// ===========================================================================
// [Fog::ImageIO::GdiPlusEncoderDevice - Construction / Destruction]
// ===========================================================================

GdiPlusEncoderDevice::GdiPlusEncoderDevice(Provider* provider) :
  EncoderDevice(provider)
{
  GdiPlus_clearCommonParameters(&_params, _fileType);
}

GdiPlusEncoderDevice::~GdiPlusEncoderDevice()
{
}

// ===========================================================================
// [Fog::ImageIO::GdiPlusEncoderDevice - AttachStream / DetachStream]
// ===========================================================================

void GdiPlusEncoderDevice::attachStream(Stream& stream)
{
  _istream = new(std::nothrow) WinIStream(stream);

  base::attachStream(stream);
}

void GdiPlusEncoderDevice::detachStream()
{
  if (_istream)
  {
    _istream->Release();
    _istream = NULL;
  }

  base::detachStream();
}

// ===========================================================================
// [Fog::ImageIO::GdiPlusEncoderDevice - Reset]
// ===========================================================================

void GdiPlusEncoderDevice::reset()
{
  GdiPlus_clearCommonParameters(&_params, _fileType);
  EncoderDevice::reset();
}

// ===========================================================================
// [Fog::ImageIO::GdiPlusEncoderDevice - WriteImage]
// ===========================================================================

err_t GdiPlusEncoderDevice::writeImage(const Image& image)
{
  if (image.isEmpty()) return ERR_IMAGE_INVALID_SIZE;

  err_t err = ERR_OK;
  if (_istream == NULL) return ERR_RT_INVALID_HANDLE;

  GpBitmap* bm = NULL;
  GpGraphics* gr = NULL;
  GpStatus status;

  CLSID encoderClsid;

  // Get GDI+ encoder CLSID.
  err = getGdiPlusEncoderClsid(
    reinterpret_cast<GdiPlusProvider*>(getProvider())->_gdipMime, &encoderClsid);
  if (err) goto end;

  // Create GpBitmap that will share raster data with our image.
  status = _gdiPlusLibrary->pGdipCreateBitmapFromScan0(
    (INT)image.getWidth(),
    (INT)image.getHeight(), 
    (INT)image.getStride(),
    gdiPlus_gpFormatFromFogFormat(image.getFormat()),
    (BYTE*)image.getData(),
    &bm);
  if (status != GpOk) { err = ERR_IMAGEIO_GDIPLUS_ERROR; goto end; }

  // Encoder parameters.
  uint8_t paramsData[sizeof(GpEncoderParameters)];
  GpEncoderParameters* params = reinterpret_cast<GpEncoderParameters*>(paramsData);

  params->Count = 0;

  switch (_fileType)
  {
    case IMAGE_IO_FILE_JPEG:
      params->Count = 1;
      params->Parameter[0].Guid = GpEncoderQuality;
      params->Parameter[0].Type = GpEncoderParameterValueTypeLong;
      params->Parameter[0].NumberOfValues = 1;
      params->Parameter[0].Value = &_params.jpeg.quality;
      break;
  }

  status = _gdiPlusLibrary->pGdipSaveImageToStream(
    (GpImage*)bm, _istream, &encoderClsid,
    // If there are no parameters then NULL pointer must be used instead.
    // This information can be found on MSDN. Windows Vista and Win7 will
    // return an error if (params.Count == 0).
    params->Count > 0 ? params : NULL);

end:
  // Delete created Gdi+ objects.
  if (bm) _gdiPlusLibrary->pGdipDisposeImage((GpImage*)bm);

  if (err == ERR_OK) updateProgress(1.0);
  return err;
}

// ===========================================================================
// [Fog::ImageIO::GdiPlusEncoderDevice - GetProperty / SetProperty]
// ===========================================================================

err_t GdiPlusEncoderDevice::getProperty(const ManagedString& name, Value& value) const
{
  err_t err = GdiPlus_getCommonParameter(&_params, _fileType, name, value);
  if (err != (err_t)0xFFFFFFFF) return err;

  return base::getProperty(name, value);
}

err_t GdiPlusEncoderDevice::setProperty(const ManagedString& name, const Value& value)
{
  err_t err = GdiPlus_setCommonParameter(&_params, _fileType, name, value);
  if (err != (err_t)0xFFFFFFFF) return err;

  return base::setProperty(name, value);
}

} // ImageIO namespace
} // Fog namespace

FOG_IMPLEMENT_OBJECT(Fog::ImageIO::GdiPlusDecoderDevice)
FOG_IMPLEMENT_OBJECT(Fog::ImageIO::GdiPlusEncoderDevice)

// ===========================================================================
// [CAPI]
// ===========================================================================

FOG_INIT_DECLARE void fog_imageio_init_gdiplus(void)
{
  using namespace Fog;

  ImageIO::_gdiPlusRefCount.init(0);
  ImageIO::GdiPlusProvider* provider;

  provider = new(std::nothrow) ImageIO::GdiPlusProvider(IMAGE_IO_FILE_PNG);
  ImageIO::addProvider(IMAGE_IO_DEVICE_BOTH, provider);

  provider = new(std::nothrow) ImageIO::GdiPlusProvider(IMAGE_IO_FILE_JPEG);
  ImageIO::addProvider(IMAGE_IO_DEVICE_BOTH, provider);

  provider = new(std::nothrow) ImageIO::GdiPlusProvider(IMAGE_IO_FILE_TIFF);
  ImageIO::addProvider(IMAGE_IO_DEVICE_BOTH, provider);
}

#endif // FOG_OS_WINDOWS
