// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_FACE_FACE_MMXEXT_H
#define _FOG_CORE_FACE_FACE_MMXEXT_H

// [Dependencies]
#include <Fog/Core/Cpu/Intrin_MMXExt.h>
#include <Fog/Core/Face/Features.h>
#include <Fog/Core/Face/Face_MMX.h>

namespace Fog {
namespace Face {

// [Warnings]
#if defined(FOG_CC_MSC)
#pragma warning(push)
#pragma warning(disable:4799) // function '...' has no EMMS instruction.
#endif // FOG_CC_MSC

//! @addtogroup Fog_Face_MMXExt
//! @{

// ============================================================================
// [Fog::Face - MMXExt - MoveMask]
// ============================================================================

// @brief Create a 8-bit mask from the most significant bits of the 8 signed 
// or unsigned 8-bit integers in a and zero extend the upper bits.
// static FOG_INLINE void m128iMoveMask8(int& dst, const m64i& a)
// {
//   dst = _mm_movemask_pi8(a);
// }

//! @}

// [Warnings]
#if defined(FOG_CC_MSC)
#pragma warning(pop)
#endif // FOG_CC_MSC

} // Face namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_FACE_FACE_MMXEXT_H
