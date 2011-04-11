// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/G2d/Font/Glyph.h>
#include <Fog/G2d/Global/Init_p.h>

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

// ============================================================================
// [Fog::G2d - Library Initializers]
// ============================================================================

FOG_NO_EXPORT void _g2d_glyph_init(void)
{
  Glyph::_dnull.init();
}

FOG_NO_EXPORT void _g2d_glyph_fini(void)
{
  Glyph::_dnull.instancep()->refCount.dec();
  Glyph::_dnull.destroy();
}

} // Fog namespace
