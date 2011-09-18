// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_IMAGEDEVICEPROVIDER_H
#define _FOG_G2D_IMAGING_IMAGEDEVICEPROVIDER_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Tools/List.h>
#include <Fog/Core/Tools/String.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

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
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  virtual ImageCodecProvider* addRef() const;
  virtual void deref();

  // --------------------------------------------------------------------------
  // [Provider]
  // --------------------------------------------------------------------------

  //! @brief Get the image device flags (encoder / decoder support).
  FOG_INLINE uint32_t getCodecType() const { return _codecType; }

  //! @brief Get the image stream type (see @c IMAGE_STREAM).
  FOG_INLINE uint32_t getStreamType() const { return _streamType; }

  //! @brief Get name of the provider.
  FOG_INLINE const StringW& getName() const { return _name; }
  //! @brief Get list of the supported image extensions.
  FOG_INLINE const List<StringW>& getImageExtensions() const { return _imageExtensions; }

  //! @brief Get whether image-decoder can be created by the provider.
  FOG_INLINE bool supportsImageDecoder() const { return (_codecType & IMAGE_CODEC_DECODER) != 0; }
  //! @brief Get whether image-encoder can be created by the provider.
  FOG_INLINE bool supportsImageEncoder() const { return (_codecType & IMAGE_CODEC_ENCODER) != 0; }

  //! @brief Get whether the provider supports an image file extension @a extension.
  bool supportsImageExtension(const StringW& extension) const;

  // --------------------------------------------------------------------------
  // [Virtual]
  // --------------------------------------------------------------------------

  //! @brief Check binary data signature (mime part of data) and return its score.
  virtual uint32_t checkSignature(const void* data, size_t length) const = 0;

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
  static err_t addProvider(ImageCodecProvider* provider);
  //! @brief Remove provider @a provider from the list.
  static err_t removeProvider(ImageCodecProvider* provider);
  //! @brief Get whether provider @a provider is in the list.
  static bool hasProvider(ImageCodecProvider* provider);

  //! @brief Get list of image providers.
  static List<ImageCodecProvider*> getProviders();

  //! @brief Get image provider by @a name.
  static ImageCodecProvider* getProviderByName(uint32_t codecType, const StringW& name);
  //! @brief Get image provider by file @a extension.
  static ImageCodecProvider* getProviderByExtension(uint32_t codecType, const StringW& extension);
  //! @brief Get image provider by signature.
  static ImageCodecProvider* getProviderBySignature(uint32_t codecType, void* mem, size_t len);

  //! @brief Create decoder for a given provider @a name.
  static err_t createDecoderByName(const StringW& name, ImageDecoder** codec);
  //! @brief Create encoder for a given provider @a name.
  static err_t createEncoderByName(const StringW& name, ImageEncoder** codec);

  //! @brief Create decoder for a given @a extension.
  static err_t createDecoderByExtension(const StringW& extension, ImageDecoder** codec);
  //! @brief Create encoder for a given @a extension.
  static err_t createEncoderByExtension(const StringW& extension, ImageEncoder** codec);

  //! @brief Create decoder for a file called @a fileName.
  //!
  //! This method is higher-level. It matches image extension, then tries to find
  //! suitable decoder device. It opens the file @a fileName and assigns the
  //! stream into the created decoder device on success.
  static err_t createDecoderForFile(const StringW& fileName, ImageDecoder** codec);

  //! @brief Create decoder device for a stream, using @a extension to match
  //! suitable decoder or using raw image signature.
  static err_t createDecoderForStream(Stream& stream, const StringW& extension, ImageDecoder** codec);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

protected:
  mutable Atomic<size_t> _reference;

  //! @brief Supported device flags (see @c IMAGE_CODEC).
  uint32_t _codecType;

  //! @brief Image stream type (see @c IMAGE_STREAM).
  uint32_t _streamType;

  //! @brief Image provider name (for example "BMP", "PNG", "PNG[GDI+]", ...).
  StringW _name;

  //! @brief List of standard image file extensions.
  List<StringW> _imageExtensions;

private:
  _FOG_NO_COPY(ImageCodecProvider)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_IMAGING_IMAGEDEVICEPROVIDER_H
