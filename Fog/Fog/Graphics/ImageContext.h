// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_IMAGECONTEXT_H
#define _FOG_GRAPHICS_IMAGECONTEXT_H

// [Dependencies]
#include <Fog/Core/Assert.h>
#include <Fog/Core/Atomic.h>
#include <Fog/Core/Static.h>
#include <Fog/Graphics/Color.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Geometry.h>
#include <Fog/Graphics/Image.h>
#include <Fog/Graphics/ImagePixels.h>

namespace Fog {

//! @addtogroup Fog_Graphics_Imaging
//! @{

// ============================================================================
// [Fog::ImageContext]
// ============================================================================

//! @brief Class that extends @c ImagePixels for basic raster painting methods.
struct FOG_API ImageContext : public ImagePixels
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  ImageContext();
  virtual ~ImageContext();

  // --------------------------------------------------------------------------
  // [Clear / Ready]
  // --------------------------------------------------------------------------

protected:
  virtual void _clear();
  virtual err_t _ready();
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GRAPHICS_IMAGECONTEXT_H
