// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Guard]
#include <Fog/Core/C++/Base.h>
#if defined(FOG_HAVE_LIBJPEG)

// [Dependencies]
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Memory/MemBufferTmp_p.h>
#include <Fog/Core/OS/Library.h>
#include <Fog/Core/Tools/InternedString.h>
#include <Fog/Core/Tools/Logger.h>
#include <Fog/Core/Tools/Stream.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/Var.h>
#include <Fog/G2d/Imaging/Codecs/JpegCodec_p.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/ImageConverter.h>

#include <stdio.h>
#include <string.h>
#include <setjmp.h>

FOG_IMPLEMENT_OBJECT(Fog::JpegDecoder)
FOG_IMPLEMENT_OBJECT(Fog::JpegEncoder)

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
# define FOG_JPEG_RGB24_RMASK 0x000000FF
# define FOG_JPEG_RGB24_GMASK 0x0000FF00
# define FOG_JPEG_RGB24_BMASK 0x00FF0000
#else
# define FOG_JPEG_RGB24_RMASK 0x00FF0000
# define FOG_JPEG_RGB24_GMASK 0x0000FF00
# define FOG_JPEG_RGB24_BMASK 0x000000FF
#endif // FOG_BYTE_ORDER

namespace Fog {

// ===========================================================================
// [Fog::JpegLibrary]
// ===========================================================================

JpegLibrary::JpegLibrary() : err(0xFFFFFFFF)
{
}

JpegLibrary::~JpegLibrary()
{
  close();
}

err_t JpegLibrary::prepare()
{
  if (err == 0xFFFFFFFF)
  {
    FOG_ONCE_LOCK();
    if (err == 0xFFFFFFFF) err = init();
    FOG_ONCE_UNLOCK();
  }

  return err;
}

err_t JpegLibrary::init()
{
  static const char symbols[] =
    "jpeg_write_scanlines\0"
    "jpeg_read_scanlines\0"
    "jpeg_set_defaults\0"
    "jpeg_set_quality\0"
    "jpeg_std_error\0"
    "jpeg_read_header\0"
    "jpeg_calc_output_dimensions\0"
    "jpeg_start_compress\0"
    "jpeg_start_decompress\0"
    "jpeg_CreateCompress\0"     // <- DIFFERENT NAMING CONVENTION!
    "jpeg_CreateDecompress\0"   // <- DIFFERENT NAMING CONVENTION!
    "jpeg_finish_compress\0"
    "jpeg_finish_decompress\0"
    "jpeg_resync_to_restart\0"
    "jpeg_destroy_compress\0"
    "jpeg_destroy_decompress\0";

  if (dll.openLibrary(StringW::fromAscii8("jpeg")) != ERR_OK)
  {
    // No JPEG library found.
    return ERR_IMAGE_LIBJPEG_NOT_LOADED;
  }

  const char* badSymbol;
  if (dll.getSymbols(addr, symbols, FOG_ARRAY_SIZE(symbols), NUM_SYMBOLS, (char**)&badSymbol) != NUM_SYMBOLS)
  {
    // Some symbol failed to load? Inform about it.
    Logger::error("Fog::JpegLibrary", "init", "Can't load symbol '%s'.", badSymbol);
    dll.close();
    return ERR_IMAGE_LIBJPEG_NOT_LOADED;
  }

  return ERR_OK;
}

void JpegLibrary::close()
{
  dll.close();
  err = 0xFFFFFFFF;
}

// ===========================================================================
// [Fog::JpegCodecProvider]
// ===========================================================================

JpegCodecProvider::JpegCodecProvider()
{
  // Name of ImageCodecProvider.
  _name = FOG_S(JPEG);

  // Supported codecs.
  _codecType = IMAGE_CODEC_BOTH;

  // Supported streams.
  _streamType = IMAGE_STREAM_JPEG;

  // Supported extensions.
  _imageExtensions.reserve(4);
  _imageExtensions.append(FOG_S(jpg));
  _imageExtensions.append(FOG_S(jpeg));
  _imageExtensions.append(FOG_S(jfi));
  _imageExtensions.append(FOG_S(jfif));
}

JpegCodecProvider::~JpegCodecProvider()
{
}

uint32_t JpegCodecProvider::checkSignature(const void* mem, size_t length) const
{
  if (!mem || length == 0) return 0;

  // Mime data.
  static const uint8_t mimeJPEG[2] = { 0xFF, 0xD8 };

  // JPEG check.
  size_t i = Math::min<size_t>(length, 2);
  if (memcmp(mem, mimeJPEG, i) == 0)
    return 15 + ((uint32_t)i * 40);

  return 0;
}

err_t JpegCodecProvider::createCodec(uint32_t codecType, ImageCodec** codec) const
{
  ImageCodec* c = NULL;

  err_t err = const_cast<JpegCodecProvider*>(this)->_jpegLibrary.prepare();
  if (FOG_IS_ERROR(err)) return err;

  switch (codecType)
  {
    case IMAGE_CODEC_DECODER:
      c = fog_new JpegDecoder(const_cast<JpegCodecProvider*>(this));
      break;
    case IMAGE_CODEC_ENCODER:
      c = fog_new JpegEncoder(const_cast<JpegCodecProvider*>(this));
      break;
    default:
      return ERR_RT_INVALID_ARGUMENT;
  }

  if (FOG_IS_NULL(c)) return ERR_RT_OUT_OF_MEMORY;

  *codec = c;
  return ERR_OK;
}

// ===========================================================================
// [Fog::JpegDecoder - Construction / Destruction]
// ===========================================================================

JpegDecoder::JpegDecoder(ImageCodecProvider* provider) :
  ImageDecoder(provider)
{
}

JpegDecoder::~JpegDecoder()
{
}

// ===========================================================================
// [Fog::JpegDecoder - Helpers]
// ===========================================================================

#define INPUT_BUFFER_SIZE 4096

struct MyJpegSourceMgr
{
  struct jpeg_source_mgr pub;
  Stream* stream;
  uint8_t buffer[INPUT_BUFFER_SIZE];
};

// Initialize source
// called by jpeg_read_header before any data is actually read.
static void FOG_CDECL MyJpegInitSource(j_decompress_ptr cinfo)
{
  // We don't actually need to do anything...
  return;
}

static boolean FOG_CDECL MyJpegFillInputBuffer(j_decompress_ptr cinfo)
{
  MyJpegSourceMgr* src = (MyJpegSourceMgr*)cinfo->src;
  size_t nbytes;

  nbytes = src->stream->read(src->buffer, INPUT_BUFFER_SIZE);
  if (nbytes <= 0)
  {
    // insert a fake EOI marker
    src->buffer[0] = (JOCTET)0xFF;
    src->buffer[1] = (JOCTET)JPEG_EOI;
    nbytes = 2;
  }

  src->pub.next_input_byte = src->buffer;
  src->pub.bytes_in_buffer = nbytes;

  return true;
}

// Skip data
// used to skip over a potentially large amount of uninteresting data (such
// as an APPn marker).
//
// Writers of suspendable-input applications must note that skip_input_data
// is not granted the right to give a suspension return.  If the skip extends
// beyond the data currently in the buffer, the buffer can be marked empty so
// that the next read will cause a fill_input_buffer call that can suspend.
// Arranging for additional bytes to be discarded before reloading the input
// buffer is the application writer's problem.
static void FOG_CDECL MyJpegSkipInputData(j_decompress_ptr cinfo, long num_bytes)
{
  MyJpegSourceMgr* src = (MyJpegSourceMgr*) cinfo->src;
  ssize_t remain = (ssize_t)( (uint8_t *)src->buffer + INPUT_BUFFER_SIZE - (uint8_t*)src->pub.next_input_byte );

  if (num_bytes < remain)
  {
    src->pub.next_input_byte += num_bytes;
    src->pub.bytes_in_buffer -= num_bytes;
  }
  else
  {
    src->stream->seek(num_bytes - remain, STREAM_SEEK_CUR);

    size_t nbytes = src->stream->read(src->buffer, INPUT_BUFFER_SIZE);
    src->pub.next_input_byte = src->buffer;
    src->pub.bytes_in_buffer = nbytes;
  }
}

// Terminate source
// called by jpeg_finish_decompress after all data has been read.
static void FOG_CDECL MyJpegTermSource(j_decompress_ptr cinfo)
{
  // we don't actually need to do anything
  return;
}

struct MyJpegErrorMgr
{
  struct jpeg_error_mgr errmgr;
  jmp_buf escape;
};

static void FOG_CDECL MyJpegErrorExit(j_common_ptr cinfo)
{
  MyJpegErrorMgr* err = (MyJpegErrorMgr*)cinfo->err;
  longjmp(err->escape, 1);
}

static void FOG_CDECL MyJpegMessage(j_common_ptr cinfo)
{
  FOG_UNUSED(cinfo);
}

// ===========================================================================
// [Fog::JpegDecoder - Reset]
// ===========================================================================

void JpegDecoder::reset()
{
  ImageDecoder::reset();
}

// ===========================================================================
// [Fog::JpegDecoder - ReadHeader]
// ===========================================================================

err_t JpegDecoder::readHeader()
{
  JpegLibrary& jpeg = reinterpret_cast<JpegCodecProvider*>(_provider)->_jpegLibrary;
  FOG_ASSERT(jpeg.err == ERR_OK);

  // Don't read header more than once.
  if (isHeaderDone()) return getHeaderResult();

  err_t err = ERR_OK;

  struct jpeg_decompress_struct cinfo;
  MyJpegSourceMgr srcmgr;
  MyJpegErrorMgr jerr;

  // create a decompression structure and load the header
  cinfo.err = jpeg.std_error(&jerr.errmgr);
  jerr.errmgr.error_exit = MyJpegErrorExit;
  jerr.errmgr.output_message = MyJpegMessage;

  if (setjmp(jerr.escape))
  {
    err = ERR_IMAGE_LIBJPEG_ERROR;
    goto _Fail;
  }

  jpeg.create_decompress(&cinfo, /* version */ 62, sizeof(struct jpeg_decompress_struct));

  cinfo.src = (struct jpeg_source_mgr *)&srcmgr;
  srcmgr.pub.init_source = MyJpegInitSource;
  srcmgr.pub.fill_input_buffer = MyJpegFillInputBuffer;
  srcmgr.pub.skip_input_data = MyJpegSkipInputData;
  srcmgr.pub.resync_to_restart = jpeg.resync_to_restart;
  srcmgr.pub.term_source = MyJpegTermSource;
  srcmgr.pub.next_input_byte = srcmgr.buffer;
  srcmgr.pub.bytes_in_buffer = 0;
  srcmgr.stream = &_stream;

  jpeg.read_header(&cinfo, true);
  jpeg.calc_output_dimensions(&cinfo);

  _headerDone = true;
  _size.w = cinfo.output_width;
  _size.h = cinfo.output_height;
  _planes = 1;
  _actualFrame = 0;
  _framesCount = 1;

  switch (cinfo.out_color_space)
  {
    case JCS_GRAYSCALE:
      _depth = 8;
      break;
    default:
      _depth = 24;
      break;
  }

  // Check whether the image size is valid.
  if (!checkImageSize())
  {
    err = ERR_IMAGE_INVALID_SIZE;
    goto _Fail;
  }

_Fail:
  jpeg.destroy_decompress(&cinfo);
  return (_headerResult = err);
}

// ===========================================================================
// [Fog::JpegDecoder - ReadImage]
// ===========================================================================

err_t JpegDecoder::readImage(Image& image)
{
  JpegLibrary& jpeg = reinterpret_cast<JpegCodecProvider*>(_provider)->_jpegLibrary;
  FOG_ASSERT(jpeg.err == ERR_OK);

  err_t err = ERR_OK;

  struct jpeg_decompress_struct cinfo;
  MyJpegSourceMgr srcmgr;
  MyJpegErrorMgr jerr;
  JSAMPROW rowptr[1];

  uint32_t format = IMAGE_FORMAT_RGB24;
  int bpp = 3;

  // Create a decompression structure and load the header.
  cinfo.err = jpeg.std_error(&jerr.errmgr);
  jerr.errmgr.error_exit = MyJpegErrorExit;
  jerr.errmgr.output_message = MyJpegMessage;

  if (setjmp(jerr.escape))
  {
    // Error condition.
    jpeg.destroy_decompress(&cinfo);
    return ERR_IMAGE_LIBJPEG_ERROR;
  }

  jpeg.create_decompress(&cinfo, JPEG_LIB_VERSION, sizeof(struct jpeg_decompress_struct));

  cinfo.src = (struct jpeg_source_mgr *)&srcmgr;
  srcmgr.pub.init_source = MyJpegInitSource;
  srcmgr.pub.fill_input_buffer = MyJpegFillInputBuffer;
  srcmgr.pub.skip_input_data = MyJpegSkipInputData;
  srcmgr.pub.resync_to_restart = jpeg.resync_to_restart;
  srcmgr.pub.term_source = MyJpegTermSource;
  srcmgr.pub.next_input_byte = srcmgr.buffer;
  srcmgr.pub.bytes_in_buffer = 0;
  srcmgr.stream = &_stream;

  jpeg.read_header(&cinfo, true);
  jpeg.calc_output_dimensions(&cinfo);

  _size.w = cinfo.output_width;
  _size.h = cinfo.output_height;
  _planes = 1;
  _actualFrame = 0;
  _framesCount = 1;

  // Check whether the image size is valid.
  if (!checkImageSize())
  {
    err = ERR_IMAGE_INVALID_SIZE;
    goto _End;
  }

  jpeg.start_decompress(&cinfo);

  // Set 8 or 24-bit output.
  if (cinfo.out_color_space == JCS_GRAYSCALE)
  {
    if (cinfo.output_components != 1)
    {
      err = ERR_IMAGEIO_UNSUPPORTED_FORMAT;
      goto _End;
    }

    format = IMAGE_FORMAT_I8;
    bpp = 1;
  }
  else if (cinfo.out_color_space == JCS_RGB)
  {
    if (cinfo.output_components != 3)
    {
      err = ERR_IMAGEIO_UNSUPPORTED_FORMAT;
      goto _End;
    }
  }
  else
  {
    cinfo.out_color_space = JCS_RGB;
    cinfo.quantize_colors = false;
  }

  // Create the image.
  if (FOG_IS_ERROR(err = image.create(_size, format))) goto _End;

  if (format == IMAGE_FORMAT_I8)
  {
    image.setPalette(ImagePalette::fromGreyscale(256));

    uint8_t* pixels = image.getFirstX();
    ssize_t stride = image.getStride();

    while (cinfo.output_scanline < cinfo.output_height)
    {
      rowptr[0] = (JSAMPROW)(pixels + (ssize_t)cinfo.output_scanline * stride);
      jpeg.read_scanlines(&cinfo, rowptr, (JDIMENSION)1);

      if ((cinfo.output_scanline & 15) == 0)
        updateProgress(cinfo.output_scanline, cinfo.output_height);
    }
  }
  else
  {
    ImageConverter converter;
    err = converter.create(
      ImageFormatDescription::getByFormat(format),
      ImageFormatDescription::fromArgb(24, IMAGE_FD_NONE, 0,
        FOG_JPEG_RGB24_RMASK,
        FOG_JPEG_RGB24_GMASK,
        FOG_JPEG_RGB24_BMASK));
    if (FOG_IS_ERROR(err)) goto _End;

    ImageConverterClosure closure;
    converter.setupClosure(&closure, PointI(0, 0));

    uint8_t* pixels = image.getFirstX();
    ssize_t stride = image.getStride();

    while (cinfo.output_scanline < cinfo.output_height)
    {
      rowptr[0] = (JSAMPROW)(pixels + (ssize_t)cinfo.output_scanline * stride);
      jpeg.read_scanlines(&cinfo, rowptr, (JDIMENSION)1);

      if (!converter.isCopy())
        converter.getBlitFn()((uint8_t*)rowptr[0], (uint8_t*)rowptr[0], _size.w, &closure);

      if ((cinfo.output_scanline & 15) == 0)
        updateProgress(cinfo.output_scanline, cinfo.output_height);

      closure.ditherOrigin.y++;
    }
  }

  jpeg.finish_decompress(&cinfo);

_End:
  jpeg.destroy_decompress(&cinfo);
  image._modified();

  return err;
}

// ===========================================================================
// [Fog::JpegEncoder - Construction / Destruction]
// ===========================================================================

JpegEncoder::JpegEncoder(ImageCodecProvider* provider) :
  ImageEncoder(provider),
  _quality(90)
{
}

JpegEncoder::~JpegEncoder()
{
}

// ===========================================================================
// [Fog::JpegEncoder - Helpers]
// ===========================================================================

#define OUTPUT_BUF_SIZE 4096

struct MyJpegDestMgr
{
  struct jpeg_destination_mgr pub;

  Stream* stream;
  uint8_t buffer[OUTPUT_BUF_SIZE * sizeof(JOCTET)];
};

static void FOG_CDECL MyJpegInitDestination(j_compress_ptr cinfo)
{
  // We don't actually need to do anything
  return;
}

static boolean FOG_CDECL MyJpegEmptyOutputBuffer(j_compress_ptr cinfo)
{
  MyJpegDestMgr* dest = (MyJpegDestMgr*) cinfo->dest;

  if (dest->stream->write(dest->buffer, OUTPUT_BUF_SIZE) != OUTPUT_BUF_SIZE)
  {
    return false;
  }
  else
  {
    dest->pub.next_output_byte = dest->buffer;
    dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;

    return true;
  }
}

static void FOG_CDECL MyJpegTermDestination(j_compress_ptr cinfo)
{
  MyJpegDestMgr* dest = (MyJpegDestMgr*) cinfo->dest;
  size_t count = OUTPUT_BUF_SIZE - dest->pub.free_in_buffer;

  if (count > 0)
  {
    if (dest->stream->write(dest->buffer, count) != count)
    {
    }
  }
}

// ===========================================================================
// [Fog::JpegEncoder - Reset]
// ===========================================================================

void JpegEncoder::reset()
{
  ImageEncoder::reset();

  // Reset also quality settings.
  _quality = 90;
}

// ===========================================================================
// [Fog::JpegEncoder - WriteImage]
// ===========================================================================

err_t JpegEncoder::writeImage(const Image& image)
{
  JpegLibrary& jpeg = reinterpret_cast<JpegCodecProvider*>(_provider)->_jpegLibrary;
  FOG_ASSERT(jpeg.err == ERR_OK);

  err_t err = ERR_OK;

  const uint8_t* pixels = image.getFirst();
  ssize_t stride = image.getStride();
  uint32_t format = image.getFormat();
  int w = image.getWidth();
  int h = image.getHeight();

  ImageConverter converter;
  MemBufferTmp<2048> buffer;

  // This struct contains the JPEG compression parameters and pointers to
  // working space (which is allocated as needed by the JPEG library).
  // It is possible to have several such structures, representing multiple
  // compression/decompression processes, in existence at once.  We refer
  // to any one struct (and its associated working data) as a "JPEG object".
  struct jpeg_compress_struct cinfo;

  // This struct represents a JPEG error handler.  It is declared separately
  // because applications often want to supply a specialized error handler
  // (see the second half of this file for an example).  But here we just
  // take the easy way out and use the standard error handler, which will
  // print a message on stderr and call exit() if compression fails.
  // Note that this struct must live as long as the main JPEG parameter
  // struct, to avoid dangling-pointer problems.
  MyJpegErrorMgr jerr;
  jerr.errmgr.error_exit = MyJpegErrorExit;
  jerr.errmgr.output_message = MyJpegMessage;

  // Destination manager.
  MyJpegDestMgr destmgr;

  // Pointer to JSAMPLE row[s].
  JSAMPROW row[1];

  if (setjmp(jerr.escape))
  {
    err = ERR_IMAGE_LIBJPEG_ERROR;
    goto _End;
  }

  // Step 0: Simple reject.
  if (!w || !h) return ERR_IMAGE_INVALID_SIZE;

  // Step 1: Allocate and initialize JPEG compression object.

  // We have to set up the error handler first, in case the initialization
  // step fails.  (Unlikely, but it could happen if you are out of memory.)
  // This routine fills in the contents of struct jerr, and returns jerr's
  // address which we place into the link field in cinfo.
  cinfo.err = jpeg.std_error((jpeg_error_mgr *)&jerr);
  // Now we can initialize the JPEG compression object.
  jpeg.create_compress(&cinfo, 62, sizeof(cinfo));

  // Step 2: Specify data destination (eg, a file).
  cinfo.dest = (jpeg_destination_mgr*)&destmgr;
  destmgr.pub.next_output_byte = (JOCTET*)destmgr.buffer;
  destmgr.pub.free_in_buffer = OUTPUT_BUF_SIZE;
  destmgr.pub.init_destination = MyJpegInitDestination;
  destmgr.pub.empty_output_buffer = MyJpegEmptyOutputBuffer;
  destmgr.pub.term_destination = MyJpegTermDestination;
  destmgr.stream = &_stream;

  // Step 3: Set parameters for compression.

  // First we supply a description of the input image.
  // Four fields of the cinfo struct must be filled in:
  cinfo.image_width = w;                 // Image width in pixels.
  cinfo.image_height = h;                // Image height in pixels.

  // JSAMPLEs per row in image_buffer.
  if (format == IMAGE_FORMAT_A8)
  {
    cinfo.input_components = 1;          // Count of color components per pixel.
    cinfo.in_color_space = JCS_GRAYSCALE;// Colorspace of input image.
  }
  else
  {
    cinfo.input_components = 3;          // Count of color components per pixel.
    cinfo.in_color_space = JCS_RGB;      // Colorspace of input image.

    if (FOG_IS_NULL(buffer.alloc(w * 3)))
    {
      err = ERR_RT_OUT_OF_MEMORY;
      goto _End;
    }

    err = converter.create(
      ImageFormatDescription::fromArgb(24, IMAGE_FD_NONE,
        0,
        FOG_JPEG_RGB24_RMASK,
        FOG_JPEG_RGB24_GMASK,
        FOG_JPEG_RGB24_BMASK),
      ImageFormatDescription::getByFormat(format),
      0, NULL, &image._d->palette);
    if (FOG_IS_ERROR(err)) goto _End;
  }

  // Now use the library's routine to set default compression parameters (You
  // must set at least cinfo.in_color_space before calling this, since the
  // defaults depend on the source color space).
  jpeg.set_defaults(&cinfo);

  // Now you can set any non-default parameters you wish to.
  // Here we just illustrate the use of quality (quantization table) scaling:
  jpeg.set_quality(&cinfo, _quality, true); // Limit to baseline-JPEG values.

  // This idea is from enlightenment. Make pixel UV sampling 1x1 if quality
  // is high (90 and higher).
  if (_quality >= 90)
  {
    cinfo.comp_info[0].h_samp_factor = 1;
    cinfo.comp_info[0].v_samp_factor = 1;
    cinfo.comp_info[1].h_samp_factor = 1;
    cinfo.comp_info[1].v_samp_factor = 1;
    cinfo.comp_info[2].h_samp_factor = 1;
    cinfo.comp_info[2].v_samp_factor = 1;
  }

  // Step 4: Start compressor.

  // true ensures that we will write a complete interchange-JPEG file.
  // Pass true unless you are very sure of what you're doing.
  jpeg.start_compress(&cinfo, true);

  // Step 5: While (scan lines remain to be written).

  // Here we use the library's state variable cinfo.next_scanline as the
  // loop counter, so that we don't have to keep track ourselves.
  // To keep things simple, we pass one scanline per call; you can pass
  // more if you wish, though.

  // jpeg_write_scanlines expects an array of pointers to scanlines.
  // Here the array is only one element long, but you could pass
  // more than one scanline at a time if that's more convenient.
  row[0] = (JSAMPLE*)buffer.getMem();

  if (converter.isValid())
  {
    ImageConverterClosure closure;
    ImageConverterBlitLineFunc blit;

    converter.setupClosure(&closure);
    blit = converter.getBlitFn();

    while (cinfo.next_scanline < cinfo.image_height)
    {
      blit((uint8_t*)buffer.getMem(),
        pixels + (ssize_t)cinfo.next_scanline * stride, w, &closure);
      jpeg.write_scanlines(&cinfo, row, 1);

      if (cinfo.next_scanline & 15)
        updateProgress(cinfo.next_scanline, cinfo.image_height);
    }
  }
  else
  {
    while (cinfo.next_scanline < cinfo.image_height)
    {
      row[0] = (JSAMPLE*)(pixels + (ssize_t)cinfo.next_scanline * stride);
      jpeg.write_scanlines(&cinfo, row, 1);

      if (cinfo.next_scanline & 15)
        updateProgress(cinfo.next_scanline, cinfo.image_height);
    }
  }

  // Step 6: Finish compression.
  jpeg.finish_compress(&cinfo);

_End:
  // Step 7: Release JPEG compression object.

  // This is an important step since it will release a good deal of memory.
  jpeg.destroy_compress(&cinfo);

  return err;
}

// ===========================================================================
// [Fog::JpegEncoder - Properties]
// ===========================================================================

err_t JpegEncoder::_getProperty(const InternedStringW& name, Var& dst) const
{
  if (name == FOG_S(quality))
    return dst.setInt(_quality);

  return Base::_getProperty(name, dst);
}

err_t JpegEncoder::_setProperty(const InternedStringW& name, const Var& src)
{
  if (name == FOG_S(quality))
    return src.getInt(_quality, 0, 100);

  return Base::_setProperty(name, src);
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void ImageCodecProvider_initJPEG(void)
{
  ImageCodecProvider* provider = fog_new JpegCodecProvider();
  ImageCodecProvider::addProvider(provider);
  provider->deref();
}

} // Fog namespace

// [Guard]
#endif // FOG_HAVE_LIBJPEG
