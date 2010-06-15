// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_RASTERENGINE_C_DEPRECATED_H
#define _FOG_GRAPHICS_RASTERENGINE_C_DEPRECATED_H

// [Dependencies]
#include <Fog/Core/Build.h>
#include <Fog/Graphics/Color.h>
#include <Fog/Graphics/ColorUtil.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Geometry.h>
#include <Fog/Graphics/Image.h>
#include <Fog/Graphics/RasterEngine_p.h>

//! @addtogroup Fog_Graphics_Raster
//! @{

// This file is included from all Fog/Graphics/RasterEngine/RasterEngine_.h 
// include files and .cpp files, so in future it may contain generic code for
// these modules.

namespace Fog {
namespace RasterEngine {

#if 1

// TODO: Remove this header!

// ============================================================================
// [Fog::Raster - Set]
// ============================================================================

// TODO: Ideally move to Fog::Memory...

static FOG_INLINE void set4(uint8_t* dest, uint32_t pattern)
{
  ((uint32_t *)dest)[0] = pattern;
}

#endif

// ============================================================================
// [Fog::Raster - PixFmt_ARGB32]
// ============================================================================

struct FOG_HIDDEN PixFmt_ARGB32
{
  enum { BytesPerPixel = 4 };

  static FOG_INLINE uint32_t fetch(const uint8_t* p)
  {
    return ((const uint32_t *)p)[0];
  }

  static FOG_INLINE void store(uint8_t* p, uint32_t s0)
  {
    ((uint32_t *)p)[0] = s0;
  }
};


// ============================================================================
// [Fog::Raster - PixFmt_A8]
// ============================================================================

struct FOG_HIDDEN PixFmt_A8
{
  enum { BytesPerPixel = 2 };

  static FOG_INLINE uint32_t fetch(const uint8_t* p)
  {
    return p[0];
  }

  static FOG_INLINE void store(uint8_t* p, uint32_t s0)
  {
    p[0] = (uint8_t)s0;
  }
};





















} // RasterEngine namespace
} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_GRAPHICS_RASTERENGINE_C_DEPRECATED_H
