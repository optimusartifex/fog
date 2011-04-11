// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Collection/Util.h>
#include <Fog/Core/Memory/Memory.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/G2d/Font/GlyphSet.h>
#include <Fog/G2d/Global/Init_p.h>

namespace Fog {

// ============================================================================
// [Statics]
// ============================================================================

static void copyGlyphs(Glyph* dst, const Glyph* src, sysuint_t count)
{
  for (sysuint_t i = 0; i < count; i++)
    dst[i]._d = src[i]._d->ref();
}

static void freeGlyphs(Glyph* data, sysuint_t count)
{
  for (sysuint_t i = 0; i < count; i++)
    data[i]._d->deref();
}

// ============================================================================
// [Fog::GlyphSet]
// ============================================================================

Static<GlyphSet::Data> GlyphSet::_dnull;

GlyphSet::GlyphSet()
{
  _d = _dnull->refAlways();
}

GlyphSet::GlyphSet(const GlyphSet& other)
{
  _d = other._d->ref();
}

GlyphSet::~GlyphSet()
{
  _d->deref();
}

void GlyphSet::clear()
{
  if (_d->length == 0) return;

  if (_d->refCount.get() > 1)
  {
    atomicPtrXchg(&_d, _dnull->refAlways())->deref();
  }
  else
  {
    freeGlyphs(_d->glyphs(), _d->length);
    _d->length = 0;
    _d->extents.reset();
    _d->advance = 0;
  }
}

void GlyphSet::reset()
{
  atomicPtrXchg(&_d, _dnull->refAlways())->deref();
}

err_t GlyphSet::reserve(sysuint_t capacity)
{
  if (_d->refCount.get() > 1)
  {
    Data* newd = Data::alloc(capacity);
    if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

    newd->length = _d->length;
    newd->extents = _d->extents;
    newd->advance = _d->advance;
    copyGlyphs(newd->glyphs(), _d->glyphs(), _d->length);

    atomicPtrXchg(&_d, newd)->deref();
    return ERR_OK;
  }
  else
  {
    if (_d->capacity < capacity)
    {
      Data* newd = Data::realloc(_d, capacity);
      if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;
      _d = newd;
    }
    return ERR_OK;
  }
}

err_t GlyphSet::grow(sysuint_t by)
{
  sysuint_t before = _d->length;
  sysuint_t after = before + by;

  if (_d->refCount.get() > 1)
  {
    sysuint_t optimalCapacity = 
      Util::getGrowCapacity(sizeof(Data), sizeof(Glyph), before, after);

    Data* newd = Data::alloc(optimalCapacity);
    if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

    newd->length = _d->length;
    newd->extents = _d->extents;
    newd->advance = _d->advance;
    copyGlyphs(newd->glyphs(), _d->glyphs(), _d->length);

    atomicPtrXchg(&_d, newd)->deref();
    return ERR_OK;
  }
  else if (_d->capacity < after)
  {
    sysuint_t optimalCapacity = 
      Util::getGrowCapacity(sizeof(Data), sizeof(Glyph), before, after);

    Data* newd = Data::realloc(_d, optimalCapacity);
    if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

    _d = newd;
  }

  return ERR_OK;
}

err_t GlyphSet::begin(sysuint_t howMuch)
{
  // 128 is good for most operations
  if (howMuch == 0) howMuch = 128;

  return grow(howMuch);
}

err_t GlyphSet::end()
{
  sysuint_t i, len = _d->length;

  int advanceX = 0;
  int advanceY = 0;

  int x0 = 0;
  int y0 = 0;
  int x2 = 0;
  int y2 = 0;

  for (i = 0; i < len; i++)
  {
    GlyphData* gd = _d->glyphs()[i]._d;

    int gx0 = advanceX + gd->offset.x;
    int gy0 = advanceY + gd->offset.y;
    int gx2 = gx0 + gd->bitmap.getWidth();
    int gy2 = gy0 + gd->bitmap.getHeight();

    if (x0 > gx0) x0 = gx0;
    if (y0 > gy0) y0 = gy0;
    if (x2 < gx2) x2 = gx2;
    if (y2 < gy2) y2 = gy2;

    advanceX += gd->advance;
  }

  _d->extents.set(x0, y0, x2 - x0, y2 - y0);
  _d->advance = advanceX;

  return ERR_OK;
}

GlyphSet& GlyphSet::operator=(const GlyphSet& other)
{
  atomicPtrXchg(&_d, other._d->ref())->deref();
  return *this;
}

// ============================================================================
// [Fog::GlyphSet::Data]
// ============================================================================

GlyphSet::Data* GlyphSet::Data::ref() const
{
  if (flags & IsSharable)
    return refAlways();
  else
    return copy(const_cast<Data*>(this));
}

void GlyphSet::Data::deref()
{
  if (refCount.deref())
  {
    freeGlyphs(glyphs(), length);
    if (flags & IsDynamic) free(this);
  }
}

GlyphSet::Data* GlyphSet::Data::adopt(void* data, sysuint_t capacity)
{
  Data* d = reinterpret_cast<Data*>(data);

  d->refCount.init(1);
  d->flags = 0;
  d->capacity = capacity;
  d->length = 0;
  d->extents.reset();
  d->advance = 0;

  return d;
}

GlyphSet::Data* GlyphSet::Data::alloc(sysuint_t capacity)
{
  sysuint_t dsize = sizeof(Data) + capacity * sizeof(Glyph);

  Data* d = reinterpret_cast<Data*>(Memory::alloc(dsize));
  if (FOG_IS_NULL(d)) return NULL;

  d->refCount.init(1);
  d->flags = IsDynamic | IsSharable;
  d->capacity = capacity;
  d->length = 0;
  d->extents.reset();
  d->advance = 0;

  return d;
}

GlyphSet::Data* GlyphSet::Data::realloc(Data* d, sysuint_t capacity)
{
  sysuint_t dsize = sizeof(Data) + capacity * sizeof(Glyph);

  Data* newd = reinterpret_cast<Data*>(Memory::realloc(d, dsize));
  if (FOG_IS_NULL(newd)) return NULL;

  newd->capacity = capacity;
  return newd;
}

GlyphSet::Data* GlyphSet::Data::copy(const Data* d)
{
  if (d->length == 0) return _dnull->refAlways();

  Data* newd = alloc(d->capacity);
  if (FOG_IS_NULL(newd)) return NULL;

  newd->length = d->length;
  newd->extents = d->extents;
  newd->advance = d->advance;
  copyGlyphs(newd->glyphs(), d->glyphs(), d->length);

  return newd;
}

void GlyphSet::Data::free(Data* d)
{
  Memory::free(reinterpret_cast<void*>(d));
}

// ============================================================================
// [Fog::G2d - Library Initializers]
// ============================================================================

FOG_NO_EXPORT void _g2d_glyphset_init(void)
{
  GlyphSet::Data* d = GlyphSet::_dnull.instancep();
  d->refCount.init(1);
  d->flags = GlyphSet::Data::IsSharable;
  d->capacity = 0;
  d->length = 0;
  d->extents.set(0, 0, 0, 0);
  memset(d->data, 0, sizeof(d->data));
}

FOG_NO_EXPORT void _g2d_glyphset_fini(void)
{
  GlyphSet::Data* d = Fog::GlyphSet::_dnull.instancep();
  d->refCount.dec();
}

} // Fog namespace
