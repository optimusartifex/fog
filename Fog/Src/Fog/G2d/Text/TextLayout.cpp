// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/G2d/Text/TextLayout.h>

namespace Fog {

// ============================================================================
// [Fog::GlyphShaper - Construction / Destruction]
// ============================================================================

GlyphShaper::GlyphShaper()
{
}

GlyphShaper::~GlyphShaper()
{
}

// ============================================================================
// [Fog::GlyphShaper - Clear]
// ============================================================================

void GlyphShaper::clear()
{
  _glyphRun.clear();
}

// ============================================================================
// [Fog::GlyphShaper - AddText]
// ============================================================================

err_t GlyphShaper::addText(const Font& font, const StringW& string)
{
  return addText(font, StubW(string.getData(), string.getLength()));
}

err_t GlyphShaper::addText(const Font& font, const StubW& string)
{
  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::TextLayout - Construction / Destruction]
// ============================================================================

TextLayout::TextLayout()
{
}

TextLayout::~TextLayout()
{
}

} // Fog namespace
