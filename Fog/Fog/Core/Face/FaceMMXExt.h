// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_FACE_FACEMMXEXT_H
#define _FOG_CORE_FACE_FACEMMXEXT_H

// [Dependencies]
#include <Fog/Core/Build.h>
#include <Fog/Core/Face/Features.h>
#include <Fog/Core/Face/FaceMMX.h>
#include <Fog/Core/Intrin/IntrinMMXExt.h>

namespace Fog {
namespace Face {

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

} // Face namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_FACE_FACEMMXEXT_H
