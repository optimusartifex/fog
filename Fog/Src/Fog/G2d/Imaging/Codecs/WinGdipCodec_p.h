// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_CODECS_WINGDIPCODEC_P_H
#define _FOG_G2D_IMAGING_CODECS_WINGDIPCODEC_P_H

#include <Fog/Core/C++/Base.h>

#if !defined(FOG_OS_WINDOWS)
#error "Fog::WinGdipCodec can be included / compiled only under Windows"
#endif // FOG_OS_WINDOWS

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/OS/Library.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/ImageCodec.h>
#include <Fog/G2d/Imaging/ImageCodecProvider.h>
#include <Fog/G2d/Imaging/ImageConverter.h>
#include <Fog/G2d/Imaging/ImageDecoder.h>
#include <Fog/G2d/Imaging/ImageEncoder.h>
#include <Fog/G2d/OS/WinGdiPlus.h>

namespace Fog {

// ============================================================================
// [Fog::WinGdipJpegParams]
// ============================================================================

//! @internal
struct WinGdipJpegParams
{
  int quality;
};

// ============================================================================
// [Fog::WinGdipPngParams]
// ============================================================================

//! @internal
struct WinGdipPngParams
{
  int dummy;
};

// ============================================================================
// [Fog::WinGdipTiffParams]
// ============================================================================

//! @internal
struct WinGdipTiffParams
{
  int dummy;
};

// ============================================================================
// [Fog::WinGdipCommonParams]
// ============================================================================

//! @internal
union WinGdipCommonParams
{
  WinGdipJpegParams jpeg;
  WinGdipPngParams png;
  WinGdipTiffParams tiff;
};

// ============================================================================
// [Fog::WinGdipCodecProvider]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT WinGdipCodecProvider : public ImageCodecProvider
{
  WinGdipCodecProvider(uint32_t streamType);
  virtual ~WinGdipCodecProvider();

  virtual uint32_t checkSignature(const void* mem, size_t length) const;
  virtual err_t createCodec(uint32_t codecType, ImageCodec** codec) const;

  const WCHAR* _gdipMime;
};

// ============================================================================
// [Fog::GdipDecoder]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT WinGdipDecoder : public ImageDecoder
{
  FOG_DECLARE_OBJECT(WinGdipDecoder, ImageDecoder)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  WinGdipDecoder(ImageCodecProvider* provider);
  virtual ~WinGdipDecoder();

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

  //! @brief IStream bridge.
  IStream* _istream;
  //! @brief Gdi+ library.
  WinGdiPlus* _gdip;
  //! @brief JPEG, PNG or TIFF parameters.
  WinGdipCommonParams _params;

  //! @brief Gdi+ shadow image instance.
  GpImage* _gpImage;
};

// ============================================================================
// [Fog::GdipEncoder]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT WinGdipEncoder : public ImageEncoder
{
  FOG_DECLARE_OBJECT(WinGdipEncoder, ImageEncoder)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  WinGdipEncoder(ImageCodecProvider* provider);
  virtual ~WinGdipEncoder();

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

  //! @brief IStream bridge.
  IStream* _istream;
  //! @brief Gdi+ library.
  WinGdiPlus* _gdip;
  //! @brief JPEG, PNG or TIFF parameters.
  WinGdipCommonParams _params;
};

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_IMAGING_CODECS_WINGDIPCODEC_P_H
