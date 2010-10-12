// [Fog-Graphics]
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
      JDIMENSION (FOG_CDECL *write_scanlines)(jpeg_compress_struct*, uint8_t**, unsigned int);
      JDIMENSION (FOG_CDECL *read_scanlines)(jpeg_decompress_struct*, uint8_t**, unsigned int);
      void (FOG_CDECL *set_defaults)(jpeg_compress_struct*);
      void (FOG_CDECL *set_quality)(jpeg_compress_struct*, int /* quality */, int /* force_baseline*/);
      struct jpeg_error_mgr* (FOG_CDECL *std_error)(jpeg_error_mgr*);
      int (FOG_CDECL *read_header)(jpeg_decompress_struct*, int);
      void (FOG_CDECL *calc_output_dimensions)(jpeg_decompress_struct*);
      int (FOG_CDECL *start_compress)(jpeg_compress_struct*, int);
      int (FOG_CDECL *start_decompress)(jpeg_decompress_struct*);
      JDIMENSION (FOG_CDECL *create_compress)(jpeg_compress_struct*, int, size_t);
      JDIMENSION (FOG_CDECL *create_decompress)(jpeg_decompress_struct*, int, size_t);
      int (FOG_CDECL *finish_compress)(jpeg_compress_struct*);
      int (FOG_CDECL *finish_decompress)(jpeg_decompress_struct*);
      int (FOG_CDECL *resync_to_restart)(jpeg_decompress_struct*, int);
      void (FOG_CDECL *destroy_compress)(jpeg_compress_struct*);
      void (FOG_CDECL *destroy_decompress)(jpeg_decompress_struct*);
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
