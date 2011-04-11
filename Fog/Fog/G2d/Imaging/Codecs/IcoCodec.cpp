// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/IO/Stream.h>
#include <Fog/Core/Memory/BSwap.h>
#include <Fog/Core/Memory/Memory.h>
#include <Fog/Core/Tools/ManagedString.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/Strings.h>
#include <Fog/G2d/Global/Constants.h>
#include <Fog/G2d/Imaging/Codecs/BmpCodec_p.h>
#include <Fog/G2d/Imaging/Codecs/IcoCodec_p.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/ImageConverter.h>

FOG_IMPLEMENT_OBJECT(Fog::IcoDecoder)

namespace Fog { 

// ============================================================================
// [Fog::IcoCodecProvider]
// ============================================================================

IcoCodecProvider::IcoCodecProvider()
{
  // Name of ImageCodecProvider.
  _name = fog_strings->getString(STR_G2D_STREAM_ICO);

  // Supported codecs.
  _codecType = IMAGE_CODEC_BOTH;

  // Supported streams.
  _streamType = IMAGE_STREAM_ICO;

  // Supported extensions.
  _imageExtensions.reserve(1);
  _imageExtensions.append(fog_strings->getString(STR_G2D_EXTENSION_ico));
}

IcoCodecProvider::~IcoCodecProvider()
{
}

uint32_t IcoCodecProvider::checkSignature(const void* mem, sysuint_t length) const
{
  if (!mem || length < sizeof(IcoHeader)) return 0;

  const uint8_t* m = (const uint8_t*)mem;
  
  if (*(const uint16_t*)m != 0x0000) return 0;
  if (Memory::bswap16le(*(const uint16_t*)(m+2)) != 0x0001) return 0;
  
  sysuint_t remaining = length - sizeof(IcoHeader);
  uint16_t count = Memory::bswap16le(*(const uint16_t*)(m+4));
    
  if (remaining < sizeof(IcoEntry) || count == 0)
  {
    // We cannot be much sure, because ICO files have no signature
    return 10;
  }

  uint16_t entriesAvail = 0;
  sysuint_t fOffset, fSize;
  IcoEntry *entry = (IcoEntry*)(m + sizeof(IcoHeader));
  sysuint_t minOffset = sizeof(IcoHeader) + (sysuint_t)count * sizeof(IcoEntry);

  while (remaining >= sizeof(IcoEntry))
  {
    fSize = Memory::bswap32le(entry->size);
    fOffset = Memory::bswap32le(entry->offset);
    
    if (fOffset < minOffset || fSize == 0) return 0;
    
    ++entriesAvail;
    remaining -= sizeof(IcoEntry);
    ++entry;
  }

  // TODO: I don't understand this. Need to saturate some values (count can be
  // large and entries small, maybe min(95, 40 + (entriesAvail) * 5) ?
  return Math::max(20, 80 - (count - entriesAvail) * 5);
}

err_t IcoCodecProvider::createCodec(uint32_t codecType, ImageCodec** codec) const
{
  ImageCodec* c = NULL;

  switch (codecType)
  {
    case IMAGE_CODEC_DECODER:
      c = fog_new IcoDecoder(const_cast<IcoCodecProvider*>(this));
      break;
    case IMAGE_CODEC_ENCODER:
      return ERR_IMAGE_NO_ENCODER;
    default:
      return ERR_RT_INVALID_ARGUMENT;
  }

  if (FOG_IS_NULL(c)) return ERR_RT_OUT_OF_MEMORY;

  *codec = c;
  return ERR_OK;
}

// ============================================================================
// [Fog::IcoDecoder - Construction / Destruction]
// ============================================================================

IcoDecoder::IcoDecoder(ImageCodecProvider* provider) :
  ImageDecoder(provider)
{
  _framesInfo = NULL;
}

IcoDecoder::~IcoDecoder()
{
  reset();
}

// ============================================================================
// [Fog::IcoDecoder - Reset]
// ============================================================================

void IcoDecoder::reset()
{
  ImageDecoder::reset();
  
  if (_framesInfo) Memory::free(_framesInfo);

  _framesInfo = NULL;
}

// ============================================================================
// [Fog::IcoDecoder - ReadHeader]
// ============================================================================

err_t IcoDecoder::readHeader()
{
  // Don't read header more than once.
  if (isHeaderDone()) return _headerResult;

  // Mark header as done.
  _headerDone = true;

  {
    IcoHeader icoHeader;
    if (getStream().read(&icoHeader, sizeof(IcoHeader)) != sizeof(IcoHeader))
    {
      return (_headerResult = ERR_IMAGE_TRUNCATED);
    }

    icoHeader.type = Memory::bswap16le(icoHeader.type);
    icoHeader.count = Memory::bswap16le(icoHeader.count);

    if (icoHeader.reserved != 0 || icoHeader.type != 1 /* don't read cursors (2) */)
    {
      return (_headerResult = ERR_IMAGE_MIME_NOT_MATCH);
    }

    _framesCount = icoHeader.count;
    _actualFrame = 0;
    // discard icoHeader now
  }

  // Read frames' info.
  if (_framesCount > 0)
  {
    // Added check for malformed _framesCount value right here. Is it possible
    // to have more than 1024 frames in ICO?
    if (_framesCount > 1024)
    {
      return (_headerResult = ERR_IMAGE_MALFORMED_HEADER);
    }

    sysuint_t memSize = _framesCount * sizeof(IcoEntry);
    
    _framesInfo = (IcoEntry*)Memory::alloc(memSize);
    if (_framesInfo == NULL)
    {
      return (_headerResult = ERR_RT_OUT_OF_MEMORY);
    }

    if (getStream().read(_framesInfo, memSize) != memSize)
    {
      Memory::free(_framesInfo);
      _framesInfo = NULL;
      _framesCount = 0;
      return (_headerResult = ERR_IMAGE_TRUNCATED);
    }
    
    // _currentOffset is now currently minimal allowed offset for a frame.
    int64_t _currentOffset = sizeof(IcoHeader) + memSize;
    IcoEntry *currentEntry = _framesInfo;
    
    for (sysuint_t i = 0; i < _framesCount; ++i, ++currentEntry)
    {
#if FOG_BYTE_ORDER == FOG_BIG_ENDIAN
      currentEntry->colorPlanes = Memory::bswap16le(currentEntry->colorPlanes);
      currentEntry->bpp         = Memory::bswap16le(currentEntry->bpp);
      currentEntry->size        = Memory::bswap32le(currentEntry->size);
      currentEntry->offset      = Memory::bswap32le(currentEntry->offset);
#endif
      
      if (currentEntry->offset < _currentOffset || currentEntry->size == 0)
      {
        Memory::free(_framesInfo);
        _framesInfo = NULL;
        _framesCount = 0;
        return (_headerResult = ERR_IMAGE_MALFORMED_HEADER);
      }
    }
    // TODO:  ? check individual frames if the stream is seekable ?
  }

  // Success.
  return (_headerResult = ERR_OK);
}

// ============================================================================
// [IcoDecoder::readImage]
// ============================================================================

err_t IcoDecoder::readImage(Image& image)
{
  if (readHeader() != ERR_OK) return _headerResult;

  if (_actualFrame == _framesCount || !_framesInfo) return ERR_IMAGE_NO_MORE_FRAMES;

  err_t err;

  IcoEntry *entry = _framesInfo + _actualFrame;
  ImageDecoder* decoder = NULL;

  int64_t toSeek = _attachedOffset + entry->offset;
  if (_stream.seek(toSeek, STREAM_SEEK_SET) == -1) return ERR_IO_CANT_SEEK;

  // Stream should be at frame position now, either PNG or BMP data follows...
  //
  // I read carefully documentation about ICO format and its new variants and it
  // seems that PNG image format is only used if WIDTH and HEIGHT in ICO entry
  // were set to zeros. Some documentation says that this means that ICO is
  // 256x256 pixels big, but I thing that correct interpretation is that instead
  // PNG format is used and ICO size can be variant.
  if (entry->width == 0 && entry->height == 0)
  {
    // Here I used createDecoderByExtension(), becuase I don't care which decoder
    // will be used for this (for example it's possible to use GDI+ under Windows).
    err = ImageCodecProvider::createDecoderByExtension(fog_strings->getString(STR_G2D_EXTENSION_png), &decoder);
    if (FOG_IS_ERROR(err)) goto _End;

    decoder->attachStream(_stream);
    err = decoder->readImage(image);
  }
  else
  {
    // Here I used createDecoderByName(), because I want exactly BMP Decoder
    // from Fog. Note the property skipFileHeader that configures this decoder
    // to skip reading the bmp file header.
    err = ImageCodecProvider::createDecoderByName(fog_strings->getString(STR_G2D_STREAM_BMP), &decoder);
    if (FOG_IS_ERROR(err)) goto _End;

    decoder->attachStream(_stream);
    decoder->setProperty(fog_strings->getString(STR_G2D_CODEC_skipFileHeader), Value::fromInt32(1));

    if ((err = decoder->readHeader()) == ERR_OK)
    {
      // There are also AND+XOR masks, we adjust image height to be half.
      decoder->_size.h /= 2;
      err = decoder->readImage(image);
    }

    // 32-bit icons contain alpha-channel.
    if (image.getFormat() == IMAGE_FORMAT_XRGB32) image.forceFormat(IMAGE_FORMAT_PRGB32);
  }

  fog_delete(decoder);

_End:
  // We always increment actual frame, because if there is no PNG loader we can
  // just skip PNG frames.
  ++_actualFrame;
  image._modified();

  return err;
}

// ============================================================================
// [Fog::G2d - Library Initializers]
// ============================================================================

FOG_NO_EXPORT void _g2d_imagecodecprovider_init_ico(void)
{
  ImageCodecProvider::addProvider(IMAGE_CODEC_DECODER, fog_new IcoCodecProvider());
}

} // Fog namespace
