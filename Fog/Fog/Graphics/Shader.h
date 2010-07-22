// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_SHADER_H
#define _FOG_GRAPHICS_SHADER_H

// [Dependencies]
#include <Fog/Core/Assert.h>
#include <Fog/Core/Atomic.h>
#include <Fog/Core/Static.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Geometry.h>

namespace Fog {

//! @addtogroup Fog_Graphics_Effects
//! @{

// ============================================================================
// [Fog::Shader]
// ============================================================================

struct FOG_API Shader
{
  Shader();
  ~Shader();
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GRAPHICS_SHADER_H
