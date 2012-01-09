// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_CODECS_JPEGCODEC_P_H
#define _FOG_G2D_IMAGING_CODECS_JPEGCODEC_P_H

#include <Fog/Core/C++/Base.h>
#if defined FOG_HAVE_LIBJPEG

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/OS/Library.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/ImageCodec.h>
#include <Fog/G2d/Imaging/ImageCodecProvider.h>
#include <Fog/G2d/Imaging/ImageDecoder.h>
#include <Fog/G2d/Imaging/ImageEncoder.h>

#include <jpeglib.h>
#include <jerror.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ===========================================================================
// [Fog::JpegLibrary]
// ===========================================================================

//! @internal
struct FOG_NO_EXPORT JpegLibrary
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
      boolean (FOG_CDECL *resync_to_restart)(jpeg_decompress_struct*, int);
      void (FOG_CDECL *destroy_compress)(jpeg_compress_struct*);
      void (FOG_CDECL *destroy_decompress)(jpeg_decompress_struct*);
    };
    void* addr[NUM_SYMBOLS];
  };

  Library dll;
  volatile err_t err;

private:
  _FOG_NO_COPY(JpegLibrary)
};

// ===========================================================================
// [Fog::JpegCodecProvider]
// ===========================================================================

//! @internal
struct FOG_NO_EXPORT JpegCodecProvider : public ImageCodecProvider
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  JpegCodecProvider();
  virtual ~JpegCodecProvider();

  // --------------------------------------------------------------------------
  // [Implementation]
  // --------------------------------------------------------------------------

  virtual uint32_t checkSignature(const void* mem, size_t length) const;
  virtual err_t createCodec(uint32_t codecType, ImageCodec** codec) const;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  JpegLibrary _jpegLibrary;
};

// ============================================================================
// [Fog::JpegDecoder]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT JpegDecoder : public ImageDecoder
{
  FOG_DECLARE_OBJECT(JpegDecoder, ImageDecoder)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  JpegDecoder(ImageCodecProvider* provider);
  virtual ~JpegDecoder();

  // --------------------------------------------------------------------------
  // [Implementation]
  // --------------------------------------------------------------------------

  virtual void reset();
  virtual err_t readHeader();
  virtual err_t readImage(Image& image);
};

// ============================================================================
// [Fog::JpegEncoder]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT JpegEncoder : public ImageEncoder
{
  FOG_DECLARE_OBJECT(JpegEncoder, ImageEncoder)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  JpegEncoder(ImageCodecProvider* provider);
  virtual ~JpegEncoder();

  // --------------------------------------------------------------------------
  // [Implementation]
  // --------------------------------------------------------------------------

  virtual void reset();
  virtual err_t writeImage(const Image& image);

  // --------------------------------------------------------------------------
  // [Properties]
  // --------------------------------------------------------------------------

  virtual err_t _getProperty(const ManagedStringW& name, Var& dst) const;
  virtual err_t _setProperty(const ManagedStringW& name, const Var& src);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

protected:
  int _quality;
};

//! @}

} // Fog namespace

// [Guard]
#endif // FOG_HAVE_LIBJPEG
#endif // _FOG_G2D_IMAGING_CODECS_JPEGCODEC_P_H
