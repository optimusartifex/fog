// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_IMAGEIO_PCX_P_H
#define _FOG_GRAPHICS_IMAGEIO_PCX_P_H

// [Dependencies]
#include <Fog/Core/Build.h>
#include <Fog/Core/Constants.h>
#include <Fog/Core/Stream.h>
#include <Fog/Core/String.h>
#include <Fog/Graphics/Image.h>
#include <Fog/Graphics/ImageIO.h>

namespace Fog {
namespace ImageIO {

//! @addtogroup Fog_Graphics_Private
//! @{

// ============================================================================
// [Fog::ImageIO::PcxHeader]
// ============================================================================

#include <Fog/Core/Pack/PackByte.h>

//! @internal
struct PcxHeader
{
  uint8_t manufacturer;
  uint8_t version;
  uint8_t encoding;
  uint8_t bitsPerPixel;
  int16_t xMin;
  int16_t yMin;
  int16_t xMax;
  int16_t yMax;
  int16_t horizontalDPI;
  int16_t verticalDPI;
  uint8_t colorMap[48];
  uint8_t reserved;
  uint8_t nPlanes;
  int16_t bytesPerLine;
  int16_t paletteInfo;
  int16_t hScreenSize;
  int16_t vScreenSize;
  uint8_t unused[54];
};

#include <Fog/Core/Pack/PackRestore.h>

// ============================================================================
// [Fog::ImageIO::PcxProvider]
// ============================================================================

//! @internal
struct FOG_HIDDEN PcxProvider : public Provider
{
  PcxProvider();
  virtual ~PcxProvider();

  virtual uint32_t checkSignature(const void* mem, sysuint_t length) const;
  virtual err_t createDevice(uint32_t deviceType, BaseDevice** device) const;
};

// ============================================================================
// [Fog::ImageIO::PcxDecoderDevice]
// ============================================================================

//! @internal
struct FOG_HIDDEN PcxDecoderDevice : public DecoderDevice
{
  FOG_DECLARE_OBJECT(PcxDecoderDevice, DecoderDevice)

  PcxDecoderDevice(Provider* provider);
  virtual ~PcxDecoderDevice();

  virtual void reset();
  virtual err_t readHeader();
  virtual err_t readImage(Image& image);

  FOG_INLINE const PcxHeader& pcxFileHeader() const { return _pcxFileHeader; }

  // Clear everything.
  FOG_INLINE void zeroall() { Memory::zero(&_pcxFileHeader, sizeof(_pcxFileHeader)); }

  PcxHeader _pcxFileHeader;
};

// ============================================================================
// [Fog::ImageIO::PcxEncoderDevice]
// ============================================================================

//! @internal
struct FOG_HIDDEN PcxEncoderDevice : public EncoderDevice
{
  FOG_DECLARE_OBJECT(PcxEncoderDevice, EncoderDevice)

  PcxEncoderDevice(Provider* provider);
  virtual ~PcxEncoderDevice();

  virtual err_t writeImage(const Image& image);
};

//! @}

} // ImageIO namespace
} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::ImageIO::PcxHeader, Fog::TYPEINFO_PRIMITIVE)

// [Guard]
#endif // _FOG_GRAPHICS_IMAGEIO_PCX_P_H
