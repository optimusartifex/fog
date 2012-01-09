// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_FACE_FACE3DNOW_H
#define _FOG_CORE_FACE_FACE3DNOW_H

// [Dependencies]
#include <Fog/Core/C++/Intrin3dNow.h>
#include <Fog/Core/Face/FaceMMX.h>
#include <Fog/Core/Math/Constants.h>
#include <Fog/Core/Math/Math.h>

// [Warnings]
#if defined(FOG_CC_MSC)
# pragma warning(push)
# pragma warning(disable:4799) // function '...' has no EMMS instruction.
#endif // FOG_CC_MSC

// ============================================================================
// [Fog::Face - 3dNow - Constants]
// ============================================================================

FOG_MMX_DECLARE_CONST_PI32_SET(m64f_sgn_mask, 0x80000000);
FOG_MMX_DECLARE_CONST_PI32_SET(m64f_num_mask, 0x7FFFFFFF);
FOG_MMX_DECLARE_CONST_PF_SET  (m64f_one     , 1.0f      );
FOG_MMX_DECLARE_CONST_PF_SET  (m64f_epsilon , Fog::MATH_EPSILON_F);

namespace Fog {
namespace Face {

//! @addtogroup Fog_Face_3dNow
//! @{

// ============================================================================
// [Fog::Face - 3dNow - Load/Store]
// ============================================================================

template<typename T>
static FOG_INLINE void m64fLoad4(__m64& dst0, const T* srcp)
{
  dst0 = _mm_cvtsi32_si64(reinterpret_cast<const int*>(srcp)[0]);
}

template<typename T>
static FOG_INLINE void m64fLoad8(__m64& dst0, const T* srcp)
{
  dst0 = reinterpret_cast<const __m64*>(srcp)[0];
}

template<typename T0, typename T1>
static FOG_INLINE void m64fLoad8(__m64& dst0, const T0* srcLo, const T1* srcHi)
{
  dst0 = _mm_cvtsi32_si64(reinterpret_cast<const int*>(srcLo)[0]);
  dst0 = _mm_unpacklo_pi32(dst0, _mm_cvtsi32_si64(reinterpret_cast<const int*>(srcHi)[0]));
}

template<typename T>
static FOG_INLINE void m64fStore4(T* dstp, const __m64& src0)
{
  reinterpret_cast<int*>(dstp)[0] = _mm_cvtsi64_si32(src0);
}

template<typename T>
static FOG_INLINE void m64fStore8(T* dstp, const __m64& src0)
{
  reinterpret_cast<__m64*>(dstp)[0] = src0;
}

// ============================================================================
// [Fog::Face - 3dNow - Convert]
// ============================================================================

static FOG_INLINE void m64fFromM64I32(__m64& dst, const __m64& a)
{
  dst = _m_pi2fd(a);
}

static FOG_INLINE void m64fToM64I32(__m64& dst, const __m64& a)
{
  dst = _m_pf2id(a);
}

// ============================================================================
// [Fog::Face - 3dNow - Shuffle]
// ============================================================================

static FOG_INLINE void m64fSwapXY(__m64& dst, const __m64& a)
{
  dst = _mm_shuffle_pi16(a, _MM_SHUFFLE(1, 0, 3, 2));
}

static FOG_INLINE void m64fExtendLo(__m64& dst, const __m64& a)
{
  dst = _mm_unpacklo_pi32(a, a);
}

static FOG_INLINE void m64fExtendHi(__m64& dst, const __m64& a)
{
  dst = _mm_unpackhi_pi32(a, a);
}

// ============================================================================
// [Fog::Face - 3dNow - Add]
// ============================================================================

static FOG_INLINE void m64fAdd(__m64& dst, const __m64& a, const __m64& b)
{
  dst = _m_pfadd(a, b);
}

//! @brief Horizontal Addition (3dNow Accumulation).
//!
//! @verbatim
//! dst[ 0-31] = a[0-31] + a[32-63]
//! dst[32-63] = b[0-31] + b[32-63]
//! @endverbatim
static FOG_INLINE void m64fHAdd(__m64& dst, const __m64& a, const __m64& b)
{
  dst = _m_pfacc(a, b);
}

// ============================================================================
// [Fog::Face - 3dNow - Sub]
// ============================================================================

static FOG_INLINE void m64fSub(__m64& dst, const __m64& a, const __m64& b)
{
  dst = _m_pfsub(a, b);
}

static FOG_INLINE void m64fSubRev(__m64& dst, const __m64& a, const __m64& b)
{
  dst = _m_pfsubr(a, b);
}

// ============================================================================
// [Fog::Face - 3dNow - Mul]
// ============================================================================

static FOG_INLINE void m64fMul(__m64& dst, const __m64& a, const __m64& b)
{
  dst = _m_pfmul(a, b);
}

// ============================================================================
// [Fog::Face - 3dNow - Min/Max]
// ============================================================================

static FOG_INLINE void m64fMin(__m64& dst, const __m64& a, const __m64& b)
{
  dst = _m_pfmin(a, b);
}

static FOG_INLINE void m64fMax(__m64& dst, const __m64& a, const __m64& b)
{
  dst = _m_pfmax(a, b);
}

// ============================================================================
// [Fog::Face - 3dNow - Reciprocal]
// ============================================================================

//! @brief Performs reciprocal approximation (high precision).
//!
//! @verbatim
//! dst[ 0-31] = rcp(a[0-31])
//! dst[32-63] = rcp(a[0-31])
//! @endverbatim
static FOG_INLINE void m64fRcpLo(__m64& dst, const __m64& a)
{
  dst = _mm_unpacklo_pi32(a, a);
  __m64 t0 = _m_pfrcp(dst);
  dst = _m_pfrcpit1(dst, t0);
  dst = _m_pfrcpit2(dst, t0);
}

//! @brief Performs reciprocal approximation (low precision).
static FOG_INLINE void m64fRcpLoStep0(__m64& dst, const __m64& a)
{
  dst = _m_pfrcp(a);
}

//! @brief Performs reciprocal approximation (step 1).
static FOG_INLINE void m64fRcpLoStep1(__m64& dst, const __m64& a, const __m64& b)
{
  dst = _m_pfrcpit1(a, b);
}

//! @brief Performs reciprocal approximation (step 2).
static FOG_INLINE void m64fRcpLoStep2(__m64& dst, const __m64& a, const __m64& b)
{
  dst = _m_pfrcpit2(a, b);
}

// ============================================================================
// [Fog::Face - 3dNow - Square-Root Reciprocal]
// ============================================================================

//! @brief Performs square root reciprocal approximation (high precision).
//!
//! dst[ 0-31] = sqrt_rcp(a[0-31])
//! dst[32-63] = sqrt_rcp(a[0-31])
static FOG_INLINE void m64fSqrtRcpLo(__m64& dst, const __m64& a)
{
  dst = _mm_unpacklo_pi32(a, a);
  __m64 t0 = _m_pfrsqrt(dst);
  dst = _m_pfrsqit1(dst, t0);
  dst = _m_pfrcpit2(dst, t0);
}

//! @brief Performs square root reciprocal approximation (low precision).
static FOG_INLINE void m64fSqrtRcpLoStep0(__m64& dst, const __m64& a)
{
  dst = _m_pfrsqrt(a);
}

//! @brief Performs square root reciprocal approximation (step 1).
static FOG_INLINE void m64fSqrtRcpLoStep1(__m64& dst, const __m64& a, const __m64& b)
{
  dst = _m_pfrsqit1(a, b);
}

//! @brief Performs square root reciprocal approximation (step 2).
static FOG_INLINE void m64fSqrtRcpLoStep2(__m64& dst, const __m64& a, const __m64& b)
{
  dst = _m_pfrcpit2(a, b);
}

// ============================================================================
// [Fog::Face - 3dNow - Div]
// ============================================================================

//! @brief Divide a[0, 1] by b[0, 0]
//!
//! @verbatim
//! dst[ 0-31] = a[ 0-31] / b[0-31]
//! dst[32-63] = a[32-63] / b[0-31]
//! @endverbatim
static FOG_INLINE void m64fDivByLo(__m64& dst, const __m64& a, const __m64& b)
{
  __m64 t0;
  __m64 w0;

  t0 = _m_pfrcp(b);
  w0 = _mm_unpacklo_pi32(b, b);
  w0 = _m_pfrcpit1(w0, t0);
  w0 = _m_pfrcpit2(w0, t0);
  dst = _m_pfmul(a, w0);
}

// ============================================================================
// [Fog::Face - 3dNow - Compare]
// ============================================================================

static FOG_INLINE void m64fCmpEQ(__m64& dst, const __m64& a, const __m64& b)
{
  dst = _m_pfcmpeq(a, b);
}

static FOG_INLINE void m64fCmpGE(__m64& dst, const __m64& a, const __m64& b)
{
  dst = _m_pfcmpge(a, b);
}

static FOG_INLINE void m64fCmpGT(__m64& dst, const __m64& a, const __m64& b)
{
  dst = _m_pfcmpgt(a, b);
}

static FOG_INLINE void m64fCmpLE(__m64& dst, const __m64& a, const __m64& b)
{
  dst = _m_pfcmpge(b, a);
}

static FOG_INLINE void m64fCmpLT(__m64& dst, const __m64& a, const __m64& b)
{
  dst = _m_pfcmpgt(b, a);
}

// ============================================================================
// [Fog::Face - 3dNow - EMMS]
// ============================================================================

static FOG_INLINE void m64fEMMS()
{
  _m_femms();
}

//! @}

} // Face namespace
} // Fog namespace

// [Warnings]
#if defined(FOG_CC_MSC)
# pragma warning(pop)
#endif // FOG_CC_MSC

// [Guard]
#endif // _FOG_CORE_FACE_FACE3DNOW_H
