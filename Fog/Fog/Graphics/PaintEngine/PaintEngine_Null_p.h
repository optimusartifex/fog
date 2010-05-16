// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_PAINTENGINE_NULL_P_H
#define _FOG_GRAPHICS_PAINTENGINE_NULL_P_H

// [Dependencies]
#include <Fog/Graphics/PaintEngine.h>
#include <Fog/Graphics/PathStroker.h>

namespace Fog {

//! @addtogroup Fog_Graphics_Private
//! @{

// ============================================================================
// [Fog::NullPaintEngine]
// ============================================================================

//! @internal
struct FOG_HIDDEN NullPaintEngine : public PaintEngine
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  NullPaintEngine() {}
  virtual ~NullPaintEngine() {}

  // --------------------------------------------------------------------------
  // [Width / Height / Format]
  // --------------------------------------------------------------------------

  virtual int getWidth() const { return 0; }
  virtual int getHeight() const { return 0; }
  virtual uint32_t getFormat() const { return IMAGE_FORMAT_NULL; }

  // --------------------------------------------------------------------------
  // [Engine / Flush]
  // --------------------------------------------------------------------------

  virtual uint32_t getEngine() const { return PAINT_ENGINE_NULL; }
  virtual err_t setEngine(uint32_t engine, uint32_t threads) { return ERR_RT_INVALID_OBJECT; }

  virtual err_t flush(uint32_t flags) { return ERR_RT_INVALID_OBJECT; }

  // --------------------------------------------------------------------------
  // [Hints]
  // --------------------------------------------------------------------------

  virtual int getHint(uint32_t hint) const { return -1; }
  virtual err_t setHint(uint32_t hint, int value) { return ERR_RT_INVALID_OBJECT; }

  // --------------------------------------------------------------------------
  // [Meta]
  // --------------------------------------------------------------------------

  virtual err_t setMetaVars(const Region& region, const IntPoint& origin) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t resetMetaVars() { return ERR_RT_INVALID_OBJECT; }

  virtual err_t setUserVars(const Region& region, const IntPoint& origin) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t resetUserVars() { return ERR_RT_INVALID_OBJECT; }

  virtual Region getMetaRegion() const { return Region::infinite(); }
  virtual Region getUserRegion() const { return Region::infinite(); }

  virtual IntPoint getMetaOrigin() const { return IntPoint(0, 0); }
  virtual IntPoint getUserOrigin() const { return IntPoint(0, 0); }

  // --------------------------------------------------------------------------
  // [Paint Parameters]
  // --------------------------------------------------------------------------

  virtual uint32_t getSourceType() const { return PAINTER_SOURCE_ARGB; }

  virtual Argb getSourceArgb() const { return Argb(0x00000000); }
  virtual Pattern getSourcePattern() const { return Pattern(); }

  virtual err_t setSource(Argb argb) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t setSource(const Pattern& pattern) { return ERR_RT_INVALID_OBJECT; }

  virtual err_t resetSource() { return ERR_RT_INVALID_OBJECT; }

  virtual uint32_t getOperator() const { return OPERATOR_SRC_OVER; }
  virtual err_t setOperator(uint32_t op) { return ERR_RT_INVALID_OBJECT; }

  virtual float getAlpha() const { return 1.0f; }
  virtual err_t setAlpha(float alpha) { return ERR_RT_INVALID_OBJECT; }

  // --------------------------------------------------------------------------
  // [Clip Parameters]
  // --------------------------------------------------------------------------

  virtual uint32_t getClipRule() const { return FILL_DEFAULT; }
  virtual err_t setClipRule(uint32_t fillRule) { return ERR_RT_INVALID_OBJECT; }

  // --------------------------------------------------------------------------
  // [Fill Parameters]
  // --------------------------------------------------------------------------

  virtual uint32_t getFillRule() const { return FILL_DEFAULT; }
  virtual err_t setFillRule(uint32_t fillRule) { return ERR_RT_INVALID_OBJECT; }

  // --------------------------------------------------------------------------
  // [Stroke Parameters]
  // --------------------------------------------------------------------------

  virtual PathStrokeParams getStrokeParams() const { return PathStrokeParams(); }
  virtual err_t setStrokeParams(const PathStrokeParams& strokeParams) { return ERR_RT_INVALID_OBJECT; }

  virtual double getLineWidth() const { return 0.0; }
  virtual err_t setLineWidth(double lineWidth) { return ERR_RT_INVALID_OBJECT; }

  virtual uint32_t getStartCap() const { return 0; }
  virtual err_t setStartCap(uint32_t startCap) { return ERR_RT_INVALID_OBJECT; }

  virtual uint32_t getEndCap() const { return 0; }
  virtual err_t setEndCap(uint32_t endCap) { return ERR_RT_INVALID_OBJECT; }

  virtual err_t setLineCaps(uint32_t lineCaps) { return ERR_RT_INVALID_OBJECT; }

  virtual uint32_t getLineJoin() const { return 0; }
  virtual err_t setLineJoin(uint32_t lineJoin) { return ERR_RT_INVALID_OBJECT; }

  virtual double getMiterLimit() const { return 0.0; }
  virtual err_t setMiterLimit(double miterLimit) { return ERR_RT_INVALID_OBJECT; }

  virtual List<double> getDashes() const { return List<double>(); }
  virtual err_t setDashes(const double* dashes, sysuint_t count) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t setDashes(const List<double>& dashes) { return ERR_RT_INVALID_OBJECT; }

  virtual double getDashOffset() const { return 0.0; }
  virtual err_t setDashOffset(double offset) { return ERR_RT_INVALID_OBJECT; }

  // --------------------------------------------------------------------------
  // [Transformations]
  // --------------------------------------------------------------------------

  virtual DoubleMatrix getMatrix() const { return DoubleMatrix(); }
  virtual err_t setMatrix(const DoubleMatrix& m) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t resetMatrix() { return ERR_RT_INVALID_OBJECT; }

  virtual err_t rotate(double angle, uint32_t order) { return ERR_RT_INVALID_OBJECT; }

  virtual err_t scale(int sx, int sy, uint32_t order) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t scale(double sx, double sy, uint32_t order) { return ERR_RT_INVALID_OBJECT; }

  virtual err_t skew(double sx, double sy, uint32_t order) { return ERR_RT_INVALID_OBJECT; }

  virtual err_t translate(int x, int y, uint32_t order) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t translate(double x, double y, uint32_t order) { return ERR_RT_INVALID_OBJECT; }

  virtual err_t transform(const DoubleMatrix& m, uint32_t order) { return ERR_RT_INVALID_OBJECT; }

  virtual err_t worldToScreen(DoublePoint* pt) const { return ERR_RT_INVALID_OBJECT; }
  virtual err_t screenToWorld(DoublePoint* pt) const { return ERR_RT_INVALID_OBJECT; }

  virtual err_t alignPoint(DoublePoint* pt) const { return ERR_RT_INVALID_OBJECT; }

  // --------------------------------------------------------------------------
  // [State]
  // --------------------------------------------------------------------------

  virtual err_t save(uint32_t flags) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t restore() { return ERR_RT_INVALID_OBJECT; }

  // --------------------------------------------------------------------------
  // [Clipping]
  // --------------------------------------------------------------------------

  virtual err_t clipRect(const IntRect& rect, uint32_t clipOp) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t clipMask(const IntPoint& pt, const Image& mask, uint32_t clipOp) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t clipRegion(const Region& region, uint32_t clipOp) { return ERR_RT_INVALID_OBJECT; }

  virtual err_t clipRect(const DoubleRect& rect, uint32_t clipOp) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t clipMask(const DoublePoint& pt, const Image& mask, uint32_t clipOp) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t clipRects(const DoubleRect* r, sysuint_t count, uint32_t clipOp) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t clipRound(const DoubleRect& r, const DoublePoint& radius, uint32_t clipOp) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t clipEllipse(const DoublePoint& cp, const DoublePoint& r, uint32_t clipOp) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t clipArc(const DoublePoint& cp, const DoublePoint& r, double start, double sweep, uint32_t clipOp) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t clipPath(const DoublePath& path, bool stroke, uint32_t clipOp) { return ERR_RT_INVALID_OBJECT; }

  virtual err_t resetClip() { return ERR_RT_INVALID_OBJECT; }

  // --------------------------------------------------------------------------
  // [Raster Drawing]
  // --------------------------------------------------------------------------

  virtual err_t drawPoint(const IntPoint& p) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t drawLine(const IntPoint& start, const IntPoint& end) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t drawRect(const IntRect& r) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t drawRound(const IntRect& r, const IntPoint& radius) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t fillRect(const IntRect& r) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t fillRects(const IntRect* r, sysuint_t count) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t fillRound(const IntRect& r, const IntPoint& radius) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t fillRegion(const Region& region) { return ERR_RT_INVALID_OBJECT; }

  virtual err_t fillAll() { return ERR_RT_INVALID_OBJECT; }

  // --------------------------------------------------------------------------
  // [Vector Drawing]
  // --------------------------------------------------------------------------

  virtual err_t drawPoint(const DoublePoint& p) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t drawLine(const DoublePoint& start, const DoublePoint& end) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t drawLine(const DoublePoint* pts, sysuint_t count) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t drawPolygon(const DoublePoint* pts, sysuint_t count) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t drawRect(const DoubleRect& r) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t drawRects(const DoubleRect* r, sysuint_t count) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t drawRound(const DoubleRect& r, const DoublePoint& radius) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t drawEllipse(const DoublePoint& cp, const DoublePoint& r) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t drawArc(const DoublePoint& cp, const DoublePoint& r, double start, double sweep) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t drawPath(const DoublePath& path) { return ERR_RT_INVALID_OBJECT; }

  virtual err_t fillPolygon(const DoublePoint* pts, sysuint_t count) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t fillRect(const DoubleRect& r) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t fillRects(const DoubleRect* r, sysuint_t count) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t fillRound(const DoubleRect& r, const DoublePoint& radius) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t fillEllipse(const DoublePoint& cp, const DoublePoint& r) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t fillArc(const DoublePoint& cp, const DoublePoint& r, double start, double sweep) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t fillPath(const DoublePath& path) { return ERR_RT_INVALID_OBJECT; }

  // --------------------------------------------------------------------------
  // [Glyph Drawing]
  // --------------------------------------------------------------------------

  virtual err_t drawGlyph(const IntPoint&p, const Image& glyph, const IntRect* irect) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t drawGlyph(const DoublePoint&p, const Image& glyph, const IntRect* irect) { return ERR_RT_INVALID_OBJECT; }

  virtual err_t drawGlyph(const IntPoint& pt, const Glyph& glyph, const IntRect* grect) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t drawGlyph(const DoublePoint& pt, const Glyph& glyph, const IntRect* grect) { return ERR_RT_INVALID_OBJECT; }

  virtual err_t drawGlyphSet(const IntPoint& pt, const GlyphSet& glyphSet, const IntRect* clip) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t drawGlyphSet(const DoublePoint& pt, const GlyphSet& glyphSet, const IntRect* clip) { return ERR_RT_INVALID_OBJECT; }

  // --------------------------------------------------------------------------
  // [Text Drawing]
  // --------------------------------------------------------------------------

  virtual err_t drawText(const IntPoint& pt, const String& text, const Font& font, const IntRect* clip) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t drawText(const DoublePoint& pt, const String& text, const Font& font, const IntRect* clip) { return ERR_RT_INVALID_OBJECT; }

  virtual err_t drawText(const IntRect& rect, const String& text, const Font& font, uint32_t align, const IntRect* clip) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t drawText(const DoubleRect& rect, const String& text, const Font& font, uint32_t align, const IntRect* clip) { return ERR_RT_INVALID_OBJECT; }

  // --------------------------------------------------------------------------
  // [Image drawing]
  // --------------------------------------------------------------------------

  virtual err_t drawImage(const IntPoint& p, const Image& image, const IntRect* irect) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t drawImage(const DoublePoint& p, const Image& image, const IntRect* irect) { return ERR_RT_INVALID_OBJECT; }
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GRAPHICS_PAINTENGINE_NULL_P_H
