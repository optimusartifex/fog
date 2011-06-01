// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Static.h>
#include <Fog/Core/Memory/Memory.h>
#include <Fog/G2d/Text/GlyphBitmap.h>

namespace Fog {

// ============================================================================
// [Fog::GlyphBitmap - Statics]
// ============================================================================

static Static<GlyphBitmapData> _G2d_GlyphBitmap_dnull;

// ============================================================================
// [Fog::GlyphBitmap - Helpers]
// ============================================================================

static FOG_INLINE GlyphBitmapData* _G2d_GlyphBitmap_dalloc(const GlyphMetricsF& metrics, const Image& bitmap)
{
  GlyphBitmapData* d = reinterpret_cast<GlyphBitmapData*>(
    Memory::alloc(sizeof(GlyphBitmapData)));
  if (FOG_IS_NULL(d)) return d;

  d->refCount.init(1);
  d->metrics = metrics;
  d->bitmap[0].initCustom1(bitmap);

  return d;
}

static FOG_INLINE GlyphBitmapData* _G2d_GlyphBitmap_dref(GlyphBitmapData* d)
{
  d->refCount.inc();
  return d;
}

static FOG_INLINE void _G2d_GlyphBitmap_deref(GlyphBitmapData* d)
{
  if (d->refCount.deref())
  {
    d->bitmap[0].destroy();
    Memory::free(d);
  }
}

// ============================================================================
// [Fog::GlyphBitmap - Construction / Destruction]
// ============================================================================

GlyphBitmap::GlyphBitmap() :
  _d(_G2d_GlyphBitmap_dref(_G2d_GlyphBitmap_dnull))
{
}

GlyphBitmap::GlyphBitmap(const GlyphBitmap& other) :
  _d(_G2d_GlyphBitmap_dref(other._d))
{
}

GlyphBitmap::~GlyphBitmap()
{
  _G2d_GlyphBitmap_deref(_d);
}

// ============================================================================
// [Fog::GlyphBitmap - Create]
// ============================================================================

err_t GlyphBitmap::create(const GlyphMetricsF& metrics, const Image& bitmap)
{
  GlyphBitmapData* newd = _G2d_GlyphBitmap_dalloc(metrics, bitmap);
  if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

  _G2d_GlyphBitmap_deref(atomicPtrXchg(&_d, newd));
  return ERR_OK;
}

// ============================================================================
// [Fog::GlyphBitmap - Operator Overload]
// ============================================================================

GlyphBitmap& GlyphBitmap::operator=(const GlyphBitmap& other)
{
  _G2d_GlyphBitmap_deref(
    atomicPtrXchg(&_d, _G2d_GlyphBitmap_dref(other._d))
  );

  return *this;
}

// ============================================================================
// [Fog::G2d - Library Initializers]
// ============================================================================

FOG_NO_EXPORT void _g2d_glyphbitmap_init(void)
{
  GlyphBitmapData* d = _G2d_GlyphBitmap_dnull.instancep();

  d->refCount.init(1);
  d->metrics.reset();
  d->bitmap[0].init();
}

FOG_NO_EXPORT void _g2d_glyphbitmap_fini(void)
{
  GlyphBitmapData* d = _G2d_GlyphBitmap_dnull.instancep();

  d->refCount.dec();
  d->bitmap[0].destroy();
}

} // Fog namespace
