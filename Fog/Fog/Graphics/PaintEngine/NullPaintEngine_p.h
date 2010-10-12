// [Fog-Graphics]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_PAINTENGINE_NULLPAINTENGINE_P_H
#define _FOG_GRAPHICS_PAINTENGINE_NULLPAINTENGINE_P_H

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
//!
//! @brief Paint engine that does nothing.
//!
//! Calling the paint or other methods will result in returning
//! @ref ERR_RT_INVALID_OBJECT error code.
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

  virtual err_t setMetaVars(const Region& region, const PointI& origin) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t resetMetaVars() { return ERR_RT_INVALID_OBJECT; }

  virtual err_t setUserVars(const Region& region, const PointI& origin) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t resetUserVars() { return ERR_RT_INVALID_OBJECT; }

  virtual Region getMetaRegion() const { return Region::infinite(); }
  virtual Region getUserRegion() const { return Region::infinite(); }

  virtual PointI getMetaOrigin() const { return PointI(0, 0); }
  virtual PointI getUserOrigin() const { return PointI(0, 0); }

  // --------------------------------------------------------------------------
  // [Paint Parameters]
  // --------------------------------------------------------------------------

  virtual uint32_t getSourceType() const { return PAINTER_SOURCE_ARGB; }

  virtual ArgbI getSourceArgb() const { return ArgbI(0x00000000); }
  virtual Pattern getSourcePattern() const { return Pattern(); }

  virtual err_t setSource(ArgbI argb) { return ERR_RT_INVALID_OBJECT; }
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

  virtual TransformD getTransform() const { return TransformD(); }
  virtual err_t setTransform(const TransformD& transform) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t resetTransform() { return ERR_RT_INVALID_OBJECT; }

  virtual err_t rotate(double angle, uint32_t order) { return ERR_RT_INVALID_OBJECT; }

  virtual err_t scale(int sx, int sy, uint32_t order) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t scale(double sx, double sy, uint32_t order) { return ERR_RT_INVALID_OBJECT; }

  virtual err_t skew(double sx, double sy, uint32_t order) { return ERR_RT_INVALID_OBJECT; }

  virtual err_t translate(int x, int y, uint32_t order) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t translate(double x, double y, uint32_t order) { return ERR_RT_INVALID_OBJECT; }

  virtual err_t transform(const TransformD& m, uint32_t order) { return ERR_RT_INVALID_OBJECT; }

  virtual err_t worldToScreen(PointD* pt) const { return ERR_RT_INVALID_OBJECT; }
  virtual err_t screenToWorld(PointD* pt) const { return ERR_RT_INVALID_OBJECT; }

  virtual err_t alignPoint(PointD* pt) const { return ERR_RT_INVALID_OBJECT; }

  // --------------------------------------------------------------------------
  // [State]
  // --------------------------------------------------------------------------

  virtual err_t save(uint32_t flags) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t restore() { return ERR_RT_INVALID_OBJECT; }

  // --------------------------------------------------------------------------
  // [Clipping]
  // --------------------------------------------------------------------------

  virtual err_t clipRect(const RectI& rect, uint32_t clipOp) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t clipMask(const PointI& pt, const Image& mask, uint32_t clipOp) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t clipRegion(const Region& region, uint32_t clipOp) { return ERR_RT_INVALID_OBJECT; }

  virtual err_t clipRect(const RectD& rect, uint32_t clipOp) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t clipMask(const PointD& pt, const Image& mask, uint32_t clipOp) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t clipRects(const RectD* r, sysuint_t count, uint32_t clipOp) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t clipRound(const RectD& r, const PointD& radius, uint32_t clipOp) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t clipEllipse(const PointD& cp, const PointD& r, uint32_t clipOp) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t clipArc(const PointD& cp, const PointD& r, double start, double sweep, uint32_t clipOp) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t clipPath(const PathD& path, bool stroke, uint32_t clipOp) { return ERR_RT_INVALID_OBJECT; }

  virtual err_t resetClip() { return ERR_RT_INVALID_OBJECT; }

  // --------------------------------------------------------------------------
  // [Raster Drawing]
  // --------------------------------------------------------------------------

  virtual err_t drawPoint(const PointI& p) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t drawLine(const PointI& start, const PointI& end) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t drawRect(const RectI& r) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t drawRound(const RectI& r, const PointI& radius) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t fillRect(const RectI& r) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t fillRects(const RectI* r, sysuint_t count) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t fillRound(const RectI& r, const PointI& radius) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t fillRegion(const Region& region) { return ERR_RT_INVALID_OBJECT; }

  virtual err_t fillAll() { return ERR_RT_INVALID_OBJECT; }

  // --------------------------------------------------------------------------
  // [Vector Drawing]
  // --------------------------------------------------------------------------

  virtual err_t drawPoint(const PointD& p) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t drawLine(const PointD& start, const PointD& end) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t drawLine(const PointD* pts, sysuint_t count) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t drawPolygon(const PointD* pts, sysuint_t count) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t drawRect(const RectD& r) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t drawRects(const RectD* r, sysuint_t count) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t drawRound(const RectD& r, const PointD& radius) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t drawEllipse(const PointD& cp, const PointD& r) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t drawArc(const PointD& cp, const PointD& r, double start, double sweep) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t drawPath(const PathD& path) { return ERR_RT_INVALID_OBJECT; }

  virtual err_t fillPolygon(const PointD* pts, sysuint_t count) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t fillRect(const RectD& r) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t fillRects(const RectD* r, sysuint_t count) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t fillRound(const RectD& r, const PointD& radius) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t fillEllipse(const PointD& cp, const PointD& r) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t fillArc(const PointD& cp, const PointD& r, double start, double sweep) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t fillPath(const PathD& path) { return ERR_RT_INVALID_OBJECT; }

  // --------------------------------------------------------------------------
  // [Glyph Drawing]
  // --------------------------------------------------------------------------

  virtual err_t drawGlyph(const PointI&p, const Image& glyph, const RectI* irect) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t drawGlyph(const PointD&p, const Image& glyph, const RectI* irect) { return ERR_RT_INVALID_OBJECT; }

  virtual err_t drawGlyph(const PointI& pt, const Glyph& glyph, const RectI* grect) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t drawGlyph(const PointD& pt, const Glyph& glyph, const RectI* grect) { return ERR_RT_INVALID_OBJECT; }

  virtual err_t drawGlyphSet(const PointI& pt, const GlyphSet& glyphSet, const RectI* clip) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t drawGlyphSet(const PointD& pt, const GlyphSet& glyphSet, const RectI* clip) { return ERR_RT_INVALID_OBJECT; }

  // --------------------------------------------------------------------------
  // [Text Drawing]
  // --------------------------------------------------------------------------

  virtual err_t drawText(const PointI& pt, const String& text, const Font& font, const RectI* clip) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t drawText(const PointD& pt, const String& text, const Font& font, const RectI* clip) { return ERR_RT_INVALID_OBJECT; }

  virtual err_t drawText(const RectI& rect, const String& text, const Font& font, uint32_t align, const RectI* clip) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t drawText(const RectD& rect, const String& text, const Font& font, uint32_t align, const RectI* clip) { return ERR_RT_INVALID_OBJECT; }

  // --------------------------------------------------------------------------
  // [Image drawing]
  // --------------------------------------------------------------------------

  virtual err_t drawImage(const PointI& p, const Image& image, const RectI* irect) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t drawImage(const PointD& p, const Image& image, const RectI* irect) { return ERR_RT_INVALID_OBJECT; }

  virtual err_t drawImage(const RectI& rect, const Image& image, const RectI* irect) { return ERR_RT_INVALID_OBJECT; }
  virtual err_t drawImage(const RectD& rect, const Image& image, const RectI* irect) { return ERR_RT_INVALID_OBJECT; }
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GRAPHICS_PAINTENGINE_NULLPAINTENGINE_P_H
