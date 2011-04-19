// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTER_RASTERSTATE_P_H
#define _FOG_G2D_PAINTING_RASTER_RASTERSTATE_P_H

// [Dependencies]
#include <Fog/G2d/Painting/PainterParams.h>
#include <Fog/G2d/Painting/Raster/RasterConstants_p.h>
#include <Fog/G2d/Painting/Raster/RasterStructs_p.h>
#include <Fog/G2d/Render/RenderStructs_p.h>

namespace Fog {

//! @addtogroup Fog_G2d_Painting_Raster
//! @{

// ============================================================================
// [Fog::RasterState]
// ============================================================================

//! @internal
//!
//! @brief Raster paint engine state.
struct FOG_NO_EXPORT RasterState
{
  // --------------------------------------------------------------------------
  // [Previous]
  // --------------------------------------------------------------------------

  //! @brief The previous state.
  RasterState* prevState;

  //! @brief RasterPainterEngine::masterFlags copy.
  uint32_t prevMasterFlags;

  // --------------------------------------------------------------------------
  // [Id]
  // --------------------------------------------------------------------------

  //! @brief Layer id.
  //!
  //! Used internally to detect if @c PainterEngine::release() is not changing
  //! the state of different layer.
  uint32_t layerId;

  //! @brief State id.
  uint32_t stateId;

  // ------------------------------------------------------------------------
  // [Always Saved / Restored]
  // ------------------------------------------------------------------------

  //! @brief RasterPainterEngine::paintHints copy.
  PaintHints paintHints;
  //! @brief RasterPainterEngine::rasterHints copy.
  RasterHints rasterHints;

  //! @brief The original opacity (float).
  float opacityF;

  // ------------------------------------------------------------------------
  // [Type and Precision]
  // ------------------------------------------------------------------------

  //! @brief Source type (see @ref PATTERN_TYPE).
  uint8_t sourceType;
  //! @brief RasterPainterEngine::masterSave copy.
  uint8_t savedStateFlags;
  //! @brief The strokeParams[F|D] precision.
  uint8_t strokeParamsPrecision;
  //! @brief The type of core clip (coreRegion, coreClipBox).
  //!
  //! @note Don't miss it with the @c finalType member!
  uint8_t coreClipType;

  // ------------------------------------------------------------------------
  // [RASTER_STATE_SOURCE]
  // ------------------------------------------------------------------------

  RasterSource source;
  RenderPatternContext* pc;

  // ------------------------------------------------------------------------
  // [RASTER_STATE_STROKE]
  // ------------------------------------------------------------------------

  struct StrokeParamsData
  {
    //! @brief Stroke parameters (float).
    Static<PathStrokerParamsF> f;
    //! @brief Stroke parameters (double).
    Static<PathStrokerParamsD> d;
  } strokeParams;

  // --------------------------------------------------------------------------
  // [RASTER_STATE_TRANSFORM]
  // --------------------------------------------------------------------------

  //! @brief The user transformation matrix (double).
  Static<TransformD> userTransform;
  //! @brief The final transformation matrix (double).
  Static<TransformD> finalTransform;
  //! @brief The final transformation matrix (float).
  Static<TransformF> finalTransformF;

  //! @brief The core translation point in pixels (negated coreOrigin).
  PointI coreTranslationI;

  struct
  {
    //! @brief The finalTransformI type, see @c RASTER_INTEGRAL_TRANSFORM.
    int _type;

    int _sx, _sy;
    int _tx, _ty;
  } finalTransformI;

  // ------------------------------------------------------------------------
  // [RASTER_STATE_CLIPPING]
  // ------------------------------------------------------------------------

  //! @brief The core clip-box.
  BoxI coreClipBox;

  //! @brief The core origin (the meta origin + the user origin).
  PointI coreOrigin;

  //! @brief The core region (the meta region & the user region).
  Static<Region> coreRegion;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTER_RASTERSTATE_P_H
