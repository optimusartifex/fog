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
#if defined(FOG_HAVE_LIBPNG)

// [Dependencies]
#include <Fog/Core/Collection/PBuffer.h>
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Global/Debug.h>
#include <Fog/Core/Global/Static.h>
#include <Fog/Core/IO/Stream.h>
#include <Fog/Core/Library/Library.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Tools/ManagedString.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/Strings.h>
#include <Fog/G2d/Global/Constants.h>
#include <Fog/G2d/Global/Init_p.h>
#include <Fog/G2d/Imaging/Codecs/PngCodec_p.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/ImageConverter.h>

#include <setjmp.h>
#include <stdio.h>
#include <string.h>

FOG_IMPLEMENT_OBJECT(Fog::PngDecoder)
FOG_IMPLEMENT_OBJECT(Fog::PngEncoder)

namespace Fog {

// ============================================================================
// [Statics]
// ============================================================================

// Because PNG API changed and we can't access the png struct anymore the
// png provider is needed. It's safe without deprecation warnings and with
// better binary compatibility theoretically with any libpng version.
static PngCodecProvider* pngProvider;

// ============================================================================
// [Fog::PngLibrary]
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
    "png_set_longjmp_fn\0"
    "png_set_IHDR\0"
    "png_set_PLTE\0"
    "png_set_sBIT\0"
    "png_get_bit_depth\0"
    "png_get_io_ptr\0"
    "png_get_valid\0"
    "png_get_IHDR\0"
    "png_error\0";

  if (dll.open(Ascii8("png")) != ERR_OK)
  {
    // No PNG library found.
    return ERR_IMAGE_LIBPNG_NOT_LOADED;
  }

  const char* badSymbol;
  if (dll.getSymbols(addr, symbols, FOG_ARRAY_SIZE(symbols), NUM_SYMBOLS, (char**)&badSymbol) != NUM_SYMBOLS)
  {
    // Some symbol failed to load? Inform about it.
    Debug::dbgFunc("Fog::PngLibrary", "init", "Can't load symbol '%s'.\n", badSymbol);
    dll.close();
    return ERR_IMAGE_LIBPNG_NOT_LOADED;
  }

  return ERR_OK;
}

void PngLibrary::close()
{
  dll.close();
  err = 0xFFFFFFFF;
}

// ============================================================================
// [Fog::PngCodecProvider]
// ============================================================================

PngCodecProvider::PngCodecProvider()
{
  pngProvider = this;

  // Name of ImageCodecProvider.
  _name = fog_strings->getString(STR_G2D_STREAM_PNG);

  // Supported codecs.
  _codecType = IMAGE_CODEC_BOTH;

  // Supported streams.
  _streamType = IMAGE_STREAM_PNG;

  // Supported extensions.
  _imageExtensions.reserve(1);
  _imageExtensions.append(fog_strings->getString(STR_G2D_EXTENSION_png));
}

PngCodecProvider::~PngCodecProvider()
{
  pngProvider = NULL;
}

uint32_t PngCodecProvider::checkSignature(const void* mem, sysuint_t length) const
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

err_t PngCodecProvider::createCodec(uint32_t codecType, ImageCodec** codec) const
{
  ImageCodec* c = NULL;

  err_t err = const_cast<PngCodecProvider*>(this)->_pngLibrary.prepare();
  if (FOG_IS_ERROR(err)) return err;

  switch (codecType)
  {
    case IMAGE_CODEC_DECODER:
      c = fog_new PngDecoder(const_cast<PngCodecProvider*>(this));
      break;
    case IMAGE_CODEC_ENCODER:
      c = fog_new PngEncoder(const_cast<PngCodecProvider*>(this));
      break;
    default:
      return ERR_RT_INVALID_ARGUMENT;
  }

  if (FOG_IS_NULL(c)) return ERR_RT_OUT_OF_MEMORY;

  *codec = c;
  return ERR_OK;
}

// ============================================================================
// [Fog::PngCodecProvider - Helpers]
// ============================================================================

static void png_user_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
  PngDecoder* decoder = reinterpret_cast<PngDecoder*>(
    pngProvider->_pngLibrary.get_io_ptr(png_ptr));

  if (decoder->getStream().read(data, length) != length)
  {
    PngLibrary& png = reinterpret_cast<PngCodecProvider*>(decoder->getProvider())->_pngLibrary;
    png.error(png_ptr, "Read Error");
  }
}

static void png_user_write_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
  PngEncoder* encoder = reinterpret_cast<PngEncoder*>(
    pngProvider->_pngLibrary.get_io_ptr(png_ptr));

  if (encoder->getStream().write(data, length) != length)
  {
    PngLibrary& png = reinterpret_cast<PngCodecProvider*>(encoder->getProvider())->_pngLibrary;
    png.error(png_ptr, "Write Error");
  }
}

static void png_user_flush_data(png_structp png_ptr)
{
}

// ============================================================================
// [Fog::PngDecoder - Construction / Destruction]
// ============================================================================

PngDecoder::PngDecoder(ImageCodecProvider* provider) :
  ImageDecoder(provider),
  _png_ptr(NULL),
  _info_ptr(NULL)
{
}

PngDecoder::~PngDecoder()
{
  _deletePngStream();
}

// ============================================================================
// [Fog::PngDecoder - Reset]
// ============================================================================

void PngDecoder::reset()
{
  _deletePngStream();
  ImageDecoder::reset();
}

// ============================================================================
// [Fog::PngDecoder - ReadHeader]
// ============================================================================

err_t PngDecoder::readHeader()
{
  // Png library pointer.
  PngLibrary& png = reinterpret_cast<PngCodecProvider*>(_provider)->_pngLibrary;
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

  if (setjmp(*png.set_longjmp_fn(_png_ptr, longjmp, sizeof(jmp_buf))))
  {
    return (_headerResult = ERR_IMAGE_LIBPNG_ERROR);
  }

  png.read_info(_png_ptr, _info_ptr);
  png.get_IHDR(_png_ptr, _info_ptr,
    (png_uint_32 *)(&w32),
    (png_uint_32 *)(&h32),
    &_png_bit_depth,
    &_png_color_type,
    &_png_interlace_type, NULL, NULL);

  _size.w = w32;
  _size.h = h32;
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

  if (_png_color_type == PNG_COLOR_TYPE_PALETTE)
    _format = IMAGE_FORMAT_I8;
  else if (_png_color_type == PNG_COLOR_TYPE_RGB_ALPHA || _png_color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    _format = IMAGE_FORMAT_PRGB32;
  else
    _format = IMAGE_FORMAT_XRGB32;

  // Success.
  return (_headerResult = ERR_OK);
}

// ============================================================================
// [Fog::PngDecoder - ReadImage]
// ============================================================================

err_t PngDecoder::readImage(Image& image)
{
  // Png library pointer.
  PngLibrary& png = reinterpret_cast<PngCodecProvider*>(_provider)->_pngLibrary;
  FOG_ASSERT(png.err == ERR_OK);

  // Read png header.
  if (readHeader() != ERR_OK) return _headerResult;

  // Don't read image more than once.
  if (isReaderDone()) return (_readerResult = ERR_IMAGE_NO_MORE_FRAMES);

  // Error code (default is success).
  uint32_t err = ERR_OK;

  // Variables.
  png_uint_32 i;
  bool hasAlpha = false;
  bool hasGrey = false;

  if (setjmp(*png.set_longjmp_fn((png_structp)_png_ptr, longjmp, sizeof(jmp_buf))))
  {
    return ERR_IMAGE_LIBPNG_ERROR;
  }

  // Change the order of packed pixels to least significant bit first.
  png.set_packswap(_png_ptr);

  // Tell libpng to strip 16 bit/color files down to 8 bits/color.
  png.set_strip_16(_png_ptr);

  if (_png_color_type == PNG_COLOR_TYPE_RGB_ALPHA ) hasAlpha = true;
  if (_png_color_type == PNG_COLOR_TYPE_GRAY_ALPHA) hasAlpha = true;
  if (_png_color_type == PNG_COLOR_TYPE_GRAY_ALPHA) hasGrey  = true;
  if (_png_color_type == PNG_COLOR_TYPE_GRAY      ) hasGrey  = true;

  if (hasAlpha) png.set_expand(_png_ptr);

  // Extract multiple pixels with bit depths of 1, 2, and 4 from a single
  // byte into separate bytes (useful for paletted and grayscale images).
  png.set_packing(_png_ptr);

  if (_png_color_type != PNG_COLOR_TYPE_PALETTE)
  {
    // We want ARGB.
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
    png.set_bgr(_png_ptr);
    png.set_filler(_png_ptr, 0xFF, PNG_FILLER_AFTER);
#else
    png.set_swap_alpha(_png_ptr);
    png.set_filler(_png_ptr, 0xFF, PNG_FILLER_BEFORE);
#endif // FOG_BYTE_ORDER

    if (png.get_valid(_png_ptr, _info_ptr, PNG_INFO_tRNS)) png.set_expand(_png_ptr);

    if (hasGrey)
    {
      png.set_gray_to_rgb(_png_ptr);
      if (png.get_bit_depth(_png_ptr, _info_ptr) < 8) png.set_expand_gray_1_2_4_to_8(_png_ptr);
    }
  }

  if ((err = image.create(_size, _format))) goto _End;

  {
    int pass, number_passes = png.set_interlace_handling(_png_ptr);
    int y = 0;
    int yi = 0;
    int ytotal = number_passes * _size.h;

    for (pass = 0; pass < number_passes; pass++)
    {
      for (i = 0; i < _size.h; i++, y++, yi++)
      {
        uint8_t* row = image._d->first + i * image.getStride();
        png.read_rows(_png_ptr, &row, NULL, 1);
        if ((yi & 15) == 0) updateProgress(yi, ytotal);
      }
    }
  }

  // TODO: PNG - Read palette.

_End:
  png.read_end(_png_ptr, _info_ptr);

  image._modified();
  updateProgress(1.0f);

  return err;
}

uint32_t PngDecoder::_createPngStream()
{
  // Already created?
  if (_png_ptr) return ERR_OK;

  // Png library pointer,
  PngLibrary& png = reinterpret_cast<PngCodecProvider*>(_provider)->_pngLibrary;

  // Should be checked earlier.
  FOG_ASSERT(png.err == ERR_OK);

  // Create png structure.
  if ((_png_ptr = png.create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL)) == NULL)
  {
    return ERR_IMAGE_LIBPNG_ERROR;
  }

  // Create info structure.
  if ((_info_ptr = png.create_info_struct((png_structp)_png_ptr)) == NULL)
  {
    goto _Fail;
  }

  // Custom IO.
  png.set_read_fn((png_structp)_png_ptr, this, (png_rw_ptr)png_user_read_data);

  // Success.
  return ERR_OK;

_Fail:
  _deletePngStream();
  return ERR_IMAGE_LIBPNG_ERROR;
}

void PngDecoder::_deletePngStream()
{
  if (_png_ptr)
  {
    // Png library pointer.
    PngLibrary& png = reinterpret_cast<PngCodecProvider*>(_provider)->_pngLibrary;
    // Should be checked earlier.
    FOG_ASSERT(png.err == ERR_OK);

    png.destroy_read_struct(&_png_ptr, &_info_ptr, (png_infopp)NULL);
  }
}

// ============================================================================
// [Fog::PngEncoder - Construction / Destruction]
// ============================================================================

PngEncoder::PngEncoder(ImageCodecProvider* provider) :
  ImageEncoder(provider),
  _compression(9)
{
}

PngEncoder::~PngEncoder()
{
}

// ===========================================================================
// [Fog::PngEncoder - WriteImage]
// ===========================================================================

err_t PngEncoder::writeImage(const Image& image)
{
  // Png library pointer.
  PngLibrary& png = reinterpret_cast<PngCodecProvider*>(_provider)->_pngLibrary;
  FOG_ASSERT(png.err == ERR_OK);

  err_t err = ERR_OK;

  const uint8_t* pixels = image.getFirst();
  sysint_t stride = image.getStride();
  uint32_t format = image.getFormat();
  int w = image.getWidth();
  int h = image.getHeight();

  png_structp png_ptr;
  png_infop info_ptr;
  png_bytep row_ptr;
  int y;

  ImageConverter converter;
  PBuffer<2048> buffer;

  // Step 0: Simple reject.
  if (!w || !h)
  {
    err = ERR_IMAGE_INVALID_SIZE;
    goto _End;
  }

  if ((png_ptr = png.create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL)) == NULL)
  {
    err = ERR_IMAGE_LIBPNG_ERROR;
    goto _End;
  }

  if ((info_ptr = png.create_info_struct(png_ptr)) == NULL)
  {
    err = ERR_IMAGE_LIBPNG_ERROR;
    goto _EndDestroyWriteStruct;
  }

  if (setjmp(*png.set_longjmp_fn((png_structp)png_ptr, longjmp, sizeof(jmp_buf))))
  {
    err = ERR_IMAGE_LIBPNG_ERROR;
    goto _EndDestroyWriteStruct;
  }

  // Use custom I/O functions.
  png.set_write_fn(png_ptr, this, (png_rw_ptr)png_user_write_data, (png_flush_ptr)png_user_flush_data);

  png_color_8 sig_bit;
  memset(&sig_bit, 0, sizeof(sig_bit));

  switch (format)
  {
    case IMAGE_FORMAT_PRGB32:
    case IMAGE_FORMAT_PRGB64:
    case IMAGE_FORMAT_A8:
    case IMAGE_FORMAT_A16:
    {
      png.set_IHDR(png_ptr, info_ptr, w, h, 8,
        PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
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

      err = converter.create(
        ImageFormatDescription::fromArgb(32, IMAGE_FD_NONE,
          ARGB32_AMASK,
          ARGB32_RMASK,
          ARGB32_GMASK,
          ARGB32_BMASK),
        ImageFormatDescription::getByFormat(format));
      if (FOG_IS_ERROR(err)) goto _End;

      if (FOG_IS_NULL(buffer.alloc(w * 4)))
      {
        err = ERR_RT_OUT_OF_MEMORY;
        goto _End;
      }
      break;
    }

    case IMAGE_FORMAT_XRGB32:
    case IMAGE_FORMAT_RGB24:
    case IMAGE_FORMAT_RGB48:
    {
      png.set_IHDR(png_ptr, info_ptr, w, h, 8,
        PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

      sig_bit.red = 8;
      sig_bit.green = 8;
      sig_bit.blue = 8;
      sig_bit.alpha = 0;
      sig_bit.gray = 0;
      png.set_sBIT(png_ptr, info_ptr, &sig_bit);

      err = converter.create(
        ImageFormatDescription::fromArgb(24, IMAGE_FD_NONE,
          0,
          FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN ? 0x000000FF : 0x00FF0000,
          FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN ? 0x0000FF00 : 0x0000FF00,
          FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN ? 0x00FF0000 : 0x000000FF),
        ImageFormatDescription::getByFormat(format));
      if (FOG_IS_ERROR(err)) goto _End;

      if (FOG_IS_NULL(buffer.alloc(w * 3)))
      {
        err = ERR_RT_OUT_OF_MEMORY;
        goto _End;
      }
      break;
    }

    case IMAGE_FORMAT_I8:
    {
      const Argb32* pal = image.getPalette().getData();
      uint32_t palLength = image.getPalette().getLength();;

      png.set_IHDR(png_ptr, info_ptr, w, h, 8,
        PNG_COLOR_TYPE_PALETTE, PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

      png_color entries[256];
      for (uint32_t i = 0; i < palLength; i++)
      {
        entries[i].red   = pal[i].getRed();
        entries[i].green = pal[i].getGreen();
        entries[i].blue  = pal[i].getBlue();
      }

      png.set_PLTE(png_ptr, info_ptr, entries, palLength);
      break;
    }

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  png.set_compression_level(png_ptr, _compression);
  png.write_info(png_ptr, info_ptr);
  png.set_shift(png_ptr, &sig_bit);
  png.set_packing(png_ptr);
  row_ptr = (png_bytep)buffer.getMem();

  if (converter.isValid())
  {
    ImageConverterClosure closure;
    ImageConverterBlitLineFn blit;

    converter.setupClosure(&closure);
    blit = converter.getBlitFn();

    for (y = 0; y < h; y++, pixels += stride)
    {
      blit((uint8_t*)row_ptr, pixels, w, &closure);
      png.write_rows(png_ptr, &row_ptr, 1);

      if ((y & 15) == 0) updateProgress(y, h);
    }
  }
  else
  {
    for (y = 0; y < h; y++, pixels += stride)
    {
      row_ptr = (png_bytep)pixels;
      png.write_rows(png_ptr, &row_ptr, 1);

      if ((y & 15) == 0) updateProgress(y, h);
    }
  }

  png.write_end(png_ptr, info_ptr);

_EndDestroyWriteStruct:
  png.destroy_write_struct(&png_ptr, (png_infopp)&info_ptr);

_End:
  updateProgress(1.0f);
  return err;
}

// ===========================================================================
// [Fog::PngEncoder - Properties]
// ===========================================================================

err_t PngEncoder::getProperty(const ManagedString& name, Value& value) const
{
  if (name == fog_strings->getString(STR_G2D_CODEC_compression)) return value.setInt32(_compression);

  return base::getProperty(name, value);
}

err_t PngEncoder::setProperty(const ManagedString& name, const Value& value)
{
  err_t err;
  int i;

  if (name == fog_strings->getString(STR_G2D_CODEC_compression))
  {
    if (err = value.getInt32(&i)) return err;
    _compression = Math::bound<int>(i, 0, 9);
    return ERR_OK;
  }
  return base::setProperty(name, value);
}

// ============================================================================
// [Fog::G2d - Library Initializers]
// ============================================================================

FOG_NO_EXPORT void _g2d_imagecodecprovider_init_png(void)
{
  PngCodecProvider* provider = fog_new PngCodecProvider();
  ImageCodecProvider::addProvider(IMAGE_CODEC_BOTH, provider);
}

} // Fog namespace

// [Guard]
#endif // FOG_HAVE_LIBPNG
