// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_FACE_FACE_SSE3_H
#define _FOG_CORE_FACE_FACE_SSE3_H

// [Dependencies]
#include <Fog/Core/Cpu/Intrin_SSE3.h>
#include <Fog/Core/Face/Face_SSE.h>
#include <Fog/Core/Face/Face_SSE2.h>

namespace Fog {
namespace Face {

//! @addtogroup Fog_Face_SSE3
//! @{

// ============================================================================
// [Fog::Face - SSE3 - Add]
// ============================================================================

static FOG_INLINE void m128dHAddPS(m128f& dst, const m128f& a, const m128f& b)
{
  dst = _mm_hadd_ps(a, b);
}

static FOG_INLINE void m128dHAddPD(m128d& dst, const m128d& a, const m128d& b)
{
  dst = _mm_hadd_pd(a, b);
}

// ============================================================================
// [Fog::Face - SSE3 - Sub]
// ============================================================================

static FOG_INLINE void m128dHSubPS(m128f& dst, const m128f& a, const m128f& b)
{
  dst = _mm_hsub_ps(a, b);
}

static FOG_INLINE void m128dHSubPD(m128d& dst, const m128d& a, const m128d& b)
{
  dst = _mm_hsub_pd(a, b);
}

// ============================================================================
// [Fog::Face - SSE3 - AddSub]
// ============================================================================

static FOG_INLINE void m128dAddSubPS(m128f& dst, const m128f& a, const m128f& b)
{
  dst = _mm_addsub_ps(a, b);
}

static FOG_INLINE void m128dAddSubPD(m128d& dst, const m128d& a, const m128d& b)
{
  dst = _mm_addsub_pd(a, b);
}

//! @}

} // Face namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_FACE_FACE_SSE3_H
