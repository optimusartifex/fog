// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/G2d/Imaging/ImagePalette.h>
#include <Fog/G2d/Render/RenderApi_p.h>
#include <Fog/G2d/Render/RenderConstants_p.h>

namespace Fog {

// ============================================================================
// [Fog::ImagePalette - Global]
// ============================================================================

static Static<ImagePaletteData> ImagePalette_dEmpty;
static Static<ImagePaletteData> ImagePalette_dGreyscale;

static Static<ImagePalette> ImagePalette_oEmpty;

// ============================================================================
// [Fog::ImagePalette - Construction / Destruction]
// ============================================================================

static void FOG_CDECL ImagePalette_ctor(ImagePalette* self)
{
  self->_d = ImagePalette_dEmpty->addRef();
}

static void FOG_CDECL ImagePalette_ctorCopy(ImagePalette* self, const ImagePalette* other)
{
  self->_d = other->_d->addRef();
}

static void FOG_CDECL ImagePalette_dtor(ImagePalette* self)
{
  self->_d->release();
}

// ============================================================================
// [Fog::ImagePalette - Sharing]
// ============================================================================

static err_t FOG_CDECL ImagePalette_detach(ImagePalette* self)
{
  ImagePaletteData* d = self->_d;

  if (d->reference.get() == 1)
    return ERR_OK;

  ImagePaletteData* newd = _api.imagepalette.dCreate();

  if (FOG_IS_NULL(newd))
    return ERR_RT_OUT_OF_MEMORY;

  newd->length = d->length;
  MemOps::copy(newd->data, d->data, 256 * sizeof(Argb32));

  atomicPtrXchg(&self->_d, newd)->release();
  return ERR_OK;
}

// ============================================================================
// [Fog::ImagePalette - Accessors]
// ============================================================================

static err_t FOG_CDECL ImagePalette_setDeep(ImagePalette* self, const ImagePalette* other)
{
  ImagePaletteData* d = self->_d;

  if (d->reference.get() > 1)
  {
    ImagePaletteData* newd = _api.imagepalette.dCreate();

    if (FOG_IS_NULL(newd))
      return ERR_RT_OUT_OF_MEMORY;

    newd->length = other->_d->length;
    MemOps::copy(newd->data, other->_d->data, 256 * sizeof(Argb32));

    atomicPtrXchg(&self->_d, newd)->release();
    return ERR_OK;
  }
  else
  {
    d->length = other->_d->length;
    MemOps::copy(d->data, other->_d->data, 256 * sizeof(Argb32));
    return ERR_OK;
  }
}

static err_t FOG_CDECL ImagePalette_setData(ImagePalette* self, const Range* range, const Argb32* data)
{
  size_t rStart = range->getStart();

  if (rStart >= 256)
    return ERR_RT_INVALID_ARGUMENT;

  size_t rEnd = Math::min<size_t>(256, range->getEnd());
  size_t rLen = rEnd - rStart;

  if (rLen == 0)
    return ERR_OK;

  ImagePaletteData* d = self->_d;
  if (d->reference.get() > 1)
  {
    ImagePaletteData* newd = _api.imagepalette.dCreate();

    if (FOG_IS_NULL(newd))
      return ERR_RT_OUT_OF_MEMORY;

    MemOps::copy(newd->data, d->data, sizeof(Argb32) * 256);
    atomicPtrXchg(&self->_d, newd)->release();

    d = newd;
  }

  d->length = Math::max<uint32_t>(d->length, (uint32_t)rEnd);

  _g2d_render.get_FuncsCompositeCore(IMAGE_FORMAT_XRGB32, COMPOSITE_SRC)->vblit_line[IMAGE_FORMAT_PRGB32](
    reinterpret_cast<uint8_t*>(d->data + rStart),
    reinterpret_cast<const uint8_t*>(data), (uint)rLen, NULL);

  return ERR_OK;
}

static err_t FOG_CDECL ImagePalette_setLength(ImagePalette* self, size_t length)
{
  ImagePaletteData* d = self->_d;

  if (FOG_UNLIKELY(length > 256))
    return ERR_RT_INVALID_ARGUMENT;

  uint32_t len32 = (uint32_t)length;
  if (d->length == len32)
    return ERR_OK;

  if (d->reference.get() > 1)
  {
    FOG_RETURN_ON_ERROR(_api.imagepalette.detach(self));
    d = self->_d;
  }

  d->length = len32;
  return ERR_OK;
}

// ============================================================================
// [Fog::ImagePalette - Clear / Reset]
// ============================================================================

static void FOG_CDECL ImagePalette_clear(ImagePalette* self)
{
  ImagePaletteData* d = self->_d;

  if (d->reference.get() > 1)
  {
    self->reset();
    return;
  }

  Argb32* data = d->data;
  for (uint32_t i = 0; i < 256; i++) data[i] = 0xFF000000;
  d->length = 256;
}

static void FOG_CDECL ImagePalette_reset(ImagePalette* self)
{
  atomicPtrXchg(&self->_d, ImagePalette_dEmpty->addRef())->release();
}

// ============================================================================
// [Fog::ImagePalette - Copy]
// ============================================================================

static err_t FOG_CDECL ImagePalette_copy(ImagePalette* self, const ImagePalette* other)
{
  atomicPtrXchg(&self->_d, other->_d->addRef())->release();
  return ERR_OK;
}

// ============================================================================
// [Fog::ImagePalette - Equality]
// ============================================================================

static bool FOG_CDECL ImagePalette_eq(const ImagePalette* a, const ImagePalette* b)
{
  const ImagePaletteData* a_d = a->_d;
  const ImagePaletteData* b_d = b->_d;

  if (a_d == b_d)
    return true;

  size_t length = a_d->length;

  if (length != b_d->length)
    return false;

  const Argb32* aData = a_d->data;
  const Argb32* bData = b_d->data;

  for (size_t i = 0; i < length; i++)
  {
    if (aData[i] != bData[i])
      return false;
  }

  return true;
}

// ============================================================================
// [Fog::ImagePalette - Find]
// ============================================================================

static uint8_t FOG_FASTCALL ImagePalette_findRgb_Default(const ImagePaletteData* d, uint32_t r, uint32_t g, uint32_t b)
{
  uint32_t i, best = 0;

  const Argb32* data = d->data;
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
// [Fog::ImagePalette - Data]
// ============================================================================

static ImagePaletteData* FOG_CDECL ImagePalette_dCreate(void)
{
  ImagePaletteData* d = reinterpret_cast<ImagePaletteData*>(MemMgr::alloc(sizeof(ImagePaletteData)));
  if (FOG_IS_NULL(d)) return NULL;

  d->reference.init(1);
  d->length = 256;
  d->findRgbFunc = ImagePalette_findRgb_Default;

  return d;
}

static ImagePaletteData* FOG_CDECL ImagePalette_dCreateGreyscale(uint32_t length)
{
  if (length <= 1 || length > 256)
    return ImagePalette_dEmpty->addRef();

  if (length == 256)
    return ImagePalette_dGreyscale->addRef();

  ImagePaletteData* d = _api.imagepalette.dCreate();
  if (FOG_IS_NULL(d))
    return ImagePalette_dEmpty->addRef();

  uint32_t i = 0;
  uint32_t c0;
  uint32_t inc = 0xFF000000 / (length - 1);

  c0 = 0x00000000;
  while (i < length)
  {
    d->data[i] = 0xFF000000 | ((c0 >> 24) * 0x00010101);
    i++;
    c0 += inc;
  }

  c0 = 0xFF000000;
  while (i < 256)
  {
    d->data[i] = c0;
    i++;
  }

  return d;
}

static ImagePaletteData* FOG_CDECL ImagePalette_dCreateColorCube(uint32_t r, uint32_t g, uint32_t b)
{
  if (r <= 1 || g <= 1 || b <= 1)
    return ImagePalette_dEmpty->addRef();

  ImagePaletteData* d = _api.imagepalette.dCreate();
  if (FOG_IS_NULL(d))
    return ImagePalette_dEmpty->addRef();

  r--;
  g--;
  b--;

  Argb32* data = d->data;

  uint32_t i = 0;

  uint32_t rPos, rVal, rInc = 0xFF000000 / r;
  uint32_t gPos, gVal, gInc = 0xFF000000 / g;
  uint32_t bPos, bVal, bInc = 0xFF000000 / b;

  for (rPos = 0, rVal = 0; rPos <= r; rPos++, rVal += rInc)
  {
    for (gPos = 0, gVal = 0; gPos <= g; gPos++, gVal += gInc)
    {
      for (bPos = 0, bVal = 0; bPos <= b; bPos++, bVal += bInc)
      {
        data[i] = Argb32(0xFF, rVal >> 24, gVal >> 24, bVal >> 24);
        if (++i == 256) goto _End;
      }
    }
  }

_End:
  return d;
}

static void FOG_CDECL ImagePalette_dFree(ImagePaletteData* d)
{
  if ((d->vType & VAR_FLAG_STATIC) == 0)
    MemMgr::free(d);
}

// ============================================================================
// [Fog::ImagePalette - Helpers]
// ============================================================================

static bool FOG_CDECL ImagePalette_isGreyscale(const Argb32* data, size_t length)
{
  size_t i;

  for (i = 0; i < length; i++)
  {
    if (data[i].getRed  () != data[i].getGreen() ||
        data[i].getGreen() != data[i].getBlue () )
    {
      return false;
    }
  }

  return true;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void ImagePalette_init(void)
{
  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  _api.imagepalette.ctor = ImagePalette_ctor;
  _api.imagepalette.ctorCopy = ImagePalette_ctorCopy;
  _api.imagepalette.dtor = ImagePalette_dtor;
  _api.imagepalette.detach = ImagePalette_detach;
  _api.imagepalette.setData = ImagePalette_setData;
  _api.imagepalette.setDeep = ImagePalette_setDeep;
  _api.imagepalette.setLength = ImagePalette_setLength;
  _api.imagepalette.clear = ImagePalette_clear;
  _api.imagepalette.reset = ImagePalette_reset;
  _api.imagepalette.copy = ImagePalette_copy;
  _api.imagepalette.eq = ImagePalette_eq;

  _api.imagepalette.dCreate = ImagePalette_dCreate;
  _api.imagepalette.dCreateGreyscale = ImagePalette_dCreateGreyscale;
  _api.imagepalette.dCreateColorCube = ImagePalette_dCreateColorCube;
  _api.imagepalette.dFree = ImagePalette_dFree;

  _api.imagepalette.isGreyscale = ImagePalette_isGreyscale;

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  ImagePaletteData* d;
  uint32_t i, c0;

  d = &ImagePalette_dEmpty;
  d->reference.init(1);
  d->vType = VAR_TYPE_IMAGE_PALETTE | VAR_FLAG_NONE;
  d->length = 256;
  d->findRgbFunc = ImagePalette_findRgb_Default;

  for (i = 0, c0 = 0xFF000000; i < 256; i++)
    d->data[i] = c0;

  d = &ImagePalette_dGreyscale;
  d->reference.init(1);
  d->vType = VAR_TYPE_IMAGE_PALETTE | VAR_FLAG_NONE;
  d->length = 256;
  d->findRgbFunc = ImagePalette_findRgb_Default;

  for (i = 0, c0 = 0xFF000000; i < 256; i++, c0 += 0x00010101)
    d->data[i] = c0;
}

} // Fog namespace
