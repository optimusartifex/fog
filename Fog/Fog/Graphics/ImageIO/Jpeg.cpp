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

#if defined(FOG_HAVE_LIBJPEG)

#include <Fog/Core/Constants.h>
#include <Fog/Core/Library.h>
#include <Fog/Core/ManagedString.h>
#include <Fog/Core/Misc.h>
#include <Fog/Core/Static.h>
#include <Fog/Core/Stream.h>
#include <Fog/Core/String.h>
#include <Fog/Core/Strings.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Image.h>
#include <Fog/Graphics/ImageIO.h>
#include <Fog/Graphics/ImageIO/Jpeg_p.h>
#include <Fog/Graphics/RasterEngine_p.h>

#include <stdio.h>
#include <string.h>
#include <setjmp.h>

namespace Fog {
namespace ImageIO {

// ===========================================================================
// [Fog::ImageIO::JpegLibrary]
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

  if (dll.open(Ascii8("jpeg")) != ERR_OK)
  {
    // No JPEG library found.
    return ERR_IMAGEIO_LIBJPEG_NOT_LOADED;
  }

  const char* badSymbol;
  if (dll.getSymbols(addr, symbols, FOG_ARRAY_SIZE(symbols), NUM_SYMBOLS, (char**)&badSymbol) != NUM_SYMBOLS)
  {
    // Some symbol failed to load? Inform about it.
    fog_debug("Fog::ImageIO::JpegLibrary::init() - Can't load symbol '%s'.", badSymbol);
    dll.close();
    return ERR_IMAGEIO_LIBJPEG_NOT_LOADED;
  }

  return ERR_OK;
}

void JpegLibrary::close()
{
  dll.close();
  err = 0xFFFFFFFF;
}

// ===========================================================================
// [Fog::ImageIO::JpegProvider]
// ===========================================================================

JpegProvider::JpegProvider()
{
  // Name of ImageIO Provider.
  _name = fog_strings->getString(STR_GRAPHICS_JPEG);

  // File type.
  _fileType = IMAGE_IO_FILE_JPEG;

  // Supported devices.
  _deviceType = IMAGE_IO_DEVICE_BOTH;

  // Supported extensions.
  _imageExtensions.reserve(4);
  _imageExtensions.append(fog_strings->getString(STR_GRAPHICS_jpg));
  _imageExtensions.append(fog_strings->getString(STR_GRAPHICS_jpeg));
  _imageExtensions.append(fog_strings->getString(STR_GRAPHICS_jfi));
  _imageExtensions.append(fog_strings->getString(STR_GRAPHICS_jfif));
}

JpegProvider::~JpegProvider()
{
}

uint32_t JpegProvider::checkSignature(const void* mem, sysuint_t length) const
{
  if (!mem || length == 0) return 0;

  // Mime data.
  static const uint8_t mimeJPEG[2] = { 0xFF, 0xD8 };

  // JPEG check.
  sysuint_t i = Math::min<sysuint_t>(length, 2);
  if (memcmp(mem, mimeJPEG, i) == 0)
    return 15 + ((uint32_t)i * 40);

  return 0;
}

err_t JpegProvider::createDevice(uint32_t deviceType, BaseDevice** device) const
{
  BaseDevice* d = NULL;

  err_t err = const_cast<JpegProvider*>(this)->_jpegLibrary.prepare();
  if (err) return err;

  switch (deviceType)
  {
    case IMAGE_IO_DEVICE_DECODER:
      d = new(std::nothrow) JpegDecoderDevice(const_cast<JpegProvider*>(this));
      break;
    case IMAGE_IO_DEVICE_ENCODER:
      d = new(std::nothrow) JpegEncoderDevice(const_cast<JpegProvider*>(this));
      break;
    default:
      return ERR_RT_INVALID_ARGUMENT;
  }

  if (!d) return ERR_RT_OUT_OF_MEMORY;

  *device = d;
  return ERR_OK;
}

// ===========================================================================
// [Fog::ImageIO::JpegDecoderDevice]
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
static FOG_CDECL void MyJpegInitSource(j_decompress_ptr cinfo)
{
  // We don't actually need to do anything...
  return;
}

static FOG_CDECL int MyJpegFillInputBuffer(j_decompress_ptr cinfo)
{
  MyJpegSourceMgr* src = (MyJpegSourceMgr*)cinfo->src;
  sysuint_t nbytes;

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
static FOG_CDECL void MyJpegSkipInputData(j_decompress_ptr cinfo, long num_bytes)
{
  MyJpegSourceMgr* src = (MyJpegSourceMgr*) cinfo->src;
  sysint_t remain = (sysint_t)( (uint8_t *)src->buffer + INPUT_BUFFER_SIZE - (uint8_t*)src->pub.next_input_byte );

  if (num_bytes < remain)
  {
    src->pub.next_input_byte += num_bytes;
    src->pub.bytes_in_buffer -= num_bytes;
  }
  else
  {
    src->stream->seek(num_bytes - remain, STREAM_SEEK_CUR);

    sysuint_t nbytes = src->stream->read(src->buffer, INPUT_BUFFER_SIZE);
    src->pub.next_input_byte = src->buffer;
    src->pub.bytes_in_buffer = nbytes;
  }
}

// Terminate source
// called by jpeg_finish_decompress after all data has been read.
static FOG_CDECL void MyJpegTermSource(j_decompress_ptr cinfo)
{
  // we don't actually need to do anything
  return;
}

struct MyJpegErrorMgr
{
  struct jpeg_error_mgr errmgr;
  jmp_buf escape;
};

static FOG_CDECL void MyJpegErrorExit(j_common_ptr cinfo)
{
  MyJpegErrorMgr* err = (MyJpegErrorMgr*)cinfo->err;
  longjmp(err->escape, 1);
}

static FOG_CDECL void MyJpegMessage(j_common_ptr cinfo)
{
  FOG_UNUSED(cinfo);
}

JpegDecoderDevice::JpegDecoderDevice(Provider* provider) :
  DecoderDevice(provider)
{
}

JpegDecoderDevice::~JpegDecoderDevice()
{
}

// ===========================================================================
// [Fog::ImageIO::JpegDecoderDevice::reset]
// ===========================================================================

void JpegDecoderDevice::reset()
{
  DecoderDevice::reset();
}

// ===========================================================================
// [Fog::ImageIO::JpegDecoderDevice::readHeader]
// ===========================================================================

err_t JpegDecoderDevice::readHeader()
{
  JpegLibrary& jpeg = reinterpret_cast<JpegProvider*>(_provider)->_jpegLibrary;
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
    err = ERR_IMAGEIO_LIBJPEG_ERROR;
    goto fail;
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
  _width = cinfo.output_width;
  _height = cinfo.output_height;
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
    goto fail;
  }

fail:
  jpeg.destroy_decompress(&cinfo);
  return (_headerResult = err);
}

// ===========================================================================
// [Fog::ImageIO::JpegDecoderDevice::readImage]
// ===========================================================================

err_t JpegDecoderDevice::readImage(Image& image)
{
  JpegLibrary& jpeg = reinterpret_cast<JpegProvider*>(_provider)->_jpegLibrary;
  FOG_ASSERT(jpeg.err == ERR_OK);

  err_t err = ERR_OK;

  struct jpeg_decompress_struct cinfo;
  MyJpegSourceMgr srcmgr;
  MyJpegErrorMgr jerr;
  JSAMPROW rowptr[1];

  int format = PIXEL_FORMAT_XRGB32;
  int bpp = 3;

  LocalBuffer<1024> bufferStorage;
  uint8_t* buffer = NULL;

  // Create a decompression structure and load the header.
  cinfo.err = jpeg.std_error(&jerr.errmgr);
  jerr.errmgr.error_exit = MyJpegErrorExit;
  jerr.errmgr.output_message = MyJpegMessage;

  if (setjmp(jerr.escape))
  {
    // error
    jpeg.destroy_decompress(&cinfo);
    return ERR_IMAGEIO_LIBJPEG_ERROR;
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

  _width = cinfo.output_width;
  _height = cinfo.output_height;
  _planes = 1;
  _actualFrame = 0;
  _framesCount = 1;

  // Check whether the image size is valid.
  if (!checkImageSize())
  {
    err = ERR_IMAGE_INVALID_SIZE;
    goto end;
  }

  jpeg.start_decompress(&cinfo);

  // Set 8 or 24-bit output.
  if (cinfo.out_color_space == JCS_GRAYSCALE)
  {
    if (cinfo.output_components != 1)
    {
      err = ERR_IMAGEIO_UNSUPPORTED_FORMAT;
      goto end;
    }
    format = PIXEL_FORMAT_I8;
    bpp = 1;
  }
  else if (cinfo.out_color_space == JCS_RGB)
  {
    if (cinfo.output_components != 3)
    {
      err = ERR_IMAGEIO_UNSUPPORTED_FORMAT;
      goto end;
    }
  }
  else
  {
    cinfo.out_color_space = JCS_RGB;
    cinfo.quantize_colors = false;
  }

  // Resize our image to jpeg size.
  if ((err = image.create(_width, _height, format))) goto end;
  if (format == PIXEL_FORMAT_I8) image.setPalette(Palette::greyscale());

  if (format == PIXEL_FORMAT_I8)
  {
    ImageData* image_d = image._d;

    // Directly load into the image buffer.
    while (cinfo.output_scanline < cinfo.output_height)
    {
      rowptr[0] = (JSAMPROW)(image_d->first + cinfo.output_scanline * image_d->stride);
      jpeg.read_scanlines(&cinfo, rowptr, (JDIMENSION)1);

      if ((cinfo.output_scanline & 15) == 0)
        updateProgress(cinfo.output_scanline, cinfo.output_height);
    }
  }
  else
  {
    buffer = reinterpret_cast<uint8_t*>(bufferStorage.alloc(_width * bpp));
    if (buffer == NULL) { err = ERR_RT_OUT_OF_MEMORY; goto end; }

    // Load to temporary buffer and convert data to an image compatible format.
    rowptr[0] = (JSAMPROW)buffer;

    while (cinfo.output_scanline < cinfo.output_height)
    {
      jpeg.read_scanlines(&cinfo, rowptr, (JDIMENSION)1);
      image.setDib(0, cinfo.output_scanline, _width, DIB_FORMAT_RGB24_BE, buffer);

      if ((cinfo.output_scanline & 15) == 0)
        updateProgress(cinfo.output_scanline, cinfo.output_height);
    }
  }

  jpeg.finish_decompress(&cinfo);

end:
  jpeg.destroy_decompress(&cinfo);
  return err;
}

// ===========================================================================
// [Fog::ImageIO::JpegEncoderDevice]
// ===========================================================================

#define OUTPUT_BUF_SIZE 4096

struct MyJpegDestMgr
{
  struct jpeg_destination_mgr pub;

  Stream* stream;
  uint8_t buffer[OUTPUT_BUF_SIZE * sizeof(JOCTET)];
};

static FOG_CDECL void MyJpegInitDestination(j_compress_ptr cinfo)
{
  // We don't actually need to do anything
  return;
}

static int MyJpegEmptyOutputBuffer(j_compress_ptr cinfo)
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
  sysuint_t count = OUTPUT_BUF_SIZE - dest->pub.free_in_buffer;

  if (count > 0)
  {
    if (dest->stream->write(dest->buffer, count) != count)
    {
    }
  }
}

JpegEncoderDevice::JpegEncoderDevice(Provider* provider) :
  EncoderDevice(provider),
  _quality(90)
{
}

JpegEncoderDevice::~JpegEncoderDevice()
{
}

// ===========================================================================
// [Fog::ImageIO::JpegEncoderDevice::reset]
// ===========================================================================

void JpegEncoderDevice::reset()
{
  EncoderDevice::reset();

  // Reset also quality settings.
  _quality = 90;
}

// ===========================================================================
// [Fog::ImageIO::JpegEncoderDevice::writeImage]
// ===========================================================================

err_t JpegEncoderDevice::writeImage(const Image& image)
{
  JpegLibrary& jpeg = reinterpret_cast<JpegProvider*>(_provider)->_jpegLibrary;
  FOG_ASSERT(jpeg.err == ERR_OK);

  LocalBuffer<4096> bufferLocal;
  uint8_t* buffer;
  sysint_t bufferStride;

  err_t err = ERR_OK;

  ImageData* d = image._d;
  int width = d->width;
  int height = d->height;
  int format = d->format;

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

  // destination manager
  MyJpegDestMgr destmgr;

  // pointer to JSAMPLE row[s]
  JSAMPROW row[1];

  if (setjmp(jerr.escape))
  {
    err = ERR_IMAGEIO_LIBJPEG_ERROR;
    goto end;
  }

  // Step 0: Simple reject
  if (!width || !height) return ERR_IMAGE_INVALID_SIZE;

  // Step 1: allocate and initialize JPEG compression object

  // We have to set up the error handler first, in case the initialization
  // step fails.  (Unlikely, but it could happen if you are out of memory.)
  // This routine fills in the contents of struct jerr, and returns jerr's
  // address which we place into the link field in cinfo.
  cinfo.err = jpeg.std_error((jpeg_error_mgr *)&jerr);
  // Now we can initialize the JPEG compression object.
  jpeg.create_compress(&cinfo, 62, sizeof(cinfo));

  // Step 2: specify data destination (eg, a file)
  cinfo.dest = (jpeg_destination_mgr*)&destmgr;
  destmgr.pub.next_output_byte = (JOCTET*)destmgr.buffer;
  destmgr.pub.free_in_buffer = OUTPUT_BUF_SIZE;
  destmgr.pub.init_destination = MyJpegInitDestination;
  destmgr.pub.empty_output_buffer = MyJpegEmptyOutputBuffer;
  destmgr.pub.term_destination = MyJpegTermDestination;
  destmgr.stream = &_stream;

  // Step 3: set parameters for compression

  // First we supply a description of the input image.
  // Four fields of the cinfo struct must be filled in:
  cinfo.image_width = width;           // image width in pixels
  cinfo.image_height = height;         // image height in pixels

  // JSAMPLEs per row in image_buffer
  if (format == PIXEL_FORMAT_A8)
  {
    cinfo.input_components = 1;          // # of color components per pixel
    cinfo.in_color_space = JCS_GRAYSCALE;// colorspace of input image

    bufferStride = width;
    buffer = (uint8_t*)bufferLocal.alloc(bufferStride);
  }
  else
  {
    cinfo.input_components = 3;          // # of color components per pixel
    cinfo.in_color_space = JCS_RGB;      // colorspace of input image

    bufferStride = width * 3;
    buffer = (uint8_t*)bufferLocal.alloc(bufferStride);
  }

  if (!buffer) { err = ERR_RT_OUT_OF_MEMORY; goto end; }

  // Now use the library's routine to set default compression parameters.
  // (You must set at least cinfo.in_color_space before calling this,
  // since the defaults depend on the source color space.)
  jpeg.set_defaults(&cinfo);

  // Now you can set any non-default parameters you wish to.
  // Here we just illustrate the use of quality (quantization table) scaling:
  jpeg.set_quality(&cinfo, _quality, true /* limit to baseline-JPEG values */);

  // This idea is from enlightenment. Make pixel UV sampling 1x1 if quality
  // is high (90 and higher)
  if (_quality >= 90)
  {
    cinfo.comp_info[0].h_samp_factor = 1;
    cinfo.comp_info[0].v_samp_factor = 1;
    cinfo.comp_info[1].h_samp_factor = 1;
    cinfo.comp_info[1].v_samp_factor = 1;
    cinfo.comp_info[2].h_samp_factor = 1;
    cinfo.comp_info[2].v_samp_factor = 1;
  }

  // Step 4: Start compressor

  // true ensures that we will write a complete interchange-JPEG file.
  // Pass true unless you are very sure of what you're doing.
  jpeg.start_compress(&cinfo, true);

  // Step 5: while (scan lines remain to be written)

  // Here we use the library's state variable cinfo.next_scanline as the
  // loop counter, so that we don't have to keep track ourselves.
  // To keep things simple, we pass one scanline per call; you can pass
  // more if you wish, though.

  // jpeg_write_scanlines expects an array of pointers to scanlines.
  // Here the array is only one element long, but you could pass
  // more than one scanline at a time if that's more convenient.
  row[0] = buffer;

  while (cinfo.next_scanline < cinfo.image_height)
  {
    if (format == PIXEL_FORMAT_A8)
    {
      row[0] = (JSAMPLE*)image.getScanline(cinfo.next_scanline);
    }
    else
    {
      image.getDib(0, cinfo.next_scanline, width, DIB_FORMAT_RGB24_BE, buffer);
    }

    jpeg.write_scanlines(&cinfo, row, 1);

    if (cinfo.next_scanline & 15)
      updateProgress(cinfo.next_scanline, cinfo.image_height);
  }

  // Step 6: Finish compression

  jpeg.finish_compress(&cinfo);

end:
  // Step 7: release JPEG compression object

  // This is an important step since it will release a good deal of memory.
  jpeg.destroy_compress(&cinfo);

  return err;
}

// ===========================================================================
// [Fog::ImageIO::JpegEncoderDevice::getProperty / setProperty]
// ===========================================================================

err_t JpegEncoderDevice::getProperty(const ManagedString& name, Value& value) const
{
  if (name == fog_strings->getString(STR_GRAPHICS_quality)) return value.setInt32(_quality);

  return base::getProperty(name, value);
}

err_t JpegEncoderDevice::setProperty(const ManagedString& name, const Value& value)
{
  int i;
  err_t err;

  if (name == fog_strings->getString(STR_GRAPHICS_quality))
  {
    if ((err = value.getInt32(&i))) return err;
    _quality = Math::bound(i, 0, 100);
    return ERR_OK;
  }

  return base::setProperty(name, value);
}

} // ImageIO namespace
} // Fog namespace

FOG_IMPLEMENT_OBJECT(Fog::ImageIO::JpegDecoderDevice)
FOG_IMPLEMENT_OBJECT(Fog::ImageIO::JpegEncoderDevice)

// ===========================================================================
// [CAPI]
// ===========================================================================

FOG_INIT_DECLARE void fog_imageio_init_jpeg(void)
{
  using namespace Fog;

  ImageIO::JpegProvider* provider = new(std::nothrow) ImageIO::JpegProvider();
  ImageIO::addProvider(IMAGE_IO_DEVICE_BOTH, provider);
}

#else

FOG_INIT_DECLARE void fog_imageio_init_jpeg(void) {}

#endif // FOG_HAVE_LIBJPEG
