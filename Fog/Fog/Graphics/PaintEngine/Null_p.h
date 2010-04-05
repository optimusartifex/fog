// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_PAINTERENGINE_NULL_P_H
#define _FOG_GRAPHICS_PAINTERENGINE_NULL_P_H

// [Dependencies]
#include <Fog/Graphics/PaintEngine.h>
#include <Fog/Graphics/Stroker.h>

//! @addtogroup Fog_Graphics_Private
//! @{

namespace Fog {

// ============================================================================
// [Fog::NullPaintEngine]
// ============================================================================

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
  virtual uint32_t getFormat() const { return PIXEL_FORMAT_NULL; }

  // --------------------------------------------------------------------------
  // [Engine / Flush]
  // --------------------------------------------------------------------------

  virtual uint32_t getEngine() const { return PAINTER_ENGINE_NULL; }
  virtual void setEngine(uint32_t engine, uint32_t cores = 0) {}

  virtual void flush(uint32_t flags) {}

  // --------------------------------------------------------------------------
  // [Hints]
  // --------------------------------------------------------------------------

  virtual int getHint(uint32_t hint) const { return -1; }
  virtual err_t setHint(uint32_t hint, int value) { return ERR_RT_INVALID_CONTEXT; }

  // --------------------------------------------------------------------------
  // [Meta]
  // --------------------------------------------------------------------------

  virtual void setMetaVars(const Region& region, const IntPoint& origin) {}
  virtual void resetMetaVars() {}

  virtual void setUserVars(const Region& region, const IntPoint& origin) {}
  virtual void setUserOrigin(const IntPoint& origin, uint32_t originOp) {}
  virtual void resetUserVars() {}

  virtual Region getMetaRegion() const { return Region::infinite(); }
  virtual Region getUserRegion() const { return Region::infinite(); }

  virtual IntPoint getMetaOrigin() const { return IntPoint(0, 0); }
  virtual IntPoint getUserOrigin() const { return IntPoint(0, 0); }

  // --------------------------------------------------------------------------
  // [Operator]
  // --------------------------------------------------------------------------

  virtual uint32_t getOperator() const { return OPERATOR_SRC_OVER; }
  virtual err_t setOperator(uint32_t op) { return ERR_RT_INVALID_CONTEXT; }

  // --------------------------------------------------------------------------
  // [Source]
  // --------------------------------------------------------------------------

  virtual uint32_t getSourceType() const { return PAINTER_SOURCE_ARGB; }

  virtual Argb getSourceArgb() const { return Argb(0x00000000); }
  virtual Pattern getSourcePattern() const { return Pattern(); }

  virtual err_t setSource(Argb argb) { return ERR_OK; }
  virtual err_t setSource(const Pattern& pattern) { return ERR_RT_INVALID_CONTEXT; }

  // --------------------------------------------------------------------------
  // [Fill Parameters]
  // --------------------------------------------------------------------------

  virtual uint32_t getFillMode() const { return FILL_DEFAULT; }
  virtual err_t setFillMode(uint32_t mode) { return ERR_RT_INVALID_CONTEXT; }

  // --------------------------------------------------------------------------
  // [Stroke Parameters]
  // --------------------------------------------------------------------------

  virtual StrokeParams getStrokeParams() const { return StrokeParams(); }
  virtual err_t setStrokeParams(const StrokeParams& strokeParams) { return ERR_RT_INVALID_CONTEXT; }

  virtual double getLineWidth() const { return 0.0; }
  virtual err_t setLineWidth(double lineWidth) { return ERR_RT_INVALID_CONTEXT; }

  virtual uint32_t getStartCap() const { return 0; }
  virtual err_t setStartCap(uint32_t startCap) { return ERR_RT_INVALID_CONTEXT; }

  virtual uint32_t getEndCap() const { return 0; }
  virtual err_t setEndCap(uint32_t endCap) { return ERR_RT_INVALID_CONTEXT; }

  virtual err_t setLineCaps(uint32_t lineCaps) { return ERR_RT_INVALID_CONTEXT; }

  virtual uint32_t getLineJoin() const { return 0; }
  virtual err_t setLineJoin(uint32_t lineJoin) { return ERR_RT_INVALID_CONTEXT; }

  virtual double getMiterLimit() const { return 0.0; }
  virtual err_t setMiterLimit(double miterLimit) { return ERR_RT_INVALID_CONTEXT; }

  virtual List<double> getDashes() const { return List<double>(); }
  virtual err_t setDashes(const double* dashes, sysuint_t count) { return ERR_RT_INVALID_CONTEXT; }
  virtual err_t setDashes(const List<double>& dashes) { return ERR_RT_INVALID_CONTEXT; }

  virtual double getDashOffset() const { return 0.0; }
  virtual err_t setDashOffset(double offset) { return ERR_RT_INVALID_CONTEXT; }

  // --------------------------------------------------------------------------
  // [Transformations]
  // --------------------------------------------------------------------------

  virtual DoubleMatrix getMatrix() const { return DoubleMatrix(); }
  virtual void setMatrix(const DoubleMatrix& m) {}
  virtual void resetMatrix() {}

  virtual void rotate(double angle, uint32_t order) {}
  virtual void scale(double sx, double sy, uint32_t order) {}
  virtual void skew(double sx, double sy, uint32_t order) {}
  virtual void translate(double x, double y, uint32_t order) {}
  virtual void transform(const DoubleMatrix& m, uint32_t order) {}

  virtual void worldToScreen(DoublePoint* pt) const {}
  virtual void screenToWorld(DoublePoint* pt) const {}

  virtual void alignPoint(DoublePoint* pt) const {}

  // --------------------------------------------------------------------------
  // [State]
  // --------------------------------------------------------------------------

  virtual void save() {}
  virtual void restore() {}

  // --------------------------------------------------------------------------
  // [Raster Drawing]
  // --------------------------------------------------------------------------

  virtual void drawPoint(const IntPoint& p) {}
  virtual void drawLine(const IntPoint& start, const IntPoint& end) {}
  virtual void drawRect(const IntRect& r) {}
  virtual void drawRound(const IntRect& r, const IntPoint& radius) {}
  virtual void fillRect(const IntRect& r) {}
  virtual void fillRects(const IntRect* r, sysuint_t count) {}
  virtual void fillRound(const IntRect& r, const IntPoint& radius) {}
  virtual void fillRegion(const Region& region) {}

  virtual void fillAll() {}

  // --------------------------------------------------------------------------
  // [Vector Drawing]
  // --------------------------------------------------------------------------

  virtual void drawPoint(const DoublePoint& p) {}
  virtual void drawLine(const DoublePoint& start, const DoublePoint& end) {}
  virtual void drawLine(const DoublePoint* pts, sysuint_t count) {}
  virtual void drawPolygon(const DoublePoint* pts, sysuint_t count) {}
  virtual void drawRect(const DoubleRect& r) {}
  virtual void drawRects(const DoubleRect* r, sysuint_t count) {}
  virtual void drawRound(const DoubleRect& r, const DoublePoint& radius) {}
  virtual void drawEllipse(const DoublePoint& cp, const DoublePoint& r) {}
  virtual void drawArc(const DoublePoint& cp, const DoublePoint& r, double start, double sweep) {}
  virtual void drawPath(const DoublePath& path) {}

  virtual void fillPolygon(const DoublePoint* pts, sysuint_t count) {}
  virtual void fillRect(const DoubleRect& r) {}
  virtual void fillRects(const DoubleRect* r, sysuint_t count) {}
  virtual void fillRound(const DoubleRect& r, const DoublePoint& radius) {}
  virtual void fillEllipse(const DoublePoint& cp, const DoublePoint& r) {}
  virtual void fillArc(const DoublePoint& cp, const DoublePoint& r, double start, double sweep) {}
  virtual void fillPath(const DoublePath& path) {}

  // --------------------------------------------------------------------------
  // [Glyph / Text Drawing]
  // --------------------------------------------------------------------------

  virtual void drawGlyph(const IntPoint& pt, const Glyph& glyph, const IntRect* clip) {}
  virtual void drawGlyphSet(const IntPoint& pt, const GlyphSet& glyphSet, const IntRect* clip) {}

  virtual void drawText(const IntPoint& p, const String& text, const Font& font, const IntRect* clip) {}
  virtual void drawText(const IntRect& r, const String& text, const Font& font, uint32_t align, const IntRect* clip) {}

  // --------------------------------------------------------------------------
  // [Image drawing]
  // --------------------------------------------------------------------------

  virtual void blitImage(const IntPoint& p, const Image& image, const IntRect* irect) {}
  virtual void blitImage(const DoublePoint& p, const Image& image, const IntRect* irect) {}
};

} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_GRAPHICS_PAINTERENGINE_NULL_P_H
