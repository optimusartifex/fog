// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_IMAGEIO_H
#define _FOG_GRAPHICS_IMAGEIO_H

// [Dependencies]
#include <Fog/Core/List.h>
#include <Fog/Core/Object.h>
#include <Fog/Core/Stream.h>
#include <Fog/Core/String.h>
#include <Fog/Core/Value.h>
#include <Fog/Graphics/Image.h>
#include <Fog/Graphics/Palette.h>

//! @addtogroup Fog_Graphics
//! @{

namespace Fog {
namespace ImageIO {

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct BaseDevice;
struct DecoderDevice;
struct EncoderDevice;
struct Provider;

// ============================================================================
// [Fog::ImageIO::Provider API]
// ============================================================================

//! @brief Add provider @a provider to the list.
FOG_API err_t addProvider(uint32_t deviceType, Provider* provider);
//! @brief Remove provider @a provider from the list.
FOG_API err_t removeProvider(uint32_t deviceType, Provider* provider);
//! @brief Get whether provider @a provider is in the list.
FOG_API bool hasProvider(uint32_t deviceType, Provider* provider);

//! @brief Get list of image providers.
FOG_API List<Provider*> getProviders(uint32_t deviceType);

//! @brief Get image provider by @a name.
FOG_API Provider* getProviderByName(uint32_t deviceType, const String& name);
//! @brief Get image provider by file @a extension.
FOG_API Provider* getProviderByExtension(uint32_t deviceType, const String& extension);
//! @brief Get image provider by signature.
FOG_API Provider* getProviderBySignature(uint32_t deviceType, void* mem, sysuint_t len);

// ============================================================================
// [Fog::ImageIO::Decoder / Encoder API]
// ============================================================================

//! @brief Create decoder device for a given provider @a name.
FOG_API err_t createDecoderByName(const String& name, DecoderDevice** device);
//! @brief Create encoder device for a given provider @a name.
FOG_API err_t createEncoderByName(const String& name, EncoderDevice** device);

//! @brief Create decoder device for a given @a extension.
FOG_API err_t createDecoderByExtension(const String& extension, DecoderDevice** device);
//! @brief Create encoder device for a given @a extension.
FOG_API err_t createEncoderByExtension(const String& extension, EncoderDevice** device);

//! @brief Create decoder device for a file called @a fileName.
//!
//! This method is higher-level. It matches image extension, then tries to find
//! suitable decoder device. It opens the file @a fileName and assigns the 
//! stream to resulting decoder device.
FOG_API err_t createDecoderForFile(const String& fileName, DecoderDevice** device);

//! @brief Create decoder device for a stream, using @a extension to match
//! suitable decoder or using raw image signature.
FOG_API err_t createDecoderForStream(Stream& stream, const String& extension, DecoderDevice** device);

// ============================================================================
// [Fog::ImageIO::Provider]
// ============================================================================

//! @brief Image file encoder/decoder provider (singleton).
//!
//! Providers are used to create image decoder and encoder devices and
//! to check if image data (signature) can be readed by decoder.
//!
//! Befire image decoder is created, often is checked relevance by
//! mime type data (first image file bytes)
struct FOG_API Provider
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Provider constructor.
  Provider();
  //! @brief Provider destructor (virtual).
  virtual ~Provider();

  virtual Provider* ref() const;
  virtual void deref();

  // --------------------------------------------------------------------------
  // [File Type]
  // --------------------------------------------------------------------------

  //! @brief Get image file type (see @c IMAGE_IO_FILE_TYPE).
  FOG_INLINE uint32_t getFileType() const { return _fileType; }

  // --------------------------------------------------------------------------
  // [Device Type]
  // --------------------------------------------------------------------------

  //! @brief Get supported device flags.
  FOG_INLINE uint32_t getDeviceType() const { return _deviceType; }

  //! @brief Get whether image decoder decide can be created by the provider.
  FOG_INLINE bool isDecoderType() const { return (_deviceType & IMAGE_IO_DEVICE_DECODER) != 0; }
  //! @brief Get whether image encoder decide can be created by the provider.
  FOG_INLINE bool isEncoderType() const { return (_deviceType & IMAGE_IO_DEVICE_ENCODER) != 0; }

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  //! @brief Get name of provider.
  FOG_INLINE const String& getName() const { return _name; }
  //! @brief Get list of supported image extensions.
  FOG_INLINE const List<String>& getImageExtensions() const { return _imageExtensions; }

  //! @brief Get whether provider supports image file extension @a extension.
  bool supportsImageExtension(const String& extension) const;

  // --------------------------------------------------------------------------
  // [Virtuals]
  // --------------------------------------------------------------------------

  //! @brief Check binary data signature (mime part of data) and return its score.
  virtual uint32_t checkSignature(const void* data, sysuint_t length) const = 0;

  //! @brief Create image decoder / encoder device.
  //!
  //! @param deviceType Requested device type, valid values are only 
  //!        @c IMAGE_IO_DEVICE_ENCODER or @c IMAGE_IO_DEVICE_DECODER.
  //! @param device Where to store pointer to the created device.
  virtual err_t createDevice(uint32_t deviceType, BaseDevice** device) const = 0;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

protected:
  mutable Atomic<sysuint_t> _refCount;

  //! @brief Image file type (see @c IMAGE_IO_FILE_TYPE).
  uint32_t _fileType;

  //! @brief Supported device flags (see @c IMAGE_IO_DEVICE_TYPE).
  uint32_t _deviceType;
  
  //! @brief Image provider name (for example "BMP", "PNG", "PNG[GDI+]", ...).
  String _name;

  //! @brief List of standard image file extensions.
  List<String> _imageExtensions;

private:
  FOG_DISABLE_COPY(Provider)
};

// ============================================================================
// [Fog::ImageIO::BaseDevice]
// ============================================================================

struct FOG_API BaseDevice : public Object
{
  FOG_DECLARE_OBJECT(BaseDevice, Object)

  // --------------------------------------------------------------------------
  // [Construction / Descruction]
  // --------------------------------------------------------------------------

  BaseDevice(Provider* provider);
  virtual ~BaseDevice();

  // --------------------------------------------------------------------------
  // [Properties]
  // --------------------------------------------------------------------------

  virtual err_t getProperty(const ManagedString& name, Value& value) const;
  virtual err_t setProperty(const ManagedString& name, const Value& value);

  // --------------------------------------------------------------------------
  // [Members Access]
  // --------------------------------------------------------------------------

  FOG_INLINE Provider* getProvider() const { return _provider; }

  FOG_INLINE uint32_t getFileType() const { return _fileType; }
  FOG_INLINE uint32_t getDeviceType() const { return _deviceType; }

  FOG_INLINE bool isEncoder() const { return (_deviceType & IMAGE_IO_DEVICE_ENCODER) != 0; }
  FOG_INLINE bool isDecoder() const { return (_deviceType & IMAGE_IO_DEVICE_DECODER) != 0; }

  FOG_INLINE uint64_t attachedOffset() const { return _attachedOffset; }
  FOG_INLINE Stream& getStream() { return _stream; }
  FOG_INLINE const Stream& getStream() const { return _stream; }

  FOG_INLINE uint32_t getWidth() const { return _width; }
  FOG_INLINE uint32_t getHeight() const { return _height; }
  FOG_INLINE uint32_t getDepth() const { return _depth; }
  FOG_INLINE uint32_t getPlanes() const { return _planes; }

  FOG_INLINE uint32_t getActualFrame() const { return _actualFrame; }
  FOG_INLINE uint32_t getFramesCount() const { return _framesCount; }

  FOG_INLINE int getFormat() const { return _format; }
  FOG_INLINE const Palette& getPalette() const { return _palette; }
  FOG_INLINE const ByteArray& getComment() const { return _comment; }

  // --------------------------------------------------------------------------
  // [Progress]
  // --------------------------------------------------------------------------

  FOG_INLINE float getProgress() const { return _progress; }

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
  // [Protected Virtuals]
  // --------------------------------------------------------------------------

protected:
  virtual void reset();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Device provider.
  Provider* _provider;

  //! @brief File format of target / source image (see @c IMAGE_IO_FILE_TYPE).
  uint32_t _fileType;
  //! @brief Device type (see @c IMAGE_IO_DEVICE_TYPE).
  uint32_t _deviceType;

  //! @brief Attached stream offset.
  uint64_t _attachedOffset;
  //! @brief Attached stream.
  Stream _stream;

  //! @brief Image or animation width.
  uint32_t _width;
  //! @brief Image or animation height.
  uint32_t _height;
  //! @brief Image or animation depth (bits per pixel).
  uint32_t _depth;
  //! @brief Image or animation planes.
  uint32_t _planes;

  //! @brief Actual frame.
  uint32_t _actualFrame;
  //! @brief Count of frames.
  uint32_t _framesCount;

  //! @brief Pixel format of target / source image.
  int _format;

  //! @brief Palette if reading / writing 8 bit or less images.
  //!
  //! This is image palette that can be contained in loaded image,
  //! but it's not needed if image is directly loaded to different
  //! image format and decoder supports this.
  Palette _palette;
  //! @brief Comment.
  ByteArray _comment;

  //! @brief Progress, 0 to 100 [percent]
  float _progress;

private:
  friend struct IcoDecoderDevice;
};

// ============================================================================
// [Fog::ImageIO::DecoderDevice]
// ============================================================================

//! @brief Image IO decoder device.
struct FOG_API DecoderDevice : public BaseDevice
{
  FOG_DECLARE_OBJECT(DecoderDevice, BaseDevice)

  // --------------------------------------------------------------------------
  // [Construction / Descruction]
  // --------------------------------------------------------------------------

  DecoderDevice(Provider* provider);
  virtual ~DecoderDevice();

  // --------------------------------------------------------------------------
  // [Members access]
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
  // [Protected virtuals]
  // --------------------------------------------------------------------------

protected:
  virtual void reset();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief @c true if header was read.
  uint32_t _headerDone : 1;
  //! @brief @c true if image was read.
  uint32_t _readerDone : 1;
  //! @brief Header decoder result code (returned by @c readHeader()).
  uint32_t _headerResult;
  //! @brief Image decoder result code (returned by @c readImage()).
  uint32_t _readerResult;
};

// ============================================================================
// [Fog::ImageIO::EncoderDevice]
// ============================================================================

//! @brief Image IO encoder device.
struct FOG_API EncoderDevice : public BaseDevice
{
  FOG_DECLARE_OBJECT(EncoderDevice, BaseDevice)

  // --------------------------------------------------------------------------
  // [Construction / Descruction]
  // --------------------------------------------------------------------------

  EncoderDevice(Provider* provider);
  virtual ~EncoderDevice();

  // --------------------------------------------------------------------------
  // [Members access]
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
  // [Protected Virtuals]
  // --------------------------------------------------------------------------

protected:
  virtual void reset();
  virtual void finalize();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint32_t _headerDone : 1;
  uint32_t _writerDone : 1;
};

} // ImageIO namespace
} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_GRAPHICS_IMAGEIO_H
