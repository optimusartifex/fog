// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_CPU_INTRIN_3DNOW_H
#define _FOG_CORE_CPU_INTRIN_3DNOW_H

// [Dependencies]
#include <Fog/Core/Config/Config.h>
#include <Fog/Core/Cpu/Intrin_MMX.h>

#include <mm3dnow.h>

// ============================================================================
// [Fog::Core - MMX-3dNow - Classes]
// ============================================================================

namespace Fog {

//! @addtogroup Fog_Core_Intrin
//! @{

union FOG_ALIGNED_TYPE(mm3dnow_t, 8)
{
  __m64     m64;
  int64_t   sq[1];
  uint64_t  uq[1];
  int32_t   sd[2];
  uint32_t  ud[2];
  int16_t   sw[4];
  uint16_t  uw[4];
  int8_t    sb[8];
  uint8_t   ub[8];
  float     f[2];
};

//! @}

}

// [Guard]
#endif // _FOG_CORE_CPU_INTRIN_3DNOW_H
