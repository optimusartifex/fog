// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/G2d/Global/Constants.h>
#include <Fog/G2d/Font/NullFontEngine_p.h>
#include <Fog/G2d/Text/TextLayout.h>

namespace Fog {

// ============================================================================
// [Fog::NullFontFace]
// ============================================================================

NullFontFace::NullFontFace()
{
}

NullFontFace::~NullFontFace()
{
}

err_t NullFontFace::getGlyphSet(const Char* str, sysuint_t length, GlyphSet& glyphSet)
{
  FOG_UNUSED(str);
  FOG_UNUSED(length);
  FOG_UNUSED(glyphSet);

  return ERR_FONT_INVALID_FACE;
}

err_t NullFontFace::getOutline(const Char* str, sysuint_t length, PathD& dst)
{
  FOG_UNUSED(str);
  FOG_UNUSED(length);
  FOG_UNUSED(dst);

  return ERR_FONT_INVALID_FACE;
}

err_t NullFontFace::getTextExtents(const Char* str, sysuint_t length, TextExtents& extents)
{
  FOG_UNUSED(str);
  FOG_UNUSED(length);

  memset(&extents, 0, sizeof(TextExtents));
  return ERR_FONT_INVALID_FACE;
}

} // Fog namespace
