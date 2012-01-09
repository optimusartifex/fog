// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_SHADER_SHADER_H
#define _FOG_G2D_SHADER_SHADER_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Transform.h>

namespace Fog {

//! @addtogroup Fog_G2d_Shader
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
#endif // _FOG_G2D_SHADER_SHADER_H
