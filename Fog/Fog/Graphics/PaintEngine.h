// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_PAINTENGINE_H
#define _FOG_GRAPHICS_PAINTENGINE_H

// [Dependencies]
#include <Fog/Core/List.h>
#include <Fog/Core/String.h>
#include <Fog/Graphics/Color.h>
#include <Fog/Graphics/Matrix.h>
#include <Fog/Graphics/Font.h>
#include <Fog/Graphics/Geometry.h>
#include <Fog/Graphics/Image.h>
#include <Fog/Graphics/Path.h>
#include <Fog/Graphics/PathStroker.h>
#include <Fog/Graphics/Pattern.h>
#include <Fog/Graphics/Region.h>

namespace Fog {

//! @addtogroup Fog_Graphics_Painting
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct ColorFilter;
struct ImageFilter;

// ============================================================================
// [Fog::PaintEngine]
// ============================================================================

//! @brief Painter engine is an abstract @c Painter backend.
//!
//! See @c Painter virtual members for documentation.
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
  virtual err_t setEngine(uint32_t engine, uint32_t threads) = 0;

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
  virtual err_t resetUserVars() = 0;

  virtual Region getMetaRegion() const = 0;
  virtual IntPoint getMetaOrigin() const = 0;

  virtual Region getUserRegion() const = 0;
  virtual IntPoint getUserOrigin() const = 0;

  // --------------------------------------------------------------------------
  // [Layers]
  // --------------------------------------------------------------------------

#if 0
  // TODO: API proposal.

  virtual err_t beginLayer() = 0;
  virtual err_t beginLayerToImage(Image& dest) = 0;
  virtual err_t beginLayerToImage(const ImageBuffer& dest) = 0;
  virtual err_t endLayer() = 0;
#endif

  // --------------------------------------------------------------------------
  // [Paint Parameters]
  // --------------------------------------------------------------------------

  virtual uint32_t getOperator() const = 0;
  virtual err_t setOperator(uint32_t op) = 0;

  virtual uint32_t getSourceType() const = 0;

  virtual Argb getSourceArgb() const = 0;
  virtual Pattern getSourcePattern() const = 0;

  virtual err_t setSource(Argb argb) = 0;
  virtual err_t setSource(const Pattern& pattern) = 0;

  virtual err_t resetSource() = 0;

  virtual float getAlpha() const = 0;
  virtual err_t setAlpha(float alpha) = 0;

  // --------------------------------------------------------------------------
  // [Clip Parameters]
  // --------------------------------------------------------------------------

  virtual uint32_t getClipRule() const = 0;
  virtual err_t setClipRule(uint32_t clipRule) = 0;

  // --------------------------------------------------------------------------
  // [Fill Parameters]
  // --------------------------------------------------------------------------

  virtual uint32_t getFillRule() const = 0;
  virtual err_t setFillRule(uint32_t fillRule) = 0;

  // --------------------------------------------------------------------------
  // [Stroke Parameters]
  // --------------------------------------------------------------------------

  virtual PathStrokeParams getStrokeParams() const = 0;
  virtual err_t setStrokeParams(const PathStrokeParams& strokeParams) = 0;

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
  virtual err_t setMatrix(const DoubleMatrix& m) = 0;
  virtual err_t resetMatrix() = 0;

  virtual err_t rotate(double angle, uint32_t order) = 0;

  virtual err_t scale(int sx, int sy, uint32_t order) = 0;
  virtual err_t scale(double sx, double sy, uint32_t order) = 0;

  virtual err_t skew(double sx, double sy, uint32_t order) = 0;

  virtual err_t translate(int x, int y, uint32_t order) = 0;
  virtual err_t translate(double x, double y, uint32_t order) = 0;

  virtual err_t transform(const DoubleMatrix& m, uint32_t order) = 0;

  virtual err_t worldToScreen(DoublePoint* pt) const = 0;
  virtual err_t screenToWorld(DoublePoint* pt) const = 0;

  virtual err_t alignPoint(DoublePoint* pt) const = 0;

  // --------------------------------------------------------------------------
  // [State]
  // --------------------------------------------------------------------------

  virtual err_t save(uint32_t flags) = 0;
  virtual err_t restore() = 0;

  // --------------------------------------------------------------------------
  // [Clipping]
  // --------------------------------------------------------------------------

  virtual err_t clipRect(const IntRect& rect, uint32_t clipOp) = 0;
  virtual err_t clipMask(const IntPoint& pt, const Image& mask, uint32_t clipOp) = 0;
  virtual err_t clipRegion(const Region& region, uint32_t clipOp) = 0;

  virtual err_t clipRect(const DoubleRect& rect, uint32_t clipOp) = 0;
  virtual err_t clipMask(const DoublePoint& pt, const Image& mask, uint32_t clipOp) = 0;
  virtual err_t clipRects(const DoubleRect* r, sysuint_t count, uint32_t clipOp) = 0;
  virtual err_t clipRound(const DoubleRect& r, const DoublePoint& radius, uint32_t clipOp) = 0;
  virtual err_t clipEllipse(const DoublePoint& cp, const DoublePoint& r, uint32_t clipOp) = 0;
  virtual err_t clipArc(const DoublePoint& cp, const DoublePoint& r, double start, double sweep, uint32_t clipOp) = 0;
  virtual err_t clipPath(const DoublePath& path, bool stroke, uint32_t clipOp) = 0;

  virtual err_t resetClip() = 0;

  // --------------------------------------------------------------------------
  // [Raster Drawing]
  // --------------------------------------------------------------------------

  virtual err_t drawPoint(const IntPoint& p) = 0;
  virtual err_t drawLine(const IntPoint& start, const IntPoint& end) = 0;
  virtual err_t drawRect(const IntRect& r) = 0;
  virtual err_t drawRound(const IntRect& r, const IntPoint& radius) = 0;
  virtual err_t fillRect(const IntRect& r) = 0;
  virtual err_t fillRects(const IntRect* r, sysuint_t count) = 0;
  virtual err_t fillRound(const IntRect& r, const IntPoint& radius) = 0;
  virtual err_t fillRegion(const Region& region) = 0;

  virtual err_t fillAll() = 0;

  // --------------------------------------------------------------------------
  // [Vector Drawing]
  // --------------------------------------------------------------------------

  virtual err_t drawPoint(const DoublePoint& p) = 0;
  virtual err_t drawLine(const DoublePoint& start, const DoublePoint& end) = 0;
  virtual err_t drawLine(const DoublePoint* pts, sysuint_t count) = 0;
  virtual err_t drawPolygon(const DoublePoint* pts, sysuint_t count) = 0;
  virtual err_t drawRect(const DoubleRect& r) = 0;
  virtual err_t drawRects(const DoubleRect* r, sysuint_t count) = 0;
  virtual err_t drawRound(const DoubleRect& r, const DoublePoint& radius) = 0;
  virtual err_t drawEllipse(const DoublePoint& cp, const DoublePoint& r) = 0;
  virtual err_t drawArc(const DoublePoint& cp, const DoublePoint& r, double start, double sweep) = 0;
  virtual err_t drawPath(const DoublePath& path) = 0;

  virtual err_t fillPolygon(const DoublePoint* pts, sysuint_t count) = 0;
  virtual err_t fillRect(const DoubleRect& r) = 0;
  virtual err_t fillRects(const DoubleRect* r, sysuint_t count) = 0;
  virtual err_t fillRound(const DoubleRect& r, const DoublePoint& radius) = 0;
  virtual err_t fillEllipse(const DoublePoint& cp, const DoublePoint& r) = 0;
  virtual err_t fillArc(const DoublePoint& cp, const DoublePoint& r, double start, double sweep) = 0;
  virtual err_t fillPath(const DoublePath& path) = 0;

  // --------------------------------------------------------------------------
  // [Glyph Drawing]
  // --------------------------------------------------------------------------

  virtual err_t drawGlyph(const IntPoint&p, const Image& glyph, const IntRect* grect) = 0;
  virtual err_t drawGlyph(const DoublePoint&p, const Image& glyph, const IntRect* grect) = 0;

  // TODO: Addition, painting API proposal.
  //
  // virtual err_t drawFilteredGlyph(const IntPoint& p, const Image& glyph, const ImageFilterBase& filter, const IntRect* grect) = 0;
  // virtual err_t drawFilteredGlyph(const DoublePoint& p, const Image& glyph, const ImageFilterBase& filter, const IntRect* grect) = 0;

  virtual err_t drawGlyph(const IntPoint& pt, const Glyph& glyph, const IntRect* clip) = 0;
  virtual err_t drawGlyph(const DoublePoint& pt, const Glyph& glyph, const IntRect* clip) = 0;

  virtual err_t drawGlyphSet(const IntPoint& pt, const GlyphSet& glyphSet, const IntRect* clip) = 0;
  virtual err_t drawGlyphSet(const DoublePoint& pt, const GlyphSet& glyphSet, const IntRect* clip) = 0;

  // --------------------------------------------------------------------------
  // [Text Drawing]
  // --------------------------------------------------------------------------

  virtual err_t drawText(const IntPoint& pt, const String& text, const Font& font, const IntRect* clip) = 0;
  virtual err_t drawText(const DoublePoint& pt, const String& text, const Font& font, const IntRect* clip) = 0;

  virtual err_t drawText(const IntRect& rect, const String& text, const Font& font, uint32_t align, const IntRect* clip) = 0;
  virtual err_t drawText(const DoubleRect& rect, const String& text, const Font& font, uint32_t align, const IntRect* clip) = 0;

  // TODO: Addition, filtering API proposal.
  //
  // virtual err_t drawFilteredText(const IntPoint& p, const String& text, const Font& font, const ImageFilterBase& filter, const IntRect* clip) = 0;
  // virtual err_t drawFilteredText(const DoublePoint& p, const String& text, const Font& font, const ImageFilterBase& filter, const IntRect* clip) = 0;

  // --------------------------------------------------------------------------
  // [Image Drawing]
  // --------------------------------------------------------------------------

  virtual err_t drawImage(const IntPoint& p, const Image& image, const IntRect* irect) = 0;
  virtual err_t drawImage(const DoublePoint& p, const Image& image, const IntRect* irect) = 0;

  // TODO: Addition, painting API proposal.
  //
  // virtual err_t drawMaskedImage(const IntPoint& p, const Image& image, const Image& mask, const IntRect* irect, const IntRect* mrect) = 0;
  // virtual err_t drawMaskedImage(const DoublePoint& p, const Image& image, const Image& mask, const IntRect* irect, const IntRect* mrect) = 0;
  //
  // TODO: Addition, filtering API proposal.
  //
  // virtual err_t drawFilteredImage(const IntPoint& p, const Image& image, const ImageFilterBase& filter, const IntRect* irect) = 0;
  // virtual err_t drawFilteredImage(const DoublePoint& p, const Image& image, const ImageFilterBase& filter, const IntRect* irect) = 0;

private:
  FOG_DISABLE_COPY(PaintEngine)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GRAPHICS_PAINTENGINE_H
