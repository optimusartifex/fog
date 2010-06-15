// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_PAINTENGINE_RASTER_ACTION_P_H
#define _FOG_GRAPHICS_PAINTENGINE_RASTER_ACTION_P_H

// [Dependencies]
#include <Fog/Graphics/PaintEngine/PaintEngine_Raster_Base_p.h>

namespace Fog {

//! @addtogroup Fog_Graphics_Private
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct RasterPaintContext;

// ============================================================================
// [Fog::RasterPaintAction]
// ============================================================================

//! @internal
//!
//! @brief Raster paint action is base class for @c RasterPaintCmd and
//! @c RasterPaintCalc classes. It shares common stuff and dispatching.
struct FOG_HIDDEN RasterPaintAction
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Action constructor.
  FOG_INLINE RasterPaintAction() {}

  //! @brief Action destructor (non-virtual).
  //!
  //! It's not virtual! Use @c release() to destroy resources associated
  //! with the action.
  FOG_INLINE ~RasterPaintAction() {}

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  //! @brief Run action (command or calculation).
  virtual void run(RasterPaintContext* ctx) = 0;
  //! @brief Release action (it means free / dereference all associated
  //! resources with this action).
  virtual void release(RasterPaintContext* ctx) = 0;

private:
  FOG_DISABLE_COPY(RasterPaintAction)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GRAPHICS_PAINTENGINE_RASTER_ACTION_P_H
