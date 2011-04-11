// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_GEOMETRY_PATHEFFECT_H
#define _FOG_G2D_GEOMETRY_PATHEFFECT_H

// [Dependencies]
#include <Fog/G2d/Geometry/Path.h>

namespace Fog {

//! @addtogroup Fog_G2d_Geometry
//! @{

// ============================================================================
// [Fog::PathEffect]
// ============================================================================

struct FOG_API PathEffect
{
  PathEffect();
  virtual ~PathEffect();

  virtual err_t process(PathF& dst, const PathF& src) const = 0;
  virtual err_t process(PathD& dst, const PathD& src) const = 0;

private:
  FOG_DISABLE_COPY(PathEffect)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_GEOMETRY_PATHEFFECT_H
