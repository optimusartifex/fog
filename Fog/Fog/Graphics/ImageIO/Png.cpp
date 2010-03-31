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

#if defined(FOG_HAVE_LIBPNG)

#include <Fog/Core/Constants.h>
#include <Fog/Core/Library.h>
#include <Fog/Core/ManagedString.h>
#include <Fog/Core/Math.h>
#include <Fog/Core/Misc.h>
#include <Fog/Core/Static.h>
#include <Fog/Core/Stream.h>
#include <Fog/Core/String.h>
#include <Fog/Core/Strings.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Image.h>
#include <Fog/Graphics/ImageIO.h>
#include <Fog/Graphics/ImageIO/Png_p.h>
#include <Fog/Graphics/RasterEngine_p.h>

#include <setjmp.h>
#include <stdio.h>
#include <string.h>

namespace Fog {
namespace ImageIO {

// ============================================================================
// [Fog::ImageIO::PngLibrary]
// ============================================================================

PngLibrary::PngLibrary() : err(0xFFFFFFFF)
{
}

PngLibrary::~PngLibrary()
{
  close();
}

err_t PngLibrary::prepare()
{
  if (err == 0xFFFFFFFF)
  {
    FOG_ONCE_LOCK();
    if (err == 0xFFFFFFFF) err = init();
    FOG_ONCE_UNLOCK();
  }

  return err;
}

err_t PngLibrary::init()
{
  static const char symbols[] =
    "png_create_read_struct\0"
    "png_destroy_read_struct\0"
    "png_create_info_struct\0"
    "png_read_info\0"
    "png_read_rows\0"
    "png_read_image\0"
    "png_read_end\0"
    "png_create_write_struct\0"
    "png_destroy_write_struct\0"
    "png_write_info\0"
    "png_write_rows\0"
    "png_write_end\0"
    "png_set_expand_gray_1_2_4_to_8\0"
    "png_set_gray_to_rgb\0"
    "png_set_strip_16\0"
    "png_set_swap_alpha\0"
    "png_set_filler\0"
    "png_set_packing\0"
    "png_set_packswap\0"
    "png_set_shift\0"
    "png_set_error_fn\0"
    "png_set_read_fn\0"
    "png_set_write_fn\0"
    "png_set_bgr\0"
    "png_set_expand\0"
    "png_set_interlace_handling\0"
    "png_set_compression_level\0"
    "png_set_IHDR\0"
    "png_set_PLTE\0"
    "png_set_sBIT\0"
    "png_get_valid\0"
    "png_get_bit_depth\0"
    "png_get_IHDR\0"
    "png_error\0";

  if (dll.open(Ascii8("png")) != ERR_OK)
  {
    // No PNG library found.
    return ERR_IMAGEIO_LIBPNG_NOT_LOADED;
  }

  const char* badSymbol;
  if (dll.getSymbols(addr, symbols, FOG_ARRAY_SIZE(symbols), NUM_SYMBOLS, (char**)&badSymbol) != NUM_SYMBOLS)
  {
    // Some symbol failed to load? Inform about it.
    fog_debug("Fog::ImageIO::PngLibrary::init() - Can't load symbol '%s'.", badSymbol);
    dll.close();
    return ERR_IMAGEIO_LIBPNG_NOT_LOADED;
  }

  return ERR_OK;
}

void PngLibrary::close()
{
  dll.close();
  err = 0xFFFFFFFF;
}

// ============================================================================
// [Fog::ImageIO::PngProvider]
// ============================================================================

PngProvider::PngProvider()
{
  // Name of ImageIO Provider.
  _name = fog_strings->getString(STR_GRAPHICS_PNG);

  // File type.
  _fileType = IMAGE_IO_FILE_PNG;

  // Supported devices.
  _deviceType = IMAGE_IO_DEVICE_BOTH;

  // Supported extensions.
  _imageExtensions.reserve(1);
  _imageExtensions.append(fog_strings->getString(STR_GRAPHICS_png));
}

PngProvider::~PngProvider()
{
}

uint32_t PngProvider::checkSignature(const void* mem, sysuint_t length) const
{
  if (!mem || length == 0) return 0;

  // Mime data.
  static const uint8_t mimePNG[8] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };

  // PNG check.
  sysuint_t i = Math::min<sysuint_t>(length, 8);
  if (memcmp(mem, mimePNG, i) == 0) 
    return 15 + ((uint32_t)i * 10);

  return 0;
}

err_t PngProvider::createDevice(uint32_t deviceType, BaseDevice** device) const
{
  BaseDevice* d = NULL;

  err_t err = const_cast<PngProvider*>(this)->_pngLibrary.prepare();
  if (err) return err;

  switch (deviceType)
  {
    case IMAGE_IO_DEVICE_DECODER:
      d = new(std::nothrow) PngDecoderDevice(const_cast<PngProvider*>(this));
      break;
    case IMAGE_IO_DEVICE_ENCODER:
      d = new(std::nothrow) PngEncoderDevice(const_cast<PngProvider*>(this));
      break;
    default:
      return ERR_RT_INVALID_ARGUMENT;
  }

  if (!d) return ERR_RT_OUT_OF_MEMORY;

  *device = d;
  return ERR_OK;
}

// ============================================================================
// [Fog::ImageIO::Png Helpers]
// ============================================================================

static void png_user_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
  PngDecoderDevice* device = reinterpret_cast<PngDecoderDevice*>(png_ptr->io_ptr);

  if (device->getStream().read(data, length) != length)
  {
    PngLibrary& png = reinterpret_cast<PngProvider*>(device->getProvider())->_pngLibrary;
    png.error(png_ptr, "Read Error");
  }
}

static void png_user_write_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
  PngEncoderDevice* device = reinterpret_cast<PngEncoderDevice*>(png_ptr->io_ptr);

  if (device->getStream().write(data, length) != length)
  {
    PngLibrary& png = reinterpret_cast<PngProvider*>(device->getProvider())->_pngLibrary;
    png.error(png_ptr, "Erite Error");
  }
}

static void user_flush_data(png_structp png_ptr)
{
}

static void user_error_fn(png_structp png_ptr, png_const_charp error_msg)
{
  longjmp(png_ptr->jmpbuf, 1);
}

// ============================================================================
// [Fog::ImageIO::PngDecoderDevice]
// ============================================================================

PngDecoderDevice::PngDecoderDevice(Provider* provider) :
  DecoderDevice(provider),
  _png_ptr(NULL),
  _info_ptr(NULL)
{
}

PngDecoderDevice::~PngDecoderDevice()
{
  _deletePngStream();
}

// ============================================================================
// [Fog::ImageIO::PngDecoderDevice::reset]
// ============================================================================

void PngDecoderDevice::reset()
{
  _deletePngStream();
  DecoderDevice::reset();
}

// ============================================================================
// [Fog::ImageIO::PngDecoderDevice::readHeader]
// ============================================================================

err_t PngDecoderDevice::readHeader()
{
  // Png library pointer.
  PngLibrary& png = reinterpret_cast<PngProvider*>(_provider)->_pngLibrary;
  FOG_ASSERT(png.err == ERR_OK);

  // Don't read header more than once.
  if (isHeaderDone()) return _headerResult;

  // Mark header as done.
  _headerDone = true;

  png_uint_32 w32, h32;

  if ((_headerResult = _createPngStream()) != ERR_OK)
  {
    return _headerResult;
  }

  if (setjmp(((png_structp)_png_ptr)->jmpbuf))
  {
    return (_headerResult = ERR_IMAGEIO_LIBPNG_ERROR);
  }

  png.read_info(_png_ptr, _info_ptr);
  png.get_IHDR(_png_ptr, _info_ptr,
    (png_uint_32 *)(&w32),
    (png_uint_32 *)(&h32),
    &_png_bit_depth,
    &_png_color_type,
    &_png_interlace_type, NULL, NULL);

  _width = w32;
  _height = h32;
  _depth = _png_bit_depth;
  _planes = 1;

  // Check whether the image size is valid.
  if (!checkImageSize())
  {
    return (_headerResult = ERR_IMAGE_INVALID_SIZE);
  }

  // Png contains only one image.
  _actualFrame = 0;
  _framesCount = 1;

  if (_info_ptr->color_type == PNG_COLOR_TYPE_PALETTE)
  {
    _format = PIXEL_FORMAT_I8;
  }
  else if (
    _info_ptr->color_type == PNG_COLOR_TYPE_RGB_ALPHA ||
    _info_ptr->color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
  {
    _format = PIXEL_FORMAT_ARGB32;
  }
  else
  {
    _format = PIXEL_FORMAT_XRGB32;
  }

  // Success.
  return (_headerResult = ERR_OK);
}

// ============================================================================
// [Fog::ImageIO::PngDecoderDevice::readImage]
// ============================================================================

err_t PngDecoderDevice::readImage(Image& image)
{
  // Png library pointer.
  PngLibrary& png = reinterpret_cast<PngProvider*>(_provider)->_pngLibrary;
  FOG_ASSERT(png.err == ERR_OK);

  // Read png header.
  if (readHeader() != ERR_OK) return _headerResult;

  // Don't read image more than once.
  if (isReaderDone()) return (_readerResult = ERR_IMAGEIO_NO_MORE_FRAMES);

  // Error code (default is success).
  uint32_t err = ERR_OK;

  // Variables.
  png_uint_32 i;
  bool hasAlpha = false;
  bool hasGrey = false;

  if (setjmp(_png_ptr->jmpbuf))
  {
    return ERR_IMAGEIO_LIBPNG_ERROR;
  }

  // Change the order of packed pixels to least significant bit first.
  png.set_packswap(_png_ptr);

  // Tell libpng to strip 16 bit/color files down to 8 bits/color.
  png.set_strip_16(_png_ptr);

  if (_info_ptr->color_type == PNG_COLOR_TYPE_RGB_ALPHA)
  {
    hasAlpha = true;
  }

  if (_info_ptr->color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
  {
    hasAlpha = true;
    hasGrey = true;
  }

  if (_info_ptr->color_type == PNG_COLOR_TYPE_GRAY)
  {
    hasGrey = true;
  }

  if (hasAlpha) png.set_expand(_png_ptr);

  // Extract multiple pixels with bit depths of 1, 2, and 4 from a single
  // byte into separate bytes (useful for paletted and grayscale images).
  png.set_packing(_png_ptr);

  if (_info_ptr->color_type != PNG_COLOR_TYPE_PALETTE)
  {
    // We want ARGB.
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
    png.set_bgr(_png_ptr);
    png.set_filler(_png_ptr, 0xFF, PNG_FILLER_AFTER);
#else
    png.set_swap_alpha(_png_ptr);
    png.set_filler(_png_ptr, 0xFF, PNG_FILLER_BEFORE);
#endif // FOG_BYTE_ORDER

    if (png.get_valid(_png_ptr, _info_ptr, PNG_INFO_tRNS))
    {
      png.set_expand(_png_ptr);
    }

    if (hasGrey)
    {
      png.set_gray_to_rgb(_png_ptr);
      if (png.get_bit_depth(_png_ptr, _info_ptr) < 8)
      {
        png.set_expand_gray_1_2_4_to_8(_png_ptr);
      }
    }
  }

  if ((err = image.create(_width, _height, _format))) goto end;

  {
    int pass, number_passes = png.set_interlace_handling(_png_ptr);
    int y = 0;
    int yi = 0;
    int ytotal = number_passes * _height;

    for (pass = 0; pass < number_passes; pass++)
    {
      for (i = 0; i < _height; i++, y++, yi++)
      {
        uint8_t* row = image._d->first + i * image.getStride();
        png.read_rows(_png_ptr, &row, NULL, 1);
        if ((yi & 15) == 0) updateProgress(yi, ytotal);
      }
    }
  }

  // TODO: PNG - Read palette.

end:
  png.read_end(_png_ptr, _info_ptr);

  updateProgress(1.0);
  return err;
}

uint32_t PngDecoderDevice::_createPngStream()
{
  // Already created?
  if (_png_ptr) return ERR_OK;

  // Png library pointer,
  PngLibrary& png = reinterpret_cast<PngProvider*>(_provider)->_pngLibrary;

  // Should be checked earlier.
  FOG_ASSERT(png.err == ERR_OK);

  // Create png structure.
  if ((_png_ptr = png.create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL)) == NULL)
  {
    return ERR_IMAGEIO_LIBPNG_ERROR;
  }

  // Create info structure.
  if ((_info_ptr = png.create_info_struct((png_structp)_png_ptr)) == NULL)
  {
    goto fail;
  }

  // Custom IO.
  png.set_read_fn((png_structp)_png_ptr, this, (png_rw_ptr)png_user_read_data);

  // Success.
  return ERR_OK;

fail:
  _deletePngStream();
  return ERR_IMAGEIO_LIBPNG_ERROR;
}

void PngDecoderDevice::_deletePngStream()
{
  if (_png_ptr)
  {
    // Png library pointer.
    PngLibrary& png = reinterpret_cast<PngProvider*>(_provider)->_pngLibrary;
    // Should be checked earlier.
    FOG_ASSERT(png.err == ERR_OK);

    png.destroy_read_struct(&_png_ptr, &_info_ptr, (png_infopp)NULL);
  }
}

// ============================================================================
// [Fog::ImageIO::PngEncoderDevice]
// ============================================================================

PngEncoderDevice::PngEncoderDevice(Provider* provider) :
  EncoderDevice(provider),
  _compression(9)
{
}

PngEncoderDevice::~PngEncoderDevice()
{
}

// ===========================================================================
// [Fog::ImageIO::PngEncoderDevice::writeImage]
// ===========================================================================

err_t PngEncoderDevice::writeImage(const Image& image)
{
  // Png library pointer.
  PngLibrary& png = reinterpret_cast<PngProvider*>(_provider)->_pngLibrary;
  FOG_ASSERT(png.err == ERR_OK);

  uint32_t err = ERR_OK;

  int  format = image.getFormat();

  LocalBuffer<2048> bufferStorage;
  uint8_t* buffer = NULL;

  png_structp  png_ptr;
  png_infop    info_ptr;
  int          width = image.getWidth();
  int          height = image.getHeight();
  int          y;
  png_bytep    row_ptr;

  // Step 0: Simple reject
  if (!width || !height)
  {
    err = ERR_IMAGE_INVALID_SIZE;
    goto end;
  }

  if ((png_ptr = png.create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL)) == NULL)
  {
    err = ERR_IMAGEIO_LIBPNG_ERROR;
    goto end;
  }

  if ((info_ptr = png.create_info_struct(png_ptr)) == NULL)
  {
    err = ERR_IMAGEIO_LIBPNG_ERROR;
    goto end_destroy_write_struct;
  }

  if (setjmp(png_ptr->jmpbuf))
  {
    err = ERR_IMAGEIO_LIBPNG_ERROR;
    goto end_destroy_write_struct;
  }

  // Use custom I/O functions.
  png.set_write_fn(png_ptr, this, (png_rw_ptr)png_user_write_data, (png_flush_ptr)user_flush_data);

  png_color_8 sig_bit;
  memset(&sig_bit, 0, sizeof(sig_bit));

  // PNG doesn't support alpha channel in palette (only RGB values) so we must
  // use different pixel format.
  if (format == PIXEL_FORMAT_I8 && image.getPalette().isAlphaUsed())
  {
    format = PIXEL_FORMAT_ARGB32;
  }

  switch (format)
  {
    case PIXEL_FORMAT_ARGB32:
    case PIXEL_FORMAT_PRGB32:
    case PIXEL_FORMAT_A8:
    {
      png.set_IHDR(png_ptr, info_ptr, width, height, 8,
        PNG_COLOR_TYPE_RGB_ALPHA, png_ptr->interlaced,
        PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
      png.set_bgr(png_ptr);
#else
      png.set_swap_alpha(png_ptr);
#endif

      sig_bit.red = 8;
      sig_bit.green = 8;
      sig_bit.blue = 8;
      sig_bit.alpha = 8;
      sig_bit.gray = 0;
      png.set_sBIT(png_ptr, info_ptr, &sig_bit);

      buffer = (uint8_t*)bufferStorage.alloc(width * 4);
      if (!buffer) { err = ERR_RT_OUT_OF_MEMORY; goto end; }
      break;
    }
    case PIXEL_FORMAT_XRGB32:
    {
      png.set_IHDR(png_ptr, info_ptr, width, height, 8,
        PNG_COLOR_TYPE_RGB, png_ptr->interlaced,
        PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

      sig_bit.red = 8;
      sig_bit.green = 8;
      sig_bit.blue = 8;
      sig_bit.alpha = 0;
      sig_bit.gray = 0;
      png.set_sBIT(png_ptr, info_ptr, &sig_bit);

      buffer = (uint8_t*)bufferStorage.alloc(width * 3);
      if (!buffer) { err = ERR_RT_OUT_OF_MEMORY; goto end; }
      break;
    }

    case PIXEL_FORMAT_I8:
    {
      const Argb* ipal = image.getPalette().getData();
      png_color entries[256];
      uint32_t numEntries = 256;

      png.set_IHDR(png_ptr, info_ptr, width, height, 8,
        PNG_COLOR_TYPE_PALETTE, png_ptr->interlaced,
        PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

      for (uint32_t i = 0; i < numEntries; i++)
      {
        entries[i].red   = ipal[i].r;
        entries[i].green = ipal[i].g;
        entries[i].blue  = ipal[i].b;
      }

      png.set_PLTE(png_ptr, info_ptr, entries, numEntries);
      break;
    }
  }

  png.set_compression_level(png_ptr, _compression);
  png.write_info(png_ptr, info_ptr);
  png.set_shift(png_ptr, &sig_bit);
  png.set_packing(png_ptr);

  row_ptr = (png_bytep)buffer;

  for (y = 0; y < height; y++)
  {
    switch (format)
    {
      case PIXEL_FORMAT_ARGB32:
      case PIXEL_FORMAT_PRGB32:
        image.getDib(0, y, width, DIB_FORMAT_ARGB32_NATIVE, buffer);
        break;
      case PIXEL_FORMAT_XRGB32:
        image.getDib(0, y, width, DIB_FORMAT_RGB24_BE, buffer);
        break;
      case PIXEL_FORMAT_I8:
        row_ptr = (png_bytep)image.getFirst() + y * image.getStride();
        break;
    }

    png.write_rows(png_ptr, &row_ptr, 1);

    if ((y & 15) == 0) updateProgress(y, height);
  }

  png.write_end(png_ptr, info_ptr);

end_destroy_write_struct:
  png.destroy_write_struct(&png_ptr, (png_infopp)&info_ptr);
end:
  updateProgress(1.0);
  return err;
}

// ===========================================================================
// [Fog::ImageIO::PngEncoderDevice::getProperty / setProperty]
// ===========================================================================

err_t PngEncoderDevice::getProperty(const ManagedString& name, Value& value) const
{
  if (name == fog_strings->getString(STR_GRAPHICS_compression)) return value.setInt32(_compression);

  return base::getProperty(name, value);
}

err_t PngEncoderDevice::setProperty(const ManagedString& name, const Value& value)
{
  err_t err;
  int i;

  if (name == fog_strings->getString(STR_GRAPHICS_compression))
  {
    if (err = value.getInt32(&i)) return err;
    _compression = Math::bound<int>(i, 0, 9);
    return ERR_OK;
  }
  return base::setProperty(name, value);
}

} // ImageIO namespace
} // Fog namespace

FOG_IMPLEMENT_OBJECT(Fog::ImageIO::PngDecoderDevice)
FOG_IMPLEMENT_OBJECT(Fog::ImageIO::PngEncoderDevice)

// ============================================================================
// [CAPI]
// ============================================================================

FOG_INIT_DECLARE void fog_imageio_init_png(void)
{
  using namespace Fog;

  ImageIO::PngProvider* provider = new(std::nothrow) ImageIO::PngProvider();
  ImageIO::addProvider(IMAGE_IO_DEVICE_BOTH, provider);
}

#else

FOG_INIT_DECLARE void fog_imageio_init_png(void) {}

#endif // FOG_HAVE_LIBPNG
