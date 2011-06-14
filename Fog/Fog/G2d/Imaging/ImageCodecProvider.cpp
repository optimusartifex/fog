// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Collection/List.h>
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Global/Static.h>
#include <Fog/Core/IO/FileSystem.h>
#include <Fog/Core/IO/MapFile.h>
#include <Fog/Core/IO/Stream.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Tools/ManagedString.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringTmp_p.h>
#include <Fog/Core/Tools/Strings.h>
#include <Fog/Core/Tools/TextCodec.h>
#include <Fog/G2d/Global/Constants.h>
#include <Fog/G2d/Global/Init_G2d_p.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/ImageCodec.h>
#include <Fog/G2d/Imaging/ImageCodecProvider.h>
#include <Fog/G2d/Imaging/ImageDecoder.h>
#include <Fog/G2d/Imaging/ImageEncoder.h>

namespace Fog {

// ============================================================================
// [Fog::ImageCodecProvider Initializers]
// ============================================================================

FOG_NO_EXPORT void _g2d_imagecodecprovider_init_bmp(void);
FOG_NO_EXPORT void _g2d_imagecodecprovider_init_gif(void);
FOG_NO_EXPORT void _g2d_imagecodecprovider_init_ico(void);
FOG_NO_EXPORT void _g2d_imagecodecprovider_init_jpeg(void);
FOG_NO_EXPORT void _g2d_imagecodecprovider_init_pcx(void);
FOG_NO_EXPORT void _g2d_imagecodecprovider_init_png(void);

#if defined(FOG_OS_WINDOWS)
FOG_NO_EXPORT void _g2d_imagecodecprovider_init_gdip(void);
#endif // FOG_OS_WINDOWS

// ============================================================================
// [Fog::ImageCodecProvider - Local]
// ============================================================================

struct ImageCodecProvider_Local
{
  // Critical section for accessing members.
  Lock lock;

  // List of decoder providers.
  List<ImageCodecProvider*> decoderProviders;
  // List of encoder providers.
  List<ImageCodecProvider*> encoderProviders;
};

static Static<ImageCodecProvider_Local> _g2d_imagecodecprovider_local;

// ============================================================================
// [Fog::ImageCodecProvider - Construction / Destruction]
// ============================================================================

ImageCodecProvider::ImageCodecProvider()
{
  _refCount.init(0);

  _codecType = IMAGE_CODEC_NONE;
  _streamType = IMAGE_STREAM_NONE;
}

ImageCodecProvider::~ImageCodecProvider()
{
}

// ============================================================================
// [Fog::ImageCodecProvider - Ref / Deref]
// ============================================================================

ImageCodecProvider* ImageCodecProvider::ref() const
{
  _refCount.inc();
  return const_cast<ImageCodecProvider*>(this);
}

void ImageCodecProvider::deref()
{
  if (_refCount.deref()) fog_delete(this);
}

// ============================================================================
// [Fog::ImageCodecProvider - Provider]
// ============================================================================

bool ImageCodecProvider::supportsImageExtension(const String& extension) const
{
  return _imageExtensions.contains(extension);
}

// ============================================================================
// [Fog::ImageCodecProvider - Statics]
// ============================================================================

err_t ImageCodecProvider::addProvider(uint32_t codecType, ImageCodecProvider* provider)
{
  if ((codecType & (IMAGE_CODEC_DECODER | IMAGE_CODEC_ENCODER)) == 0)
    return ERR_RT_INVALID_ARGUMENT;

  err_t err = ERR_OK;
  AutoLock locked(_g2d_imagecodecprovider_local->lock);

  if ((codecType & IMAGE_CODEC_DECODER) &&
      (_g2d_imagecodecprovider_local->decoderProviders.indexOf(provider) == INVALID_INDEX))
  {
    err |= _g2d_imagecodecprovider_local->decoderProviders.append(provider);
    if (FOG_IS_ERROR(err)) goto _End;
    provider->ref();
  }

  if ((codecType & IMAGE_CODEC_ENCODER) &&
      (_g2d_imagecodecprovider_local->encoderProviders.indexOf(provider) == INVALID_INDEX))
  {
    err |= _g2d_imagecodecprovider_local->encoderProviders.append(provider);
    if (FOG_IS_ERROR(err)) goto _End;
    provider->ref();
  }

_End:
  return err;
}

err_t ImageCodecProvider::removeProvider(uint32_t codecType, ImageCodecProvider* provider)
{
  if ((codecType & (IMAGE_CODEC_DECODER | IMAGE_CODEC_ENCODER)) == 0)
    return ERR_RT_INVALID_ARGUMENT;

  err_t err = ERR_OK;
  size_t index;
  AutoLock locked(_g2d_imagecodecprovider_local->lock);

  if ((codecType & IMAGE_CODEC_DECODER) &&
      (index = _g2d_imagecodecprovider_local->decoderProviders.indexOf(provider)) != INVALID_INDEX)
  {
    err |= _g2d_imagecodecprovider_local->decoderProviders.removeAt(index);
    if (FOG_IS_ERROR(err)) goto _End;
    provider->deref();
  }

  if ((codecType & IMAGE_CODEC_ENCODER) &&
      (index = _g2d_imagecodecprovider_local->encoderProviders.indexOf(provider)) != INVALID_INDEX)
  {
    err |= _g2d_imagecodecprovider_local->encoderProviders.removeAt(index);
    if (FOG_IS_ERROR(err)) goto _End;
    provider->deref();
  }

_End:
  return err;
}

bool ImageCodecProvider::hasProvider(uint32_t codecType, ImageCodecProvider* provider)
{
  AutoLock locked(_g2d_imagecodecprovider_local->lock);

  switch (codecType & (IMAGE_CODEC_DECODER | IMAGE_CODEC_ENCODER))
  {
    case IMAGE_CODEC_DECODER:
      return _g2d_imagecodecprovider_local->decoderProviders.indexOf(provider) != INVALID_INDEX;
    case IMAGE_CODEC_ENCODER:
      return _g2d_imagecodecprovider_local->encoderProviders.indexOf(provider) != INVALID_INDEX;
    case IMAGE_CODEC_DECODER | IMAGE_CODEC_ENCODER:
      return _g2d_imagecodecprovider_local->decoderProviders.indexOf(provider) != INVALID_INDEX &&
             _g2d_imagecodecprovider_local->encoderProviders.indexOf(provider) != INVALID_INDEX;
    default:
      return false;
  }
}

List<ImageCodecProvider*> ImageCodecProvider::getProviders(uint32_t codecType)
{
  AutoLock locked(_g2d_imagecodecprovider_local->lock);

  if ((codecType & (IMAGE_CODEC_DECODER | IMAGE_CODEC_ENCODER)) == IMAGE_CODEC_ENCODER)
    return _g2d_imagecodecprovider_local->encoderProviders;

  if ((codecType & (IMAGE_CODEC_DECODER | IMAGE_CODEC_ENCODER)) == IMAGE_CODEC_DECODER)
    return _g2d_imagecodecprovider_local->decoderProviders;

  return List<ImageCodecProvider*>();
}

ImageCodecProvider* ImageCodecProvider::getProviderByName(uint32_t codecType, const String& name)
{
  List<ImageCodecProvider*> providers = getProviders(codecType);
  List<ImageCodecProvider*>::ConstIterator it(providers);

  for (; it.isValid(); it.toNext())
  {
    if (it.value()->getName() == name) return it.value();
  }

  return 0;
}

ImageCodecProvider* ImageCodecProvider::getProviderByExtension(uint32_t codecType, const String& extension)
{
  List<ImageCodecProvider*> providers = getProviders(codecType);
  List<ImageCodecProvider*>::ConstIterator it(providers);

  // Convert extension to lower case.
  StringTmp<16> e(extension);
  e.lower();

  for (it.toStart(); it.isValid(); it.toNext())
  {
    if (it.value()->supportsImageExtension(e)) return it.value();
  }

  return NULL;
}

ImageCodecProvider* ImageCodecProvider::getProviderBySignature(uint32_t codecType, void* mem, size_t len)
{
  if (!mem || len == 0) return NULL;

  List<ImageCodecProvider*> providers = getProviders(codecType);
  List<ImageCodecProvider*>::ConstIterator it(providers);

  uint32_t bestScore = 0;
  ImageCodecProvider* bestProvider = NULL;

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

static err_t createImageCodecByName(uint32_t codecType, const String& name, ImageCodec** codec)
{
  err_t err = ERR_OK;
  if (codec == NULL) return ERR_RT_INVALID_ARGUMENT;

  ImageCodecProvider* provider = ImageCodecProvider::getProviderByName(codecType, name);

  err = (!provider)
    ? (codecType == IMAGE_CODEC_DECODER
      ? ERR_IMAGE_NO_DECODER
      : ERR_IMAGE_NO_ENCODER)
    : provider->createCodec(codecType, codec);

  return err;
}

err_t ImageCodecProvider::createDecoderByName(const String& name, ImageDecoder** codec)
{
  return createImageCodecByName(
    IMAGE_CODEC_DECODER, name, reinterpret_cast<ImageCodec**>(codec));
}

err_t ImageCodecProvider::createEncoderByName(const String& name, ImageEncoder** codec)
{
  return createImageCodecByName(
    IMAGE_CODEC_ENCODER, name, reinterpret_cast<ImageCodec**>(codec));
}

static err_t createImageCodecByExtension(uint32_t codecType, const String& extension, ImageCodec** codec)
{
  err_t err = ERR_OK;
  if (codec == NULL) return ERR_RT_INVALID_ARGUMENT;

  ImageCodecProvider* provider = ImageCodecProvider::getProviderByExtension(codecType, extension);

  err = (!provider)
    ? (codecType == IMAGE_CODEC_DECODER
      ? ERR_IMAGE_NO_DECODER
      : ERR_IMAGE_NO_ENCODER)
    : provider->createCodec(codecType, codec);

  return err;
}

err_t ImageCodecProvider::createDecoderByExtension(const String& extension, ImageDecoder** codec)
{
  return createImageCodecByExtension(
    IMAGE_CODEC_DECODER, extension, reinterpret_cast<ImageCodec**>(codec));
}

err_t ImageCodecProvider::createEncoderByExtension(const String& extension, ImageEncoder** codec)
{
  return createImageCodecByExtension(
    IMAGE_CODEC_ENCODER, extension, reinterpret_cast<ImageCodec**>(codec));
}

err_t ImageCodecProvider::createDecoderForFile(const String& fileName, ImageDecoder** codec)
{
  if (codec == NULL) return ERR_RT_INVALID_ARGUMENT;

  Stream stream;
  String extension;

  err_t err = stream.openMMap(fileName, false);
  // MMap failed? Try to open the file using standard stream.
  if (FOG_IS_ERROR(err)) err = stream.openFile(fileName, STREAM_OPEN_READ);

  // If err is not ERR_OK then file can't be open.
  if (FOG_IS_ERROR(err)) goto _End;

  // Extract extension to help opening from stream.
  if ((err = FileSystem::extractExtension(extension, fileName)) || (err = extension.lower())) goto _End;

  // Finally, create codec.
  err = createDecoderForStream(stream, extension, codec);

_End:
  return err;
}

err_t ImageCodecProvider::createDecoderForStream(Stream& stream, const String& extension, ImageDecoder** codec)
{
  if (codec == NULL) return ERR_RT_INVALID_ARGUMENT;

  err_t err = ERR_OK;

  ImageCodecProvider* provider = NULL;
  ImageDecoder* decoder = NULL;

  int64_t pos;

  // This is default, 128 bytes should be enough for any image file type.
  uint8_t mime[128];
  size_t readn;

  if (!stream.isSeekable()) { err = ERR_IO_CANT_SEEK; goto _End; }

  pos = stream.tell();
  readn = stream.read(mime, 128);

  if (stream.seek(pos, STREAM_SEEK_SET) == -1) { err =  ERR_IO_CANT_SEEK; goto _End; }

  // First try to use extension.
  if (!extension.isEmpty()) provider = getProviderByExtension(IMAGE_CODEC_DECODER, extension);
  // Fallback to signature checking if extension match failed.
  if (!provider) provider = getProviderBySignature(IMAGE_CODEC_DECODER, mime, readn);
  // Bail if signature checking failed too.
  if (!provider) { err = ERR_IMAGE_NO_DECODER; goto _End; }

  err = provider->createCodec(IMAGE_CODEC_DECODER, reinterpret_cast<ImageCodec**>(&decoder));
  if (err == ERR_OK) decoder->attachStream(stream);

_End:
  // Seek to begin if failed.
  if (FOG_IS_ERROR(err)) stream.seek(pos, STREAM_SEEK_SET);

  *codec = decoder;
  return err;
}

// ============================================================================
// [Fog::G2d - Library Initializers]
// ============================================================================

FOG_NO_EXPORT void _g2d_imagecodecprovider_init(void)
{
  // Init _g2d_imagecodecprovider_local.
  _g2d_imagecodecprovider_local.init();

  // Init all built-in providers.
  _g2d_imagecodecprovider_init_bmp();
  _g2d_imagecodecprovider_init_gif();
  _g2d_imagecodecprovider_init_ico();
  _g2d_imagecodecprovider_init_pcx();

#if defined(FOG_HAVE_LIBJPEG)
  _g2d_imagecodecprovider_init_jpeg();
#endif // FOG_HAVE_LIBJPEG

#if defined(FOG_HAVE_LIBPNG)
  _g2d_imagecodecprovider_init_png();
#endif // FOG_HAVE_LIBPNG

#if defined(FOG_OS_WINDOWS)
  _g2d_imagecodecprovider_init_gdip();
#endif // FOG_OS_WINDOWS
}

FOG_NO_EXPORT void _g2d_imagecodecprovider_fini(void)
{
  // Remove (and delete) all providers
  //
  // Do not need to lock, because we are shutting down. All threads should
  // been already destroyed.
  {
    List<ImageCodecProvider*>::ConstIterator it(_g2d_imagecodecprovider_local->decoderProviders);
    for (it.toStart(); it.isValid(); it.toNext()) it.value()->deref();
  }

  {
    List<ImageCodecProvider*>::ConstIterator it(_g2d_imagecodecprovider_local->encoderProviders);
    for (it.toStart(); it.isValid(); it.toNext()) it.value()->deref();
  }

  // Shutdown _g2d_imagecodecprovider_local.
  _g2d_imagecodecprovider_local.destroy();
}

} // Fog namespace
