// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_IMAGEIO_PNG_P_H
#define _FOG_GRAPHICS_IMAGEIO_PNG_P_H

// [Dependencies]
#include <Fog/Core/Build.h>

#if defined FOG_HAVE_LIBPNG

#include <Fog/Core/Constants.h>
#include <Fog/Core/Library.h>
#include <Fog/Core/Stream.h>
#include <Fog/Core/String.h>
#include <Fog/Graphics/Image.h>
#include <Fog/Graphics/ImageIO.h>

#include <png.h>

namespace Fog {
namespace ImageIO {

//! @addtogroup Fog_Graphics_Private
//! @{

// ============================================================================
// [Fog::ImageIO::PngLibrary]
// ============================================================================

//! @internal
struct FOG_HIDDEN PngLibrary
{
  PngLibrary();
  ~PngLibrary();

  err_t prepare();
  err_t init();
  void close();

  enum { NUM_SYMBOLS = 34 };
  union
  {
    struct
    {
      FOG_CDECL png_structp (*create_read_struct)(png_const_charp user_ver, png_voidp error_ptr, png_error_ptr error_fn, png_error_ptr warn_fn);
      FOG_CDECL void (*destroy_read_struct)(png_structpp ptr_ptr, png_infopp info_ptr_ptr, png_infopp end_info_ptr_ptr);
      FOG_CDECL png_infop (*create_info_struct)(png_structp png_ptr);
      FOG_CDECL void (*read_info)(png_structp png_ptr, png_infop info_ptr);
      FOG_CDECL void(*read_rows)(png_structp png_ptr, png_bytepp row, png_bytepp display_row, png_uint_32 num_rows);
      FOG_CDECL void (*read_image)(png_structp png_ptr, png_bytepp image);
      FOG_CDECL void (*read_end)(png_structp png_ptr, png_infop info_ptr);
      FOG_CDECL png_structp (*create_write_struct)(png_const_charp user_ver, png_voidp error_ptr, png_error_ptr error_fn, png_error_ptr warn_fn);
      FOG_CDECL void (*destroy_write_struct)(png_structpp ptr_ptr, png_infopp info_ptr_ptr);
      FOG_CDECL void (*write_info)(png_structp png_ptr, png_infop info_ptr);
      FOG_CDECL void (*write_rows)(png_structp png_ptr, png_bytepp row, png_uint_32 num_rows);
      FOG_CDECL void (*write_end)(png_structp png_ptr, png_infop info_ptr);
      FOG_CDECL void (*set_expand_gray_1_2_4_to_8)(png_structp png_ptr);
      FOG_CDECL void (*set_gray_to_rgb)(png_structp png_ptr);
      FOG_CDECL void (*set_strip_16)(png_structp png_ptr);
      FOG_CDECL void (*set_swap_alpha)(png_structp png_ptr);
      FOG_CDECL void (*set_filler)(png_structp png_ptr, png_uint_32 filler, int flags);
      FOG_CDECL void (*set_packing)(png_structp png_ptr);
      FOG_CDECL void (*set_packswap)(png_structp png_ptr);
      FOG_CDECL void (*set_shift)(png_structp png_ptr, png_color_8p true_bits);
      FOG_CDECL void (*set_error_fn)(png_structp png_ptr, png_voidp error_ptr, png_error_ptr error_fn, png_error_ptr warning_fn);
      FOG_CDECL void (*set_read_fn)(png_structp png_ptr, png_voidp io_ptr, png_rw_ptr read_data_fn);
      FOG_CDECL void (*set_write_fn)(png_structp png_ptr, png_voidp io_ptr, png_rw_ptr write_data_fn, png_flush_ptr output_flush_fn);
      FOG_CDECL void (*set_bgr)(png_structp png_ptr);
      FOG_CDECL void (*set_expand)(png_structp png_ptr);
      FOG_CDECL int (*set_interlace_handling)(png_structp png_ptr);
      FOG_CDECL void (*set_compression_level)(png_structp png_ptr, int level);
      FOG_CDECL void (*set_IHDR)(png_structp png_ptr,
        png_infop info_ptr, png_uint_32 width, png_uint_32 height, int bit_depth,
        int color_type, int interlace_method, int compression_method,
        int filter_method);
      FOG_CDECL void (*set_PLTE)(png_structp png_ptr, png_infop info_ptr, png_colorp palette, int num_palette);
      FOG_CDECL void (*set_sBIT)(png_structp png_ptr, png_infop info_ptr, png_color_8p sig_bit);

      FOG_CDECL png_uint_32 (*get_valid)(png_structp png_ptr, png_infop info_ptr, png_uint_32 flag);
      FOG_CDECL png_byte (*get_bit_depth)(png_structp png_ptr, png_infop info_ptr);
      FOG_CDECL png_uint_32 (*get_IHDR)(png_structp png_ptr,
        png_infop info_ptr, png_uint_32 *width, png_uint_32 *height,
        int *bit_depth, int *color_type, int *interlace_method,
        int *compression_method, int *filter_method);
      FOG_CDECL void (*error)(png_structp png_ptr, png_const_charp error_message);
    };
    void* addr[NUM_SYMBOLS];
  };

  Library dll;
  err_t err;

private:
  FOG_DISABLE_COPY(PngLibrary)
};

// ============================================================================
// [Fog::ImageIO::PngProvider]
// ============================================================================

//! @internal
struct FOG_HIDDEN PngProvider : public Provider
{
  PngProvider();
  virtual ~PngProvider();

  virtual uint32_t checkSignature(const void* mem, sysuint_t length) const;
  virtual err_t createDevice(uint32_t deviceType, BaseDevice** device) const;

  PngLibrary _pngLibrary;
};

// ============================================================================
// [Fog::ImageIO::PngDecoderDevice]
// ============================================================================

//! @internal
struct FOG_HIDDEN PngDecoderDevice : public DecoderDevice
{
  FOG_DECLARE_OBJECT(PngDecoderDevice, DecoderDevice)

  PngDecoderDevice(Provider* provider);
  virtual ~PngDecoderDevice();

  virtual void reset();
  virtual err_t readHeader();
  virtual err_t readImage(Image& image);

  png_structp _png_ptr;
  png_infop _info_ptr;
  int _png_bit_depth;
  int _png_color_type;
  int _png_interlace_type;

  uint32_t _createPngStream();
  void _deletePngStream();
};

// ============================================================================
// [Fog::ImageIO::PngEncoderDevice]
// ============================================================================

//! @internal
struct FOG_HIDDEN PngEncoderDevice : public EncoderDevice
{
  FOG_DECLARE_OBJECT(PngEncoderDevice, EncoderDevice)

  PngEncoderDevice(Provider* provider);
  virtual ~PngEncoderDevice();

  virtual err_t writeImage(const Image& image);

  // [Properties]

  virtual err_t getProperty(const ManagedString& name, Value& value) const;
  virtual err_t setProperty(const ManagedString& name, const Value& value);

  // [Members]

  int _compression;
};

//! @}

} // ImageIO namespace
} // Fog namespace

#endif // FOG_HAVE_LIBPNG

// [Guard]
#endif // _FOG_GRAPHICS_IMAGEIO_PNG_P_H
