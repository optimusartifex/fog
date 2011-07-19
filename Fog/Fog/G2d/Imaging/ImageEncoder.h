// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_IMAGEENCODER_H
#define _FOG_G2D_IMAGING_IMAGEENCODER_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/G2d/Imaging/ImageCodec.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ============================================================================
// [Fog::ImageEncoder]
// ============================================================================

//! @brief Image encoder.
struct FOG_API ImageEncoder : public ImageCodec
{
  FOG_DECLARE_OBJECT(ImageEncoder, ImageCodec)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  ImageEncoder(ImageCodecProvider* provider);
  virtual ~ImageEncoder();

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isHeaderDone() const { return _headerDone; }
  FOG_INLINE bool isWriterDone() const { return _writerDone; }

  FOG_INLINE void setComment(const ByteArray& comment) { _comment = comment; }

  // --------------------------------------------------------------------------
  // [Virtuals]
  // --------------------------------------------------------------------------

  virtual void detachStream();
  virtual err_t writeImage(const Image& image) = 0;

  // --------------------------------------------------------------------------
  // [Internal]
  // --------------------------------------------------------------------------

protected:
  virtual void reset();
  virtual void finalize();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

protected:
  uint32_t _headerDone : 1;
  uint32_t _writerDone : 1;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_IMAGING_IMAGEENCODER_H
