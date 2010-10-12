// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_FACE_FACESSE2_H
#define _FOG_CORE_FACE_FACESSE2_H

// [Dependencies]
#include <Fog/Core/Build.h>
#include <Fog/Core/Face/Features.h>
#include <Fog/Core/Math.h>

#include <Fog/Core/Intrin/IntrinSSE2.h>

// ============================================================================
// [Fog::Face - SSE2 - Constants]
// ============================================================================

FOG_SSE_DECLARE_CONST_PI64_SET(m128d_sgn_mask, FOG_UINT64_C(0x8000000000000000));
FOG_SSE_DECLARE_CONST_PI64_SET(m128d_num_mask, FOG_UINT64_C(0x7FFFFFFFFFFFFFFF));
FOG_SSE_DECLARE_CONST_PD_SET  (m128d_one     , 1.0                             );
FOG_SSE_DECLARE_CONST_PD_SET  (m128d_epsilon , Fog::MATH_EPSILON_D             );

namespace Fog {
namespace Face {

//! @addtogroup Fog_Face_SSE2
//! @{

// ============================================================================
// [Fog::Face - SSE2 - Types]
// ============================================================================

typedef __m128d m128d;
typedef __m128i m128i;

// ============================================================================
// [Fog::Face - SSE2 - Load / Store]
// ============================================================================

template<typename SrcT>
static FOG_INLINE void m128iLoad1(m128i& dst0, const SrcT* srcp)
{
  dst0 = _mm_cvtsi32_si128(reinterpret_cast<const uint8_t*>(srcp)[0]);
}

template<typename SrcT>
static FOG_INLINE void m128iLoad2(m128i& dst0, const SrcT* srcp)
{
  dst0 = _mm_cvtsi32_si128(reinterpret_cast<const uint16_t*>(srcp)[0]);
}

template<typename SrcT>
static FOG_INLINE void m128iLoad4(m128i& dst0, const SrcT* srcp)
{
  dst0 = _mm_cvtsi32_si128(reinterpret_cast<const int*>(srcp)[0]);
}

template<typename SrcT>
static FOG_INLINE void m128iLoad8(m128i& dst0, const SrcT* srcp)
{
  dst0 = _mm_loadl_epi64(reinterpret_cast<const m128i*>(srcp));
}

template<typename SrcT>
static FOG_INLINE void m128dLoad8(m128d& dst0, const SrcT* srcp)
{
  dst0 = _mm_load_sd(reinterpret_cast<const double*>(srcp));
}

template<typename SrcT>
static FOG_INLINE void m128iLoad16a(m128i& dst0, const SrcT* srcp)
{
  dst0 = _mm_load_si128(reinterpret_cast<const m128i*>(srcp));
}

template<typename SrcT>
static FOG_INLINE void m128dLoad16a(m128d& dst0, const SrcT* srcp)
{
  dst0 = _mm_load_pd(reinterpret_cast<const double*>(srcp));
}

template<typename SrcT>
static FOG_INLINE void m128iLoad16u(m128i& dst0, const SrcT* srcp)
{
  // Load unaligned:
  //   dst0 = _mm_loadu_si128(reinterpret_cast<const m128i*>(srcp));
  //
  // Following code may be faster:
  //   dst0 = _mm_loadl_epi64((m128i*)(srcp));
  //   dst0 = reinterpret_cast<m128i>(_mm_loadh_pi(reinterpret_cast<__m128>(dst0), ((__m64*)srcp) + 1));

  dst0 = _mm_loadu_si128(reinterpret_cast<const m128i*>(srcp));
}

template<typename SrcT>
static FOG_INLINE void m128dLoad16u(m128d& dst0, const SrcT* srcp)
{
  dst0 = _mm_loadu_pd(reinterpret_cast<const double*>(srcp));
}

template<typename DstT>
static FOG_INLINE void m128iStore4(DstT* dstp, const m128i& src0)
{
  reinterpret_cast<int*>(dstp)[0] = _mm_cvtsi128_si32(src0);
}

template<typename DstT>
static FOG_INLINE void m128iStore4nt(DstT* dstp, uint32_t src0)
{
  _mm_stream_si32(reinterpret_cast<int*>(dstp), (int)src0);
}

template<typename DstT>
static FOG_INLINE void m128iStore8(DstT* dstp, const m128i& src0)
{
  _mm_storel_epi64(reinterpret_cast<m128i*>(dstp), src0);
}

template<typename DstT>
static FOG_INLINE void m128dStore8(DstT* dstp, const m128d& src0)
{
  _mm_store_sd(reinterpret_cast<double*>(dstp), src0);
}

template<typename DstT>
static FOG_INLINE void m128iStore16a(DstT* dstp, const m128i& src0)
{
  _mm_store_si128(reinterpret_cast<m128i*>(dstp), src0);
}

template<typename DstT>
static FOG_INLINE void m128dStore16a(DstT* dstp, const m128d& src0)
{
  _mm_store_pd(reinterpret_cast<double*>(dstp), src0);
}

template<typename DstT>
static FOG_INLINE void m128iStore16u(DstT* dstp, const m128i& src0)
{
  _mm_storeu_si128(reinterpret_cast<m128i*>(dstp), src0);
}

template<typename DstT>
static FOG_INLINE void m128dStore16u(DstT* dstp, const m128d& src0)
{
  _mm_storeu_pd(reinterpret_cast<double*>(dstp), src0);
}

template<typename DstT>
static FOG_INLINE void m128iStore16nta(DstT* dstp, const m128i& src0)
{
  _mm_stream_si128(reinterpret_cast<m128i*>(dstp), src0);
}

template<typename DstT>
static FOG_INLINE void m128dStore16nta(DstT* dstp, const m128d& src0)
{
  _mm_stream_pd(reinterpret_cast<m128i*>(dstp), src0);
}

// ============================================================================
// [Fog::Face - SSE2 - Shuffle]
// ============================================================================

static FOG_INLINE void m128dExtractLo(m128d& dst, const m128d& a)
{
  dst = _mm_unpacklo_pd(a, a);
}

static FOG_INLINE void m128dExtractLo(m128d& dst, const m128d& a, const m128d& b)
{
  dst = _mm_unpacklo_pd(a, b);
}

static FOG_INLINE void m128dExtractHi(m128d& dst, const m128d& a)
{
  dst = _mm_unpackhi_pd(a, a);
}

static FOG_INLINE void m128dExtractHi(m128d& dst, const m128d& a, const m128d& b)
{
  dst = _mm_unpackhi_pd(a, b);
}

// ============================================================================
// [Fog::Face - SSE2 - Add]
// ============================================================================

static FOG_INLINE void m128dAddSD(m128d& dst, const m128d& a, const m128d& b)
{
  dst = _mm_add_sd(a, b);
}

static FOG_INLINE void m128dAddPD(m128d& dst, const m128d& a, const m128d& b)
{
  dst = _mm_add_pd(a, b);
}

// ============================================================================
// [Fog::Face - SSE2 - Sub]
// ============================================================================

static FOG_INLINE void m128dSubSD(m128d& dst, const m128d& a, const m128d& b)
{
  dst = _mm_sub_sd(a, b);
}

static FOG_INLINE void m128dSubPD(m128d& dst, const m128d& a, const m128d& b)
{
  dst = _mm_sub_pd(a, b);
}

// ============================================================================
// [Fog::Face - SSE2 - Mul]
// ============================================================================

static FOG_INLINE void m128dMulSD(m128d& dst, const m128d& a, const m128d& b)
{
  dst = _mm_mul_sd(a, b);
}

static FOG_INLINE void m128dMulPD(m128d& dst, const m128d& a, const m128d& b)
{
  dst = _mm_mul_pd(a, b);
}

// ============================================================================
// [Fog::Face - SSE2 - Div]
// ============================================================================

static FOG_INLINE void m128dDivSD(m128d& dst, const m128d& a, const m128d& b)
{
  dst = _mm_div_sd(a, b);
}

static FOG_INLINE void m128dDivPD(m128d& dst, const m128d& a, const m128d& b)
{
  dst = _mm_div_pd(a, b);
}

// ============================================================================
// [Fog::Face - SSE2 - Sqrt]
// ============================================================================

//! @brief Calculate square-root of @a a.
//!
//! @verbatim
//! dst[0] = sqrt(a[0])
//! dst[1] = a[1]
//! @endverbatim
static FOG_INLINE void m128dSqrtSD(m128d& dst, const m128d& a)
{
  dst = _mm_sqrt_sd(a, a);
}

//! @brief Calculate square-root of @a b.
//!
//! @verbatim
//! dst[0] = sqrt(b[0])
//! dst[1] = a[1]
//! @endverbatim
static FOG_INLINE void m128dSqrtSD(m128d& dst, const m128d& a, const m128d& b)
{
  dst = _mm_sqrt_sd(a, b);
}

//! @brief Calculate square-root of @a a.
//!
//! @verbatim
//! dst[0] = sqrt(a[0])
//! dst[1] = sqrt(a[1])
//! @endverbatim
static FOG_INLINE void m128dSqrtPD(m128d& dst, const m128d& a)
{
  dst = _mm_sqrt_pd(a);
}

// ============================================================================
// [Fog::Face - SSE2 - Reciprocal]
// ============================================================================

static FOG_INLINE void m128dRcpSD(m128d& dst, const m128d& a)
{
  dst = _mm_div_sd(_mm_load_sd(&FOG_SSE_GET_CONST_SD(m128d_one)), a);
}

static FOG_INLINE void m128dRcpPD(m128d& dst, const m128d& a)
{
  dst = _mm_div_pd(FOG_SSE_GET_CONST_PD(m128d_one), a);
}

// ============================================================================
// [Fog::Face - SSE2 - Min/Max]
// ============================================================================

static FOG_INLINE void m128dMinSD(m128d& dst, const m128d& a, const m128d& b)
{
  dst = _mm_min_sd(a, b);
}

static FOG_INLINE void m128dMinPD(m128d& dst, const m128d& a, const m128d& b)
{
  dst = _mm_min_pd(a, b);
}

static FOG_INLINE void m128dMaxSD(m128d& dst, const m128d& a, const m128d& b)
{
  dst = _mm_max_sd(a, b);
}

static FOG_INLINE void m128dMaxPD(m128d& dst, const m128d& a, const m128d& b)
{
  dst = _mm_max_pd(a, b);
}

// ============================================================================
// [Fog::Face - SSE2 - BitOps]
// ============================================================================

static FOG_INLINE void m128dAnd(m128d& dst, const m128d& a, const m128d& b)
{
  dst = _mm_and_pd(a, b);
}

static FOG_INLINE void m128iAnd(m128i& dst, const m128i& a, const m128i& b)
{
  dst = _mm_and_si128(a, b);
}

static FOG_INLINE void m128dAndNot(m128d& dst, const m128d& a, const m128d& b)
{
  dst = _mm_andnot_pd(a, b);
}

static FOG_INLINE void m128iAndNot(m128i& dst, const m128i& a, const m128i& b)
{
  dst = _mm_andnot_si128(a, b);
}

static FOG_INLINE void m128dOr(m128d& dst, const m128d& a, const m128d& b)
{
  dst = _mm_or_pd(a, b);
}

static FOG_INLINE void m128iOr(m128i& dst, const m128i& a, const m128i& b)
{
  dst = _mm_or_si128(a, b);
}

static FOG_INLINE void m128dXor(m128d& dst, const m128d& a, const m128d& b)
{
  dst = _mm_xor_pd(a, b);
}

static FOG_INLINE void m128iXor(m128i& dst, const m128i& a, const m128i& b)
{
  dst = _mm_xor_si128(a, b);
}

//! @}

} // Face namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_FACE_FACESSE2_H
