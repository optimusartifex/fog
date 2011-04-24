// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_IMAGEDEVICEPROVIDER_H
#define _FOG_G2D_IMAGING_IMAGEDEVICEPROVIDER_H

// [Dependencies]
#include <Fog/Core/Collection/List.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/G2d/Global/Constants.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct ImageDecoder;
struct ImageCodec;
struct ImageEncoder;

// ============================================================================
// [Fog::ImageCodecProvider]
// ============================================================================

//! @brief Image file encoder/decoder provider (singleton).
//!
//! Providers are used to create image decoder and encoder devices and
//! to check if image data (signature) can be readed by decoder.
//!
//! Befire image decoder is created, often is checked relevance by
//! mime type data (first image file bytes)
struct FOG_API ImageCodecProvider
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief The image provider constructor.
  ImageCodecProvider();
  //! @brief The image provider destructor (virtual).
  virtual ~ImageCodecProvider();

  // --------------------------------------------------------------------------
  // [Ref / Deref]
  // --------------------------------------------------------------------------

  virtual ImageCodecProvider* ref() const;
  virtual void deref();

  // --------------------------------------------------------------------------
  // [Provider]
  // --------------------------------------------------------------------------

  //! @brief Get the image device flags (encoder / decoder support).
  FOG_INLINE uint32_t getCodecType() const { return _codecType; }

  //! @brief Get the image stream type (see @c IMAGE_STREAM_TYPE).
  FOG_INLINE uint32_t getStreamType() const { return _streamType; }

  //! @brief Get name of the provider.
  FOG_INLINE const String& getName() const { return _name; }
  //! @brief Get list of the supported image extensions.
  FOG_INLINE const List<String>& getImageExtensions() const { return _imageExtensions; }

  //! @brief Get whether image-decoder can be created by the provider.
  FOG_INLINE bool supportsImageDecoder() const { return (_codecType & IMAGE_CODEC_DECODER) != 0; }
  //! @brief Get whether image-encoder can be created by the provider.
  FOG_INLINE bool supportsImageEncoder() const { return (_codecType & IMAGE_CODEC_ENCODER) != 0; }

  //! @brief Get whether the provider supports an image file extension @a extension.
  bool supportsImageExtension(const String& extension) const;

  // --------------------------------------------------------------------------
  // [Virtual]
  // --------------------------------------------------------------------------

  //! @brief Check binary data signature (mime part of data) and return its score.
  virtual uint32_t checkSignature(const void* data, sysuint_t length) const = 0;

  //! @brief Create image decoder / encoder.
  //!
  //! @param codecType Requested codec type, valid values are only
  //!        @c IMAGE_CODEC_ENCODER or @c IMAGE_CODEC_DECODER.
  //! @param device Where to store pointer to the created device.
  virtual err_t createCodec(uint32_t codecType, ImageCodec** codec) const = 0;

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  //! @brief Add provider @a provider to the list.
  static err_t addProvider(uint32_t codecType, ImageCodecProvider* provider);
  //! @brief Remove provider @a provider from the list.
  static err_t removeProvider(uint32_t codecType, ImageCodecProvider* provider);
  //! @brief Get whether provider @a provider is in the list.
  static bool hasProvider(uint32_t codecType, ImageCodecProvider* provider);

  //! @brief Get list of image providers.
  static List<ImageCodecProvider*> getProviders(uint32_t codecType);

  //! @brief Get image provider by @a name.
  static ImageCodecProvider* getProviderByName(uint32_t codecType, const String& name);
  //! @brief Get image provider by file @a extension.
  static ImageCodecProvider* getProviderByExtension(uint32_t codecType, const String& extension);
  //! @brief Get image provider by signature.
  static ImageCodecProvider* getProviderBySignature(uint32_t codecType, void* mem, sysuint_t len);

  //! @brief Create decoder for a given provider @a name.
  static err_t createDecoderByName(const String& name, ImageDecoder** codec);
  //! @brief Create encoder for a given provider @a name.
  static err_t createEncoderByName(const String& name, ImageEncoder** codec);

  //! @brief Create decoder for a given @a extension.
  static err_t createDecoderByExtension(const String& extension, ImageDecoder** codec);
  //! @brief Create encoder for a given @a extension.
  static err_t createEncoderByExtension(const String& extension, ImageEncoder** codec);

  //! @brief Create decoder for a file called @a fileName.
  //!
  //! This method is higher-level. It matches image extension, then tries to find
  //! suitable decoder device. It opens the file @a fileName and assigns the
  //! stream into the created decoder device on success.
  static err_t createDecoderForFile(const String& fileName, ImageDecoder** codec);

  //! @brief Create decoder device for a stream, using @a extension to match
  //! suitable decoder or using raw image signature.
  static err_t createDecoderForStream(Stream& stream, const String& extension, ImageDecoder** codec);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

protected:
  mutable Atomic<sysuint_t> _refCount;

  //! @brief Supported device flags (see @c IMAGE_CODEC_TYPE).
  uint32_t _codecType;

  //! @brief Image stream type (see @c IMAGE_STREAM_TYPE).
  uint32_t _streamType;

  //! @brief Image provider name (for example "BMP", "PNG", "PNG[GDI+]", ...).
  String _name;

  //! @brief List of standard image file extensions.
  List<String> _imageExtensions;

private:
  FOG_DISABLE_COPY(ImageCodecProvider)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_IMAGING_IMAGEDEVICEPROVIDER_H
