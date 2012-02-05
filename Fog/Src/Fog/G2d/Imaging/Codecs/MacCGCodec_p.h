// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_CODECS_MACCGCODEC_P_H
#define _FOG_G2D_IMAGING_CODECS_MACCGCODEC_P_H

#include <Fog/Core/C++/Base.h>

#if !defined(FOG_OS_MAC)
#error "Fog::MacCGCodec can be included / compiled only under Mac/iOS"
#endif // FOG_OS_MAC

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/OS/MacUtil.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/ImageCodec.h>
#include <Fog/G2d/Imaging/ImageCodecProvider.h>
#include <Fog/G2d/Imaging/ImageConverter.h>
#include <Fog/G2d/Imaging/ImageDecoder.h>
#include <Fog/G2d/Imaging/ImageEncoder.h>

namespace Fog {

// ============================================================================
// [Fog::MacCGCodecProvider]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT MacCGCodecProvider : public ImageCodecProvider
{
  MacCGCodecProvider(uint32_t streamType);
  virtual ~MacCGCodecProvider();

  virtual uint32_t checkSignature(const void* mem, size_t length) const;
  virtual err_t createCodec(uint32_t codecType, ImageCodec** codec) const;
  
  CFStringRef getUTType() const;
};

// ============================================================================
// [Fog::MacCGDecoder]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT MacCGDecoder : public ImageDecoder
{
  FOG_DECLARE_OBJECT(MacCGDecoder, ImageDecoder)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  MacCGDecoder(ImageCodecProvider* provider);
  virtual ~MacCGDecoder();

  // --------------------------------------------------------------------------
  // [Stream]
  // --------------------------------------------------------------------------

  virtual void attachStream(Stream& stream);
  virtual void detachStream();

  // --------------------------------------------------------------------------
  // [Virtuals]
  // --------------------------------------------------------------------------

  virtual void reset();

  virtual err_t readHeader();
  virtual err_t readImage(Image& image);

  // --------------------------------------------------------------------------
  // [Properties]
  // --------------------------------------------------------------------------

  virtual err_t _getProperty(const InternedStringW& name, Var& dst) const;
  virtual err_t _setProperty(const InternedStringW& name, const Var& src);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  CGDataProviderSequentialCallbacks _providerCallbacks;
  CGImageSourceRef _cgImageSource;
};

// ============================================================================
// [Fog::MacCGEncoder]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT MacCGEncoder : public ImageEncoder
{
  FOG_DECLARE_OBJECT(MacCGEncoder, ImageEncoder)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  MacCGEncoder(ImageCodecProvider* provider);
  virtual ~MacCGEncoder();

  // --------------------------------------------------------------------------
  // [Stream]
  // --------------------------------------------------------------------------

  virtual void attachStream(Stream& stream);
  virtual void detachStream();

  // --------------------------------------------------------------------------
  // [Virtuals]
  // --------------------------------------------------------------------------

  virtual void reset();
  virtual err_t writeImage(const Image& image);

  // --------------------------------------------------------------------------
  // [Properties]
  // --------------------------------------------------------------------------

  virtual err_t _getProperty(const InternedStringW& name, Var& dst) const;
  virtual err_t _setProperty(const InternedStringW& name, const Var& src);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  CGDataConsumerCallbacks _consumerCallbacks;
};

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_IMAGING_CODECS_MACCGCODEC_P_H
