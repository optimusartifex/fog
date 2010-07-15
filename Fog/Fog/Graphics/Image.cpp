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
#include <Fog/Core/FileSystem.h>
#include <Fog/Core/MapFile.h>
#include <Fog/Core/Math.h>
#include <Fog/Core/Memory.h>
#include <Fog/Core/Misc.h>
#include <Fog/Core/Std.h>
#include <Fog/Graphics/ColorLut.h>
#include <Fog/Graphics/ColorMatrix.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Image.h>
#include <Fog/Graphics/ImageContext.h>
#include <Fog/Graphics/ImageFilter.h>
#include <Fog/Graphics/ImageIO.h>
#include <Fog/Graphics/ImagePixels.h>
#include <Fog/Graphics/RasterEngine_p.h>
#include <Fog/Graphics/RasterEngine/Bresenham_p.h>
#include <Fog/Graphics/RasterEngine/C_p.h>
#include <Fog/Graphics/Rasterizer_p.h>
#include <Fog/Graphics/Reduce_p.h>
#include <Fog/Graphics/Scanline_p.h>

#if defined(FOG_OS_WINDOWS)
#include <Fog/Graphics/ImageData_WinDib_p.h>
#endif // FOG_OS_WINDOWS

namespace Fog {

// ============================================================================
// [Helpers]
// ============================================================================

static FOG_INLINE bool ImageData_containsEmbeddedBuffer(const ImageData* d)
{
  return d->data == d->buffer;
}

// ============================================================================
// [Fog::ImageData]
// ============================================================================

ImageData::ImageData()
{
  refCount.init(1);

  width = 0;
  height = 0;
  locked = 0;

  type = IMAGE_TYPE_MEMORY;
  flags = IMAGE_DATA_FLAG_DYNAMIC | IMAGE_DATA_FLAG_SHARABLE;
  format = IMAGE_FORMAT_NULL;
  bytesPerPixel = 0;

  data = NULL;
  first = NULL;
  stride = 0;
}

ImageData::~ImageData()
{
}

ImageData* ImageData::ref() const
{
  if (flags & IMAGE_DATA_FLAG_SHARABLE)
    return refAlways();
  else
    return clone();
}

void ImageData::deref()
{
  if (refCount.deref())
  {
    bool wasDynamic = (flags & IMAGE_DATA_FLAG_DYNAMIC) != 0;
    this->~ImageData();
    if (wasDynamic) Memory::free(this);
  }
}

ImageData* ImageData::clone() const
{
  ImageData* newd;

  if (this->width && this->height)
  {
    newd = Image::_allocData(this->width, this->height, this->format);
    if (!newd) return NULL;

    rasterFuncs.dib.vblit_rect[newd->format](
      newd->first, newd->stride,
      this->first, this->stride,
      newd->width, newd->height, NULL);
    newd->palette = this->palette;
  }
  else
  {
    newd = Image::_dnull->refAlways();
  }

  return newd;
}

err_t ImageData::create(int w, int h, uint32_t format)
{
  return ERR_RT_INVALID_OBJECT;
}

err_t ImageData::destroy()
{
  return ERR_RT_INVALID_OBJECT;
}

void* ImageData::getHandle()
{
  return NULL;
}

// ============================================================================
// [Fog::Image]
// ============================================================================

Static<ImageData> Image::_dnull;

Image::Image() : 
  _d(_dnull->refAlways())
{
}

Image::Image(const Image& other) : 
  _d(other._d->ref())
{
}

Image::Image(int w, int h, uint32_t format, uint32_t type) :
  _d(_dnull->refAlways())
{
  create(w, h, format, type);
}

Image::~Image()
{
  _d->deref();
}

err_t Image::_detach()
{
  ImageData* d = _d;
  if (d == _dnull.instancep()) return ERR_OK;

  if (d->stride == 0)
  {
    atomicPtrXchg(&_d, _dnull->refAlways())->deref();
    return ERR_OK;
  }

  if (d->refCount.get() > 1 || (d->flags & IMAGE_DATA_FLAG_READ_ONLY))
  {
    ImageData* newd = d->clone();
    if (!newd) return ERR_RT_OUT_OF_MEMORY;

    atomicPtrXchg(&_d, newd)->deref();
  }

  return ERR_OK;
}

void Image::free()
{
  atomicPtrXchg(&_d, _dnull.instancep()->refAlways())->deref();
}

err_t Image::create(int w, int h, uint32_t format, uint32_t type)
{
  ImageData* d = _d;

  // Detect invalid arguments.
  if (format >= IMAGE_FORMAT_COUNT)
  {
    free();
    return ERR_RT_INVALID_ARGUMENT;
  }

  // Don't create image with invalid size.
  if (w <= 0 || h <= 0 || w >= IMAGE_MAX_WIDTH || h >= IMAGE_MAX_HEIGHT)
  {
    free();
    return ERR_IMAGE_INVALID_SIZE;
  }

  // If the size of this image is the same, and all other members passes
  // the @c Image::create() arguments then we can simply return.
  if (d->width  == w         && d->height == h    &&
      d->format == format    && (d->type == type || type == IMAGE_TYPE_IGNORE) &&
      d->refCount.get() == 1 && !(d->flags & IMAGE_DATA_FLAG_READ_ONLY))
  {
    return ERR_OK;
  }

  err_t err = ERR_OK;
  ImageData* newd = NULL;

  switch (type)
  {
    case IMAGE_TYPE_MEMORY:
    case IMAGE_TYPE_IGNORE:
      // Create new memory image (the default).
      newd = Image::_allocData(w, h, format);
      if (newd == NULL)
      {
        err = ERR_RT_OUT_OF_MEMORY;
      }
      break;

#if defined(FOG_OS_WINDOWS)
    case IMAGE_TYPE_WIN_DIB:
      newd = WinDibImageData::_createInstance();
      if (newd == NULL)
      {
        err = ERR_RT_OUT_OF_MEMORY;
      }
      else if ((err = newd->create(w, h, format)) != ERR_OK)
      {
        newd->deref();
        newd = NULL;
      }
      break;
#endif // FOG_OS_WINDOWS

    default:
      err = ERR_RT_INVALID_ARGUMENT;
  }

  if (!newd)
  {
    free();
    return err;
  }

  atomicPtrXchg(&_d, newd)->deref();
  return err;
}

err_t Image::adopt(const ImageBuffer& buffer, uint32_t adoptFlags)
{
  ImageData* d = _d;

  if (!buffer.isValid())
  {
    free();
    return ERR_RT_INVALID_ARGUMENT;
  }

  if ((d->refCount.get() > 1) ||
     ((d->flags & IMAGE_DATA_FLAG_DYNAMIC) && !ImageData_containsEmbeddedBuffer(d)))
  {
    ImageData* newd = _allocData(0);
    if (!newd)
    {
      free();
      return ERR_RT_OUT_OF_MEMORY;
    }

    atomicPtrXchg(&_d, newd)->deref();
    d = newd;
  }

  // Fill basic variables
  d->width = buffer.width;
  d->height = buffer.height;
  d->format = (uint8_t)buffer.format;

  d->data = (uint8_t*)buffer.data;
  d->first = (uint8_t*)buffer.data;
  d->stride = buffer.stride;

  // Bottom-to-top data?
  if (adoptFlags & IMAGE_ADOPT_REVERSED)
  {
    d->stride = -d->stride;
    d->first += (d->height - 1) * d->stride;
  }

  // Read only memory ?
  if ((adoptFlags & IMAGE_ATOPT_READ_ONLY) != 0)
    d->flags |= IMAGE_DATA_FLAG_READ_ONLY;
  else
    d->flags &= ~IMAGE_DATA_FLAG_READ_ONLY;

  return ERR_OK;
}

err_t Image::set(const Image& other)
{
  if (_d == other._d) return ERR_OK;

  if (other.isEmpty())
  {
    atomicPtrXchg(&_d, _dnull->refAlways())->deref();
    return ERR_OK;
  }

  if (isKeepAlive() || !other.isSharable())
  {
    return setDeep(other);
  }
  else
  {
    atomicPtrXchg(&_d, other._d->refAlways())->deref();
    return ERR_OK;
  }
}

err_t Image::set(const Image& other, const IntRect& area)
{
  if (other.isEmpty())
  {
    free();
    return ERR_OK;
  }

  int x1 = area.x;
  int y1 = area.y;
  int x2 = x1 + area.w;
  int y2 = y1 + area.h;

  if (x1 < 0) x1 = 0;
  if (y1 < 0) y1 = 0;
  if (x2 > other.getWidth()) x2 = other.getWidth();
  if (y2 > other.getHeight()) y2 = other.getHeight();

  if (x1 >= x2 || y1 >= y2)
    return ERR_RT_INVALID_ARGUMENT;
  if (x1 == 0 && y1 == 0 && x2 == getWidth() && y2 == getHeight()) 
    return set(other);

  int w = x2 - x1;
  int h = y2 - y1;

  Static<Image> selfCopy;
  bool selfBlit = (_d == other._d);

  if (selfBlit) selfCopy.init(other);

  FOG_RETURN_ON_ERROR(create(w, h, other.getFormat()));

  uint8_t* dstCur = _d->first;
  uint8_t* srcCur = other._d->first;

  sysint_t dstStride = _d->stride;
  sysint_t srcStride = other._d->stride;

  srcCur += (uint)y1 * srcStride + (uint)x1 * other.getBytesPerPixel();
  sysuint_t bpl = w * other.getBytesPerPixel();

  for (sysint_t i = 0; i < h; i++, dstCur += dstStride, srcCur += srcStride)
  {
    Memory::copy(dstCur, srcCur, bpl);
  }

  if (selfBlit) selfCopy.destroy();
  return ERR_OK;
}

err_t Image::setDeep(const Image& other)
{
  if (_d == other._d) return ERR_OK;

  if (other.isEmpty())
  {
    atomicPtrXchg(&_d, _dnull->refAlways())->deref();
    return ERR_OK;
  }

  if (getWidth() == other.getWidth() &&
      getHeight() == other.getHeight() &&
      getBytesPerPixel() == other.getBytesPerPixel())
  {
    const ImageData* _o = other._d;
    rasterFuncs.dib.vblit_rect[_d->format](
      _d->first, _d->stride,
      _o->first, _o->stride,
      _d->width, _d->height, NULL);

    _d->format = _o->format;
    _d->palette = other._d->palette;
    return ERR_OK;
  }

  ImageData* newd = other._d->clone();
  if (!newd) return ERR_RT_OUT_OF_MEMORY;

  atomicPtrXchg(&_d, newd)->deref();
  return ERR_OK;
}

err_t Image::convert(uint32_t format)
{
  ImageData* d = _d;

  uint32_t sourceFormat = _d->format;
  uint32_t targetFormat = format;

  if (targetFormat >= IMAGE_FORMAT_COUNT)
    return ERR_RT_INVALID_ARGUMENT;

  if (sourceFormat == targetFormat) return ERR_OK;

  int w = d->width;
  int h = d->height;
  int y;

  const RasterFuncs::CompositeFuncs* funcs = rasterFuncs.getCompositeFuncs(OPERATOR_SRC, targetFormat);
  RasterVBlitFullFn vblit_full = funcs->vblit_full[sourceFormat];

  RasterClosure closure;
  closure.srcPalette = reinterpret_cast<const uint32_t*>(_d->palette.getData());
  closure.dstPalette = NULL;

  // Special cases
  if (targetFormat == IMAGE_FORMAT_I8) return to8Bit();

  switch ((sourceFormat << 16) | (targetFormat))
  {
    case (IMAGE_FORMAT_XRGB32 << 16) | (IMAGE_FORMAT_A8):
      vblit_full = rasterFuncs.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_GREY8];
      break;
    case (IMAGE_FORMAT_A8 << 16) | (IMAGE_FORMAT_XRGB32):
      vblit_full = rasterFuncs.dib.convert[DIB_FORMAT_GREY8][IMAGE_FORMAT_XRGB32];
      break;
  }

  // We can optimize converting if we can do conversion to current image data.
  if (getDepth() == getDepthFromFormat(format) && isDetached() && !isReadOnly())
  {
    uint8_t* dstCur = _d->first;
    sysint_t dstStride = _d->stride;

    for (y = 0; y < h; y++, dstCur += dstStride)
    {
      vblit_full(dstCur, dstCur, w, &closure);
    }

    _d->format = targetFormat;
    return ERR_OK;
  }
  else
  {
    ImageData* newd = _allocData(w, h, targetFormat);
    if (!newd) return ERR_RT_OUT_OF_MEMORY;

    uint8_t* dstCur = newd->first;
    uint8_t* srcCur = _d->first;

    sysint_t dstStride = newd->stride;
    sysint_t srcStride = _d->stride;

    for (y = 0; y < h; y++, dstCur += dstStride, srcCur += srcStride)
    {
      vblit_full(dstCur, srcCur, w, &closure);
    }

    atomicPtrXchg(&_d, newd)->deref();
    return ERR_OK;
  }
}

err_t Image::to8Bit()
{
  err_t err;
  if (isEmpty()) return ERR_OK;

  switch (getFormat())
  {
    case IMAGE_FORMAT_PRGB32:
    case IMAGE_FORMAT_ARGB32:
    case IMAGE_FORMAT_XRGB32:
      // These formats will be processed.
      break;
    case IMAGE_FORMAT_A8:
      return forceFormat(IMAGE_FORMAT_I8);
    case IMAGE_FORMAT_I8:
      // This is format we want. Nothing to do.
      return ERR_OK;
    default:
      FOG_ASSERT_NOT_REACHED();
      return ERR_RT_ASSERTION_FAILURE;
  }

  Reduce reducer;

  int w = getWidth();
  int h = getHeight();

  Image i;
  if ( (err = i.create(w, h, IMAGE_FORMAT_I8)) ) return err;

  uint8_t* dstBase = i._d->first;
  uint8_t* srcBase = _d->first;

  sysint_t dstStride = i._d->stride;
  sysint_t srcStride = _d->stride;

  int y;

  if (reducer.analyze(*this, true))
  {
    if ( (err = i.setPalette(reducer.toPalette())) ) return err;

    for (y = 0; y < h; y++, dstBase += dstStride, srcBase += srcStride)
    {
      uint8_t* dstCur = dstBase;
      uint8_t* srcCur = srcBase;

      switch (getFormat())
      {
        case IMAGE_FORMAT_ARGB32:
        case IMAGE_FORMAT_PRGB32:
          for (int x = 0; x < w; x++, dstCur += 1, srcCur += 4)
          {
            dstCur[0] = reducer.traslate(
              ((uint32_t*)srcCur)[0] | ARGB32_AMASK);
          }
          break;
      }
    }
  }
  else
  {
    Palette pal = Palette::colorCube(4, 8, 4);
    if ( (err = i.setPalette(pal)) ) return err;

    uint8_t palConv[256];
    for (y = 0; y < 256; y++) palConv[y] = y;

    RasterDither8Fn converter = NULL;

    switch (getFormat())
    {
      case IMAGE_FORMAT_ARGB32:
      case IMAGE_FORMAT_PRGB32:
      case IMAGE_FORMAT_XRGB32:
        converter = rasterFuncs.dib.i8rgb232_from_xrgb32_dither;
        break;
      default:
        FOG_ASSERT_NOT_REACHED();
        return ERR_RT_ASSERTION_FAILURE;
    }

    for (y = 0; y < h; y++, dstBase += dstStride, srcBase += srcStride)
    {
      converter(dstBase, srcBase, w, IntPoint(0, y), palConv);
    }
  }

  return set(i);
}

err_t Image::to8Bit(const Palette& pal)
{
  if (isEmpty()) return ERR_OK;

  err_t err;

  int w = getWidth();
  int h = getHeight();

  Image i;
  if ( (err = i.create(w, h, IMAGE_FORMAT_I8)) ) return err;
  if ( (err = i.setPalette(pal)) ) return err;

  uint8_t* dstBase = i._d->first;
  uint8_t* srcBase = _d->first;

  sysint_t dstStride = i._d->stride;
  sysint_t srcStride = _d->stride;

  int y, x;

  switch (getFormat())
  {
    case IMAGE_FORMAT_ARGB32:
    case IMAGE_FORMAT_PRGB32:
    case IMAGE_FORMAT_XRGB32:
    {
      for (y = 0; y < h; y++, dstBase += dstStride, srcBase += srcStride)
      {
        uint8_t* dstCur = dstBase;
        uint8_t* srcCur = srcBase;
        for (x = 0; x < w; x++, dstCur += 1, srcCur += 4)
        {
          Argb c = ((uint32_t*)srcCur)[0];
          dstCur[0] = pal.findColor(c.getRed(), c.getGreen(), c.getBlue());
        }
      }
      break;
    }
    case IMAGE_FORMAT_I8:
    case IMAGE_FORMAT_A8:
    {
      // Build lookup table.
      uint8_t table[256];

      if (getFormat() == IMAGE_FORMAT_I8)
      {
        for (y = 0; y < 256; y++)
        {
          Argb c = _d->palette.at(y);
          table[y] = pal.findColor(c.getRed(), c.getGreen(), c.getBlue());
        }
      }
      else
      {
        for (y = 0; y < 256; y++)
        {
          uint8_t c = _d->palette.at(y).getGrey();
          table[y] = pal.findColor(c, c, c);
        }
      }

      for (y = 0; y < h; y++, dstBase += dstStride, srcBase += srcStride)
      {
        uint8_t* dstCur = dstBase;
        uint8_t* srcCur = srcBase;
        for (x = 0; x < w; x++, dstCur += 1, srcCur += 1)
        {
          dstCur[0] = table[srcCur[0]];
        }
      }
      break;

    }

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  return set(i);
}

err_t Image::forceFormat(uint32_t format)
{
  if (format >= (uint)IMAGE_FORMAT_COUNT)
    return ERR_RT_INVALID_ARGUMENT;

  if (getDepth() != getDepthFromFormat(format))
    return ERR_RT_INVALID_ARGUMENT;
  
  if (_d->format == format)
    return ERR_OK;

  FOG_RETURN_ON_ERROR(detach());

  _d->format = format;
  return ERR_OK;
}

err_t Image::setPalette(const Palette& palette)
{
  FOG_RETURN_ON_ERROR(detach());

  _d->palette = palette;
  return ERR_OK;
}

err_t Image::setPalette(sysuint_t index, const Argb* rgba, sysuint_t count)
{
  if (count == 0) return ERR_OK;

  FOG_RETURN_ON_ERROR(detach());

  return _d->palette.setArgb32(index, rgba, count);
}

















































// ============================================================================
// [Lock / Unlock Pixels]
// ============================================================================

err_t Image::lockPixels(ImagePixels& pixels, uint32_t lockMode)
{
  if (FOG_UNLIKELY(pixels._data != NULL))
    return ERR_RT_INVALID_STATE;

  if (FOG_UNLIKELY(isEmpty()))
    return ERR_IMAGE_INVALID_SIZE;

  if (!isDetached())
  {
    FOG_RETURN_ON_ERROR((lockMode == IMAGE_LOCK_OVERWRITE)
      ? create(_d->width, _d->height, _d->format)
      : _detach());
  }

  ImageData* d = _d;
  d->locked++;

  pixels._size.set(d->width, d->height);
  pixels._format = d->format;
  pixels._flags = 0;
  pixels._stride = d->stride;
  pixels._data = d->first;
  pixels._relatedImage->_d = d;

  return ERR_OK;
}

err_t Image::lockPixels(ImagePixels& pixels, const IntRect& rect, uint32_t lockMode)
{
  if (rect.x == 0 && rect.y == 0 && rect.w == getWidth() && rect.h == getHeight())
    return lockPixels(pixels, lockMode);

  if (FOG_UNLIKELY(pixels._data != NULL))
    return ERR_RT_INVALID_STATE;

  if (FOG_UNLIKELY(isEmpty()))
    return ERR_IMAGE_INVALID_SIZE;

  if (!isDetached())
  {
    FOG_RETURN_ON_ERROR((lockMode == IMAGE_LOCK_OVERWRITE)
      ? create(_d->width, _d->height, _d->format)
      : _detach());
  }

  ImageData* d = _d;
  d->locked++;

  pixels._size.set(d->width, d->height);
  pixels._format = d->format;
  pixels._flags = 0;
  pixels._stride = d->stride;
  pixels._data = d->first;
  pixels._relatedImage->_d = d;

  pixels._ready();
  return ERR_OK;
}

err_t Image::unlockPixels(ImagePixels& pixels)
{
  ImageData* d = _d;

  if (FOG_UNLIKELY(d->locked == 0))
  {
    pixels._clear();
    return ERR_RT_INVALID_STATE;
  }

  d->locked--;

  pixels._clear();
  return ERR_OK;
}

// ============================================================================
// [Fog::Image - GetDib / SetDib]
// ============================================================================

err_t Image::getDib(int x, int y, uint w, uint32_t dibFormat, void* dst) const
{
  ImageData* d = _d;

  if ((uint)dibFormat >= DIB_FORMAT_COUNT)
    return ERR_IMAGE_INVALID_FORMAT;

  if ((uint)x >= (uint)d->width || (uint)y >= (uint)d->height || (uint)w > (uint)(d->width - x))
    return ERR_RT_INVALID_ARGUMENT;

  const uint8_t* src = d->first + (sysint_t)y * d->stride + (sysint_t)x * d->bytesPerPixel;

  RasterVBlitFullFn vblit_full;
  RasterClosure closure;

  vblit_full = rasterFuncs.dib.convert[dibFormat][getFormat()];
  if (vblit_full == NULL) return ERR_IMAGE_INVALID_FORMAT;

  closure.srcPalette = reinterpret_cast<const uint32_t*>(d->palette.getData());
  closure.dstPalette = NULL;

  vblit_full((uint8_t*)dst, src, w, &closure);
  return ERR_OK;
}

err_t Image::setDib(int x, int y, uint w, uint32_t dibFormat, const void* src)
{
  FOG_RETURN_ON_ERROR(detach());
  ImageData* d = _d;

  if ((uint)dibFormat >= DIB_FORMAT_COUNT)
    return ERR_IMAGE_INVALID_FORMAT;

  if ((uint)x >= (uint)d->width || (uint)y >= (uint)d->height || (uint)w > (uint)(d->width - x))
    return ERR_RT_INVALID_ARGUMENT;

  uint8_t* dst = d->first + (sysint_t)y * d->stride + (sysint_t)x * d->bytesPerPixel;

  RasterVBlitFullFn vblit_full;
  RasterClosure closure;

  vblit_full = rasterFuncs.dib.convert[getFormat()][dibFormat];
  if (vblit_full == NULL) return ERR_IMAGE_INVALID_FORMAT;

  closure.srcPalette = NULL;
  closure.dstPalette = reinterpret_cast<const uint32_t*>(d->palette.getData());

  vblit_full((uint8_t*)dst, (const uint8_t*)src, w, &closure);
  return ERR_OK;
}

// ============================================================================
// [Fog::Image - Swap RGB and ARGB]
// ============================================================================

err_t Image::swapRgb()
{
  if (isEmpty()) return ERR_OK;
  if (getFormat() == IMAGE_FORMAT_A8) return ERR_OK;

  FOG_RETURN_ON_ERROR(detach());
  ImageData* d = _d;

  int x, y;
  int w = d->width;
  int h = d->height;
  int fmt = d->format;

  sysint_t stride = d->stride;
  uint8_t* dst = d->first;

  switch (fmt)
  {
    case IMAGE_FORMAT_I8:
      dst = (uint8_t*)d->palette.getMData();
      if (!dst) return ERR_RT_OUT_OF_MEMORY;
      w = 256;
      h = 1;
      // ... fall through ...

    case IMAGE_FORMAT_PRGB32:
    case IMAGE_FORMAT_ARGB32:
    case IMAGE_FORMAT_XRGB32:
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
      dst += 0;
#else
      dst += 1;
#endif // FOG_BYTE_ORDER
      for (y = h; y; y--, dst += stride)
      {
        uint8_t* dstCur = dst;
        for (x = w; x; x--, dstCur += 4)
        {
          uint8_t t = dstCur[0];
          dstCur[0] = dstCur[2];
          dstCur[2] = t;
        }
      }

      if (fmt == IMAGE_FORMAT_I8) d->palette.update();
      break;

    default:
      FOG_ASSERT_NOT_REACHED();
      return ERR_IMAGE_INVALID_FORMAT;
  }

  return ERR_OK;
}

err_t Image::swapArgb()
{
  if (isEmpty()) return ERR_OK;

  // These formats have only alpha values
  if (getFormat() == IMAGE_FORMAT_A8) return ERR_OK;

  FOG_RETURN_ON_ERROR(detach());
  ImageData* d = _d;

  int x, y;
  int w = d->width;
  int h = d->height;
  int fmt = d->format;

  sysint_t stride = d->stride;
  uint8_t* dst = d->first;

  switch (fmt)
  {
    case IMAGE_FORMAT_I8:
      dst = (uint8_t*)d->palette.getMData();
      if (!dst) return ERR_RT_OUT_OF_MEMORY;
      w = 256;
      h = 1;
      // ... fall through ...

    case IMAGE_FORMAT_PRGB32:
    case IMAGE_FORMAT_ARGB32:
    case IMAGE_FORMAT_XRGB32:
    {
      for (y = h; y; y--, dst += stride)
      {
        uint8_t* dstCur = dst;
        for (x = w; x; x--, dstCur += 4)
        {
          ((uint32_t *)dstCur)[0] = Memory::bswap32(((uint32_t *)dstCur)[0]);
        }
      }
      if (fmt == IMAGE_FORMAT_I8) d->palette.update();
      break;
    }

    default:
      FOG_ASSERT_NOT_REACHED();
      return ERR_IMAGE_INVALID_FORMAT;
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::Image - Premultiply / Demultiply]
// ============================================================================

err_t Image::premultiply()
{
  if (getFormat() == IMAGE_FORMAT_ARGB32)
    return convert(IMAGE_FORMAT_PRGB32);
  else
    return ERR_OK;
}

err_t Image::demultiply()
{
  if (getFormat() == IMAGE_FORMAT_PRGB32)
    return convert(IMAGE_FORMAT_ARGB32);
  else
    return ERR_OK;
}












































// ============================================================================
// [Fog::Image - Invert]
// ============================================================================

err_t Image::invert(Image& dst, const Image& src, uint32_t channels)
{
  if (channels >= 16) return ERR_RT_INVALID_ARGUMENT;

  uint32_t format = src.getFormat();

  // First check for some invertion flags in source image format.
  if (src.isEmpty() || channels == 0 ||
      (!(channels & (COLOR_CHANNEL_RGB  )) && (format == IMAGE_FORMAT_XRGB32)) ||
      (!(channels & (COLOR_CHANNEL_ALPHA)) && (format == IMAGE_FORMAT_A8    )) )
  {
    return dst.set(src);
  }

  // Destination and source can share same data.
  FOG_RETURN_ON_ERROR(dst._d != src._d
    ? dst.create(src.getWidth(), src.getHeight(), format)
    : dst.detach());

  // Prepare data.
  ImageData* dst_d = dst._d;
  ImageData* src_d = src._d;

  sysint_t dstStride = dst_d->stride;
  sysint_t srcStride = src_d->stride;

  uint8_t* dstPixels = dst_d->first;
  uint8_t* srcPixels = src_d->first;

  uint8_t* dstCur;
  uint8_t* srcCur;

  int w = dst_d->width;
  int h = dst_d->height;
  int x;
  int y;

  // Don't touch unused channel for XRGB32 format/
  if (format == IMAGE_FORMAT_XRGB32) channels &= ~COLOR_CHANNEL_ALPHA;

  // Special case for PRGB32 format. We can use direct manipulation without
  // demultiply/premultiply if alpha channel will not be inverted.
  if (format == IMAGE_FORMAT_PRGB32 && (channels & COLOR_CHANNEL_ALPHA) == 0)
  {
    for (y = h; y; y--, dstPixels += dstStride, srcPixels += srcStride)
    {
      uint32_t masks = 0;
      uint32_t maskd = 0;

      if (channels & COLOR_CHANNEL_RED  ) maskd |= ARGB32_RMASK;
      if (channels & COLOR_CHANNEL_GREEN) maskd |= ARGB32_GMASK;
      if (channels & COLOR_CHANNEL_BLUE ) maskd |= ARGB32_BMASK;

      masks = maskd ^ 0xFFFFFFFF;
      dstCur = dstPixels;

      for (x = w; x; x--, dstCur += 4)
      {
        uint32_t pixs = ((uint32_t*)dstCur)[0];
        uint32_t pixd;
        uint32_t pa = (pixs & 0xFF000000) >> 8;

        pixd  = pa - (pixs & 0x00FF0000); pa >>= 8;
        pixd |= pa - (pixs & 0x0000FF00); pa >>= 8;
        pixd |= pa - (pixs & 0x000000FF);

        ((uint32_t*)dstCur)[0] = (pixs & masks) | (pixd & maskd);
      }
    }

    return ERR_OK;
  }

  // Some pixel formats needs special invertion process.
  switch (format)
  {
    case IMAGE_FORMAT_I8:
      dstCur = (uint8_t*)dst_d->palette.getMData();
      if (!dstCur) return ERR_RT_OUT_OF_MEMORY;
      srcCur = (uint8_t*)src_d->palette.getMData();
      if (!srcCur) return ERR_RT_OUT_OF_MEMORY;
      w = 256;
      h = 1;
      // ... fall through ...

    case IMAGE_FORMAT_PRGB32:
    case IMAGE_FORMAT_ARGB32:
    case IMAGE_FORMAT_XRGB32:
    {
      uint32_t mask = 0;

      if (channels & COLOR_CHANNEL_RED  ) mask |= ARGB32_RMASK;
      if (channels & COLOR_CHANNEL_GREEN) mask |= ARGB32_GMASK;
      if (channels & COLOR_CHANNEL_BLUE ) mask |= ARGB32_BMASK;

      // Don't touch alpha-channel if it's not used.
      if ((channels & COLOR_CHANNEL_ALPHA) != 0 &&
          (format == IMAGE_FORMAT_ARGB32 || format == IMAGE_FORMAT_PRGB32))
      {
        mask |= ARGB32_AMASK;
      }

      if (format == IMAGE_FORMAT_PRGB32)
      {
        for (y = h; y; y--, dstPixels += dstStride, srcPixels += srcStride)
        {
          rasterFuncs.dib.convert[IMAGE_FORMAT_ARGB32][IMAGE_FORMAT_PRGB32](
            dstPixels, srcPixels, w, NULL);

          dstCur = dstPixels;
          for (x = w; x; x--, dstCur += 4) ((uint32_t*)dstCur)[0] ^= mask;

          rasterFuncs.dib.convert[IMAGE_FORMAT_PRGB32][IMAGE_FORMAT_ARGB32](
            dstPixels, dstPixels, w, NULL);
        }
      }
      else
      {
        for (y = h; y; y--, dstPixels += dstStride, srcPixels += srcStride)
        {
          dstCur = dstPixels;
          srcCur = srcPixels;

          for (x = w; x; x--, dstCur += 4, srcCur += 4)
          {
            ((uint32_t*)dstCur)[0] = ((uint32_t *)srcCur)[0] ^ mask;
          }
        }
      }

      if (format == IMAGE_FORMAT_I8) dst_d->palette.update();
      break;
    }

    case IMAGE_FORMAT_A8:
    {
      // Should be guaranted that alpha invert is set.
      FOG_ASSERT((channels & COLOR_CHANNEL_ALPHA) != 0);

      for (y = h; y; y--, dstPixels += dstStride, srcPixels += srcStride)
      {
        dstCur = dstPixels;
        srcCur = srcPixels;

        x = w;

        // this will make main loop a much faster
        while (((sysuint_t)dstCur & 7) != 0 && x)
        {
          *dstCur++ = *srcCur++ ^ 0xFF;
          x--;
        }

        while (x >= 8)
        {
#if FOG_ARCH_BITS == 32
          ((uint32_t *)dstCur)[0] = ((uint32_t *)srcCur)[0] ^ 0xFFFFFFFF;
          ((uint32_t *)dstCur)[1] = ((uint32_t *)srcCur)[1] ^ 0xFFFFFFFF;
#else
          ((uint64_t *)dstCur)[0] = ((uint64_t *)srcCur)[0] ^ FOG_UINT64_C(0xFFFFFFFFFFFFFFFF);
#endif // FOG_ARCH_BITS

          dstCur += 8;
          srcCur += 8;
          x -= 8;
        }

        switch (x)
        {
          case 7: *dstCur++ = *srcCur++ ^ 0xFF;
          case 6: *dstCur++ = *srcCur++ ^ 0xFF;
          case 5: *dstCur++ = *srcCur++ ^ 0xFF;
          case 4: *dstCur++ = *srcCur++ ^ 0xFF;
          case 3: *dstCur++ = *srcCur++ ^ 0xFF;
          case 2: *dstCur++ = *srcCur++ ^ 0xFF;
          case 1: *dstCur++ = *srcCur++ ^ 0xFF;
        }
      }
      break;
    }

    default:
      FOG_ASSERT_NOT_REACHED();
      return ERR_IMAGE_INVALID_FORMAT;
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::Image - Mirror]
// ============================================================================

typedef void (FOG_FASTCALL *MirrorFunc)(uint8_t*, uint8_t*, sysuint_t);

static void FOG_FASTCALL mirror_copy_src_is_not_dst_32(uint8_t* dst, uint8_t* src, sysuint_t w)
{ Memory::copy(dst, src, w * 4); }

static void FOG_FASTCALL mirror_copy_src_is_not_dst_8(uint8_t* dst, uint8_t* src, sysuint_t w)
{ Memory::copy(dst, src, w); }

static void FOG_FASTCALL mirror_flip_src_is_not_dst_32(uint8_t* dst, uint8_t* src, sysuint_t w)
{
  src += w * 4 - 4;

  sysuint_t x;
  for (x = w; x; x--, dst += 4, src -= 4) Memory::copy4B(dst, src);
}

static void FOG_FASTCALL mirror_flip_src_is_not_dst_8(uint8_t* dst, uint8_t* src, sysuint_t w)
{
  src += w - 1;

  for (sysuint_t x = w; x; x--, dst++, src--) dst[0] = src[0];
}

static void FOG_FASTCALL mirror_copy_src_is_dst_32(uint8_t* dst, uint8_t* src, sysuint_t w)
{
  Memory::xchg(dst, src, w * 4);
}

static void FOG_FASTCALL mirror_copy_src_is_dst_8(uint8_t* dst, uint8_t* src, sysuint_t w)
{
  Memory::xchg(dst, src, w);
}

static void FOG_FASTCALL mirror_flip_src_is_dst_32(uint8_t* dst, uint8_t* src, sysuint_t w)
{
  sysuint_t x = w;
  if (src == dst) x >>= 1;

  src += ((w - 1) * 4);
  for (; x; x--, dst += 4, src -= 4) Memory::xchg4B(dst, src);
}

static void FOG_FASTCALL mirror_flip_src_is_dst_8(uint8_t* dst, uint8_t* src, sysuint_t w)
{
  sysuint_t x = w;
  if (src == dst) x >>= 1;

  src += w - 1;
  for (; x; x--, dst += 3, src -= 3) Memory::xchg1B(dst, src);
}

static const MirrorFunc mirror_funcs_copy_src_is_not_dst[] =
{
  NULL,
  mirror_copy_src_is_not_dst_8,
  NULL,
  NULL,
  mirror_copy_src_is_not_dst_32
};

static const MirrorFunc mirror_funcs_flip_src_is_not_dst[] =
{
  NULL,
  mirror_flip_src_is_not_dst_8,
  NULL,
  NULL,
  mirror_flip_src_is_not_dst_32
};

static const MirrorFunc mirror_funcs_copy_src_is_dst[] =
{
  NULL,
  mirror_copy_src_is_dst_8,
  NULL,
  NULL,
  mirror_copy_src_is_dst_32
};

static const MirrorFunc mirror_funcs_flip_src_is_dst[] =
{
  NULL,
  mirror_flip_src_is_dst_8,
  NULL,
  NULL,
  mirror_flip_src_is_dst_32
};

err_t Image::mirror(Image& dst, const Image& src, uint32_t mirrorMode)
{
  if (src.isEmpty() || mirrorMode == 0) return dst.set(src);
  if (mirrorMode >= 4) return ERR_RT_INVALID_ARGUMENT;

  uint32_t format = src.getFormat();

  FOG_RETURN_ON_ERROR(dst._d != src._d
    ? dst.create(src.getWidth(), src.getHeight(), format)
    : dst.detach());

  ImageData* dst_d = dst._d;
  ImageData* src_d = src._d;

  sysint_t dstStride = dst_d->stride;
  sysint_t srcStride = src_d->stride;

  uint8_t* dstPixels = dst_d->first;
  uint8_t* srcPixels = src_d->first;

  int w = dst_d->width;
  int h = dst_d->height;

  int bpp = src.getBytesPerPixel();

  MirrorFunc func;

  switch (mirrorMode)
  {
    case IMAGE_MIRROR_VERTICAL:
      srcPixels += srcStride * ((sysint_t)h - 1);
      srcStride = -srcStride;

      if (dst_d != src_d)
      {
        func = mirror_funcs_copy_src_is_not_dst[bpp];
      }
      else
      {
        func = mirror_funcs_copy_src_is_dst[bpp];
        h >>= 1;
      }
      break;

    case IMAGE_MIRROR_HORIZONTAL:
      if (dst_d != src_d)
      {
        func = mirror_funcs_flip_src_is_not_dst[bpp];
      }
      else
      {
        func = mirror_funcs_flip_src_is_dst[bpp];
      }
      break;

    case IMAGE_MIRROR_BOTH:
      srcPixels += srcStride * ((sysint_t)h - 1);
      srcStride = -srcStride;

      if (dst_d != src_d)
      {
        func = mirror_funcs_flip_src_is_not_dst[bpp];
      }
      else
      {
        func = mirror_funcs_flip_src_is_dst[bpp];
        h >>= 1;
      }
      break;
  }

  for (int y = h; y; y--, dstPixels += dstStride, srcPixels += srcStride)
  {
    func(dstPixels, srcPixels, w);
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::Image - Rotate]
// ============================================================================

err_t Image::rotate(Image& dst, const Image& src, uint32_t rotateMode)
{
  if (rotateMode >= 4) return ERR_RT_INVALID_ARGUMENT;
  if (rotateMode == IMAGE_ROTATE_0 || src.isEmpty()) return dst.set(src);

  // rotate by 180 degrees has same effect as MirrorBoth, so we will simply 
  // check for this.
  if (rotateMode == IMAGE_ROTATE_180) return mirror(dst, src, IMAGE_MIRROR_BOTH);

  // Now we have only two possibilities:
  // - rotate by 90
  // - rotate by 270

  // Destination == source?
  if (&dst == &src) return rotate(dst, Image(src), rotateMode);

  err_t err = dst.create(src.getHeight(), src.getWidth(), src.getFormat());
  if (err) return err;

  ImageData* dst_d = dst._d;
  ImageData* src_d = src._d;

  sysint_t dstStride = dst_d->stride;
  sysint_t srcStride = src_d->stride;

  uint8_t* dstPixels = dst_d->first;
  uint8_t* srcPixels = src_d->first;

  uint8_t* dstCur;
  uint8_t* srcCur;

  sysint_t sz1 = (sysuint_t)src_d->width;
  sysint_t sz2 = (sysuint_t)src_d->height;

  sysint_t sz1m1 = sz1-1;
  sysint_t sz2m1 = sz2-1;

  sysint_t i;
  sysint_t j;

  sysint_t srcInc1;
  sysint_t srcInc2;

  sysint_t bpp = src.getBytesPerPixel();

  if (rotateMode == IMAGE_ROTATE_90)
  {
    srcPixels += ((sysint_t)sz2m1 * srcStride);
    srcInc1 = bpp;
    srcInc2 = -srcStride;
  }
  else
  {
    srcPixels += sz1m1 * bpp;
    srcInc1 = -bpp;
    srcInc2 = srcStride;
  }

  switch (bpp)
  {
    case 1:
      for (i = sz1; i; i--, dstPixels += dstStride, srcPixels += srcInc1)
      {
        dstCur = dstPixels;
        srcCur = srcPixels;

        for (j = sz2; j; j--, dstCur += 1, srcCur += srcInc2) dstCur[0] = srcCur[0];
      }
      break;
    case 3:
      for (i = sz1; i; i--, dstPixels += dstStride, srcPixels += srcInc1)
      {
        dstCur = dstPixels;
        srcCur = srcPixels;

        for (j = sz2; j; j--, dstCur += 3, srcCur += srcInc2) Memory::copy3B(dstCur, srcCur);
      }
      break;
    case 4:
      for (i = sz1; i; i--, dstPixels += dstStride, srcPixels += srcInc1)
      {
        dstCur = dstPixels;
        srcCur = srcPixels;

        for (j = sz2; j; j--, dstCur += 4, srcCur += srcInc2) Memory::copy4B(dstCur, srcCur);
      }
      break;

    default:
      FOG_ASSERT_NOT_REACHED();
      return ERR_RT_ASSERTION_FAILURE;
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::Image - Channel related]
// ============================================================================

Image Image::extractChannel(uint32_t channel) const
{
  Image i;

  // Invalid argument error.
  if (channel >= COLOR_CHANNEL_COUNT) return i;
  if ((channel & (channel-1)) != channel) return i;

  // Requested alpha channel on A8 image?
  if (channel == COLOR_CHANNEL_ALPHA && getFormat() == IMAGE_FORMAT_A8)
  {
    i = *this;
    return i;
  }

  // Create image to hold 8 bit channel.
  if (i.create(getWidth(), getHeight(), IMAGE_FORMAT_A8) != ERR_OK) return i;

  // We treat images without alpha channel as full opacity images.
  if (channel == COLOR_CHANNEL_ALPHA && (getFormat() == IMAGE_FORMAT_XRGB32))
  {
    i.clear(0xFF);
    return i;
  }

  // We treat A8 images as 0xAA000000 data.
  if (channel != COLOR_CHANNEL_ALPHA && getFormat() == IMAGE_FORMAT_A8)
  {
    i.clear(0x00);
    return i;
  }

  // Conversion...
  sysint_t dstStride = i._d->stride;
  sysint_t srcStride = _d->stride;

  uint8_t* dstBuf = i._d->first;
  const uint8_t* srcBuf = _d->first;

  sysint_t y, h = i._d->height;
  sysint_t srcStart = 0;
  sysint_t srcInc = _d->bytesPerPixel;

  switch (getFormat())
  {
    case IMAGE_FORMAT_PRGB32:
    case IMAGE_FORMAT_ARGB32:
    case IMAGE_FORMAT_XRGB32:
    case IMAGE_FORMAT_I8:
      if (channel == COLOR_CHANNEL_RED)
        srcStart = ARGB32_RBYTE;
      else if (channel == COLOR_CHANNEL_GREEN)
        srcStart = ARGB32_GBYTE;
      else if (channel == COLOR_CHANNEL_BLUE)
        srcStart = ARGB32_BBYTE;
      else
        srcStart = ARGB32_ABYTE;
      break;
  }

  if (getFormat() != IMAGE_FORMAT_I8)
  {
    for (y = 0; y < h; y++, dstBuf += dstStride, srcBuf += srcStride)
    {
      uint8_t* dstCur = dstBuf;
      const uint8_t* srcCur = srcBuf;

      dstCur[0] = srcCur[0];
      dstCur++;
      srcCur += srcInc;
    }
  }
  else
  {
    const uint8_t* pal = reinterpret_cast<const uint8_t*>(_d->palette.getData()) + srcStart;
    for (y = 0; y < h; y++, dstBuf += dstStride, srcBuf += srcStride)
    {
      uint8_t* dstCur = dstBuf;
      const uint8_t* srcCur = srcBuf;

      dstCur[0] = pal[srcCur[0] * 4];
      dstCur++;
      srcCur++;
    }
  }

  return i;
}

// ============================================================================
// [Fog::Image - Color Filter]
// ============================================================================

static err_t applyColorFilter(Image& im, const IntBox& box, ColorFilterFn fn, const void* context)
{
  // Clip.
  int imgw = im.getWidth();
  int imgh = im.getHeight();

  int x1 = Math::max<int>(box.getX1(), 0);
  int y1 = Math::max<int>(box.getY1(), 0);
  int x2 = Math::min<int>(box.getX2(), imgw);
  int y2 = Math::min<int>(box.getY2(), imgh);

  if (x1 >= x2 || y1 >= y2) return ERR_OK;

  int w = x2 - x1;
  int h = y2 - y1;

  FOG_RETURN_ON_ERROR(im.detach());

  uint8_t* imData = im.getXData();
  sysint_t imStride = im.getStride();
  sysint_t imBpp = im.getBytesPerPixel();

  sysuint_t y;

  uint8_t* cur = imData + (sysint_t)y1 * imStride + (sysint_t)x1 * imBpp;
  for (y = (uint)h; y; y--, cur += imStride) fn(context, cur, cur, w);

  return ERR_OK;
}

err_t Image::filter(const ColorFilter& f, const IntRect* area)
{
  IntBox abox(0, 0, getWidth(), getHeight());
  if (area) abox.set(area->getX1(), area->getY1(), area->getX2(), area->getY2());

  ColorFilterFn fn = f.getEngine()->getColorFilterFn(getFormat());
  if (!fn) return ERR_IMAGE_UNSUPPORTED_FORMAT;

  const void* context = f.getEngine()->getContext();
  err_t err = applyColorFilter(*this, abox, fn, context);

  f.getEngine()->releaseContext(context);
  return err;
}

err_t Image::filter(const ColorLut& lut, const IntRect* area)
{
  IntBox abox(0, 0, getWidth(), getHeight());
  if (area) abox.set(area->getX1(), area->getY1(), area->getX2(), area->getY2());

  ColorFilterFn fn = (ColorFilterFn)rasterFuncs.filter.color_lut[getFormat()];
  if (!fn) return ERR_IMAGE_UNSUPPORTED_FORMAT;

  return applyColorFilter(*this, abox, fn, lut.getData());
}

err_t Image::filter(const ColorMatrix& cm, const IntRect* area)
{
  IntBox abox(0, 0, getWidth(), getHeight());
  if (area) abox.set(area->getX1(), area->getY1(), area->getX2(), area->getY2());

  ColorFilterFn fn = (ColorFilterFn)rasterFuncs.filter.color_matrix[getFormat()];
  if (!fn) return ERR_IMAGE_UNSUPPORTED_FORMAT;

  return applyColorFilter(*this, abox, fn, &cm);
}

// ============================================================================
// [Fog::Image - Image Filter]
// ============================================================================

static err_t applyImageFilter(Image& im, const IntBox& box, const ImageFilter& filter)
{
  // Never call applyImageFilter() with color filter, see applyColorFilter().
  FOG_ASSERT((filter.getCharacteristics() & IMAGE_FILTER_CHAR_COLOR_TRANSFORM) == 0);

  // Clip.
  int imgw = im.getWidth();
  int imgh = im.getHeight();
  int imgf = im.getFormat();

  int x1 = Math::max<int>(box.getX1(), 0);
  int y1 = Math::max<int>(box.getY1(), 0);
  int x2 = Math::min<int>(box.getX2(), imgw);
  int y2 = Math::min<int>(box.getY2(), imgh);

  if (x1 >= x2 || y1 >= y2) return ERR_OK;

  int w = x2 - x1;
  int h = y2 - y1;

  uint32_t filterFormat = imgf;
  uint32_t filterCharacteristics = filter.getCharacteristics();

  if ((filterCharacteristics & (IMAGE_FILTER_CHAR_HV_PROCESSING | IMAGE_FILTER_CHAR_ENTIRE_PROCESSING)) == 0)
  {
    // NOP.
    return ERR_OK;
  }

  FOG_RETURN_ON_ERROR(im.detach());

  const void* context = filter.getEngine()->getContext();
  ImageFilterFn fn;

  uint8_t* imData = im.getXData();
  sysint_t imStride = im.getStride();
  sysint_t imBpp = im.getBytesPerPixel();

  uint8_t* imBegin = imData + (sysint_t)y1 * imStride + (sysint_t)x1 * imBpp;
  uint8_t* imCur = imBegin;

  // Demultiply if needed.
  if (imgf == IMAGE_FORMAT_PRGB32 && (filterCharacteristics & IMAGE_FILTER_CHAR_SUPPORTS_PRGB32) == 0)
  {
    RasterVBlitFullFn vblit_full = 
      rasterFuncs.dib.convert[IMAGE_FORMAT_ARGB32][IMAGE_FORMAT_PRGB32];
    for (int y = h; y; y--, imCur += imStride) vblit_full(imCur, imCur, w, NULL);

    imCur = imBegin;
    filterFormat = IMAGE_FORMAT_ARGB32;
  }

  err_t err = ERR_OK;

  // Vertical & Horizontal processing.
  if ((filterCharacteristics & IMAGE_FILTER_CHAR_HV_PROCESSING) == IMAGE_FILTER_CHAR_HV_PROCESSING)
  {
    fn = filter.getEngine()->getImageFilterFn(filterFormat, IMAGE_FILTER_CHAR_VERT_PROCESSING);
    if (!fn) { err = ERR_IMAGE_UNSUPPORTED_FORMAT; goto end; }

    fn(context, imCur, imStride, imCur, imStride, w, h, -1);

    fn = filter.getEngine()->getImageFilterFn(filterFormat, IMAGE_FILTER_CHAR_HORZ_PROCESSING);
    if (!fn) { err = ERR_IMAGE_UNSUPPORTED_FORMAT; goto end; }

    fn(context, imCur, imStride, imCur, imStride, w, h, -1);
  }
  // Vertical processing only (one pass).
  else if ((filterCharacteristics & IMAGE_FILTER_CHAR_VERT_PROCESSING) != 0)
  {
    fn = filter.getEngine()->getImageFilterFn(filterFormat, IMAGE_FILTER_CHAR_VERT_PROCESSING);
    if (!fn) { err = ERR_IMAGE_UNSUPPORTED_FORMAT; goto end; }

    fn(context, imCur, imStride, imCur, imStride, w, h, -1);
  }
  // Horizontal processing only (one pass).
  else if ((filterCharacteristics & IMAGE_FILTER_CHAR_HORZ_PROCESSING) != 0)
  {
    fn = filter.getEngine()->getImageFilterFn(filterFormat, IMAGE_FILTER_CHAR_HORZ_PROCESSING);
    if (!fn) { err = ERR_IMAGE_UNSUPPORTED_FORMAT; goto end; }

    fn(context, imCur, imStride, imCur, imStride, w, h, -1);
  }
  // Entire processing (one pass).
  else if ((filterCharacteristics & IMAGE_FILTER_CHAR_ENTIRE_PROCESSING) != 0)
  {
    fn = filter.getEngine()->getImageFilterFn(filterFormat, IMAGE_FILTER_CHAR_ENTIRE_PROCESSING);
    if (!fn) { err = ERR_IMAGE_UNSUPPORTED_FORMAT; goto end; }

    fn(context, imCur, imStride, imCur, imStride, w, h, -1);
  }

  // Premultiply if demultiplied.
  if (imgf == IMAGE_FORMAT_PRGB32 && (filterCharacteristics & IMAGE_FILTER_CHAR_SUPPORTS_PRGB32) == 0)
  {
    RasterVBlitFullFn vblit_full = 
      rasterFuncs.dib.convert[IMAGE_FORMAT_PRGB32][IMAGE_FORMAT_ARGB32];
    for (int y = h; y; y--, imCur += imStride) vblit_full(imCur, imCur, w, NULL);
  }

end:
  filter.getEngine()->releaseContext(context);
  return err;
}

err_t Image::filter(const ImageFilter& f, const IntRect* area)
{
  // Use optimized way for ColorFilter if image doest color transform.
  if (f.getCharacteristics() & IMAGE_FILTER_CHAR_COLOR_TRANSFORM)
  {
    return filter(reinterpret_cast<const ColorFilter&>(f), area);
  }

  IntBox abox(0, 0, getWidth(), getHeight());
  if (area) abox.set(area->getX1(), area->getY1(), area->getX2(), area->getY2());

  return applyImageFilter(*this, abox, f);
}

// ============================================================================
// [Fog::Image - Scaling]
// ============================================================================

Image Image::scale(const IntSize& to, uint32_t interpolationType)
{
  Image dst;

  if (isEmpty() || (getWidth() == to.w && getHeight() == to.h))
  {
    dst = *this;
    return dst;
  }

  if (to.w == 0 || to.h == 0) return dst;
  if (dst.create(to.w, to.h, getFormat()) != ERR_OK) return dst;

  RasterPattern ctx;
  err_t err = rasterFuncs.pattern.texture_init_scale(&ctx, *this, to.w, to.h, interpolationType);
  if (err != ERR_OK) { dst.free(); return dst; }

  uint8_t* dstData = (uint8_t*)dst.getData();
  sysint_t dstStride = dst.getStride();

  SpanExt8 span;
  span.setPositionAndType(0, to.w, SPAN_TYPE_CMASK);
  span.setCMask(0xFF);
  span.setNext(NULL);

  for (int y = 0; y < to.h; y++, dstData += dstStride)
  {
    ctx.fetch(&ctx, &span, dstData, y, PATTERN_FETCH_BUFFER_ONLY);
  }

  ctx.destroy(&ctx);
  return dst;
}

// ============================================================================
// [Fog::Image - Painting]
// ============================================================================

err_t Image::clear(Argb c0)
{
  return fillRect(IntRect(0, 0, getWidth(), getHeight()), c0, OPERATOR_SRC);
}

err_t Image::drawPixel(const IntPoint& pt, Argb c0)
{
  if ((uint)pt.x >= (uint)getWidth() || (uint)pt.y >= (uint)getHeight())
    return ERR_OK;

  FOG_RETURN_ON_ERROR(detach());

  uint8_t* dstCur = _d->first + pt.y * _d->stride;

  switch (getFormat())
  {
    case IMAGE_FORMAT_ARGB32:
    case IMAGE_FORMAT_PRGB32:
    case IMAGE_FORMAT_XRGB32:
      ((uint32_t*)dstCur)[(uint)pt.x] = c0;
      break;

    case IMAGE_FORMAT_A8:
    case IMAGE_FORMAT_I8:
      dstCur[(uint)pt.x] = (uint8_t)c0;
      break;

    default:
      FOG_ASSERT_NOT_REACHED();
      return ERR_RT_ASSERTION_FAILURE;
  }

  return ERR_OK;
}

// TODO: Update this, some structures must be defined to be usable.
template<typename Op>
static void Draw_BresenhamLine(uint8_t* dst, sysint_t dstStride, uint32_t c0, RasterEngine::BresenhamLineIterator& line, bool last)
{
  sysint_t dstIncX = Op::BytesPerPixel;
  sysint_t dstIncY = dstStride;

  if (line.incx < 0) dstIncX = -dstIncX;
  if (line.incy < 0) dstIncY = -dstIncY;

  if (line.hOriented())
  {
    while (!line.hDone())
    {
      Op::store(dst, c0);
      line.hIncPtr(&dst, dstIncX, dstIncY);
    }
  }
  else
  {
    while (!line.vDone())
    {
      Op::store(dst, c0);
      line.vIncPtr(&dst, dstIncX, dstIncY);
    }
  }

  // TODO: When clipped it's omitted ?
  if (last) Op::store(dst, c0);
}

err_t Image::drawLine(const IntPoint& pt0, const IntPoint& pt1, Argb c0, bool lastPoint)
{
  if (isEmpty())
    return ERR_OK;

  RasterEngine::BresenhamLineIterator line;
  if (!line.initAndClip(pt0.getX(), pt0.getY(), pt1.getX(), pt1.getY(), 0, 0, getWidth()-1, getHeight()-1))
    return ERR_OK;

  FOG_RETURN_ON_ERROR(detach());

  sysint_t dstStride = _d->stride;
  uint8_t* dstCur = _d->first + line.y * dstStride;

  switch (getFormat())
  {
    case IMAGE_FORMAT_PRGB32:
    case IMAGE_FORMAT_ARGB32:
    case IMAGE_FORMAT_XRGB32:
      dstCur += (uint)line.x * 4;
      Draw_BresenhamLine<RasterEngine::PixFmt_ARGB32>(dstCur, dstStride, c0, line, lastPoint);
      break;

    case IMAGE_FORMAT_A8:
    case IMAGE_FORMAT_I8:
      dstCur += line.x;
      Draw_BresenhamLine<RasterEngine::PixFmt_A8>(dstCur, dstStride, c0, line, lastPoint);
      break;

    default:
      FOG_ASSERT_NOT_REACHED();
      return ERR_RT_ASSERTION_FAILURE;
  }

  return ERR_OK;
}

err_t Image::fillRect(const IntRect& r, Argb c0, int op)
{
  if ((uint)op >= OPERATOR_COUNT) return ERR_RT_INVALID_ARGUMENT;

  int x1 = r.getX1();
  int y1 = r.getY1();
  int x2 = r.getX2();
  int y2 = r.getY2();

  int w = _d->width;
  int h = _d->height;
  int fmt = _d->format;

  bool isOpaque = (c0 & 0xFF000000) == 0xFF000000;
  if (op == OPERATOR_SRC_OVER && isOpaque) op = OPERATOR_SRC;

  if (x1 < 0) x1 = 0;
  if (y1 < 0) y1 = 0;
  if (x2 > w) x2 = w;
  if (y2 > h) y2 = h;

  const RasterFuncs::CompositeFuncs* funcs = rasterFuncs.getCompositeFuncs(op, fmt);
  if (funcs == NULL) return ERR_RT_NOT_IMPLEMENTED;

  if ((w = x2 - x1) <= 0) return ERR_OK;
  if ((h = y2 - y1) <= 0) return ERR_OK;

  FOG_RETURN_ON_ERROR(detach());

  sysint_t dstStride = _d->stride;
  uint8_t* dstCur = _d->first + y1 * dstStride + x1 * getBytesPerPixel();

  RasterSolid solid;
  solid.argb = c0;
  solid.prgb = ColorUtil::premultiply(c0);

  RasterCBlitFullFn cblit_full = 
    funcs->cblit_full[isOpaque ? IMAGE_FORMAT_XRGB32 : IMAGE_FORMAT_PRGB32];

  RasterClosure closure;
  closure.dstPalette = reinterpret_cast<const uint32_t*>(_d->palette.getData());
  closure.srcPalette = NULL;

  for (int i = 0; i < h; i++, dstCur += dstStride)
  {
    cblit_full(dstCur, &solid, w, &closure);
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::Image - Painting - Gradients]
// ============================================================================

err_t Image::fillQGradient(const IntRect& r, Argb c0, Argb c1, Argb c2, Argb c3, int op)
{
  if (getFormat() == IMAGE_FORMAT_I8) return ERR_IMAGE_UNSUPPORTED_FORMAT;

  // Optimized variants.
  if (c0 == c1 && c2 == c3) return fillVGradient(r, c0, c2, op);
  if (c0 == c2 && c1 == c3) return fillHGradient(r, c0, c1, op);

  int i;

  int x1 = r.getX1();
  int y1 = r.getY1();
  int x2 = r.getX2();
  int y2 = r.getY2();

  int w = _d->width;
  int h = _d->height;
  int fmt = _d->format;

  bool isOpaque = (c0 & 0xFF000000) == 0xFF000000 &&
                  (c1 & 0xFF000000) == 0xFF000000 &&
                  (c2 & 0xFF000000) == 0xFF000000 &&
                  (c3 & 0xFF000000) == 0xFF000000 ;
  uint32_t sourceFormat = isOpaque ? IMAGE_FORMAT_XRGB32 : IMAGE_FORMAT_ARGB32;
  if (op == OPERATOR_SRC_OVER && isOpaque) op = OPERATOR_SRC;

  if (x1 < 0) x1 = 0;
  if (y1 < 0) y1 = 0;
  if (x2 > w) x2 = w;
  if (y2 > h) y2 = h;

  const RasterFuncs::CompositeFuncs* funcs = rasterFuncs.getCompositeFuncs(op, fmt);
  if (funcs == NULL) return ERR_RT_NOT_IMPLEMENTED;

  if ((w = x2 - x1) <= 0) return ERR_OK;
  if ((h = y2 - y1) <= 0) return ERR_OK;

  FOG_RETURN_ON_ERROR(detach());

  sysint_t dstStride = _d->stride;
  uint8_t* dstCur = _d->first + y1 * dstStride + x1 * getBytesPerPixel();

  LocalBuffer<512> memStorage;
  uint8_t* mem = (uint8_t*)memStorage.alloc(h * 2 * sizeof(uint32_t) + w * sizeof(uint32_t));
  if (!mem) return ERR_RT_OUT_OF_MEMORY;

  uint8_t* shade0 = mem;
  uint8_t* shade1 = shade0 + h * sizeof(uint32_t);
  uint8_t* shadeW = shade1 + h * sizeof(uint32_t);

  RasterInterpolateArgbFn gradientSpan;

  // Interpolate vertical lines (c0 to c2 and c1 to c3).
  gradientSpan = rasterFuncs.interpolate.gradient[IMAGE_FORMAT_ARGB32];
  gradientSpan(shade0, c0, c2, h, 0, h);
  gradientSpan(shade1, c1, c3, h, 0, h);

  switch (fmt)
  {
    case IMAGE_FORMAT_PRGB32:
      // We must premultiply if dst is premultiplied. The reason why we are
      // setting it here and not before is that all gradient functions need
      // colors in ARGB colorspace.
      gradientSpan = rasterFuncs.interpolate.gradient[IMAGE_FORMAT_PRGB32];
      sourceFormat = IMAGE_FORMAT_PRGB32;
      // ... fall throught ...
    case IMAGE_FORMAT_ARGB32:
    case IMAGE_FORMAT_XRGB32:
      // If operator is CompositeSrc, we will directly render gradient spans
      // into image buffer (this is fastest possible gradient rendering).
      if (op == OPERATOR_SRC)
      {
        for (i = 0; i < h; i++, dstCur += dstStride, shade0 += sizeof(uint32_t), shade1 += sizeof(uint32_t))
        {
          gradientSpan(dstCur, ((uint32_t*)shade0)[0], ((uint32_t*)shade1)[0], w, 0, w);
        }
        break;
      }
      // ... fall throught ...
    default:
    {
      RasterVBlitFullFn vblit_full = funcs->vblit_full[sourceFormat];

      RasterClosure closure;
      closure.dstPalette = NULL;
      closure.srcPalette = NULL;

      for (i = 0; i < h; i++, dstCur += dstStride, shade0 += sizeof(uint32_t), shade1 += sizeof(uint32_t))
      {
        gradientSpan(shadeW, ((uint32_t*)shade0)[0], ((uint32_t*)shade1)[0], w, 0, w);
        vblit_full(dstCur, shadeW, w, &closure);
      }
      break;
    }
  }
  return ERR_OK;
}

err_t Image::fillHGradient(const IntRect& r, Argb c0, Argb c1, int op)
{
  if (getFormat() == IMAGE_FORMAT_I8) return ERR_IMAGE_UNSUPPORTED_FORMAT;
  if (c0 == c1) return fillRect(r, c0, op);

  int i;

  int x1 = r.getX1();
  int y1 = r.getY1();
  int x2 = r.getX2();
  int y2 = r.getY2();

  int w = _d->width;
  int h = _d->height;
  int fmt = _d->format;

  bool isOpaque = Argb::isAlpha0xFF(c0) && Argb::isAlpha0xFF(c1);
  if (op == OPERATOR_SRC_OVER && isOpaque) op = OPERATOR_SRC;

  if (x1 < 0) x1 = 0;
  if (y1 < 0) y1 = 0;
  if (x2 > w) x2 = w;
  if (y2 > h) y2 = h;

  const RasterFuncs::CompositeFuncs* funcs = rasterFuncs.getCompositeFuncs(op, fmt);
  if (funcs == NULL) return ERR_RT_NOT_IMPLEMENTED;

  if ((w = x2 - x1) <= 0) return ERR_OK;
  if ((h = y2 - y1) <= 0) return ERR_OK;

  FOG_RETURN_ON_ERROR(detach());

  sysint_t dstStride = _d->stride;
  uint8_t* dstCur = _d->first + y1 * dstStride + x1 * getBytesPerPixel();

  LocalBuffer<512> memStorage;
  uint8_t* shade0 = (uint8_t*)memStorage.alloc(w * sizeof(uint32_t));
  if (!shade0) return ERR_RT_OUT_OF_MEMORY;

  RasterInterpolateArgbFn gradientSpan =
    fmt == IMAGE_FORMAT_PRGB32
      ? rasterFuncs.interpolate.gradient[IMAGE_FORMAT_PRGB32]
      : rasterFuncs.interpolate.gradient[IMAGE_FORMAT_ARGB32];

  gradientSpan(shade0, c0, c1, w, 0, w);

  RasterVBlitFullFn vblit_full = 
    funcs->vblit_full[fmt == IMAGE_FORMAT_PRGB32 ? IMAGE_FORMAT_PRGB32 : IMAGE_FORMAT_ARGB32];

  RasterClosure closure;
  closure.dstPalette = NULL;
  closure.srcPalette = NULL;

  for (i = 0; i < h; i++, dstCur += dstStride)
  {
    vblit_full(dstCur, shade0, w, &closure);
  }
  return ERR_OK;
}

err_t Image::fillVGradient(const IntRect& r, Argb c0, Argb c1, int op)
{
  if (getFormat() == IMAGE_FORMAT_I8) return ERR_IMAGE_UNSUPPORTED_FORMAT;
  if (c0 == c1) return fillRect(r, c0, op);

  int i;

  int x1 = r.getX1();
  int y1 = r.getY1();
  int x2 = r.getX2();
  int y2 = r.getY2();

  int w = _d->width;
  int h = _d->height;
  int fmt = _d->format;

  bool isOpaque = Argb::isAlpha0xFF(c0) && Argb::isAlpha0xFF(c1);
  if (op == OPERATOR_SRC_OVER && isOpaque) op = OPERATOR_SRC;

  if (x1 < 0) x1 = 0;
  if (y1 < 0) y1 = 0;
  if (x2 > w) x2 = w;
  if (y2 > h) y2 = h;

  const RasterFuncs::CompositeFuncs* funcs = rasterFuncs.getCompositeFuncs(op, fmt);
  if (funcs == NULL) return ERR_RT_NOT_IMPLEMENTED;

  if ((w = x2 - x1) <= 0) return ERR_OK;
  if ((h = y2 - y1) <= 0) return ERR_OK;

  FOG_RETURN_ON_ERROR(detach());

  sysint_t dstStride = _d->stride;
  uint8_t* dstCur = _d->first + y1 * dstStride + x1 * getBytesPerPixel();

  LocalBuffer<512> memStorage;
  uint8_t* mem = (uint8_t*)memStorage.alloc(h * sizeof(uint32_t));
  if (!mem) return ERR_RT_OUT_OF_MEMORY;

  uint8_t* shade0 = mem;

  RasterInterpolateArgbFn gradientSpan = rasterFuncs.interpolate.gradient[IMAGE_FORMAT_ARGB32];
  gradientSpan(shade0, c0, c1, h, 0, h);

  RasterCBlitFullFn cblit_full =
    funcs->cblit_full[isOpaque ? IMAGE_FORMAT_XRGB32 : IMAGE_FORMAT_PRGB32];

  RasterClosure closure;
  closure.dstPalette = NULL;
  closure.srcPalette = NULL;

  for (i = 0; i < h; i++, dstCur += dstStride, shade0 += sizeof(uint32_t))
  {
    RasterSolid solid;
    solid.argb = ((uint32_t*)shade0)[0];
    solid.prgb = ColorUtil::premultiply(solid.argb);
    cblit_full(dstCur, &solid, w, &closure);
  }
  return ERR_OK;
}

// ============================================================================
// [Fog::Image - Painting - Blit]
// ============================================================================

static err_t _drawImage(
  ImageData* dstD, int dstX, int dstY,
  ImageData* srcD, int srcX, int srcY,
  int w, int h,
  uint32_t op, uint32_t opacity)
{
  sysint_t dstStride = dstD->stride;
  sysint_t srcStride = srcD->stride;

  uint8_t* dstPixels = dstD->first;
  const uint8_t* srcPixels = srcD->first;

  // Special case if dst and src overlaps.
  if (dstD == srcD && dstY >= srcY && (dstY - srcY) <= h)
  {
    dstPixels += (dstY + h - 1) * dstStride;
    srcPixels += (srcY + h - 1) * srcStride;

    dstStride = -dstStride;
    srcStride = -srcStride;
  }
  else
  {
    dstPixels += dstY * dstStride;
    srcPixels += srcY * srcStride;
  }

  dstPixels += dstX * dstD->bytesPerPixel;
  srcPixels += srcX * srcD->bytesPerPixel;

  const RasterFuncs::CompositeFuncs* funcs = rasterFuncs.getCompositeFuncs(op, dstD->format);

  RasterClosure closure;
  closure.dstPalette = reinterpret_cast<const uint32_t*>(dstD->palette.getData());
  closure.srcPalette = reinterpret_cast<const uint32_t*>(srcD->palette.getData());

  // Support for overlapping blit.
  if (dstD == srcD && dstX >= srcX && (dstX - srcX) <= w)
  {
    LocalBuffer<512> bufStorage;
    sysint_t bufSize = w * srcD->bytesPerPixel;
    uint8_t* buf = (uint8_t*)bufStorage.alloc(bufSize);
    if (!buf) return ERR_RT_OUT_OF_MEMORY;

    RasterVBlitFullFn vblit_copy = rasterFuncs.dib.memcpy8;

    if (opacity >= 255)
    {
      RasterVBlitFullFn vblit_full = funcs->vblit_full[srcD->format];

      for (int y = h; y; y--, dstPixels += dstStride, srcPixels += srcStride)
      {
        vblit_copy(buf, srcPixels, (int)bufSize, NULL);
        vblit_full(dstPixels, buf, w, &closure);
      }
    }
    else
    {
      RasterVBlitSpanFn vblit_span = funcs->vblit_span[srcD->format];

      SpanExt8 span;
      span.setPositionAndType(0, w, SPAN_TYPE_CMASK);
      span.setCMask(opacity);
      span.setNext(NULL);
      span.setData(buf);

      for (int y = h; y; y--, dstPixels += dstStride, srcPixels += srcStride)
      {
        vblit_copy(buf, srcPixels, (int)bufSize, NULL);
        vblit_span(dstPixels, &span, &closure);
      }
    }
  }
  // Normal case.
  else
  {
    if (opacity >= 255)
    {
      RasterVBlitFullFn vblit_full = funcs->vblit_full[srcD->format];

      for (int y = h; y; y--, dstPixels += dstStride, srcPixels += srcStride)
      {
        vblit_full(dstPixels, srcPixels, w, &closure);
      }
    }
    else
    {
      RasterVBlitSpanFn vblit_span = funcs->vblit_span[srcD->format];

      SpanExt8 span;
      span.setPositionAndType(0, w, SPAN_TYPE_CMASK);
      span.setCMask(opacity);
      span.setNext(NULL);

      for (int y = h; y; y--, dstPixels += dstStride, srcPixels += srcStride)
      {
        span.setData(const_cast<uint8_t*>(srcPixels));
        vblit_span(dstPixels, &span, &closure);
      }
    }
  }
  return ERR_OK;
}

err_t Image::drawImage(const IntPoint& pt, const Image& src, uint32_t op, uint32_t opacity)
{
  if ((uint)op >= OPERATOR_COUNT) return ERR_RT_INVALID_ARGUMENT;
  if (opacity == 0) return ERR_OK;

  ImageData* dst_d = _d;
  ImageData* src_d = src._d;

  int w = dst_d->width;
  int h = dst_d->height;

  int x1 = pt.getX();
  int y1 = pt.getY();
  int x2 = x1 + src_d->width;
  int y2 = y1 + src_d->height;

  if (x1 < 0) x1 = 0;
  if (y1 < 0) y1 = 0;
  if (x2 > w) x2 = w;
  if (y2 > h) y2 = h;

  if (x1 >= x2 || y1 >= y2) return ERR_OK;

  FOG_RETURN_ON_ERROR(detach());
  return _drawImage(_d, x1, y1, src_d, x1 - pt.getX(), y1 - pt.getY(), x2 - x1, y2 - y1, op, opacity);
}

err_t Image::drawImage(const IntPoint& pt, const Image& src, const IntRect& srcRect, uint32_t op, uint32_t opacity)
{
  if ((uint)op >= OPERATOR_COUNT) return ERR_RT_INVALID_ARGUMENT;
  if (!srcRect.isValid()) return ERR_OK;
  if (opacity == 0) return ERR_OK;

  ImageData* dst_d = _d;
  ImageData* src_d = src._d;

  int srcX1 = srcRect.getX1();
  int srcY1 = srcRect.getY1();
  int srcX2 = srcRect.getX2();
  int srcY2 = srcRect.getY2();

  if (srcX1 < 0) srcX1 = 0;
  if (srcY1 < 0) srcY1 = 0;
  if (srcX2 > src_d->width) srcX1 = src_d->width;
  if (srcY2 > src_d->height) srcY2 = src_d->height;

  if (srcX1 >= srcX2 || srcY1 >= srcY2) return ERR_OK;

  int dstX1 = pt.getX() + (srcX1 - srcRect.getX1());
  int dstY1 = pt.getY() + (srcY1 - srcRect.getY1());
  int dstX2 = dstX1 + (srcX2 - srcX1);
  int dstY2 = dstY1 + (srcY2 - srcY1);

  if (dstX1 < 0) { srcX1 -= dstX1; dstX1 = 0; }
  if (dstY1 < 0) { srcY1 -= dstY1; dstY1 = 0; }
  if (dstX2 > dst_d->width) dstX2 = dst_d->width;
  if (dstY2 > dst_d->height) dstY2 = dst_d->height;

  if (dstX1 >= dstX2 || dstY1 >= dstY2) return ERR_OK;

  FOG_RETURN_ON_ERROR(detach());
  return _drawImage(_d, dstX1, dstY1, src_d, srcX1, srcY1, dstX2 - dstX1, dstY2 - dstY1, op, opacity);
}

// ============================================================================
// [Fog::Image - Painting - Scroll]
// ============================================================================

err_t Image::scroll(int scrollX, int scrollY)
{
  return scroll(scrollX, scrollY, IntRect(0, 0, getWidth(), getHeight()));
}

err_t Image::scroll(int scrollX, int scrollY, const IntRect& r)
{
  if (scrollX == 0 && scrollY == 0) return ERR_OK;

  ImageData* d = _d;

  int x1 = r.getX1();
  int y1 = r.getY1();
  int x2 = r.getX2();
  int y2 = r.getY2();

  if (x1 < 0) x1 = 0;
  if (y1 < 0) y1 = 0;
  if (x2 > d->width) x1 = d->width;
  if (y2 > d->height) y2 = d->height;

  if (x1 >= x2 || y1 >= y2) return ERR_OK;

  int absX = abs(scrollX);
  int absY = abs(scrollY);

  int scrollW = x2 - x1;
  int scrollH = y2 - y1;

  int srcX, srcY;
  int dstX, dstY;

  if (absX >= scrollW || absY >= scrollH) return ERR_OK;

  FOG_RETURN_ON_ERROR(detach());

  if (scrollX < 0) { srcX = absX; dstX = 0; } else { srcX = 0; dstX = absX; }
  if (scrollY < 0) { srcY = absY; dstY = 0; } else { srcY = 0; dstY = absY; }

  scrollW -= absX;
  scrollH -= absY;

  sysint_t stride = d->stride;
  sysint_t size = scrollW * d->bytesPerPixel;

  uint8_t* dstPixels = d->first + dstX * d->bytesPerPixel;
  uint8_t* srcPixels = d->first + srcX * d->bytesPerPixel;

  dstPixels += dstY * stride;
  srcPixels += srcY * stride;

  if (scrollY > 0)
  {
    dstPixels += (scrollH - 1) * stride;
    srcPixels += (scrollH - 1) * stride;

    stride = -stride;
  }

  for (int y = scrollH; y; y--, dstPixels += stride, srcPixels += stride)
    memmove(dstPixels, srcPixels, size);

  return ERR_OK;
}

// ============================================================================
// [Fog::Image - Misc]
// ============================================================================

err_t Image::glyphFromPath(Image& glyph, IntPoint& offset, const DoublePath& path)
{
  if (path.isEmpty())
  {
    glyph.free();
    offset.clear();
    return ERR_OK;
  }

  Rasterizer* rasterizer = Rasterizer::getRasterizer();
  if (rasterizer == NULL) return ERR_RT_OUT_OF_MEMORY;

  err_t err = ERR_OK;

  if (path.isFlat())
  {
    rasterizer->addPath(path);
  }
  else
  {
    DoublePath temp;

    err = path.flattenTo(temp, NULL, 1.0);
    if (err != ERR_OK) goto end;

    rasterizer->addPath(temp);
  }

  rasterizer->finalize();

  if (rasterizer->isValid())
  {
    IntBox bounds(rasterizer->getBoundingBox());

    Scanline8 scanline;

    int w = bounds.getWidth();
    int h = bounds.getHeight();

    err = glyph.create(w, h, IMAGE_FORMAT_A8);
    if (err != ERR_OK) goto end;

    uint8_t* glyphData = glyph.getXData();
    sysint_t glyphStride = glyph.getStride();

    int y;
    for (y = 0; y < h; y++, glyphData += glyphStride)
    {
      Memory::zero(glyphData, glyphStride);

      Span8* span = rasterizer->sweepScanline(scanline, y + bounds.y1);
      while (span)
      {
        uint8_t* p = glyphData + (uint)(span->getX1() - bounds.x1);
        uint len = (uint)span->getLength();

        if (span->isCMask())
          memset(p, span->getCMask(), len);
        else
          memcpy(p, span->getVMask(), len);

        span = span->getNext();
      }
    }
  }
  else
  {
    goto empty;
  }

end:
  if (err != ERR_OK)
  {
empty:
    glyph.free();
    offset.clear();
  }

  Rasterizer::releaseRasterizer(rasterizer);
  return err;
}

// ============================================================================
// [Fog::Image - Windows Specific]
// ============================================================================

#if defined(FOG_OS_WINDOWS)
/*
  Following code can be used to test Image <=> HBITMAP conversion:

  HDC hdc = CreateCompatibleDC(NULL);
  HBITMAP hbmp = CreateCompatibleBitmap(hdc, 320, 200);
  RECT r = {0 , 0, 320, 200 };

  HGDIOBJ old = SelectObject(hdc, (HGDIOBJ)hbmp);
  FillRect(hdc, &r, (HBRUSH)GetStockObject(WHITE_BRUSH));
  SetBkMode(hdc, TRANSPARENT);
  TextOut(hdc, 0, 0, "Abcdefghijklmnop", 16);
  SelectObject(hdc, old);
  DeleteObject(hdc);

  Image i;
  i.fromHBITMAP(hbmp);

  DeleteObject((HGDIOBJ)hbmp);
*/

HBITMAP Image::toHBITMAP()
{
  ImageData* d = _d;
  if (d->width == 0 || d->height == 0) return NULL;

  // If the image format is XRGB32 or PRGB32 then it's very easy, we just 
  // create DIBSECTION and copy bits there. If image format is ARGB32 then
  // we need to premultiply the output. If image format is A8/I8 then we
  // create 32-bit DIBSECTION and copy there the alphas, this image will
  // be still usable when using functions like AlphaBlend(). 

  uint8_t* dstBits = NULL;
  uint32_t dstFormat = IMAGE_FORMAT_NULL;
  sysint_t dstStride = 0;

  switch (d->format)
  {
    case IMAGE_FORMAT_PRGB32:
    case IMAGE_FORMAT_ARGB32:
    case IMAGE_FORMAT_A8:
    case IMAGE_FORMAT_I8:
      dstFormat = IMAGE_FORMAT_PRGB32;
      break;
    case IMAGE_FORMAT_XRGB32:
      dstFormat = IMAGE_FORMAT_XRGB32;
      break;
    default:
      FOG_ASSERT_NOT_REACHED();
  }

  HBITMAP hBitmap = WinDibImageData::_createDibSection(
    d->width, d->height, d->format, &dstBits, &dstStride);
  if (hBitmap == NULL) return hBitmap;

  switch (d->format)
  {
    case IMAGE_FORMAT_PRGB32:
    case IMAGE_FORMAT_XRGB32:
    {
      rasterFuncs.dib.vblit_rect[IMAGE_FORMAT_PRGB32](
        dstBits, dstStride, d->first, d->stride, d->width, d->height, NULL);
      break;
    }

    case IMAGE_FORMAT_ARGB32:
    case IMAGE_FORMAT_A8:
    case IMAGE_FORMAT_I8:
    {
      RasterVBlitFullFn vblit_full = 
        rasterFuncs.getCompositeFuncs(OPERATOR_SRC, IMAGE_FORMAT_PRGB32)->vblit_full[d->format];
      RasterClosure closure;

      closure.dstPalette = NULL;
      closure.srcPalette = reinterpret_cast<const uint32_t*>(d->palette.getData());

      uint8_t* srcBits = d->first;
      sysint_t srcStride = d->stride;

      int w = d->width;

      for (sysint_t y = d->height; y; y--, dstBits += dstStride, srcBits += srcStride)
      {
        vblit_full(dstBits, srcBits, w, &closure);
      }
      break;
    }
  }

  return hBitmap;
}

// TODO: Not complete.
err_t Image::fromHBITMAP(HBITMAP hBitmap)
{
  if (hBitmap == NULL) return ERR_RT_INVALID_ARGUMENT;

  BITMAP bm;       // Source.
  BITMAPINFO dibi; // Target (for GetDIBits() function).
  GetObjectW(hBitmap, sizeof(BITMAP), &bm);

  uint32_t format = IMAGE_FORMAT_XRGB32;

  switch (bm.bmBitsPixel)
  {
    case 32:
    case 24:
      format = IMAGE_FORMAT_XRGB32;
      break;
  }

  err_t err = create(bm.bmWidth, bm.bmHeight, format);
  if (err) return err;

  if (bm.bmBits)
  {
    // DIB
    // TODO:
    return ERR_RT_NOT_IMPLEMENTED;
  }
  else 
  {
    // DDB
    HDC hdc = CreateCompatibleDC(NULL);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdc, (HGDIOBJ)hBitmap);

    uint8_t* destPixels = _d->first;
    sysint_t destStride = _d->stride;

    memset(&dibi, 0, sizeof(BITMAPINFO));
    dibi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    dibi.bmiHeader.biCompression = BI_RGB;
    dibi.bmiHeader.biWidth = bm.bmWidth;
    dibi.bmiHeader.biHeight = bm.bmHeight;
    dibi.bmiHeader.biBitCount = getDepth();
    dibi.bmiHeader.biPlanes = 1;
    // GetDIBits(hdc, hBitmap, 0, 1, NULL, &dibi, DIB_RGB_COLORS);

    uint32_t y;
    for (y = bm.bmHeight; y; y--, destPixels += destStride)
    {
      GetDIBits(hdc, hBitmap, y, 1, (LPVOID)destPixels, &dibi, DIB_RGB_COLORS);
    }

    SelectObject(hdc, (HGDIOBJ)hOldBitmap);
    DeleteDC(hdc);
  }

  return true;
}

HDC Image::getDC()
{
  if (_d->type != IMAGE_TYPE_WIN_DIB) return (HDC)NULL;
  if (detach() != ERR_OK) return (HDC)NULL;

  return reinterpret_cast<WinDibImageData*>(_d)->getDC();
}

void Image::releaseDC(HDC hDC)
{
  if (_d->type != IMAGE_TYPE_WIN_DIB) return;
  reinterpret_cast<WinDibImageData*>(_d)->releaseDC(hDC);
}

#endif // FOG_OS_WINDOWS

// ============================================================================
// [Fog::Image - ImageIO]
// ============================================================================

err_t Image::readFromFile(const String& fileName)
{
  ImageIO::DecoderDevice* decoder = NULL;
  err_t err = ImageIO::createDecoderForFile(fileName, &decoder);
  if (err != ERR_OK) return err;

  err = decoder->readImage(*this);

  delete decoder;
  return err;
}

err_t Image::readFromStream(Stream& stream)
{
  return readFromStream(stream, String());
}

err_t Image::readFromStream(Stream& stream, const String& extension)
{
  ImageIO::DecoderDevice* decoder = NULL;
  err_t err = ImageIO::createDecoderForStream(stream, extension, &decoder);
  if (err != ERR_OK) return err;

  err = decoder->readImage(*this);

  delete decoder;
  return err;
}

err_t Image::readFromBuffer(const ByteArray& buffer)
{
  return readFromBuffer(buffer.getData(), buffer.getLength());
}

err_t Image::readFromBuffer(const ByteArray& buffer, const String& extension)
{
  return readFromBuffer(buffer.getData(), buffer.getLength(), extension);
}

err_t Image::readFromBuffer(const void* buffer, sysuint_t size)
{
  Stream stream;
  stream.openBuffer((void*)buffer, size, STREAM_OPEN_READ);
  return readFromStream(stream, String());
}

err_t Image::readFromBuffer(const void* buffer, sysuint_t size, const String& extension)
{
  Stream stream;
  stream.openBuffer((void*)buffer, size, STREAM_OPEN_READ);
  return readFromStream(stream, extension);
}

err_t Image::writeToFile(const String& fileName) const
{
  Stream stream;

  err_t err = stream.openFile(fileName, 
    STREAM_OPEN_WRITE       |
    STREAM_OPEN_CREATE      |
    STREAM_OPEN_CREATE_PATH |
    STREAM_OPEN_TRUNCATE    );
  if (err) return err;

  TemporaryString<16> extension;
  if ((err = FileSystem::extractExtension(extension, fileName)) || (err = extension.lower()))
  {
    return err;
  }

  err = writeToStream(stream, extension);
  return err;
}

err_t Image::writeToStream(Stream& stream, const String& extension) const
{
  ImageIO::Provider* provider = ImageIO::getProviderByExtension(IMAGE_IO_DEVICE_ENCODER, extension);
  if (provider != NULL)
  {
    ImageIO::EncoderDevice* encoder = NULL;
    err_t err = provider->createDevice(IMAGE_IO_DEVICE_ENCODER, 
      reinterpret_cast<ImageIO::BaseDevice**>(&encoder));
    if (err) return err;

    encoder->attachStream(stream);
    err = encoder->writeImage(*this);

    delete encoder;
    return err;
  }

  return ERR_IMAGEIO_NO_ENCODER;
}

err_t Image::writeToBuffer(ByteArray& buffer, const String& extension) const
{
  Stream stream;
  err_t err = stream.openBuffer(buffer);
  if (err) return err;

  stream.seek(buffer.getLength(), STREAM_SEEK_SET);
  return writeToStream(stream, extension);
}

// ============================================================================
// [Fog::Image - Statics]
// ============================================================================

sysint_t Image::getStrideFromWidth(int width, uint32_t depth)
{
  sysint_t result = 0;

  switch (depth)
  {
    case  1: result = (width + 7) >> 3;
      break;

    case  4: result = (width + 1) >> 1;
      break;

    case  5:
    case  6:
    case  7:
    case  8:
      result = width;
      break;

    case 15:
    case 16:
      result = width *  2;
      // Overflow.
      if (result < width) return 0;
      break;

    default:
      FOG_ASSERT(depth > 0 && (depth & 0x7) == 0);
      result = width * (int)(depth >> 3);
      // Overflow.
      if (result < width) return 0;
      break;
  }

  // Align to 32-bit boudary.
  result += 3;
  result &= ~3;

  // Success.
  return result;
}

uint32_t Image::getDepthFromFormat(uint32_t format)
{
  switch (format)
  {
    case IMAGE_FORMAT_ARGB32:
    case IMAGE_FORMAT_PRGB32:
    case IMAGE_FORMAT_XRGB32:
      return 32;
    case IMAGE_FORMAT_A8:
    case IMAGE_FORMAT_I8:
      return 8;
    // Everything else is invalid.
    default:
      return 0;
  }
}

uint32_t Image::getBytesPerPixelFromFormat(uint32_t format)
{
  switch (format)
  {
    case IMAGE_FORMAT_ARGB32:
    case IMAGE_FORMAT_PRGB32:
    case IMAGE_FORMAT_XRGB32:
      return 4;
    case IMAGE_FORMAT_A8:
    case IMAGE_FORMAT_I8:
      return 1;
    // Everything else is invalid.
    default:
      return 0;
  }
}

ImageData* Image::_allocData(sysuint_t size)
{
  ImageData* d = (ImageData*)Memory::alloc(ImageData::getSizeFor(size));
  if (!d) return NULL;

  new (d) ImageData();
  d->data = (size != 0) ? d->buffer : NULL;
  d->first = d->data;
  return d;
}

ImageData* Image::_allocData(int w, int h, uint32_t format)
{
  ImageData* d;
  sysint_t stride;
  uint64_t size;

  FOG_ASSERT(w >= 0 && h >= 0);

  // Zero or negative coordinates are invalid.
  if (w == 0 || h == 0) return NULL;

  // Limit the image size to IMAGE_LIMITS.
  if (w >= IMAGE_MAX_WIDTH || h >= IMAGE_MAX_HEIGHT)
    return NULL;

  // Prevent multiply overflow (64 bit int type).
  size = (int64_t)w * h;
  if (size > INT_MAX) return NULL;

  // Calculate depth.
  uint32_t depth = Image::getDepthFromFormat(format);

  // Calculate stride.
  if ((stride = Image::getStrideFromWidth(w, depth)) == 0) return 0;

  // Try to alloc data.
  d = _allocData((sysuint_t)(h * stride));
  if (FOG_UNLIKELY(d == NULL)) return NULL;

  d->width = w;
  d->height = h;

  d->format = format;
  d->bytesPerPixel = (uint8_t)(depth >> 3);

  d->stride = stride;

  return d;
}

} // Fog namespace

// ============================================================================
// [Library Initializers]
// ============================================================================

FOG_INIT_DECLARE err_t fog_image_init(void)
{
  using namespace Fog;

  Image::_dnull.init();
  ImageData* d = Image::_dnull.instancep();
  d->refCount.init(1);
  d->flags = IMAGE_DATA_FLAG_SHARABLE;

  return ERR_OK;
}

FOG_INIT_DECLARE void fog_image_shutdown(void)
{
  using namespace Fog;

  Image::_dnull.instancep()->refCount.dec();
  Image::_dnull.destroy();
}
