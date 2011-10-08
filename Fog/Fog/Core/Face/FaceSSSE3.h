// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_FACE_FACESSSE3_H
#define _FOG_CORE_FACE_FACESSSE3_H

// [Dependencies]
#include <Fog/Core/C++/Base.h>
#include <Fog/Core/C++/IntrinSSSE3.h>

#include <Fog/Core/Face/FaceSSE.h>
#include <Fog/Core/Face/FaceSSE2.h>
#include <Fog/Core/Face/FaceSSE3.h>

namespace Fog {
namespace Face {

//! @addtogroup Fog_Face_SSSE3
//! @{

// ============================================================================
// [Fog::Face - SSSE3 - Alignr]
// ============================================================================

template<int BYTES>
static FOG_INLINE void m128iAlignrPI8(
  m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_alignr_epi8(x0, y0, BYTES);
}

// ============================================================================
// [Fog::Face - SSSE3 - Shuffle]
// ============================================================================

static FOG_INLINE void m128iShufflePI8(
  m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_shuffle_epi8(x0, y0);
}

// ============================================================================
// [Fog::Face - SSSE3 - HAdd]
// ============================================================================

static FOG_INLINE void m128iHAddPI16(
  m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_hadd_epi16(x0, y0);
}

static FOG_INLINE void m128iHAddPI32(
  m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_hadd_epi32(x0, y0);
}

// ============================================================================
// [Fog::Face - SSSE3 - HAdd-Saturate]
// ============================================================================

static FOG_INLINE void m128iHAddsPI16(
  m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_hadds_epi16(x0, y0);
}

// ============================================================================
// [Fog::Face - SSSE3 - HSub]
// ============================================================================

static FOG_INLINE void m128iHSubPI16(
  m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_hsub_epi16(x0, y0);
}

static FOG_INLINE void m128iHSubPI32(
  m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_hsub_epi32(x0, y0);
}

// ============================================================================
// [Fog::Face - SSSE3 - HSub-Saturate]
// ============================================================================

static FOG_INLINE void m128iHSubsPI16(
  m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_hsubs_epi16(x0, y0);
}

// ============================================================================
// [Fog::Face - SSSE3 - Mul-HRS]
// ============================================================================

//! @verbatim
//! dst0.w[0..7] := (int16_t)( ((x0[0..7] * y0[0..7]) + 0x4000) >> 15 )
//! @endverbatim
static FOG_INLINE void m128iMulhrsPI16(
  m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_mulhrs_epi16(x0, y0);
}

// ============================================================================
// [Fog::Face - SSSE3 - MAdd-Saturate]
// ============================================================================

static FOG_INLINE void m128iMAddubsPI16(
  m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_maddubs_epi16(x0, y0);
}

// ============================================================================
// [Fog::Face - SSSE3 - Abs]
// ============================================================================

static FOG_INLINE void m128iAbsPI8(
  m128i& dst0, const m128i& x0)
{
  dst0 = _mm_abs_epi8(x0);
}

static FOG_INLINE void m128iAbsPI16(
  m128i& dst0, const m128i& x0)
{
  dst0 = _mm_abs_epi16(x0);
}

static FOG_INLINE void m128iAbsPI32(
  m128i& dst0, const m128i& x0)
{
  dst0 = _mm_abs_epi32(x0);
}

// ============================================================================
// [Fog::Face - SSSE3 - Sign]
// ============================================================================

static FOG_INLINE void m128iSignPI8(
  m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_sign_epi8(x0, y0);
}

static FOG_INLINE void m128iSignPI16(
  m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_sign_epi16(x0, y0);
}

static FOG_INLINE void m128iSignPI32(
  m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_sign_epi32(x0, y0);
}

//! @}

} // Face namespace
} // Fog namespace

// MMX+SSE2 extensions require both, Face_MMX.h, and Face_SSE2.h headers.
#if defined(_FOG_CORE_FACE_FACEMMX_H)
# include <Fog/Core/Face/FaceMMXSSSE3.h>
#endif // _FOG_CORE_FACE_FACEMMX_H

// [Guard]
#endif // _FOG_CORE_FACE_FACESSSE3_H
