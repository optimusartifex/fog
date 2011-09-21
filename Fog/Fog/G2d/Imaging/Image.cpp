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
#include <Fog/Core/Face/Face_C.h>
#include <Fog/Core/IO/FileSystem.h>
#include <Fog/Core/IO/MapFile.h>
#include <Fog/Core/Math/FloatBits.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/BSwap.h>
#include <Fog/Core/Memory/MemBufferTmp_p.h>
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/Core/Tools/StringTmp_p.h>
#include <Fog/G2d/Face/Face_Raster_C.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/ImageBits.h>
#include <Fog/G2d/Imaging/ImageCodec.h>
#include <Fog/G2d/Imaging/ImageCodecProvider.h>
#include <Fog/G2d/Imaging/ImageConverter.h>
#include <Fog/G2d/Imaging/ImageDecoder.h>
#include <Fog/G2d/Imaging/ImageEffect.h>
#include <Fog/G2d/Imaging/ImageEncoder.h>
#include <Fog/G2d/Painting/RasterFiller_p.h>
#include <Fog/G2d/Painting/RasterScanline_p.h>
#include <Fog/G2d/Painting/Rasterizer_p.h>
#include <Fog/G2d/Render/RenderApi_p.h>
#include <Fog/G2d/Render/RenderConstants_p.h>
#include <Fog/G2d/Render/RenderStructs_p.h>
#include <Fog/G2d/Render/RenderUtil_p.h>
#include <Fog/G2d/Tools/ColorAnalyzer_p.h>
#include <Fog/G2d/Tools/Reduce_p.h>

#if defined(FOG_OS_WINDOWS)
# include <Fog/G2d/Imaging/WinImage_p.h>
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
  reference.init(1);
  locked = 0;

  type = IMAGE_BUFFER_MEMORY;
  flags = NO_FLAGS;
  format = IMAGE_FORMAT_NULL;
  bytesPerPixel = 0;

  colorKey = 0xFFFFFFFF;
#if FOG_ARCH_BITS >= 64
  padding = 0;
#endif // FOG_ARCH_BITS

  size.reset();

  data = NULL;
  first = NULL;
  stride = 0;
}

ImageData::~ImageData()
{
}

ImageData* ImageData::addRef() const
{
  if (locked) return NULL;
  return refAlways();
}

void ImageData::deref()
{
  if (reference.deref())
  {
    bool wasStatic = (flags & VAR_FLAG_STATIC) != 0;

    // FATAL: Image dereferenced during painting.
    if (locked)
    {
      Debug::failFunc("Fog::ImageData", "deref", "Image dereferenced during painting.\n");
    }

    this->~ImageData();
    if (!wasStatic) MemMgr::free(this);
  }
}

ImageData* ImageData::clone() const
{
  if (!size.isValid()) return Image::_dnull->refAlways();

  ImageData* newd = Image::_dalloc(size, format);
  if (FOG_IS_NULL(newd)) return NULL;

  _g2d_render.getCopyRectFunc(newd->format)(
    newd->first, newd->stride,
    this->first, this->stride,
    newd->size.w, newd->size.h, NULL);
  newd->palette = this->palette;

  return newd;
}

void* ImageData::getHandle()
{
  return NULL;
}

void ImageData::paletteModified(const Range& range)
{
  // NOP.
}

// ============================================================================
// [Fog::Image - Construction / Destruction]
// ============================================================================

Image::Image() :
  _d(_dnull->refAlways())
{
}

Image::Image(const Image& other) :
  _d(other._d->addRef())
{
  if (FOG_UNLIKELY(_d == NULL)) _d = _dnull->refAlways();
}

Image::Image(const SizeI& size, uint32_t format, uint32_t bufferType) :
  _d(_dnull->refAlways())
{
  create(size, format, bufferType);
}

Image::~Image()
{
  _d->deref();
}

// ============================================================================
// [Fog::Image - Sharing]
// ============================================================================

err_t Image::_detach()
{
  ImageData* d = _d;
  if (d == &_dnull) return ERR_OK;

  if (d->stride == 0)
  {
    atomicPtrXchg(&_d, _dnull->refAlways())->deref();
    return ERR_OK;
  }

  if (d->reference.get() > 1 || (d->flags & VAR_FLAG_READ_ONLY))
  {
    ImageData* newd = d->clone();
    if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

    atomicPtrXchg(&_d, newd)->deref();
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::Image - Reset]
// ============================================================================

void Image::reset()
{
  atomicPtrXchg(&_d, _dnull->refAlways())->deref();
}

// ============================================================================
// [Fog::Image - Create / Adopt]
// ============================================================================

err_t Image::create(const SizeI& size, uint32_t format, uint32_t bufferType)
{
  err_t err = ERR_OK;

  ImageData* d = _d;
  ImageData* newd = NULL;

  // Detect invalid arguments.
  if (FOG_UNLIKELY(format >= IMAGE_FORMAT_COUNT))
  {
    err = ERR_RT_INVALID_ARGUMENT;
    goto _Fail;
  }

  // Don't create an image with invalid size.
  if ((uint)size.w >= IMAGE_MAX_WIDTH || (uint)size.h >= IMAGE_MAX_HEIGHT)
  {
    err = ERR_IMAGE_INVALID_SIZE;
    goto _Fail;
  }

  // If the size of this image is the same, and all other members pass
  // the @c Image::create() arguments then we are done.
  if (d->size == size && d->format == format && (d->type == bufferType || bufferType == IMAGE_BUFFER_IGNORE) && isDetached())
  {
    return ERR_OK;
  }

  switch (bufferType)
  {
    case IMAGE_BUFFER_MEMORY:
    case IMAGE_BUFFER_IGNORE:
      // Create new memory image (the default).
      newd = Image::_dalloc(size, format);
      if (newd == NULL)
      {
        err = ERR_RT_OUT_OF_MEMORY;
        goto _Fail;
      }
      break;

#if defined(FOG_OS_WINDOWS)
    case IMAGE_BUFFER_WIN_DIB:
      err = WinDibImageData::_createDibImage(size, format, &newd);
      if (FOG_IS_ERROR(err)) goto _Fail;
      break;
#endif // FOG_OS_WINDOWS

    default:
      err = ERR_RT_INVALID_ARGUMENT;
      goto _Fail;
  }

  FOG_ASSERT(newd != NULL);
  atomicPtrXchg(&_d, newd)->deref();
  return err;

_Fail:
  reset();
  return err;
}

err_t Image::adopt(const ImageBits& imageBits, uint32_t adoptFlags)
{
  ImageData* d = _d;

  if (!imageBits.isValid())
  {
    reset();
    return ERR_RT_INVALID_ARGUMENT;
  }

  if (!isDetached() || (!(d->flags & VAR_FLAG_STATIC) && !ImageData_containsEmbeddedBuffer(d)))
  {
    ImageData* newd = _dalloc(0);
    if (FOG_IS_NULL(newd))
    {
      reset();
      return ERR_RT_OUT_OF_MEMORY;
    }

    atomicPtrXchg(&_d, newd)->deref();
    d = newd;
  }

  // Fill basic variables.
  d->format = imageBits.format;
  d->bytesPerPixel = ImageFormatDescription::getByFormat(imageBits.format).getBytesPerPixel();

  d->colorKey = 0xFFFFFFFF;
  d->size = imageBits.size;

  d->data = (uint8_t*)imageBits.data;
  d->first = (uint8_t*)imageBits.data;
  d->stride = imageBits.stride;

  // Bottom-to-top data?
  if (adoptFlags & IMAGE_ADOPT_REVERSED)
  {
    d->stride = -d->stride;
    d->first += (d->size.h - 1) * d->stride;
  }

  // Read only memory ?
  if ((adoptFlags & IMAGE_ATOPT_READ_ONLY) != 0)
    d->flags |= VAR_FLAG_READ_ONLY;
  else
    d->flags &= ~VAR_FLAG_READ_ONLY;

  return ERR_OK;
}

// ============================================================================
// [Fog::Image - Modified]
// ============================================================================

void Image::_modified()
{
  // TODO:
}

// ============================================================================
// [Fog::Image - Set]
// ============================================================================

err_t Image::set(const Image& other)
{
  if (_d == other._d) return ERR_OK;

  if (other.isEmpty())
  {
    atomicPtrXchg(&_d, _dnull->refAlways())->deref();
    return ERR_OK;
  }

  atomicPtrXchg(&_d, other._d->refAlways())->deref();
  return ERR_OK;
}

err_t Image::set(const Image& other, const RectI& area)
{
  if (other.isEmpty())
  {
    reset();
    return ERR_OK;
  }

  int x0 = area.x;
  int y0 = area.y;
  int x1 = x0 + area.w;
  int y1 = y0 + area.h;

  if (x0 < 0) x0 = 0;
  if (y0 < 0) y0 = 0;
  if (x1 > other.getWidth()) x1 = other.getWidth();
  if (y1 > other.getHeight()) y1 = other.getHeight();

  if (x0 >= x1 || y0 >= y1)
    return ERR_RT_INVALID_ARGUMENT;
  if (x0 == 0 && y0 == 0 && x1 == getWidth() && y1 == getHeight())
    return set(other);

  int w = x1 - x0;
  int h = y1 - y0;

  ImageData* selfCopy = NULL;
  if (_d == other._d) selfCopy = other._d->refAlways();

  FOG_RETURN_ON_ERROR(create(SizeI(w, h), other.getFormat()));

  uint8_t* dstCur = _d->first;
  uint8_t* srcCur = other._d->first;

  ssize_t dstStride = _d->stride;
  ssize_t srcStride = other._d->stride;

  srcCur += (uint)y0 * srcStride + (uint)x0 * other.getBytesPerPixel();
  size_t bpl = w * other.getBytesPerPixel();

  for (ssize_t i = 0; i < h; i++, dstCur += dstStride, srcCur += srcStride)
  {
    MemOps::copy(dstCur, srcCur, bpl);
  }

  if (selfCopy) selfCopy->deref();
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

  uint32_t type = getType();

  if (getSize() == other.getSize() && type == other.getType())
  {
    if ((type != IMAGE_BUFFER_MEMORY && (getFormat()        == other.getFormat()       )) ||
        (type == IMAGE_BUFFER_MEMORY && (getBytesPerPixel() == other.getBytesPerPixel())) )
    {
      const ImageData* _o = other._d;
      _g2d_render.getCopyRectFunc(_d->format)(
        _d->first, _d->stride,
        _o->first, _o->stride,
        _d->size.w, _d->size.h, NULL);

      _d->format = _o->format;
      _d->palette = other._d->palette;
      return ERR_OK;
    }
  }

  ImageData* newd = other._d->clone();
  if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

  atomicPtrXchg(&_d, newd)->deref();
  return ERR_OK;
}

// ============================================================================
// [Fog::Image - Conversion]
// ============================================================================

err_t Image::convert(uint32_t format)
{
  if (isEmpty()) return ERR_OK;

  ImageData* d = _d;

  uint32_t sourceFormat = _d->format;
  uint32_t targetFormat = format;

  if (FOG_UNLIKELY(targetFormat >= IMAGE_FORMAT_COUNT))
    return ERR_RT_INVALID_ARGUMENT;
  if (targetFormat == sourceFormat)
    return ERR_OK;
  if (targetFormat == IMAGE_FORMAT_I8)
    return convertTo8BitDepth();

  int w = d->size.w;
  int h = d->size.h;
  int y;

  const G2dRenderApi::_FuncsCompositeCore* funcs = _g2d_render.get_FuncsCompositeCore(targetFormat, COMPOSITE_SRC);
  RenderVBlitLineFunc blitLine = funcs->vblit_line[sourceFormat];

  RenderClosure closure;
  closure.ditherOrigin.reset();
  closure.palette = _d->palette._d;
  closure.data = NULL;

  // If the image target and source depths are the same, we do not need to
  // create the different image data (image have not to be shared and read-only).
  if (getDepth() == ImageFormatDescription::getByFormat(format).getDepth() && isDetached() && !isReadOnly())
  {
    uint8_t* dstCur = _d->first;
    ssize_t dstStride = _d->stride;

    for (y = 0; y < h; y++, dstCur += dstStride)
      blitLine(dstCur, dstCur, w, &closure);

    _d->format = targetFormat;
    return ERR_OK;
  }
  else
  {
    ImageData* newd = _dalloc(d->size, targetFormat);
    if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

    uint8_t* dstCur = newd->first;
    uint8_t* srcCur = _d->first;

    ssize_t dstStride = newd->stride;
    ssize_t srcStride = _d->stride;

    for (y = 0; y < h; y++, dstCur += dstStride, srcCur += srcStride)
      blitLine(dstCur, srcCur, w, &closure);

    atomicPtrXchg(&_d, newd)->deref();
    return ERR_OK;
  }
}

err_t Image::forceFormat(uint32_t format)
{
  if (isEmpty()) return ERR_OK;

  if (FOG_UNLIKELY(format >= (uint)IMAGE_FORMAT_COUNT))
    return ERR_RT_INVALID_ARGUMENT;
  if (FOG_UNLIKELY(getDepth() != ImageFormatDescription::getByFormat(format).getDepth()))
    return ERR_RT_INVALID_ARGUMENT;
  if (_d->format == format)
    return ERR_OK;

  FOG_RETURN_ON_ERROR(detach());
  _d->format = format;

  _modified();
  return ERR_OK;
}

static err_t Image_convertTo8BPC(Image* self)
{
  uint32_t srcFormat = self->getFormat();
  uint32_t dstFormat = srcFormat;

  // ${IMAGE_FORMAT:BEGIN}
  switch (srcFormat)
  {
    case IMAGE_FORMAT_PRGB32:
    case IMAGE_FORMAT_PRGB64:
      dstFormat = IMAGE_FORMAT_PRGB32;
      break;

    case IMAGE_FORMAT_XRGB32:
    case IMAGE_FORMAT_RGB24:
      break;

    case IMAGE_FORMAT_RGB48:
      dstFormat = IMAGE_FORMAT_XRGB32;
      break;

    case IMAGE_FORMAT_A8:
    case IMAGE_FORMAT_A16:
      dstFormat = IMAGE_FORMAT_A8;
      break;

    case IMAGE_FORMAT_I8:
      break;
  }
  // ${IMAGE_FORMAT:END}

  return (dstFormat != srcFormat) ? self->convert(dstFormat) : ERR_OK;
}

// ============================================================================
// [Fog::Image - Quantization]
// ============================================================================

err_t Image::convertTo8BitDepth()
{
  if (isEmpty()) return ERR_OK;

  FOG_RETURN_ON_ERROR(Image_convertTo8BPC(this));
  if (getFormat() == IMAGE_FORMAT_I8 || getFormat() == IMAGE_FORMAT_A8) return ERR_OK;

  Image i;
  FOG_RETURN_ON_ERROR(i.create(getSize(), IMAGE_FORMAT_I8));

  uint8_t* dstCur = i._d->first;
  ssize_t dstStride = i._d->stride;

  uint8_t* srcCur = _d->first;
  ssize_t srcStride = _d->stride;

  int w = getWidth();
  int h = getHeight();

  int x;
  int y;

  Reduce reduce;
  if (reduce.analyze(*this, true))
  {
    FOG_RETURN_ON_ERROR(i.setPalette(reduce.getPalette()));

    dstStride -= w * 1;
    srcStride -= w * getBytesPerPixel();

    uint32_t mask = reduce.getMask();

    for (y = 0; y < h; y++, dstCur += dstStride, srcCur += srcStride)
    {
      switch (getFormat())
      {
        case IMAGE_FORMAT_PRGB32:
        case IMAGE_FORMAT_XRGB32:
          for (x = 0; x < w; x++, dstCur += 1, srcCur += 4)
          {
            Face::p32 pix0p;

            Face::p32Load4aNative(pix0p, srcCur);
            Face::p32And(pix0p, pix0p, mask);
            dstCur[0] = reduce.traslate(pix0p);
          }
          break;

        case IMAGE_FORMAT_RGB24:
          for (x = 0; x < w; x++, dstCur += 1, srcCur += 3)
          {
            Face::p32 pix0p;

            Face::p32Load3bNative(pix0p, srcCur);
            Face::p32And(pix0p, pix0p, mask);
            dstCur[0] = reduce.traslate(pix0p);
          }
          break;

        default:
          return ERR_RT_INVALID_STATE;
      }
    }
  }
  else
  {
    ImagePalette pallette = ImagePalette::fromColorCube(6, 6, 6);
    FOG_RETURN_ON_ERROR(i.setPalette(pallette));

    ImageDither8Params params;
    params.rCount = 6;
    params.gCount = 6;
    params.bCount = 6;
    params.transposeTable = NULL;
    params.transposeTableLength = 0;

    ImageConverter converter;
    FOG_RETURN_ON_ERROR(converter.createDithered8(params, i.getFormatDescription(), NULL, NULL));

    converter.blitRect(dstCur, dstStride, srcCur, srcStride, w, h, PointI(0, 0));
  }

  return set(i);
}

err_t Image::convertTo8BitDepth(const ImagePalette& pal)
{
  if (isEmpty()) return ERR_OK;
  FOG_RETURN_ON_ERROR(Image_convertTo8BPC(this));

  int w = getWidth();
  int h = getHeight();

  Image i;
  FOG_RETURN_ON_ERROR(i.create(SizeI(w, h), IMAGE_FORMAT_I8));
  FOG_RETURN_ON_ERROR(i.setPalette(pal));

  uint8_t* dstCur = i._d->first;
  ssize_t dstStride = i._d->stride - w * i._d->bytesPerPixel;

  uint8_t* srcCur = _d->first;
  ssize_t srcStride = _d->stride - w * _d->bytesPerPixel;

  int y, x;

  // ${IMAGE_FORMAT:BEGIN}
  switch (getFormat())
  {
    case IMAGE_FORMAT_PRGB32:
    case IMAGE_FORMAT_XRGB32:
    {
      for (y = 0; y < h; y++, dstCur += dstStride, srcCur += srcStride)
      {
        for (x = 0; x < w; x++, dstCur += 1, srcCur += 4)
        {
          Face::p32 pix0p;
          Face::p32Load4aNative(pix0p, srcCur);

          dstCur[0] = pal.findRgb(Face::p32PBB2AsU32(pix0p),
                                  Face::p32PBB1AsU32(pix0p),
                                  Face::p32PBB0AsU32(pix0p));
        }
      }
      break;
    }

    case IMAGE_FORMAT_RGB24:
    {
      for (y = 0; y < h; y++, dstCur += dstStride, srcCur += srcStride)
      {
        for (x = 0; x < w; x++, dstCur += 1, srcCur += 3)
        {
          dstCur[0] = pal.findRgb(srcCur[PIXEL_RGB24_POS_R],
                                  srcCur[PIXEL_RGB24_POS_G],
                                  srcCur[PIXEL_RGB24_POS_B]);
        }
      }
      break;
    }

    case IMAGE_FORMAT_I8:
    {
      // Build a lookup table.
      uint8_t table[256];

      uint srcPalLength = (uint)_d->palette.getLength();
      const Argb32* srcPalData = _d->palette.getData();

      for (y = 0; y < (int)srcPalLength; y++)
      {
        Argb32 c0 = srcPalData[y];
        table[y] = pal.findRgb(c0.getRed(), c0.getGreen(), c0.getBlue());
      }
      for (; y < 256; y++) table[y] = (uint8_t)(uint)y;

      for (y = 0; y < h; y++, dstCur += dstStride, srcCur += srcStride)
      {
        for (x = 0; x < w; x++, dstCur += 1, srcCur += 1)
        {
          dstCur[0] = table[srcCur[0]];
        }
      }
      break;
    }

    case IMAGE_FORMAT_PRGB64:
      FOG_RETURN_ON_ERROR(convert(IMAGE_FORMAT_PRGB32));
      return convertTo8BitDepth(pal);

    case IMAGE_FORMAT_RGB48:
      FOG_RETURN_ON_ERROR(convert(IMAGE_FORMAT_XRGB32));
      return convertTo8BitDepth(pal);

    default:
      FOG_ASSERT_NOT_REACHED();
  }
  // ${IMAGE_FORMAT:END}

  return set(i);
}

err_t Image::setPalette(const ImagePalette& palette)
{
  if (isEmpty()) return ERR_RT_INVALID_STATE;

  FOG_RETURN_ON_ERROR(detach());
  FOG_RETURN_ON_ERROR(_d->palette.setData(palette));

  _d->paletteModified(Range(0, palette.getLength()));
  _modified();

  return ERR_OK;
}

err_t Image::setPalette(const Range& range, const Argb32* pal)
{
  if (isEmpty()) return ERR_RT_INVALID_STATE;

  FOG_RETURN_ON_ERROR(detach());
  FOG_RETURN_ON_ERROR(_d->palette.setData(range, pal));

  // Should be filtered by Palette::setData()
  FOG_ASSERT(range.getStart() < 256);

  _d->paletteModified(
    Range(range.getStart(), Math::min<ssize_t>(256, range.getEnd())));
  _modified();

  return ERR_OK;
}

// ============================================================================
// [Fog::Image - Alpha Distribution]
// ============================================================================

uint32_t Image::getAlphaDistribution() const
{
  uint32_t result;

  ColorAnalyzer::AnalyzerFunc analyzer = NULL;
  int aPos = 0;
  int inc = 0;

  switch (getFormat())
  {
    case IMAGE_FORMAT_PRGB32:
      analyzer = ColorAnalyzer::analyzeAlpha32; aPos = PIXEL_ARGB32_POS_A; inc = 4;
      break;

    case IMAGE_FORMAT_A8:
      analyzer = ColorAnalyzer::analyzeAlpha32; aPos = 0; inc = 1;
      break;

    case IMAGE_FORMAT_PRGB64:
      analyzer = ColorAnalyzer::analyzeAlpha64; aPos = PIXEL_ARGB64_POS_A; inc = 8;
      break;

    case IMAGE_FORMAT_A16:
      analyzer = ColorAnalyzer::analyzeAlpha64; aPos = 0; inc = 2;
      break;
  }

  if (analyzer != NULL)
    result = analyzer(getFirst(), getStride(), getWidth(), getHeight(), aPos, inc);
  else
    result = ALPHA_DISTRIBUTION_FULL;

  return result;
}

// ============================================================================
// [Fog::Image - Invert]
// ============================================================================

// Non premultiplied:
//
//   C'  = (1 - C)
//   A'  = (1 - A)
//
// Premultiplied:
//
//   Ca' = (1 - Ca/A) * (1 - A)
//   A'  = (1 - A)
//
// If the image format is premultiplied, but alpha is not modified then the
// formula looks simpler:
//
//   Ca' = A - Ca
//   A'  = A
err_t Image::invert(Image& dst, const Image& src, uint32_t channels)
{
  if (channels >= 16) return ERR_RT_INVALID_ARGUMENT;

  uint32_t format = src.getFormat();
  const ImageFormatDescription& desc = src.getFormatDescription();

  if ((desc.getComponentMask() & IMAGE_COMPONENT_ALPHA) == 0) channels &= COLOR_CHANNEL_RGB;
  if ((desc.getComponentMask() & IMAGE_COMPONENT_RGB  ) == 0) channels &= COLOR_CHANNEL_ALPHA;

  // First check for some invertion flags in source image format.
  if (src.isEmpty() || channels == 0) return dst.set(src);

  // The destination image can share data with the source image.
  FOG_RETURN_ON_ERROR(dst._d != src._d ? dst.create(src.getSize(), format) : dst.detach());

  // Prepare data.
  ImageData* dst_d = dst._d;
  ImageData* src_d = src._d;

  int w = dst_d->size.w;
  int h = dst_d->size.h;

  int x;
  int y;

  uint8_t* dstPixels = dst_d->first;
  ssize_t dstStride = dst_d->stride - w * dst_d->bytesPerPixel;

  const uint8_t* srcPixels = src_d->first;
  ssize_t srcStride = src_d->stride - w * src_d->bytesPerPixel;

  // ${IMAGE_FORMAT:BEGIN}
  switch (format)
  {
    case IMAGE_FORMAT_I8:
    {
      FOG_RETURN_ON_ERROR(dst._d->palette.detach());

      w = (int)dst._d->palette.getLength();
      h = 1;

      dstPixels = reinterpret_cast<uint8_t*>(dst._d->palette.getDataX());
      srcPixels = reinterpret_cast<const uint8_t*>(src._d->palette.getData());

      dstStride = 0;
      srcStride = 0;
      // ... Fall through ...
    }

    case IMAGE_FORMAT_PRGB32:
    case IMAGE_FORMAT_XRGB32:
    {
      Face::p32 mask = 0;

      if (channels & COLOR_CHANNEL_ALPHA) Face::p32Or(mask, mask, PIXEL_ARGB32_MASK_A);
      if (channels & COLOR_CHANNEL_RED  ) Face::p32Or(mask, mask, PIXEL_ARGB32_MASK_A);
      if (channels & COLOR_CHANNEL_GREEN) Face::p32Or(mask, mask, PIXEL_ARGB32_MASK_A);
      if (channels & COLOR_CHANNEL_BLUE ) Face::p32Or(mask, mask, PIXEL_ARGB32_MASK_A);

      if (format != IMAGE_FORMAT_PRGB32)
      {
        for (y = 0; y < h; y++, dstPixels += dstStride, srcPixels += srcStride)
        {
          for (x = 0; x < w; x++, dstPixels += 4, srcPixels += 4)
          {
            Face::p32 pix0p;

            Face::p32Load4aNative(pix0p, srcPixels);
            Face::p32Xor(pix0p, pix0p, mask);
            Face::p32Store4aNative(dstPixels, pix0p);
          }
        }

        if (format == IMAGE_FORMAT_I8)
        {
          dst._d->paletteModified(Range(0, dst._d->palette.getLength()));
        }
      }
      else if ((channels & COLOR_CHANNEL_ALPHA) == 0)
      {
        // If alpha is not inverted, the fast-path formula might be used.
        for (y = 0; y < h; y++, dstPixels += dstStride, srcPixels += srcStride)
        {
          for (x = 0; x < w; x++, dstPixels += 4, srcPixels += 4)
          {
            Face::p32 pix0p;
            Face::p32 pix0a;

            Face::p32Load4aNative(pix0p, srcPixels);
            Face::p32ExpandPBBFromPBB3(pix0a, pix0p);
            Face::p32Sub(pix0a, pix0a, pix0p);
            Face::p32And(pix0p, pix0p, ~mask);
            Face::p32And(pix0a, pix0a, mask);
            Face::p32Or(pix0p, pix0p, pix0a);
            Face::p32Store4aNative(dstPixels, pix0p);
          }
        }
      }
      else
      {
        // Alpha is inverted: Demultiply, invert, and premultiply.
        for (y = 0; y < h; y++, dstPixels += dstStride, srcPixels += srcStride)
        {
          for (x = 0; x < w; x++, dstPixels += 4, srcPixels += 4)
          {
            Face::p32 pix0p;

            Face::p32Load4aNative(pix0p, srcPixels);
            Face::p32ARGB32FromPRGB32(pix0p, pix0p);
            Face::p32Xor(pix0p, pix0p, mask);
            Face::p32PRGB32FromARGB32(pix0p, pix0p);
            Face::p32Store4aNative(dstPixels, pix0p);
          }
        }
      }
      break;
    }

    case IMAGE_FORMAT_RGB24:
    {
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
      uint8_t m0 = (channels & COLOR_CHANNEL_BLUE ) ? 0xFF : 0x00;
      uint8_t m1 = (channels & COLOR_CHANNEL_GREEN) ? 0xFF : 0x00;
      uint8_t m2 = (channels & COLOR_CHANNEL_RED  ) ? 0xFF : 0x00;
#else
      uint8_t m0 = (channels & COLOR_CHANNEL_RED  ) ? 0xFF : 0x00;
      uint8_t m1 = (channels & COLOR_CHANNEL_GREEN) ? 0xFF : 0x00;
      uint8_t m2 = (channels & COLOR_CHANNEL_BLUE ) ? 0xFF : 0x00;
#endif // FOG_BYTE_ORDER

      for (y = 0; y < h; y++, dstPixels += dstStride, srcPixels += srcStride)
      {
        for (x = 0; x < w; x++, dstPixels += 3, srcPixels += 3)
        {
          dstPixels[0] = srcPixels[0] ^ m0;
          dstPixels[1] = srcPixels[1] ^ m1;
          dstPixels[2] = srcPixels[2] ^ m2;
        }
      }
      break;
    }

    case IMAGE_FORMAT_A8:
    {
      for (y = 0; y < h; y++, dstPixels += dstStride, srcPixels += srcStride)
      {
        for (x = 0; x < w; x++, dstPixels += 1, srcPixels += 1)
        {
          uint8_t pix0p = srcPixels[0];
          pix0p ^= 0xFF;
          dstPixels[0] = pix0p;
        }
      }
      break;
    }

    case IMAGE_FORMAT_PRGB64:
    {
      Face::p64 mask;
      Face::p64Clear(mask);

      if (channels & COLOR_CHANNEL_ALPHA) Face::p64Or(mask, mask, Face::p64FromU64(PIXEL_ARGB64_MASK_A));
      if (channels & COLOR_CHANNEL_RED  ) Face::p64Or(mask, mask, Face::p64FromU64(PIXEL_ARGB64_MASK_R));
      if (channels & COLOR_CHANNEL_GREEN) Face::p64Or(mask, mask, Face::p64FromU64(PIXEL_ARGB64_MASK_G));
      if (channels & COLOR_CHANNEL_BLUE ) Face::p64Or(mask, mask, Face::p64FromU64(PIXEL_ARGB64_MASK_B));

      if ((channels & COLOR_CHANNEL_ALPHA) == 0)
      {
        // If alpha is not inverted, the fast-path formula might be used.
        Face::p64 maski;
        Face::p64Neg(maski, mask);

        for (y = 0; y < h; y++, dstPixels += dstStride, srcPixels += srcStride)
        {
          for (x = 0; x < w; x++, dstPixels += 8, srcPixels += 8)
          {
            Face::p64 pix0p;
            Face::p64 pix0a;

            Face::p64Load8aNative(pix0p, srcPixels);
            Face::p64ExpandPWWFromPWW3(pix0p, pix0p);

            Face::p64Sub(pix0a, pix0a, pix0p);
            Face::p64And(pix0p, pix0p, maski);
            Face::p64And(pix0a, pix0a, mask);
            Face::p64Or(pix0p, pix0p, pix0a);
            Face::p64Store8aNative(dstPixels, pix0p);
          }
        }
      }
      else
      {
        // Alpha is inverted: Demultiply, invert, and premultiply.
        for (y = 0; y < h; y++, dstPixels += dstStride, srcPixels += srcStride)
        {
          for (x = 0; x < w; x++, dstPixels += 8, srcPixels += 8)
          {
            Face::p64 pix0p;

            Face::p64Load8aNative(pix0p, srcPixels);
            Face::p64ARGB64FromPRGB64(pix0p, pix0p);
            Face::p64Xor(pix0p, pix0p, mask);
            Face::p64PRGB64FromARGB64(pix0p, pix0p);
            Face::p64Store8aNative(dstPixels, pix0p);
          }
        }
      }
      break;
    }

    case IMAGE_FORMAT_RGB48:
    {
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
      uint16_t m0 = (channels & COLOR_CHANNEL_BLUE ) ? 0xFFFF : 0x0000;
      uint16_t m1 = (channels & COLOR_CHANNEL_GREEN) ? 0xFFFF : 0x0000;
      uint16_t m2 = (channels & COLOR_CHANNEL_RED  ) ? 0xFFFF : 0x0000;
#else
      uint16_t m0 = (channels & COLOR_CHANNEL_RED  ) ? 0xFFFF : 0x0000;
      uint16_t m1 = (channels & COLOR_CHANNEL_GREEN) ? 0xFFFF : 0x0000;
      uint16_t m2 = (channels & COLOR_CHANNEL_BLUE ) ? 0xFFFF : 0x0000;
#endif // FOG_BYTE_ORDER

      for (y = 0; y < h; y++, dstPixels += dstStride, srcPixels += srcStride)
      {
        for (x = 0; x < w; x++, dstPixels += 6, srcPixels += 6)
        {
          ((uint16_t*)dstPixels)[0] = ((const uint16_t*)srcPixels)[0] ^ m0;
          ((uint16_t*)dstPixels)[1] = ((const uint16_t*)srcPixels)[1] ^ m1;
          ((uint16_t*)dstPixels)[2] = ((const uint16_t*)srcPixels)[2] ^ m2;
        }
      }
      break;
    }

    case IMAGE_FORMAT_A16:
    {
      for (y = 0; y < h; y++, dstPixels += dstStride, srcPixels += srcStride)
      {
        for (x = 0; x < w; x++, dstPixels += 2, srcPixels += 2)
        {
          uint16_t pix0p = ((const uint16_t*)srcPixels)[0];
          pix0p ^= 0xFFFF;
          ((uint16_t*)dstPixels)[0] = pix0p;
        }
      }
      break;
    }
  }
  // ${IMAGE_FORMAT:END}

  return ERR_OK;
}

// ============================================================================
// [Fog::Image - Mirror]
// ============================================================================

typedef void (FOG_FASTCALL *MirrorFunc)(uint8_t*, uint8_t*, int);

template<int SIZE>
static void FOG_FASTCALL _MirrorCopySrcIsNotDst(uint8_t* dst, uint8_t* src, int w)
{
  MemOps::copy(dst, src, w * SIZE);
}

template<int SIZE>
static void FOG_FASTCALL _MirrorFlipSrcIsNotDst(uint8_t* dst, uint8_t* src, int w)
{
  src += w * SIZE - SIZE;
  for (int x = 0; x < w; x++, dst += SIZE, src -= SIZE) MemOps::copy_s<SIZE>(dst, src);
}

template<int SIZE>
static void FOG_FASTCALL _MirrorCopySrcIsDst(uint8_t* dst, uint8_t* src, int w)
{
  MemOps::xchg(dst, src, w * SIZE);
}

template<int SIZE>
static void FOG_FASTCALL _MirrorFlipSrcIsDst(uint8_t* dst, uint8_t* src, int w)
{
  int x = w;
  if (src == dst) x >>= 1;

  src += w * SIZE - SIZE;
  for (; x; x--, dst += SIZE, src -= SIZE) MemOps::xchg_s<SIZE>(dst, src);
}

static const MirrorFunc _MirrorFuncsCopySrcIsNotDst[] =
{
  NULL,
  _MirrorCopySrcIsNotDst<1>,
  _MirrorCopySrcIsNotDst<2>,
  _MirrorCopySrcIsNotDst<3>,
  _MirrorCopySrcIsNotDst<4>,
  NULL,
  _MirrorCopySrcIsNotDst<6>,
  NULL,
  _MirrorCopySrcIsNotDst<8>
};

static const MirrorFunc _MirrorFuncsFlipSrcIsNotDst[] =
{
  NULL,
  _MirrorFlipSrcIsNotDst<1>,
  _MirrorFlipSrcIsNotDst<2>,
  _MirrorFlipSrcIsNotDst<3>,
  _MirrorFlipSrcIsNotDst<4>,
  NULL,
  _MirrorFlipSrcIsNotDst<6>,
  NULL,
  _MirrorFlipSrcIsNotDst<8>
};

static const MirrorFunc _MirrorFuncsCopySrcIsDst[] =
{
  NULL,
  _MirrorCopySrcIsDst<1>,
  _MirrorCopySrcIsDst<2>,
  _MirrorCopySrcIsDst<3>,
  _MirrorCopySrcIsDst<4>,
  NULL,
  _MirrorCopySrcIsDst<6>,
  NULL,
  _MirrorCopySrcIsDst<8>
};

static const MirrorFunc _MirrorFuncsFlipSrcIsDst[] =
{
  NULL,
  _MirrorFlipSrcIsDst<1>,
  _MirrorFlipSrcIsDst<2>,
  _MirrorFlipSrcIsDst<3>,
  _MirrorFlipSrcIsDst<4>,
  NULL,
  _MirrorFlipSrcIsDst<6>,
  NULL,
  _MirrorFlipSrcIsDst<8>
};

err_t Image::mirror(Image& dst, const Image& src, uint32_t mirrorMode)
{
  if (src.isEmpty() || mirrorMode == 0) return dst.set(src);
  if (mirrorMode >= 4) return ERR_RT_INVALID_ARGUMENT;

  uint32_t format = src.getFormat();

  FOG_RETURN_ON_ERROR(dst._d != src._d
    ? dst.create(src.getSize(), format)
    : dst.detach());

  if (format == IMAGE_FORMAT_I8)
    FOG_RETURN_ON_ERROR(dst.setPalette(src.getPalette()));

  ImageData* dst_d = dst._d;
  ImageData* src_d = src._d;

  ssize_t dstStride = dst_d->stride;
  ssize_t srcStride = src_d->stride;

  uint8_t* dstPixels = dst_d->first;
  uint8_t* srcPixels = src_d->first;

  int w = dst_d->size.w;
  int h = dst_d->size.h;

  int bytesPerPixel = src.getBytesPerPixel();
  MirrorFunc func;

  switch (mirrorMode)
  {
    case IMAGE_MIRROR_VERTICAL:
      srcPixels += srcStride * ((ssize_t)h - 1);
      srcStride = -srcStride;

      if (dst_d != src_d)
      {
        func = _MirrorFuncsCopySrcIsNotDst[bytesPerPixel];
      }
      else
      {
        func = _MirrorFuncsCopySrcIsDst[bytesPerPixel];
        h >>= 1;
      }
      break;

    case IMAGE_MIRROR_HORIZONTAL:
      if (dst_d != src_d)
      {
        func = _MirrorFuncsFlipSrcIsNotDst[bytesPerPixel];
      }
      else
      {
        func = _MirrorFuncsFlipSrcIsDst[bytesPerPixel];
      }
      break;

    case IMAGE_MIRROR_BOTH:
      srcPixels += srcStride * ((ssize_t)h - 1);
      srcStride = -srcStride;

      if (dst_d != src_d)
      {
        func = _MirrorFuncsFlipSrcIsNotDst[bytesPerPixel];
      }
      else
      {
        func = _MirrorFuncsFlipSrcIsDst[bytesPerPixel];
        h >>= 1;
      }
      break;
  }

  for (int y = h; y; y--, dstPixels += dstStride, srcPixels += srcStride)
  {
    func(dstPixels, srcPixels, w);
  }

  dst._modified();
  return ERR_OK;
}

// ============================================================================
// [Fog::Image - Rotate]
// ============================================================================

err_t Image::rotate(Image& dst, const Image& src, uint32_t rotateMode)
{
  if (rotateMode >= 4) return ERR_RT_INVALID_ARGUMENT;
  if (rotateMode == IMAGE_ROTATE_0 || src.isEmpty()) return dst.set(src);

  // Rotation by 180 degrees has the same effect as MIRROR_BOTH.
  if (rotateMode == IMAGE_ROTATE_180) return mirror(dst, src, IMAGE_MIRROR_BOTH);

  // Now we have only two possibilities:
  // - rotate by 90
  // - rotate by 270

  // Destination == source?
  if (&dst == &src) return rotate(dst, Image(src), rotateMode);

  err_t err = dst.create(src.getSize(), src.getFormat());
  if (FOG_IS_ERROR(err)) return err;

  ImageData* dst_d = dst._d;
  ImageData* src_d = src._d;

  ssize_t dstStride = dst_d->stride;
  ssize_t srcStride = src_d->stride;

  uint8_t* dstPixels = dst_d->first;
  uint8_t* srcPixels = src_d->first;

  uint8_t* dstCur;
  uint8_t* srcCur;

  int sz1 = src_d->size.w;
  int sz2 = src_d->size.h;

  int sz1m1 = sz1-1;
  int sz2m1 = sz2-1;

  int i;
  int j;

  ssize_t srcInc1;
  ssize_t srcInc2;

  int bytesPerPixel = src.getBytesPerPixel();

  if (rotateMode == IMAGE_ROTATE_90)
  {
    srcPixels += ((ssize_t)sz2m1 * srcStride);
    srcInc1 = bytesPerPixel;
    srcInc2 = -srcStride;
  }
  else
  {
    srcPixels += (ssize_t)sz1m1 * bytesPerPixel;
    srcInc1 = -bytesPerPixel;
    srcInc2 = srcStride;
  }

  switch (bytesPerPixel)
  {
#define _FOG_ROTATE_LOOP(_Size_) \
    FOG_MACRO_BEGIN \
      for (i = sz1; i; i--, dstPixels += dstStride, srcPixels += srcInc1) \
      { \
        dstCur = dstPixels; \
        srcCur = srcPixels; \
        \
        for (j = sz2; j; j--, dstCur += _Size_, srcCur += srcInc2) \
        { \
          MemOps::copy_s<_Size_>(dstCur, srcCur); \
        } \
      } \
    FOG_MACRO_END

    case 1 : _FOG_ROTATE_LOOP(1); break;
    case 2 : _FOG_ROTATE_LOOP(2); break;
    case 3 : _FOG_ROTATE_LOOP(3); break;
    case 4 : _FOG_ROTATE_LOOP(4); break;
    case 6 : _FOG_ROTATE_LOOP(6); break;
    case 8 : _FOG_ROTATE_LOOP(8); break;

    default:
      FOG_ASSERT_NOT_REACHED();
      return ERR_RT_ASSERTION_FAILURE;
  }

  dst._modified();
  return ERR_OK;
}

#if 0
// ============================================================================
// [Fog::Image - Color Filter]
// ============================================================================

static err_t applyColorFilter(Image& im, const BoxI& box, ColorFilterFunc fn, const void* context)
{
  // Clip.
  int imgw = im.getWidth();
  int imgh = im.getHeight();

  int x0 = Math::max<int>(box.getX0(), 0);
  int y0 = Math::max<int>(box.getY0(), 0);
  int x2 = Math::min<int>(box.getX1(), imgw);
  int y2 = Math::min<int>(box.getY1(), imgh);

  if (x0 >= x2 || y0 >= y2) return ERR_OK;

  int w = x2 - x0;
  int h = y2 - y0;

  FOG_RETURN_ON_ERROR(im.detach());

  uint8_t* imData = im.getDataX();
  ssize_t imStride = im.getStride();
  ssize_t imBpp = im.getBytesPerPixel();

  size_t y;

  uint8_t* cur = imData + (ssize_t)y0 * imStride + (ssize_t)x0 * imBpp;
  for (y = (uint)h; y; y--, cur += imStride) fn(context, cur, cur, w);

  return ERR_OK;
}

err_t Image::filter(const ColorFilter& f, const RectI* area)
{
  BoxI abox(0, 0, getWidth(), getHeight());
  if (area) abox.set(area->getX0(), area->getY0(), area->getX1(), area->getY1());

  ColorFilterFunc fn = f.getEngine()->getColorFilterFunc(getFormat());
  if (!fn) return ERR_IMAGE_UNSUPPORTED_FORMAT;

  const void* context = f.getEngine()->getContext();
  err_t err = applyColorFilter(*this, abox, fn, context);

  f.getEngine()->releaseContext(context);
  return err;
}

err_t Image::filter(const ColorLutFx& lut, const RectI* area)
{
  BoxI abox(0, 0, getWidth(), getHeight());
  if (area) abox.set(area->getX0(), area->getY0(), area->getX1(), area->getY1());

  ColorFilterFunc fn = (ColorFilterFunc)rasterFuncs.filter.color_lut[getFormat()];
  if (!fn) return ERR_IMAGE_UNSUPPORTED_FORMAT;

  return applyColorFilter(*this, abox, fn, lut.getData());
}

err_t Image::filter(const ColorMatrix& cm, const RectI* area)
{
  BoxI abox(0, 0, getWidth(), getHeight());
  if (area) abox.set(area->getX0(), area->getY0(), area->getX1(), area->getY1());

  ColorFilterFunc fn = (ColorFilterFunc)rasterFuncs.filter.color_matrix[getFormat()];
  if (!fn) return ERR_IMAGE_UNSUPPORTED_FORMAT;

  return applyColorFilter(*this, abox, fn, &cm);
}

// ============================================================================
// [Fog::Image - Image Filter]
// ============================================================================

static err_t applyImageFilter(Image& im, const BoxI& box, const ImageFxFilter& filter)
{
  // Never call applyImageFilter() with color filter, see applyColorFilter().
  FOG_ASSERT((filter.getCharacteristics() & IMAGE_EFFECT_CHAR_COLOR_TRANSFORM) == 0);

  // Clip.
  int imgw = im.getWidth();
  int imgh = im.getHeight();
  int imgf = im.getFormat();

  int x0 = Math::max<int>(box.getX0(), 0);
  int y0 = Math::max<int>(box.getY0(), 0);
  int x2 = Math::min<int>(box.getX1(), imgw);
  int y2 = Math::min<int>(box.getY1(), imgh);

  if (x0 >= x2 || y0 >= y2) return ERR_OK;

  int w = x2 - x0;
  int h = y2 - y0;

  uint32_t filterFormat = imgf;
  uint32_t filterCharacteristics = filter.getCharacteristics();

  if ((filterCharacteristics & (IMAGE_EFFECT_CHAR_HV_PROCESSING | IMAGE_EFFECT_CHAR_ENTIRE_PROCESSING)) == 0)
  {
    // NOP.
    return ERR_OK;
  }

  FOG_RETURN_ON_ERROR(im.detach());

  const void* context = filter.getEngine()->getContext();
  ImageFilterFn fn;

  uint8_t* imData = im.getDataX();
  ssize_t imStride = im.getStride();
  ssize_t imBpp = im.getBytesPerPixel();

  uint8_t* imBegin = imData + (ssize_t)y0 * imStride + (ssize_t)x0 * imBpp;
  uint8_t* imCur = imBegin;

  // Demultiply if needed.
  if (imgf == IMAGE_FORMAT_PRGB32 && (filterCharacteristics & IMAGE_EFFECT_CHAR_SUPPORTS_PRGB32) == 0)
  {
    RenderVBlitLineFunc vblit_line =
      rasterFuncs.dib.convert[IMAGE_FORMAT_ARGB32][IMAGE_FORMAT_PRGB32];
    for (int y = h; y; y--, imCur += imStride) vblit_line(imCur, imCur, w, NULL);

    imCur = imBegin;
    filterFormat = IMAGE_FORMAT_ARGB32;
  }

  err_t err = ERR_OK;

  // Vertical & Horizontal processing.
  if ((filterCharacteristics & IMAGE_EFFECT_CHAR_HV_PROCESSING) == IMAGE_EFFECT_CHAR_HV_PROCESSING)
  {
    fn = filter.getEngine()->getImageFilterFunc(filterFormat, IMAGE_EFFECT_CHAR_VERT_PROCESSING);
    if (!fn) { err = ERR_IMAGE_UNSUPPORTED_FORMAT; goto _End; }

    fn(context, imCur, imStride, imCur, imStride, w, h, -1);

    fn = filter.getEngine()->getImageFilterFunc(filterFormat, IMAGE_EFFECT_CHAR_HORZ_PROCESSING);
    if (!fn) { err = ERR_IMAGE_UNSUPPORTED_FORMAT; goto _End; }

    fn(context, imCur, imStride, imCur, imStride, w, h, -1);
  }
  // Vertical processing only (one pass).
  else if ((filterCharacteristics & IMAGE_EFFECT_CHAR_VERT_PROCESSING) != 0)
  {
    fn = filter.getEngine()->getImageFilterFunc(filterFormat, IMAGE_EFFECT_CHAR_VERT_PROCESSING);
    if (!fn) { err = ERR_IMAGE_UNSUPPORTED_FORMAT; goto _End; }

    fn(context, imCur, imStride, imCur, imStride, w, h, -1);
  }
  // Horizontal processing only (one pass).
  else if ((filterCharacteristics & IMAGE_EFFECT_CHAR_HORZ_PROCESSING) != 0)
  {
    fn = filter.getEngine()->getImageFilterFunc(filterFormat, IMAGE_EFFECT_CHAR_HORZ_PROCESSING);
    if (!fn) { err = ERR_IMAGE_UNSUPPORTED_FORMAT; goto _End; }

    fn(context, imCur, imStride, imCur, imStride, w, h, -1);
  }
  // Entire processing (one pass).
  else if ((filterCharacteristics & IMAGE_EFFECT_CHAR_ENTIRE_PROCESSING) != 0)
  {
    fn = filter.getEngine()->getImageFilterFunc(filterFormat, IMAGE_EFFECT_CHAR_ENTIRE_PROCESSING);
    if (!fn) { err = ERR_IMAGE_UNSUPPORTED_FORMAT; goto _End; }

    fn(context, imCur, imStride, imCur, imStride, w, h, -1);
  }

  // Premultiply if demultiplied.
  if (imgf == IMAGE_FORMAT_PRGB32 && (filterCharacteristics & IMAGE_EFFECT_CHAR_SUPPORTS_PRGB32) == 0)
  {
    RenderVBlitLineFunc vblit_line =
      rasterFuncs.dib.convert[IMAGE_FORMAT_PRGB32][IMAGE_FORMAT_ARGB32];
    for (int y = h; y; y--, imCur += imStride) vblit_line(imCur, imCur, w, NULL);
  }

_End:
  filter.getEngine()->releaseContext(context);
  return err;
}

err_t Image::filter(const ImageFxFilter& f, const RectI* area)
{
  // Use optimized way for ColorFilter if image doest color transform.
  if (f.getCharacteristics() & IMAGE_EFFECT_CHAR_COLOR_TRANSFORM)
  {
    return filter(reinterpret_cast<const ColorFilter&>(f), area);
  }

  BoxI abox(0, 0, getWidth(), getHeight());
  if (area) abox.set(area->getX0(), area->getY0(), area->getX1(), area->getY1());

  return applyImageFilter(*this, abox, f);
}
#endif

// ============================================================================
// [Fog::Image - Filter]
// ============================================================================

#if 0
err_t Image::filter(
  const ImageFx& fx,
  Image& dst, PointI* dstOffset,
  const Image& src, const RectI* srcFragment)
{
  ImageFxFilter fxFilter;
  FOG_RETURN_ON_ERROR(fxFilter.setEffect(fx));

  return filter(fxFilter, dst, dstOffset, src, srcFragment);
}

err_t Image::filter(
  const ImageFxFilter& fxFilter,
  Image& dst, PointI* dstOffset,
  const Image& src, const RectI* srcFragment)
{
  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

Image Image::filtered(const ImageFx& fx, const RectI* srcFragment)
{
  Image dst;
  ImageFxFilter fxFilter;

  if (fxFilter.setEffect(fx) == ERR_OK)
  {
    filter(fxFilter, dst, NULL, *this, srcFragment);
  }

  return dst;
}

Image Image::filtered(const ImageFxFilter& fxFilter, const RectI* srcFragment)
{
  Image dst;
  filter(fxFilter, dst, NULL, *this, srcFragment);
  return dst;
}
#endif

// ============================================================================
// [Fog::Image - Scale]
// ============================================================================

#if 0
Image Image::scaled(const SizeI& to, uint32_t interpolationType) const
{
  Image dst;

  if (isEmpty() || (getWidth() == to.w && getHeight() == to.h))
  {
    dst = *this;
    return dst;
  }

  if (to.w == 0 || to.h == 0) return dst;
  if (dst.create(to, getFormat()) != ERR_OK) return dst;

  RenderPatternContext ctx;
  err_t err = rasterFuncs.pattern.texture_init_scale(&ctx, *this, to.w, to.h, interpolationType);
  if (FOG_IS_ERROR(err)) { dst.reset(); return dst; }

  uint8_t* dstData = (uint8_t*)dst.getData();
  ssize_t dstStride = dst.getStride();

  RasterSpanExt8 span;
  span.setPositionAndType(0, to.w, RASTER_SPAN_C);
  span.setCMask(0xFF);
  span.setNext(NULL);

  for (int y = 0; y < to.h; y++, dstData += dstStride)
  {
    ctx.fetch(&ctx, &span, dstData, y, RENDER_FETCH_COPY);
  }

  ctx.destroy(&ctx);
  return dst;
}
#endif

// ============================================================================
// [Fog::Image - Fill]
// ============================================================================

err_t Image::clear(const Color& color)
{
  if (isEmpty()) return ERR_OK;
  if (getFormat() == IMAGE_FORMAT_I8) return ERR_IMAGE_UNSUPPORTED_FORMAT;

  if (!isDetached())
    FOG_RETURN_ON_ERROR(create(getSize(), getFormat(), getType()));

  return fillRect(RectI(0, 0, getWidth(), getHeight()), color, COMPOSITE_SRC, 1.0f);
}

err_t Image::fillRect(const RectI& r, const Color& color,  uint32_t compositingOperator, float opacity)
{
  if (FOG_UNLIKELY((uint)compositingOperator >= COMPOSITE_COUNT)) return ERR_RT_INVALID_ARGUMENT;
  if (getFormat() == IMAGE_FORMAT_I8) return ERR_IMAGE_UNSUPPORTED_FORMAT;

  int dstFormat = _d->format;

  bool isPRGBPixel;
  bool isOpaque;
  uint32_t spanOpacity = 0;

  RenderSolid solid;
  RasterSpan span;

  switch (ImageFormatDescription::getByFormat(dstFormat).getPrecision())
  {
    case IMAGE_PRECISION_BYTE:
    {
      Face::p32 pix0 = color.getArgb32();
      Face::p32PRGB32FromARGB32(solid.prgb32.p32, pix0);

      Face::p32 opacity_8;
      Face::f32CvtU8FromFX(opacity_8, opacity);
      if (opacity_8 == 0) return ERR_OK;

      isPRGBPixel = Face::p32PRGB32IsAlphaFF(pix0);
      isOpaque    = (opacity_8 == 0x100);

      spanOpacity = spanOpacity;
      break;
    }

    case IMAGE_PRECISION_WORD:
    {
      Face::p64 pix0 = Face::p64FromU64(color.getArgb64());
      Face::p64PRGB64FromARGB64(solid.prgb64.p64, pix0);

      Face::p32 opacity_16;
      Face::f32CvtU16FromFX(opacity_16, opacity);
      if (opacity_16 == 0) return ERR_OK;

      isPRGBPixel = Face::p64PRGB64IsAlphaFFFF(pix0);
      isOpaque    = (opacity_16 == 0x10000);

      spanOpacity = opacity_16;
      break;
    }

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  int x0 = r.getX0();
  int y0 = r.getY0();
  int x1 = r.getX1();
  int y1 = r.getY1();

  int w = _d->size.w;
  int h = _d->size.h;

  if (x0 < 0) x0 = 0;
  if (y0 < 0) y0 = 0;
  if (x1 > w) x1 = w;
  if (y1 > h) y1 = h;

  if ((w = x1 - x0) <= 0) return ERR_OK;
  if ((h = y1 - y0) <= 0) return ERR_OK;

  FOG_RETURN_ON_ERROR(detach());

  ssize_t dstStride = _d->stride;
  uint8_t* dstPixels = _d->first + y0 * dstStride;

  RenderClosure closure;
  closure.ditherOrigin.set(x0, y0);
  closure.palette = NULL;
  closure.data = NULL;

  if (isOpaque)
  {
    RenderCBlitLineFunc blitLine = _g2d_render.getCBlitLine(dstFormat, compositingOperator, isPRGBPixel);

    dstPixels += (uint)x0 * getBytesPerPixel();
    for (int i = 0; i < h; i++, dstPixels += dstStride)
    {
      blitLine(dstPixels, &solid, w, &closure);
    }
  }
  else
  {
    RenderCBlitSpanFunc blitSpan = _g2d_render.getCBlitSpan(dstFormat, compositingOperator, isPRGBPixel);

    span.setPositionAndType(x0, x1, RASTER_SPAN_C);
    span.setNext(NULL);
    reinterpret_cast<RasterSpan16*>(&span)->setConstMask(spanOpacity);

    for (int i = 0; i < h; i++, dstPixels += dstStride)
    {
      blitSpan(dstPixels, &solid, &span, &closure);
    }
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::Image - Blit]
// ============================================================================

static err_t Image_blitImage(
  Image& dst, int dstX, int dstY,
  const Image& src, int srcX, int srcY,
  int w, int h,
  uint32_t compositingOperator, float opacity)
{
  uint32_t dstFormat = dst._d->format;

  bool isOpaque;
  RasterSpanExt8 span;

  switch (ImageFormatDescription::getByFormat(dstFormat).getPrecision())
  {
    case IMAGE_PRECISION_BYTE:
    {
      Face::p32 opacity_8;
      Face::f32CvtU8FromFX(opacity_8, opacity);
      if (opacity_8 == 0) return ERR_OK;

      isOpaque = (opacity_8 == 0x100);
      reinterpret_cast<RasterSpan8*>(&span)->setConstMask(opacity_8);
      break;
    }

    case IMAGE_PRECISION_WORD:
    {
      Face::p32 opacity_16;
      Face::f32CvtU16FromFX(opacity_16, opacity);
      if (opacity_16 == 0) return ERR_OK;

      isOpaque = (opacity_16 == 0x10000);
      reinterpret_cast<RasterSpan16*>(&span)->setConstMask(opacity_16);
      break;
    }

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  FOG_RETURN_ON_ERROR(dst.detach());

  uint8_t* dstPixels = dst._d->first;
  ssize_t dstStride = dst._d->stride;

  ssize_t srcStride = src._d->stride;
  const uint8_t* srcPixels = src._d->first;
  uint32_t srcFormat = src._d->format;

  // Special case if dst and src overlaps.
  if (dst._d == src._d && dstY >= srcY && (dstY - srcY) <= h)
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

  dstPixels += dstX * dst._d->bytesPerPixel;
  srcPixels += srcX * src._d->bytesPerPixel;

  RenderClosure closure;
  closure.ditherOrigin.reset();
  closure.palette = src._d->palette._d;
  closure.data = NULL;

  MemBufferTmp<2048> buffer;

  if (isOpaque)
  {
    RenderVBlitLineFunc blitLine;
    RenderVBlitLineFunc converter = NULL;

    if (RenderUtil::isCompositeCoreOperator(compositingOperator))
    {
      blitLine = _g2d_render.get_FuncsCompositeCore(dstFormat, compositingOperator)->vblit_line[srcFormat];
    }
    else
    {
      uint32_t compat  = RenderUtil::getCompatFormat(dstFormat, srcFormat);
      uint32_t vBlitId = RenderUtil::getCompatVBlitId(dstFormat, srcFormat);

      blitLine = _g2d_render.get_FuncsCompositeExt(dstFormat, compositingOperator)->vblit_line[compat];

      if (compat != srcFormat)
      {
        converter = _g2d_render.get_FuncsCompositeCore(compat, COMPOSITE_SRC)->vblit_line[srcFormat];

        if (FOG_IS_NULL(buffer.alloc(w * ImageFormatDescription::getByFormat(compat).getBytesPerPixel())))
          return ERR_RT_OUT_OF_MEMORY;
      }
    }

    // Overlapped.
    if (converter == NULL && dst._d == src._d && dstX >= srcX && (dstX - srcX) <= w)
    {
      converter = _g2d_render.get_FuncsCompositeCore(srcFormat, COMPOSITE_SRC)->vblit_line[srcFormat];

      if (FOG_IS_NULL(buffer.alloc(w * src.getBytesPerPixel())))
        return ERR_RT_OUT_OF_MEMORY;
    }

    if (converter == NULL)
    {
      for (int i = 0; i < h; i++, dstPixels += dstStride, srcPixels += srcStride)
      {
        blitLine(dstPixels, srcPixels, w, &closure);
      }
    }
    else
    {
      for (int i = 0; i < h; i++, dstPixels += dstStride, srcPixels += srcStride)
      {
        converter(reinterpret_cast<uint8_t*>(buffer.getMem()), srcPixels, w, NULL);
        blitLine(dstPixels, reinterpret_cast<uint8_t*>(buffer.getMem()), w, &closure);
      }
    }
  }
  else
  {
    RenderVBlitSpanFunc blitLine;
    RenderVBlitLineFunc converter = NULL;

    if (RenderUtil::isCompositeCoreOperator(compositingOperator))
    {
      blitLine = _g2d_render.get_FuncsCompositeCore(dstFormat, compositingOperator)->vblit_span[srcFormat];
    }
    else
    {
      uint32_t compat  = RenderUtil::getCompatFormat(dstFormat, srcFormat);
      uint32_t vBlitId = RenderUtil::getCompatVBlitId(dstFormat, srcFormat);

      blitLine = _g2d_render.get_FuncsCompositeExt(dstFormat, compositingOperator)->vblit_span[compat];

      if (compat != srcFormat)
      {
        converter = _g2d_render.get_FuncsCompositeCore(compat, COMPOSITE_SRC)->vblit_line[srcFormat];

        if (FOG_IS_NULL(buffer.alloc(w * ImageFormatDescription::getByFormat(compat).getBytesPerPixel())))
          return ERR_RT_OUT_OF_MEMORY;
      }
    }

    // Overlapped.
    if (converter == NULL && dst._d == src._d && dstX >= srcX && (dstX - srcX) <= w)
    {
      converter = _g2d_render.get_FuncsCompositeCore(srcFormat, COMPOSITE_SRC)->vblit_line[srcFormat];

      if (FOG_IS_NULL(buffer.alloc(w * src.getBytesPerPixel())))
        return ERR_RT_OUT_OF_MEMORY;
    }

    span.setPositionAndType(0, w, RASTER_SPAN_C);
    span.setNext(NULL);

    if (converter == NULL)
    {
      for (int i = 0; i < h; i++, dstPixels += dstStride, srcPixels += srcStride)
      {
        span.setData(const_cast<uint8_t*>(srcPixels));
        blitLine(dstPixels, &span, &closure);
      }
    }
    else
    {
      span.setData(reinterpret_cast<uint8_t*>(buffer.getMem()));

      for (int i = 0; i < h; i++, dstPixels += dstStride, srcPixels += srcStride)
      {
        converter(reinterpret_cast<uint8_t*>(buffer.getMem()), srcPixels, w, NULL);
        blitLine(dstPixels, &span, &closure);
      }
    }
  }

  return ERR_OK;
}

err_t Image::blitImage(const PointI& pt, const Image& src, uint32_t compositingOperator, float opacity)
{
  if (FOG_UNLIKELY(compositingOperator >= COMPOSITE_COUNT)) return ERR_RT_INVALID_ARGUMENT;

  ImageData* dst_d = _d;
  ImageData* src_d = src._d;

  int w = dst_d->size.w;
  int h = dst_d->size.h;

  int x0 = pt.getX();
  int y0 = pt.getY();
  int x1 = x0 + src_d->size.w;
  int y1 = y0 + src_d->size.h;

  if (x0 < 0) x0 = 0;
  if (y0 < 0) y0 = 0;
  if (x1 > w) x1 = w;
  if (y1 > h) y1 = h;

  if (x0 >= x1 || y0 >= y1) return ERR_OK;
  return Image_blitImage(*this, x0, y0, src, x0 - pt.getX(), y0 - pt.getY(), x1 - x0, y1 - y0, compositingOperator, opacity);
}

err_t Image::blitImage(const PointI& pt, const Image& src, const RectI& srcRect, uint32_t compositingOperator, float opacity)
{
  if (FOG_UNLIKELY(compositingOperator >= COMPOSITE_COUNT)) return ERR_RT_INVALID_ARGUMENT;
  if (FOG_UNLIKELY(!srcRect.isValid())) return ERR_OK;

  ImageData* dst_d = _d;
  ImageData* src_d = src._d;

  int srcX0 = srcRect.getX0();
  int srcY0 = srcRect.getY0();
  int srcX1 = srcRect.getX1();
  int srcY1 = srcRect.getY1();

  if (srcX0 < 0) srcX0 = 0;
  if (srcY0 < 0) srcY0 = 0;
  if (srcX1 > src_d->size.w) srcX1 = src_d->size.w;
  if (srcY1 > src_d->size.h) srcY1 = src_d->size.h;

  if (srcX0 >= srcX1 || srcY0 >= srcY1) return ERR_OK;

  int dstX0 = pt.getX() + (srcX0 - srcRect.getX0());
  int dstY0 = pt.getY() + (srcY0 - srcRect.getY0());
  int dstX1 = dstX0 + (srcX1 - srcX0);
  int dstY1 = dstY0 + (srcY1 - srcY0);

  if (dstX0 < 0) { srcX0 -= dstX0; dstX0 = 0; }
  if (dstY0 < 0) { srcY0 -= dstY0; dstY0 = 0; }
  if (dstX1 > dst_d->size.w) dstX1 = dst_d->size.w;
  if (dstY1 > dst_d->size.h) dstY1 = dst_d->size.h;

  if (dstX0 >= dstX1 || dstY0 >= dstY1) return ERR_OK;
  return Image_blitImage(*this, dstX0, dstY0, src, srcX0, srcY0, dstX1 - dstX0, dstY1 - dstY0, compositingOperator, opacity);
}

// ============================================================================
// [Fog::Image - Scroll]
// ============================================================================

err_t Image::scroll(int scrollX, int scrollY)
{
  return scroll(scrollX, scrollY, RectI(0, 0, getWidth(), getHeight()));
}

err_t Image::scroll(int scrollX, int scrollY, const RectI& r)
{
  if (scrollX == 0 && scrollY == 0) return ERR_OK;

  ImageData* d = _d;

  int x0 = r.getX0();
  int y0 = r.getY0();
  int x1 = r.getX1();
  int y1 = r.getY1();

  if (x0 < 0) x0 = 0;
  if (y0 < 0) y0 = 0;
  if (x1 > d->size.w) x1 = d->size.w;
  if (y1 > d->size.h) y1 = d->size.h;

  if (x0 >= x1 || y0 >= y1) return ERR_OK;

  int absX = abs(scrollX);
  int absY = abs(scrollY);

  int scrollW = x1 - x0;
  int scrollH = y1 - y0;

  int srcX, srcY;
  int dstX, dstY;

  if (absX >= scrollW || absY >= scrollH) return ERR_OK;

  FOG_RETURN_ON_ERROR(detach());
  d = _d;

  if (scrollX < 0) { srcX = absX; dstX = 0; } else { srcX = 0; dstX = absX; }
  if (scrollY < 0) { srcY = absY; dstY = 0; } else { srcY = 0; dstY = absY; }

  scrollW -= absX;
  scrollH -= absY;

  ssize_t stride = d->stride;
  ssize_t size = scrollW * d->bytesPerPixel;

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
  {
    memmove(dstPixels, srcPixels, size);
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::Image - Misc]
// ============================================================================

struct GlyphFromPathFiller8 : public RasterFiller
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE GlyphFromPathFiller8(uint8_t* pixels, ssize_t stride, int x0, int y0)
  {
    this->_prepare = (RasterFiller::PrepareFunc)advanceFn;
    this->_process = (RasterFiller::ProcessFunc)processFn;
    this->_skip = (RasterFiller::SkipFunc)advanceFn;

    this->pixels = pixels - y0 * stride - x0;
    this->stride = stride;
  }

  // --------------------------------------------------------------------------
  // [Callbacks]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL advanceFn(GlyphFromPathFiller8* self, int y)
  {
    self->pixels += y * self->stride;
  }

  static void FOG_FASTCALL processFn(GlyphFromPathFiller8* self, RasterSpan* _span)
  {
    RasterSpan8* span = reinterpret_cast<RasterSpan8*>(_span);

    do {
      uint8_t* p = self->pixels + (uint)(span->getX0());
      uint8_t* m = reinterpret_cast<uint8_t*>(span->getGenericMask());

      uint i;
      uint len = (uint)span->getLength();

      switch (span->getType())
      {
        case RASTER_SPAN_C:
          memset(p, (0xFF * RasterSpan8::getConstMaskFromPointer(m)) >> 8, len);
          break;
        case RASTER_SPAN_A8_GLYPH:
        case RASTER_SPAN_AX_GLYPH:
          memcpy(p, m, len);
          break;
        case RASTER_SPAN_AX_EXTRA:
          for (i = 0; i < len; i++) p[i] = (uint8_t)((0xFF * ((uint16_t*)m)[i]) >> 8);
          break;
      }

      span = span->getNext();
    } while (span);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint8_t* pixels;
  ssize_t stride;
};


err_t Image::glyphFromPath(Image& glyph, PointI& offset, const PathD& path, uint32_t precision)
{
  err_t err = ERR_OK;
  BoxD boundingBox(UNINITIALIZED);

  int x0;
  int y0;
  int x1;
  int y1;

  int w;
  int h;

  if (FOG_UNLIKELY(precision) >= IMAGE_PRECISION_COUNT)
  {
    err = ERR_RT_INVALID_ARGUMENT;
    goto _Fail;
  }

  err = path.getBoundingBox(boundingBox);
  if (FOG_IS_ERROR(err))
  {
    if (err == ERR_GEOMETRY_NONE) err = ERR_OK;
    goto _Fail;
  }

  x0 = Math::ifloor(boundingBox.getX0());
  y0 = Math::ifloor(boundingBox.getY0());
  x1 = Math::iceil(boundingBox.getX1());
  y1 = Math::iceil(boundingBox.getY1());

  offset.set(x0, y0);
  w = x1 - x0;
  h = y1 - y0;

  switch (precision)
  {
    case IMAGE_PRECISION_BYTE:
    {
      PathRasterizer8 rasterizer;
      RasterScanline8 scanline;

      rasterizer.setSceneBox(BoxI(x0, y0, x1, y1));
      rasterizer.addPath(path);
      rasterizer.finalize();

      if (!rasterizer.isValid())
      {
        err = rasterizer.getError();
        goto _Fail;
      }

      err = glyph.create(SizeI(w, h), IMAGE_FORMAT_A8);
      if (FOG_IS_ERROR(err)) goto _Fail;

      uint8_t* pixels = glyph.getFirstX();
      ssize_t stride = glyph.getStride();

      MemOps::zero(pixels, glyph.getHeight() * stride);

      GlyphFromPathFiller8 filler(pixels, stride, x0, y0);
      rasterizer.render(&filler, &scanline);
      break;
    }

    case IMAGE_PRECISION_WORD:
    {
      // TODO: Rasterizer precision.
      break;
    }

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  if (FOG_IS_ERROR(err))
  {
_Fail:
    glyph.reset();
    offset.reset();
  }

  return err;
}

// ============================================================================
// [Fog::Image - Windows Support]
// ============================================================================

#if defined(FOG_OS_WINDOWS)
/*
  The following code can be used to test Image <=> HBITMAP conversion:

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
  i.fromWinBitmap(hbmp);

  DeleteObject((HGDIOBJ)hbmp);
*/

HBITMAP Image::toWinBitmap() const
{
  ImageData* d = _d;
  if (d->size.w == 0 || d->size.h == 0) return NULL;

  // If the image format is PRGB32/XRGB32/RGB24 then it's easy, it's only
  // needed to create DIBSECTION and copy bits there. If the image format
  // is A8/I8 then we create 32-bit DIBSECTION and copy there the alphas,
  // this image will be still usable when using functions like AlphaBlend().

  uint8_t* dstBits = NULL;
  ssize_t dstStride = 0;

  uint8_t* srcBits = d->first;
  ssize_t srcStride = d->stride;

  uint32_t dstFormat = IMAGE_FORMAT_NULL;

  switch (d->format)
  {
    case IMAGE_FORMAT_PRGB32:
    case IMAGE_FORMAT_PRGB64:
      dstFormat = IMAGE_FORMAT_PRGB32;
      break;

    case IMAGE_FORMAT_XRGB32:
    case IMAGE_FORMAT_RGB24:
      dstFormat = d->format;
      break;

    // There is no such concept like Alpha-only image in Windows GDI. So we
    // treat this kind of image as premultiplied RGB. It's waste, but I have
    // no idea how to improve it.
    case IMAGE_FORMAT_A8:
    case IMAGE_FORMAT_A16:
      dstFormat = IMAGE_FORMAT_PRGB32;
      break;

    case IMAGE_FORMAT_I8:
      dstFormat = IMAGE_FORMAT_XRGB32;
      break;

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  HBITMAP hBitmap;
  err_t err = WinDibImageData::_createDibSection(d->size, dstFormat, &hBitmap, &dstBits, &dstStride);
  if (FOG_IS_ERROR(err)) return NULL;

  RenderVBlitLineFunc blitLine = _g2d_render.get_FuncsCompositeCore(dstFormat, COMPOSITE_SRC)->vblit_line[d->format];

  RenderClosure closure;
  closure.ditherOrigin.reset();
  closure.palette = d->palette._d;
  closure.data = NULL;

  int w = d->size.w;
  int h = d->size.h;

  for (int y = 0; y < h; y++, dstBits += dstStride, srcBits += srcStride, closure.ditherOrigin.y++)
  {
    blitLine(dstBits, srcBits, w, &closure);
  }

  return hBitmap;
}

err_t Image::fromWinBitmap(HBITMAP hBitmap)
{
  DIBSECTION ds;

  if (hBitmap == NULL) return ERR_RT_INVALID_ARGUMENT;
  uint32_t format = IMAGE_FORMAT_NULL;

  // DIB-Section.
  if (GetObjectW(hBitmap, sizeof(DIBSECTION), &ds) != 0)
  {
    RenderVBlitLineFunc blitLine = NULL;

    // RGB24
    if (ds.dsBm.bmBitsPixel == 24)
    {
      format = IMAGE_FORMAT_RGB24;
      blitLine = _g2d_render.convert.copy[RENDER_CONVERTER_COPY_24];
    }

    // RGB32.
    if (ds.dsBm.bmBitsPixel == 32 && ds.dsBitfields[0] == 0x00FF0000 &&
                                     ds.dsBitfields[1] == 0x0000FF00 &&
                                     ds.dsBitfields[2] == 0x000000FF)
    {
      format = IMAGE_FORMAT_XRGB32;
      blitLine = _g2d_render.convert.copy[RENDER_CONVERTER_COPY_32];
    }

    if (blitLine)
    {
      int w = (int)ds.dsBm.bmWidth;
      int h = Math::abs(ds.dsBm.bmHeight);

      FOG_RETURN_ON_ERROR(create(SizeI(w, h), format));

      uint8_t* dstPixels = _d->first;
      ssize_t dstStride = _d->stride;

      const uint8_t* srcPixels = (const uint8_t*)ds.dsBm.bmBits;
      ssize_t srcStride = ds.dsBm.bmWidthBytes;

      // Bottom-To-Top.
      if (ds.dsBm.bmHeight > 0)
      {
        srcPixels += (ssize_t)(h - 1) * srcStride;
        srcStride = -srcStride;
      }

      for (int y = ds.dsBm.bmHeight - 1; y >= 0; y--, dstPixels += dstStride, srcPixels += srcStride)
      {
        blitLine(dstPixels, srcPixels, w, NULL);
      }
      return ERR_OK;
    }
  }

  if (GetObjectW(hBitmap, sizeof(BITMAP), &ds.dsBm) != 0)
  {
    HDC hdc;
    HBITMAP hOldBitmap;
    BITMAPINFO di;

    switch (ds.dsBm.bmBitsPixel)
    {
      case  1:
      case  4:
      case  8:
      case 16:
      case 24: format = IMAGE_FORMAT_RGB24; break;
      case 32: format = IMAGE_FORMAT_XRGB32; break;

      default:
        return ERR_IMAGE_INVALID_FORMAT;
    }

    FOG_RETURN_ON_ERROR(create(SizeI((int)ds.dsBm.bmWidth, (int)ds.dsBm.bmHeight), format));

    uint8_t* dstPixels = _d->first;
    ssize_t dstStride = _d->stride;

    // DDB bitmap.
    if ((hdc = CreateCompatibleDC(NULL)) == NULL)
    {
      return GetLastError();
    }
    if ((hOldBitmap = (HBITMAP)SelectObject(hdc, (HGDIOBJ)hBitmap)) == NULL)
    {
      DeleteDC(hdc);
      return GetLastError();
    }

    ZeroMemory(&di, sizeof(BITMAPINFO));
    di.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    di.bmiHeader.biCompression = BI_RGB;
    di.bmiHeader.biWidth = ds.dsBm.bmWidth;
    di.bmiHeader.biHeight = ds.dsBm.bmHeight;
    di.bmiHeader.biBitCount = getDepth();
    di.bmiHeader.biPlanes = 1;

    for (int y = ds.dsBm.bmHeight - 1; y >= 0; y--, dstPixels += dstStride)
    {
      GetDIBits(hdc, hBitmap, y, 1, (LPVOID)dstPixels, &di, DIB_RGB_COLORS);
    }

    SelectObject(hdc, (HGDIOBJ)hOldBitmap);
    DeleteDC(hdc);
    return ERR_OK;
  }

  return ERR_RT_INVALID_ARGUMENT;
}

HDC Image::getDC()
{
  if (_d->type != IMAGE_BUFFER_WIN_DIB) return (HDC)NULL;
  if (detach() != ERR_OK) return (HDC)NULL;

  return reinterpret_cast<WinDibImageData*>(_d)->getDC();
}

void Image::releaseDC(HDC hDC)
{
  if (_d->type != IMAGE_BUFFER_WIN_DIB) return;
  reinterpret_cast<WinDibImageData*>(_d)->releaseDC(hDC);
}

#endif // FOG_OS_WINDOWS

// ============================================================================
// [Fog::Image - Read]
// ============================================================================

err_t Image::readFromFile(const StringW& fileName)
{
  ImageDecoder* decoder = NULL;
  err_t err = ImageCodecProvider::createDecoderForFile(fileName, &decoder);
  if (FOG_IS_ERROR(err)) return err;

  err = decoder->readImage(*this);

  fog_delete(decoder);
  return err;
}

err_t Image::readFromStream(Stream& stream)
{
  return readFromStream(stream, StringW());
}

err_t Image::readFromStream(Stream& stream, const StringW& extension)
{
  ImageDecoder* decoder = NULL;
  err_t err = ImageCodecProvider::createDecoderForStream(stream, extension, &decoder);
  if (FOG_IS_ERROR(err)) return err;

  err = decoder->readImage(*this);

  fog_delete(decoder);
  return err;
}

err_t Image::readFromBuffer(const StringA& buffer)
{
  return readFromBuffer(buffer.getData(), buffer.getLength());
}

err_t Image::readFromBuffer(const StringA& buffer, const StringW& extension)
{
  return readFromBuffer(buffer.getData(), buffer.getLength(), extension);
}

err_t Image::readFromBuffer(const void* buffer, size_t size)
{
  Stream stream;
  stream.openBuffer((void*)buffer, size, STREAM_OPEN_READ);
  return readFromStream(stream, StringW());
}

err_t Image::readFromBuffer(const void* buffer, size_t size, const StringW& extension)
{
  Stream stream;
  stream.openBuffer((void*)buffer, size, STREAM_OPEN_READ);
  return readFromStream(stream, extension);
}

// ============================================================================
// [Fog::Image - Write]
// ============================================================================

err_t Image::writeToFile(const StringW& fileName) const
{
  Stream stream;

  err_t err = stream.openFile(fileName,
    STREAM_OPEN_WRITE       |
    STREAM_OPEN_CREATE      |
    STREAM_OPEN_CREATE_PATH |
    STREAM_OPEN_TRUNCATE    );
  if (FOG_IS_ERROR(err)) return err;

  StringTmpW<16> extension;
  if ((err = FileSystem::extractExtension(extension, fileName)) || (err = extension.lower()))
  {
    return err;
  }

  err = writeToStream(stream, extension);
  return err;
}

err_t Image::writeToStream(Stream& stream, const StringW& extension) const
{
  ImageCodecProvider* provider = ImageCodecProvider::getProviderByExtension(IMAGE_CODEC_ENCODER, extension);
  if (provider != NULL)
  {
    ImageEncoder* encoder = NULL;
    err_t err = provider->createCodec(IMAGE_CODEC_ENCODER,
      reinterpret_cast<ImageCodec**>(&encoder));
    if (FOG_IS_ERROR(err)) return err;

    encoder->attachStream(stream);
    err = encoder->writeImage(*this);

    fog_delete(encoder);
    return err;
  }

  return ERR_IMAGE_NO_ENCODER;
}

err_t Image::writeToBuffer(StringA& buffer, const StringW& extension) const
{
  Stream stream;
  err_t err = stream.openBuffer(buffer);
  if (FOG_IS_ERROR(err)) return err;

  stream.seek(buffer.getLength(), STREAM_SEEK_SET);
  return writeToStream(stream, extension);
}

// ============================================================================
// [Fog::Image - Statics]
// ============================================================================

Static<ImageData> Image::_dnull;

ssize_t Image::getStrideFromWidth(int width, uint32_t depth)
{
  ssize_t result = 0;
  FOG_ASSUME(width >= 0);

  switch (depth)
  {
    case  1: result = (width + 7) >> 3; goto _Align;
    case  4: result = (width + 1) >> 1; goto _Align;
    case  5:
    case  6:
    case  7:
    case  8: result = width; break;

    case 15:
    case 16: result = width * 2; break;

    case 24:
    case 32:
    case 48:
    case 64:
    case 96:
    case 128:
      result = width * (int)(depth >> 3);
      break;
    default:
      return 0;
  }

  // Overflow.
  if (result < width) return 0;

  // Align to 32-bit boudary.
_Align:
  result += 3;
  result &= ~(ssize_t)3;

  // Success.
  return result;
}

ImageData* Image::_dalloc(size_t size)
{
  ImageData* d = (ImageData*)MemMgr::alloc(ImageData::getSizeOf(size));
  if (FOG_IS_NULL(d)) return NULL;

  fog_new_p(d) ImageData();
  d->data = (size != 0) ? d->buffer : NULL;
  d->first = d->data;
  return d;
}

ImageData* Image::_dalloc(const SizeI& size, uint32_t format)
{
  ImageData* d;
  ssize_t stride;

  FOG_ASSERT(size.isValid());

  // Zero or negative coordinates are invalid.
  if (size.w == 0 || size.h == 0) return NULL;

  // Limit the image size to IMAGE_LIMITS.
  if (size.w >= IMAGE_MAX_WIDTH || size.h >= IMAGE_MAX_HEIGHT) return NULL;

  // Prevent multiply overflow (64 bit int type).
  uint64_t sizeTotal = (int64_t)size.w * size.h;
  if (sizeTotal > (int64_t)(INT_MAX)) return NULL;

  // Calculate depth.
  uint32_t depth = ImageFormatDescription::getByFormat(format).getDepth();

  // Calculate stride.
  if ((stride = Image::getStrideFromWidth(size.w, depth)) == 0) return 0;

  // Try to alloc data.
  d = _dalloc((size_t)(size.h * stride));
  if (FOG_UNLIKELY(d == NULL)) return NULL;

  d->size = size;
  d->format = format;
  d->bytesPerPixel = (uint8_t)(depth >> 3);
  d->stride = stride;

  return d;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Image_init(void)
{
  ImageData* d = Image::_dnull.init();
  d->reference.init(1);
  d->flags = NO_FLAGS;
}

} // Fog namespace
