// [Fog-Graphics]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Graphics/Glyph.h>

namespace Fog {

// ============================================================================
// [Fog::Glyph]
// ============================================================================

Static<GlyphData> Glyph::_dnull;

// ============================================================================
// [Fog::GlyphData]
// ============================================================================

GlyphData::GlyphData() :
  offset(0, 0),
  beginWidth(0),
  endWidth(0),
  advance(0)
{
  refCount.init(1);
}

GlyphData::GlyphData(const GlyphData* other) :
  bitmap(other->bitmap),
  offset(other->offset),
  beginWidth(other->beginWidth),
  endWidth(other->endWidth),
  advance(other->advance)
{
  refCount.init(1);
}

GlyphData::~GlyphData()
{
}

// ============================================================================
// [Fog::Glyph]
// ============================================================================

err_t Glyph::_detach()
{
  if (isDetached()) return ERR_OK;

  GlyphData* newd = fog_new GlyphData(_d);
  if (newd == NULL) return ERR_RT_OUT_OF_MEMORY;

  atomicPtrXchg(&_d, newd)->deref();
  return ERR_OK;
}

err_t Glyph::setBitmap(const Image& bitmap)
{
  FOG_RETURN_ON_ERROR(detach());

  return _d->bitmap.set(bitmap);
}

err_t Glyph::setOffset(const PointI& offset)
{
  FOG_RETURN_ON_ERROR(detach());

  _d->offset = offset;
  return ERR_OK;
}

Glyph& Glyph::operator=(const Glyph& other)
{
  atomicPtrXchg(&_d, other._d->ref())->deref();
  return *this;
}

} // Fog namespace

// ============================================================================
// [Library Initializers]
// ============================================================================

FOG_INIT_DECLARE err_t fog_glyph_init(void)
{
  using namespace Fog;

  Glyph::_dnull.init();
  return ERR_OK;
}

FOG_INIT_DECLARE void fog_glyph_shutdown(void)
{
  using namespace Fog;

  Glyph::_dnull.instancep()->refCount.dec();
  Glyph::_dnull.destroy();
}
