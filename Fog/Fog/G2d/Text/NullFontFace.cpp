// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/G2d/Text/NullFontFace_p.h>

namespace Fog {

// ============================================================================
// [Fog::NullFontFace - Construction / Destruction]
// ============================================================================

NullFontFace::NullFontFace()
{
  id = FONT_FACE_NULL;
  features = NO_FLAGS;
}

NullFontFace::~NullFontFace()
{
}

// ============================================================================
// [Fog::NullFontFace - Interface]
// ============================================================================

err_t NullFontFace::getTextOutline(PathF& dst, const FontData* d, const PointF& pt, const Utf16& str)
{
  return ERR_FONT_INVALID_FACE;
}

err_t NullFontFace::getTextOutline(PathD& dst, const FontData* d, const PointD& pt, const Utf16& str)
{
  return ERR_FONT_INVALID_FACE;
}

err_t NullFontFace::getTextExtents(TextExtents& extents, const FontData* d, const Utf16& str)
{
  return ERR_FONT_INVALID_FACE;
}

FontKerningTableF* NullFontFace::getKerningTable(const FontData* d)
{
  return NULL;
}

// ============================================================================
// [Fog::NullFontFace - Private]
// ============================================================================

err_t NullFontFace::_renderGlyphOutline(PathF& dst, GlyphMetricsF& metrics, const FontData* d, uint32_t uc, void* ctx)
{
  return ERR_FONT_INVALID_FACE;
}

err_t NullFontFace::_renderGlyphOutline(PathD& dst, GlyphMetricsF& metrics, const FontData* d, uint32_t uc, void* ctx)
{
  return ERR_FONT_INVALID_FACE;
}

// ============================================================================
// [Fog::NullFontFace - Statics]
// ============================================================================

Static<FontFace> NullFontFace::_dnull;

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void FontFace_initNull(void)
{
  NullFontFace* d = reinterpret_cast<NullFontFace*>(NullFontFace::_dnull.instancep());

  fog_new_p(d) NullFontFace();
}

FOG_NO_EXPORT void FontFace_finiNull(void)
{
  NullFontFace* d = reinterpret_cast<NullFontFace*>(NullFontFace::_dnull.instancep());

  d->refCount.dec();
  d->~NullFontFace();
}

} // Fog namespace
