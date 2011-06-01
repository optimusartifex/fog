// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_CODECS_BMPCODEC_P_H
#define _FOG_G2D_IMAGING_CODECS_BMPCODEC_P_H

// [Dependencies]
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/IO/Stream.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/G2d/Global/Constants.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/ImageCodec.h>
#include <Fog/G2d/Imaging/ImageCodecProvider.h>
#include <Fog/G2d/Imaging/ImageDecoder.h>
#include <Fog/G2d/Imaging/ImageEncoder.h>
#include <Fog/G2d/Imaging/ImageFormatDescription.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ============================================================================
// [Fog::BmpFileHeader]
// ============================================================================

#include <Fog/Core/Pack/PackByte.h>

//! @internal
//!
//! @brief Bitmap file header (always 14 bytes in little endian format).
//!
//! @note Size of BmpFileHeader must be 14 bytes.
struct BmpFileHeader
{
  uint8_t magic_B;          //!< @brief Bitmap mime type ('B').
  uint8_t magic_M;          //!< @brief Bitmap mime type ('M').
  uint32_t fileSize;        //!< @brief Bitmap file size in bytes.
  uint16_t reserved1;
  uint16_t reserved2;
  uint32_t imageOffset;     //!< @brief Offset to image data (54, 124, ...).
};

#include <Fog/Core/Pack/PackRestore.h>

// ============================================================================
// [Fog::BmpOS2V1Header]
// ============================================================================

#include <Fog/Core/Pack/PackByte.h>

//! @internal
//!
//! @brief Bitmap OS/2 header.
//!
//! @note This header is 12 bytes length.
struct BmpOS2V1Header
{
  uint32_t headerSize;      //!< @brief Header size (40, 52).
  uint16_t width;           //!< @brief Bitmap width (16-bit value).
  uint16_t height;          //!< @brief Bitmap height (16-bit value).
  uint16_t planes;          //!< @brief Number of color planes (only 1).
  uint16_t bitsPerPixel;    //!< @brief Bits per pixel (1, 4, 8 or 24).
};

#include <Fog/Core/Pack/PackRestore.h>

// ============================================================================
// [Fog::BmpWinV3Header]
// ============================================================================

#include <Fog/Core/Pack/PackByte.h>

//! @internal
//!
//! @brief Bitmap V3 header.
//!
//! @note This header is 40 bytes length.
struct BmpWinV3Header
{
  uint32_t headerSize;      //!< @brief Header size (40, 52).
  uint32_t width;           //!< @brief Bitmap width.
  uint32_t height;          //!< @brief Bitmap height.
  uint16_t planes;          //!< @brief Count of planes, always 1.
  uint16_t bitsPerPixel;    //!< @brief Bits per pixel (1, 4, 8, 16, 24 or 32).
  uint32_t compression;     //!< @brief Compression methods used (see @c BMP_BI).
  uint32_t imageSize;       //!< @brief Image data size (in bytes).
  uint32_t horzResolution;  //!< @brief Horizontal resolution in pixels per meter.
  uint32_t vertResolution;  //!< @brief Vertical resolution in pixels per meter.
  uint32_t colorsUsed;      //!< @brief Number of colors in the image.
  uint32_t colorsImportant; //!< @brief Minimum number of important colors.
};

#include <Fog/Core/Pack/PackRestore.h>

// ============================================================================
// [Fog::BmpWinV4Header]
// ============================================================================

#include <Fog/Core/Pack/PackByte.h>

//! @internal
//!
//! @brief Bitmap V4 header.
//!
//! @note This header is 108 bytes length.
struct BmpWinV4Header : public BmpWinV3Header
{
  uint32_t rMask;           //!< @brief Mask identifying bits of red component.
  uint32_t gMask;           //!< @brief Mask identifying bits of green component.
  uint32_t bMask;           //!< @brief Mask identifying bits of blue component.
  uint32_t aMask;           //!< @brief Mask identifying bits of alpha component.
  uint32_t colorspace;      //!< @brief Color space type.
  uint32_t rX;              //!< @brief X coordinate of red endpoint.
  uint32_t rY;              //!< @brief Y coordinate of red endpoint.
  uint32_t rZ;              //!< @brief Z coordinate of red endpoint.
  uint32_t gX;              //!< @brief X coordinate of green endpoint.
  uint32_t gY;              //!< @brief Y coordinate of green endpoint.
  uint32_t gZ;              //!< @brief Z coordinate of green endpoint.
  uint32_t bX;              //!< @brief X coordinate of blue endpoint.
  uint32_t bY;              //!< @brief Y coordinate of blue endpoint.
  uint32_t bZ;              //!< @brief Z coordinate of blue endpoint.
  uint32_t rGamma;          //!< @brief Gamma red coordinate scale value.
  uint32_t gGamma;          //!< @brief Gamma green coordinate scale value.
  uint32_t bGamma;          //!< @brief Gamma blue coordinate scale value.
};

#include <Fog/Core/Pack/PackRestore.h>

// ============================================================================
// [Fog::BmpWinV5Header]
// ============================================================================

#include <Fog/Core/Pack/PackByte.h>

//! @internal
//!
//! @brief Bitmap V5 header.
//!
//! @note This header is 124 bytes length.
struct BmpWinV5Header : public BmpWinV4Header
{
  uint32_t intent;          //!< @brief Rendering intent for bitmap.
  uint32_t profileData;     //!< @brief The offset, in bytes, from the beginning
                            //!< of the BmpWinV5Header structure to the start of the profile data.
  uint32_t profileSize;     //!< @brief Size, in bytes, of embedded profile data.
  uint32_t reserved;        //!< @brief Reserved, should be zero.
};

#include <Fog/Core/Pack/PackRestore.h>

// ============================================================================
// [Fog::BmpDataHeader]
// ============================================================================

//! @internal
//!
//! @brief All bitmap headers in one union.
union BmpDataHeader
{
  uint32_t headerSize;

  BmpOS2V1Header os2v1;
  BmpWinV3Header winv3;
  BmpWinV4Header winv4;
  BmpWinV5Header winv5;
};

// ============================================================================
// [Fog::BMP_HEADER_SIZE]
// ============================================================================

//! @internal
enum BMP_HEADER_SIZE
{
  BMP_HEADER_SIZE_OS2_V1 = 12,
  BMP_HEADER_SIZE_WIN_V3 = 40,
  BMP_HEADER_SIZE_WIN_V4 = 108,
  BMP_HEADER_SIZE_WIN_V5 = 124
};

// ============================================================================
// [Fog::BMP_BI]
// ============================================================================

//! @internal
enum BMP_BI
{
  BMP_BI_RGB        = 0,
  BMP_BI_RLE8       = 1,
  BMP_BI_RLE4       = 2,
  BMP_BI_BITFIELDS  = 3,
  BMP_BI_JPEG       = 4,
  BMP_BI_PNG        = 5
};

// ============================================================================
// [Fog::BMP_RLE]
// ============================================================================

//! @internal
enum BMP_RLE
{
  BMP_RLE_NEXT_LINE = 0,
  BMP_RLE_END       = 1,
  BMP_RLE_MOVE      = 2
};

// ============================================================================
// [Fog::BMP_COLORSPACE_TYPE]
// ============================================================================

//! @internal
enum BMP_COLORSPACE_TYPE
{
  BMP_COLORSPACE_CALIBRATED_RGB = 0,
  BMP_COLORSPACE_DD_RGB = 1,
  BMP_COLORSPACE_DD_CMYK = 2
};

// ============================================================================
// [Fog::BmpCodecProvider]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT BmpCodecProvider : public ImageCodecProvider
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  BmpCodecProvider();
  virtual ~BmpCodecProvider();

  // --------------------------------------------------------------------------
  // [Implementation]
  // --------------------------------------------------------------------------

  virtual uint32_t checkSignature(const void* mem, sysuint_t length) const;
  virtual err_t createCodec(uint32_t codecType, ImageCodec** codec) const;
};

// ============================================================================
// [Fog::BmpDecoder]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT BmpDecoder : public ImageDecoder
{
  FOG_DECLARE_OBJECT(BmpDecoder, ImageDecoder)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  BmpDecoder(ImageCodecProvider* provider);
  virtual ~BmpDecoder();

  // --------------------------------------------------------------------------
  // [Implementation]
  // --------------------------------------------------------------------------

  virtual void reset();
  virtual err_t readHeader();
  virtual err_t readImage(Image& image);

  // --------------------------------------------------------------------------
  // [Properties]
  // --------------------------------------------------------------------------

  virtual err_t getProperty(const ManagedString& name, Value& value) const;
  virtual err_t setProperty(const ManagedString& name, const Value& value);

  // --------------------------------------------------------------------------
  // [Helpers]
  // --------------------------------------------------------------------------

  // Clear everything.
  FOG_INLINE void zeroall()
  {
    static const sysuint_t ddsize = sizeof(ImageDecoder);
    Memory::zero((uint8_t*)this + ddsize, sizeof(BmpDecoder) - ddsize);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  // Bitmap File Header (14 bytes).
  BmpFileHeader bmpFileHeader;
  BmpDataHeader bmpDataHeader;

  ImageFormatDescription bmpFormat;

  // Bmp.
  uint32_t bmpCompression;
  uint32_t bmpImageSize;
  uint32_t bmpStride;
  // How many bytes to skip to get the bitmap data.
  uint32_t bmpSkipBytes;

  //! @brief Used by the IcoDecoder to skip uninteresing part
  int _skipFileHeader;
};

// ============================================================================
// [Fog::BmpEncoder]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT BmpEncoder : public ImageEncoder
{
  FOG_DECLARE_OBJECT(BmpEncoder, ImageEncoder)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  BmpEncoder(ImageCodecProvider* provider);
  virtual ~BmpEncoder();

  // --------------------------------------------------------------------------
  // [Implementation]
  // --------------------------------------------------------------------------

  virtual err_t writeImage(const Image& image);
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPEINFO_DECLARE(Fog::BmpFileHeader, Fog::TYPEINFO_PRIMITIVE)
_FOG_TYPEINFO_DECLARE(Fog::BmpOS2V1Header, Fog::TYPEINFO_PRIMITIVE)
_FOG_TYPEINFO_DECLARE(Fog::BmpWinV3Header, Fog::TYPEINFO_PRIMITIVE)
_FOG_TYPEINFO_DECLARE(Fog::BmpWinV4Header, Fog::TYPEINFO_PRIMITIVE)
_FOG_TYPEINFO_DECLARE(Fog::BmpWinV5Header, Fog::TYPEINFO_PRIMITIVE)
_FOG_TYPEINFO_DECLARE(Fog::BmpDataHeader, Fog::TYPEINFO_PRIMITIVE)

// [Guard]
#endif // _FOG_G2D_IMAGING_CODECS_BMPCODEC_P_H
