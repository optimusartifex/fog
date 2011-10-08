// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_FACE_FACEMMXSSSE3_H
#define _FOG_CORE_FACE_FACEMMXSSSE3_H

// [Dependencies]
#include <Fog/Core/C++/IntrinMMX.h>
#include <Fog/Core/C++/IntrinSSSE3.h>
#include <Fog/Core/Face/Features.h>

namespace Fog {
namespace Face {

// [Warnings]
#if defined(FOG_CC_MSC)
# pragma warning(push)
# pragma warning(disable:4799) // function '...' has no EMMS instruction.
#endif // FOG_CC_MSC

//! @addtogroup Fog_Face_MMX
//! @addtogroup Fog_Face_SSSE3
//! @{

// ============================================================================
// [Fog::Face - MMX_SSSE3 - Alignr]
// ============================================================================

template<int BYTES>
static FOG_INLINE void iAlignrPI8(
  i& dst0, const i& x0, const i& y0)
{
  dst0 = _mm_alignr_pi8(x0, y0, BYTES);
}

// ============================================================================
// [Fog::Face - MMX_SSSE3 - Shuffle]
// ============================================================================

static FOG_INLINE void iShufflePI8(
  i& dst0, const i& x0, const i& y0)
{
  dst0 = _mm_shuffle_pi8(x0, y0);
}

// ============================================================================
// [Fog::Face - MMX_SSSE3 - HAdd]
// ============================================================================

static FOG_INLINE void iHAddPI16(
  i& dst0, const i& x0, const i& y0)
{
  dst0 = _mm_hadd_pi16(x0, y0);
}

static FOG_INLINE void iHAddPI32(
  i& dst0, const i& x0, const i& y0)
{
  dst0 = _mm_hadd_pi32(x0, y0);
}

// ============================================================================
// [Fog::Face - MMX_SSSE3 - HAdd-Saturate]
// ============================================================================

static FOG_INLINE void iHAddsPI16(
  i& dst0, const i& x0, const i& y0)
{
  dst0 = _mm_hadds_pi16(x0, y0);
}

// ============================================================================
// [Fog::Face - MMX_SSSE3 - HSub]
// ============================================================================

static FOG_INLINE void iHSubPI16(
  i& dst0, const i& x0, const i& y0)
{
  dst0 = _mm_hsub_pi16(x0, y0);
}

static FOG_INLINE void iHSubPI32(
  i& dst0, const i& x0, const i& y0)
{
  dst0 = _mm_hsub_pi32(x0, y0);
}

// ============================================================================
// [Fog::Face - MMX_SSSE3 - HSub-Saturate]
// ============================================================================

static FOG_INLINE void iHSubsPI16(
  i& dst0, const i& x0, const i& y0)
{
  dst0 = _mm_hsubs_pi16(x0, y0);
}

// ============================================================================
// [Fog::Face - MMX_SSSE3 - Mul-HRS]
// ============================================================================

//! @verbatim
//! dst0.w[0..3] := (int16_t)( ((x0[0..3] * y0[0..3]) + 0x4000) >> 15 )
//! @endverbatim
static FOG_INLINE void iMulhrsPI16(
  i& dst0, const i& x0, const i& y0)
{
  dst0 = _mm_mulhrs_pi16(x0, y0);
}

// ============================================================================
// [Fog::Face - MMX_SSSE3 - MAdd-Saturate]
// ============================================================================

static FOG_INLINE void iMAddubsPI16(
  i& dst0, const i& x0, const i& y0)
{
  dst0 = _mm_maddubs_pi16(x0, y0);
}

// ============================================================================
// [Fog::Face - MMX_SSSE3 - Abs]
// ============================================================================

static FOG_INLINE void iAbsPI8(
  i& dst0, const i& x0)
{
  dst0 = _mm_abs_pi8(x0);
}

static FOG_INLINE void iAbsPI16(
  i& dst0, const i& x0)
{
  dst0 = _mm_abs_pi16(x0);
}

static FOG_INLINE void iAbsPI32(
  i& dst0, const i& x0)
{
  dst0 = _mm_abs_pi32(x0);
}

// ============================================================================
// [Fog::Face - MMX_SSSE3 - Sign]
// ============================================================================

static FOG_INLINE void iSignPI8(
  i& dst0, const i& x0, const i& y0)
{
  dst0 = _mm_sign_pi8(x0, y0);
}

static FOG_INLINE void iSignPI16(
  i& dst0, const i& x0, const i& y0)
{
  dst0 = _mm_sign_pi16(x0, y0);
}

static FOG_INLINE void iSignPI32(
  i& dst0, const i& x0, const i& y0)
{
  dst0 = _mm_sign_pi32(x0, y0);
}

//! @}

// [Warnings]
#if defined(FOG_CC_MSC)
# pragma warning(pop)
#endif // FOG_CC_MSC

// [Guard]
#endif // _FOG_CORE_FACE_FACEMMXSSSE3_H
