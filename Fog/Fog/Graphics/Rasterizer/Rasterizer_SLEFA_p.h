// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_RASTERIZER_SLEFA_P_H
#define _FOG_GRAPHICS_RASTERIZER_SLEFA_P_H

// [Dependencies]
#include <Fog/Core/Build.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Geometry.h>
#include <Fog/Graphics/Scanline_p.h>
#include <Fog/Graphics/Rasterizer_p.h>

namespace Fog {

//! @addtogroup Fog_Graphics_Private
//! @{

// ============================================================================
// [Fog::SLEFARasterizer]
// ============================================================================

#if 0
//! @internal
//!
//! @brief Scanline-edge flag rasterizer.
struct FOG_HIDDEN SLEFARasterizer : public Rasterizer
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SLEFARasterizer();
  virtual ~SLEFARasterizer();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual void pooled();
  virtual void reset();

  virtual void setClipBox(const IntBox& clipBox);
  virtual void resetClipBox();

  virtual void setError(err_t error);
  virtual void resetError();

  virtual void setFillRule(uint32_t fillRule);
  virtual void setAlpha(uint32_t alpha);

  virtual void addPath(const DoublePath& path);

  virtual void finalize();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

private:
  FOG_DISABLE_COPY(SLEFARasterizer)
};
#endif

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GRAPHICS_RASTERIZER_SLEFA_P_H
