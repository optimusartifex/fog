// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_FILTERS_FEBASE_H
#define _FOG_G2D_IMAGING_FILTERS_FEBASE_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ============================================================================
// [Fog::FeBase]
// ============================================================================

struct FOG_NO_EXPORT FeBase
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getFeType() const { return _feType; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint32_t _feType;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_IMAGING_FILTERS_FEBASE_H
