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
#include <Fog/Core/Memory.h>
#include <Fog/Core/Static.h>
#include <Fog/Graphics/Palette.h>
#include <Fog/Graphics/RasterEngine_p.h>

namespace Fog {

// ============================================================================
// [Fog::Palette]
// ============================================================================

Static<PaletteData> Palette::sharedNull;
Static<PaletteData> Palette::sharedGrey;
Static<PaletteData> Palette::sharedA8;

Palette::Palette() : _d(sharedNull->ref()) {}
Palette::Palette(const Palette& other) : _d(other._d->ref()) {}

Palette::~Palette()
{
  _d->deref();
}

err_t Palette::_detach()
{
  if (isDetached()) return ERR_OK;

  PaletteData* newd = reinterpret_cast<PaletteData*>(
    Memory::alloc(sizeof(PaletteData)));
  if (!newd) return ERR_RT_OUT_OF_MEMORY;

  newd->refCount.init(1);
  newd->isAlphaUsed = _d->isAlphaUsed;
  Memory::copy(newd->data, _d->data, sizeof(Argb) * 512);

  atomicPtrXchg(&_d, newd)->deref();
  return ERR_OK;
}

void Palette::free()
{
  atomicPtrXchg(&_d, sharedNull->ref())->deref();
}

void Palette::clear()
{
  if (isDetached())
    Memory::zero(_d->data, 512 * sizeof(Argb));
  else
    atomicPtrXchg(&_d, sharedNull->ref())->deref();
}

err_t Palette::set(const Palette& other)
{
  atomicPtrXchg(&_d, other._d->ref())->deref();
  return ERR_OK;
}

err_t Palette::setDeep(const Palette& other)
{
  err_t err = detach();
  if (err) return err;

  Memory::copy(_d->data, other._d->data, 256 * 2 * sizeof(Argb));
  return ERR_OK;
}

err_t Palette::setArgb32(sysuint_t index, const Argb* pal, sysuint_t count)
{
  if (index > 255 || count == 0) return ERR_RT_INVALID_ARGUMENT;
  if (256 - index > count) count = 256 - index;

  err_t err = detach();
  if (err) return err;

  uint32_t oldContainsAlpha = _d->isAlphaUsed ? isAlphaUsed(_d->data + INDEX_ARGB32 + index, count) : false;
  uint32_t newContainsAlpha = isAlphaUsed(pal, count);

  // Copy data to palette.
  Memory::copy(_d->data + INDEX_ARGB32 + index, pal, sizeof(Argb) * count);

  // Premultiply.
  if (newContainsAlpha)
  {
    RasterEngine::functionMap->dib.convert[PIXEL_FORMAT_PRGB32][PIXEL_FORMAT_ARGB32](
      reinterpret_cast<uint8_t*>(_d->data + INDEX_PRGB32 + index),
      reinterpret_cast<const uint8_t*>(pal), count, NULL);
  }
  else
  {
    RasterEngine::functionMap->dib.memcpy32(
      reinterpret_cast<uint8_t*>(_d->data + INDEX_PRGB32 + index),
      reinterpret_cast<const uint8_t*>(pal), count, NULL);
  }

  if (oldContainsAlpha && !newContainsAlpha)
  {
    _d->isAlphaUsed = isAlphaUsed(_d->data, 256);
  }
  else if (!oldContainsAlpha && newContainsAlpha)
  {
    _d->isAlphaUsed = true;
  }

  return ERR_OK;
}

err_t Palette::setXrgb32(sysuint_t index, const Argb* pal, sysuint_t count)
{
  if (index > 255 || count == 0) return ERR_RT_INVALID_ARGUMENT;
  if (256 - index > count) count = 256 - index;

  err_t err = detach();
  if (err) return err;

  // Copy data to palette.
  uint32_t updateAlpha = _d->isAlphaUsed ? isAlphaUsed(_d->data + INDEX_ARGB32 + index, count) : 0;

  RasterEngine::functionMap->dib.convert[PIXEL_FORMAT_ARGB32][PIXEL_FORMAT_XRGB32](
    reinterpret_cast<uint8_t*>(_d->data + INDEX_ARGB32 + index),
    reinterpret_cast<const uint8_t*>(pal), count, NULL);
  RasterEngine::functionMap->dib.convert[PIXEL_FORMAT_PRGB32][PIXEL_FORMAT_XRGB32](
    reinterpret_cast<uint8_t*>(_d->data + INDEX_PRGB32 + index),
    reinterpret_cast<const uint8_t*>(pal), count, NULL);

  if (updateAlpha) _d->isAlphaUsed = isAlphaUsed(_d->data, 256);
  return ERR_OK;
}

uint8_t Palette::findColor(uint8_t r, uint8_t g, uint8_t b) const
{
  sysuint_t i, best = 0;
  const Argb* data = _d->data;
  int smallest = INT_MAX;

  for (i = 0; i < 256; i++, data++)
  {
    int rd = (int)data->r - (int)r;
    int gd = (int)data->g - (int)g;
    int bd = (int)data->b - (int)b;

    int dist = (rd * rd) + (gd * gd) + (bd * bd);
    if (dist < smallest)
    {
      if (dist == 0) return i;
      best = i;
      smallest = dist;
    }
  }
  return (uint8_t)best;
}

void Palette::update()
{
  uint32_t alpha = isAlphaUsed(_d->data + INDEX_ARGB32, 256);

  // Premultiply all.
  if (alpha)
  {
    RasterEngine::functionMap->dib.convert[PIXEL_FORMAT_PRGB32][PIXEL_FORMAT_ARGB32](
      (uint8_t*)(_d->data + INDEX_PRGB32),
      (uint8_t*)(_d->data + INDEX_ARGB32), 256, NULL);
  }
  else
  {
    RasterEngine::functionMap->dib.memcpy32(
      (uint8_t*)(_d->data + INDEX_PRGB32),
      (uint8_t*)(_d->data + INDEX_ARGB32), 256, NULL);
  }

  _d->isAlphaUsed = alpha;
}

Palette Palette::greyscale()
{
  return Palette(sharedGrey->ref());
}

Palette Palette::a8()
{
  return Palette(sharedA8->ref());
}

Palette Palette::colorCube(int nr, int ng, int nb)
{
  Palette pal;
  if (pal.detach()) return pal;
  if (nr <= 0 || ng <= 0 || nb <= 0) return pal;

  Argb* data = pal._d->data;

  int i = 0, r, g, b;

  for (r = 0; r < nr; r++)
  {
    for (g = 0; g < ng; g++)
    {
      for (b = 0; b < nb; b++)
      {
        int palr = 0;
        int palg = 0;
        int palb = 0;

        if (nr > 1) palr = (r * 255) / (nr - 1);
        if (ng > 1) palg = (g * 255) / (ng - 1);
        if (nb > 1) palb = (b * 255) / (nb - 1);

        Argb color(0xFF, palr, palg, palb);
        data[INDEX_ARGB32 + i] = color;
        data[INDEX_PRGB32 + i] = color;

        if (++i == 256) goto end;
      }
    }
  }

end:
  return pal;
}

// static
bool Palette::isGreyOnly(const Argb* data, sysuint_t count)
{
  sysuint_t i;

  for (i = count; i; i--, data++)
  {
    if (data->a != 0xFF || data->r != data->g || data->g != data->b) break;
  }

  return i == 0;
}

bool Palette::isAlphaUsed(const Argb* data, sysuint_t count)
{
  for (int i = 0; i < 256; i++)
  {
    if (data[i].a != 0xFF) return true;
  }

  return false;
}

} // Fog namespace

// ============================================================================
// [Library Initializers]
// ============================================================================

FOG_INIT_DECLARE err_t fog_palette_init(void)
{
  using namespace Fog;

  PaletteData* d;
  uint32_t i, c0;

  Palette::sharedNull;
  Palette::sharedGrey;

  // Setup null palette;
  d = Palette::sharedNull.instancep();
  d->refCount.init(1);
  d->isAlphaUsed = false;
  Memory::zero(d->data, 512 * sizeof(Argb));

  for (i = 0, c0 = 0xFF000000; i < 256; i++)
  {
    d->data[Palette::INDEX_ARGB32 + i] = c0;
    d->data[Palette::INDEX_PRGB32 + i] = c0;
  }

  // Setup greyscale palette;
  d = Palette::sharedGrey.instancep();
  d->refCount.init(1);
  d->isAlphaUsed = false;

  for (i = 0, c0 = 0xFF000000; i < 256; i++, c0 += 0x00010101)
  {
    d->data[Palette::INDEX_ARGB32 + i] = c0;
    d->data[Palette::INDEX_PRGB32 + i] = c0;
  }

  // Setup alpha8 palette;
  d = Palette::sharedA8.instancep();
  d->refCount.init(1);
  d->isAlphaUsed = true;

  for (i = 0, c0 = 0x00000000; i < 256; i++, c0 += 0x01000000)
  {
    d->data[Palette::INDEX_ARGB32 + i] = c0;
    d->data[Palette::INDEX_PRGB32 + i] = c0;
  }

  return ERR_OK;
}

FOG_INIT_DECLARE void fog_palette_shutdown(void)
{
  using namespace Fog;

  Palette::sharedNull.instancep()->refCount.dec();
  Palette::sharedGrey.instancep()->refCount.dec();
  Palette::sharedA8.instancep()->refCount.dec();
}
