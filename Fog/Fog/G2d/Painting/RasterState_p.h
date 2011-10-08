// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTERSTATE_P_H
#define _FOG_G2D_PAINTING_RASTERSTATE_P_H

// [Dependencies]
#include <Fog/G2d/Painting/PaintParams.h>
#include <Fog/G2d/Painting/RasterConstants_p.h>
#include <Fog/G2d/Painting/RasterPaintStructs_p.h>
#include <Fog/G2d/Painting/RasterStructs_p.h>
#include <Fog/G2d/Painting/Rasterizer_p.h>

namespace Fog {

//! @addtogroup Fog_G2d_Painting
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

  //! @brief RasterPaintEngine::masterFlags copy.
  uint32_t prevMasterFlags;

  // --------------------------------------------------------------------------
  // [Id]
  // --------------------------------------------------------------------------

  //! @brief Layer id.
  //!
  //! Used internally to detect if @c PaintEngine::release() is not changing
  //! the state of different layer.
  uint32_t layerId;

  //! @brief State id.
  uint32_t stateId;

  // ------------------------------------------------------------------------
  // [Always Saved / Restored]
  // ------------------------------------------------------------------------

  //! @brief RasterPaintEngine::paintHints copy.
  PaintHints paintHints;
  //! @brief RasterPaintEngine::rasterHints copy.
  RasterHints rasterHints;

  //! @brief The original opacity (float).
  float opacityF;

  // ------------------------------------------------------------------------
  // [Type & Precision]
  // ------------------------------------------------------------------------

  //! @brief Source type (see @ref RASTER_SOURCE).
  uint8_t sourceType;
  //! @brief RasterPaintEngine::masterSave copy.
  uint8_t savedStateFlags;
  //! @brief The strokeParams[F|D] precision.
  uint8_t strokerPrecision;

  // ------------------------------------------------------------------------
  // [Integral Transform]
  // ------------------------------------------------------------------------

  //! @brief The integralTransform type, see @c RASTER_INTEGRAL_TRANSFORM.
  uint8_t integralTransformType;

  struct _IntegralTransform
  {
    int _sx, _sy;
    int _tx, _ty;
  } integralTransform;

  // ------------------------------------------------------------------------
  // [Source]
  // ------------------------------------------------------------------------

  RasterSource source;
  RasterSolid solid;
  RasterPattern* pc;

  // ------------------------------------------------------------------------
  // [Stroke]
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
  Static<TransformD> userTransformD;
  //! @brief The final transformation matrix (double).
  Static<TransformD> finalTransformD;
  //! @brief The final transformation matrix (float).
  Static<TransformF> finalTransformF;

  // ------------------------------------------------------------------------
  // [RASTER_STATE_CLIPPING]
  // ------------------------------------------------------------------------

  // TODO: Clipping

  //! @brief The clip-region.
  //Region ctxClipRegion;
  //! @brief The clip-box.
  //BoxI ctxClipBox;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTERSTATE_P_H
