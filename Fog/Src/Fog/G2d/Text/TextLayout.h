// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_TEXTLAYOUT_H
#define _FOG_G2D_TEXT_TEXTLAYOUT_H

// [Dependencies]
#include <Fog/G2d/Text/Font.h>
#include <Fog/G2d/Text/TextRect.h>

namespace Fog {

//! @addtogroup Fog_G2d_Text
//! @{

// ============================================================================
// [Fog::GlyphItem]
// ============================================================================

struct FOG_NO_EXPORT GlyphItem
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getGlyphIndex() const { return _glyphIndex; }
  FOG_INLINE void setGlyphIndex(uint32_t index) { _glyphIndex = index; }

  FOG_INLINE uint32_t getProperties() const { return _properties; }
  FOG_INLINE void setProperties(uint32_t properties) { _properties = properties; }

  FOG_INLINE uint32_t getCluster() const { return _cluster; }
  FOG_INLINE void setCluster(uint32_t cluster) { _cluster = cluster; }

  FOG_INLINE uint16_t getComponent() const { return _component; }
  FOG_INLINE void setComponent(uint16_t component) { _component = component; }

  FOG_INLINE uint16_t getLigatureId() const { return _ligatureId; }
  FOG_INLINE void setLigatureId(uint32_t ligatureId) { _ligatureId = ligatureId; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    MemOps::zero_t<GlyphItem>(this);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------
  
  uint32_t _glyphIndex;
  uint32_t _properties;
  uint32_t _cluster;
  uint16_t _component;
  uint16_t _ligatureId;
};

// ============================================================================
// [Fog::GlyphPosition]
// ============================================================================

struct FOG_NO_EXPORT GlyphPosition
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const PointF& getPosition() const { return _position; }

  FOG_INLINE void setPosition(const PointF& pos) { _position = pos; }
  FOG_INLINE void setPosition(float x, float y) { _position.set(x, y); }

  FOG_INLINE const PointF& getAdvance() const { return _advance; }

  FOG_INLINE void setAdvance(const PointF& advance) { _advance = advance; }
  FOG_INLINE void setAdvance(float x, float y) { _advance.set(x, y); }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    MemOps::zero_t<GlyphPosition>(this);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  PointF _position;
  PointF _advance;

  uint32_t _newAdvance : 1;
  uint32_t _back : 15;
  int32_t _cursiveChain : 16;
};

// ============================================================================
// [Fog::GlyphRun]
// ============================================================================

struct FOG_NO_EXPORT GlyphRun
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getLength() const { return _itemList.getLength(); }

  FOG_INLINE const List<GlyphItem>& getItemList() const { return _itemList; }
  FOG_INLINE const List<GlyphPosition>& getPositionList() const { return _positionList; }

  // --------------------------------------------------------------------------
  // [Clear]
  // --------------------------------------------------------------------------

  FOG_INLINE void clear()
  {
    _itemList.clear();
    _positionList.clear();
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  List<GlyphItem> _itemList;
  List<GlyphPosition> _positionList;
};

// ============================================================================
// [Fog::GlyphShaper]
// ============================================================================

//! @brief Low level text shaper.
//!
//! Glyph layout is only useful to layout text which is contained in a font.
//! 
struct FOG_API GlyphShaper
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  GlyphShaper();
  ~GlyphShaper();

  // --------------------------------------------------------------------------
  // [Interface]
  // --------------------------------------------------------------------------

  void clear();

  err_t addText(const Font& font, const StringW& string);
  err_t addText(const Font& font, const StubW& string);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  GlyphRun _glyphRun;
  uint32_t _encoding;

private:
  FOG_NO_COPY(GlyphShaper)
};

// ============================================================================
// [Fog::TextLayout]
// ============================================================================

struct FOG_API TextLayout
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  TextLayout();
  virtual ~TextLayout();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  StringW _text;

private:
  FOG_NO_COPY(TextLayout)
};

// ============================================================================
// [Implemented-Later]
// ============================================================================

FOG_INLINE err_t Font::getOutlineFromGlyphRun(PathF& dst, uint32_t cntOp, const PointF& pt,
  const GlyphRun& glyphRun) const
{
  FOG_ASSERT(glyphRun._itemList.getLength() == glyphRun._positionList.getLength());

  const GlyphItem* glyphs = glyphRun._itemList.getData();
  const GlyphPosition* positions = glyphRun._positionList.getData();
  size_t length = glyphRun.getLength();

  return fog_api.font_getOutlineFromGlyphRunF(this, &dst, cntOp, &pt,
    &glyphs->_glyphIndex, sizeof(GlyphItem), &positions->_position, sizeof(GlyphPosition), length);
}

FOG_INLINE err_t Font::getOutlineFromGlyphRun(PathD& dst, uint32_t cntOp, const PointD& pt,
  const GlyphRun& glyphRun) const
{
  FOG_ASSERT(glyphRun._itemList.getLength() == glyphRun._positionList.getLength());

  const GlyphItem* glyphs = glyphRun._itemList.getData();
  const GlyphPosition* positions = glyphRun._positionList.getData();
  size_t length = glyphRun.getLength();

  return fog_api.font_getOutlineFromGlyphRunD(this, &dst, cntOp, &pt,
    &glyphs->_glyphIndex, sizeof(GlyphItem), &positions->_position, sizeof(GlyphPosition), length);
}

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TEXT_TEXTLAYOUT_H
