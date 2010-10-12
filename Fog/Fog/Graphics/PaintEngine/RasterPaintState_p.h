// [Fog-Graphics]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_PAINTENGINE_RASTERPAINTSTATE_P_H
#define _FOG_GRAPHICS_PAINTENGINE_RASTERPAINTSTATE_P_H

// [Dependencies]
#include <Fog/Core/Assert.h>
#include <Fog/Graphics/Geometry.h>
#include <Fog/Graphics/PathStroker.h>
#include <Fog/Graphics/Pattern.h>
#include <Fog/Graphics/Region.h>
#include <Fog/Graphics/Transform.h>

#include <Fog/Graphics/PaintEngine/RasterPaintBase_p.h>

namespace Fog {

//! @addtogroup Fog_Graphics_Private
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct RasterPaintEngine;

// ============================================================================
// [Fog::RasterPaintState]
// ============================================================================

//! @internal
//!
//! @brief Raster paint engine state.
struct FOG_HIDDEN RasterPaintState
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RasterPaintState(uint32_t layerId, uint32_t flags) :
    layerId(layerId),
    flags(flags)
  {
  }

  FOG_INLINE ~RasterPaintState()
  {
  }

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  //! @brief Save state (can be called only once).
  virtual void save(RasterPaintEngine* engine);

  //! @brief Restore state and free all variables inside (can be called only
  //! once).
  virtual void restore(RasterPaintEngine* engine);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief State id. Used internally to detect if @c PaintEngine::release() is
  //! not popping state in different layer.
  uint32_t layerId;

  //! @brief State flags (what is in the state).
  uint32_t flags;
};

// ============================================================================
// [Fog::RasterPaintVarState]
// ============================================================================

//! @internal
struct FOG_HIDDEN RasterPaintVarState : public RasterPaintState
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RasterPaintVarState(uint32_t layerId, uint32_t flags) :
    RasterPaintState(layerId, flags)
  {
  }

  FOG_INLINE ~RasterPaintVarState()
  {
  }

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual void save(RasterPaintEngine* engine);
  virtual void restore(RasterPaintEngine* engine);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Context paint ops.
  //!
  //! Used by @c PAINTER_STATE_PAINT_PARAMS.
  RasterPaintOps ops;

  //! @brief Context paint hints.
  //!
  //! Used by @c PAINTER_STATE_HINTS,
  //!         @c PAINTER_STATE_CLIP_PARAMS,
  //!         @c PAINTER_STATE_FILL_PARAMS.
  RasterPaintHints hints;

  //! @brief Work clip type.
  //!
  //! Used by @c PAINTER_STATE_CLIP_AREA.
  uint32_t workClipType;

  //! @brief Final clip type.
  //!
  //! Used by @c PAINTER_STATE_CLIP_AREA.
  uint32_t finalClipType;

  //! @brief Some no-paint flags.
  uint32_t clipNoPaint;

  //! @brief Paint alpha.
  //!
  //! Used by @c PAINTER_STATE_PAINT_PARAMS.
  float alpha;

  //! @brief Painter stroke parameters.
  //!
  //! Used by @c PAINTER_STATE_STROKE_PARAMS.
  Static<PathStrokeParams> strokeParams;

  //! @brief Painter solid color.
  //!
  //! Used by @c PAINTER_STATE_PAINT_PARAMS if source type is @c PAINTER_SOURCE_ARGB.
  RasterSolid solid;

  //! @brief Painter pattern.
  //!
  //! Used by @c PAINTER_STATE_PAINT_PARAMS if source type is @c PAINTER_SOURCE_PATTERN.
  Static<Pattern> pattern;

  //! @brief Painter pattern context.
  //!
  //! Used by @c PAINTER_STATE_SOURCE if source type is @c PAINTER_SOURCE_PATTERN.
  RasterPattern* pctx;

  //! @brief Painter user matrix.
  //!
  //! Used by @c PAINTER_STATE_MATRIX.
  Static<TransformD> userTransform;

  //! @brief Painter work origin (meta + user origin).
  //!
  //! Used by @c PAINTER_STATE_CLIP_AREA.
  PointI finalOrigin;

  //! @brief Painter work region (combined meta region and user region).
  //!
  //! Used by @c PAINTER_STATE_CLIP_AREA.
  Static<Region> finalRegion;

  //! @brief Clip box (work region extents).
  //!
  //! Used by @c PAINTER_STATE_CLIP_AREA.
  BoxI finalClipBox;

  //! @brief Clip mask.
  //!
  //! Used by @c PAINTER_STATE_CLIP_AREA.
  RasterClipMask* mask;

  //! @brief Clip mask save counter.
  uint32_t maskSavedCounter;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GRAPHICS_PAINTENGINE_RASTERPAINTSTATE_P_H
