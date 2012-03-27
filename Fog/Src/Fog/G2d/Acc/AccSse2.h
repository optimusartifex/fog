// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_ACC_ACCSSE2_H
#define _FOG_G2D_ACC_ACCSSE2_H

// [Dependencies]
#include <Fog/Core/Acc/AccSse2.h>

namespace Fog {
namespace Acc {

//! @addtogroup Fog_Core_Acc_Sse2
//! @{

// ============================================================================
// [Fog::Acc - SSE2 - UnpackMask]
// ============================================================================

static FOG_INLINE void m128iUnpackMaskPI16(__m128i& dst0, __m128i& dst1, const __m128i& x0)
{
  m128iUnpackPI32FromPI16Lo(dst0, x0, x0);
  m128iShufflePI32<3, 3, 2, 2>(dst1, dst0);
  m128iShufflePI32<1, 1, 0, 0>(dst0, dst0);
}

//! @}

} // Acc namespace
} // Fog namespace

// [Guard]
#endif // _FOG_G2D_ACC_ACCSSE2_H
