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
#include <Fog/Core/Memory.h>
#include <Fog/Core/Misc.h>
#include <Fog/Core/Stream.h>
#include <Fog/Core/String.h>
#include <Fog/Core/Strings.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Image.h>
#include <Fog/Graphics/ImageIO.h>
#include <Fog/Graphics/ImageIO/Pcx_p.h>
#include <Fog/Graphics/RasterEngine_p.h>

namespace Fog {
namespace ImageIO {

// ============================================================================
// [Fog::ImageIO::PcxProvider]
// ============================================================================

PcxProvider::PcxProvider()
{
  // Name of ImageIO Provider.
  _name = fog_strings->getString(STR_GRAPHICS_PCX);

  // File type.
  _fileType = IMAGE_IO_FILE_PCX;

  // Supported devices.
  _deviceType = IMAGE_IO_DEVICE_BOTH;

  // Supported extensions.
  _imageExtensions.reserve(1);
  _imageExtensions.append(fog_strings->getString(STR_GRAPHICS_pcx));
}

PcxProvider::~PcxProvider()
{
}

uint32_t PcxProvider::checkSignature(const void* mem, sysuint_t length) const
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

err_t PcxProvider::createDevice(uint32_t deviceType, BaseDevice** device) const
{
  BaseDevice* d = NULL;

  switch (deviceType)
  {
    case IMAGE_IO_DEVICE_DECODER:
      d = new(std::nothrow) PcxDecoderDevice(const_cast<PcxProvider*>(this));
      break;
    case IMAGE_IO_DEVICE_ENCODER:
      d = new(std::nothrow) PcxEncoderDevice(const_cast<PcxProvider*>(this));
      break;
    default:
      return ERR_RT_INVALID_ARGUMENT;
  }

  if (!d) return ERR_RT_OUT_OF_MEMORY;

  *device = d;
  return ERR_OK;
}

// ============================================================================
// [Fog::ImageIO::PCX utils]
// ============================================================================

static void PCX_byteSwapHeader(PcxHeader* h)
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

// windows did not define strnlen() ?
static uint PCX_strnlen(const uint8_t* str, uint maxlen)
{
  uint n = 0;
  while (n < maxlen && str[n]) n++;
  return n;
}

#define PCX_Strict 1

// decodes scanline compressed by PCX-RLE.
// Returns 0 on success, otherwise error code.
static uint PCX_decodeScanline(
  uint8_t* dest,
  const uint8_t** src, const uint8_t* end, 
  sysuint_t length, sysuint_t ignore, sysuint_t increment)
{
  const uint8_t* cur = *src;
  sysuint_t left = length;
  sysuint_t error = ERR_IMAGEIO_TRUNCATED;

  // Decode RLE scanline and write it to 'dest'
  while (left) 
  {
    if (cur == end) goto fail;
    uint8_t b = *cur++;

    if (b >= 192)
    {
      uint count = b & 0x3F;
      uint extra = 0;

      if (cur == end) goto fail;
      b = *cur++;

      if (left < count) 
      {
        extra = count - left;
        count = left;
      }

      left -= count;

      while (count--) 
      {
        *dest = b;
        dest += increment;
      }

      if (extra)
      {
        // This should happen only if 'ignore' is not zero
        if (extra <= ignore)
        {
          ignore -= extra;
        }
        else
        {
          ignore = 0;
          error = ERR_IMAGEIO_MALFORMED_RLE;

          if (PCX_Strict) goto fail;
        }
      }
    }
    else
    {
      *dest = b;
      dest += increment;
      left--;
    }
  }

  // Decode RLE scanline and discard it
  while (ignore)
  {
    if (cur == end) goto fail;
    uint8_t b = *cur++;

    if (b >= 192)
    {
      uint count = b & 0x3F;

      if (cur == end) goto fail;
      b = *cur++;

      if (ignore < count)
      {
        ignore = 0;
        error = ERR_IMAGEIO_MALFORMED_RLE;
        if (PCX_Strict) goto fail;
      }
      else
        ignore -= count;
    }
    else
      ignore--;
  }

  error = ERR_OK;

fail:
  *src = cur;
  return error;
}

// Encode scanline using PCX_RLE compression
static bool PCX_encodeScanline(
  Stream& stream,
  uint8_t* buffer, const uint8_t* dataCur,
  sysuint_t length, sysuint_t alignment, sysuint_t increment)
{
  uint8_t *bufferPtr = buffer;
  sysuint_t rleCandidate;
  sysuint_t rleCount;

  const uint8_t* dataEnd = dataCur + length * increment;

  do {
    // Get equal pixels
    rleCandidate = (sysuint_t)dataCur[0];
    rleCount = 1;
    dataCur += increment;

    while (dataCur != dataEnd && rleCandidate == (sysuint_t)dataCur[0] && rleCount < 62)
    {
      dataCur += increment;
      rleCount++;
    }

    // Write RLE sequence
    if (rleCount > 1)
    {
      // Multiple pixels (Double-byte code)
      bufferPtr[0] = (uint8_t)(0xC0 | (uint8_t)rleCount);
      bufferPtr[1] = (uint8_t)(rleCandidate);
      bufferPtr += 2;
    }
    else
    {
      // Single pixel
      if (((uint8_t)rleCandidate & 0xC0) != 0xC0)
      {
        // Single-byte code
        *bufferPtr++ = (uint8_t)(rleCandidate);
      }
      else
      {
        // Double-byte code
        bufferPtr[0] = (uint8_t)(0xC1);
        bufferPtr[1] = (uint8_t)(rleCandidate);
        bufferPtr += 2;
      }
    }
  } while (dataCur != dataEnd);

  // Align to 2 bytes.
  //
  // This is in specification, but it's not probabbly used by most applications
  // and libraries.
  if (alignment)
  {
    // If the last bytes are encoded using RLE, we can increment
    // it if it not exceeds 191
    /*if ((sysuint_t)(bufferPtr - buffer) >= 2 &&
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

  // Write compressed line into stream and return
  length = (sysuint_t)(bufferPtr - buffer);
  return stream.write((const char *)buffer, length) == length;
}

static void PCX_fillMonoPalette(uint32_t* palette)
{
  palette[0] = 0xFF000000;
  palette[1] = 0xFFFFFFFF;
}

static void PCX_fillGreyPalette(uint32_t* palette)
{
  uint32_t c = 0xFF000000, i;
  for (i = 256; i; i--, c += 0x00010101, palette++) palette[0] = c;
}

static void PCX_fillEgaPalette(uint32_t* palette)
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

static void PCX_applyPalette(uint8_t* pixels, sysuint_t count, const uint32_t* palette)
{
  ulong i;

  for (i = count; i; i--, pixels += 4)
  {
    ((uint32_t *)pixels)[0] = palette[pixels[0]];
  }
}

static void PCX_convertPaletteToPcx(uint8_t* dest, const uint8_t* src, sysuint_t count, sysuint_t entrySize)
{
  for (sysuint_t i = 0; i != count; i++, dest += 3, src += entrySize)
  {
    uint32_t c = *(uint32_t *)src;
    dest[0] = (uint8_t)( (c & 0x00FF0000) >> 16 );
    dest[1] = (uint8_t)( (c & 0x0000FF00) >>  8 );
    dest[2] = (uint8_t)( (c & 0x000000FF)       );
  }
}

// ============================================================================
// [Fog::ImageIO::PcxDecoderDevice]
// ============================================================================

PcxDecoderDevice::PcxDecoderDevice(Provider* provider) :
  DecoderDevice(provider)
{
  zeroall();
}

PcxDecoderDevice::~PcxDecoderDevice()
{
}

// ============================================================================
// [Fog::ImageIO::PcxDecoderDevice::reset]
// ============================================================================

void PcxDecoderDevice::reset()
{
  DecoderDevice::reset();
  zeroall();
}

// ============================================================================
// [Fog::ImageIO::PcxDecoderDevice::readHeader]
// ============================================================================

err_t PcxDecoderDevice::readHeader()
{
  // Don't read header more than once.
  if (isHeaderDone()) return getHeaderResult();

  // Mark header as done.
  _headerDone = true;

  // Read pcx header.
  if (_stream.read(&_pcxFileHeader, sizeof(PcxHeader)) != sizeof(PcxHeader))
  {
    return ERR_IMAGEIO_TRUNCATED;
  }

  // Check for correct mime.
  if (/*pcxFileHeader().manufacturer != 10 ||*/
    pcxFileHeader().version != 5 ||
    pcxFileHeader().encoding != 1)
  {
    return ERR_IMAGEIO_MIME_NOT_MATCH;
  }

  // Byteswap header.
  PCX_byteSwapHeader(&_pcxFileHeader);

  int16_t xMin = pcxFileHeader().xMin;
  int16_t yMin = pcxFileHeader().yMin;
  int16_t xMax = pcxFileHeader().xMax;
  int16_t yMax = pcxFileHeader().yMax;

  // Size reject.
  if (xMin > xMax || yMin > yMax) return ERR_IMAGE_INVALID_SIZE;

  // Decode header.
  _width  = (uint)(xMax - xMin) + 1;
  _height = (uint)(yMax - yMin) + 1;
  _depth  = (uint32_t)(pcxFileHeader().bitsPerPixel);
  _planes = (uint32_t)(pcxFileHeader().nPlanes);

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
      if ((getPlanes() == 1 ||
         getPlanes() == 3 ||
         getPlanes() == 4) &&
        pcxFileHeader().version >= 4) break;
      // Go through.
    default:
      return (_headerResult = ERR_IMAGEIO_UNSUPPORTED_FORMAT);
  }

  _format = PIXEL_FORMAT_I8;

  // Standardize output for Fog::ImageIO.
  if (getDepth() == 8 && getPlanes() == 4)
  {
    _depth = 32;
    _planes = 1;
    _format = PIXEL_FORMAT_ARGB32;
  }
  else if (getDepth() == 8 && getPlanes() == 3)
  {
    _depth = 24;
    _planes = 1;
    _format = PIXEL_FORMAT_XRGB32;
  }

  // Success.
  return (_headerResult = ERR_OK);
}

// ============================================================================
// [Fog::ImageIO::PcxDecoderDevice::readImage]
// ============================================================================

err_t PcxDecoderDevice::readImage(Image& image)
{
  // read bmp header
  if (readHeader() != ERR_OK) return getHeaderResult();

  // don't read image more than once
  if (isReaderDone()) return (_readerResult = ERR_IMAGEIO_NO_MORE_FRAMES);

  // error code (default is success)
  uint32_t error = ERR_OK;

  // Source
  ByteArray dataArray;
  const uint8_t* dataCur;
  const uint8_t* dataEnd;

  // Destination
  uint8_t *pixelsCur;

  // Temporary plane data
  LocalBuffer<1024> temporary;
  uint8_t* mem;

  // Bytes per line
  sysuint_t bytesPerLine = (sysuint_t)pcxFileHeader().bytesPerLine;

  // Loop variables
  uint x, y;

  // Secondary palette
  bool info_secPalette = false;
  uint info_secPaletteColors = 0;

  // Image text
  if (pcxFileHeader().unused[0])
  {
    const uint8_t* str = (const uint8_t*)pcxFileHeader().unused;
    _comment.set(Ascii8(str, PCX_strnlen(str, 54)));
  }
  else
    _comment.free();

  _stream.readAll(dataArray);
  dataCur = (const uint8_t*)dataArray.getData();
  dataEnd = dataCur + dataArray.getLength();

  if ((error = image.create(getWidth(), getHeight(), getFormat()))) goto end;

  // BitsPP: 1
  // Planes: 1, 2, 3, 4
  if (_depth == 1)
  {
    sysuint_t plane;
    uint8_t b;

    if (!temporary.alloc(bytesPerLine))
    {
      error = ERR_RT_OUT_OF_MEMORY;
      goto end;
    }

    for (y = 0; y != _height; y++)
    {
      // Expand planes to 8 BPP
      for (plane = 0; plane != _planes; plane++)
      {
        pixelsCur = image.getXScanline(y);
        mem = (uint8_t*)temporary.mem();

        if ((error = PCX_decodeScanline(mem, &dataCur, dataEnd, bytesPerLine, 0, 1)) != ERR_OK) goto end;

        for (x = 0; x != _width; x++, pixelsCur++)
        {
          if ((x & 7) == 0) 
            b = *mem++;
          else
            b <<= 1;

          pixelsCur[0] |= ((b >> 7) & 1) << plane;
        }
      }
      if ((y & 15) == 0) updateProgress(y, _height);
    }
  }

  // BitsPP: 4
  // Planes: 1
  else if (_depth == 4)
  {
    if (!temporary.alloc(bytesPerLine))
    {
      error = ERR_RT_OUT_OF_MEMORY;
      goto end;
    }

    for (y = 0; y != _height; y++)
    {
      pixelsCur = image.getXScanline(y);
      mem = (uint8_t*)temporary.mem();

      if ((error = PCX_decodeScanline(mem, &dataCur, dataEnd, bytesPerLine, 0, 1)) != ERR_OK) goto end;

      for (x = 0; x != _width; x++, pixelsCur++)
      {
        pixelsCur[0] = (!(x & 1)) ? ((*mem & 0xF0) >> 4) : (*mem++ & 0xF);
      }

      if ((y & 15) == 0) updateProgress(y, _height);
    }
  }

  // BitsPP: 8
  // Planes: 1, 3, 4
  else
  {
    sysuint_t pos[4];
    sysuint_t ignore = bytesPerLine - _width;
    sysuint_t increment;
    sysuint_t plane;
    sysuint_t planeMax = 1;

    switch (_depth)
    {
      case 8:
        pos[0] = 0;
        increment = 1;
        break;
      case 24:
        pos[0] = ARGB32_RBYTE;
        pos[1] = ARGB32_GBYTE;
        pos[2] = ARGB32_BBYTE;
        increment = 3;
        planeMax = 3;
      case 32:
        pos[0] = ARGB32_RBYTE;
        pos[1] = ARGB32_GBYTE;
        pos[2] = ARGB32_BBYTE;
        pos[3] = ARGB32_ABYTE;
        increment = 4;
        planeMax = 4;
        break;
      default:
        FOG_ASSERT_NOT_REACHED();
    }

    for (y = 0; y != _height; y++)
    {
      pixelsCur = image.getXScanline(y);
      for (plane = 0; plane < planeMax; plane++)
      {
        if ((error = PCX_decodeScanline(pixelsCur + pos[plane], &dataCur, dataEnd, _width, ignore, increment)) != ERR_OK) goto end;
      }

      if ((y & 15) == 0) updateProgress(y, _height);
    }
  }

  // Read palette
  if (_depth <= 8)
  {
    uint32_t pal[256];
    uint32_t* pdest = pal;

    uint nColors = 1 << (_depth * _planes);
    bool read = true;

    // Setup basic palette settings.
    Memory::zero(pdest, 256 * sizeof(uint32_t));

    if (_depth == 1 && _planes == 1)
    {
      PCX_fillMonoPalette(pdest);
      if (pcxFileHeader().version == 2) read = false;
    }
    else
    {
      PCX_fillEgaPalette(pdest);
    }

    // 256 color palette.
    if (_depth == 8)
    {
      // Find 0x0C marker.
      while (dataCur != dataEnd)
      {
        if (*dataCur++ == 0x0C) 
        {
          info_secPalette = true;
          break;
        }
      }

      if (dataCur == dataEnd)
      {
        // Marker not found, so use greyscale ?
        // if (pcxFileHeader().paletteInfo == 2)
        // {
        PCX_fillGreyPalette(pdest);
        read = false;
        // }
      }
      else
      {
        sysuint_t n = (sysuint_t)(dataEnd - dataCur) / 3;
        if (n < nColors) nColors = (uint)n;
      }
      info_secPaletteColors = nColors;
    }
    else if (pcxFileHeader().version != 3)
    {
      dataCur = pcxFileHeader().colorMap;
      if (nColors > 16)
      {
        error = ERR_IMAGEIO_UNSUPPORTED_FORMAT;
        goto end;
      }
    }
    else
    {
      read = false;
    }

    // Read primary or secondary palette (from PCX header or end of file)
    if (read)
    {
      for (x = 0; x < nColors; x++) 
      {
        pdest[x] = Argb(0xFF, dataCur[0], dataCur[1], dataCur[2]);
        dataCur += 3;
      }
    }

    _palette.setXrgb32(0, (Argb*)pal, 256);
  }

  // apply palette if needed
  if (_depth <= 8) image.setPalette(_palette);

end:
  if (error == ERR_OK) updateProgress(1.0);
  return error;
}

// ============================================================================
// [Fog::ImageIO::PcxEncoder]
// ============================================================================

PcxEncoderDevice::PcxEncoderDevice(Provider* provider) :
  EncoderDevice(provider)
{
}

PcxEncoderDevice::~PcxEncoderDevice()
{
}

err_t PcxEncoderDevice::writeImage(const Image& image)
{
  PcxHeader pcx;
  Memory::zero((void*)&pcx, sizeof(pcx));

  err_t err = ERR_OK;
  int width = image.getWidth();
  int height = image.getHeight();
  int format = image.getFormat();
  int version;
  int bitsPerPixel;
  int nPlanes;
  int bpl;
  int alignment;
  int y;

  LocalBuffer<1024> rleenc;

  const uint8_t* pixels;

  if (!width || !height) return ERR_IMAGE_INVALID_SIZE;

  version = 5;
  bitsPerPixel = 8;
  nPlanes = 1;
  bpl = width;

  // PCX not supports alpha channel in palette so we must use ARGB32 format if
  // palette alpha channel is used.
  if (image.getPalette().isAlphaUsed()) format = PIXEL_FORMAT_ARGB32;

  switch (format)
  {
    case PIXEL_FORMAT_ARGB32:
    case PIXEL_FORMAT_PRGB32:
      nPlanes = 4;
      break;
    case PIXEL_FORMAT_XRGB32:
      nPlanes = 3;
      break;
    case PIXEL_FORMAT_A8:
    case PIXEL_FORMAT_I8:
      nPlanes = 1;
      break;
    default:
      FOG_ASSERT_NOT_REACHED();
      break;
  }

  // Align bpl to 16 bits.
  alignment = bpl & 1; 

  pcx.manufacturer = 10;
  pcx.version = version;
  pcx.encoding = 1;
  pcx.bitsPerPixel = bitsPerPixel;
  pcx.xMax = (uint)width - 1;
  pcx.yMax = (uint)height - 1;
  pcx.hScreenSize = (uint)width;
  pcx.vScreenSize = (uint)height;
  pcx.nPlanes = (uint)nPlanes;
  pcx.bytesPerLine = (uint)bpl + (uint)alignment;
  // color or mono.
  pcx.paletteInfo = 1;

  // Try to write comment.
  if (!_comment.isEmpty())
  {
    sysuint_t length = _comment.getLength();
    if (length > 54) length = 54;
    memcpy(pcx.unused, (const char*)_comment.getData(), length);
  }

  PCX_byteSwapHeader(&pcx);

  // Write pcx header.
  if (_stream.write((const char *)(&pcx), sizeof(PcxHeader)) != sizeof(PcxHeader)) goto fail;

  // Alloc buffers.
  if (!rleenc.alloc((width << 1) + 2)) return ERR_RT_OUT_OF_MEMORY;

  // Write 8 bit image.
  if (image.getDepth() == 8 && format != PIXEL_FORMAT_ARGB32)
  {
    for (y = 0; y != height; y++)
    {
      pixels = image.getScanline(y);
      if (!PCX_encodeScanline(_stream, (uint8_t*)rleenc.mem(), pixels, bpl, alignment, 1)) goto fail;

      if ((y & 15) == 0) updateProgress(y, height);
    }

    // Write secondary palette.
    {
      uint8_t palette[768+1];
      palette[0] = 0xC;

      // A8 will be saved as greyscale.
      if (format == PIXEL_FORMAT_A8)
      {
        uint8_t* p = palette;
        for (y = 0; y < 256; y++, p += 3) { p[0] = p[1] = p[2] = (uint8_t)y; }
      }
      // Standard indexed image.
      else
      {
        PCX_convertPaletteToPcx(palette + 1, (uint8_t*)image.getPalette().getData(), 256, sizeof(Argb));
      }

      if (_stream.write((const char*)palette, 768+1) != 768+1) goto fail;
    }
  }
  // Write 8 bit image as ARGB32 image (alpha channel in palette used).
  else if (image.getDepth() == 8 && format == PIXEL_FORMAT_ARGB32)
  {
    LocalBuffer<1024> bufferLocal;

    uint8_t* buffer = reinterpret_cast<uint8_t*>(bufferLocal.alloc(width * 4));
    if (!buffer) { err = ERR_RT_OUT_OF_MEMORY; goto end; }

    sysint_t pos[4];
    sysint_t plane;
    sysint_t increment = 4;

    pos[0] = ARGB32_RBYTE;
    pos[1] = ARGB32_GBYTE;
    pos[2] = ARGB32_BBYTE;
    pos[3] = ARGB32_ABYTE;

    for (y = 0; y != height; y++)
    {
      image.getDib(0, y, width, DIB_FORMAT_ARGB32_NATIVE, buffer);
      for (plane = 0; plane != nPlanes; plane++)
      {
        if (!PCX_encodeScanline(_stream, (uint8_t*)rleenc.mem(), buffer + pos[plane], bpl, alignment, increment)) goto fail;
      }
      if ((y & 15) == 0) updateProgress(y, height);
    }
  }
  // Write 24 or 32 bit RGB image.
  else
  {
    sysint_t pos[4];
    sysint_t plane;
    sysint_t increment = image.getBytesPerPixel();

    pos[0] = ARGB32_RBYTE;
    pos[1] = ARGB32_GBYTE;
    pos[2] = ARGB32_BBYTE;
    pos[3] = ARGB32_ABYTE;

    for (y = 0; y != height; y++)
    {
      pixels = image.getScanline(y);
      for (plane = 0; plane != nPlanes; plane++)
      {
        if (!PCX_encodeScanline(_stream, (uint8_t*)rleenc.mem(), pixels + pos[plane], bpl, alignment, increment)) goto fail;
      }
      if ((y & 15) == 0) updateProgress(y, height);
    }
  }

end:
  updateProgress(1.0);
  return err;
fail:
  updateProgress(1.0);
  return ERR_IO_CANT_WRITE;
}

} // ImageIO namespace
} // Fog namespace

FOG_IMPLEMENT_OBJECT(Fog::ImageIO::PcxDecoderDevice)
FOG_IMPLEMENT_OBJECT(Fog::ImageIO::PcxEncoderDevice)

// ============================================================================
// [CAPI]
// ============================================================================

FOG_INIT_DECLARE void fog_imageio_init_pcx(void)
{
  using namespace Fog;
  ImageIO::addProvider(IMAGE_IO_DEVICE_BOTH, new(std::nothrow) ImageIO::PcxProvider());
}
