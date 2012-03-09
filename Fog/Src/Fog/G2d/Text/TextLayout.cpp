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
#include <Fog/G2d/Text/OpenType/OTCMap.h>
#include <Fog/G2d/Text/OpenType/OTEnum.h>
#include <Fog/G2d/Text/OpenType/OTFace.h>

namespace Fog {

// ============================================================================
// [Fog::GlyphShaper - Construction / Destruction]
// ============================================================================

GlyphShaper::GlyphShaper() :
  _encoding(OT_ENCODING_ID_UNICODE)
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
  OTFace* ot = font.getFace()->getOTFace();
  if (FOG_IS_NULL(ot))
    return ERR_FONT_INVALID_FACE;

  OTCMap* cmap = ot->getCMap();
  if (FOG_IS_NULL(cmap))
    return ERR_FONT_CMAP_NOT_FOUND;

  OTCMapContext cctx;
  cctx.init(cmap, _encoding);

  const CharW* sData = string.getData();
  size_t sLength = string.getComputedLength();

  GlyphItem* glyphs = _glyphRun._itemList._prepare(CONTAINER_OP_APPEND, sLength);
  cctx.getGlyphPlacement(&glyphs->_glyphIndex, sizeof(GlyphItem),
    reinterpret_cast<const uint16_t*>(sData), sLength);

  // TODO:
  GlyphPosition* pos = _glyphRun._positionList._prepare(CONTAINER_OP_APPEND, sLength);
  for (size_t i = 0; i < sLength; i++)
  {
    pos[i].reset();
    pos[i].setPosition(PointF(0.0f + i * 20.0f, 0.0f));
  }

  return ERR_OK;
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
