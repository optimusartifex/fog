// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Constants.h>
#include <Fog/Core/ManagedString.h>
#include <Fog/Core/Math.h>
#include <Fog/Core/Memory.h>
#include <Fog/Core/Misc.h>
#include <Fog/Core/Stream.h>
#include <Fog/Core/String.h>
#include <Fog/Core/Strings.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Image.h>
#include <Fog/Graphics/ImageIO.h>
#include <Fog/Graphics/ImageIO/ImageIO_Bmp_p.h>
#include <Fog/Graphics/RasterEngine_p.h>

namespace Fog { 
namespace ImageIO {

// ============================================================================
// [Fog::ImageIO::BmpProvider]
// ============================================================================

BmpProvider::BmpProvider()
{
  // Name of ImageIO Provider.
  _name = fog_strings->getString(STR_GRAPHICS_BMP);

  // File type.
  _fileType = IMAGE_IO_FILE_BMP;

  // Supported devices.
  _deviceType = IMAGE_IO_DEVICE_BOTH;

  // Supported extensions.
  _imageExtensions.reserve(2);
  _imageExtensions.append(fog_strings->getString(STR_GRAPHICS_bmp));
  _imageExtensions.append(fog_strings->getString(STR_GRAPHICS_ras));
}

BmpProvider::~BmpProvider()
{
}

uint32_t BmpProvider::checkSignature(const void* mem, sysuint_t length) const
{
  if (!mem || length == 0) return 0;

  const uint8_t* m = (const uint8_t*)mem;

  // Check for 'BM' mime.
  if (length >= 1 && m[0] != (uint8_t)'B') return 0;
  if (length >= 2 && m[1] != (uint8_t)'M') return 0;

  // Check for correct header size.
  if (length >= 18)
  {
    uint32_t headerSize = Memory::bswap32le( *(const uint32_t *)(m + 14) );
    if (headerSize != 12 || headerSize != 40) return 0;

    return 95;
  }
  else
    return 75;
}

err_t BmpProvider::createDevice(uint32_t deviceType, BaseDevice** device) const
{
  BaseDevice* d = NULL;

  switch (deviceType)
  {
    case IMAGE_IO_DEVICE_DECODER:
      d = new(std::nothrow) BmpDecoderDevice(const_cast<BmpProvider*>(this));
      break;
    case IMAGE_IO_DEVICE_ENCODER:
      d = new(std::nothrow) BmpEncoderDevice(const_cast<BmpProvider*>(this));
      break;
    default:
      return ERR_RT_INVALID_ARGUMENT;
  }

  if (!d) return ERR_RT_OUT_OF_MEMORY;

  *device = d;
  return ERR_OK;
}

// ============================================================================
// [Fog::ImageIO::BmpDecoderDevice]
// ============================================================================

BmpDecoderDevice::BmpDecoderDevice(Provider* provider) :
  DecoderDevice(provider)
{
  zeroall();
}

BmpDecoderDevice::~BmpDecoderDevice()
{
}

// ============================================================================
// [Fog::ImageIO::BmpDecoderDevice::reset]
// ============================================================================

void BmpDecoderDevice::reset()
{
  DecoderDevice::reset();
  zeroall();
}

// ============================================================================
// [Fog::ImageIO::BmpDecoderDevice::readHeader]
// ============================================================================

err_t BmpDecoderDevice::readHeader()
{
  // Don't read header more than once.
  if (isHeaderDone()) return _headerResult;

  // Mark header as done.
  _headerDone = true;

  // Clear headers. This is not important step, but safe.
  Memory::zero(&bmpFileHeader, sizeof(BmpFileHeader));
  Memory::zero(&bmpDataHeader, sizeof(bmpDataHeader));

  // Read bmp header or skip it if configured for this.
  if (!_skipFileHeader)
  {
    if (_stream.read(&bmpFileHeader, sizeof(BmpFileHeader)) != sizeof(BmpFileHeader))
    {
      return (_headerResult = ERR_IMAGEIO_TRUNCATED);
    }

    // Match bmp mime type "BM".
    if (bmpFileHeader.magic_B != 'B' || bmpFileHeader.magic_M != 'M')
    {
      return (_headerResult = ERR_IMAGEIO_MIME_NOT_MATCH);
    }
  }

#if FOG_BYTE_ORDER == FOG_BIG_ENDIAN
  bmpFileHeader.fileSize = Memory::bswap32le(bmpFileHeader.fileSize);
  bmpFileHeader.imageOffset = Memory::bswap32le(bmpFileHeader.imageOffset);
#endif

  // Read DWORD, header size in bytes, this is important for determining header type.
  if (_stream.read(&bmpDataHeader.headerSize, 4) != 4)
  {
    return (_headerResult = ERR_IMAGEIO_TRUNCATED);
  }

#if FOG_BYTE_ORDER == FOG_BIG_ENDIAN
  bmpDataHeader.os2v1.fileSize = Memory::bswap32le(bmpDataHeader.os2v1.fileSize);
#endif

  // First check if header is supported by our BmpDecoderDevice.
  switch (bmpDataHeader.headerSize)
  {
    case BMP_HEADER_OS2_V1:
    case BMP_HEADER_WIN_V3:
    case BMP_HEADER_WIN_V4:
    case BMP_HEADER_WIN_V5:
      break;
    default:
      return (_headerResult = ERR_IMAGEIO_UNSUPPORTED_FORMAT);
  }

  // File header and bmp header.
  uint32_t headerTotalBytes = 14 + bmpDataHeader.headerSize;

  // Now load the rest of header
  uint32_t rest = bmpDataHeader.headerSize - 4;

  if (_stream.read(reinterpret_cast<char*>(&bmpDataHeader) + 4, rest) != rest)
  {
    return (_headerResult = ERR_IMAGEIO_TRUNCATED);
  }

#if FOG_BYTE_ORDER == FOG_BIG_ENDIAN
  switch (bmpDataHeader.headerSize)
  {
    case BMP_HEADER_OS2_V1:
      bmpDataHeader.os2v1.width           = Memory::bswap16le(bmpDataHeader.os2v1.width);
      bmpDataHeader.os2v1.height          = Memory::bswap16le(bmpDataHeader.os2v1.height);
      bmpDataHeader.os2v1.planes          = Memory::bswap16le(bmpDataHeader.os2v1.planes);
      bmpDataHeader.os2v1.bitsPerPixel    = Memory::bswap16le(bmpDataHeader.os2v1.bitsPerPixel);
      break;

    case BMP_HEADER_WIN_V5:
      bmpDataHeader.winv5.intent          = Memory::bswap32le(bmpDataHeader.winv5.intent);
      bmpDataHeader.winv5.profileData     = Memory::bswap32le(bmpDataHeader.winv5.profileData);
      bmpDataHeader.winv5.profileSize     = Memory::bswap32le(bmpDataHeader.winv5.profileSize);
      // Fall through...

    case BMP_HEADER_WIN_V4:
      bmpDataHeader.winv4.rMask           = Memory::bswap32le(bmpDataHeader.winv4.rMask);
      bmpDataHeader.winv4.gMask           = Memory::bswap32le(bmpDataHeader.winv4.gMask);
      bmpDataHeader.winv4.bMask           = Memory::bswap32le(bmpDataHeader.winv4.bMask);
      bmpDataHeader.winv4.aMask           = Memory::bswap32le(bmpDataHeader.winv4.aMask);
      bmpDataHeader.winv4.colorSpace      = Memory::bswap32le(bmpDataHeader.winv4.colorSpace);
      bmpDataHeader.winv4.rX              = Memory::bswap32le(bmpDataHeader.winv4.rX);
      bmpDataHeader.winv4.rY              = Memory::bswap32le(bmpDataHeader.winv4.rY);
      bmpDataHeader.winv4.rZ              = Memory::bswap32le(bmpDataHeader.winv4.rZ);
      bmpDataHeader.winv4.gX              = Memory::bswap32le(bmpDataHeader.winv4.gX);
      bmpDataHeader.winv4.gY              = Memory::bswap32le(bmpDataHeader.winv4.gY);
      bmpDataHeader.winv4.gZ              = Memory::bswap32le(bmpDataHeader.winv4.gZ);
      bmpDataHeader.winv4.bX              = Memory::bswap32le(bmpDataHeader.winv4.bX);
      bmpDataHeader.winv4.bY              = Memory::bswap32le(bmpDataHeader.winv4.bY);
      bmpDataHeader.winv4.bZ              = Memory::bswap32le(bmpDataHeader.winv4.bZ);
      bmpDataHeader.winv4.rGamma          = Memory::bswap32le(bmpDataHeader.winv4.rGamma);
      bmpDataHeader.winv4.gGamma          = Memory::bswap32le(bmpDataHeader.winv4.gGamma);
      bmpDataHeader.winv4.bGamma          = Memory::bswap32le(bmpDataHeader.winv4.bGamma);
      // Fall through...

    case BMP_HEADER_WIN_V3:
      bmpDataHeader.winv3.width           = Memory::bswap32le(bmpDataHeader.winv3.width);
      bmpDataHeader.winv3.height          = Memory::bswap32le(bmpDataHeader.winv3.height);
      bmpDataHeader.winv3.planes          = Memory::bswap16le(bmpDataHeader.winv3.planes);
      bmpDataHeader.winv3.bitsPerPixel    = Memory::bswap16le(bmpDataHeader.winv3.bitsPerPixel);
      bmpDataHeader.winv3.compression     = Memory::bswap32le(bmpDataHeader.winv3.compression);
      bmpDataHeader.winv3.imageSize       = Memory::bswap32le(bmpDataHeader.winv3.imageSize);
      bmpDataHeader.winv3.horzResolution  = Memory::bswap32le(bmpDataHeader.winv3.horzResolution);
      bmpDataHeader.winv3.vertResolution  = Memory::bswap32le(bmpDataHeader.winv3.vertResolution);
      bmpDataHeader.winv3.colorsUsed      = Memory::bswap32le(bmpDataHeader.winv3.colorsUsed);
      bmpDataHeader.winv3.colorsImportant = Memory::bswap32le(bmpDataHeader.winv3.colorsImportant);
      break;
  }
#endif

  switch (bmpDataHeader.headerSize)
  {
    case BMP_HEADER_OS2_V1:
      _width  = bmpDataHeader.os2v1.width;
      _height = bmpDataHeader.os2v1.height;
      _depth  = bmpDataHeader.os2v1.bitsPerPixel;
      _planes = bmpDataHeader.os2v1.planes;

      bmpCompression = BMP_BI_RGB;
      bmpSkipBytes = 0;
      break;

    case BMP_HEADER_WIN_V5:
    case BMP_HEADER_WIN_V4:
    case BMP_HEADER_WIN_V3:
      _width  = bmpDataHeader.winv3.width;
      _height = bmpDataHeader.winv3.height;
      _depth  = bmpDataHeader.winv3.bitsPerPixel;
      _planes = bmpDataHeader.winv3.planes;

      bmpCompression = bmpDataHeader.winv3.compression;
      bmpImageSize   = bmpDataHeader.winv3.imageSize;

      bmpFileHeader.imageOffset = Math::max(bmpFileHeader.imageOffset, headerTotalBytes);
      bmpSkipBytes = bmpFileHeader.imageOffset - headerTotalBytes;
      break;
  }

  // Check for correct depth.
  switch(_depth)
  {
    case 1:
    case 4:
    case 8:
    case 16:
    case 24:
    case 32:
      break;
    default:
      return (_headerResult = ERR_IMAGEIO_UNSUPPORTED_FORMAT);
  }

  // Check whether the image size is valid.
  if (!checkImageSize())
  {
    return (_headerResult = ERR_IMAGE_INVALID_SIZE);
  }

  // Bmp contains only one image.
  _actualFrame = 0;
  _framesCount = 1;

  bmpStride = (((_width * _depth + 7) >> 3) + 3) & ~3;

  // OS2 header didn't define image size.
  if (bmpImageSize == 0) bmpImageSize = bmpStride * _height;

  // Setup defaults.
  rMask = 0x00000000; rShift = 0; rLoss = 0;
  gMask = 0x00000000; gShift = 0; gLoss = 0;
  bMask = 0x00000000; bShift = 0; bLoss = 0;
  aMask = 0x00000000; aShift = 0; aLoss = 0;

  switch (_depth)
  {
    // Setup palette for indexed images.
    case 1:
    case 4:
    case 8:
    {
      uint32_t* pdestCur = reinterpret_cast<uint32_t*>(_palette.getMData());
      sysuint_t i;
      sysuint_t nColors = (bmpFileHeader.imageOffset - headerTotalBytes);
      uint32_t palSize;

      if (bmpDataHeader.headerSize == BMP_HEADER_OS2_V1)
      {
        uint8_t psrc24[768];
        uint8_t* psrc24Cur = psrc24;

        nColors = Math::min<sysuint_t>(nColors / 3, 256);
        palSize = (uint32_t)nColors * 3;

        if (_stream.read(psrc24, palSize) != palSize)
        {
          return (_headerResult = ERR_IMAGEIO_TRUNCATED);
        }

        for (i = nColors; i; i--, pdestCur++, psrc24Cur += 3)
        {
          *pdestCur = Argb(psrc24Cur[2], psrc24Cur[1], psrc24Cur[0], 0xFF);
        }
      }
      else
      {
        nColors = Math::min<sysuint_t>(nColors / 4, 256);
        palSize = (uint32_t)nColors * 4;

        if (_stream.read(pdestCur, palSize) != palSize)
        {
          return (_headerResult = ERR_IMAGEIO_TRUNCATED);
        }

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
        for (i = 0; i != nColors; i++, pdestCur++) *pdestCur |= 0xFF000000;
#else
        for (i = 0; i != nColors; i++, pdestCur++) *pdestCur = Memory::bswap32(*pdestCur) | 0xFF000000;
#endif
      }

      if (bmpSkipBytes < palSize)
        bmpSkipBytes = 0;
      else
        bmpSkipBytes -= palSize;

      _format = IMAGE_FORMAT_I8;
      _palette.update();
      break;
    }

    // Setup 16-bit RGB.
    case 16:
      rMask = 0x7C00; rShift = 10; rLoss = 3;
      gMask = 0x03E0; gShift =  5; gLoss = 3;
      bMask = 0x001F; bShift =  0; bLoss = 3;

      _format = IMAGE_FORMAT_XRGB32;
      break;

    // Setup 24-bit RGB.
    case 24:
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
      rMask = 0x00FF0000; rShift = 16;
      gMask = 0x0000FF00; gShift =  8;
      bMask = 0x000000FF; bShift =  0;
#else
      rMask = 0x000000FF; rShift =  0;
      gMask = 0x0000FF00; gShift =  8;
      bMask = 0x00FF0000; bShift = 16;
#endif // FOG_BYTE_ORDER
      _format = IMAGE_FORMAT_XRGB32;
      break;

    // Setup 32-bit RGB (It's possible that alpha-channel is active).
    case 32:
      if (bmpDataHeader.headerSize > BMP_HEADER_WIN_V3)
      {
        rMask = bmpDataHeader.winv4.rMask;
        gMask = bmpDataHeader.winv4.gMask;
        bMask = bmpDataHeader.winv4.bMask;

        if (bmpCompression != BMP_BI_BITFIELDS)
          aMask = bmpDataHeader.winv4.aMask;
      }
      else
      {
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
        rMask = 0x00FF0000; rShift = 16;
        gMask = 0x0000FF00; gShift =  8;
        bMask = 0x000000FF; bShift =  0;
#else
        rMask = 0x0000FF00; rShift =  8;
        gMask = 0x00FF0000; gShift = 16;
        bMask = 0xFF000000; bShift = 24;
#endif // FOG_BYTE_ORDER
      }

      _format = (aMask == 0x00000000) ? IMAGE_FORMAT_XRGB32 : IMAGE_FORMAT_ARGB32;
      break;
  }

  // Bitfields.
  if (bmpCompression == BMP_BI_BITFIELDS &&
      bmpDataHeader.headerSize == BMP_HEADER_WIN_V3 &&
      (_depth == 16 || _depth == 32))
  {
    uint32_t masks[3];

    if (_stream.read(masks, 12) != 12) 
    {
      return (_headerResult = ERR_IMAGEIO_TRUNCATED);
    }

    if (bmpSkipBytes < 12)
      bmpSkipBytes = 0;
    else
      bmpSkipBytes -= 12;

    rMask = Memory::bswap32le(masks[0]); rShift = 0;
    gMask = Memory::bswap32le(masks[1]); gShift = 0;
    bMask = Memory::bswap32le(masks[2]); bShift = 0;
  }

  // Calculate correct shift and loss values for any depth larger than 8 in case
  // that it's not already initialized.
  if (_depth > 8 && rShift == 0 && gShift == 0 && bShift == 0 && aShift == 0)
  {
    uint32_t rSize = 0;
    uint32_t gSize = 0;
    uint32_t bSize = 0;
    uint32_t aSize = 0;

    for (int bit = (int)(_depth) - 1; bit >= 0; bit--)
    {
      if (bMask & (1 << bit)) { bShift = bit; bSize++; }
      if (gMask & (1 << bit)) { gShift = bit; gSize++; }
      if (rMask & (1 << bit)) { rShift = bit; rSize++; }
      if (aMask & (1 << bit)) { aShift = bit; aSize++; }
    }

    // If bit size for component is larger than 8 we just degrade precision.
    if (rSize > 8) { rShift += (rSize - 8); rSize = 8; }
    if (gSize > 8) { gShift += (gSize - 8); gSize = 8; }
    if (bSize > 8) { bShift += (bSize - 8); bSize = 8; }
    if (aSize > 8) { aShift += (aSize - 8); aSize = 8; }

    rLoss = 8 - rSize;
    gLoss = 8 - gSize;
    bLoss = 8 - bSize;
    aLoss = 8 - aSize;
  }

  // Success.
  return (_headerResult = ERR_OK);
}

// ============================================================================
// [BmpDecoderDevice::readImage]
// ============================================================================

err_t BmpDecoderDevice::readImage(Image& image)
{
  // Read bmp header.
  if (readHeader() != ERR_OK) return _headerResult;

  // Don't read image more than once.
  if (isReaderDone()) return (_readerResult = ERR_IMAGEIO_NO_MORE_FRAMES);

  // Error code (default is success).
  uint32_t err = ERR_OK;

  // Buffer pointers.
  uint8_t* pixelsBegin;
  uint8_t* pixelsCur;

  // Reader variables.
  uint32_t x = 0;
  uint32_t y = 0;
  uint32_t i;
  sysint_t stride;

  LocalBuffer<512> rawBufferStorage;
  LocalBuffer<512> rleBufferStorage;
  uint8_t* buffer = (uint8_t *)rawBufferStorage.alloc(bmpStride);
  uint8_t* rleBuffer = NULL;

  // First skip bytes if needed.
  if (bmpSkipBytes > 0 && _stream.seek(bmpSkipBytes, STREAM_SEEK_CUR) == -1)
  {
    err = ERR_IO_CANT_SEEK;
    goto end;
  }

  // Create image.
  if ((err = image.create(_width, _height, _format))) goto end;

  stride = image.getStride();
  pixelsBegin = image._d->first;
  pixelsCur = pixelsBegin + stride * (_height - 1);

  // -------------------- Conversion from 1 bit depth --------------------

  if (_depth == 1)
  {
    if (bmpCompression == BMP_BI_RGB)
    {
      uint8_t* bufferCur;
      uint32_t b;

      for (y = 0; y != _height; y++)
      {
        if (_stream.read(buffer, bmpStride) != bmpStride) goto truncated;
        bufferCur = buffer;
        pixelsCur = pixelsBegin + (_height - y - 1) * stride;

        for (i = _width; i >= 8; i -= 8, pixelsCur += 8, bufferCur++)
        {
          b = (uint32_t)(*bufferCur);
          ((uint8_t *)pixelsCur)[0] = (uint8_t)((b >> 7) & 1);
          ((uint8_t *)pixelsCur)[1] = (uint8_t)((b >> 6) & 1);
          ((uint8_t *)pixelsCur)[2] = (uint8_t)((b >> 5) & 1);
          ((uint8_t *)pixelsCur)[3] = (uint8_t)((b >> 4) & 1);
          ((uint8_t *)pixelsCur)[4] = (uint8_t)((b >> 3) & 1);
          ((uint8_t *)pixelsCur)[5] = (uint8_t)((b >> 2) & 1);
          ((uint8_t *)pixelsCur)[6] = (uint8_t)((b >> 1) & 1);
          ((uint8_t *)pixelsCur)[7] = (uint8_t)((b     ) & 1);
        }
        if (i)
        {
          b = (uint32_t)(*bufferCur);
          for (; i; i--, pixelsCur += 1, b <<= 1)
          {
            ((uint8_t *)pixelsCur)[0] = (uint8_t)((b >> 7) & 1);
          }
        }
        if ((y & 15) == 0) { updateProgress(y, _height); }
      }
    }
  }

  // -------------------- Conversion from 4 bit depth --------------------

  else if (_depth == 4)
  {
    // ==== 4 BIT RLE DATA ====
    if (bmpCompression == BMP_BI_RLE4)
    {
      uint8_t* rleCur;
      uint8_t* rleEnd;
      uint8_t b0;
      uint8_t b1;

      if ((rleBuffer = (uint8_t *)rleBufferStorage.alloc(bmpImageSize)) == NULL) goto outOfMemory;
      if (_stream.read(rleBuffer, bmpImageSize) != bmpImageSize) goto truncated;

      rleCur = rleBuffer;
      rleEnd = rleBuffer + bmpImageSize;

BI_RLE_4_BEGIN:
      if (x >= _width || y >= _height) goto rleError;

      pixelsCur = pixelsBegin + (_height - y - 1) * stride;
      updateProgress(y, _height);

      for (;;)
      {
        if (rleCur + 2 > rleEnd) goto truncated;
        b0 = *rleCur++;
        b1 = *rleCur++;

        if (b0)
        {
          // b0 = Length.
          // b1 = 2 Colors.
          uint8_t c0 = b1 >> 4;
          uint8_t c1 = b1 & 0xF;

          i = Math::min<uint32_t>(b0, _width - x);
          x += i;

          while (i >= 2)
          {
            *pixelsCur++ = c0;
            *pixelsCur++ = c1;
            i -= 2;
          }
          if (i) *pixelsCur++ = c0;
        }
        else
        {
          // b1 = Chunk type.
          switch (b1)
          {
            case BMP_RLE_NEXT_LINE: x = 0; y++; goto BI_RLE_4_BEGIN;
            case BMP_RLE_END: goto end;
            case BMP_RLE_MOVE:
              if (rleCur + 2 > rleEnd) goto truncated;
              x += *rleCur++;
              y += *rleCur++;
              goto BI_RLE_4_BEGIN;
            // FILL BITS (b1 == length).
            default:
            {
              uint8_t* backup = rleCur;
              i = Math::min<uint32_t>(b1, _width - x);

              if (rleCur + ((b1 + 1) >> 1) > rleEnd) goto truncated;
              x += i;

              while (i >= 2)
              {
                b0 = *rleCur++;
                *pixelsCur++ = b0 >> 4;
                *pixelsCur++ = b0 & 0xF;
                i -= 2;
              }
              if (i)
              {
                *pixelsCur++ = *rleCur++ >> 4;
              }

              // Skip padding pixels.
              rleCur = backup + ((b1 + 1) >> 1);

              // Skip RLE padding.
              if ((b1 & 3) == 1) rleCur += 2;
              else if ((b1 & 3) == 2) rleCur++;

              break;
            }
          }
        }
      }
    }

    // ==== 4 BIT RAW DATA ====
    else if (bmpCompression == BMP_BI_RGB)
    {
      uint8_t* bufferCur;
      uint8_t b;

      for (y = 0; y != _height; y++)
      {
        if (_stream.read(buffer, bmpStride) != bmpStride) goto truncated;
        bufferCur = buffer;
        pixelsCur = pixelsBegin + (_height - y - 1) * stride;

        for (x = 0; x + 2 <= _width; x += 2)
        {
          b = *bufferCur++;
          *pixelsCur++ = b >> 4;
          *pixelsCur++ = b & 0xF;
        }
        if (x < _width)
        {
          *pixelsCur = *bufferCur >> 4;
        }
        if ((y & 15) == 0) updateProgress(y, _height);
      }
    }
  }

  // -------------------- Conversion from 8 bit depth --------------------

  else if (_depth == 8)
  {
    // ==== 8 BIT RLE DATA ====
    if (bmpCompression == BMP_BI_RLE8)
    {
      uint8_t* rleCur;
      uint8_t* rleEnd;
      uint8_t b0;
      uint8_t b1;

      if ((rleBuffer = (uint8_t *)rleBufferStorage.alloc(bmpImageSize)) == NULL) goto outOfMemory;
      if (_stream.read(rleBuffer, bmpImageSize) != bmpImageSize) goto truncated;

      rleCur = rleBuffer;
      rleEnd = rleBuffer + bmpImageSize;

BI_RLE_8_BEGIN:
      if (x >= _width || y >= _height) goto rleError;

      pixelsCur = pixelsBegin + (_height - y - 1) * stride;
      updateProgress(y, _height);

      for (;;)
      {
        if (rleCur + 2 > rleEnd) goto truncated;
        b0 = *rleCur++;
        b1 = *rleCur++;

        if (b0)
        {
          // b0 = Length.
          // b1 = Color.
          i = Math::min<uint32_t>(b0, _width - x);
          while (i--) *pixelsCur++ = b1;
        }
        else 
        {
          // b1 = Chunk type.
          switch (b1)
          {
            case BMP_RLE_NEXT_LINE: x = 0; y++; goto BI_RLE_8_BEGIN;
            case BMP_RLE_END: goto end;
            case BMP_RLE_MOVE:
              if (rleCur + 2 > rleEnd) goto truncated;
              x += *rleCur++;
              y += *rleCur++; 
              goto BI_RLE_8_BEGIN;
            // FILL BITS (b1 == length).
            default:
            {
              uint8_t* backup = rleCur;

              i = Math::min<uint32_t>(b1, _width - x);
              if (rleCur + b1 > rleEnd) goto truncated;
              x += i;

              while (i--) *pixelsCur++ = *rleCur++;

              rleCur = backup + b1;
              if (b1 & 1) rleCur++;
              break;
            }
          }
        }
      }
      updateProgress(y, _height);
    }

    // ==== 8 BIT RAW DATA ====
    else if (bmpCompression == BMP_BI_RGB)
    {
      for (y = 0; y < _height; y++)
      {
        pixelsCur = pixelsBegin + (_height - y - 1) * stride;
        if (_stream.read(pixelsCur, bmpStride) != bmpStride) goto truncated;
        if ((y & 15) == 0) updateProgress(y, _height);
      }
    }
  }

  // --------------- Conversion from 16, 24 and 32 bit depth --------------

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  // Direct loading.
  else if (
    ((_depth == 32 && _format == IMAGE_FORMAT_ARGB32) ||
     (_depth == 32 && _format == IMAGE_FORMAT_XRGB32)) &&
     (rMask == ARGB32_RMASK && gMask == ARGB32_GMASK && bMask == ARGB32_BMASK))
  {
    sysuint_t readBytes = _width * (_depth >> 3);
    sysuint_t tailBytes = bmpStride - readBytes;

    for (y = 0; y < _height; y++)
    {
      pixelsCur = pixelsBegin + (_height - y - 1) * stride;
      if (_stream.read(pixelsCur, readBytes) != readBytes ||
          _stream.read(buffer, tailBytes) != tailBytes)
      {
        goto truncated;
      }
      if ((y & 15) == 0) updateProgress(y, _height);
    }
  }
#endif // FOG_LITTLE_ENDIAN
  // Buffered loading.
  else
  {
    for (y = 0; y < _height; y++, pixelsCur += stride)
    {
      uint8_t* bufferCur = buffer;
      pixelsCur = pixelsBegin + (_height - y - 1) * stride;
      if (_stream.read(bufferCur, bmpStride) != bmpStride) goto truncated;

      switch (_depth)
      {
        case 16:
        {
          uint32_t rAdjust = 8 - rLoss;
          uint32_t gAdjust = 8 - gLoss;
          uint32_t bAdjust = 8 - bLoss;

          for (x = 0; x < _width; x++, pixelsCur += 3, bufferCur += 2)
          {
            uint32_t pix = Memory::bswap16le(((const uint16_t*)bufferCur)[0]);
            uint32_t pixr = ((pix & rMask) >> rShift) << rLoss;
            uint32_t pixg = ((pix & gMask) >> gShift) << gLoss;
            uint32_t pixb = ((pix & bMask) >> bShift) << bLoss;

            pixelsCur[ARGB32_RBYTE] = pixr |= (pixr >> rAdjust);
            pixelsCur[ARGB32_GBYTE] = pixg |= (pixg >> gAdjust);
            pixelsCur[ARGB32_BBYTE] = pixb |= (pixb >> bAdjust);
            pixelsCur[ARGB32_ABYTE] = 0xFF;
          }
          break;
        }

        case 24:
        {
          // 24 bit fields are not supported, standard format is BGR.
          for (x = 0; x < _width; x++, pixelsCur += 3, bufferCur += 3)
          {
            pixelsCur[ARGB32_BBYTE] = bufferCur[0];
            pixelsCur[ARGB32_GBYTE] = bufferCur[1];
            pixelsCur[ARGB32_RBYTE] = bufferCur[2];
            pixelsCur[ARGB32_ABYTE] = 0xFF;
          }
          break;
        }

        case 32:
        {
          for (x = 0; x < _width; x++, pixelsCur += 4, bufferCur += 4)
          {
            uint32_t pix = Memory::bswap32le(((const uint32_t*)bufferCur)[0]);
            uint32_t pixr = ((pix & rMask) >> rShift);
            uint32_t pixg = ((pix & gMask) >> gShift);
            uint32_t pixb = ((pix & bMask) >> bShift);
            uint32_t pixa = ((pix & aMask) >> aShift);

            ((uint32_t*)pixelsCur)[0] =
              (pixr << ARGB32_RSHIFT) | (pixg << ARGB32_GSHIFT) |
              (pixb << ARGB32_BSHIFT) | (pixa << ARGB32_ASHIFT) ;
          }
          break;
        }

        default:
          FOG_ASSERT_NOT_REACHED();
          break;
      }

      if ((y & 15) == 0) updateProgress(y, _height);
    }
  }

  goto end;

truncated:
  err = ERR_IMAGEIO_TRUNCATED;
  goto end;
rleError:
  err = ERR_IMAGEIO_MALFORMED_RLE;
  goto end;
outOfMemory:
  err = ERR_RT_OUT_OF_MEMORY;
  goto end;

end:
  // Apply palette if needed.
  if (_depth <= 8 && !image.isEmpty()) image.setPalette(_palette);

  if (err == ERR_OK) updateProgress(1.0);
  return (_readerResult = err);
}

err_t BmpDecoderDevice::getProperty(const ManagedString& name, Value& value) const
{
  if (name == fog_strings->getString(STR_GRAPHICS_skipFileHeader))
  {
    return value.setInt32(_skipFileHeader);
  }

  return base::getProperty(name, value);
}

err_t BmpDecoderDevice::setProperty(const ManagedString& name, const Value& value)
{
  err_t err;
  int i;

  if (name == fog_strings->getString(STR_GRAPHICS_skipFileHeader))
  {
    if ((err = value.getInt32(&i))) return err;
    _skipFileHeader = i ? 1 : 0;
    return ERR_OK;
  }

  return base::setProperty(name, value);
}

// ============================================================================
// [Fog::ImageIO::BmpEncoder]
// ============================================================================

BmpEncoderDevice::BmpEncoderDevice(Provider* provider) :
  EncoderDevice(provider)
{
}

BmpEncoderDevice::~BmpEncoderDevice()
{
}

err_t BmpEncoderDevice::writeImage(const Image& image)
{
  static const uint8_t tailBytes[4] = { 0, 0, 0, 0 };
  err_t err = ERR_OK;

  ImageData* d = image._d;
  int width = d->width;
  int height = d->height;
  uint32_t format = d->format;
  uint32_t bpl;
  uint32_t skip;

  BmpFileHeader bmpFileHeader;
  BmpDataHeader bmpDataHeader;
  uint32_t headerSize = BMP_HEADER_WIN_V3;

  LocalBuffer<1024> bufferLocal;

  if (!width || !height)
  {
    err = ERR_IMAGE_INVALID_SIZE;
    goto end;
  }

  // BMP not supports alpha channel in palette so we must use ARGB32 format if
  // palette alpha channel is used.
  if (format == IMAGE_FORMAT_I8 && image.getPalette().isAlphaUsed())
  {
    format = IMAGE_FORMAT_ARGB32;
  }

  if (format == IMAGE_FORMAT_ARGB32 || format == IMAGE_FORMAT_PRGB32)
  {
    headerSize = BMP_HEADER_WIN_V4;
  }

  Memory::zero(&bmpDataHeader, sizeof(BmpDataHeader));

  // Bitmap file header.
  bmpFileHeader.magic_B                = 'B';
  bmpFileHeader.magic_M                = 'M';
  bmpFileHeader.reserved1              = 0x0000;
  bmpFileHeader.reserved2              = 0x0000;
  bmpFileHeader.imageOffset            = Memory::bswap32le(14 + headerSize);

  // Bitmap data header.
  bmpDataHeader.winv3.headerSize       = Memory::bswap32le(headerSize);
  bmpDataHeader.winv3.width            = Memory::bswap32le(width);
  bmpDataHeader.winv3.height           = Memory::bswap32le(height);
  bmpDataHeader.winv3.planes           = Memory::bswap16le(1);
  bmpDataHeader.winv3.compression      = Memory::bswap32le(BMP_BI_RGB);
  bmpDataHeader.winv4.colorspace       = Memory::bswap32le(BMP_COLORSPACE_DD_RGB);

  switch (format)
  {
    case IMAGE_FORMAT_ARGB32:
    case IMAGE_FORMAT_PRGB32:
    {
      uint imageSize;

      bpl = width * 4;
      skip = 0;
      imageSize = bpl * height;

      bmpFileHeader.fileSize           = Memory::bswap32le(14 + headerSize + imageSize);
      bmpDataHeader.winv3.bitsPerPixel = Memory::bswap16le(32);
      bmpDataHeader.winv3.imageSize    = Memory::bswap32le(imageSize);

      bmpDataHeader.winv4.rMask        = Memory::bswap32le(0x00FF0000);
      bmpDataHeader.winv4.gMask        = Memory::bswap32le(0x0000FF00);
      bmpDataHeader.winv4.bMask        = Memory::bswap32le(0x000000FF);
      bmpDataHeader.winv4.aMask        = Memory::bswap32le(0xFF000000);

      break;
    }

    case IMAGE_FORMAT_XRGB32:
    {
      uint imageSize;

      bpl = width * 3;
      skip = (4 - (bpl & 3)) & 3;
      imageSize = (bpl + skip) * height;

      bmpFileHeader.fileSize           = Memory::bswap32le(14 + headerSize + imageSize);
      bmpDataHeader.winv3.bitsPerPixel = Memory::bswap16le(24);
      bmpDataHeader.winv3.imageSize    = Memory::bswap32le(imageSize);

      break;
    }

    case IMAGE_FORMAT_A8:
    case IMAGE_FORMAT_I8:
    {
      uint imageSize;

      bpl = width;
      skip = (4 - (bpl & 3)) & 3;
      imageSize = (uint)(bpl + skip) * (uint)height;

      // 1024 == palette size (4 * 256).
      bmpFileHeader.fileSize           = Memory::bswap32le(14 + headerSize + 1024 + imageSize);
      bmpFileHeader.imageOffset        = Memory::bswap32le(14 + headerSize + 1024);
      bmpDataHeader.winv3.bitsPerPixel = Memory::bswap16le(8);
      bmpDataHeader.winv3.imageSize    = Memory::bswap32le(imageSize);
      break;
    }

    default:
    {
      err = ERR_IMAGE_UNSUPPORTED_FORMAT;
      goto fail;
    }
  }

  // Write file and bmp header.
  if (_stream.write(&bmpFileHeader, 14) != 14) goto fail;
  if (_stream.write(&bmpDataHeader, headerSize) != headerSize) goto fail;

  switch (format)
  {
    case IMAGE_FORMAT_ARGB32:
    case IMAGE_FORMAT_PRGB32:
    {
      uint8_t *buffer = (uint8_t *)bufferLocal.alloc(bpl);
      if (!buffer) { err = ERR_RT_OUT_OF_MEMORY; goto end; }

      // Write 32 bit BMP data (ARGB32 non-premultiplied).
      for (uint y = 0; y != height; y++)
      {
        image.getDib(0, (height - 1 - y), width, DIB_FORMAT_ARGB32_LE, buffer);
        if (_stream.write((const void *)buffer, bpl) != bpl) goto fail;
        if ((y & 15) == 0) updateProgress(y, height);
      }
      break;
    }

    case IMAGE_FORMAT_XRGB32:
    {
      uint8_t* buffer = (uint8_t *)bufferLocal.alloc(bpl + skip);
      if (!buffer) { err = ERR_RT_OUT_OF_MEMORY; goto end; }

      // Cleanup tail.
      memset(buffer + bpl, 0, skip);
      bpl += skip;

      // Write 24-bit BMP data (BGR format).
      for (int y = 0; y != height; y++)
      {
        image.getDib(0, (height - 1 - y), width, DIB_FORMAT_RGB24_LE, buffer);
        if (_stream.write(buffer, bpl) != (sysuint_t)bpl) goto fail;
        if ((y & 15) == 0) updateProgress(y, height);
      }
      break;
    }

    case IMAGE_FORMAT_A8:
    case IMAGE_FORMAT_I8:
    {
      // Write palette.
      if (format == IMAGE_FORMAT_A8)
      {
        // Write greyscale palette for A8 formats.
        uint32_t pl[256];
        for (uint i = 0; i != 256; i += 4)
        {
          pl[i] = Memory::bswap32le(i | (i << 8) | (i << 16) | (0xFF << 24));
        }
        if (_stream.write((const void*)pl, 1024) != 1024) goto fail;
      }
      else
      {
        // Standard palette.
        if (_stream.write(image.getPalette().getData(), 1024) != 1024) goto fail;
      }

      // Write 8 bit BMP data.
      for (int y = 0; y != height; y++)
      {
        if (_stream.write((const void *)image.getScanline(height - 1 - y), width) != width) goto fail;
        if (_stream.write((const void *)tailBytes, skip) != skip) goto fail;
        if ((y & 15) == 0) updateProgress(y, height);
      }
      break;
    }
  }

end:
  return err;

fail:
  return ERR_IO_CANT_WRITE;
}

} // ImageIO namespace
} // Fog namespace

FOG_IMPLEMENT_OBJECT(Fog::ImageIO::BmpDecoderDevice)
FOG_IMPLEMENT_OBJECT(Fog::ImageIO::BmpEncoderDevice)

// ============================================================================
// [CAPI]
// ============================================================================

FOG_INIT_DECLARE void fog_imageio_init_bmp(void)
{
  using namespace Fog;
  ImageIO::addProvider(IMAGE_IO_DEVICE_BOTH, new(std::nothrow) ImageIO::BmpProvider());
}
