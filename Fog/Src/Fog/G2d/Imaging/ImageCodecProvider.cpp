// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/OS/FilePath.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Tools/InternedString.h>
#include <Fog/Core/Tools/List.h>
#include <Fog/Core/Tools/Stream.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringTmp_p.h>
#include <Fog/Core/Tools/TextCodec.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/ImageCodec.h>
#include <Fog/G2d/Imaging/ImageCodecProvider.h>
#include <Fog/G2d/Imaging/ImageDecoder.h>
#include <Fog/G2d/Imaging/ImageEncoder.h>

namespace Fog {

// ============================================================================
// [Fog::ImageCodecProvider Initializers]
// ============================================================================

FOG_NO_EXPORT void ImageCodecProvider_initBMP(void);
FOG_NO_EXPORT void ImageCodecProvider_initGIF(void);
FOG_NO_EXPORT void ImageCodecProvider_initICO(void);
FOG_NO_EXPORT void ImageCodecProvider_initJPEG(void);
FOG_NO_EXPORT void ImageCodecProvider_initPCX(void);
FOG_NO_EXPORT void ImageCodecProvider_initPNG(void);

#if defined(FOG_OS_WINDOWS)
FOG_NO_EXPORT void ImageCodecProvider_initWinGdip(void);
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_MAC)
FOG_NO_EXPORT void ImageCodecProvider_initMacCG(void);
#endif // FOG_OS_MAC

// ============================================================================
// [Fog::ImageCodecProvider - Global]
// ============================================================================

struct ImageCodecProvider_Global
{
  // Critical section for accessing members.
  Lock lock;

  // List of image-codec providers.
  List<ImageCodecProvider*> providers;
};

static Static<ImageCodecProvider_Global> ImageCodecProvider_global;

// ============================================================================
// [Fog::ImageCodecProvider - Construction / Destruction]
// ============================================================================

ImageCodecProvider::ImageCodecProvider()
{
  _reference.init(1);

  _codecType = IMAGE_CODEC_NONE;
  _streamType = IMAGE_STREAM_NONE;
}

ImageCodecProvider::~ImageCodecProvider()
{
}

// ============================================================================
// [Fog::ImageCodecProvider - Ref / Deref]
// ============================================================================

ImageCodecProvider* ImageCodecProvider::addRef() const
{
  _reference.inc();
  return const_cast<ImageCodecProvider*>(this);
}

void ImageCodecProvider::deref()
{
  if (_reference.deref())
    fog_delete(this);
}

// ============================================================================
// [Fog::ImageCodecProvider - Provider]
// ============================================================================

bool ImageCodecProvider::supportsImageExtension(const StringW& extension) const
{
  return _imageExtensions.contains(extension);
}

// ============================================================================
// [Fog::ImageCodecProvider - Statics]
// ============================================================================

err_t ImageCodecProvider::addProvider(ImageCodecProvider* provider)
{
  if (FOG_IS_NULL(provider))
    return ERR_RT_INVALID_ARGUMENT;

  AutoLock locked(ImageCodecProvider_global->lock);

  if (ImageCodecProvider_global->providers.indexOf(provider) == INVALID_INDEX)
  {
    FOG_RETURN_ON_ERROR(ImageCodecProvider_global->providers.append(provider));
    provider->addRef();
  }

  return ERR_OK;
}

err_t ImageCodecProvider::removeProvider(ImageCodecProvider* provider)
{
  if (FOG_IS_NULL(provider))
    return ERR_RT_INVALID_ARGUMENT;

  AutoLock locked(ImageCodecProvider_global->lock);

  size_t index = ImageCodecProvider_global->providers.indexOf(provider);
  if (index == INVALID_INDEX) return ERR_RT_OBJECT_NOT_FOUND;

  FOG_RETURN_ON_ERROR(ImageCodecProvider_global->providers.removeAt(index));

  provider->deref();
  return ERR_OK;
}

bool ImageCodecProvider::hasProvider(ImageCodecProvider* provider)
{
  AutoLock locked(ImageCodecProvider_global->lock);
  return ImageCodecProvider_global->providers.indexOf(provider) != INVALID_INDEX;
}

List<ImageCodecProvider*> ImageCodecProvider::getProviders()
{
  AutoLock locked(ImageCodecProvider_global->lock);
  return ImageCodecProvider_global->providers;
}

ImageCodecProvider* ImageCodecProvider::getProviderByName(uint32_t codecType, const StringW& name)
{
  if (codecType == IMAGE_CODEC_NONE || codecType > IMAGE_CODEC_BOTH)
    return NULL;

  List<ImageCodecProvider*> providers = getProviders();
  ListIterator<ImageCodecProvider*> it(providers);

  while (it.isValid())
  {
    ImageCodecProvider* provider = it.getItem();
    if ((provider->getCodecType() & codecType) == codecType && provider->getName() == name)
      return provider;
    it.next();
  }

  return 0;
}

ImageCodecProvider* ImageCodecProvider::getProviderByExtension(uint32_t codecType, const StringW& extension)
{
  if (codecType == IMAGE_CODEC_NONE || codecType > IMAGE_CODEC_BOTH)
    return NULL;

  List<ImageCodecProvider*> providers = getProviders();
  ListIterator<ImageCodecProvider*> it(providers);

  // Convert extension to lower case.
  StringTmpW<16> e(extension);
  e.lower();

  while (it.isValid())
  {
    ImageCodecProvider* provider = it.getItem();
    if ((provider->getCodecType() & codecType) == codecType && provider->supportsImageExtension(e))
      return provider;
    it.next();
  }

  return NULL;
}

ImageCodecProvider* ImageCodecProvider::getProviderBySignature(uint32_t codecType, void* mem, size_t len)
{
  if (codecType == IMAGE_CODEC_NONE || codecType > IMAGE_CODEC_BOTH)
    return NULL;

  if (mem == NULL || len == 0)
    return NULL;

  List<ImageCodecProvider*> providers = getProviders();
  ListIterator<ImageCodecProvider*> it(providers);

  uint32_t bestScore = 0;
  ImageCodecProvider* bestProvider = NULL;

  while (it.isValid())
  {
    ImageCodecProvider* provider = it.getItem();

    if ((provider->getCodecType() & codecType) != codecType)
      continue;

    uint32_t score = provider->checkSignature(mem, len);
    if (score > bestScore)
    {
      bestScore = score;
      bestProvider = provider;
    }

    it.next();
  }

  return bestProvider;
}

static err_t createImageCodecByName(uint32_t codecType, const StringW& name, ImageCodec** codec)
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

err_t ImageCodecProvider::createDecoderByName(const StringW& name, ImageDecoder** codec)
{
  return createImageCodecByName(
    IMAGE_CODEC_DECODER, name, reinterpret_cast<ImageCodec**>(codec));
}

err_t ImageCodecProvider::createEncoderByName(const StringW& name, ImageEncoder** codec)
{
  return createImageCodecByName(
    IMAGE_CODEC_ENCODER, name, reinterpret_cast<ImageCodec**>(codec));
}

static err_t createImageCodecByExtension(uint32_t codecType, const StringW& extension, ImageCodec** codec)
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

err_t ImageCodecProvider::createDecoderByExtension(const StringW& extension, ImageDecoder** codec)
{
  return createImageCodecByExtension(
    IMAGE_CODEC_DECODER, extension, reinterpret_cast<ImageCodec**>(codec));
}

err_t ImageCodecProvider::createEncoderByExtension(const StringW& extension, ImageEncoder** codec)
{
  return createImageCodecByExtension(
    IMAGE_CODEC_ENCODER, extension, reinterpret_cast<ImageCodec**>(codec));
}

err_t ImageCodecProvider::createDecoderForFile(const StringW& fileName, ImageDecoder** codec)
{
  if (codec == NULL) return ERR_RT_INVALID_ARGUMENT;

  Stream stream;
  StringW extension;

  err_t err = stream.openMMap(fileName, false);
  // MMap failed? Try to open the file using standard stream.
  if (FOG_IS_ERROR(err)) err = stream.openFile(fileName, STREAM_OPEN_READ);

  // If err is not ERR_OK then file can't be open.
  if (FOG_IS_ERROR(err)) goto _End;

  // Extract extension to help opening from stream.
  if ((err = FilePath::extractExtension(extension, fileName)) || (err = extension.lower())) goto _End;

  // Finally, create codec.
  err = createDecoderForStream(stream, extension, codec);

_End:
  return err;
}

err_t ImageCodecProvider::createDecoderForStream(Stream& stream, const StringW& extension, ImageDecoder** codec)
{
  if (codec == NULL)
    return ERR_RT_INVALID_ARGUMENT;

  err_t err = ERR_OK;

  ImageCodecProvider* provider = NULL;
  ImageDecoder* decoder = NULL;

  int64_t pos;

  // This is default, 128 bytes should be enough for any image file type.
  uint8_t mime[128];
  size_t readn;

  if (!stream.isSeekable())
  {
    err = ERR_IO_CANT_SEEK;
    goto _End;
  }

  pos = stream.tell();
  readn = stream.read(mime, 128);

  if (stream.seek(pos, STREAM_SEEK_SET) == -1)
  {
    err =  ERR_IO_CANT_SEEK;
    goto _End;
  }

  // First try to use extension.
  if (!extension.isEmpty())
    provider = getProviderByExtension(IMAGE_CODEC_DECODER, extension);
  
  // Fallback to signature checking if extension match failed.
  if (provider == NULL)
    provider = getProviderBySignature(IMAGE_CODEC_DECODER, mime, readn);
  
  // Bail if signature checking failed too.
  if (provider == NULL)
  {
    err = ERR_IMAGE_NO_DECODER;
    goto _End;
  }

  err = provider->createCodec(IMAGE_CODEC_DECODER, reinterpret_cast<ImageCodec**>(&decoder));
  if (err == ERR_OK)
    decoder->attachStream(stream);

_End:
  // Seek to begin if failed.
  if (FOG_IS_ERROR(err))
    stream.seek(pos, STREAM_SEEK_SET);

  *codec = decoder;
  return err;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void ImageCodecProvider_init(void)
{
  // Global.
  ImageCodecProvider_global.init();

  // Init all built-in providers.
  ImageCodecProvider_initBMP();
  ImageCodecProvider_initGIF();
  ImageCodecProvider_initICO();
  ImageCodecProvider_initPCX();

#if defined(FOG_HAVE_LIBJPEG)
  ImageCodecProvider_initJPEG();
#endif // FOG_HAVE_LIBJPEG

#if defined(FOG_HAVE_LIBPNG)
  ImageCodecProvider_initPNG();
#endif // FOG_HAVE_LIBPNG

#if defined(FOG_OS_WINDOWS)
  ImageCodecProvider_initWinGdip();
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_MAC)
  ImageCodecProvider_initMacCG();
#endif // FOG_OS_MAC
}

FOG_NO_EXPORT void ImageCodecProvider_fini(void)
{
  // Remove (and delete) all providers
  //
  // Do not need to lock, because we are shutting down. All threads have
  // should be already destroyed.
  ListIterator<ImageCodecProvider*> it(ImageCodecProvider_global->providers);
  while (it.isValid())
  {
    it.getItem()->deref();
    it.next();
  }

  // Shutdown.
  ImageCodecProvider_global.destroy();
}

} // Fog namespace
