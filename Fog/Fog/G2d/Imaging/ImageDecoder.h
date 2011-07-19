// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_IMAGEDECODER_H
#define _FOG_G2D_IMAGING_IMAGEDECODER_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/G2d/Imaging/ImageCodec.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ============================================================================
// [Fog::ImageDecoder]
// ============================================================================

//! @brief Image decoder.
struct FOG_API ImageDecoder : public ImageCodec
{
  FOG_DECLARE_OBJECT(ImageDecoder, ImageCodec)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  ImageDecoder(ImageCodecProvider* provider);
  virtual ~ImageDecoder();

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isHeaderDone() const { return _headerDone; }
  FOG_INLINE bool isReaderDone() const { return _readerDone; }
  FOG_INLINE uint32_t getHeaderResult() const { return _headerResult; }
  FOG_INLINE uint32_t getReaderResult() const { return _readerResult; }

  // --------------------------------------------------------------------------
  // [Virtuals]
  // --------------------------------------------------------------------------

  virtual err_t readHeader() = 0;
  virtual err_t readImage(Image& image) = 0;

  // --------------------------------------------------------------------------
  // [Internal]
  // --------------------------------------------------------------------------

protected:
  virtual void reset();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

protected:
  //! @brief @c true if header was read.
  uint32_t _headerDone : 1;
  //! @brief @c true if image was read.
  uint32_t _readerDone : 1;
  //! @brief Header decoder result code (returned by @c readHeader()).
  uint32_t _headerResult;
  //! @brief Image decoder result code (returned by @c readImage()).
  uint32_t _readerResult;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_IMAGING_IMAGEDECODER_H
