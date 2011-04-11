// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_CODECS_PNGCODEC_P_H
#define _FOG_G2D_IMAGING_CODECS_PNGCODEC_P_H

#include <Fog/Core/Config/Config.h>
#if defined FOG_HAVE_LIBPNG

// [Dependencies]
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/IO/Stream.h>
#include <Fog/Core/Library/Library.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/G2d/Global/Constants.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/ImageCodec.h>
#include <Fog/G2d/Imaging/ImageCodecProvider.h>
#include <Fog/G2d/Imaging/ImageDecoder.h>
#include <Fog/G2d/Imaging/ImageEncoder.h>

#include <png.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ============================================================================
// [Fog::PngLibrary]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT PngLibrary
{
  PngLibrary();
  ~PngLibrary();

  err_t prepare();
  err_t init();
  void close();

  enum { NUM_SYMBOLS = 36 };
  union
  {
    struct
    {
      png_structp (FOG_CDECL *create_read_struct)(png_const_charp user_ver, png_voidp error_ptr, png_error_ptr error_fn, png_error_ptr warn_fn);
      void (FOG_CDECL *destroy_read_struct)(png_structpp ptr_ptr, png_infopp info_ptr_ptr, png_infopp end_info_ptr_ptr);
      png_infop (FOG_CDECL *create_info_struct)(png_structp png_ptr);
      void (FOG_CDECL *read_info)(png_structp png_ptr, png_infop info_ptr);
      void (FOG_CDECL *read_rows)(png_structp png_ptr, png_bytepp row, png_bytepp display_row, png_uint_32 num_rows);
      void (FOG_CDECL *read_image)(png_structp png_ptr, png_bytepp image);
      void (FOG_CDECL *read_end)(png_structp png_ptr, png_infop info_ptr);
      png_structp (FOG_CDECL *create_write_struct)(png_const_charp user_ver, png_voidp error_ptr, png_error_ptr error_fn, png_error_ptr warn_fn);
      void (FOG_CDECL *destroy_write_struct)(png_structpp ptr_ptr, png_infopp info_ptr_ptr);
      void (FOG_CDECL *write_info)(png_structp png_ptr, png_infop info_ptr);
      void (FOG_CDECL *write_rows)(png_structp png_ptr, png_bytepp row, png_uint_32 num_rows);
      void (FOG_CDECL *write_end)(png_structp png_ptr, png_infop info_ptr);

      void (FOG_CDECL *set_expand_gray_1_2_4_to_8)(png_structp png_ptr);
      void (FOG_CDECL *set_gray_to_rgb)(png_structp png_ptr);
      void (FOG_CDECL *set_strip_16)(png_structp png_ptr);
      void (FOG_CDECL *set_swap_alpha)(png_structp png_ptr);
      void (FOG_CDECL *set_filler)(png_structp png_ptr, png_uint_32 filler, int flags);
      void (FOG_CDECL *set_packing)(png_structp png_ptr);
      void (FOG_CDECL *set_packswap)(png_structp png_ptr);
      void (FOG_CDECL *set_shift)(png_structp png_ptr, png_color_8p true_bits);
      void (FOG_CDECL *set_error_fn)(png_structp png_ptr, png_voidp error_ptr, png_error_ptr error_fn, png_error_ptr warning_fn);
      void (FOG_CDECL *set_read_fn)(png_structp png_ptr, png_voidp io_ptr, png_rw_ptr read_data_fn);
      void (FOG_CDECL *set_write_fn)(png_structp png_ptr, png_voidp io_ptr, png_rw_ptr write_data_fn, png_flush_ptr output_flush_fn);
      void (FOG_CDECL *set_bgr)(png_structp png_ptr);
      void (FOG_CDECL *set_expand)(png_structp png_ptr);
      int (FOG_CDECL *set_interlace_handling)(png_structp png_ptr);
      void (FOG_CDECL *set_compression_level)(png_structp png_ptr, int level);
      jmp_buf* (FOG_CDECL *set_longjmp_fn)(png_structp png_ptr, png_longjmp_ptr fn, size_t jmp_buf_size);
      void (FOG_CDECL *set_IHDR)(png_structp png_ptr,
        png_infop info_ptr, png_uint_32 width, png_uint_32 height, int bit_depth,
        int color_type, int interlace_method, int compression_method,
        int filter_method);
      void (FOG_CDECL *set_PLTE)(png_structp png_ptr, png_infop info_ptr, png_colorp palette, int num_palette);
      void (FOG_CDECL *set_sBIT)(png_structp png_ptr, png_infop info_ptr, png_color_8p sig_bit);

      png_byte (FOG_CDECL *get_bit_depth)(png_structp png_ptr, png_infop info_ptr);
      png_voidp (FOG_CDECL *get_io_ptr)(png_structp png_ptr);
      png_uint_32 (FOG_CDECL *get_valid)(png_structp png_ptr, png_infop info_ptr, png_uint_32 flag);
      png_uint_32 (FOG_CDECL *get_IHDR)(png_structp png_ptr,
        png_infop info_ptr, png_uint_32 *width, png_uint_32 *height,
        int *bit_depth, int *color_type, int *interlace_method,
        int *compression_method, int *filter_method);
      void (FOG_CDECL *error)(png_structp png_ptr, png_const_charp error_message);
    };
    void* addr[NUM_SYMBOLS];
  };

  Library dll;
  err_t err;

private:
  FOG_DISABLE_COPY(PngLibrary)
};

// ============================================================================
// [Fog::PngCodecProvider]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT PngCodecProvider : public ImageCodecProvider
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  PngCodecProvider();
  virtual ~PngCodecProvider();

  // --------------------------------------------------------------------------
  // [Implementation]
  // --------------------------------------------------------------------------

  virtual uint32_t checkSignature(const void* mem, sysuint_t length) const;
  virtual err_t createCodec(uint32_t codecType, ImageCodec** codec) const;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  PngLibrary _pngLibrary;
};

// ============================================================================
// [Fog::PngDecoder]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT PngDecoder : public ImageDecoder
{
  FOG_DECLARE_OBJECT(PngDecoder, ImageDecoder)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  PngDecoder(ImageCodecProvider* provider);
  virtual ~PngDecoder();

  // --------------------------------------------------------------------------
  // [Implementation]
  // --------------------------------------------------------------------------

  virtual void reset();
  virtual err_t readHeader();
  virtual err_t readImage(Image& image);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  png_structp _png_ptr;
  png_infop _info_ptr;
  int _png_bit_depth;
  int _png_color_type;
  int _png_interlace_type;

  uint32_t _createPngStream();
  void _deletePngStream();
};

// ============================================================================
// [Fog::PngEncoder]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT PngEncoder : public ImageEncoder
{
  FOG_DECLARE_OBJECT(PngEncoder, ImageEncoder)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  PngEncoder(ImageCodecProvider* provider);
  virtual ~PngEncoder();

  // --------------------------------------------------------------------------
  // [Implementation]
  // --------------------------------------------------------------------------

  virtual err_t writeImage(const Image& image);

  // --------------------------------------------------------------------------
  // [Properties]
  // --------------------------------------------------------------------------

  virtual err_t getProperty(const ManagedString& name, Value& value) const;
  virtual err_t setProperty(const ManagedString& name, const Value& value);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  int _compression;
};

//! @}

} // Fog namespace

// [Guard]
#endif // FOG_HAVE_LIBPNG
#endif // _FOG_G2D_IMAGING_CODECS_PNGCODEC_P_H
