// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTERPAINTACTION_P_H
#define _FOG_G2D_PAINTING_RASTERPAINTACTION_P_H

// [Dependencies]
#include <Fog/G2d/Painting/PaintParams.h>
#include <Fog/G2d/Painting/RasterPaintConstants_p.h>
#include <Fog/G2d/Painting/RasterPaintStructs_p.h>
#include <Fog/G2d/Render/RenderStructs_p.h>

namespace Fog {

//! @addtogroup Fog_G2d_Painting_Raster
//! @{

// ============================================================================
// [Fog::RasterDoLayerChange]
// ============================================================================

struct FOG_NO_EXPORT RasterDoLayerChange
{
  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  RasterLayer layer;
  PointI finalOrigin;

  uint32_t workClipType;
  uint32_t finalClipType;

  Static<Region> workRegion;
  Static<Region> finalRegion;
};

// ============================================================================
// [Fog::RasterDoUpdateFinalRegion]
// ============================================================================

//! @internal
//!
//! @brief Raster paint engine command that is used to update final clip
//! variables in worker threads.
//!
//! @note This command is used only to update final clip variables, it will not
//! update others (meta clip or work clip variables), becuase they can't change.
struct FOG_NO_EXPORT RasterDoUpdateFinalRegion
{
  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  PointI finalOrigin;

  uint32_t workClipType;
  uint32_t finalClipType;

  Static<Region> workRegion;
  Static<Region> finalRegion;
};

// ============================================================================
// [Fog::RasterDoDraw]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT RasterDoDraw
{
  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  //! @brief Initialize paint command to classic painting (used by all paint
  //! commands except image
  //! blitting).
  FOG_INLINE void _initPaint(RasterContext* ctx);

  //! @brief Initialize paint command for image blitting (used by all image
  //! blit commands, but not image affine blit commands).
  FOG_INLINE void _initBlit(RasterContext* ctx);

  FOG_INLINE void _beforeBlit(RasterContext* ctx);

  //! @brief Called before paint in worker thread, by @c run() method.
  FOG_INLINE void _beforePaint(RasterContext* ctx);

  //! @brief Called after paint in worker thread, by @c run() method.
  FOG_INLINE void _afterPaint(RasterContext* ctx);

  //! @brief This method destroys only general @c RasterCmdDraw data.
  FOG_INLINE void _releasePattern(RasterContext* ctx);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Context paint hints, set to the worker context, immediately.
  PaintHints paintHints;
  //! @brief Context raster hints, set to the worker context, immediately.
  RasterHints rasterHints;
  //! @brief Context opacity-ops, set to the worker context, immediately.
  uint32_t opacity;

  union
  {
    //! @brief RenderSolid color (if source type is @c PATTERN_SOURCE_COLOR).
    RenderSolid solid;
    //! @brief Pattern context (if source type is @c PATTERN_SOURCE_PATTERN).
    RenderPatternContext* pctx;
  };
};

// ============================================================================
// [Fog::RasterCmdRegion]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT RasterDoRegion : public RasterDoDraw
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RasterDoRegion() {};
  FOG_INLINE ~RasterDoRegion() {};

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual void run(RasterContext* ctx);
  virtual void release(RasterContext* ctx);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Static<Region> region;
};

// ============================================================================
// [Fog::RasterCmdBoxes]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT RasterDoBoxes : public RasterDoDraw
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RasterDoBoxes() {};
  FOG_INLINE ~RasterDoBoxes() {};

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual void run(RasterContext* ctx);
  virtual void release(RasterContext* ctx);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  size_t count;
  BoxI boxes[1];
};

// ============================================================================
// [Fog::RasterCmdImage]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT RasterDoImage : public RasterDoDraw
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RasterDoImage() {};
  FOG_INLINE ~RasterDoImage() {};

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual void run(RasterContext* ctx);
  virtual void release(RasterContext* ctx);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Static<Image> image;

  RectI dst;
  RectI src;
};

// ============================================================================
// [Fog::RasterCmdGlyphSet]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT RasterDoGlyphSet : public RasterDoDraw
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RasterDoGlyphSet() {};
  FOG_INLINE ~RasterDoGlyphSet() {};

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual void run(RasterContext* ctx);
  virtual void release(RasterContext* ctx);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  // Static<GlyphSet> glyphSet;

  PointI pt;
  BoxI boundingBox;
};

// ============================================================================
// [Fog::RasterCmdPath]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT RasterDoPath : public RasterDoDraw
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RasterDoPath() {};
  FOG_INLINE ~RasterDoPath() {};

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual void run(RasterContext* ctx);
  virtual void release(RasterContext* ctx);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Rasterizer (where path is rasterized by @c RasterCalc).
  //!
  //! Rasterizer is set-up by calculation.
  Static<Rasterizer8> rasterizer;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTERPAINTACTION_P_H
