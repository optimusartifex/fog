// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_ACC_ACCMMXEXT_H
#define _FOG_CORE_ACC_ACCMMXEXT_H

// [Dependencies]
#include <Fog/Core/C++/IntrinMmxExt.h>
#include <Fog/Core/Acc/AccMmx.h>

namespace Fog {
namespace Acc {

// [Warnings]
#if defined(FOG_CC_MSC)
# pragma warning(push)
# pragma warning(disable:4799) // function '...' has no EMMS instruction.
#endif // FOG_CC_MSC

//! @addtogroup Fog_Core_Acc_MmxExt
//! @{

// ============================================================================
// [Fog::Acc - MmxExt - MoveMask]
// ============================================================================

// @brief Create a 8-bit mask from the most significant bits of the 8 signed
// or unsigned 8-bit integers in a and zero extend the upper bits.
//
// static FOG_INLINE void m128iMoveMask8(int& dst, const __m64& a)
// {
//   dst = _mm_movemask_pi8(a);
// }

//! @}

// [Warnings]
#if defined(FOG_CC_MSC)
# pragma warning(pop)
#endif // FOG_CC_MSC

} // Acc namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_ACC_ACCMMXEXT_H
