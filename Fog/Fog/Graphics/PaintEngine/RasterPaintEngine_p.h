// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_PAINTENGINE_RASTERPAINTENGINE_P_H
#define _FOG_GRAPHICS_PAINTENGINE_RASTERPAINTENGINE_P_H

// [Dependencies]
#include <Fog/Core/Assert.h>
#include <Fog/Core/Lock.h>
#include <Fog/Core/Memory.h>
#include <Fog/Core/MemoryAllocator_p.h>
#include <Fog/Core/Thread.h>
#include <Fog/Core/ThreadCondition.h>
#include <Fog/Graphics/AnalyticRasterizer_p.h>
#include <Fog/Graphics/ImageFilter.h>
#include <Fog/Graphics/PaintEngine.h>
#include <Fog/Graphics/Path.h>
#include <Fog/Graphics/PathStroker.h>
#include <Fog/Graphics/RasterEngine_p.h>
#include <Fog/Graphics/Scanline_p.h>
#include <Fog/Graphics/Span_p.h>

#include <Fog/Graphics/PaintEngine/RasterPaintBase_p.h>
#include <Fog/Graphics/PaintEngine/RasterPaintCmd_p.h>
#include <Fog/Graphics/PaintEngine/RasterPaintContext_p.h>
#include <Fog/Graphics/PaintEngine/RasterPaintState_p.h>
#include <Fog/Graphics/PaintEngine/RasterPaintUtil_p.h>
#include <Fog/Graphics/PaintEngine/RasterPaintWorker_p.h>

namespace Fog {

//! @addtogroup Fog_Graphics_Private
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct RasterPaintAction;
struct RasterPaintCalc;
struct RasterPaintCmd;

// ============================================================================
// [Fog::RasterPaintEngine]
// ============================================================================

//! @internal
struct FOG_HIDDEN RasterPaintEngine : public PaintEngine
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Create new instance of @c RasterPaintEngine.
  //!
  //! @param buffer Buffer where to painting will be performed.
  //! @param imaged Image data (can be @c NULL), only for increasing and
  //! decreasing @c ImageData::locked counter.
  //! @param initFlags Initialization flags, see @c PAINTER_INIT_FLAGS.
  RasterPaintEngine(const ImageBuffer& buffer, ImageData* imaged, uint32_t initFlags);

  //! @brief Destroy paint engine, decreasing @c ImageData::locked if needed.
  virtual ~RasterPaintEngine();

  // --------------------------------------------------------------------------
  // [Width / Height / Format]
  // --------------------------------------------------------------------------

  virtual int getWidth() const;
  virtual int getHeight() const;
  virtual uint32_t getFormat() const;

  // --------------------------------------------------------------------------
  // [Engine / Flush]
  // --------------------------------------------------------------------------

  virtual uint32_t getEngine() const;
  virtual err_t setEngine(uint32_t engine, uint32_t threads);

  virtual err_t flush(uint32_t flags);

  //! @brief Special flush that is called when the paint engine instance is
  //! being destroyed (or switched to other buffer).
  void flushWithQuit();

  //! @brief Get whether the engine is singlethreaded.
  //!
  //! Engine is singlethreaded if @c workerManager is @c NULL, there isn't
  //! flag or something else.
  FOG_INLINE bool isSingleThreaded() const { return workerManager == NULL; }

  // --------------------------------------------------------------------------
  // [Hints]
  // --------------------------------------------------------------------------

  virtual int getHint(uint32_t hint) const;
  virtual err_t setHint(uint32_t hint, int value);

  // --------------------------------------------------------------------------
  // [Meta]
  // --------------------------------------------------------------------------

  virtual err_t setMetaVars(const Region& region, const IntPoint& origin);
  virtual err_t resetMetaVars();

  virtual err_t setUserVars(const Region& region, const IntPoint& origin);
  virtual err_t resetUserVars();

  virtual Region getMetaRegion() const;
  virtual Region getUserRegion() const;

  virtual IntPoint getMetaOrigin() const;
  virtual IntPoint getUserOrigin() const;

  // --------------------------------------------------------------------------
  // [Paint Parameters]
  // --------------------------------------------------------------------------

  virtual uint32_t getSourceType() const;

  virtual Argb getSourceArgb() const;
  virtual Pattern getSourcePattern() const;

  virtual err_t setSource(Argb argb);
  virtual err_t setSource(const Pattern& pattern);

  virtual err_t resetSource();

  virtual uint32_t getOperator() const;
  virtual err_t setOperator(uint32_t op);

  virtual float getAlpha() const;
  virtual err_t setAlpha(float alpha);

  // --------------------------------------------------------------------------
  // [Clip Parameters]
  // --------------------------------------------------------------------------

  virtual uint32_t getClipRule() const;
  virtual err_t setClipRule(uint32_t clipRule);

  // --------------------------------------------------------------------------
  // [Fill Parameters]
  // --------------------------------------------------------------------------

  virtual uint32_t getFillRule() const;
  virtual err_t setFillRule(uint32_t fillRule);

  // --------------------------------------------------------------------------
  // [Stroke Parameters]
  // --------------------------------------------------------------------------

  virtual PathStrokeParams getStrokeParams() const;
  virtual err_t setStrokeParams(const PathStrokeParams& strokeParams);

  virtual double getLineWidth() const;
  virtual err_t setLineWidth(double lineWidth);

  virtual uint32_t getStartCap() const;
  virtual err_t setStartCap(uint32_t startCap);

  virtual uint32_t getEndCap() const;
  virtual err_t setEndCap(uint32_t endCap);

  virtual err_t setLineCaps(uint32_t lineCaps);

  virtual uint32_t getLineJoin() const;
  virtual err_t setLineJoin(uint32_t lineJoin);

  virtual double getMiterLimit() const;
  virtual err_t setMiterLimit(double miterLimit);

  virtual List<double> getDashes() const;
  virtual err_t setDashes(const double* dashes, sysuint_t count);
  virtual err_t setDashes(const List<double>& dashes);

  virtual double getDashOffset() const;
  virtual err_t setDashOffset(double offset);

  // --------------------------------------------------------------------------
  // [Transformations]
  // --------------------------------------------------------------------------

  virtual DoubleMatrix getMatrix() const;
  virtual err_t setMatrix(const DoubleMatrix& m);
  virtual err_t resetMatrix();

  virtual err_t rotate(double angle, uint32_t order);

  virtual err_t scale(int sx, int sy, uint32_t order);
  virtual err_t scale(double sx, double sy, uint32_t order);

  virtual err_t skew(double sx, double sy, uint32_t order);

  virtual err_t translate(int x, int y, uint32_t order);
  virtual err_t translate(double x, double y, uint32_t order);

  virtual err_t transform(const DoubleMatrix& m, uint32_t order);

  virtual err_t worldToScreen(DoublePoint* pt) const;
  virtual err_t screenToWorld(DoublePoint* pt) const;

  virtual err_t alignPoint(DoublePoint* pt) const;

  // --------------------------------------------------------------------------
  // [State]
  // --------------------------------------------------------------------------

  virtual err_t save(uint32_t flags);
  virtual err_t restore();

  // --------------------------------------------------------------------------
  // [Clipping]
  // --------------------------------------------------------------------------

  virtual err_t clipRect(const IntRect& rect, uint32_t clipOp);
  virtual err_t clipMask(const IntPoint& pt, const Image& mask, uint32_t clipOp);
  virtual err_t clipRegion(const Region& region, uint32_t clipOp);

  virtual err_t clipRect(const DoubleRect& rect, uint32_t clipOp);
  virtual err_t clipMask(const DoublePoint& pt, const Image& mask, uint32_t clipOp);
  virtual err_t clipRects(const DoubleRect* r, sysuint_t count, uint32_t clipOp);
  virtual err_t clipRound(const DoubleRect& r, const DoublePoint& radius, uint32_t clipOp);
  virtual err_t clipEllipse(const DoublePoint& cp, const DoublePoint& r, uint32_t clipOp);
  virtual err_t clipArc(const DoublePoint& cp, const DoublePoint& r, double start, double sweep, uint32_t clipOp);
  virtual err_t clipPath(const DoublePath& path, bool stroke, uint32_t clipOp);

  virtual err_t resetClip();

  // --------------------------------------------------------------------------
  // [Raster Drawing]
  // --------------------------------------------------------------------------

  virtual err_t drawPoint(const IntPoint& p);
  virtual err_t drawLine(const IntPoint& start, const IntPoint& end);
  virtual err_t drawRect(const IntRect& r);
  virtual err_t drawRound(const IntRect& r, const IntPoint& radius);
  virtual err_t fillRect(const IntRect& r);
  virtual err_t fillRects(const IntRect* r, sysuint_t count);
  virtual err_t fillRound(const IntRect& r, const IntPoint& radius);
  virtual err_t fillRegion(const Region& region);

  virtual err_t fillAll();

  // --------------------------------------------------------------------------
  // [Vector Drawing]
  // --------------------------------------------------------------------------

  virtual err_t drawPoint(const DoublePoint& p);
  virtual err_t drawLine(const DoublePoint& start, const DoublePoint& end);
  virtual err_t drawLine(const DoublePoint* pts, sysuint_t count);
  virtual err_t drawPolygon(const DoublePoint* pts, sysuint_t count);
  virtual err_t drawRect(const DoubleRect& r);
  virtual err_t drawRects(const DoubleRect* r, sysuint_t count);
  virtual err_t drawRound(const DoubleRect& r, const DoublePoint& radius);
  virtual err_t drawEllipse(const DoublePoint& cp, const DoublePoint& r);
  virtual err_t drawArc(const DoublePoint& cp, const DoublePoint& r, double start, double sweep);
  virtual err_t drawPath(const DoublePath& path);

  virtual err_t fillPolygon(const DoublePoint* pts, sysuint_t count);
  virtual err_t fillRect(const DoubleRect& r);
  virtual err_t fillRects(const DoubleRect* r, sysuint_t count);
  virtual err_t fillRound(const DoubleRect& r, const DoublePoint& radius);
  virtual err_t fillEllipse(const DoublePoint& cp, const DoublePoint& r);
  virtual err_t fillArc(const DoublePoint& cp, const DoublePoint& r, double start, double sweep);
  virtual err_t fillPath(const DoublePath& path);

  // --------------------------------------------------------------------------
  // [Glyph Drawing]
  // --------------------------------------------------------------------------

  virtual err_t drawGlyph(const IntPoint&p, const Image& glyph, const IntRect* irect);
  virtual err_t drawGlyph(const DoublePoint&p, const Image& glyph, const IntRect* irect);

  virtual err_t drawGlyph(const IntPoint& pt, const Glyph& glyph, const IntRect* grect);
  virtual err_t drawGlyph(const DoublePoint& pt, const Glyph& glyph, const IntRect* grect);

  virtual err_t drawGlyphSet(const IntPoint& pt, const GlyphSet& glyphSet, const IntRect* clip);
  virtual err_t drawGlyphSet(const DoublePoint& pt, const GlyphSet& glyphSet, const IntRect* clip);

  // --------------------------------------------------------------------------
  // [Text Drawing]
  // --------------------------------------------------------------------------

  virtual err_t drawText(const IntPoint& pt, const String& text, const Font& font, const IntRect* clip);
  virtual err_t drawText(const DoublePoint& pt, const String& text, const Font& font, const IntRect* clip);

  virtual err_t drawText(const IntRect& rect, const String& text, const Font& font, uint32_t align, const IntRect* clip);
  virtual err_t drawText(const DoubleRect& rect, const String& text, const Font& font, uint32_t align, const IntRect* clip);

  // --------------------------------------------------------------------------
  // [Image drawing]
  // --------------------------------------------------------------------------

  virtual err_t drawImage(const IntPoint& p, const Image& image, const IntRect* irect);
  virtual err_t drawImage(const DoublePoint& p, const Image& image, const IntRect* irect);

  virtual err_t drawImage(const IntRect& rect, const Image& image, const IntRect* irect);
  virtual err_t drawImage(const DoubleRect& rect, const Image& image, const IntRect* irect);

  err_t stretchImageHelper(
    const DoublePoint& pt, const Image& image, const IntRect& srcRect,
    double scaleX, double scaleY);

  // --------------------------------------------------------------------------
  // [Helpers - Core]
  // --------------------------------------------------------------------------

  //! @brief Switch painter to different buffer. Like calling @c Painter::end()
  //! and @c Painter::begin(), but the most allocated data structures are
  //! reused.
  err_t switchTo(const ImageBuffer& buffer, ImageData* d);

  void _endWithPainting();

  // --------------------------------------------------------------------------
  // [Helpers - Region]
  // --------------------------------------------------------------------------

  //! @brief Called when meta or user region was changed.
  //!
  //! The purpose of this function is to combine meta + user region and to
  //! reset the clip region (or clip mask). The resulting (meta & user) region
  //! is called work region. Work region is never changed during painting, can
  //! be change only by another @c _updateFinalRegion() call.
  //!
  //! If user need the current clip region (that can be saved or restored by
  //! calling save() / restore() sequences) then raster paint engine will
  //! always combine this custom region with work region, the resulting region
  //! is finalRegion that is used by renderers (_doPaint... methods).
  //!
  //! Called by @c setMetaVars() and @c setUserVars() methods.
  //!
  //! @sa _updateFinalRegion().
  //!
  //! @note Calling this method will reset current clipping set by @c clip...
  //! methods.
  err_t _updateWorkRegion();

  //! @brief Called to combine work region with clip region.
  //!
  //! Clip region is result of calls to @c clip...() methods that can be aligned
  //! to the pixel grid. Resulting region is called final region and this region
  //! is used by renderers.
  // err_t _updateFinalRegion();

  // --------------------------------------------------------------------------
  // [Helpers - Caps]
  // --------------------------------------------------------------------------

  err_t _updateMatrix(bool translationChangedOnly);

  FOG_INLINE void _updateLineWidth()
  {
    ctx.hints.lineIsSimple = (
      ctx.strokeParams.getLineWidth() == 1.0 &&
      ctx.strokeParams.getDashes().getLength() == 0);
  }

  // --------------------------------------------------------------------------
  // [Helpers - Defaults]
  // --------------------------------------------------------------------------

  void _setClipDefaults();
  void _setCapsDefaults();

  // --------------------------------------------------------------------------
  // [Helpers - Pattern]
  // --------------------------------------------------------------------------

  RasterPattern* _getRasterPatternContext();
  void _resetRasterPatternContext();

  // --------------------------------------------------------------------------
  // [Helpers - Clipping]
  // --------------------------------------------------------------------------

  //! @brief Do clip operation with null region.
  err_t _clipOpNull(uint32_t clipOp);

  //! @brief Do clip operation with infinite.
  err_t _clipOpInfinite(uint32_t clipOp);

  //! @brief Do clip operation with box.
  err_t _clipOpBox(const IntBox& box, uint32_t clipOp);

  //! @brief Do clip operation with region.
  err_t _clipOpRegion(const Region& region, uint32_t clipOp);

  //! @brief Set clip box to empty rectangle.
  //!
  //! Helper method that will destroy clip mask (if exists) and set clipping
  //! to null box. Clip type is set to @c RASTER_CLIP_NULL and schedule flags
  //! are set when running in the multithreaded mode.
  err_t _clipSetNull();

  //! @brief Set default clip region.
  err_t _clipSetDefault();

  //! @brief Set clip box to @a box.
  //!
  //! Helper method that will destroy clip mask (if exists) and set clipping
  //! to @a box. Clip type is set to @c RASTER_CLIP_SIMPLE and schedule flags
  //! are set when running in the multithreaded mode.
  err_t _clipSetBox(const IntBox& box);

  //! @brief Set clip region to @a region.
  //!
  //! Helper method that will destroy clip mask (if exists) and set clipping
  //! to @a region. Clip type is set to @c RASTER_CLIP_REGION and schedule flags
  //! are set when running in the multithreaded mode.
  err_t _clipSetRegion(const Region& region);

  // --------------------------------------------------------------------------
  // [Helpers - States]
  // --------------------------------------------------------------------------

  FOG_INLINE void _deleteState(RasterPaintState* state)
  {
    state->~RasterPaintState();
    blockAllocator.free(state);
  }

  void _restoreStates(uint32_t layerId);

  // --------------------------------------------------------------------------
  // [Helpers - Layer]
  // --------------------------------------------------------------------------

  //! @brief Setup layer.
  //!
  //! Fills layer secondaryFormat, toSecondary and fromSecondary members.
  void _setupPaintLayer(RasterPaintLayer* layer);

  // --------------------------------------------------------------------------
  // [Serializers - Painting]
  // --------------------------------------------------------------------------

  // Serializers are always called from painter thread.
  err_t _serializePaintRegion(const Region& region);
  err_t _serializePaintBoxes(const IntBox* box, sysuint_t count);
  err_t _serializePaintImage(const IntRect& dst, const Image& image, const IntRect& src);
  err_t _serializePaintImageAffine(const DoublePoint& pt, const Image& image, const IntRect& irect);
  err_t _serializePaintGlyphSet(const IntPoint& pt, const GlyphSet& glyphSet, const IntRect* clip);
  err_t _serializePaintPath(const DoublePath& path, bool stroke);

  // --------------------------------------------------------------------------
  // [Serializers - Clipping]
  // --------------------------------------------------------------------------

  RasterClipMask* _allocClipMask(const IntBox& workBox);
  void _destroyClipMask(RasterClipMask* mask);

  //! @brief Serialize new clip mask command.
  //!
  //! Can be used only to switch between null/simple/region clip to mask-clip
  //! mode.
  err_t _serializeMaskNew();

  //! @brief Serialize reset clip mask (or destroy it immediately).
  //!
  //! Called always after the new region was set. This action can destroy the
  //! current mask if it's reference count is decreased to zero.
  err_t _serializeMaskReset();

  //! @brief Serialize convert current clip region to mask command.
  //!
  //! Used when clipping operation that is not CLIP_OP_REPLACE is scheduled when
  //! using RASTER_CLIP_SIMPLE or RASTER_CLIP_REGION mode.
  //!
  //! @param bounds Limit bounds of new mask to @a bounds. Used when next clip
  //! operator is @c CLIP_OP_INTERSECT to prevent creating of unnecessary spans.
  //! In all other cases the bounds must be get by ctx.finalRegion.getExtents().
  err_t _serializeMaskConvert(const IntBox& bounds);

  // Serializers are only used if we are working with RASTER_CLIP_MASK clipping.
  // If clip can be represented using Region then serializers are not used, but
  // some flags may be scheduled using master context state - ctx.state.

  err_t _serializeMaskSave();
  err_t _serializeMaskRestore(RasterClipMask* toMask);

  err_t _serializeMaskBox(const IntBox& box, uint32_t clipOp);
  // err_t _serializeClipMask(const IntPoint& pt, const Image& mask, const IntBox& ibox);
  err_t _serializeMaskRegion(const Region& region, uint32_t clipOp);
  err_t _serializeMaskPath(const DoublePath& path, bool stroke, uint32_t clipOp);

  // --------------------------------------------------------------------------
  // [Serializers - Helpers]
  // --------------------------------------------------------------------------

  // BIG NOTE:
  //
  // There is something wrong in Visual Studio 2005. In older code there was
  // only one function:
  //
  //    template<typename T>
  //    FOG_INLINE T* _createCommand(sysuint_t size = sizeof(T));
  //
  // But the size returned by sizeof(T) was 32 under 32-bit mode.
  //
  // So, if the memory corruption happen again, check whether the size in alloc()
  // corresponds to size of the structure. This was really hard to find bug and
  // I don't like to find it again.
  //
  // - Petr
  template<typename T> FOG_INLINE T* _createCommand();
  template<typename T> FOG_INLINE T* _createCommand(sysuint_t size);
  template<typename T> FOG_INLINE T* _createCalc();

  FOG_INLINE void _beforeNewAction();

  //! @brief Post command @a cmd.
  void _postCommand(RasterPaintCmd* cmd);

  //! @brief Post command @a cmd and calculation @a clc.
  void _postCommand(RasterPaintCmd* cmd, RasterPaintCalc* clc);

  //! @brief Post pending commands encoded in @c ctx.state.
  void _postPending();

  // --------------------------------------------------------------------------
  // [Fatal]
  // --------------------------------------------------------------------------

  //! @brief Called when some work failed due to memory allocation error,
  //! always from master context.
  //! @param err The error that will be returned (this is convenience).
  //!
  //! This method will set the paint engine to FATAL state, setting
  //! @c RASTER_STATE_FATAL_ERROR bit in the master context state. All clipping
  //! regions are purged (no paint) and appropriate flags are set, so no
  //! painting is possible after calling this method.
  err_t _failed(err_t err);

  // --------------------------------------------------------------------------
  // [Rasterization]
  // --------------------------------------------------------------------------

  //! @brief Rasterizer used only by singlethreaded mode. Output is places to
  //! the default rasterizer.
  //!
  //! @note Can be only called if singlethreaded mode is used. There is assert
  //! so don't try it in mt-mode :)
  bool _doRasterizePath_st(const DoublePath& path, const IntBox& clipBox, uint32_t fillRule, bool stroke);

  // --------------------------------------------------------------------------
  // [Renderers - Painting]
  // --------------------------------------------------------------------------

  // Paint region represented by @a count of boxes in @a box.
  //
  // Input boxes are clipped to finalRegion (or workRegion if clip-mask is used).
  static void _doPaintBoxes_st(RasterPaintContext* ctx, const IntBox* box, sysuint_t count);
  static void _doPaintBoxes_mt(RasterPaintContext* ctx, const IntBox* box, sysuint_t count);

  // Paint an image.
  static void _doPaintImage_st(RasterPaintContext* ctx, const IntRect& dst, const Image& image, const IntRect& src);
  static void _doPaintImage_mt(RasterPaintContext* ctx, const IntRect& dst, const Image& image, const IntRect& src);

  // Paint a glyph set.
  static void _doPaintGlyphSet_st(RasterPaintContext* ctx, const IntPoint& pt, const GlyphSet& glyphSet, const IntBox& boundingBox);
  static void _doPaintGlyphSet_mt(RasterPaintContext* ctx, const IntPoint& pt, const GlyphSet& glyphSet, const IntBox& boundingBox);

  // Paint an output from rasterizer (rasterized path).
  static void _doPaintPath_st(RasterPaintContext* ctx, AnalyticRasterizer8* ras);
  static void _doPaintPath_mt(RasterPaintContext* ctx, AnalyticRasterizer8* ras);

  // --------------------------------------------------------------------------
  // [Renderers - Clipping]
  // --------------------------------------------------------------------------

  // Convert clip region represented by @a count of boxes in @a box.
  //
  // Used to convert region into non-initialized mask, rows will be overwritten.
  static void _doMaskConvert_st(RasterPaintContext* ctx, const IntBox* box, sysuint_t count);
  static void _doMaskConvert_mt(RasterPaintContext* ctx, const IntBox* box, sysuint_t count);

  // Combine region represented by @a count of boxes in @a box with clip-mask.
  static void _doMaskBoxes_st(RasterPaintContext* ctx, const IntBox* box, sysuint_t count, uint32_t clipOp);
  static void _doMaskBoxes_mt(RasterPaintContext* ctx, const IntBox* box, sysuint_t count, uint32_t clipOp);

  // Combine rasterized path with clip-mask.
  static void _doMaskPath_st(RasterPaintContext* ctx, AnalyticRasterizer8* ras, uint32_t clipOp);
  static void _doMaskPath_mt(RasterPaintContext* ctx, AnalyticRasterizer8* ras, uint32_t clipOp);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Custom memory allocator instance used by the raster paint engine
  //! for the most memory allocations.
  BlockMemoryAllocator blockAllocator;

  //! @brief Main raster context.
  RasterPaintMasterContext ctx;

  //! @brief States stack (for @c save() and @c restore() methods).
  List<RasterPaintState*> states;

  // If we are running in single-core environment it's better to use one
  // rasterizer for everythging.
  AnalyticRasterizer8 rasterizer;

  // Multithreading
  RasterWorkerManager* workerManager;

  // Temporary path.
  DoublePath curPath;

  // Temporary path #0.
  DoublePath tmpPath0;
  // Temporary path #1.
  DoublePath tmpPath1;

  // Temporary glyph set.
  GlyphSet tmpGlyphSet;

  // Temporary region #0.
  Region tmpRegion0;
  // Temporary region #1.
  Region tmpRegion1;

  //! @brief Whether the engine is in destructor.
  //!
  //! This member is here mainly to inform setEngine() that it can't fail,
  //! because painter is being destroyed.
  uint finishing;
};

// ============================================================================
// [Inline]
// ============================================================================

FOG_INLINE void RasterPaintCmdDraw::_initPaint(RasterPaintContext* ctx)
{
  ops.data = ctx->ops.data;

  if (FOG_LIKELY(ops.sourceType == PAINTER_SOURCE_ARGB))
  {
    solid = ctx->solid;
  }
  else // if (ctx.ops.sourceType == PAINTER_SOURCE_PATTERN)
  {
    // Pattern context must be initialized if we are here.
    FOG_ASSERT(ctx->pctx && ctx->pctx->initialized);
    pctx = ctx->pctx;
    pctx->refCount.inc();
  }
}

FOG_INLINE void RasterPaintCmdDraw::_beforeBlit(RasterPaintContext* ctx)
{
  // Used before image is blit to the destination buffer. Image blitting
  // is not using patterns (except for affine unbound blit which need
  // _beforePaint() method instead).
  FOG_ASSERT(ops.sourceType == PAINTER_SOURCE_ARGB);

  ctx->ops.data = ops.data;
  ctx->funcs = rasterFuncs.getCompositeFuncs(ops.op, ctx->paintLayer.format);
}

FOG_INLINE void RasterPaintCmdDraw::_beforePaint(RasterPaintContext* ctx)
{
  ctx->ops.data = ops.data;
  ctx->funcs = rasterFuncs.getCompositeFuncs(ops.op, ctx->paintLayer.format);

  if (FOG_LIKELY(ops.sourceType == PAINTER_SOURCE_ARGB))
    ctx->solid = solid;
  else
    ctx->pctx = pctx;
}

FOG_INLINE void RasterPaintCmdDraw::_afterPaint(RasterPaintContext* ctx)
{
  // It's not needed to set the pctx to NULL so we omit it in release mode.
  // However in debug mode we like to see the bugs:
#if defined(FOG_DEBUG)
  ctx->pctx = NULL;
#endif // FOG_DEBUG
}

FOG_INLINE void RasterPaintCmdDraw::_initBlit(RasterPaintContext* ctx)
{
  ops.data = ctx->ops.data;
  ops.sourceType = PAINTER_SOURCE_ARGB;
}

FOG_INLINE void RasterPaintCmdDraw::_releasePattern(RasterPaintContext* ctx)
{
  if (ops.sourceType == PAINTER_SOURCE_PATTERN && pctx->refCount.deref())
  {
    pctx->destroy(pctx);
    ctx->engine->blockAllocator.free(pctx);
  }
}

template <typename T>
FOG_INLINE T* RasterPaintEngine::_createCommand()
{
  T* command = reinterpret_cast<T*>(workerManager->commandAllocator.alloc(sizeof(T)));
  if (FOG_UNLIKELY(command == NULL)) return NULL;

  fog_new_p(command) T;

  command->refCount.init((uint)workerManager->numWorkers);
  command->status.init(RASTER_COMMAND_READY);
  command->calculation = NULL;

  return command;
}

template <typename T>
FOG_INLINE T* RasterPaintEngine::_createCommand(sysuint_t size)
{
  // Sanity...
  FOG_ASSERT(size >= sizeof(T));

  T* command = reinterpret_cast<T*>(workerManager->commandAllocator.alloc(size));
  if (FOG_UNLIKELY(command == NULL)) return NULL;

  fog_new_p(command) T;

  command->refCount.init((uint)workerManager->numWorkers);
  command->status.init(RASTER_COMMAND_READY);
  command->calculation = NULL;

  return command;
}

template<typename T>
T* RasterPaintEngine::_createCalc()
{
  T* calculation = reinterpret_cast<T*>(workerManager->commandAllocator.alloc(sizeof(T)));
  if (FOG_UNLIKELY(calculation == NULL)) return NULL;

  return fog_new_p(calculation) T;
}

FOG_INLINE void RasterPaintEngine::_beforeNewAction()
{
  // Flush everything if commands get maximum. We need to ensure that there are
  // at least four commands left, because _postCommand() can create some change
  // clip / region / something else commands.
  if (FOG_UNLIKELY(workerManager->cmdPosition >= RASTER_MAX_COMMANDS - 3))
  {
#if defined(FOG_DEBUG_RASTER_COMMANDS)
    fog_debug("Fog::Painter::_postCommand() - command buffer is full");
#endif // FOG_DEBUG_RASTER_COMMANDS
    flush(PAINTER_FLUSH_SYNC);
  }
}

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GRAPHICS_PAINTENGINE_RASTERPAINTENGINE_P_H
