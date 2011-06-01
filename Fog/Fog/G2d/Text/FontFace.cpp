// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Internal_Core_p.h>
#include <Fog/Core/Tools/TextIterator.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Text/GlyphOutline.h>
#include <Fog/G2d/Text/GlyphOutlineCache.h>
#include <Fog/G2d/Text/Font.h>
#include <Fog/G2d/Text/FontFace.h>

namespace Fog {

// ============================================================================
// [Fog::FontFace - Construction / Destruction]
// ============================================================================

FontFace::FontFace()
{
  refCount.init(1);

  id = FONT_FACE_NULL;
  features = NO_FLAGS;

  designEm = 0.0f;
  designMetrics.reset();
}

FontFace::~FontFace()
{
}

// ============================================================================
// [Fog::FontFace - Private]
// ============================================================================

template<typename NumT, bool UseKerning>
static FOG_INLINE err_t _G2d_FontFace_getTextOutline(FontFace* self,
  NumT_(Path)& dst,
  GlyphOutlineCache* outlineCache,
  const FontKerningTableF* kerningTable,
  const FontData* d, const NumT_(Point)& pt, const Utf16& str, void* ctx)
{
  // Initialize text iterator and check if we can continue. Text iterator
  // expects correct text, but can detect basic surrogate failures at the begin
  // or at the end of a given string.
  TextIterator ti;
  FOG_RETURN_ON_ERROR(ti.assign(str));

  if (!ti.hasNext()) return ERR_OK;

  // Build the transform.
  float scale = d->scale;
  NumT_(Transform) transform(NumI_(Transform)::fromScaling(NumT(scale), NumT(scale)));
  transform.translate(pt, MATRIX_ORDER_APPEND);

  // Two chars are only used when specialized to use kerning.
  uint32_t uc0; if (UseKerning) uc0 = 0;
  uint32_t uc1; if (UseKerning) uc1 = ti.nextU32();

  do {
    NumT_(Point) advance(UNINITIALIZED);

    if (UseKerning)
    {
      uc0 = uc1;
      uc1 = ti.hasNext() ? ti.nextU32() : 0xFFFFFFFF;
    }
    else
    {
      uc0 = ti.nextU32();
    }

    // ------------------------------------------------------------------------
    // [Glyph]
    // ------------------------------------------------------------------------

    const GlyphOutline& glyphOutline = outlineCache->get(uc0);
    if (FOG_UNLIKELY(glyphOutline._isNull()))
    {
      PathF path;
      GlyphMetricsF metrics;

      FOG_RETURN_ON_ERROR(self->_renderGlyphOutline(path, metrics, d, uc0, ctx));

      if (FOG_LIKELY(uc0 <= UNICHAR_MAX))
      {
        GlyphOutline tmpOutline;
        if (tmpOutline.create(metrics, path) == ERR_OK)
          outlineCache->put(uc0, tmpOutline);
      }

      FOG_RETURN_ON_ERROR(dst.appendTransformed(path, transform));
      advance = metrics.getHorizontalAdvance();
    }
    else
    {
      const PathF& path = glyphOutline.getOutline();
      const GlyphMetricsF& metrics = glyphOutline.getMetrics();

      FOG_RETURN_ON_ERROR(dst.appendTransformed(path, transform));
      advance = metrics.getHorizontalAdvance();
    }

    // ------------------------------------------------------------------------
    // [Kerning]
    // ------------------------------------------------------------------------

    if (UseKerning)
    {
      FOG_ASSERT(kerningTable != NULL);

      const FontKerningPairF* pair = kerningTable->find(uc0, uc1);
      if (pair != NULL) advance.x += pair->amount;
    }

    // ------------------------------------------------------------------------
    // [Advance]
    // ------------------------------------------------------------------------

    transform.translate(advance);

  } while ( (UseKerning && uc1 != 0xFFFFFFFF) || (!UseKerning && ti.hasNext()) );

  return ERR_OK;
}

err_t FontFace::_getTextOutline(PathF& dst, GlyphOutlineCache* outlineCache, const FontData* d, const PointF& pt, const Utf16& str, void* ctx)
{
  const FontKerningTableF* kerningTable = NULL;
  if (d->hints.getKerning() == FONT_KERNING_ENABLED)
    kerningTable = getKerningTable(d);

  if (kerningTable)
    return _G2d_FontFace_getTextOutline<float, true >(this, dst, outlineCache, kerningTable, d, pt, str, ctx);
  else
    return _G2d_FontFace_getTextOutline<float, false>(this, dst, outlineCache, kerningTable, d, pt, str, ctx);
}

err_t FontFace::_getTextOutline(PathD& dst, GlyphOutlineCache* outlineCache, const FontData* d, const PointD& pt, const Utf16& str, void* ctx)
{
  const FontKerningTableF* kerningTable = NULL;
  if (d->hints.getKerning() == FONT_KERNING_ENABLED)
    kerningTable = getKerningTable(d);

  if (kerningTable)
    return _G2d_FontFace_getTextOutline<double, true >(this, dst, outlineCache, kerningTable, d, pt, str, ctx);
  else
    return _G2d_FontFace_getTextOutline<double, false>(this, dst, outlineCache, kerningTable, d, pt, str, ctx);
}

} // Fog namespace
