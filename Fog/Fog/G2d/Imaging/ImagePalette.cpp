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
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/Alloc.h>
#include <Fog/Core/Memory/Ops.h>
#include <Fog/G2d/Imaging/ImagePalette.h>
#include <Fog/G2d/Render/RenderApi_p.h>
#include <Fog/G2d/Render/RenderConstants_p.h>

namespace Fog {

// ============================================================================
// [Fog::ImagePalette - Helpers]
// ============================================================================

static ImagePaletteData* ImagePalette_dalloc(void)
{
  ImagePaletteData* d = reinterpret_cast<ImagePaletteData*>(Memory::alloc(sizeof(ImagePaletteData)));
  if (FOG_IS_NULL(d)) return NULL;

  d->refCount.init(1);
  d->length = 256;

  return d;
}

// ============================================================================
// [Fog::ImagePalette - Construction / Destruction]
// ============================================================================

ImagePalette::ImagePalette() : _d(_dnull->ref()) {}
ImagePalette::ImagePalette(const ImagePalette& other) : _d(other._d->ref()) {}
ImagePalette::~ImagePalette() { _d->deref(); }

// ============================================================================
// [Fog::ImagePalette - Data]
// ============================================================================

err_t ImagePalette::_detach()
{
  if (isDetached()) return ERR_OK;

  ImagePaletteData* newd = ImagePalette_dalloc();
  if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

  newd->length = _d->length;
  Memory::copy(newd->data, _d->data, sizeof(Argb32) * 256);

  atomicPtrXchg(&_d, newd)->deref();
  return ERR_OK;
}

err_t ImagePalette::setData(const ImagePalette& other)
{
  atomicPtrXchg(&_d, other._d->ref())->deref();
  return ERR_OK;
}

err_t ImagePalette::setDeep(const ImagePalette& other)
{
  if (!isDetached())
  {
    ImagePaletteData* newd = ImagePalette_dalloc();
    if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

    newd->length = other._d->length;
    Memory::copy(newd->data, other._d->data, sizeof(Argb32) * 256);

    atomicPtrXchg(&_d, newd)->deref();
  }
  else
  {
    _d->length = other._d->length;
    Memory::copy(_d->data, other._d->data, sizeof(Argb32) * 256);
  }

  return ERR_OK;
}

err_t ImagePalette::setData(const Range& range, const Argb32* entries)
{
  size_t rstart = range.getStart();
  if (FOG_UNLIKELY(rstart >= 256)) return ERR_RT_INVALID_ARGUMENT;

  size_t rend = Math::min<size_t>(256, range.getEnd());
  size_t rlen = rend - rstart;
  if (FOG_UNLIKELY(rlen == 0)) return ERR_OK;

  if (!isDetached())
  {
    ImagePaletteData* newd = ImagePalette_dalloc();
    if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

    Memory::copy(newd->data, _d->data, sizeof(Argb32) * 256);
    atomicPtrXchg(&_d, newd)->deref();
  }

  _d->length = Math::max<uint32_t>(_d->length, (uint32_t)rend);
  _g2d_render.get_FuncsCompositeCore(IMAGE_FORMAT_XRGB32, COMPOSITE_SRC)->vblit_line[IMAGE_FORMAT_PRGB32](
    reinterpret_cast<uint8_t*>(_d->data + rstart),
    reinterpret_cast<const uint8_t*>(entries), (int)(uint)rlen, NULL);

  return ERR_OK;
}

err_t ImagePalette::setLength(size_t length)
{
  if (FOG_UNLIKELY(length > 256))
    return ERR_RT_INVALID_ARGUMENT;

  uint32_t len32 = (uint32_t)length;

  if (FOG_UNLIKELY(_d->length == len32))
    return ERR_OK;

  FOG_RETURN_ON_ERROR(detach());
  _d->length = len32;

  return ERR_OK;
}

// ============================================================================
// [Fog::ImagePalette - Clear / Reset]
// ============================================================================

void ImagePalette::clear()
{
  if (!isDetached()) { reset(); return; }

  for (uint32_t i = 0; i < 256; i++) _d->data[i] = 0xFF000000;
  _d->length = 256;
}

void ImagePalette::reset()
{
  atomicPtrXchg(&_d, _dnull->ref())->deref();
}

// ============================================================================
// [Fog::ImagePalette - Find]
// ============================================================================

uint8_t ImagePalette::findColor(uint8_t r, uint8_t g, uint8_t b) const
{
  uint32_t i, best = 0;

  const Argb32* data = _d->data;
  int smallest = INT_MAX;

  for (i = 0; i < 256; i++, data++)
  {
    int rd = (int)data->getRed  () - (int)r;
    int gd = (int)data->getGreen() - (int)g;
    int bd = (int)data->getBlue () - (int)b;

    int dist = (rd * rd) + (gd * gd) + (bd * bd);
    if (dist < smallest)
    {
      if (dist == 0) return (uint8_t)i;
      best = i;
      smallest = dist;
    }
  }

  return (uint8_t)best;
}

// ============================================================================
// [Fog::ImagePalette - Statics]
// ============================================================================

Static<ImagePaletteData> ImagePalette::_dnull;
static Static<ImagePaletteData> ImagePalette_dgrey;

ImagePalette ImagePalette::fromGreyscale(uint count)
{
  ImagePaletteData* d = NULL;

  if (count <= 1 || count > 256) { goto _End; }
  if (count == 256) { d = ImagePalette_dgrey->ref(); goto _Ret; }

  d = ImagePalette_dalloc();
  if (FOG_LIKELY(d != NULL))
  {
    uint32_t i = 0;
    uint32_t cval = 0;
    uint32_t cinc = 0xFF000000 / (count - 1);

    for (; i < count; i++, cval += cinc);
      d->data[i] = 0xFF000000 | ((cval >> 24) * 0x00010101);

    for (; i < 256; i++)
      d->data[i] = 0xFF000000;
  }

_End:
  if (FOG_IS_NULL(d)) d = _dnull->ref();

_Ret:
  return ImagePalette(d);
}

ImagePalette ImagePalette::fromCube(uint32_t nr, uint32_t ng, uint32_t nb)
{
  ImagePaletteData* d = NULL;

  if (nr <= 1 || ng <= 1 || nb <= 1)
    goto _End;

  d = ImagePalette_dalloc();
  if (FOG_LIKELY(d != NULL))
  {
    nr--;
    ng--;
    nb--;

    Argb32* colors = d->data;

    uint32_t i = 0;
    uint32_t rpos, rval, rinc = 0xFF000000 / nr;
    uint32_t gpos, gval, ginc = 0xFF000000 / ng;
    uint32_t bpos, bval, binc = 0xFF000000 / nb;

    for (rpos = 0, rval = 0; rpos <= nr; rpos++, rval += rinc)
    {
      for (gpos = 0, gval = 0; gpos <= ng; gpos++, gval += ginc)
      {
        for (bpos = 0, bval = 0; bpos <= nb; bpos++, bval += binc)
        {
          colors[i] = Argb32(0xFF, rval >> 24, gval >> 24, bval >> 24);
          if (++i == 256) goto _End;
        }
      }
    }
  }

_End:
  if (FOG_IS_NULL(d)) d = _dnull->ref();
  return ImagePalette(d);
}

bool ImagePalette::isGreyscale(const Argb32* data, size_t count)
{
  size_t i;

  for (i = 0; i < count; i++)
  {
    if (data[i].getRed  () != data[i].getGreen() ||
        data[i].getGreen() != data[i].getBlue () )
    {
      break;
    }
  }

  return i == count;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void ImagePalette_init(void)
{
  ImagePaletteData* d;

  uint32_t i;
  uint32_t c0;

  // --------------------------------------------------------------------------
  // Setup 'Null' palette.
  // --------------------------------------------------------------------------

  d = ImagePalette::_dnull.instancep();
  d->refCount.init(1);
  d->length = 256;

  for (i = 0, c0 = 0xFF000000; i < 256; i++)
    d->data[i] = c0;

  // --------------------------------------------------------------------------
  // Setup 'Greyscale' palette
  // --------------------------------------------------------------------------

  d = ImagePalette_dgrey.instancep();
  d->refCount.init(1);
  d->length = 256;

  for (i = 0, c0 = 0xFF000000; i < 256; i++, c0 += 0x00010101)
    d->data[i] = c0;
}

} // Fog namespace
