// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_IMAGEIO_JPEG_P_H
#define _FOG_GRAPHICS_IMAGEIO_JPEG_P_H

// [Dependencies]
#include <Fog/Core/Build.h>

#if defined FOG_HAVE_LIBJPEG

#include <Fog/Core/Constants.h>
#include <Fog/Core/Library.h>
#include <Fog/Core/Stream.h>
#include <Fog/Core/String.h>
#include <Fog/Graphics/Image.h>
#include <Fog/Graphics/ImageIO.h>

#include <jpeglib.h>
#include <jerror.h>

namespace Fog {
namespace ImageIO {

//! @addtogroup Fog_Graphics_Private
//! @{

// ===========================================================================
// [Fog::ImageIO::JpegLibrary]
// ===========================================================================

//! @internal
struct FOG_HIDDEN JpegLibrary
{
  JpegLibrary();
  ~JpegLibrary();

  err_t prepare();
  err_t init();
  void close();

  enum { NUM_SYMBOLS = 16 };
  union
  {
    struct
    {
      FOG_CDECL JDIMENSION (*write_scanlines)(jpeg_compress_struct*, uint8_t**, unsigned int);
      FOG_CDECL JDIMENSION (*read_scanlines)(jpeg_decompress_struct*, uint8_t**, unsigned int);
      FOG_CDECL void (*set_defaults)(jpeg_compress_struct*);
      FOG_CDECL void (*set_quality)(jpeg_compress_struct*, int /* quality */, int /* force_baseline*/);
      FOG_CDECL struct jpeg_error_mgr* (*std_error)(jpeg_error_mgr*);
      FOG_CDECL int (*read_header)(jpeg_decompress_struct*, int);
      FOG_CDECL void (*calc_output_dimensions)(jpeg_decompress_struct*);
      FOG_CDECL int (*start_compress)(jpeg_compress_struct*, int);
      FOG_CDECL int (*start_decompress)(jpeg_decompress_struct*);
      FOG_CDECL JDIMENSION (*create_compress)(jpeg_compress_struct*, int, size_t);
      FOG_CDECL JDIMENSION (*create_decompress)(jpeg_decompress_struct*, int, size_t);
      FOG_CDECL int (*finish_compress)(jpeg_compress_struct*);
      FOG_CDECL int (*finish_decompress)(jpeg_decompress_struct*);
      FOG_CDECL int (*resync_to_restart)(jpeg_decompress_struct*, int);
      FOG_CDECL void (*destroy_compress)(jpeg_compress_struct*);
      FOG_CDECL void (*destroy_decompress)(jpeg_decompress_struct*);
    };
    void* addr[NUM_SYMBOLS];
  };

  Library dll;
  volatile err_t err;

private:
  FOG_DISABLE_COPY(JpegLibrary)
};

// ===========================================================================
// [Fog::ImageIO::JpegProvider]
// ===========================================================================

//! @internal
struct FOG_HIDDEN JpegProvider : public Provider
{
  JpegProvider();
  virtual ~JpegProvider();

  virtual uint32_t checkSignature(const void* mem, sysuint_t length) const;
  virtual err_t createDevice(uint32_t deviceType, BaseDevice** device) const;

  JpegLibrary _jpegLibrary;
};

// ============================================================================
// [Fog::ImageIO::JpegDecoderDevice]
// ============================================================================

//! @internal
struct FOG_HIDDEN JpegDecoderDevice : public DecoderDevice
{
  FOG_DECLARE_OBJECT(JpegDecoderDevice, DecoderDevice)

  // [Construction / Destruction]

  JpegDecoderDevice(Provider* provider);
  virtual ~JpegDecoderDevice();

  // [Virtuals]

  virtual void reset();
  virtual err_t readHeader();
  virtual err_t readImage(Image& image);
};

// ============================================================================
// [Fog::ImageIO::JpegEncoderDevice]
// ============================================================================

//! @internal
struct FOG_HIDDEN JpegEncoderDevice : public EncoderDevice
{
  FOG_DECLARE_OBJECT(JpegEncoderDevice, EncoderDevice)

  // [Construction / Destruction]

  JpegEncoderDevice(Provider* provider);
  virtual ~JpegEncoderDevice();

  // [Virtuals]

  virtual void reset();
  virtual err_t writeImage(const Image& image);

  // [Properties]

  virtual err_t getProperty(const ManagedString& name, Value& value) const;
  virtual err_t setProperty(const ManagedString& name, const Value& value);

private:
  int _quality;
};

//! @}

} // ImageIO namespace
} // Fog namespace

#endif // FOG_HAVE_LIBJPEG

// [Guard]
#endif // _FOG_GRAPHICS_IMAGEIO_JPEG_P_H
