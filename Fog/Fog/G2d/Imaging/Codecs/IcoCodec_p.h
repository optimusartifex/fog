// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_CODECS_ICOCODEC_P_H
#define _FOG_G2D_IMAGING_CODECS_ICOCODEC_P_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/IO/Stream.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/ImageCodec.h>
#include <Fog/G2d/Imaging/ImageCodecProvider.h>
#include <Fog/G2d/Imaging/ImageDecoder.h>
#include <Fog/G2d/Imaging/ImageEncoder.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ============================================================================
// [Fog::IcoHeader]
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
// [Fog::IcoEntry]
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
// [Fog::IcoCodecProvider]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT IcoCodecProvider : public ImageCodecProvider
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  IcoCodecProvider();
  virtual ~IcoCodecProvider();

  // --------------------------------------------------------------------------
  // [Implementation]
  // --------------------------------------------------------------------------

  virtual uint32_t checkSignature(const void* mem, size_t length) const;
  virtual err_t createCodec(uint32_t codecType, ImageCodec** codec) const;
};

// ============================================================================
// [Fog::IcoDecoder]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT IcoDecoder : public ImageDecoder
{
  FOG_DECLARE_OBJECT(IcoDecoder, ImageDecoder)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  IcoDecoder(ImageCodecProvider* provider);
  virtual ~IcoDecoder();

  // --------------------------------------------------------------------------
  // [Implementation]
  // --------------------------------------------------------------------------

  virtual void reset();
  virtual err_t readHeader();
  virtual err_t readImage(Image& image);

protected:
  // For determining offset/size of "frames" LE numbers are already converted
  // to BE numbers on BE systems.
  IcoEntry *_framesInfo;
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPEINFO_DECLARE(Fog::IcoHeader, Fog::TYPEINFO_PRIMITIVE)
_FOG_TYPEINFO_DECLARE(Fog::IcoEntry, Fog::TYPEINFO_PRIMITIVE)

// [Guard]
#endif // _FOG_G2D_IMAGING_CODECS_ICOCODEC_P_H
