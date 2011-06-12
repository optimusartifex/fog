// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_FACE_FACE_3DNOW_H
#define _FOG_CORE_FACE_FACE_3DNOW_H

// [Dependencies]
#include <Fog/Core/Cpu/Intrin_3dNow.h>
#include <Fog/Core/Face/Features.h>
#include <Fog/Core/Face/Face_MMX.h>
#include <Fog/Core/Math/Constants.h>
#include <Fog/Core/Math/Math.h>

// ============================================================================
// [Fog::Face - 3dNow - Constants]
// ============================================================================

FOG_MMX_DECLARE_CONST_PI32_SET(m64f_sgn_mask, 0x80000000);
FOG_MMX_DECLARE_CONST_PI32_SET(m64f_num_mask, 0x7FFFFFFF);
FOG_MMX_DECLARE_CONST_PF_SET  (m64f_one     , 1.0f      );
FOG_MMX_DECLARE_CONST_PF_SET  (m64f_epsilon , Fog::MATH_EPSILON_F);

namespace Fog {
namespace Face {

// [Warnings]
#if defined(FOG_CC_MSC)
#pragma warning(push)
#pragma warning(disable:4799) // function '...' has no EMMS instruction.
#endif // FOG_CC_MSC

//! @addtogroup Fog_Face_3dNow
//! @{

// ============================================================================
// [Fog::Face - 3dNow - Load/Store]
// ============================================================================

template<typename T>
static FOG_INLINE void m64fLoad4(m64f& dst0, const T* srcp)
{
  dst0 = _mm_cvtsi32_si64(reinterpret_cast<const int*>(srcp)[0]);
}

template<typename T>
static FOG_INLINE void m64fLoad8(m64f& dst0, const T* srcp)
{
  dst0 = reinterpret_cast<const m64f*>(srcp)[0];
}

template<typename T0, typename T1>
static FOG_INLINE void m64fLoad8(m64f& dst0, const T0* srcLo, const T1* srcHi)
{
  dst0 = _mm_cvtsi32_si64(reinterpret_cast<const int*>(srcLo)[0]);
  dst0 = _mm_unpacklo_pi32(dst0, _mm_cvtsi32_si64(reinterpret_cast<const int*>(srcHi)[0]));
}

template<typename T>
static FOG_INLINE void m64fStore4(T* dstp, const m64f& src0)
{
  reinterpret_cast<int*>(dstp)[0] = _mm_cvtsi64_si32(src0);
}

template<typename T>
static FOG_INLINE void m64fStore8(T* dstp, const m64f& src0)
{
  reinterpret_cast<m64f*>(dstp)[0] = src0;
}

// ============================================================================
// [Fog::Face - 3dNow - Convert]
// ============================================================================

static FOG_INLINE void m64fFromM64I32(m64i& dst, const m64f& a)
{
  dst = _m_pi2fd(a);
}

static FOG_INLINE void m64fToM64I32(m64i& dst, const m64f& a)
{
  dst = _m_pf2id(a);
}

// ============================================================================
// [Fog::Face - 3dNow - Shuffle]
// ============================================================================

static FOG_INLINE void m64fSwapXY(m64f& dst, const m64f& a)
{
  dst = _mm_shuffle_pi16(a, _MM_SHUFFLE(1, 0, 3, 2));
}

static FOG_INLINE void m64fExtendLo(m64f& dst, const m64f& a)
{
  dst = _mm_unpacklo_pi32(a, a);
}

static FOG_INLINE void m64fExtendHi(m64f& dst, const m64f& a)
{
  dst = _mm_unpackhi_pi32(a, a);
}

// ============================================================================
// [Fog::Face - 3dNow - Add]
// ============================================================================

static FOG_INLINE void m64fAdd(m64f& dst, const m64f& a, const m64f& b)
{
  dst = _m_pfadd(a, b);
}

//! @brief Horizontal Addition (3dNow Accumulation).
//!
//! @verbatim
//! dst[ 0-31] = a[0-31] + a[32-63]
//! dst[32-63] = b[0-31] + b[32-63]
//! @endverbatim
static FOG_INLINE void m64fHAdd(m64f& dst, const m64f& a, const m64f& b)
{
  dst = _m_pfacc(a, b);
}

// ============================================================================
// [Fog::Face - 3dNow - Sub]
// ============================================================================

static FOG_INLINE void m64fSub(m64f& dst, const m64f& a, const m64f& b)
{
  dst = _m_pfsub(a, b);
}

static FOG_INLINE void m64fSubRev(m64f& dst, const m64f& a, const m64f& b)
{
  dst = _m_pfsubr(a, b);
}

// ============================================================================
// [Fog::Face - 3dNow - Mul]
// ============================================================================

static FOG_INLINE void m64fMul(m64f& dst, const m64f& a, const m64f& b)
{
  dst = _m_pfmul(a, b);
}

// ============================================================================
// [Fog::Face - 3dNow - Min/Max]
// ============================================================================

static FOG_INLINE void m64fMin(m64f& dst, const m64f& a, const m64f& b)
{
  dst = _m_pfmin(a, b);
}

static FOG_INLINE void m64fMax(m64f& dst, const m64f& a, const m64f& b)
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
static FOG_INLINE void m64fRcpLo(m64f& dst, const m64f& a)
{
  dst = _mm_unpacklo_pi32(a, a);
  m64f t0 = _m_pfrcp(dst);
  dst = _m_pfrcpit1(dst, t0);
  dst = _m_pfrcpit2(dst, t0);
}

//! @brief Performs reciprocal approximation (low precision).
static FOG_INLINE void m64fRcpLoStep0(m64f& dst, const m64f& a)
{
  dst = _m_pfrcp(a);
}

//! @brief Performs reciprocal approximation (step 1).
static FOG_INLINE void m64fRcpLoStep1(m64f& dst, const m64f& a, const m64f& b)
{
  dst = _m_pfrcpit1(a, b);
}

//! @brief Performs reciprocal approximation (step 2).
static FOG_INLINE void m64fRcpLoStep2(m64f& dst, const m64f& a, const m64f& b)
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
static FOG_INLINE void m64fSqrtRcpLo(m64f& dst, const m64f& a)
{
  dst = _mm_unpacklo_pi32(a, a);
  m64f t0 = _m_pfrsqrt(dst);
  dst = _m_pfrsqit1(dst, t0);
  dst = _m_pfrcpit2(dst, t0);
}

//! @brief Performs square root reciprocal approximation (low precision).
static FOG_INLINE void m64fSqrtRcpLoStep0(m64f& dst, const m64f& a)
{
  dst = _m_pfrsqrt(a);
}

//! @brief Performs square root reciprocal approximation (step 1).
static FOG_INLINE void m64fSqrtRcpLoStep1(m64f& dst, const m64f& a, const m64f& b)
{
  dst = _m_pfrsqit1(a, b);
}

//! @brief Performs square root reciprocal approximation (step 2).
static FOG_INLINE void m64fSqrtRcpLoStep2(m64f& dst, const m64f& a, const m64f& b)
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
static FOG_INLINE void m64fDivByLo(m64f& dst, const m64f& a, const m64f& b)
{
  m64f t0;
  m64f w0;

  t0 = _m_pfrcp(b);
  w0 = _mm_unpacklo_pi32(b, b);
  w0 = _m_pfrcpit1(w0, t0);
  w0 = _m_pfrcpit2(w0, t0);
  dst = _m_pfmul(a, w0);
}

// ============================================================================
// [Fog::Face - 3dNow - Compare]
// ============================================================================

static FOG_INLINE void m64fCmpEQ(m64i& dst, const m64f& a, const m64f& b)
{
  dst = _m_pfcmpeq(a, b);
}

static FOG_INLINE void m64fCmpGE(m64i& dst, const m64f& a, const m64f& b)
{
  dst = _m_pfcmpge(a, b);
}

static FOG_INLINE void m64fCmpGT(m64i& dst, const m64f& a, const m64f& b)
{
  dst = _m_pfcmpgt(a, b);
}

static FOG_INLINE void m64fCmpLE(m64i& dst, const m64f& a, const m64f& b)
{
  dst = _m_pfcmpge(b, a);
}

static FOG_INLINE void m64fCmpLT(m64i& dst, const m64f& a, const m64f& b)
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

// [Warnings]
#if defined(FOG_CC_MSC)
#pragma warning(pop)
#endif // FOG_CC_MSC

} // Face namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_FACE_FACE_3DNOW_H
