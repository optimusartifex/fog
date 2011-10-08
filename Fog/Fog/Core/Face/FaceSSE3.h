// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_FACE_FACESSE3_H
#define _FOG_CORE_FACE_FACESSE3_H

// [Dependencies]
#include <Fog/Core/C++/Base.h>
#include <Fog/Core/C++/IntrinSSE3.h>

#include <Fog/Core/Face/FaceSSE.h>
#include <Fog/Core/Face/FaceSSE2.h>

namespace Fog {
namespace Face {

//! @addtogroup Fog_Face_SSE3
//! @{

// ============================================================================
// [Fog::Face - SSE3 - Load]
// ============================================================================

template<typename SrcT>
static FOG_INLINE void m128dLoad8Dup(m128d& dst0, const SrcT* srcp)
{
  dst0 = _mm_loaddup_pd(reinterpret_cast<const double*>(srcp));
}

template<typename SrcT>
static FOG_INLINE void m128dLoad16dqu(m128i& dst0, const SrcT* srcp)
{
  dst0 = _mm_lddqu_si128(reinterpret_cast<const m128i*>(srcp));
}

// ============================================================================
// [Fog::Face - SSE3 - Move]
// ============================================================================

//! @brief Duplicate the second and fourth 32-bit values.
//! @verbatim
//! dst0[0] = x0[0];
//! dst0[1] = x0[0];
//! dst0[2] = x0[2];
//! dst0[3] = x0[2];
//! @endverbatim
static FOG_INLINE void m128fMoveLDup(
  m128f& dst0, const m128f& x0)
{
  dst0 = _mm_moveldup_ps(x0);
}

//! @brief Duplicate the second and fourth 32-bit values.
//! @verbatim
//! dst0[0] = x0[1];
//! dst0[1] = x0[1];
//! dst0[2] = x0[3];
//! dst0[3] = x0[3];
//! @endverbatim
static FOG_INLINE void m128fMoveHDup(
  m128f& dst0, const m128f& x0)
{
  dst0 = _mm_movehdup_ps(x0);
}

//! @brief Duplicate the second and fourth 32-bit values.
//! @verbatim
//! dst0[0] = x0[0];
//! dst0[1] = x0[0];
//! @endverbatim
static FOG_INLINE void m128dMoveLDup(
  m128d& dst0, const m128d& x0)
{
  dst0 = _mm_movedup_pd(x0);
}

// ============================================================================
// [Fog::Face - SSE3 - HAdd]
// ============================================================================

static FOG_INLINE void m128dHAddPS(
  m128f& dst0, const m128f& x0, const m128f& y0)
{
  dst0 = _mm_hadd_ps(x0, y0);
}

static FOG_INLINE void m128dHAddPD(
  m128d& dst0, const m128d& x0, const m128d& y0)
{
  dst0 = _mm_hadd_pd(x0, y0);
}

// ============================================================================
// [Fog::Face - SSE3 - HSub]
// ============================================================================

static FOG_INLINE void m128dHSubPS(
  m128f& dst0, const m128f& x0, const m128f& y0)
{
  dst0 = _mm_hsub_ps(x0, y0);
}

static FOG_INLINE void m128dHSubPD(
  m128d& dst0, const m128d& x0, const m128d& y0)
{
  dst0 = _mm_hsub_pd(x0, y0);
}

// ============================================================================
// [Fog::Face - SSE3 - AddSub]
// ============================================================================

static FOG_INLINE void m128dAddSubPS(
  m128f& dst0, const m128f& x0, const m128f& y0)
{
  dst0 = _mm_addsub_ps(x0, y0);
}

static FOG_INLINE void m128dAddSubPD(
  m128d& dst0, const m128d& x0, const m128d& y0)
{
  dst0 = _mm_addsub_pd(x0, y0);
}

//! @}

} // Face namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_FACE_FACESSE3_H
