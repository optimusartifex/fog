// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_PAINTER_H
#define _FOG_GRAPHICS_PAINTER_H

// [Dependencies]
#include <Fog/Graphics/PaintEngine.h>

namespace Fog {

//! @addtogroup Fog_Graphics_Painting
//! @{

// ============================================================================
// [Fog::Painter]
// ============================================================================

//! @brief Painter.
//!
//! Painter is high level class that can be used to draw into the images or raw
//! memory buffers. Fog painter is state based and each rendering call depends
//! to current painter state that can be changed by methods like @c setFillRule(),
//! @c setLineWidth(), @c setLineCaps(), @c setOperator(), etc...
//!
//! @section Beginning and finalizing rendering.
//!
//! To start painting to an image or raw memory buffer use @c Painter constructor
//! or call @c begin() method with your raw buffer or @c Image instance. After 
//! you called @c begin() method that succeeded (check if return value is 
//! @c Fog::ERR_OK) you can start painting. If you no longer need painter
//! instance delete it (by @c delete operator or static destructor) or call
//! @c end() method.
//!
//! It's very important to destroy painter instance or call @c end() method,
//! because rendering can be asynchronous and destroying or enging will ensure
//! that all painting operations are visible to the target image.
//!
//! @section Meta and user regions, clipping.
//!
//! Painter supports two indenpendent region and origin information that can
//! be used to affect painter origin and clipping. First region and origin 
//! information are stored as meta region and meta origin. These variables
//! shouldn't be changed during rendering and are used usually by windowing
//! system to set correct origin and window clipping. If you are using Fog-Gui
//! library, never change these variables in @c Widget::onPaint() event.
//!
//! Also the meta region and origin is generally immutable. When you set it
//! you are forced to use it in all your painting code. Reseting these regions
//! will also reset all clipping and painter states. To set meta region and
//! origin use ... .
//! 
//! Second region and origin information are stored as user region and user
//! origin. These variables are designed to be changeable by user during 
//! rendering, so use them if it's useable for you.
//!
//! Origin and region clipping is pixel based and it's not affected by affine
//! transformations (so resetting transformations will not reset user origin).
//!
//! @section Affine transformations.
//!
//! TODO
struct FOG_API Painter 
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Create null painter instance.
  Painter();

  //! @brief Create painter instance for a given image @a image.
  //!
  //! @sa Painter::begin().
  Painter(Image& image, uint32_t initFlags = NO_FLAGS);

  //! @brief Create painter instance for a given image @a image, using only
  //! the @a rect area.
  //!
  //! @sa Painter::begin().
  Painter(Image& image, const IntRect& rect, uint32_t initFlags = NO_FLAGS);

  //! @brief Destroy painter instance.
  //!
  //! If painter is multithreaded, destructor would wait to finish the rendering
  //! (it will call @c end() method).
  //!
  //! @sa Painter::end().
  ~Painter();

  // --------------------------------------------------------------------------
  // [Begin / End]
  // --------------------------------------------------------------------------

  //! @brief Begin painting into the @a image.
  //!
  //! Always call @c Painter::end() or destroy painter before you try to 
  //! destroy or work with image. If you want to use the image you provided,
  //! but you want to continue paniting on it then call @c Painter::flush() 
  //! with @c PAINTER_FLUSH_SYNC flag to make content in the image ready.
  err_t begin(Image& image, uint32_t initFlags = NO_FLAGS);

  //! @brief Begin painting to a given rectange @a rect into the @a image.
  //!
  //! Always call @c Painter::end() or destroy painter before you try to 
  //! destroy or work with image. If you want to use the image you provided,
  //! but you want to continue paniting on it then call @c Painter::flush() 
  //! with @c PAINTER_FLUSH_SYNC flag to make content in the image ready.
  err_t begin(Image& image, const IntRect& rect, uint32_t initFlags = NO_FLAGS);

  //! @brief Begin painting to the custom image buffer @a buffer.
  //!
  //! This method can be used to paint to non-fog image buffer (WinGDI, Cairo,
  //! Qt4, SDL_Surface, etc...).
  err_t begin(const ImageBuffer& buffer, uint32_t initFlags = NO_FLAGS);

  //! @brief Destroy paint engine assigning null painter engine to the 
  //! @c Painter instance.
  //!
  //! Destroying also means that content in a provided image or buffer (through
  //! @c Painter::begin() call) will be ready to use.
  void end();

  //! @brief Finish painting and switch target to @a image, leaving
  //! the engine configuration.
  //!
  //! @note Calling Painter::switchTo() will destroy all layers and saved states,
  //! it's like calling @c Painter::end(), but reusing the paint engine.
  err_t switchTo(Image& image);

  //! @brief Finish painting and switch target to @a image, leaving
  //! the engine configuration.
  //!
  //! @note Calling Painter::switchTo() will destroy all layers and saved states,
  //! it's like calling @c Painter::end(), but reusing the paint engine.
  err_t switchTo(Image& image, const IntRect& rect);

  //! @brief Finish painting and switch target to @a buffer, leaving 
  //! the engine configuration.
  //!
  //! @note Calling Painter::switchTo() will destroy all layers and saved states,
  //! it's like calling @c Painter::end(), but reusing the paint engine.
  err_t switchTo(const ImageBuffer& buffer);

  // --------------------------------------------------------------------------
  // [Width / Height / Format]
  // --------------------------------------------------------------------------

  //! @brief Get painter width in pixels (width returned is width passed to @c begin() method).
  FOG_INLINE int getWidth() const { return _engine->getWidth(); }

  //! @brief Get painter height in pixels (height returned is height passed to @c begin() method).
  FOG_INLINE int getHeight() const { return _engine->getHeight(); }

  //! @brief Get painter format  (format returned is format passed to @c begin() method).
  FOG_INLINE uint32_t getFormat() const { return _engine->getFormat(); }

  // --------------------------------------------------------------------------
  // [Engine / Flush]
  // --------------------------------------------------------------------------

  //! @brief Get painter engine ID, see @c PAINT_ENGINE.
  FOG_INLINE uint32_t getEngine() const { return _engine->getEngine(); }

  //! @brief Set Painter engine ID, see @c PAINT_ENGINE.
  //!
  //! @param engine The engine ID to use.
  //! @param threads If the demanded engine is multithreaded then @a threads 
  //! specifies number of threads to use.
  FOG_INLINE err_t setEngine(uint32_t engine, uint32_t threads = 0) { return _engine->setEngine(engine, threads); }

  //! @brief Flush painter, see @c PAINTER_FLUSH_FLAGS.
  FOG_INLINE err_t flush(uint32_t flags) { return _engine->flush(flags); }

  // --------------------------------------------------------------------------
  // [Hints]
  // --------------------------------------------------------------------------

  //! @brief Get painter hint, see @c PAINTER_HINT.
  //!
  //! Painter hints can be used to control quality and behavior of painting.
  FOG_INLINE int getHint(uint32_t hint) const { return _engine->getHint(hint); }

  //! @brief Set painter hint, see @c PAINTER_HINT.
  //!
  //! Painter hints can be used to control quality and behavior of painting.
  FOG_INLINE err_t setHint(uint32_t hint, int value) { return _engine->setHint(hint, value); }

  // --------------------------------------------------------------------------
  // [Meta]
  // --------------------------------------------------------------------------

  //! @brief Set painter meta variables (meta origin, meta region) and reset
  //! all user variables (user origin, user region), clipping and states.
  //!
  //! This function was designed as an optimization for windowing systems to
  //! set all important origin and clipping information per one function call.
  FOG_INLINE err_t setMetaVars(const Region& region, const IntPoint& origin) { return _engine->setMetaVars(region, origin); }

  //! @brief Reset meta variables (setting meta region to infinite and meta
  //! origin to zero).
  //!
  //! This function also resets all clipping and states like @c setMetaVars()
  //! do.
  FOG_INLINE err_t resetMetaVars() { return _engine->resetMetaVars(); }

  //! @brief Set user variables (user origin, user region).
  FOG_INLINE err_t setUserVars(const Region& region, const IntPoint& origin) { return _engine->setUserVars(region, origin); }
  //! @brief Reset user variables (setting user region to infinite and user
  //! origin to zero).
  FOG_INLINE err_t resetUserVars() { return _engine->resetUserVars(); }

  //! @brief Get meta region.
  FOG_INLINE Region getMetaRegion() const { return _engine->getMetaRegion(); }
  //! @brief Get user region.
  FOG_INLINE Region getUserRegion() const { return _engine->getUserRegion(); }

  //! @brief Get meta origin.
  FOG_INLINE IntPoint getMetaOrigin() const { return _engine->getMetaOrigin(); }
  //! @brief Get user origin.
  FOG_INLINE IntPoint getUserOrigin() const { return _engine->getUserOrigin(); }

  // --------------------------------------------------------------------------
  // [Paint Parameters]
  // --------------------------------------------------------------------------

  //! @brief Get source type, see @c PAINTER_SOURCE_TYPE.
  FOG_INLINE uint32_t getSourceType() const { return _engine->getSourceType(); }

  //! @brief Get source color as @c Argb instance.
  //!
  //! If current source isn't solid color, the @c Argb(0x00000000) color is
  //! returned.
  FOG_INLINE Argb getSourceArgb() const { return _engine->getSourceArgb(); }

  //! @brief Get source pattern.
  //!
  //! If source color was set through @c setSource(Argb argb) method,
  //! pattern is created automatically.
  FOG_INLINE Pattern getSourcePattern() const { return _engine->getSourcePattern(); }

  //! @brief Set source as solid @a rgba color.
  FOG_INLINE err_t setSource(Argb argb) { return _engine->setSource(argb); }

  //! @brief Set source as pattern @a pattern.
  FOG_INLINE err_t setSource(const Pattern& pattern) { return _engine->setSource(pattern); }

  //! @brief Reset source to fully-opaque black color (0xFF000000).
  //!
  //! This is convenience method that can help to dereference the pattern
  //! previously set by setSource(const Pattern& pattern) method. If you
  //! are planning to modify the pattern and set it back then resetting
  //! source could improve performance when using single-threaded paint 
  //! engine.
  //!
  //! @note If you are using multi-threaded paint engine then resetting has
  //! usually no effect, because other thread(s) can keep the pattern instance 
  //! until all rendering where the pattern is used is finished.
  FOG_INLINE err_t resetSource() { return _engine->resetSource(); }

  //! @brief Get compositing operator.
  //!
  //! See @c OPERATOR_TYPE enumeration for operators and their descriptions.
  FOG_INLINE uint32_t getOperator() const { return _engine->getOperator(); }

  //! @brief Set compositing operator.
  //!
  //! See @c OPERATOR_TYPE enumeration for operators and their descriptions.
  FOG_INLINE err_t setOperator(uint32_t op) { return _engine->setOperator(op); }

  //! @brief Get the global painter opacity value.
  FOG_INLINE float getAlpha() const { return _engine->getAlpha(); }

  //! @brief Set the global painter opacity value to @a opacity.
  //! @param opacity The opacity valud to set (0.0 to 1.0 inclusive).
  FOG_INLINE err_t setAlpha(float alpha) { return _engine->setAlpha(alpha); }

  // --------------------------------------------------------------------------
  // [Clip Parameters]
  // --------------------------------------------------------------------------

  //! @brief Get clip fill rule, see @c CLIP_RULE enumeration.
  FOG_INLINE uint32_t getClipRule() const { return _engine->getClipRule(); }

  //! @brief Set clip fill rule, see @c CLIP_RULE enumeration.
  FOG_INLINE err_t setClipRule(uint32_t clipRule) { return _engine->setClipRule(clipRule); }

  // --------------------------------------------------------------------------
  // [Fill Parameters]
  // --------------------------------------------------------------------------

  //! @brief Get paint fill rule, see @c FILL_RULE enumeration.
  FOG_INLINE uint32_t getFillRule() const { return _engine->getFillRule(); }

  //! @brief Set paint fill rule, see @c FILL_RULE enumeration.
  FOG_INLINE err_t setFillRule(uint32_t fillRule) { return _engine->setFillRule(fillRule); }

  // --------------------------------------------------------------------------
  // [Stroke Parameters]
  // --------------------------------------------------------------------------

  //! @brief Get line parameters.
  FOG_INLINE PathStrokeParams getStrokeParams() const { return _engine->getStrokeParams(); }

  //! @brief Set line parameters.
  FOG_INLINE err_t setStrokeParams(const PathStrokeParams& strokeParams) { return _engine->setStrokeParams(strokeParams); }

  //! @brief Get line width.
  FOG_INLINE double getLineWidth() const { return _engine->getLineWidth(); }
  //! @brief Set line width.
  //!
  //! @note Line width is scaled by affine transformations.
  FOG_INLINE err_t setLineWidth(double lineWidth) { return _engine->setLineWidth(lineWidth); }

  //! @brief Get line start cap.
  FOG_INLINE uint32_t getStartCap() const { return _engine->getStartCap(); }

  //! @brief Set line start cap.
  FOG_INLINE err_t setStartCap(uint32_t startCap) { return _engine->setStartCap(startCap);}

  //! @brief Get line end cap.
  FOG_INLINE uint32_t getEndCap() const { return _engine->getEndCap(); }

  //! @brief Set line end cap.
  FOG_INLINE err_t setEndCap(uint32_t endCap) { return _engine->setEndCap(endCap);}

  //! @brief Set line start and end caps.
  FOG_INLINE err_t setLineCaps(uint32_t lineCaps) { return _engine->setLineCaps(lineCaps);}

  //! @brief Get line join.
  FOG_INLINE uint32_t getLineJoin() const { return _engine->getLineJoin(); }

  //! @brief Set line join.
  FOG_INLINE err_t setLineJoin(uint32_t lineJoin) { return _engine->setLineJoin(lineJoin); }

  //! @brief Get line miter limit.
  FOG_INLINE double getMiterLimit() const { return _engine->getMiterLimit(); }

  //! @brief Set line miter limit.
  FOG_INLINE err_t setMiterLimit(double miterLimit) { return _engine->setMiterLimit(miterLimit); }

  //! @brief Set line dash.
  FOG_INLINE List<double> getDashes() const { return _engine->getDashes(); }

  //! @brief Set line dash.
  FOG_INLINE err_t setDashes(const List<double>& dashes) { return _engine->setDashes(dashes); }

  //! @overload.
  FOG_INLINE err_t setDashes(const double* dashes, sysuint_t count) { return _engine->setDashes(dashes, count); }

  //! @brief Get line dash offset.
  FOG_INLINE double getDashOffset() const { return _engine->getDashOffset(); }

  //! @brief Set line dash offset.
  FOG_INLINE err_t setDashOffset(double offset) { return _engine->setDashOffset(offset); }

  // --------------------------------------------------------------------------
  // [Transformations]
  // --------------------------------------------------------------------------

  //! @brief Get current working matrix.
  FOG_INLINE DoubleMatrix getMatrix() const { return _engine->getMatrix(); }
  //! @brief Set current working matrix to @a m.
  FOG_INLINE err_t setMatrix(const DoubleMatrix& m) { return _engine->setMatrix(m); }
  //! @brief Reset current working matrix (to identity).
  FOG_INLINE err_t resetMatrix() { return _engine->resetMatrix(); }

  //! @brief Rotate current working matrix .
  FOG_INLINE err_t rotate(double angle, uint32_t order = MATRIX_PREPEND) { return _engine->rotate(angle, order); }

  FOG_INLINE err_t scale(int sx, int sy, uint32_t order = MATRIX_PREPEND) { return _engine->scale(sx, sy, order); }
  //! @brief Scale current working matrix.
  FOG_INLINE err_t scale(double sx, double sy, uint32_t order = MATRIX_PREPEND) { return _engine->scale(sx, sy, order); }

  //! @brief Skew current working matrix.
  FOG_INLINE err_t skew(double sx, double sy, uint32_t order = MATRIX_PREPEND) { return _engine->skew(sx, sy, order); }

  //! @brief Translate current working matrix.
  FOG_INLINE err_t translate(int x, int y, uint32_t order = MATRIX_PREPEND) { return _engine->translate(x, y, order); }
  //! @brief Translate current working matrix.
  FOG_INLINE err_t translate(double x, double y, uint32_t order = MATRIX_PREPEND) { return _engine->translate(x, y, order); }

  //! @brief Transform current working matrix.
  FOG_INLINE err_t transform(const DoubleMatrix& m, uint32_t order = MATRIX_PREPEND) { return _engine->transform(m, order); }

  //! @brief Convert world coordinate into screen one (using transformation matrix).
  FOG_INLINE err_t worldToScreen(DoublePoint* pt) const { return _engine->worldToScreen(pt); }
  //! @brief Convert screen coordinate into world one (using transformation matrix).
  FOG_INLINE err_t screenToWorld(DoublePoint* pt) const { return _engine->screenToWorld(pt); }

  //! @brief Align point to center of the raster (X.5, Y.5), after after all
  //! transformations were applied.
  //!
  //! This method is useful when you need to paint really sharp lines or other
  //! primitives.
  FOG_INLINE err_t alignPoint(DoublePoint* pt) const { return _engine->alignPoint(pt); }

  // --------------------------------------------------------------------------
  // [State]
  // --------------------------------------------------------------------------

  //! @brief Save current painter state.
  FOG_INLINE err_t save(uint32_t flags = PAINTER_STATE_ALL) { return _engine->save(flags); }
  //! @brief Restore current painter state.
  FOG_INLINE err_t restore() { return _engine->restore(); }

  // --------------------------------------------------------------------------
  // [Clipping]
  // --------------------------------------------------------------------------

  //! @brief Combine actual clip mask with rectangle @a rect.
  FOG_INLINE err_t clipRect(const IntRect& rect, uint32_t clipOp) { return _engine->clipRect(rect, clipOp); }
  //! @brief Combine actual clip mask with mask @a mask at position @a pt.
  FOG_INLINE err_t clipMask(const IntPoint& pt, const Image& mask, uint32_t clipOp) { return _engine->clipMask(pt, mask, clipOp); }
  //! @brief Combine actual clip mask with region @a region.
  FOG_INLINE err_t clipRegion(const Region& region, uint32_t clipOp) { return _engine->clipRegion(region, clipOp); }

  //! @brief Combine actual clip mask with rectangle @a rect.
  FOG_INLINE err_t clipRect(const DoubleRect& rect, uint32_t clipOp) { return _engine->clipRect(rect, clipOp); }
  //! @brief Combine actual clip mask with mask @a mask at position @a pt.
  FOG_INLINE err_t clipMask(const DoublePoint& pt, const Image& mask, uint32_t clipOp) { return _engine->clipMask(pt, mask, clipOp); }
  //! @brief Combine actual clip mask with rectangles @a r.
  FOG_INLINE err_t clipRects(const DoubleRect* r, sysuint_t count, uint32_t clipOp) { return _engine->clipRects(r, count, clipOp); }
  //! @brief Combine actual clip mask with round @a r.
  FOG_INLINE err_t clipRound(const DoubleRect& r, const DoublePoint& radius, uint32_t clipOp) { return _engine->clipRound(r, radius, clipOp); }
  //! @brief Combine actual clip mask with ellipse at @a cp with radius @a r.
  FOG_INLINE err_t clipEllipse(const DoublePoint& cp, const DoublePoint& r, uint32_t clipOp) { return _engine->clipEllipse(cp, r, clipOp); }
  //! @brief Combine actual clip mask with arc at @a cp.
  FOG_INLINE err_t clipArc(const DoublePoint& cp, const DoublePoint& r, double start, double sweep, uint32_t clipOp) { return _engine->clipArc(cp, r, start, sweep, clipOp); }
  //! @brief Combine actual clip mask with path @a path.
  FOG_INLINE err_t clipPath(const DoublePath& path, bool stroke, uint32_t clipOp) { return _engine->clipPath(path, stroke, clipOp); }

  //! @brief Reset clipping.
  FOG_INLINE err_t resetClip() { return _engine->resetClip(); }

  // --------------------------------------------------------------------------
  // [Raster Drawing]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t drawPoint(const IntPoint& p) { return _engine->drawPoint(p); }
  FOG_INLINE err_t drawLine(const IntPoint& start, const IntPoint& end) { return _engine->drawLine(start, end); }
  FOG_INLINE err_t drawRect(const IntRect& r) { return _engine->drawRect(r); }
  FOG_INLINE err_t drawRound(const IntRect& r, const IntPoint& radius) { return _engine->drawRound(r, radius); }
  FOG_INLINE err_t fillRect(const IntRect& r) { return _engine->fillRect(r); }
  FOG_INLINE err_t fillRects(const IntRect* r, sysuint_t count) { return _engine->fillRects(r, count); }
  FOG_INLINE err_t fillRound(const IntRect& r, const IntPoint& radius) { return _engine->fillRound(r, radius); }
  FOG_INLINE err_t fillRegion(const Region& region) { return _engine->fillRegion(region); }

  FOG_INLINE err_t fillAll() { return _engine->fillAll(); }

  // --------------------------------------------------------------------------
  // [Vector Drawing]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t drawPoint(const DoublePoint& p) { return _engine->drawPoint(p); }
  FOG_INLINE err_t drawLine(const DoublePoint& start, const DoublePoint& end) { return _engine->drawLine(start, end); }
  FOG_INLINE err_t drawLine(const DoublePoint* pts, sysuint_t count) { return _engine->drawLine(pts, count); }
  FOG_INLINE err_t drawPolygon(const DoublePoint* pts, sysuint_t count) { return _engine->drawPolygon(pts, count); }
  FOG_INLINE err_t drawRect(const DoubleRect& r) { return _engine->drawRect(r); }
  FOG_INLINE err_t drawRects(const DoubleRect* r, sysuint_t count) { return _engine->drawRects(r, count); }
  FOG_INLINE err_t drawRound(const DoubleRect& r, const DoublePoint& radius) { return _engine->drawRound(r, radius); }
  FOG_INLINE err_t drawEllipse(const DoublePoint& cp, const DoublePoint& r) { return _engine->drawEllipse(cp, r); }
  FOG_INLINE err_t drawArc(const DoublePoint& cp, const DoublePoint& r, double start, double sweep) { return _engine->drawArc(cp, r, start, sweep); }
  FOG_INLINE err_t drawPath(const DoublePath& path) { return _engine->drawPath(path); }

  FOG_INLINE err_t fillPolygon(const DoublePoint* pts, sysuint_t count) { return _engine->fillPolygon(pts, count); }
  FOG_INLINE err_t fillRect(const DoubleRect& r) { return _engine->fillRect(r); }
  FOG_INLINE err_t fillRects(const DoubleRect* r, sysuint_t count) { return _engine->fillRects(r, count); }
  FOG_INLINE err_t fillRound(const DoubleRect& r, const DoublePoint& radius) { return _engine->fillRound(r, radius); }
  FOG_INLINE err_t fillEllipse(const DoublePoint& cp, const DoublePoint& r) { return _engine->fillEllipse(cp, r); }
  FOG_INLINE err_t fillArc(const DoublePoint& cp, const DoublePoint& r, double start, double sweep) { return _engine->fillArc(cp, r, start, sweep); }
  FOG_INLINE err_t fillPath(const DoublePath& path) { return _engine->fillPath(path); }

  // --------------------------------------------------------------------------
  // [Glyph Drawing]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t drawGlyph(const IntPoint& pt, const Image& glyph, const IntRect* irect = 0)
  { return _engine->drawGlyph(pt, glyph, irect); }

  FOG_INLINE err_t drawGlyph(const DoublePoint& pt, const Image& glyph, const IntRect* irect = 0)
  { return _engine->drawGlyph(pt, glyph, irect); }

  FOG_INLINE err_t drawGlyph(const IntPoint& pt, const Glyph& glyph, const IntRect* grect = 0)
  { return _engine->drawGlyph(pt, glyph, grect); }

  FOG_INLINE err_t drawGlyph(const DoublePoint& pt, const Glyph& glyph, const IntRect* grect = 0)
  { return _engine->drawGlyph(pt, glyph, grect); }

  FOG_INLINE err_t drawGlyphSet(const IntPoint& pt, const GlyphSet& glyphSet, const IntRect* clip = 0)
  { return _engine->drawGlyphSet(pt, glyphSet, clip); }

  FOG_INLINE err_t drawGlyphSet(const DoublePoint& pt, const GlyphSet& glyphSet, const IntRect* clip = 0)
  { return _engine->drawGlyphSet(pt, glyphSet, clip); }

  // --------------------------------------------------------------------------
  // [Text drawing]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t drawText(const IntPoint& pt, const String& text, const Font& font, const IntRect* clip = NULL)
  { return _engine->drawText(pt, text, font, clip); }

  FOG_INLINE err_t drawText(const DoublePoint& pt, const String& text, const Font& font, const IntRect* clip = NULL)
  { return _engine->drawText(pt, text, font, clip); }

  FOG_INLINE err_t drawText(const IntRect& rect, const String& text, const Font& font, uint32_t align, const IntRect* clip = NULL)
  { return _engine->drawText(rect, text, font, align, clip); }

  FOG_INLINE err_t drawText(const DoubleRect& rect, const String& text, const Font& font, uint32_t align, const IntRect* clip = NULL)
  { return _engine->drawText(rect, text, font, align, clip); }

  // --------------------------------------------------------------------------
  // [Image Drawing]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t drawImage(const IntPoint& p, const Image& image)
  { return _engine->drawImage(p, image, NULL); }

  FOG_INLINE err_t drawImage(const IntPoint& p, const Image& image, const IntRect& irect)
  { return _engine->drawImage(p, image, &irect); }

  FOG_INLINE err_t drawImage(const DoublePoint& p, const Image& image)
  { return _engine->drawImage(p, image, NULL); }

  FOG_INLINE err_t drawImage(const DoublePoint& p, const Image& image, const IntRect& irect)
  { return _engine->drawImage(p, image, &irect); }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  //! @brief Null paint engine used for non-initialized painters.
  static PaintEngine* _dnull;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Paint engine instance.
  PaintEngine* _engine;

private:
  FOG_DISABLE_COPY(Painter);
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GRAPHICS_PAINTER_H
