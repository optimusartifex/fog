// [Fog-Graphics]
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
#include <Fog/Graphics/Font.h>
#include <Fog/Graphics/Geometry.h>
#include <Fog/Graphics/Image.h>
#include <Fog/Graphics/Path.h>
#include <Fog/Graphics/PathStroker.h>
#include <Fog/Graphics/Pattern.h>
#include <Fog/Graphics/Region.h>
#include <Fog/Graphics/Transform.h>

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

  virtual err_t setMetaVars(const Region& region, const PointI& origin) = 0;
  virtual err_t resetMetaVars() = 0;

  virtual err_t setUserVars(const Region& region, const PointI& origin) = 0;
  virtual err_t resetUserVars() = 0;

  virtual Region getMetaRegion() const = 0;
  virtual PointI getMetaOrigin() const = 0;

  virtual Region getUserRegion() const = 0;
  virtual PointI getUserOrigin() const = 0;

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

  virtual ArgbI getSourceArgb() const = 0;
  virtual Pattern getSourcePattern() const = 0;

  virtual err_t setSource(ArgbI argb) = 0;
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

  virtual TransformD getTransform() const = 0;
  virtual err_t setTransform(const TransformD& m) = 0;
  virtual err_t resetTransform() = 0;

  virtual err_t rotate(double angle, uint32_t order) = 0;

  virtual err_t scale(int sx, int sy, uint32_t order) = 0;
  virtual err_t scale(double sx, double sy, uint32_t order) = 0;

  virtual err_t skew(double sx, double sy, uint32_t order) = 0;

  virtual err_t translate(int x, int y, uint32_t order) = 0;
  virtual err_t translate(double x, double y, uint32_t order) = 0;

  virtual err_t transform(const TransformD& m, uint32_t order) = 0;

  virtual err_t worldToScreen(PointD* pt) const = 0;
  virtual err_t screenToWorld(PointD* pt) const = 0;

  virtual err_t alignPoint(PointD* pt) const = 0;

  // --------------------------------------------------------------------------
  // [State]
  // --------------------------------------------------------------------------

  virtual err_t save(uint32_t flags) = 0;
  virtual err_t restore() = 0;

  // --------------------------------------------------------------------------
  // [Clipping]
  // --------------------------------------------------------------------------

  virtual err_t clipRect(const RectI& rect, uint32_t clipOp) = 0;
  virtual err_t clipMask(const PointI& pt, const Image& mask, uint32_t clipOp) = 0;
  virtual err_t clipRegion(const Region& region, uint32_t clipOp) = 0;

  virtual err_t clipRect(const RectD& rect, uint32_t clipOp) = 0;
  virtual err_t clipMask(const PointD& pt, const Image& mask, uint32_t clipOp) = 0;
  virtual err_t clipRects(const RectD* r, sysuint_t count, uint32_t clipOp) = 0;
  virtual err_t clipRound(const RectD& r, const PointD& radius, uint32_t clipOp) = 0;
  virtual err_t clipEllipse(const PointD& cp, const PointD& r, uint32_t clipOp) = 0;
  virtual err_t clipArc(const PointD& cp, const PointD& r, double start, double sweep, uint32_t clipOp) = 0;
  virtual err_t clipPath(const PathD& path, bool stroke, uint32_t clipOp) = 0;

  virtual err_t resetClip() = 0;

  // --------------------------------------------------------------------------
  // [Raster Drawing]
  // --------------------------------------------------------------------------

  virtual err_t drawPoint(const PointI& p) = 0;
  virtual err_t drawLine(const PointI& start, const PointI& end) = 0;

  virtual err_t drawRect(const RectI& r) = 0;
  virtual err_t drawRound(const RectI& r, const PointI& radius) = 0;

  virtual err_t fillRect(const RectI& r) = 0;
  virtual err_t fillRects(const RectI* r, sysuint_t count) = 0;
  virtual err_t fillRound(const RectI& r, const PointI& radius) = 0;
  virtual err_t fillRegion(const Region& region) = 0;

  virtual err_t fillAll() = 0;

  // --------------------------------------------------------------------------
  // [Vector Drawing]
  // --------------------------------------------------------------------------

  virtual err_t drawPoint(const PointD& p) = 0;
  virtual err_t drawLine(const PointD& start, const PointD& end) = 0;
  virtual err_t drawLine(const PointD* pts, sysuint_t count) = 0;

  virtual err_t drawPolygon(const PointD* pts, sysuint_t count) = 0;
  virtual err_t drawRect(const RectD& r) = 0;
  virtual err_t drawRects(const RectD* r, sysuint_t count) = 0;
  virtual err_t drawRound(const RectD& r, const PointD& radius) = 0;
  virtual err_t drawEllipse(const PointD& cp, const PointD& r) = 0;
  virtual err_t drawArc(const PointD& cp, const PointD& r, double start, double sweep) = 0;
  virtual err_t drawPath(const PathD& path) = 0;

  virtual err_t fillPolygon(const PointD* pts, sysuint_t count) = 0;
  virtual err_t fillRect(const RectD& r) = 0;
  virtual err_t fillRects(const RectD* r, sysuint_t count) = 0;
  virtual err_t fillRound(const RectD& r, const PointD& radius) = 0;
  virtual err_t fillEllipse(const PointD& cp, const PointD& r) = 0;
  virtual err_t fillArc(const PointD& cp, const PointD& r, double start, double sweep) = 0;
  virtual err_t fillPath(const PathD& path) = 0;

  // --------------------------------------------------------------------------
  // [Glyph Drawing]
  // --------------------------------------------------------------------------

  virtual err_t drawGlyph(const PointI& pt, const Image& glyph, const RectI* grect) = 0;
  virtual err_t drawGlyph(const PointD& pt, const Image& glyph, const RectI* grect) = 0;

  // TODO: Addition, painting API proposal.
  //
  // virtual err_t drawFilteredGlyph(const PointI& p, const Image& glyph, const ImageFilterBase& filter, const RectI* grect) = 0;
  // virtual err_t drawFilteredGlyph(const PointD& p, const Image& glyph, const ImageFilterBase& filter, const RectI* grect) = 0;

  virtual err_t drawGlyph(const PointI& pt, const Glyph& glyph, const RectI* clip) = 0;
  virtual err_t drawGlyph(const PointD& pt, const Glyph& glyph, const RectI* clip) = 0;

  virtual err_t drawGlyphSet(const PointI& pt, const GlyphSet& glyphSet, const RectI* clip) = 0;
  virtual err_t drawGlyphSet(const PointD& pt, const GlyphSet& glyphSet, const RectI* clip) = 0;

  // --------------------------------------------------------------------------
  // [Text Drawing]
  // --------------------------------------------------------------------------

  virtual err_t drawText(const PointI& pt, const String& text, const Font& font, const RectI* clip) = 0;
  virtual err_t drawText(const PointD& pt, const String& text, const Font& font, const RectI* clip) = 0;

  virtual err_t drawText(const RectI& rect, const String& text, const Font& font, uint32_t align, const RectI* clip) = 0;
  virtual err_t drawText(const RectD& rect, const String& text, const Font& font, uint32_t align, const RectI* clip) = 0;

  // TODO: API Addition, filtering API proposal.
  //
  // virtual err_t drawFilteredText(const PointI& p, const String& text, const Font& font, const ImageFilterBase& filter, const RectI* clip) = 0;
  // virtual err_t drawFilteredText(const PointD& p, const String& text, const Font& font, const ImageFilterBase& filter, const RectI* clip) = 0;

  // --------------------------------------------------------------------------
  // [Image Drawing]
  // --------------------------------------------------------------------------

  virtual err_t drawImage(const PointI& pt, const Image& image, const RectI* irect) = 0;
  virtual err_t drawImage(const PointD& pt, const Image& image, const RectI* irect) = 0;

  virtual err_t drawImage(const RectI& rect, const Image& image, const RectI* irect) = 0;
  virtual err_t drawImage(const RectD& rect, const Image& image, const RectI* irect) = 0;

  // TODO: API Addition, painting API proposal.
  //
  // virtual err_t drawMaskedImage(const PointI& p, const Image& image, const Image& mask, const RectI* irect, const RectI* mrect) = 0;
  // virtual err_t drawMaskedImage(const PointD& p, const Image& image, const Image& mask, const RectI* irect, const RectI* mrect) = 0;
  //
  // TODO: API Addition, filtering API proposal.
  //
  // virtual err_t drawFilteredImage(const PointI& p, const Image& image, const ImageFilterBase& filter, const RectI* irect) = 0;
  // virtual err_t drawFilteredImage(const PointD& p, const Image& image, const ImageFilterBase& filter, const RectI* irect) = 0;

private:
  FOG_DISABLE_COPY(PaintEngine)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GRAPHICS_PAINTENGINE_H
