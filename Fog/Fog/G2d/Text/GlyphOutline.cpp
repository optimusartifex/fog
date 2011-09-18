// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/G2d/Text/GlyphOutline.h>

namespace Fog {

// ============================================================================
// [Fog::GlyphOutline - Statics]
// ============================================================================

static Static<GlyphOutlineData> _GlyphOutline_dnull;

// ============================================================================
// [Fog::GlyphOutline - Helpers]
// ============================================================================

static FOG_INLINE GlyphOutlineData* _GlyphOutline_dalloc(const GlyphMetricsF& metrics, const PathF& outline)
{
  GlyphOutlineData* d = reinterpret_cast<GlyphOutlineData*>(
    MemMgr::alloc(sizeof(GlyphOutlineData)));
  if (FOG_IS_NULL(d)) return d;

  d->reference.init(1);
  d->metrics = metrics;
  d->outline.initCustom1(outline);

  return d;
}

static FOG_INLINE GlyphOutlineData* _GlyphOutline_dref(GlyphOutlineData* d)
{
  d->reference.inc();
  return d;
}

static FOG_INLINE void _GlyphOutline_deref(GlyphOutlineData* d)
{
  if (d->reference.deref())
  {
    d->outline.destroy();
    MemMgr::free(d);
  }
}

// ============================================================================
// [Fog::GlyphOutline - Construction / Destruction]
// ============================================================================

GlyphOutline::GlyphOutline() :
  _d(_GlyphOutline_dref(&_GlyphOutline_dnull))
{
}

GlyphOutline::GlyphOutline(const GlyphOutline& other) :
  _d(_GlyphOutline_dref(other._d))
{
}

GlyphOutline::~GlyphOutline()
{
  _GlyphOutline_deref(_d);
}

// ============================================================================
// [Fog::GlyphOutline - Create]
// ============================================================================

err_t GlyphOutline::create(const GlyphMetricsF& metrics, const PathF& outline)
{
  GlyphOutlineData* newd = _GlyphOutline_dalloc(metrics, outline);
  if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

  _GlyphOutline_deref(atomicPtrXchg(&_d, newd));
  return ERR_OK;
}

// ============================================================================
// [Fog::GlyphOutline - Operator Overload]
// ============================================================================

GlyphOutline& GlyphOutline::operator=(const GlyphOutline& other)
{
  _GlyphOutline_deref(
    atomicPtrXchg(&_d, _GlyphOutline_dref(other._d))
  );

  return *this;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void GlyphOutline_init(void)
{
  GlyphOutlineData* d = &_GlyphOutline_dnull;

  d->reference.init(1);
  d->metrics.reset();
  d->outline.init();
}

FOG_NO_EXPORT void GlyphOutline_fini(void)
{
  GlyphOutlineData* d = &_GlyphOutline_dnull;

  d->reference.dec();
  d->outline.destroy();
}

} // Fog namespace
