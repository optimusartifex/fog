// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Guard]
#include <Fog/Core/C++/Base.h>
#if defined(FOG_OS_MAC)

// [Dependencies]
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/OS/MacUtil.h>
#include <Fog/Core/Tools/InternedString.h>
#include <Fog/Core/Tools/Stream.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/G2d/Imaging/Codecs/MacCGCodec_p.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/ImageConverter.h>
#include <Fog/G2d/Imaging/ImageFormatDescription.h>

FOG_IMPLEMENT_OBJECT(Fog::MacCGDecoder)
FOG_IMPLEMENT_OBJECT(Fog::MacCGEncoder)

namespace Fog {

// ===========================================================================
// [Fog::MacCGCodecProvider]
// ===========================================================================

MacCGCodecProvider::MacCGCodecProvider(uint32_t streamType)
{
  // Supported codecs.
  _codecType = IMAGE_CODEC_BOTH;

  // Supported streams.
  _streamType = streamType;

  // Name of ImageCodecProvider.
  switch (_streamType)
  {
    case IMAGE_STREAM_JPEG:
      _name = FOG_S(JPEG);
      break;
    case IMAGE_STREAM_PNG:
      _name = FOG_S(PNG);
      break;
    case IMAGE_STREAM_TIFF:
      _name = FOG_S(TIFF);
      break;
    default:
      FOG_ASSERT_NOT_REACHED();
  }

  // All CG providers contains "[CG]" suffix.
  _name.append(Ascii8("[CG]"));

  // Supported extensions.
  switch (_streamType)
  {
    case IMAGE_STREAM_JPEG:
      _imageExtensions.reserve(4);
      _imageExtensions.append(FOG_S(jpg));
      _imageExtensions.append(FOG_S(jpeg));
      _imageExtensions.append(FOG_S(jfi));
      _imageExtensions.append(FOG_S(jfif));
      break;
    case IMAGE_STREAM_PNG:
      _imageExtensions.reserve(1);
      _imageExtensions.append(FOG_S(png));
      break;
    case IMAGE_STREAM_TIFF:
      _imageExtensions.reserve(2);
      _imageExtensions.append(FOG_S(tif));
      _imageExtensions.append(FOG_S(tiff));
      break;
    default:
      FOG_ASSERT_NOT_REACHED();
  }
}

MacCGCodecProvider::~MacCGCodecProvider()
{
}

uint32_t MacCGCodecProvider::checkSignature(const void* mem, size_t length) const
{
  // Borrowed from WinGdipCodec.
  if (!mem || length == 0)
    return 0;

  uint32_t score = 0;
  size_t i;

  // Mime data.
  static const uint8_t mimeJPEG[2]    = { 0xFF, 0xD8 };
  static const uint8_t mimePNG[8]     = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
  static const uint8_t mimeTIFF_LE[4] = { 0x49, 0x49, 0x00, 0x42 };
  static const uint8_t mimeTIFF_BE[4] = { 0x4D, 0x4D, 0x42, 0x00 };

  // Mime check.
  switch (_streamType)
  {
    case IMAGE_STREAM_JPEG:
      i = Math::min<size_t>(length, 2);
      if (memcmp(mem, mimeJPEG, i) == 0)
        score = Math::max<uint32_t>(score, 14 + ((uint32_t)i * 40));
      break;

    case IMAGE_STREAM_PNG:
      i = Math::min<size_t>(length, 8);
      if (memcmp(mem, mimePNG, i) == 0)
        score = Math::max<uint32_t>(score, 14 + ((uint32_t)i * 10));
      break;

    case IMAGE_STREAM_TIFF:
      i = Math::min<size_t>(length, 4);
      if (memcmp(mem, mimeTIFF_LE, i) == 0 || memcmp(mem, mimeTIFF_BE, i) == 0)
        score = Math::max<uint32_t>(score, 14 + ((uint32_t)i * 20));
      break;

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  return score;
}

err_t MacCGCodecProvider::createCodec(uint32_t codecType, ImageCodec** codec) const
{
  FOG_ASSERT(codec != NULL);

  ImageCodec* c = NULL;
  switch (codecType)
  {
    case IMAGE_CODEC_DECODER:
      c = fog_new MacCGDecoder(const_cast<MacCGCodecProvider*>(this));
      break;
    case IMAGE_CODEC_ENCODER:
      c = fog_new MacCGEncoder(const_cast<MacCGCodecProvider*>(this));
      break;
    default:
      return ERR_RT_INVALID_ARGUMENT;
  }

  if (FOG_IS_NULL(c))
    return ERR_RT_OUT_OF_MEMORY;

  *codec = c;
  return ERR_OK;
}

CFStringRef MacCGCodecProvider::getUTType() const
{
  switch (_streamType)
  {
    case IMAGE_STREAM_JPEG: return kUTTypeJPEG;
    case IMAGE_STREAM_PNG : return kUTTypePNG;
    case IMAGE_STREAM_TIFF: return kUTTypeTIFF;

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  return NULL;
}

// ===========================================================================
// [Fog::MacCGDecoder - Helpers]
// ===========================================================================

static size_t FOG_CDECL MacCGProvider_getBytes(void* info, void* buffer, size_t count)
{
  MacCGDecoder* decoder = static_cast<MacCGDecoder*>(info);
  return decoder->_stream.read(buffer, count);
}

static off_t FOG_CDECL MacCGProvider_skipBytes(void* info, off_t count)
{
  MacCGDecoder* decoder = static_cast<MacCGDecoder*>(info);
  return decoder->_stream.seek(count, STREAM_SEEK_CUR) - decoder->_attachedOffset;
}

static void FOG_CDECL MacCGProvider_rewind(void* info)
{
  MacCGDecoder* decoder = static_cast<MacCGDecoder*>(info);
  decoder->_stream.seek(decoder->_attachedOffset, STREAM_SEEK_SET);
}

static void FOG_CDECL MacCGProvider_release(void* info)
{
  FOG_UNUSED(info);
}

// ===========================================================================
// [Fog::MacCGDecoder - Construction / Destruction]
// ===========================================================================

MacCGDecoder::MacCGDecoder(ImageCodecProvider* provider) :
  ImageDecoder(provider)
{
  _providerCallbacks.version = 0;
  _providerCallbacks.getBytes = MacCGProvider_getBytes;
  _providerCallbacks.skipForward = MacCGProvider_skipBytes;
  _providerCallbacks.rewind = MacCGProvider_rewind;
  _providerCallbacks.releaseInfo = MacCGProvider_release;
}

MacCGDecoder::~MacCGDecoder()
{
}

// ===========================================================================
// [Fog::MacCGDecoder - AttachStream / DetachStream]
// ===========================================================================

void MacCGDecoder::attachStream(Stream& stream)
{
  Base::attachStream(stream);

  CGDataProviderRef cgDataProvider = CGDataProviderCreateSequential(this, &_providerCallbacks);
  if (cgDataProvider != NULL)
  {
    _cgImageSource = CGImageSourceCreateWithDataProvider(cgDataProvider, NULL);
    CFRelease(cgDataProvider);
  }
}

void MacCGDecoder::detachStream()
{
  if (_cgImageSource != NULL)
  {
    CFRelease(_cgImageSource);
    _cgImageSource = NULL;
  }

  Base::detachStream();
}

// ===========================================================================
// [Fog::MacCGDecoder - Reset]
// ===========================================================================

void MacCGDecoder::reset()
{
  Base::reset();
}

// ===========================================================================
// [Fog::MacCGDecoder - ReadHeader]
// ===========================================================================

err_t MacCGDecoder::readHeader()
{
  // Do not read header more than once.
  if (_headerResult)
    return _headerResult;

  CFDictionaryRef cgDict = CGImageSourceCopyProperties(_cgImageSource, NULL);
  if (cgDict == NULL)
    return (_headerResult = ERR_IMAGEIO_INTERNAL_ERROR);

  // TODO: How to get width/height/depth and other information from the
  // cgDict?
  

  CFRelease(cgDict);
  return ERR_OK;
}

// ===========================================================================
// [Fog::MacCGDecoder - ReadImage]
// ===========================================================================

err_t MacCGDecoder::readImage(Image& image)
{
  err_t err = ERR_OK;

  if (_cgImageSource == NULL)
    return ERR_RT_INVALID_STATE;

  if (readHeader() != ERR_OK)
    return _headerResult;

  CGImageRef cgImage = CGImageSourceCreateImageAtIndex(_cgImageSource, _actualFrame, NULL);
  if (cgImage == NULL)
  {
    if (_actualFrame == 0)
      err = ERR_IMAGEIO_INTERNAL_ERROR;
    else
      err = ERR_IMAGE_NO_FRAMES;
    goto _End;
  }

  image.fromCGImage(cgImage);
  CGImageRelease(cgImage);

_End:
  if (err == ERR_OK)
    updateProgress(1.0f);
  return (_readerResult = err);
}

// ===========================================================================
// [Fog::MacCGDecoder - Properties]
// ===========================================================================

err_t MacCGDecoder::_getProperty(const InternedStringW& name, Var& dst) const
{
  return Base::_getProperty(name, dst);
}

err_t MacCGDecoder::_setProperty(const InternedStringW& name, const Var& src)
{
  return Base::_setProperty(name, src);
}

// ===========================================================================
// [Fog::MacCGEncoder - Helpers]
// ===========================================================================

static size_t FOG_CDECL MacCGConsumer_putBytes(void* info, const void* buffer, size_t count)
{
  MacCGDecoder* decoder = static_cast<MacCGDecoder*>(info);
  return decoder->_stream.write(buffer, count);
}

static void FOG_CDECL MacCGConsumer_releaseConsumer(void* info)
{
  FOG_UNUSED(info);
}

// ===========================================================================
// [Fog::MacCGEncoder - Construction / Destruction]
// ===========================================================================

MacCGEncoder::MacCGEncoder(ImageCodecProvider* provider) :
  ImageEncoder(provider)
{
  _consumerCallbacks.putBytes = MacCGConsumer_putBytes;
  _consumerCallbacks.releaseConsumer = MacCGConsumer_releaseConsumer;
}

MacCGEncoder::~MacCGEncoder()
{
}

// ===========================================================================
// [Fog::MacCGEncoder - AttachStream / DetachStream]
// ===========================================================================

void MacCGEncoder::attachStream(Stream& stream)
{
  Base::attachStream(stream);
}

void MacCGEncoder::detachStream()
{
  Base::detachStream();
}

// ===========================================================================
// [Fog::MacCGEncoder - Reset]
// ===========================================================================

void MacCGEncoder::reset()
{
  Base::reset();
}

// ===========================================================================
// [Fog::MacCGEncoder - WriteImage]
// ===========================================================================

err_t MacCGEncoder::writeImage(const Image& image)
{
  Image tmp;
  if (image.isEmpty())
    return ERR_IMAGE_INVALID_SIZE;

  err_t err = ERR_OK;

  CGDataConsumerRef cgConsumer = NULL;
  CGImageDestinationRef cgDestination = NULL;
  CGImageRef cgImage = NULL;

  cgConsumer = CGDataConsumerCreate(this, &_consumerCallbacks);
  if (FOG_IS_NULL(cgConsumer))
  {
    err = ERR_RT_OUT_OF_MEMORY;
    goto _End;
  }

  cgDestination = CGImageDestinationCreateWithDataConsumer(cgConsumer,
    static_cast<MacCGCodecProvider*>(_provider)->getUTType(), 1, NULL);
  CFRelease(cgConsumer);

  if (FOG_IS_NULL(cgDestination))
  {
    err = ERR_RT_OUT_OF_MEMORY;
    goto _End;
  }

  if (image.getType() == IMAGE_TYPE_MAC_CG)
    cgImage = static_cast<CGImageRef>(const_cast<Image&>(image).getHandle());

  if (FOG_IS_NULL(cgImage))
  {
    err = image.toCGImage(&cgImage);
    if (FOG_IS_ERROR(err))
      goto _End;
  }

  CGImageDestinationAddImage(cgDestination, cgImage, NULL);
  if (!CGImageDestinationFinalize(cgDestination))
  {
    err = ERR_IO_CANT_WRITE;
  }

_End:
  CFRelease(cgDestination);

  if (err == ERR_OK)
    updateProgress(1.0f);
  return err;
}

// ===========================================================================
// [Fog::MacCGEncoder - Properties]
// ===========================================================================

err_t MacCGEncoder::_getProperty(const InternedStringW& name, Var& dst) const
{
  return Base::_getProperty(name, dst);
}

err_t MacCGEncoder::_setProperty(const InternedStringW& name, const Var& src)
{
  return Base::_setProperty(name, src);
}

// ===========================================================================
// [Init / Fini]
// ===========================================================================

FOG_NO_EXPORT void ImageCodecProvider_initMacCG(void)
{
  ImageCodecProvider* provider;

  provider = fog_new MacCGCodecProvider(IMAGE_STREAM_PNG);
  ImageCodecProvider::addProvider(provider);
  provider->deref();

  provider = fog_new MacCGCodecProvider(IMAGE_STREAM_JPEG);
  ImageCodecProvider::addProvider(provider);
  provider->deref();

  provider = fog_new MacCGCodecProvider(IMAGE_STREAM_TIFF);
  ImageCodecProvider::addProvider(provider);
  provider->deref();
}

} // Fog namespace

#endif // FOG_OS_MAC
