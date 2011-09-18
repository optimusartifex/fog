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
#include <Fog/Core/IO/Stream.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/BSwap.h>
#include <Fog/Core/Memory/MemBufferTmp_p.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/Core/Tools/ManagedString.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/Strings.h>
#include <Fog/G2d/Imaging/Codecs/BmpCodec_p.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/ImageConverter.h>

FOG_IMPLEMENT_OBJECT(Fog::BmpDecoder)
FOG_IMPLEMENT_OBJECT(Fog::BmpEncoder)

namespace Fog {

// ============================================================================
// [Fog::BmpCodecProvider]
// ============================================================================

BmpCodecProvider::BmpCodecProvider()
{
  // Name of ImageCodecProvider.
  _name = fog_strings->getString(STR_G2D_STREAM_BMP);

  // Supported codecs.
  _codecType = IMAGE_CODEC_BOTH;

  // Supported stream.
  _streamType = IMAGE_STREAM_BMP;

  // Supported extensions.
  _imageExtensions.reserve(2);
  _imageExtensions.append(fog_strings->getString(STR_G2D_EXTENSION_bmp));
  _imageExtensions.append(fog_strings->getString(STR_G2D_EXTENSION_ras));
}

BmpCodecProvider::~BmpCodecProvider()
{
}

uint32_t BmpCodecProvider::checkSignature(const void* mem, size_t length) const
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

err_t BmpCodecProvider::createCodec(uint32_t codecType, ImageCodec** codec) const
{
  ImageCodec* c = NULL;

  switch (codecType)
  {
    case IMAGE_CODEC_DECODER:
      c = fog_new BmpDecoder(const_cast<BmpCodecProvider*>(this));
      break;
    case IMAGE_CODEC_ENCODER:
      c = fog_new BmpEncoder(const_cast<BmpCodecProvider*>(this));
      break;
    default:
      return ERR_RT_INVALID_ARGUMENT;
  }

  if (FOG_IS_NULL(c)) return ERR_RT_OUT_OF_MEMORY;

  *codec = c;
  return ERR_OK;
}

// ============================================================================
// [Fog::BmpCodecProvider - Helpers]
// ============================================================================

static bool _BmpCheckHeaderSize(uint32_t headerSize)
{
  return headerSize == BMP_HEADER_SIZE_OS2_V1 ||
         headerSize == BMP_HEADER_SIZE_WIN_V3 ||
         headerSize == BMP_HEADER_SIZE_WIN_V4 ||
         headerSize == BMP_HEADER_SIZE_WIN_V5 ;
}

static bool _BmpCheckDepth(uint32_t depth)
{
  return depth ==  1 ||
         depth ==  4 ||
         depth ==  8 ||
         depth == 16 ||
         depth == 24 ||
         depth == 32 ;
}

static void _BmpSwapFileHeader(BmpFileHeader* h)
{
#if FOG_BYTE_ORDER == FOG_BIG_ENDIAN
  h->fileSize                  = Memory::bswap32(h->fileSize);
  h->imageOffset               = Memory::bswap32(h->imageOffset);
#endif // FOG_BYTE_ORDER
}

static void _BmpSwapDataHeader(BmpDataHeader* h, uint32_t headerSize)
{
#if FOG_BYTE_ORDER == FOG_BIG_ENDIAN
  switch (headerSize)
  {
    case BMP_HEADER_SIZE_OS2_V1:
    {
      h->os2v1.width           = Memory::bswap16le(h->os2v1.width);
      h->os2v1.height          = Memory::bswap16le(h->os2v1.height);
      h->os2v1.planes          = Memory::bswap16le(h->os2v1.planes);
      h->os2v1.bitsPerPixel    = Memory::bswap16le(h->os2v1.bitsPerPixel);
      break;
    }

    case BMP_HEADER_SIZE_WIN_V5:
      h->winv5.intent          = Memory::bswap32le(h->winv5.intent);
      h->winv5.profileData     = Memory::bswap32le(h->winv5.profileData);
      h->winv5.profileSize     = Memory::bswap32le(h->winv5.profileSize);
      // ... Fall through ...

    case BMP_HEADER_SIZE_WIN_V4:
      h->winv4.rMask           = Memory::bswap32le(h->winv4.rMask);
      h->winv4.gMask           = Memory::bswap32le(h->winv4.gMask);
      h->winv4.bMask           = Memory::bswap32le(h->winv4.bMask);
      h->winv4.aMask           = Memory::bswap32le(h->winv4.aMask);
      h->winv4.colorspace      = Memory::bswap32le(h->winv4.colorspace);
      h->winv4.rX              = Memory::bswap32le(h->winv4.rX);
      h->winv4.rY              = Memory::bswap32le(h->winv4.rY);
      h->winv4.rZ              = Memory::bswap32le(h->winv4.rZ);
      h->winv4.gX              = Memory::bswap32le(h->winv4.gX);
      h->winv4.gY              = Memory::bswap32le(h->winv4.gY);
      h->winv4.gZ              = Memory::bswap32le(h->winv4.gZ);
      h->winv4.bX              = Memory::bswap32le(h->winv4.bX);
      h->winv4.bY              = Memory::bswap32le(h->winv4.bY);
      h->winv4.bZ              = Memory::bswap32le(h->winv4.bZ);
      h->winv4.rGamma          = Memory::bswap32le(h->winv4.rGamma);
      h->winv4.gGamma          = Memory::bswap32le(h->winv4.gGamma);
      h->winv4.bGamma          = Memory::bswap32le(h->winv4.bGamma);
      // ... Fall through ...

    case BMP_HEADER_SIZE_WIN_V3:
      h->winv3.width           = Memory::bswap32le(h->winv3.width);
      h->winv3.height          = Memory::bswap32le(h->winv3.height);
      h->winv3.planes          = Memory::bswap16le(h->winv3.planes);
      h->winv3.bitsPerPixel    = Memory::bswap16le(h->winv3.bitsPerPixel);
      h->winv3.compression     = Memory::bswap32le(h->winv3.compression);
      h->winv3.imageSize       = Memory::bswap32le(h->winv3.imageSize);
      h->winv3.horzResolution  = Memory::bswap32le(h->winv3.horzResolution);
      h->winv3.vertResolution  = Memory::bswap32le(h->winv3.vertResolution);
      h->winv3.colorsUsed      = Memory::bswap32le(h->winv3.colorsUsed);
      h->winv3.colorsImportant = Memory::bswap32le(h->winv3.colorsImportant);
      break;
  }
#endif
}

// ============================================================================
// [Fog::BmpDecoder - Construction / Destruction]
// ============================================================================

BmpDecoder::BmpDecoder(ImageCodecProvider* provider) :
  ImageDecoder(provider)
{
  zeroall();
}

BmpDecoder::~BmpDecoder()
{
}

// ============================================================================
// [Fog::BmpDecoder - Reset]
// ============================================================================

void BmpDecoder::reset()
{
  ImageDecoder::reset();
  zeroall();
}

// ============================================================================
// [Fog::BmpDecoder - ReadHeader]
// ============================================================================

err_t BmpDecoder::readHeader()
{
  uint32_t headerSize;
  uint32_t headerTotalBytes;

  // Don't read header more than once.
  if (isHeaderDone()) return _headerResult;

  // Mark header as done.
  _headerDone = true;

  // Clear headers. This is not important step, but safe.
  MemOps::zero(&bmpFileHeader, sizeof(BmpFileHeader));
  MemOps::zero(&bmpDataHeader, sizeof(bmpDataHeader));

  // Read bmp header or skip it if configured for this.
  if (!_skipFileHeader)
  {
    if (_stream.read(&bmpFileHeader, sizeof(BmpFileHeader)) != sizeof(BmpFileHeader))
      return (_headerResult = ERR_IMAGE_TRUNCATED);

    // Check for the BMP-Mime "BM".
    if (bmpFileHeader.magic_B != 'B' || bmpFileHeader.magic_M != 'M')
      return (_headerResult = ERR_IMAGE_MIME_NOT_MATCH);

    _BmpSwapFileHeader(&bmpFileHeader);
  }

  // Read the header size to be able to determine the bitmap version.
  if (_stream.read(&bmpDataHeader.headerSize, 4) != 4)
    return (_headerResult = ERR_IMAGE_TRUNCATED);
  headerSize = Memory::bswap32le(bmpDataHeader.headerSize);

  // First check if the header is supported by the BmpDecoder.
  if (!_BmpCheckHeaderSize(headerSize))
    return (_headerResult = ERR_IMAGEIO_UNSUPPORTED_FORMAT);
  headerTotalBytes = 14 + headerSize;

  // Read the data (DIB) header.
  if (_stream.read(reinterpret_cast<char*>(&bmpDataHeader) + 4, headerSize - 4) != headerSize - 4)
    return (_headerResult = ERR_IMAGE_TRUNCATED);
  _BmpSwapDataHeader(&bmpDataHeader, headerSize);

  switch (headerSize)
  {
    case BMP_HEADER_SIZE_OS2_V1:
      _size.w = bmpDataHeader.os2v1.width;
      _size.h = bmpDataHeader.os2v1.height;
      _depth  = bmpDataHeader.os2v1.bitsPerPixel;
      _planes = bmpDataHeader.os2v1.planes;

      bmpCompression = BMP_BI_RGB;
      bmpSkipBytes = 0;
      break;

    case BMP_HEADER_SIZE_WIN_V5:
    case BMP_HEADER_SIZE_WIN_V4:
    case BMP_HEADER_SIZE_WIN_V3:
      _size.w = bmpDataHeader.winv3.width;
      _size.h = bmpDataHeader.winv3.height;
      _depth  = bmpDataHeader.winv3.bitsPerPixel;
      _planes = bmpDataHeader.winv3.planes;

      bmpCompression = bmpDataHeader.winv3.compression;
      bmpImageSize   = bmpDataHeader.winv3.imageSize;

      bmpFileHeader.imageOffset = Math::max(bmpFileHeader.imageOffset, headerTotalBytes);
      bmpSkipBytes = bmpFileHeader.imageOffset - headerTotalBytes;
      break;
  }

  // Check for correct depth.
  if (!_BmpCheckDepth(_depth))
    return (_headerResult = ERR_IMAGEIO_UNSUPPORTED_FORMAT);

  // Check whether the image size is valid.
  if (!checkImageSize())
    return (_headerResult = ERR_IMAGE_INVALID_SIZE);

  // BMP contains only one image.
  _actualFrame = 0;
  _framesCount = 1;

  // Calculate the BMP stride (BMP is always aligned to 32 bits).
  bmpStride = (((_size.w * _depth + 7) >> 3) + 3) & ~3;

  // OS2 header doesn't define the image size.
  if (bmpImageSize == 0) bmpImageSize = bmpStride * _size.h;

  switch (_depth)
  {
    // Setup palette for indexed images.
    case 1:
    case 4:
    case 8:
    {
      _headerResult = _palette.detach();
      if (FOG_IS_ERROR(_headerResult)) return _headerResult;

      uint32_t* pal = reinterpret_cast<uint32_t*>(_palette.getDataX());
      uint32_t palSize;

      size_t i;
      size_t nColors = (bmpFileHeader.imageOffset - headerTotalBytes);

      if (headerSize == BMP_HEADER_SIZE_OS2_V1)
      {
        uint8_t pal24[768];
        uint8_t* pal24Cur = pal24;

        nColors = Math::min<size_t>(nColors / 3, 256);
        palSize = (uint32_t)nColors * 3;

        if (_stream.read(pal24, palSize) != palSize)
          return (_headerResult = ERR_IMAGE_TRUNCATED);

        for (i = 0; i < nColors; i++, pal24Cur += 3)
        {
          pal[i] = Argb32(pal24Cur[2], pal24Cur[1], pal24Cur[0], 0xFF);
        }
      }
      else
      {
        nColors = Math::min<size_t>(nColors / 4, 256);
        palSize = (uint32_t)nColors * 4;

        if (_stream.read(pal, palSize) != palSize)
          return (_headerResult = ERR_IMAGE_TRUNCATED);

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
        for (i = 0; i != nColors; i++) pal[i] |= 0xFF000000;
#else
        for (i = 0; i != nColors; i++) pal[i] = Memory::bswap32(pal[i]) | 0xFF000000;
#endif
      }

      bmpSkipBytes = (bmpSkipBytes < palSize) ? 0 : bmpSkipBytes - palSize;
      bmpFormat = ImageFormatDescription::getByFormat(IMAGE_FORMAT_I8);
      break;
    }

    // Setup 16-bit RGB (555).
    case 16:
    {
      _headerResult = bmpFormat.createArgb(16,
        FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
          ? IMAGE_FD_NONE
          : IMAGE_FD_IS_BYTESWAPPED,
        0, 0x7C00, 0x03E0, 0x001F);
      break;
    }

    // Setup 24-bit RGB.
    case 24:
    {
      _headerResult = bmpFormat.createArgb(24,
        FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
          ? IMAGE_FD_NONE
          : IMAGE_FD_IS_BYTESWAPPED,
        0, 0x00FF0000, 0x0000FF00, 0x000000FF);
      break;
    }

    // Setup 32-bit RGB (It's possible that alpha-channel is active).
    case 32:
    {
      if (bmpDataHeader.headerSize > BMP_HEADER_SIZE_WIN_V3)
      {
        _headerResult = bmpFormat.createArgb(32,
          FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
            ? IMAGE_FD_IS_PREMULTIPLIED
            : IMAGE_FD_IS_PREMULTIPLIED | IMAGE_FD_IS_BYTESWAPPED,
          (bmpCompression != BMP_BI_BITFIELDS) ? bmpDataHeader.winv4.aMask : 0,
          bmpDataHeader.winv4.rMask,
          bmpDataHeader.winv4.gMask,
          bmpDataHeader.winv4.bMask);
      }
      else
      {
        _headerResult = bmpFormat.createArgb(32,
          FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
            ? IMAGE_FD_NONE
            : IMAGE_FD_IS_BYTESWAPPED,
          0, 0x00FF0000, 0x0000FF00, 0x000000FF);
      }
      break;
    }
  }

  // Load bitfields.
  if (bmpDataHeader.headerSize == BMP_HEADER_SIZE_WIN_V3 &&
      bmpCompression == BMP_BI_BITFIELDS &&
      (_depth == 16 || _depth == 32))
  {
    uint32_t bitFieldMask[3];
    if (_stream.read(bitFieldMask, 12) != 12)
      return (_headerResult = ERR_IMAGE_TRUNCATED);

    bmpSkipBytes = (bmpSkipBytes < 12) ? 0 : bmpSkipBytes - 12;
    bitFieldMask[0] = Memory::bswap32le(bitFieldMask[0]);
    bitFieldMask[1] = Memory::bswap32le(bitFieldMask[1]);
    bitFieldMask[2] = Memory::bswap32le(bitFieldMask[2]);

    // The bits in the pixel are ordered from most significant to least
    // significant bits. The ImageConverter expects the opposite ordering.
    if (_depth == 16)
    {
      bitFieldMask[0] >>= 16;
      bitFieldMask[1] >>= 16;
      bitFieldMask[2] >>= 16;
    }

    _headerResult = bmpFormat.createArgb(_depth,
      FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
        ? IMAGE_FD_NONE
        : IMAGE_FD_IS_BYTESWAPPED,
      0, bitFieldMask[0], bitFieldMask[1], bitFieldMask[2]);
  }

  // Success.
  return (_headerResult = ERR_OK);
}

// ============================================================================
// [Fog::BmpDecoder - ReadImage]
// ============================================================================

err_t BmpDecoder::readImage(Image& image)
{
  // Read the bmp header.
  if (readHeader() != ERR_OK) return _headerResult;

  // Don't read the image more than once.
  if (isReaderDone()) return (_readerResult = ERR_IMAGE_NO_MORE_FRAMES);

  // Error code (default is success).
  uint32_t err = ERR_OK;

  // Buffer pointers.
  uint8_t* pixelsBegin;
  uint8_t* pixelsCur;
  ssize_t stride;

  // Reader variables.
  uint32_t x = 0;
  uint32_t y = 0;
  uint32_t i;

  MemBufferTmp<512> rawBufferStorage;
  MemBufferTmp<512> rleBufferStorage;

  uint8_t* buffer = reinterpret_cast<uint8_t*>(rawBufferStorage.alloc(bmpStride));
  uint8_t* rleBuffer = NULL;

  // First skip bytes if needed.
  if (bmpSkipBytes > 0 && _stream.seek(bmpSkipBytes, STREAM_SEEK_CUR) == -1)
  {
    err = ERR_IO_CANT_SEEK;
    goto _End;
  }

  // Create image.
  if ((err = image.create(_size, _format))) goto _End;

  pixelsBegin = image.getScanlineX(0);
  stride = image.getStride();
  pixelsCur = pixelsBegin + stride * (_size.h - 1);

  // --------------------------------------------------------------------------
  // [Read - 1 Bit]
  // --------------------------------------------------------------------------

  if (_depth == 1)
  {
    FOG_ASSERT(_format == IMAGE_FORMAT_I8);

    if (bmpCompression == BMP_BI_RGB)
    {
      uint8_t* bufferCur;
      uint32_t b;

      for (y = 0; y != (uint)_size.h; y++)
      {
        if (_stream.read(buffer, bmpStride) != bmpStride) goto _Truncated;
        bufferCur = buffer;
        pixelsCur = pixelsBegin + (_size.h - y - 1) * stride;

        for (i = _size.w; i >= 8; i -= 8, pixelsCur += 8, bufferCur++)
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

        if ((y & 15) == 0) { updateProgress(y, _size.h); }
      }
    }
  }

  // --------------------------------------------------------------------------
  // [Read - 4 Bits RLE]
  // --------------------------------------------------------------------------

  else if (_depth == 4 && bmpCompression == BMP_BI_RLE4)
  {
    uint8_t* rleCur;
    uint8_t* rleEnd;
    uint8_t b0;
    uint8_t b1;

    rleBuffer = reinterpret_cast<uint8_t*>(rleBufferStorage.alloc(bmpImageSize));
    if (FOG_IS_NULL(rleBuffer))
      goto _OutOfMemory;
    
    if (_stream.read(rleBuffer, bmpImageSize) != bmpImageSize)
      goto _Truncated;

    rleCur = rleBuffer;
    rleEnd = rleBuffer + bmpImageSize;

_Rle4Start:
    if (x >= (uint32_t)_size.w || y >= (uint32_t)_size.h) goto _RleError;

    pixelsCur = pixelsBegin + (_size.h - y - 1) * stride;
    updateProgress(y, _size.h);

    for (;;)
    {
      if (rleCur + 2 > rleEnd) goto _Truncated;
      b0 = *rleCur++;
      b1 = *rleCur++;

      if (b0)
      {
        // b0 = Length.
        // b1 = 2 Colors.
        uint8_t c0 = b1 >> 4;
        uint8_t c1 = b1 & 0xF;

        i = Math::min<uint32_t>(b0, _size.w - x);
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
          case BMP_RLE_NEXT_LINE: x = 0; y++; goto _Rle4Start;
          case BMP_RLE_END: goto _End;
          case BMP_RLE_MOVE:
            if (rleCur + 2 > rleEnd) goto _Truncated;
            x += *rleCur++;
            y += *rleCur++;
            goto _Rle4Start;
          // FILL BITS (b1 == length).
          default:
          {
            uint8_t* backup = rleCur;
            i = Math::min<uint32_t>(b1, _size.w - x);

            if (rleCur + ((b1 + 1) >> 1) > rleEnd) goto _Truncated;
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

  // --------------------------------------------------------------------------
  // [Read - 4 Bits RAW]
  // --------------------------------------------------------------------------

  else if (_depth == 4)
  {
    FOG_ASSERT(_format == IMAGE_FORMAT_I8);

    uint8_t* bufferCur;
    uint8_t b;

    for (y = 0; y != (uint)_size.h; y++)
    {
      if (_stream.read(buffer, bmpStride) != bmpStride) goto _Truncated;
      bufferCur = buffer;
      pixelsCur = pixelsBegin + (_size.h - y - 1) * stride;

      for (x = 0; x + 2 <= (uint32_t)_size.w; x += 2)
      {
        b = *bufferCur++;
        *pixelsCur++ = b >> 4;
        *pixelsCur++ = b & 0xF;
      }
      if (x < (uint32_t)_size.w)
      {
        *pixelsCur = *bufferCur >> 4;
      }
      if ((y & 15) == 0) updateProgress(y, _size.h);
    }
  }

  // --------------------------------------------------------------------------
  // [Read - 8 Bits RLE]
  // --------------------------------------------------------------------------

  else if (_depth == 8 && bmpCompression == BMP_BI_RLE8)
  {
    uint8_t* rleCur;
    uint8_t* rleEnd;
    uint8_t b0;
    uint8_t b1;

    if ((rleBuffer = (uint8_t *)rleBufferStorage.alloc(bmpImageSize)) == NULL) goto _OutOfMemory;
    if (_stream.read(rleBuffer, bmpImageSize) != bmpImageSize) goto _Truncated;

    rleCur = rleBuffer;
    rleEnd = rleBuffer + bmpImageSize;

_Rle8Start:
    if (x >= (uint32_t)_size.w || y >= (uint32_t)_size.h) goto _RleError;

    pixelsCur = pixelsBegin + (_size.h - y - 1) * stride;
    updateProgress(y, _size.h);

    for (;;)
    {
      if (rleCur + 2 > rleEnd) goto _Truncated;
      b0 = *rleCur++;
      b1 = *rleCur++;

      if (b0)
      {
        // b0 = Length.
        // b1 = Color.
        i = Math::min<uint32_t>(b0, (uint32_t)_size.w - x);
        while (i--) *pixelsCur++ = b1;
      }
      else
      {
        // b1 = Chunk type.
        switch (b1)
        {
          case BMP_RLE_NEXT_LINE: x = 0; y++; goto _Rle8Start;
          case BMP_RLE_END: goto _End;
          case BMP_RLE_MOVE:
            if (rleCur + 2 > rleEnd) goto _Truncated;
            x += *rleCur++;
            y += *rleCur++;
            goto _Rle8Start;
          // FILL BITS (b1 == length).
          default:
          {
            uint8_t* backup = rleCur;

            i = Math::min<uint32_t>(b1, _size.w - x);
            if (rleCur + b1 > rleEnd) goto _Truncated;
            x += i;

            while (i--) *pixelsCur++ = *rleCur++;

            rleCur = backup + b1;
            if (b1 & 1) rleCur++;
            break;
          }
        }
      }
    }
    updateProgress(y, _size.h);
  }

  // --------------------------------------------------------------------------
  // [Read - 8 Bits RAW]
  // --------------------------------------------------------------------------

  else if (_depth == 8)
  {
    for (y = 0; y < (uint32_t)_size.h; y++)
    {
      pixelsCur = pixelsBegin + (_size.h - y - 1) * stride;
      if (_stream.read(pixelsCur, bmpStride) != bmpStride) goto _Truncated;
      if ((y & 15) == 0) updateProgress(y, _size.h);
    }
  }

  // --------------------------------------------------------------------------
  // [Read - 16-32 Bits]
  // --------------------------------------------------------------------------

  else
  {
    ImageConverter converter;
    err = converter.create(ImageFormatDescription::getByFormat(_format), bmpFormat);
    if (FOG_IS_ERROR(err)) goto _End;

    pixelsCur = pixelsBegin + (_size.h - y - 1) * stride;

    if (converter.isCopy())
    {
      size_t readBytes = _size.w * (_depth >> 3);
      size_t zeroBytes = bmpStride - readBytes;

      for (y = 0; y < (uint32_t)_size.h; y++, pixelsCur -= stride)
      {
        if (_stream.read(pixelsCur, readBytes) != readBytes) goto _Truncated;
        if (zeroBytes && _stream.read(buffer, zeroBytes) != zeroBytes) goto _Truncated;

        if ((y & 15) == 0) updateProgress(y, _size.h);
      }
    }
    else
    {
      PointI ditherOrigin(0, 0);
      for (y = 0; y < (uint32_t)_size.h; y++, pixelsCur -= stride, ditherOrigin.y++)
      {
        if (_stream.read(buffer, bmpStride) != bmpStride) goto _Truncated;
        converter.blitSpan(pixelsCur, buffer, _size.w, ditherOrigin);

        if ((y & 15) == 0) updateProgress(y, _size.h);
      }
    }
  }
  goto _End;

_Truncated:
  err = ERR_IMAGE_TRUNCATED;
  goto _End;

_RleError:
  err = ERR_IMAGE_MALFORMED_RLE;
  goto _End;

_OutOfMemory:
  err = ERR_RT_OUT_OF_MEMORY;
  goto _End;

_End:
  image._modified();
  // Apply palette if needed.
  if (_depth <= 8 && !image.isEmpty()) image.setPalette(_palette);

  if (err == ERR_OK) updateProgress(1.0f);
  return (_readerResult = err);
}

err_t BmpDecoder::getProperty(const ManagedString& name, Var& dst) const
{
  if (name == fog_strings->getString(STR_G2D_CODEC_skipFileHeader))
    return dst.setInt(_skipFileHeader);

  return base::getProperty(name, dst);
}

err_t BmpDecoder::setProperty(const ManagedString& name, const Var& src)
{
  if (name == fog_strings->getString(STR_G2D_CODEC_skipFileHeader))
    return src.getInt(_skipFileHeader, 0, 1);

  return base::setProperty(name, src);
}

// ============================================================================
// [Fog::BmpEncoder - Construction / Destruction]
// ============================================================================

BmpEncoder::BmpEncoder(ImageCodecProvider* provider) :
  ImageEncoder(provider)
{
}

BmpEncoder::~BmpEncoder()
{
}

// ============================================================================
// [Fog::BmpEncoder - WriteImage]
// ============================================================================

err_t BmpEncoder::writeImage(const Image& image)
{
  static const uint8_t zeroBytes[4] = { 0, 0, 0, 0 };
  err_t err = ERR_OK;

  BmpFileHeader bmpFileHeader;
  BmpDataHeader bmpDataHeader;
  MemBufferTmp<1024> bufferLocal;

  ImageConverter converter;
  ImageData* d = image._d;

  int w = d->size.w;
  int h = d->size.h;

  uint32_t bpl;
  uint32_t skip;
  uint32_t headerSize = BMP_HEADER_SIZE_WIN_V3;
  uint32_t imageSize = 0;
  uint32_t paletteEntries = 0;

  if (w <= 0 || h <= 0)
  {
    err = ERR_IMAGE_INVALID_SIZE;
    goto _End;
  }

  MemOps::zero(&bmpDataHeader, sizeof(BmpDataHeader));

  // Bitmap file header.
  bmpFileHeader.magic_B                = 'B';
  bmpFileHeader.magic_M                = 'M';
  bmpFileHeader.reserved1              = 0x0000;
  bmpFileHeader.reserved2              = 0x0000;

  // Bitmap data header.
  bmpDataHeader.winv3.width            = w;
  bmpDataHeader.winv3.height           = h;
  bmpDataHeader.winv3.planes           = 1;
  bmpDataHeader.winv3.compression      = BMP_BI_RGB;
  bmpDataHeader.winv4.colorspace       = BMP_COLORSPACE_DD_RGB;

  switch (d->format)
  {
    case IMAGE_FORMAT_PRGB32:
    case IMAGE_FORMAT_PRGB64:
    case IMAGE_FORMAT_A8:
    case IMAGE_FORMAT_A16:
    {
      bpl = (uint32_t)w * 4;
      skip = 0;
      imageSize = bpl * (uint32_t)h;
      headerSize = BMP_HEADER_SIZE_WIN_V4;

      bmpDataHeader.winv3.bitsPerPixel = 32;
      bmpDataHeader.winv4.rMask = 0x00FF0000;
      bmpDataHeader.winv4.gMask = 0x0000FF00;
      bmpDataHeader.winv4.bMask = 0x000000FF;
      bmpDataHeader.winv4.aMask = 0xFF000000;

      err = converter.create(
        ImageFormatDescription::fromArgb(32,
          FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
            ? IMAGE_FD_IS_PREMULTIPLIED
            : IMAGE_FD_IS_PREMULTIPLIED | IMAGE_FD_IS_BYTESWAPPED,
          0xFF000000U, 0x00FF0000U, 0x0000FF00U, 0x000000FFU),
        ImageFormatDescription::getByFormat(d->format));
      break;
    }

    case IMAGE_FORMAT_XRGB32:
    case IMAGE_FORMAT_RGB24:
    case IMAGE_FORMAT_RGB48:
    {
      bpl = (uint32_t)w * 3;
      skip = (4 - (bpl & 3)) & 3;
      imageSize = (bpl + skip) * (uint32_t)h;

      bmpDataHeader.winv3.bitsPerPixel = 24;

      err = converter.create(
        ImageFormatDescription::fromArgb(24,
          FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
            ? IMAGE_FD_NONE
            : IMAGE_FD_IS_BYTESWAPPED,
          0, 0x00FF0000U, 0x0000FF00U, 0x000000FFU),
        ImageFormatDescription::getByFormat(d->format));
      break;
    }

    case IMAGE_FORMAT_I8:
    {
      bpl = (uint32_t)w;
      skip = (4 - (bpl & 3)) & 3;
      imageSize = (uint32_t)(bpl + skip) * (uint32_t)h;
      paletteEntries = 256;

      bmpDataHeader.winv3.bitsPerPixel = 8;

      err = converter.create(
        ImageFormatDescription::getByFormat(IMAGE_FORMAT_I8),
        ImageFormatDescription::getByFormat(IMAGE_FORMAT_I8), false, &d->palette, &d->palette);
      break;
    }

    default:
    {
      err = ERR_IMAGE_UNSUPPORTED_FORMAT;
      break;
    }
  }

  if (FOG_IS_ERROR(err)) goto _End;

  bmpFileHeader.imageOffset      = 14 + headerSize + paletteEntries * 4;
  bmpFileHeader.fileSize         = bmpFileHeader.imageOffset + imageSize;
  bmpDataHeader.winv3.headerSize = headerSize;
  bmpDataHeader.winv3.imageSize  = imageSize;

  _BmpSwapFileHeader(&bmpFileHeader);
  _BmpSwapDataHeader(&bmpDataHeader, headerSize);

  // Write headers.
  if (_stream.write(&bmpFileHeader, 14) != 14) goto _Fail;
  if (_stream.write(&bmpDataHeader, headerSize) != headerSize) goto _Fail;

  // Write palette.
  if (paletteEntries)
  {
    uint32_t* buffer = reinterpret_cast<uint32_t*>(bufferLocal.getMem());
    const uint32_t* pal = reinterpret_cast<const uint32_t*>(d->palette.getData());

    FOG_ASSERT(buffer != NULL);

    for (uint i = 0; i < paletteEntries; i++)
      buffer[i] = pal[i] & 0x00FFFFFF;

    if (_stream.write((const void*)buffer, paletteEntries * 4) != paletteEntries * 4)
      goto _Fail;
  }

  // Write scanlines.
  {
    const uint8_t* scanline = image.getScanline(h - 1);
    ssize_t stride = -image.getStride();

    if (converter.isCopy())
    {
      for (uint y = 0; y != (uint)h; y++, scanline += stride)
      {
        if (_stream.write(scanline, bpl) != bpl) goto _Fail;
        if (skip && _stream.write(zeroBytes, skip) != skip) goto _Fail;

        if ((y & 15) == 0) updateProgress(y, h);
      }
    }
    else
    {
      uint8_t* buffer = (uint8_t *)bufferLocal.alloc(bpl + skip);
      if (FOG_IS_NULL(buffer)) { err = ERR_RT_OUT_OF_MEMORY; goto _End; }

      PointI ditherOrigin(0, h - 1);

      // Zero the padding bytes.
      memset(buffer + bpl, 0, skip);
      bpl += skip;

      for (uint y = 0; y != (uint)h; y++, scanline += stride, ditherOrigin.y--)
      {
        converter.blitSpan(buffer, scanline, w, ditherOrigin);
        if (_stream.write(buffer, bpl) != bpl) goto _Fail;

        if ((y & 15) == 0) updateProgress(y, h);
      }
    }
  }

_End:
  return err;

_Fail:
  return ERR_IO_CANT_WRITE;
}

// ===========================================================================
// [Init / Fini]
// ===========================================================================

FOG_NO_EXPORT void ImageCodecProvider_initBMP(void)
{
  ImageCodecProvider* provider = fog_new BmpCodecProvider();
  ImageCodecProvider::addProvider(provider);
  provider->deref();
}

} // Fog namespace
