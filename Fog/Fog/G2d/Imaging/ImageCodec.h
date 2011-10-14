// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_IMAGEDEVICE_H
#define _FOG_G2D_IMAGING_IMAGEDEVICE_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Kernel/Object.h>
#include <Fog/Core/Tools/Stream.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/Var.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/ImagePalette.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct IcoDecoder;

// ============================================================================
// [Fog::ImageCodec]
// ============================================================================

struct FOG_API ImageCodec : public Object
{
  FOG_DECLARE_OBJECT(ImageCodec, Object)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  ImageCodec(ImageCodecProvider* provider);
  virtual ~ImageCodec();

  // --------------------------------------------------------------------------
  // [Properties]
  // --------------------------------------------------------------------------

  virtual err_t _getProperty(const ManagedStringW& name, Var& dst) const;
  virtual err_t _setProperty(const ManagedStringW& name, const Var& src);

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE ImageCodecProvider* getProvider() const { return _provider; }

  FOG_INLINE uint32_t getCodecType() const { return _codecType; }
  FOG_INLINE uint32_t getStreamType() const { return _streamType; }

  FOG_INLINE bool isEncoder() const { return (_codecType & IMAGE_CODEC_ENCODER) != 0; }
  FOG_INLINE bool isDecoder() const { return (_codecType & IMAGE_CODEC_DECODER) != 0; }

  FOG_INLINE uint64_t getAttachedOffset() const { return _attachedOffset; }
  FOG_INLINE Stream& getStream() { return _stream; }
  FOG_INLINE const Stream& getStream() const { return _stream; }

  FOG_INLINE const SizeI& getSize() const { return _size; }
  FOG_INLINE int getWidth() const { return _size.w; }
  FOG_INLINE int getHeight() const { return _size.h; }

  FOG_INLINE uint32_t getDepth() const { return _depth; }
  FOG_INLINE uint32_t getPlanes() const { return _planes; }

  FOG_INLINE uint32_t getActualFrame() const { return _actualFrame; }
  FOG_INLINE uint32_t getFramesCount() const { return _framesCount; }

  FOG_INLINE uint32_t getFormat() const { return _format; }
  FOG_INLINE float getProgress() const { return _progress; }

  FOG_INLINE const ImagePalette& getPalette() const { return _palette; }
  FOG_INLINE const StringA& getComment() const { return _comment; }

  // --------------------------------------------------------------------------
  // [Progress]
  // --------------------------------------------------------------------------

  //! @brief Update progress to @a value.
  void updateProgress(float value);
  //! @overload
  void updateProgress(uint32_t y, uint32_t height);

  // --------------------------------------------------------------------------
  // [Image Size]
  // --------------------------------------------------------------------------

  bool checkImageSize() const;

  // --------------------------------------------------------------------------
  // [Stream]
  // --------------------------------------------------------------------------

  virtual void attachStream(Stream& stream);
  virtual void detachStream();

  // --------------------------------------------------------------------------
  // [Internal]
  // --------------------------------------------------------------------------

  virtual void reset();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Image provider.
  ImageCodecProvider* _provider;

  //! @brief Device type (see @c IMAGE_CODEC).
  uint32_t _codecType;
  //! @brief File format of target / source image (see @c IMAGE_STREAM).
  uint32_t _streamType;

  //! @brief Attached stream offset.
  uint64_t _attachedOffset;
  //! @brief Attached stream.
  Stream _stream;

  //! @brief Image or animation size.
  SizeI _size;
  //! @brief Image or animation depth (bits per pixel).
  uint32_t _depth;
  //! @brief Image or animation planes.
  uint32_t _planes;

  //! @brief Actual frame.
  uint32_t _actualFrame;
  //! @brief Count of frames.
  uint32_t _framesCount;

  //! @brief Pixel format of target / source image.
  uint32_t _format;

  //! @brief Progress, 0 to 100 [percent]
  float _progress;

  //! @brief ImagePalette if reading / writing 8 bit or less depth.
  //!
  //! This is image palette that can be contained in loaded image, but it's
  //! not needed if it's supported to load image into the different image
  //! format.
  ImagePalette _palette;
  //! @brief Comment.
  StringA _comment;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_IMAGING_IMAGEDEVICE_H
