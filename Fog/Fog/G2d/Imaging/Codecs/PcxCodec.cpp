// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Collection/BufferP.h>
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/IO/Stream.h>
#include <Fog/Core/Memory/BSwap.h>
#include <Fog/Core/Memory/Ops.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/Strings.h>
#include <Fog/G2d/Imaging/Codecs/PcxCodec_p.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/ImageConverter.h>
#include <Fog/G2d/Render/RenderApi_p.h>

// [Options]
#define FOG_PCX_STRICT 1

FOG_IMPLEMENT_OBJECT(Fog::PcxDecoder)
FOG_IMPLEMENT_OBJECT(Fog::PcxEncoder)

namespace Fog {

// ============================================================================
// [Fog::PcxCodecProvider]
// ============================================================================

PcxCodecProvider::PcxCodecProvider()
{
  // Name of ImageCodecProvider.
  _name = fog_strings->getString(STR_G2D_STREAM_PCX);

  // Supported codecs.
  _codecType = IMAGE_CODEC_BOTH;

  // Supported stream.
  _streamType = IMAGE_STREAM_PCX;

  // Supported extensions.
  _imageExtensions.reserve(1);
  _imageExtensions.append(fog_strings->getString(STR_G2D_EXTENSION_pcx));
}

PcxCodecProvider::~PcxCodecProvider()
{
}

uint32_t PcxCodecProvider::checkSignature(const void* mem, size_t length) const
{
  if (!mem || length == 0) return 0;

  const uint8_t* m = (const uint8_t*)mem;

  if (length >= 3)
  {
    // [0] == Manufacturer (0x0A).
    // [1] == Version      (0x05).
    // [2] == Encoding     (0x01).
    if (m[0] == 0x0A && m[1] <= 0x05 && m[2] == 0x01) return 90;
  }

  return 0;
}

err_t PcxCodecProvider::createCodec(uint32_t codecType, ImageCodec** codec) const
{
  ImageCodec* c = NULL;

  switch (codecType)
  {
    case IMAGE_CODEC_DECODER:
      c = fog_new PcxDecoder(const_cast<PcxCodecProvider*>(this));
      break;
    case IMAGE_CODEC_ENCODER:
      c = fog_new PcxEncoder(const_cast<PcxCodecProvider*>(this));
      break;
    default:
      return ERR_RT_INVALID_ARGUMENT;
  }

  if (FOG_IS_NULL(c)) return ERR_RT_OUT_OF_MEMORY;

  *codec = c;
  return ERR_OK;
}

// ============================================================================
// [Fog::PcxCodecProvider - Helpers]
// ============================================================================

static void _PcxSwapHeader(PcxHeader* h)
{
#if FOG_BYTE_ORDER == FOG_BIG_ENDIAN
  h->xMin          = Memory::bswap16(h->xMin);
  h->yMin          = Memory::bswap16(h->yMin);
  h->xMax          = Memory::bswap16(h->xMax);
  h->yMax          = Memory::bswap16(h->yMax);
  h->horizontalDPI = Memory::bswap16(h->horizontalDPI);
  h->verticalDPI   = Memory::bswap16(h->verticalDPI);
  h->bytesPerLine  = Memory::bswap16(h->bytesPerLine);
  h->paletteInfo   = Memory::bswap16(h->paletteInfo);
  h->hScreenSize   = Memory::bswap16(h->hScreenSize);
  h->vScreenSize   = Memory::bswap16(h->vScreenSize);
#endif
}

static uint _PcxStrNLen(const uint8_t* str, uint maxlen)
{
  uint n = 0;
  while (n < maxlen && str[n]) n++;
  return n;
}

// Decode the scanline compressed by PCX-RLE.
//
// Returns 0 on success, otherwise an error code.
static err_t _PcxDecodeScanline(
  uint8_t* dst,
  const uint8_t** src, const uint8_t* end,
  uint32_t length, uint32_t ignore, uint32_t increment)
{
  err_t err = ERR_IMAGE_TRUNCATED;

  const uint8_t* cur = *src;
  uint left = length;

  // Decode RLE scanline and write it to 'dst'.
  while (left)
  {
    if (cur == end) goto _Fail;
    uint8_t b = *cur++;

    if (b >= 192)
    {
      uint count = b & 0x3F;
      uint extra = 0;

      if (cur == end) goto _Fail;
      b = *cur++;

      if (left < count)
      {
        extra = count - left;
        count = left;
      }

      left -= count;

      while (count--)
      {
        *dst = b;
        dst += increment;
      }

      if (extra)
      {
        // This should happen only if 'ignore' is not zero.
        if (extra <= ignore)
        {
          ignore -= extra;
        }
        else
        {
          ignore = 0;
          err = ERR_IMAGE_MALFORMED_RLE;

          if (FOG_PCX_STRICT) goto _Fail;
        }
      }
    }
    else
    {
      *dst = b;
      dst += increment;
      left--;
    }
  }

  // Decode RLE scanline and discard it.
  while (ignore)
  {
    if (cur == end) goto _Fail;
    uint8_t b = *cur++;

    if (b >= 192)
    {
      uint count = b & 0x3F;

      if (cur == end) goto _Fail;
      b = *cur++;

      if (ignore < count)
      {
        ignore = 0;
        err = ERR_IMAGE_MALFORMED_RLE;
        if (FOG_PCX_STRICT) goto _Fail;
      }
      else
        ignore -= count;
    }
    else
      ignore--;
  }

  err = ERR_OK;

_Fail:
  *src = cur;
  return err;
}

// Encode the scanline using the PCX_RLE compression.
static bool _PcxEncodeScanline(
  Stream& stream,
  uint8_t* buffer, const uint8_t* dataCur,
  size_t length, size_t alignment, size_t increment)
{
  uint8_t *bufferPtr = buffer;
  size_t rleCandidate;
  size_t rleCount;

  const uint8_t* dataEnd = dataCur + length * increment;

  do {
    // Get the sequence of equal pixels.
    rleCandidate = (size_t)dataCur[0];
    rleCount = 1;
    dataCur += increment;

    while (dataCur != dataEnd && rleCandidate == (size_t)dataCur[0] && rleCount < 62)
    {
      dataCur += increment;
      rleCount++;
    }

    // Write the RLE sequence.
    if (rleCount > 1)
    {
      // Multiple pixels (Double-byte code).
      bufferPtr[0] = (uint8_t)(0xC0 | (uint8_t)rleCount);
      bufferPtr[1] = (uint8_t)(rleCandidate);
      bufferPtr += 2;
    }
    else
    {
      // Single pixel.
      if (((uint8_t)rleCandidate & 0xC0) != 0xC0)
      {
        // Single-byte code.
        *bufferPtr++ = (uint8_t)(rleCandidate);
      }
      else
      {
        // Double-byte code.
        bufferPtr[0] = (uint8_t)(0xC1);
        bufferPtr[1] = (uint8_t)(rleCandidate);
        bufferPtr += 2;
      }
    }
  } while (dataCur != dataEnd);

  // Align to 2 bytes.
  //
  // This is in specification, but some libraries ignore it.
  if (alignment)
  {
    // If the last bytes are encoded using RLE, we can increment
    // it if it not exceeds 191
    /*if ((size_t)(bufferPtr - buffer) >= 2 &&
        bufferPtr[-1] > 192 &&
        bufferPtr[-1] < 255 )
    {
      bufferPtr[-1]++;
    }*/
    // Or just add zero byte.
    /*else*/
    {
      *bufferPtr++ = 0;
    }
  }

  // Write the compressed line into the stream and return.
  length = (size_t)(bufferPtr - buffer);
  return stream.write((const char *)buffer, length) == length;
}

static void _PcxFillMonoPalette(uint32_t* palette)
{
  palette[0] = 0xFF000000;
  palette[1] = 0xFFFFFFFF;
}

static void _PcxFillGreyPalette(uint32_t* palette)
{
  uint32_t c = 0xFF000000, i;
  for (i = 256; i; i--, c += 0x00010101, palette++) palette[0] = c;
}

static void _PcxFillEgaPalette(uint32_t* palette)
{
  palette[0x0] = 0xFF000000;
  palette[0x1] = 0xFF0000AA;
  palette[0x2] = 0xFF00AA00;
  palette[0x3] = 0xFF00AAAA;
  palette[0x4] = 0xFFAA0000;
  palette[0x5] = 0xFFAA00AA;
  palette[0x6] = 0xFFAA5500;
  palette[0x7] = 0xFFAAAAAA;
  palette[0x8] = 0xFF555555;
  palette[0x9] = 0xFF5555FF;
  palette[0xA] = 0xFF55FF55;
  palette[0xB] = 0xFF55FFFF;
  palette[0xC] = 0xFFFF5555;
  palette[0xD] = 0xFFFF55FF;
  palette[0xE] = 0xFFFFFF55;
  palette[0xF] = 0xFFFFFFFF;
}

static void _PcxPaletteFromXRGB32(uint8_t* dst, const uint8_t* src, size_t count)
{
  for (size_t i = 0; i != count; i++, dst += 3, src += 4)
  {
    uint32_t c = ((uint32_t *)src)[0];
    dst[0] = (uint8_t)( (c & 0x00FF0000) >> 16 );
    dst[1] = (uint8_t)( (c & 0x0000FF00) >>  8 );
    dst[2] = (uint8_t)( (c & 0x000000FF)       );
  }
}

// ============================================================================
// [Fog::PcxDecoder - Construction / Destruction]
// ============================================================================

PcxDecoder::PcxDecoder(ImageCodecProvider* provider) :
  ImageDecoder(provider)
{
  zeroall();
}

PcxDecoder::~PcxDecoder()
{
}

// ============================================================================
// [Fog::PcxDecoder - Reset]
// ============================================================================

void PcxDecoder::reset()
{
  ImageDecoder::reset();
  zeroall();
}

// ============================================================================
// [Fog::PcxDecoder - ReadHeader]
// ============================================================================

err_t PcxDecoder::readHeader()
{
  // Don't read header more than once.
  if (isHeaderDone()) return getHeaderResult();

  // Mark header as done.
  _headerDone = true;

  // Read the PCX header.
  if (_stream.read(&_pcxFileHeader, sizeof(PcxHeader)) != sizeof(PcxHeader))
  {
    return ERR_IMAGE_TRUNCATED;
  }

  // Check for the correct mime.
  if (/*_pcxFileHeader.manufacturer != 10 ||*/
    _pcxFileHeader.version != 5 ||
    _pcxFileHeader.encoding != 1)
  {
    return ERR_IMAGE_MIME_NOT_MATCH;
  }

  // Byteswap header.
  _PcxSwapHeader(&_pcxFileHeader);

  int16_t xMin = _pcxFileHeader.xMin;
  int16_t yMin = _pcxFileHeader.yMin;
  int16_t xMax = _pcxFileHeader.xMax;
  int16_t yMax = _pcxFileHeader.yMax;

  // Size reject.
  if (xMin > xMax || yMin > yMax) return ERR_IMAGE_INVALID_SIZE;

  // Decode header.
  _size.w = xMax - xMin + 1;
  _size.h = yMax - yMin + 1;
  _depth  = (uint32_t)(_pcxFileHeader.bitsPerPixel);
  _planes = (uint32_t)(_pcxFileHeader.nPlanes);

  // Check whether the image size is valid.
  if (!checkImageSize())
  {
    return (_headerResult = ERR_IMAGE_INVALID_SIZE);
  }

  // Pcx contains only one image.
  _actualFrame = 0;
  _framesCount = 1;

  // Accept only supported formats.
  switch (getDepth())
  {
    case 1:
      if (getPlanes() >= 1 && getPlanes() <= 4) break;
      return (_headerResult = ERR_IMAGEIO_UNSUPPORTED_FORMAT);

    case 4:
      if (getPlanes() == 1) break;
      return (_headerResult = ERR_IMAGEIO_UNSUPPORTED_FORMAT);

    case 8:
      if ((getPlanes() == 1 || getPlanes() == 3 || getPlanes() == 4) &&
          (_pcxFileHeader.version >= 4)) break;
      // ... Fall through ...

    default:
      return (_headerResult = ERR_IMAGEIO_UNSUPPORTED_FORMAT);
  }

  _format = IMAGE_FORMAT_I8;

  // Standardize output to match the Fog-Framework image format.
  if (getDepth() == 8 && getPlanes() == 4)
  {
    _depth = 32;
    _planes = 1;
    _format = IMAGE_FORMAT_PRGB32;
  }
  else if (getDepth() == 8 && getPlanes() == 3)
  {
    _depth = 24;
    _planes = 1;
    _format = IMAGE_FORMAT_RGB24;
  }

  // Success.
  return (_headerResult = ERR_OK);
}

// ============================================================================
// [Fog::PcxDecoder - ReadImage]
// ============================================================================

err_t PcxDecoder::readImage(Image& image)
{
  // Read the PCX header.
  if (readHeader() != ERR_OK) return getHeaderResult();

  // Don't read image more than once.
  if (isReaderDone()) return (_readerResult = ERR_IMAGE_NO_MORE_FRAMES);

  // Error code.
  uint32_t err = ERR_OK;

  // Source.
  ByteArray dataArray;
  const uint8_t* dataCur;
  const uint8_t* dataEnd;

  // Destination.
  uint8_t* pixels;
  sysint_t stride;

  // Temporary plane data.
  BufferP<1024> temporary;
  uint8_t* mem;

  // Bytes per line.
  uint32_t bytesPerLine = _pcxFileHeader.bytesPerLine;

  // Loop variables.
  uint x, y;

  // Secondary palette.
  bool infoSecPalette = false;
  uint infoSecPaletteColors = 0;

  // Image text.
  if (_pcxFileHeader.unused[0])
  {
    const uint8_t* str = (const uint8_t*)_pcxFileHeader.unused;
    _comment.set(Ascii8(str, _PcxStrNLen(str, 54)));
  }
  else
  {
    _comment.reset();
  }

  _stream.readAll(dataArray);
  dataCur = (const uint8_t*)dataArray.getData();
  dataEnd = dataCur + dataArray.getLength();

  if ((err = image.create(_size, _format))) goto _End;

  pixels = image.getScanlineX(0);
  stride = image.getStride();

  // --------------------------------------------------------------------------
  // [Read - 1 Bit, 1-4 Planes]
  // --------------------------------------------------------------------------

  if (_depth == 1)
  {
    uint32_t plane;
    uint8_t b;

    if (!temporary.alloc(bytesPerLine))
    {
      err = ERR_RT_OUT_OF_MEMORY;
      goto _End;
    }

    for (y = 0; y != (uint32_t)_size.h; y++, pixels += stride)
    {
      // Expand planes to 8 BPP.
      for (plane = 0; plane != _planes; plane++)
      {
        mem = (uint8_t*)temporary.getMem();
        if ((err = _PcxDecodeScanline(mem, &dataCur, dataEnd, bytesPerLine, 0, 1)) != ERR_OK) goto _End;

        for (x = 0; x != (uint32_t)_size.w; x++)
        {
          if ((x & 7) == 0)
            b = *mem++;
          else
            b <<= 1;

          pixels[x] |= ((b >> 7) & 1) << plane;
        }
      }
      if ((y & 15) == 0) updateProgress(y, (uint32_t)_size.h);
    }
  }

  // --------------------------------------------------------------------------
  // [Read - 4 Bits, 1 Plane]
  // --------------------------------------------------------------------------

  else if (_depth == 4)
  {
    if (!temporary.alloc(bytesPerLine))
    {
      err = ERR_RT_OUT_OF_MEMORY;
      goto _End;
    }

    for (y = 0; y != (uint32_t)_size.h; y++, pixels += stride)
    {
      mem = (uint8_t*)temporary.getMem();
      if ((err = _PcxDecodeScanline(mem, &dataCur, dataEnd, bytesPerLine, 0, 1)) != ERR_OK) goto _End;

      for (x = 0; x != (uint32_t)_size.w; x++)
      {
        pixels[x] = (!(x & 1)) ? ((*mem & 0xF0) >> 4) : (*mem++ & 0xF);
      }

      if ((y & 15) == 0) updateProgress(y, (uint32_t)_size.h);
    }
  }

  // --------------------------------------------------------------------------
  // [Read - 8 Bits, 1, 3, and 4 Planes]
  // --------------------------------------------------------------------------

  else
  {
    uint32_t pos[4];
    uint32_t ignore = bytesPerLine - (uint32_t)_size.w;
    uint32_t increment;
    uint32_t plane;
    uint32_t planeMax = 1;

    switch (_depth)
    {
      case 8:
        pos[0] = 0;
        increment = 1;
        break;

      case 24:
        pos[0] = PIXEL_RGB24_POS_R;
        pos[1] = PIXEL_RGB24_POS_G;
        pos[2] = PIXEL_RGB24_POS_B;
        increment = 3;
        planeMax = 3;
        break;

      case 32:
        pos[0] = PIXEL_ARGB32_POS_R;
        pos[1] = PIXEL_ARGB32_POS_G;
        pos[2] = PIXEL_ARGB32_POS_B;
        pos[3] = PIXEL_ARGB32_POS_A;
        increment = 4;
        planeMax = 4;
        break;

      default:
        FOG_ASSERT_NOT_REACHED();
    }

    for (y = 0; y != (uint32_t)_size.h; y++, pixels += stride)
    {
      for (plane = 0; plane < planeMax; plane++)
      {
        if ((err = _PcxDecodeScanline(pixels + pos[plane], &dataCur, dataEnd, (uint32_t)_size.w, ignore, increment)) != ERR_OK) goto _End;
      }
      if (planeMax == 4) _g2d_render.convert.prgb32_from_argb32(pixels, pixels, _size.w, NULL);
      if ((y & 15) == 0) updateProgress(y, (uint32_t)_size.h);
    }
  }

  // --------------------------------------------------------------------------
  // [Read - Palette]
  // --------------------------------------------------------------------------

  if (_depth <= 8)
  {
    uint32_t palData[256];
    uint32_t palLength = 1 << (_depth * _planes);

    bool palRead = true;

    // Setup basic palette settings.
    Memory::zero(palData, 256 * sizeof(uint32_t));

    if (_depth == 1 && _planes == 1)
    {
      _PcxFillMonoPalette(palData);
      if (_pcxFileHeader.version == 2) palRead = false;
    }
    else
    {
      _PcxFillEgaPalette(palData);
    }

    // 256 color palette.
    if (_depth == 8)
    {
      // Find 0x0C marker.
      while (dataCur != dataEnd)
      {
        if (*dataCur++ == 0x0C)
        {
          infoSecPalette = true;
          break;
        }
      }

      if (dataCur == dataEnd)
      {
        // Marker not found, so use greyscale?
        // if (_pcxFileHeader.paletteInfo == 2)
        // {
        _PcxFillGreyPalette(palData);
        palRead = false;
        // }
      }
      else
      {
        size_t n = (size_t)(dataEnd - dataCur) / 3;
        if (n < palLength) palLength = (uint)n;
      }
      infoSecPaletteColors = palLength;
    }
    else if (_pcxFileHeader.version != 3)
    {
      dataCur = _pcxFileHeader.colorMap;
      if (palLength > 16)
      {
        err = ERR_IMAGEIO_UNSUPPORTED_FORMAT;
        goto _End;
      }
    }
    else
    {
      palRead = false;
    }

    // Read primary or secondary palette (from PCX header or end of file)
    if (palRead)
    {
      for (x = 0; x < palLength; x++, dataCur += 3)
      {
        palData[x] = Argb32(0xFF, dataCur[0], dataCur[1], dataCur[2]);
      }
    }

    _palette.setData(Range(0, 256), reinterpret_cast<Argb32*>(palData));
  }

  // apply palette if needed
  if (_depth <= 8) image.setPalette(_palette);

_End:
  image._modified();

  if (err == ERR_OK) updateProgress(1.0);
  return err;
}

// ============================================================================
// [Fog::PcxEncoder - Construction / Destruction]
// ============================================================================

PcxEncoder::PcxEncoder(ImageCodecProvider* provider) :
  ImageEncoder(provider)
{
}

PcxEncoder::~PcxEncoder()
{
}

// ============================================================================
// [Fog::PcxEncoder - WriteImage]
// ============================================================================

err_t PcxEncoder::writeImage(const Image& image)
{
  err_t err = ERR_OK;

  PcxHeader pcx;
  BufferP<1024> rle;

  ImageConverter converter;
  ImageData* d = image._d;

  int w = d->size.w;
  int h = d->size.h;

  uint32_t version = 5;
  uint32_t bitsPerPixel = 8;
  uint32_t nPlanes = 1;
  uint32_t bpl = w;
  uint32_t alignment = bpl & 1; // Align bpl to 16 bits (PCX Specification).
  sysint_t pos[4] = { 0, 0, 0, 0 };

  const uint8_t* pixels = d->first;
  size_t stride = d->stride;

  if (w <= 0 || h <= 0)
  {
    return ERR_IMAGE_INVALID_SIZE;
  }

  switch (d->format)
  {
    case IMAGE_FORMAT_PRGB32:
    case IMAGE_FORMAT_PRGB64:
    case IMAGE_FORMAT_A8:
    case IMAGE_FORMAT_A16:
      converter.create(
        ImageFormatDescription::fromArgb(32, NO_FLAGS, PIXEL_ARGB32_MASK_A, PIXEL_ARGB32_MASK_R, PIXEL_ARGB32_MASK_G, PIXEL_ARGB32_MASK_B),
        ImageFormatDescription::getByFormat(d->format));

      pos[0] = PIXEL_ARGB32_POS_R;
      pos[1] = PIXEL_ARGB32_POS_G;
      pos[2] = PIXEL_ARGB32_POS_B;
      pos[3] = PIXEL_ARGB32_POS_A;
      nPlanes = 4;
      break;

    case IMAGE_FORMAT_RGB24:
      converter.create(
        ImageFormatDescription::getByFormat(IMAGE_FORMAT_RGB24),
        ImageFormatDescription::getByFormat(IMAGE_FORMAT_RGB24));

      pos[0] = PIXEL_RGB24_POS_R;
      pos[1] = PIXEL_RGB24_POS_G;
      pos[2] = PIXEL_RGB24_POS_B;
      nPlanes = 3;
      break;

    case IMAGE_FORMAT_XRGB32:
    case IMAGE_FORMAT_RGB48:
      converter.create(
        ImageFormatDescription::getByFormat(IMAGE_FORMAT_XRGB32),
        ImageFormatDescription::getByFormat(d->format));

      pos[0] = PIXEL_ARGB32_POS_R;
      pos[1] = PIXEL_ARGB32_POS_G;
      pos[2] = PIXEL_ARGB32_POS_B;
      nPlanes = 3;
      break;

    case IMAGE_FORMAT_I8:
      nPlanes = 1;
      break;

    default:
      FOG_ASSERT_NOT_REACHED();
      break;
  }

  Memory::zero((void*)&pcx, sizeof(pcx));
  pcx.manufacturer = 10;
  pcx.version = version;
  pcx.encoding = 1;
  pcx.bitsPerPixel = bitsPerPixel;
  pcx.xMax = (uint)w - 1;
  pcx.yMax = (uint)h - 1;
  pcx.hScreenSize = (uint)w;
  pcx.vScreenSize = (uint)h;
  pcx.nPlanes = (uint)nPlanes;
  pcx.bytesPerLine = (uint)bpl + (uint)alignment;
  // Color or mono?
  pcx.paletteInfo = 1;

  // Try to write comment.
  if (!_comment.isEmpty())
  {
    size_t length = _comment.getLength();
    if (length > 54) length = 54;
    memcpy(pcx.unused, (const char*)_comment.getData(), length);
  }

  _PcxSwapHeader(&pcx);

  // Initialize the RLE buffer and write the PCX header.
  if (!rle.alloc(w * 2 + 2)) return ERR_RT_OUT_OF_MEMORY;

  if (_stream.write((const char *)(&pcx), sizeof(PcxHeader)) != sizeof(PcxHeader)) goto _Fail;

  // --------------------------------------------------------------------------
  // [Write - 8 Bits]
  // --------------------------------------------------------------------------

  if (nPlanes == 1)
  {
    uint8_t palette[768+1];

    for (int y = 0; y != h; y++, pixels += stride)
    {
      if (!_PcxEncodeScanline(_stream, (uint8_t*)rle.getMem(), pixels, bpl, alignment, 1)) goto _Fail;

      if ((y & 15) == 0) updateProgress(y, h);
    }

    // Write the secondary palette at the end of file.
    palette[0] = 0xC;
    _PcxPaletteFromXRGB32(palette + 1, (uint8_t*)image.getPalette().getData(), 256);

    if (_stream.write((const char*)palette, 768+1) != 768+1) goto _Fail;
  }

  // --------------------------------------------------------------------------
  // [Write - 24/32 Bits]
  // --------------------------------------------------------------------------

  else
  {
    uint32_t inc = converter.getDestinationFormatDescription().getBytesPerPixel();

    if (converter.isCopy())
    {
      for (int y = 0; y != h; y++, pixels += stride)
      {
        for (uint plane = 0; plane != nPlanes; plane++)
        {
          if (!_PcxEncodeScanline(_stream, (uint8_t*)rle.getMem(), pixels + pos[plane], bpl, alignment, inc)) goto _Fail;
        }

        if ((y & 15) == 0) updateProgress(y, h);
      }
    }
    else
    {
      BufferP<1024> buffer;
      PointI ditherOrigin(0, 0);

      if (!buffer.alloc(w * inc)) return ERR_RT_OUT_OF_MEMORY;

      for (int y = 0; y != h; y++, pixels += stride, ditherOrigin.y++)
      {
        converter.blitSpan(buffer.getMem(), pixels, w, ditherOrigin);

        for (uint plane = 0; plane != nPlanes; plane++)
        {
          if (!_PcxEncodeScanline(_stream, (uint8_t*)rle.getMem(), (uint8_t*)buffer.getMem() + pos[plane], bpl, alignment, inc)) goto _Fail;
        }

        if ((y & 15) == 0) updateProgress(y, h);
      }
    }
  }

  updateProgress(1.0f);
  return err;

_Fail:
  updateProgress(1.0f);
  return ERR_IO_CANT_WRITE;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void ImageCodecProvider_initPCX(void)
{
  ImageCodecProvider* provider = fog_new PcxCodecProvider();
  ImageCodecProvider::addProvider(provider);
  provider->deref();
}

} // Fog namespace
