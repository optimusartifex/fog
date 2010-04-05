// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_PAINTERENGINE_H
#define _FOG_GRAPHICS_PAINTERENGINE_H

// [Dependencies]
#include <Fog/Core/List.h>
#include <Fog/Core/String.h>
#include <Fog/Graphics/Argb.h>
#include <Fog/Graphics/Matrix.h>
#include <Fog/Graphics/Font.h>
#include <Fog/Graphics/Geometry.h>
#include <Fog/Graphics/Image.h>
#include <Fog/Graphics/Path.h>
#include <Fog/Graphics/Pattern.h>
#include <Fog/Graphics/Region.h>
#include <Fog/Graphics/Stroker.h>

//! @addtogroup Fog_Graphics
//! @{

namespace Fog {

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct ColorFilter;
struct ImageFilter;

// ============================================================================
// [Fog::PaintEngine]
// ============================================================================

//! @brief Painter engine is @c Painter backend.
//!
//! See @c Painter all virtual members documentation.
struct FOG_API PaintEngine
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  PaintEngine();
  virtual ~PaintEngine() = 0;

  // --------------------------------------------------------------------------
  // [Width / Height / Format]
  // --------------------------------------------------------------------------

  virtual int getWidth() const = 0;
  virtual int getHeight() const = 0;
  virtual uint32_t getFormat() const = 0;

  // --------------------------------------------------------------------------
  // [Engine / Flush]
  // --------------------------------------------------------------------------

  virtual uint32_t getEngine() const = 0;
  virtual err_t setEngine(uint32_t engine, uint32_t cores = 0) = 0;

  virtual err_t flush(uint32_t flags) = 0;

  // --------------------------------------------------------------------------
  // [Hints]
  // --------------------------------------------------------------------------

  virtual int getHint(uint32_t hint) const = 0;
  virtual err_t setHint(uint32_t hint, int value) = 0;

  // --------------------------------------------------------------------------
  // [Meta]
  // --------------------------------------------------------------------------

  virtual err_t setMetaVars(const Region& region, const IntPoint& origin) = 0;
  virtual err_t resetMetaVars() = 0;

  virtual err_t setUserVars(const Region& region, const IntPoint& origin) = 0;
  virtual err_t setUserRegion(const Region& region, uint32_t regionOp) = 0;
  virtual err_t setUserOrigin(const IntPoint& origin, uint32_t originOp) = 0;
  virtual err_t resetUserVars() = 0;

  virtual Region getMetaRegion() const = 0;
  virtual Region getUserRegion() const = 0;

  virtual IntPoint getMetaOrigin() const = 0;
  virtual IntPoint getUserOrigin() const = 0;

  // --------------------------------------------------------------------------
  // [Operator]
  // --------------------------------------------------------------------------

  virtual uint32_t getOperator() const = 0;
  virtual err_t setOperator(uint32_t op) = 0;

  // --------------------------------------------------------------------------
  // [Source]
  // --------------------------------------------------------------------------

  virtual uint32_t getSourceType() const = 0;

  virtual Argb getSourceArgb() const = 0;
  virtual Pattern getSourcePattern() const = 0;

  virtual err_t setSource(Argb argb) = 0;
  virtual err_t setSource(const Pattern& pattern) = 0;

#if 0
  // API proposal

  // --------------------------------------------------------------------------
  // [Global Opacity]
  // --------------------------------------------------------------------------

  virtual float getOpacity() const = 0;
  virtual void setOpacity(float opacity) = 0;

  // --------------------------------------------------------------------------
  // [Clipping]
  // --------------------------------------------------------------------------

  virtual void clipRect(const IntRect& rect, uint32_t clipOp) = 0;
  virtual void clipMask(const IntPoint& pt, const Image& mask, uint32_t clipOp) = 0;
  virtual void clipRegion(const Region& region, uint32_t clipOp) = 0;

  virtual void clipRect(const DoubleRect& rect, uint32_t clipOp) = 0;
  virtual void clipMask(const DoublePoint& pt, const Image& mask, uint32_t clipOp) = 0;
  virtual void clipRects(const DoubleRect* r, sysuint_t count, uint32_t clipOp) = 0;
  virtual void clipRound(const DoubleRect& r, const DoublePoint& radius, uint32_t clipOp) = 0;
  virtual void clipEllipse(const DoublePoint& cp, const DoublePoint& r, uint32_t clipOp) = 0;
  virtual void clipArc(const DoublePoint& cp, const DoublePoint& r, double start, double sweep, uint32_t clipOp) = 0;
  virtual void clipPath(const DoublePath& path, uint32_t clipOp) = 0;

  virtual void resetClip() = 0;
#endif

  // --------------------------------------------------------------------------
  // [Fill Parameters]
  // --------------------------------------------------------------------------

  virtual uint32_t getFillMode() const = 0;
  virtual err_t setFillMode(uint32_t mode) = 0;

  // --------------------------------------------------------------------------
  // [Stroke Parameters]
  // --------------------------------------------------------------------------

  virtual StrokeParams getStrokeParams() const = 0;
  virtual err_t setStrokeParams(const StrokeParams& strokeParams) = 0;

  virtual double getLineWidth() const = 0;
  virtual err_t setLineWidth(double lineWidth) = 0;

  virtual uint32_t getStartCap() const = 0;
  virtual err_t setStartCap(uint32_t startCap) = 0;

  virtual uint32_t getEndCap() const = 0;
  virtual err_t setEndCap(uint32_t endCap) = 0;

  virtual err_t setLineCaps(uint32_t lineCap) = 0;

  virtual uint32_t getLineJoin() const = 0;
  virtual err_t setLineJoin(uint32_t lineJoin) = 0;

  virtual double getMiterLimit() const = 0;
  virtual err_t setMiterLimit(double miterLimit) = 0;

  virtual List<double> getDashes() const = 0;
  virtual err_t setDashes(const double* dashes, sysuint_t count) = 0;
  virtual err_t setDashes(const List<double>& dashes) = 0;

  virtual double getDashOffset() const = 0;
  virtual err_t setDashOffset(double offset) = 0;

  // --------------------------------------------------------------------------
  // [Transformations]
  // --------------------------------------------------------------------------

  virtual DoubleMatrix getMatrix() const = 0;
  virtual void setMatrix(const DoubleMatrix& m) = 0;
  virtual void resetMatrix() = 0;

  virtual void rotate(double angle, uint32_t order) = 0;
  virtual void scale(double sx, double sy, uint32_t order) = 0;
  virtual void skew(double sx, double sy, uint32_t order) = 0;
  virtual void translate(double x, double y, uint32_t order) = 0;
  virtual void transform(const DoubleMatrix& m, uint32_t order) = 0;

  virtual void worldToScreen(DoublePoint* pt) const = 0;
  virtual void screenToWorld(DoublePoint* pt) const = 0;

  virtual void alignPoint(DoublePoint* pt) const = 0;

  // --------------------------------------------------------------------------
  // [State]
  // --------------------------------------------------------------------------

  virtual void save() = 0;
  virtual void restore() = 0;

  // --------------------------------------------------------------------------
  // [Raster Drawing]
  // --------------------------------------------------------------------------

  virtual void drawPoint(const IntPoint& p) = 0;
  virtual void drawLine(const IntPoint& start, const IntPoint& end) = 0;
  virtual void drawRect(const IntRect& r) = 0;
  virtual void drawRound(const IntRect& r, const IntPoint& radius) = 0;
  virtual void fillRect(const IntRect& r) = 0;
  virtual void fillRects(const IntRect* r, sysuint_t count) = 0;
  virtual void fillRound(const IntRect& r, const IntPoint& radius) = 0;
  virtual void fillRegion(const Region& region) = 0;

  virtual void fillAll() = 0;

  // --------------------------------------------------------------------------
  // [Vector Drawing]
  // --------------------------------------------------------------------------

  virtual void drawPoint(const DoublePoint& p) = 0;
  virtual void drawLine(const DoublePoint& start, const DoublePoint& end) = 0;
  virtual void drawLine(const DoublePoint* pts, sysuint_t count) = 0;
  virtual void drawPolygon(const DoublePoint* pts, sysuint_t count) = 0;
  virtual void drawRect(const DoubleRect& r) = 0;
  virtual void drawRects(const DoubleRect* r, sysuint_t count) = 0;
  virtual void drawRound(const DoubleRect& r, const DoublePoint& radius) = 0;
  virtual void drawEllipse(const DoublePoint& cp, const DoublePoint& r) = 0;
  virtual void drawArc(const DoublePoint& cp, const DoublePoint& r, double start, double sweep) = 0;
  virtual void drawPath(const DoublePath& path) = 0;

  virtual void fillPolygon(const DoublePoint* pts, sysuint_t count) = 0;
  virtual void fillRect(const DoubleRect& r) = 0;
  virtual void fillRects(const DoubleRect* r, sysuint_t count) = 0;
  virtual void fillRound(const DoubleRect& r, const DoublePoint& radius) = 0;
  virtual void fillEllipse(const DoublePoint& cp, const DoublePoint& r) = 0;
  virtual void fillArc(const DoublePoint& cp, const DoublePoint& r, double start, double sweep) = 0;
  virtual void fillPath(const DoublePath& path) = 0;

  // --------------------------------------------------------------------------
  // [Glyph / Text Drawing]
  // --------------------------------------------------------------------------

  virtual void drawGlyph(const IntPoint& pt, const Glyph& glyph, const IntRect* clip) = 0;
  virtual void drawGlyphSet(const IntPoint& pt, const GlyphSet& glyphSet, const IntRect* clip) = 0;

  virtual void drawText(const IntPoint& p, const String& text, const Font& font, const IntRect* clip) = 0;
  virtual void drawText(const IntRect& r, const String& text, const Font& font, uint32_t align, const IntRect* clip) = 0;

  // --------------------------------------------------------------------------
  // [Image Drawing]
  // --------------------------------------------------------------------------

  virtual void blitImage(const IntPoint& p, const Image& image, const IntRect* irect) = 0;
  virtual void blitImage(const DoublePoint& p, const Image& image, const IntRect* irect) = 0;

private:
  FOG_DISABLE_COPY(PaintEngine)
};

} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_GRAPHICS_PAINTERENGINE_H
