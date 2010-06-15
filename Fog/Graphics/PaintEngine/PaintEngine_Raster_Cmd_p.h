// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_PAINTENGINE_RASTER_CMD_P_H
#define _FOG_GRAPHICS_PAINTENGINE_RASTER_CMD_P_H

// [Dependencies]
#include <Fog/Core/Assert.h>
#include <Fog/Core/Atomic.h>
#include <Fog/Core/Static.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Geometry.h>
#include <Fog/Graphics/Glyph.h>
#include <Fog/Graphics/GlyphSet.h>
#include <Fog/Graphics/Image.h>
#include <Fog/Graphics/Matrix.h>
#include <Fog/Graphics/Path.h>
#include <Fog/Graphics/Region.h>

#include <Fog/Graphics/PaintEngine/PaintEngine_Raster_Action_p.h>
#include <Fog/Graphics/PaintEngine/PaintEngine_Raster_Base_p.h>
#include <Fog/Graphics/PaintEngine/PaintEngine_Raster_Render_p.h>

namespace Fog {

//! @addtogroup Fog_Graphics_Private
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct RasterPaintCalc;
struct Rasterizer;

// ============================================================================
// [Fog::RasterPaintCmd]
// ============================================================================

//! @internal
//!
//! @brief Command - base class for all painter commands.
//!
//! Command is action that is executed by each painter's thread (worker). To
//! make sure that threads are working with own data the @c run() method must
//! use worker delta and offset variables.
//!
//! Commands are executed in FIFO (first-in, first-out) order for each thread,
//! but threads are not synchronized. It means that threads may process different
//! commands in parallel, but one-by-one.
//!
//! Command can depend to @c calculation. If calculation is not @c NULL and
//! command status is @c RASTER_COMMAND_WAIT then thread can't process
//! current command until command status is set to @c RASTER_COMMAND_READY
//! or @c RASTER_COMMAND_SKIP (that means skip this command). Instead of waiting
//! the worker can work on different calculation, see @c RasterWorkerTask::run()
//! implementation. Notice that command status is set by @c RasterPaintCalc if
//! used.
//!
//! After command is processed the @c RasterPaintAction::release() is called.
//! To create own command the method must be implemented and you must ensure
//! to call @c RasterPaintCmd::_releasePattern() for painting commands. When
//! writing command concentrate only to @c RasterPaintAction::release() method,
//! because all other stuff should be done by worker task.
struct FOG_HIDDEN RasterPaintCmd : public RasterPaintAction
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RasterPaintCmd() {};
  FOG_INLINE ~RasterPaintCmd() {};

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Reference count.
  //!
  //! Initial value is count of workers and each worker decreases it when
  //! finished. If value is decreased to zero worker must release the command
  //! using @c RasterPaintAction::release() method. This is done by the worker
  //! task.
  Atomic<int> refCount;

  //! @brief Status of this command.
  Atomic<int> status;

  //! @brief Calculation.
  RasterPaintCalc* calculation;
};

// ============================================================================
// [Fog::RasterPaintCmdLayerChange]
// ============================================================================

//! @internal
struct FOG_HIDDEN RasterPaintCmdLayerChange : public RasterPaintCmd
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RasterPaintCmdLayerChange() {};
  FOG_INLINE ~RasterPaintCmdLayerChange() {};

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual void run(RasterPaintContext* ctx);
  virtual void release(RasterPaintContext* ctx);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief The layer data.
  RasterPaintLayer paintLayer;

  IntPoint finalOrigin;

  uint32_t workClipType;
  uint32_t finalClipType;

  Static<Region> workRegion;
  Static<Region> finalRegion;
};

// ============================================================================
// [Fog::RasterPaintCmdUpdateFinalRegion]
// ============================================================================

//! @internal
//!
//! @brief Raster paint engine command that is used to update final clip
//! variables in worker threads.
//!
//! @note This command is used only to update final clip variables, it will not
//! update others (meta clip or work clip variables), becuase they can't change.
struct FOG_HIDDEN RasterPaintCmdUpdateRegion : public RasterPaintCmd
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RasterPaintCmdUpdateRegion() {};
  FOG_INLINE ~RasterPaintCmdUpdateRegion() {};

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual void run(RasterPaintContext* ctx);
  virtual void release(RasterPaintContext* ctx);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  IntPoint finalOrigin;

  uint32_t workClipType;
  uint32_t finalClipType;

  Static<Region> workRegion;
  Static<Region> finalRegion;
};

// ============================================================================
// [Fog::RasterPaintCmdDraw]
// ============================================================================

//! @internal
struct FOG_HIDDEN RasterPaintCmdDraw : public RasterPaintCmd
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RasterPaintCmdDraw() {};
  FOG_INLINE ~RasterPaintCmdDraw() {};

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  //! @brief Initialize paint command to classic painting (used by all paint
  //! commands except image
  //! blitting).
  FOG_INLINE void _initPaint(RasterPaintContext* ctx);

  //! @brief Initialize paint command for image blitting (used by all image
  //! blit commands, but not image affine blit commands).
  FOG_INLINE void _initBlit(RasterPaintContext* ctx);

  FOG_INLINE void _beforeBlit(RasterPaintContext* ctx);

  //! @brief Called before paint in worker thread, by @c run() method.
  FOG_INLINE void _beforePaint(RasterPaintContext* ctx);

  //! @brief Called after paint in worker thread, by @c run() method.
  FOG_INLINE void _afterPaint(RasterPaintContext* ctx);

  //! @brief This method destroys only general @c RasterPaintCmdDraw data.
  FOG_INLINE void _releasePattern(RasterPaintContext* ctx);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Context ops, set to the worker context, immediately.
  RasterPaintOps ops;

  union
  {
    //! @brief RasterSolid color (if source type is @c PAINTER_SOURCE_ARGB).
    RasterSolid solid;
    //! @brief Pattern context (if source type is @c PAINTER_SOURCE_PATTERN).
    RasterPattern* pctx;
  };
};

// ============================================================================
// [Fog::RasterPaintCmdRegion]
// ============================================================================

//! @internal
struct FOG_HIDDEN RasterPaintCmdRegion : public RasterPaintCmdDraw
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RasterPaintCmdRegion() {};
  FOG_INLINE ~RasterPaintCmdRegion() {};

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual void run(RasterPaintContext* ctx);
  virtual void release(RasterPaintContext* ctx);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Static<Region> region;
};

// ============================================================================
// [Fog::RasterPaintCmdBoxes]
// ============================================================================

//! @internal
struct FOG_HIDDEN RasterPaintCmdBoxes : public RasterPaintCmdDraw
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RasterPaintCmdBoxes() {};
  FOG_INLINE ~RasterPaintCmdBoxes() {};

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual void run(RasterPaintContext* ctx);
  virtual void release(RasterPaintContext* ctx);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  sysuint_t count;
  IntBox boxes[1];
};

// ============================================================================
// [Fog::RasterPaintCmdImage]
// ============================================================================

//! @internal
struct FOG_HIDDEN RasterPaintCmdImage : public RasterPaintCmdDraw
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RasterPaintCmdImage() {};
  FOG_INLINE ~RasterPaintCmdImage() {};

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual void run(RasterPaintContext* ctx);
  virtual void release(RasterPaintContext* ctx);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Static<Image> image;

  IntRect dst;
  IntRect src;
};

// ============================================================================
// [Fog::RasterPaintCmdGlyphSet]
// ============================================================================

//! @internal
struct FOG_HIDDEN RasterPaintCmdGlyphSet : public RasterPaintCmdDraw
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RasterPaintCmdGlyphSet() {};
  FOG_INLINE ~RasterPaintCmdGlyphSet() {};

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual void run(RasterPaintContext* ctx);
  virtual void release(RasterPaintContext* ctx);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Static<GlyphSet> glyphSet;

  IntPoint pt;
  IntBox boundingBox;
};

// ============================================================================
// [Fog::RasterPaintCmdPath]
// ============================================================================

//! @internal
struct FOG_HIDDEN RasterPaintCmdPath : public RasterPaintCmdDraw
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RasterPaintCmdPath() {};
  FOG_INLINE ~RasterPaintCmdPath() {};

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual void run(RasterPaintContext* ctx);
  virtual void release(RasterPaintContext* ctx);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Rasterizer (where path is rasterized by @c RasterPaintCalc).
  //!
  //! Rasterizer is set-up by calculation.
  Rasterizer* ras;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GRAPHICS_PAINTENGINE_RASTER_CMD_P_H
