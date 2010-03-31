// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_IMAGEIO_BMP_P_H
#define _FOG_GRAPHICS_IMAGEIO_BMP_P_H

// [Dependencies]
#include <Fog/Build/Build.h>
#include <Fog/Core/Constants.h>
#include <Fog/Core/Stream.h>
#include <Fog/Core/String.h>
#include <Fog/Graphics/Image.h>
#include <Fog/Graphics/ImageIO.h>

//! @addtogroup Fog_Graphics_Private
//! @{

namespace Fog {
namespace ImageIO {

// ============================================================================
// [Fog::ImageIO::BmpFileHeader]
// ============================================================================

#include <Fog/Core/Compiler/PackByte.h>

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

#include <Fog/Core/Compiler/PackRestore.h>

// ============================================================================
// [Fog::ImageIO::BmpOS2V1Header]
// ============================================================================

#include <Fog/Core/Compiler/PackByte.h>

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

#include <Fog/Core/Compiler/PackRestore.h>

// ============================================================================
// [Fog::ImageIO::BmpWinV3Header]
// ============================================================================

#include <Fog/Core/Compiler/PackByte.h>

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
  uint32_t compression;     //!< Compression methods used (see @c BMP_BI).
  uint32_t imageSize;       //!< @brief Image data size (in bytes).
  uint32_t horzResolution;  //!< @brief Horizontal resolution in pixels per meter.
  uint32_t vertResolution;  //!< @brief Vertical resolution in pixels per meter.
  uint32_t colorsUsed;      //!< @brief Number of colors in the image.
  uint32_t colorsImportant; //!< @brief Minimum number of important colors.
};

#include <Fog/Core/Compiler/PackRestore.h>

// ============================================================================
// [Fog::ImageIO::BmpWinV4Header]
// ============================================================================

#include <Fog/Core/Compiler/PackByte.h>

//! @brief Bitmap V4 header.
//!
//! @note This header is 108 bytes length.
struct BmpWinV4Header
{
  uint32_t headerSize;      //!< @brief Header size (108).
  uint32_t width;           //!< @brief Bitmap width.
  uint32_t height;          //!< @brief Bitmap height.
  uint16_t planes;          //!< @brief Count of planes, always 1.
  uint16_t bitsPerPixel;    //!< @brief Bits per pixel (1, 4, 8, 16, 24 or 32).
  uint32_t compression;     //!< Compression methods used (see @c BMP_BI).
  uint32_t imageSize;       //!< @brief Image data size (in bytes).
  uint32_t horzResolution;  //!< @brief Horizontal resolution in pixels per meter.
  uint32_t vertResolution;  //!< @brief Vertical resolution in pixels per meter.
  uint32_t colorsUsed;      //!< @brief Number of colors in the image.
  uint32_t colorsImportant; //!< @brief Minimum number of important colors.

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

#include <Fog/Core/Compiler/PackRestore.h>

// ============================================================================
// [Fog::ImageIO::BmpWinV5Header]
// ============================================================================

#include <Fog/Core/Compiler/PackByte.h>

//! @brief Bitmap V5 header.
//!
//! @note This header is 124 bytes length.
struct BmpWinV5Header
{
  uint32_t headerSize;      //!< @brief Header size (108).
  uint32_t width;           //!< @brief Bitmap width.
  uint32_t height;          //!< @brief Bitmap height.
  uint16_t planes;          //!< @brief Count of planes, always 1.
  uint16_t bitsPerPixel;    //!< @brief Bits per pixel (1, 4, 8, 16, 24 or 32).
  uint32_t compression;     //!< Compression methods used (see @c BMP_BI).
  uint32_t imageSize;       //!< @brief Image data size (in bytes).
  uint32_t horzResolution;  //!< @brief Horizontal resolution in pixels per meter.
  uint32_t vertResolution;  //!< @brief Vertical resolution in pixels per meter.
  uint32_t colorsUsed;      //!< @brief Number of colors in the image.
  uint32_t colorsImportant; //!< @brief Minimum number of important colors.

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

  uint32_t intent;          //!< @brief Rendering intent for bitmap.
  uint32_t profileData;     //!< @brief The offset, in bytes, from the beginning
                            //!< of the BmpWinV5Header structure to the start of the profile data.
  uint32_t profileSize;     //!< @brief Size, in bytes, of embedded profile data.
  uint32_t reserved;        //!< @brief Reserved, should be zero.
};

#include <Fog/Core/Compiler/PackRestore.h>

// ============================================================================
// [Fog::ImageIO::BmpDataHeader]
// ============================================================================

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
// [Fog::ImageIO::BMP_BI]
// ============================================================================

enum BMP_HEADER_TYPE
{
  BMP_HEADER_OS2_V1 = 12,
  BMP_HEADER_WIN_V3 = 40,
  BMP_HEADER_WIN_V4 = 108,
  BMP_HEADER_WIN_V5 = 124
};

// ============================================================================
// [Fog::ImageIO::BMP_BI]
// ============================================================================

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
// [Fog::ImageIO::BMP_RLE]
// ============================================================================

enum BMP_RLE
{
  BMP_RLE_NEXT_LINE = 0,
  BMP_RLE_END       = 1,
  BMP_RLE_MOVE      = 2
};

// ============================================================================
// [Fog::ImageIO::BMP_COLORSPACE_TYPE]
// ============================================================================

enum BMP_COLORSPACE_TYPE
{
  BMP_COLORSPACE_CALIBRATED_RGB = 0,
  BMP_COLORSPACE_DD_RGB = 1,
  BMP_COLORSPACE_DD_CMYK = 2
};

// ============================================================================
// [Fog::ImageIO::BmpProvider]
// ============================================================================

struct FOG_HIDDEN BmpProvider : public Provider
{
  BmpProvider();
  virtual ~BmpProvider();

  virtual uint32_t checkSignature(const void* mem, sysuint_t length) const;
  virtual err_t createDevice(uint32_t deviceType, BaseDevice** device) const;
};

// ============================================================================
// [Fog::ImageIO::BmpDecoderDevice]
// ============================================================================

struct FOG_HIDDEN BmpDecoderDevice : public DecoderDevice
{
  FOG_DECLARE_OBJECT(BmpDecoderDevice, DecoderDevice)

  BmpDecoderDevice(Provider* provider);
  virtual ~BmpDecoderDevice();

  virtual void reset();
  virtual err_t readHeader();
  virtual err_t readImage(Image& image);

  // [Properties]

  virtual err_t getProperty(const ManagedString& name, Value& value) const;
  virtual err_t setProperty(const ManagedString& name, const Value& value);

  // [Helpers]

  // Clear everything.
  FOG_INLINE void zeroall()
  {
    static const sysuint_t ddsize = sizeof(DecoderDevice);
    Memory::zero((uint8_t*)this + ddsize, sizeof(BmpDecoderDevice) - ddsize);
  }

  // [Members]

  int _skipFileHeader;

  // Bitmap File Header (14 bytes).
  BmpFileHeader bmpFileHeader;
  BmpDataHeader bmpDataHeader;

  // Bmp.
  uint32_t bmpCompression;
  uint32_t bmpImageSize;
  uint32_t bmpStride;
  // How many bytes to skip to get bitmap data.
  uint32_t bmpSkipBytes;

  // Argb masks / shifts.
  uint32_t rMask;
  uint32_t gMask;
  uint32_t bMask;
  uint32_t aMask;

  uint32_t rShift;
  uint32_t gShift;
  uint32_t bShift;
  uint32_t aShift;

  // Rgb - only used by converter from 16 BPP.
  uint32_t rLoss;
  uint32_t gLoss;
  uint32_t bLoss;
  uint32_t aLoss;

  // True if 16 bpp is byteswapped (big endian machine and 16/15 BPP format).
  uint32_t isByteSwapped16;
};

// ============================================================================
// [Fog::ImageIO::BmpEncoderDevice]
// ============================================================================

struct FOG_HIDDEN BmpEncoderDevice : public EncoderDevice
{
  FOG_DECLARE_OBJECT(BmpEncoderDevice, EncoderDevice)

  BmpEncoderDevice(Provider* provider);
  virtual ~BmpEncoderDevice();

  virtual err_t writeImage(const Image& image);
};

} // ImageIO namespace
} // Fog namespace

//! @}

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::ImageIO::BmpFileHeader, Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::ImageIO::BmpOS2V1Header, Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::ImageIO::BmpWinV3Header, Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::ImageIO::BmpWinV4Header, Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::ImageIO::BmpWinV5Header, Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::ImageIO::BmpDataHeader, Fog::TYPEINFO_PRIMITIVE)

// [Guard]
#endif // _FOG_GRAPHICS_IMAGEIO_BMP_P_H
