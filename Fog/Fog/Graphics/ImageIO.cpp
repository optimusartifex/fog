// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Assert.h>
#include <Fog/Core/AutoLock.h>
#include <Fog/Core/FileSystem.h>
#include <Fog/Core/FileUtil.h>
#include <Fog/Core/List.h>
#include <Fog/Core/Lock.h>
#include <Fog/Core/ManagedString.h>
#include <Fog/Core/MapFile.h>
#include <Fog/Core/Memory.h>
#include <Fog/Core/Static.h>
#include <Fog/Core/Std.h>
#include <Fog/Core/Stream.h>
#include <Fog/Core/String.h>
#include <Fog/Core/Strings.h>
#include <Fog/Core/TextCodec.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Image.h>
#include <Fog/Graphics/ImageIO.h>

// [Provider Ininializers]
FOG_CAPI_EXTERN void fog_imageio_init_bmp(void);
FOG_CAPI_EXTERN void fog_imageio_init_gif(void);
FOG_CAPI_EXTERN void fog_imageio_init_ico(void);
FOG_CAPI_EXTERN void fog_imageio_init_jpeg(void);
FOG_CAPI_EXTERN void fog_imageio_init_pcx(void);
FOG_CAPI_EXTERN void fog_imageio_init_png(void);

#if defined(FOG_OS_WINDOWS)
FOG_INIT_DECLARE void fog_imageio_init_gdiplus(void);
#endif // FOG_OS_WINDOWS

FOG_IMPLEMENT_OBJECT(Fog::ImageIO::BaseDevice)
FOG_IMPLEMENT_OBJECT(Fog::ImageIO::DecoderDevice)
FOG_IMPLEMENT_OBJECT(Fog::ImageIO::EncoderDevice)

namespace Fog {

// ============================================================================
// [Fog::ImageIO - Local]
// ============================================================================

struct ImageIO_Local
{
  // Critical section for accessing members.
  Lock lock;

  // List of decoder providers.
  List<ImageIO::Provider*> decoderProviders;
  // List of encoder providers.
  List<ImageIO::Provider*> encoderProviders;
};

static Static<ImageIO_Local> imageio_local;

namespace ImageIO {

// ============================================================================
// [Fog::ImageIO::Provider API]
// ============================================================================

FOG_API err_t addProvider(uint32_t deviceType, Provider* provider)
{
  if ((deviceType & (IMAGE_IO_DEVICE_DECODER | IMAGE_IO_DEVICE_ENCODER)) == 0)
    return ERR_RT_INVALID_ARGUMENT;

  err_t err = ERR_OK;
  AutoLock locked(imageio_local->lock);

  if ((deviceType & IMAGE_IO_DEVICE_DECODER) && 
      (imageio_local->decoderProviders.indexOf(provider) == INVALID_INDEX))
  {
    err |= imageio_local->decoderProviders.append(provider);
    if (err) goto end;
    provider->ref();
  }

  if ((deviceType & IMAGE_IO_DEVICE_ENCODER) && 
      (imageio_local->encoderProviders.indexOf(provider) == INVALID_INDEX))
  {
    err |= imageio_local->encoderProviders.append(provider);
    if (err) goto end;
    provider->ref();
  }

end:
  return err;
}

FOG_API err_t removeProvider(uint32_t deviceType, Provider* provider)
{
  if ((deviceType & (IMAGE_IO_DEVICE_DECODER | IMAGE_IO_DEVICE_ENCODER)) == 0)
    return ERR_RT_INVALID_ARGUMENT;

  err_t err = ERR_OK;
  sysuint_t index;
  AutoLock locked(imageio_local->lock);

  if ((deviceType & IMAGE_IO_DEVICE_DECODER) && 
      (index = imageio_local->decoderProviders.indexOf(provider)) != INVALID_INDEX)
  {
    err |= imageio_local->decoderProviders.removeAt(index);
    if (err) goto end;
    provider->deref();
  }

  if ((deviceType & IMAGE_IO_DEVICE_ENCODER) && 
      (index = imageio_local->encoderProviders.indexOf(provider)) != INVALID_INDEX)
  {
    err |= imageio_local->encoderProviders.removeAt(index);
    if (err) goto end;
    provider->deref();
  }

end:
  return err;
}

FOG_API bool hasProvider(uint32_t deviceType, Provider* provider)
{
  AutoLock locked(imageio_local->lock);

  switch (deviceType & (IMAGE_IO_DEVICE_DECODER | IMAGE_IO_DEVICE_ENCODER))
  {
    case IMAGE_IO_DEVICE_DECODER:
      return imageio_local->decoderProviders.indexOf(provider) != INVALID_INDEX;
    case IMAGE_IO_DEVICE_ENCODER:
      return imageio_local->encoderProviders.indexOf(provider) != INVALID_INDEX;
    case IMAGE_IO_DEVICE_DECODER | IMAGE_IO_DEVICE_ENCODER:
      return imageio_local->decoderProviders.indexOf(provider) != INVALID_INDEX &&
             imageio_local->encoderProviders.indexOf(provider) != INVALID_INDEX;
    default:
      return false;
  }
}

FOG_API List<Provider*> getProviders(uint32_t deviceType)
{
  AutoLock locked(imageio_local->lock);

  if ((deviceType & (IMAGE_IO_DEVICE_DECODER | IMAGE_IO_DEVICE_ENCODER)) == IMAGE_IO_DEVICE_ENCODER)
    return imageio_local->encoderProviders;

  if ((deviceType & (IMAGE_IO_DEVICE_DECODER | IMAGE_IO_DEVICE_ENCODER)) == IMAGE_IO_DEVICE_DECODER)
    return imageio_local->decoderProviders;

  return List<Provider*>();
}

FOG_API Provider* getProviderByName(uint32_t deviceType, const String& name)
{
  List<Provider*> providers = getProviders(deviceType);
  List<Provider*>::ConstIterator it(providers);

  for (; it.isValid(); it.toNext())
  {
    if (it.value()->getName() == name) return it.value();
  }

  return 0;
}

FOG_API Provider* getProviderByExtension(uint32_t deviceType, const String& extension)
{
  List<Provider*> providers = getProviders(deviceType);
  List<Provider*>::ConstIterator it(providers);

  // Convert extension to lower case.
  TemporaryString<16> e(extension);
  e.lower();

  for (it.toStart(); it.isValid(); it.toNext())
  {
    if (it.value()->supportsImageExtension(e)) return it.value();
  }

  return NULL;
}

FOG_API Provider* getProviderBySignature(uint32_t deviceType, void* mem, sysuint_t len)
{
  if (!mem || len == 0) return NULL;

  List<Provider*> providers = getProviders(deviceType);
  List<Provider*>::ConstIterator it(providers);

  uint32_t bestScore = 0;
  Provider* bestProvider = NULL;

  for (it.toStart(); it.isValid(); it.toNext())
  {
    uint32_t score = it.value()->checkSignature(mem, len);

    if (score > bestScore)
    {
      bestScore = score;
      bestProvider = it.value();
    }
  }

  return bestProvider;
}

// ============================================================================
// [Fog::ImageIO::Decoder / Encoder API]
// ============================================================================

static err_t createBaseDeviceByName(uint32_t deviceType, const String& name, BaseDevice** device)
{
  err_t err = ERR_OK;
  if (device == NULL) return ERR_RT_INVALID_ARGUMENT;

  Provider* provider = getProviderByName(deviceType, name);

  err = (!provider)
    ? (deviceType == IMAGE_IO_DEVICE_DECODER 
      ? ERR_IMAGEIO_NO_DECODER
      : ERR_IMAGEIO_NO_ENCODER)
    : provider->createDevice(deviceType, device);

end:
  return err;
}

FOG_API err_t createDecoderByName(const String& name, DecoderDevice** device)
{
  return createBaseDeviceByName(
    IMAGE_IO_DEVICE_DECODER, name, reinterpret_cast<BaseDevice**>(device));
}

FOG_API err_t createEncoderByName(const String& name, EncoderDevice** device)
{
  return createBaseDeviceByName(
    IMAGE_IO_DEVICE_ENCODER, name, reinterpret_cast<BaseDevice**>(device));
}

static err_t createBaseDeviceByExtension(uint32_t deviceType, const String& extension, BaseDevice** device)
{
  err_t err = ERR_OK;
  if (device == NULL) return ERR_RT_INVALID_ARGUMENT;

  Provider* provider = getProviderByExtension(deviceType, extension);

  err = (!provider)
    ? (deviceType == IMAGE_IO_DEVICE_DECODER 
      ? ERR_IMAGEIO_NO_DECODER
      : ERR_IMAGEIO_NO_ENCODER)
    : provider->createDevice(deviceType, device);

end:
  return err;
}

FOG_API err_t createDecoderByExtension(const String& extension, DecoderDevice** device)
{
  return createBaseDeviceByExtension(
    IMAGE_IO_DEVICE_DECODER, extension, reinterpret_cast<BaseDevice**>(device));
}

FOG_API err_t createEncoderByExtension(const String& extension, EncoderDevice** device)
{
  return createBaseDeviceByExtension(
    IMAGE_IO_DEVICE_ENCODER, extension, reinterpret_cast<BaseDevice**>(device));
}

FOG_API err_t createDecoderForFile(const String& fileName, DecoderDevice** device)
{
  if (device == NULL) return ERR_RT_INVALID_ARGUMENT;

  Stream stream;
  String extension;

  err_t err = stream.openMMap(fileName, false);
  // MMap failed? Try to open the file using standard stream.
  if (err != ERR_OK) err = stream.openFile(fileName, STREAM_OPEN_READ);

  // If err is not ERR_OK then file can't be open.
  if (err != ERR_OK) goto end;

  // Extract extension to help opening from stream.
  if ((err = FileUtil::extractExtension(extension, fileName)) || (err = extension.lower())) goto end;

  // Finally create device.
  err = createDecoderForStream(stream, extension, device);

end:
  return err;
}

FOG_API err_t createDecoderForStream(Stream& stream, const String& extension, DecoderDevice** device)
{
  if (device == NULL) return ERR_RT_INVALID_ARGUMENT;

  err_t err = ERR_OK;

  Provider* provider = NULL;
  DecoderDevice* decoder = NULL;

  int64_t pos;

  // This is default, 128 bytes should be enough for any image file type.
  uint8_t mime[128];
  sysuint_t readn;

  if (!stream.isSeekable()) { err = ERR_IO_CANT_SEEK; goto end; }

  pos = stream.tell();
  readn = stream.read(mime, 128);

  if (stream.seek(pos, STREAM_SEEK_SET) == -1) { err =  ERR_IO_CANT_SEEK; goto end; }

  // First try to use extension.
  if (!extension.isEmpty()) provider = getProviderByExtension(IMAGE_IO_DEVICE_DECODER, extension);
  // Fallback to signature checking if extension match failed.
  if (!provider) provider = getProviderBySignature(IMAGE_IO_DEVICE_DECODER, mime, readn);
  // Bail if signature checking failed too.
  if (!provider) { err = ERR_IMAGEIO_NO_DECODER; goto end; }

  err = provider->createDevice(IMAGE_IO_DEVICE_DECODER, reinterpret_cast<BaseDevice**>(&decoder));
  if (err == ERR_OK) decoder->attachStream(stream);

end:
  // Seek to begin if failed.
  if (err) stream.seek(pos, STREAM_SEEK_SET);

  *device = decoder;
  return err;
}

// ============================================================================
// [Fog::ImageIO::Provider]
// ============================================================================

Provider::Provider()
{
  _refCount.init(0);

  _fileType = IMAGE_IO_FILE_NONE;
  _deviceType = IMAGE_IO_DEVICE_NONE;
}

Provider::~Provider()
{
}

Provider* Provider::ref() const
{
  _refCount.inc();
  return const_cast<Provider*>(this);
}

void Provider::deref()
{
  if (_refCount.deref()) delete this;
}

bool Provider::supportsImageExtension(const String& extension) const
{
  return _imageExtensions.contains(extension);
}

// ============================================================================
// [Fog::ImageIO::BaseDevice]
// ============================================================================

BaseDevice::BaseDevice(Provider* provider) :
  _provider(provider),
  _fileType(provider->getFileType()),
  _deviceType(IMAGE_IO_DEVICE_NONE),
  _attachedOffset(FOG_UINT64_C(0)),
  _stream(),
  _width(0),
  _height(0),
  _depth(0),
  _planes(0),
  _actualFrame(0),
  _framesCount(0),
  _progress(0.0f)
{
}

BaseDevice::~BaseDevice()
{
}

// ============================================================================
// [Fog::ImageIO::BaseDevice - Properties]
// ============================================================================

err_t BaseDevice::getProperty(const ManagedString& name, Value& value) const
{
  if (name == fog_strings->getString(STR_GRAPHICS_width      )) return value.setInt32(_width);
  if (name == fog_strings->getString(STR_GRAPHICS_height     )) return value.setInt32(_height);
  if (name == fog_strings->getString(STR_GRAPHICS_depth      )) return value.setInt32(_depth);
  if (name == fog_strings->getString(STR_GRAPHICS_planes     )) return value.setInt32(_planes);
  if (name == fog_strings->getString(STR_GRAPHICS_actualFrame)) return value.setInt32(_actualFrame);
  if (name == fog_strings->getString(STR_GRAPHICS_framesCount)) return value.setInt32(_framesCount);
  if (name == fog_strings->getString(STR_GRAPHICS_progress   )) return value.setDouble(_progress);

  return base::getProperty(name, value);
}

err_t BaseDevice::setProperty(const ManagedString& name, const Value& value)
{
  if (name == fog_strings->getString(STR_GRAPHICS_width) ||
      name == fog_strings->getString(STR_GRAPHICS_height) ||
      name == fog_strings->getString(STR_GRAPHICS_depth) ||
      name == fog_strings->getString(STR_GRAPHICS_planes) ||
      name == fog_strings->getString(STR_GRAPHICS_actualFrame) ||
      name == fog_strings->getString(STR_GRAPHICS_framesCount) ||
      name == fog_strings->getString(STR_GRAPHICS_progress))
  {
    return ERR_OBJECT_READ_ONLY_PROPERTY;
  }

  return base::setProperty(name, value);
}

// ============================================================================
// [Fog::ImageIO::BaseDevice - Progress]
// ============================================================================

void BaseDevice::updateProgress(float value)
{
  _progress = value;
}

void BaseDevice::updateProgress(uint32_t y, uint32_t height)
{
  updateProgress((float)( (double)y / (double)height ));
}

// ============================================================================
// [Fog::ImageIO::BaseDevice - Image Size]
// ============================================================================

bool BaseDevice::checkImageSize() const 
{
  return (
    (_width  > 0) & (_width  <= IMAGE_MAX_WIDTH ) &
    (_height > 0) & (_height <= IMAGE_MAX_HEIGHT) );
}

// ============================================================================
// [Fog::ImageIO::BaseDevice - Stream]
// ============================================================================

void BaseDevice::attachStream(Stream& stream)
{
  // detach current stream first
  if (_stream.isOpen()) detachStream();

  _stream = stream;
  _attachedOffset = (uint64_t)_stream.tell();
}

void BaseDevice::detachStream()
{
  if (_stream.isOpen())
  {
    reset();
    _stream.close();
  }
}

// ============================================================================
// [Fog::ImageIO::BaseDevice - Reset]
// ============================================================================

void BaseDevice::reset()
{
  _attachedOffset = FOG_UINT64_C(0);
  _width = 0;
  _height = 0;
  _depth = 0;
  _planes = 0;
  _actualFrame = 0;
  _framesCount = 0;
  _progress = 0.0f;

  _format = IMAGE_FORMAT_NULL;

  _palette.free();
  _comment.free();
}

// ============================================================================
// [Fog::ImageIO::DecoderDevice]
// ============================================================================

DecoderDevice::DecoderDevice(Provider* provider) :
  BaseDevice(provider),
  _headerDone(false),
  _readerDone(false),
  _headerResult(ERR_OK),
  _readerResult(ERR_OK)
{
  _deviceType = IMAGE_IO_DEVICE_DECODER;
}

DecoderDevice::~DecoderDevice()
{
}

// [Reset]

void DecoderDevice::reset()
{
  BaseDevice::reset();

  _headerDone = false;
  _readerDone = false;

  _headerResult = ERR_OK;
  _readerResult = ERR_OK;
}

// ============================================================================
// [EncoderDevice]
// ============================================================================

EncoderDevice::EncoderDevice(Provider* provider) :
  BaseDevice(provider),
  _headerDone(false),
  _writerDone(false)
{
  _deviceType = IMAGE_IO_DEVICE_ENCODER;
}

EncoderDevice::~EncoderDevice()
{
}

void EncoderDevice::detachStream()
{
  finalize();

  BaseDevice::detachStream();
}

void EncoderDevice::reset()
{
  BaseDevice::reset();

  _headerDone = false;
  _writerDone = false;
}

void EncoderDevice::finalize()
{
  _headerDone = false;
  _writerDone = false;
}

} // ImageIO namespace
} // Fog namespace

// ============================================================================
// [Library Initializers]
// ============================================================================

FOG_INIT_DECLARE err_t fog_imageio_init(void)
{
  using namespace Fog;

  // Init imageio_local.
  imageio_local.init();

  // Init all built-in providers.
  fog_imageio_init_bmp();
  fog_imageio_init_gif();
  fog_imageio_init_ico();
  fog_imageio_init_jpeg();
  fog_imageio_init_pcx();
  fog_imageio_init_png();

#if defined(FOG_OS_WINDOWS)
  fog_imageio_init_gdiplus();
#endif // FOG_OS_WINDOWS

  return ERR_OK;
}

FOG_INIT_DECLARE void fog_imageio_shutdown(void)
{
  using namespace Fog;

  // Remove (and delete) all providers
  //
  // Do not need to lock, because we are shutting down. All threads should
  // been already destroyed.
  {
    List<ImageIO::Provider*>::ConstIterator it(imageio_local->decoderProviders);
    for (it.toStart(); it.isValid(); it.toNext()) it.value()->deref();
  }

  {
    List<ImageIO::Provider*>::ConstIterator it(imageio_local->encoderProviders);
    for (it.toStart(); it.isValid(); it.toNext()) it.value()->deref();
  }

  // Shutdown imageio_local.
  imageio_local.destroy();
}
