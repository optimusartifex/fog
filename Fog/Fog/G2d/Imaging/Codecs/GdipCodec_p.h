// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_CODECS_GDIPCODEC_P_H
#define _FOG_G2D_IMAGING_CODECS_GDIPCODEC_P_H

#include <Fog/Core/C++/Base.h>

#if !defined(FOG_OS_WINDOWS)
#error "Fog::GdipCodecProvider can be included / compiled only under Windows"
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
#include <Fog/G2d/Win/GdipLibrary.h>

namespace Fog {

// ============================================================================
// [Fog::GdipJpegParams]
// ============================================================================

//! @internal
struct GdipJpegParams
{
  int quality;
};

// ============================================================================
// [Fog::GdipPngParams]
// ============================================================================

//! @internal
struct GdipPngParams
{
  int dummy;
};

// ============================================================================
// [Fog::GdipTiffParams]
// ============================================================================

//! @internal
struct GdipTiffParams
{
  int dummy;
};

// ============================================================================
// [Fog::GdipCommonParams]
// ============================================================================

//! @internal
union GdipCommonParams
{
  GdipJpegParams jpeg;
  GdipPngParams png;
  GdipTiffParams tiff;
};

// ============================================================================
// [Fog::GdipCodecProvider]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT GdipCodecProvider : public ImageCodecProvider
{
  GdipCodecProvider(uint32_t streamType);
  virtual ~GdipCodecProvider();

  virtual uint32_t checkSignature(const void* mem, size_t length) const;
  virtual err_t createCodec(uint32_t codecType, ImageCodec** codec) const;

  const WCHAR* _gdipMime;
};

// ============================================================================
// [Fog::GdipDecoder]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT GdipDecoder : public ImageDecoder
{
  FOG_DECLARE_OBJECT(GdipDecoder, ImageDecoder)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  GdipDecoder(ImageCodecProvider* provider);
  virtual ~GdipDecoder();

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

  virtual err_t getProperty(const ManagedString& name, Var& dst) const;
  virtual err_t setProperty(const ManagedString& name, const Var& src);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief IStream bridge.
  IStream* _istream;
  //! @brief Gdi+ library.
  GdipLibrary* _gdip;
  //! @brief JPEG, PNG or TIFF parameters.
  GdipCommonParams _params;

  //! @brief Gdi+ shadow image instance.
  GpImage* _gpImage;
};

// ============================================================================
// [Fog::GdipEncoder]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT GdipEncoder : public ImageEncoder
{
  FOG_DECLARE_OBJECT(GdipEncoder, ImageEncoder)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  GdipEncoder(ImageCodecProvider* provider);
  virtual ~GdipEncoder();

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

  virtual err_t getProperty(const ManagedString& name, Var& dst) const;
  virtual err_t setProperty(const ManagedString& name, const Var& src);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief IStream bridge.
  IStream* _istream;
  //! @brief Gdi+ library.
  GdipLibrary* _gdip;
  //! @brief JPEG, PNG or TIFF parameters.
  GdipCommonParams _params;
};

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_IMAGING_CODECS_GDIPCODEC_P_H
