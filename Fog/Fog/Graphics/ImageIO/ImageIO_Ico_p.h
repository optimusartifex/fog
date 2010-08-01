// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_IMAGEIO_ICO_P_H
#define _FOG_GRAPHICS_IMAGEIO_ICO_P_H

// [Dependencies]
#include <Fog/Core/Build.h>
#include <Fog/Core/Constants.h>
#include <Fog/Core/Stream.h>
#include <Fog/Core/String.h>
#include <Fog/Graphics/Image.h>
#include <Fog/Graphics/ImageIO.h>
#include <Fog/Graphics/ImageIO/ImageIO_Bmp_p.h>

namespace Fog {
namespace ImageIO {

//! @addtogroup Fog_Graphics_Private
//! @{

// ============================================================================
// [Fog::ImageIO::IcoHeader]
// ============================================================================

#include <Fog/Core/Pack/PackByte.h>

//! @internal
//!
//! @brief Size of IcoHeader == 6
struct IcoHeader
{
  uint16_t reserved;
  uint16_t type;
  uint16_t count;
};

#include <Fog/Core/Pack/PackRestore.h>

// ============================================================================
// [Fog::ImageIO::IcoEntry]
// ============================================================================

#include <Fog/Core/Pack/PackByte.h>

//! @internal
//!
//! @brief Size of IcoEntry == 16.
struct IcoEntry
{
  uint8_t width;
  uint8_t height;
  uint8_t colorCount;
  uint8_t reserved;
  uint16_t planes;
  uint16_t bpp;
  uint32_t size;
  uint32_t offset;
};

#include <Fog/Core/Pack/PackRestore.h>

// ============================================================================
// [Fog::ImageIO::IcoProvider]
// ============================================================================

//! @internal
struct FOG_HIDDEN IcoProvider : public Provider
{
  IcoProvider();
  virtual ~IcoProvider();

  virtual uint32_t checkSignature(const void* mem, sysuint_t length) const;
  virtual err_t createDevice(uint32_t deviceType, BaseDevice** device) const;
};

// ============================================================================
// [Fog::ImageIO::IcoDecoderDevice]
// ============================================================================

//! @internal
struct FOG_HIDDEN IcoDecoderDevice : public DecoderDevice
{
  FOG_DECLARE_OBJECT(IcoDecoderDevice, DecoderDevice)

  IcoDecoderDevice(Provider* provider);
  virtual ~IcoDecoderDevice();

  virtual void reset();
  virtual err_t readHeader();
  virtual err_t readImage(Image& image);

private:
  // For determining offsets/sizes of "frames" LE numbers are already
  // converted to BE numbers on BE systems.
  IcoEntry *_framesInfo;
};

//! @}

} // ImageIO namespace
} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::ImageIO::IcoHeader, Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::ImageIO::IcoEntry, Fog::TYPEINFO_PRIMITIVE)

// [Guard]
#endif // _FOG_GRAPHICS_IMAGEIO_ICO_P_H
