// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_ACC_ACCSSE3_H
#define _FOG_CORE_ACC_ACCSSE3_H

// [Dependencies]
#include <Fog/Core/C++/Base.h>
#include <Fog/Core/C++/IntrinSse3.h>

#include <Fog/Core/Acc/AccSse.h>
#include <Fog/Core/Acc/AccSse2.h>

namespace Fog {
namespace Acc {

//! @addtogroup Fog_Core_Acc_Sse3
//! @{

// ============================================================================
// [Fog::Acc - SSE3 - Load]
// ============================================================================

template<typename SrcT>
static FOG_INLINE void m128dLoad8Dup(__m128d& dst0, const SrcT* srcp)
{
  dst0 = _mm_loaddup_pd(reinterpret_cast<const double*>(srcp));
}

template<typename SrcT>
static FOG_INLINE void m128dLoad16dqu(__m128i& dst0, const SrcT* srcp)
{
  dst0 = _mm_lddqu_si128(reinterpret_cast<const __m128i*>(srcp));
}

// ============================================================================
// [Fog::Acc - SSE3 - Move]
// ============================================================================

//! @brief Duplicate the second and fourth 32-bit values.
//! @verbatim
//! dst0[0] = x0[0];
//! dst0[1] = x0[0];
//! dst0[2] = x0[2];
//! dst0[3] = x0[2];
//! @endverbatim
static FOG_INLINE void m128fMoveLDup(
  __m128f& dst0, const __m128f& x0)
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
  __m128f& dst0, const __m128f& x0)
{
  dst0 = _mm_movehdup_ps(x0);
}

//! @brief Duplicate the second and fourth 32-bit values.
//! @verbatim
//! dst0[0] = x0[0];
//! dst0[1] = x0[0];
//! @endverbatim
static FOG_INLINE void m128dMoveLDup(
  __m128d& dst0, const __m128d& x0)
{
  dst0 = _mm_movedup_pd(x0);
}

// ============================================================================
// [Fog::Acc - SSE3 - HAdd]
// ============================================================================

static FOG_INLINE void m128dHAddPS(
  __m128f& dst0, const __m128f& x0, const __m128f& y0)
{
  dst0 = _mm_hadd_ps(x0, y0);
}

static FOG_INLINE void m128dHAddPD(
  __m128d& dst0, const __m128d& x0, const __m128d& y0)
{
  dst0 = _mm_hadd_pd(x0, y0);
}

// ============================================================================
// [Fog::Acc - SSE3 - HSub]
// ============================================================================

static FOG_INLINE void m128dHSubPS(
  __m128f& dst0, const __m128f& x0, const __m128f& y0)
{
  dst0 = _mm_hsub_ps(x0, y0);
}

static FOG_INLINE void m128dHSubPD(
  __m128d& dst0, const __m128d& x0, const __m128d& y0)
{
  dst0 = _mm_hsub_pd(x0, y0);
}

// ============================================================================
// [Fog::Acc - SSE3 - AddSub]
// ============================================================================

static FOG_INLINE void m128dAddSubPS(
  __m128f& dst0, const __m128f& x0, const __m128f& y0)
{
  dst0 = _mm_addsub_ps(x0, y0);
}

static FOG_INLINE void m128dAddSubPD(
  __m128d& dst0, const __m128d& x0, const __m128d& y0)
{
  dst0 = _mm_addsub_pd(x0, y0);
}

//! @}

} // Acc namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_ACC_ACCSSE3_H
