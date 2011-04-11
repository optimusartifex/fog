// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTER_RASTERRENDERER_P_H
#define _FOG_G2D_PAINTING_RASTER_RASTERRENDERER_P_H

// [Dependencies]
#include <Fog/G2d/Global/Constants.h>
#include <Fog/G2d/Painting/Raster/RasterConstants_p.h>
#include <Fog/G2d/Painting/Raster/RasterStructs_p.h>

namespace Fog {

//! @addtogroup Fog_G2d_Painting_Raster
//! @{

// ============================================================================
// [Fog::RasterRender - VTable]
// ============================================================================

struct FOG_NO_EXPORT RasterRenderVTable
{
  // --------------------------------------------------------------------------
  // [FillTransformedBox - Types]
  // --------------------------------------------------------------------------

  typedef void (FOG_FASTCALL *FillTransformedBoxI)(RasterContext& ctx, const BoxI& box);
  typedef void (FOG_FASTCALL *FillTransformedBoxF)(RasterContext& ctx, const BoxF& box);
  typedef void (FOG_FASTCALL *FillTransformedBoxD)(RasterContext& ctx, const BoxD& box);

  // --------------------------------------------------------------------------
  // [FillTransformedBox - Funcs]
  // --------------------------------------------------------------------------

  FillTransformedBoxI fillTransformedBoxI;
  FillTransformedBoxF fillTransformedBoxF;
  FillTransformedBoxD fillTransformedBoxD;

  // --------------------------------------------------------------------------
  // [FillRasterizedShape - Types]
  // --------------------------------------------------------------------------

  typedef void (FOG_FASTCALL *FillRasterizedShape)(RasterContext& ctx, void* _rasterizer);

  // --------------------------------------------------------------------------
  // [FillRasterizedShape - Funcs]
  // --------------------------------------------------------------------------

  FillRasterizedShape fillRasterizedShape;

  // --------------------------------------------------------------------------
  // FillTransformedPath - Types]
  // --------------------------------------------------------------------------

  typedef void (FOG_FASTCALL *FillTransformedPathF)(RasterContext& ctx, const PathF& path, uint32_t fillRule);
  typedef void (FOG_FASTCALL *FillTransformedPathD)(RasterContext& ctx, const PathD& path, uint32_t fillRule);

  // --------------------------------------------------------------------------
  // [FillTransformedPath - Funcs]
  // --------------------------------------------------------------------------

  FillTransformedPathF fillTransformedPathF;
  FillTransformedPathD fillTransformedPathD;
};

//! @internal
//!
//! @brief @c RasterPainterEngine VTable instances.
//!
//! For performance reasons there are vtable for these combinations:
//!
//!   1. Threading model - ST(Singlethreaded) and MT(Multithreaded).
//!   2. Precision - BYTE and WORD.
//!   3. Clipping - BOX, REGION and MASK.
extern FOG_NO_EXPORT RasterRenderVTable _G2d_RasterRender_vtable[RASTER_MODE_COUNT][IMAGE_PRECISION_COUNT][RASTER_CLIP_COUNT];

// ============================================================================
// [Fog::RasterRender - Initializer]
// ============================================================================

FOG_NO_EXPORT void _g2d_painter_init_raster_render(void);

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTER_RASTERRENDERER_P_H
