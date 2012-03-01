// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_ACC_ACCSSE_H
#define _FOG_CORE_ACC_ACCSSE_H

// [Dependencies]
#include <Fog/Core/C++/Base.h>
#include <Fog/Core/C++/IntrinSse.h>

#if defined(FOG_HARDCODE_SSE2)
# include <Fog/Core/C++/IntrinSse2.h>
#endif // FOG_HARDCODE_SSE2

#include <Fog/Core/Math/Constants.h>
#include <Fog/Core/Math/Math.h>

// ============================================================================
// [Fog::Acc - SSE - Constants]
// ============================================================================

FOG_XMM_DECLARE_CONST_PI32_VAR(m128f_sn_sn_sn_sn    , 0x80000000, 0x80000000, 0x80000000, 0x80000000);
FOG_XMM_DECLARE_CONST_PI32_VAR(m128f_p0_p0_sn_sn    , 0x00000000, 0x00000000, 0x80000000, 0x80000000);
FOG_XMM_DECLARE_CONST_PI32_VAR(m128f_sn_sn_p0_p0    , 0x80000000, 0x80000000, 0x00000000, 0x00000000);
FOG_XMM_DECLARE_CONST_PI32_VAR(m128f_nm_nm_nm_nm    , 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF);

FOG_XMM_DECLARE_CONST_PS_VAR  (m128f_p0_p0_p0_p1    , 0.0f, 0.0f, 0.0f, 1.0f);

FOG_XMM_DECLARE_CONST_PS_SET  (m128f_p1_p1_p1_p1    , 1.0f);
FOG_XMM_DECLARE_CONST_PS_SET  (m128f_eps_eps_eps_eps, Fog::MATH_EPSILON_F);

FOG_XMM_DECLARE_CONST_PS_SET  (m128f_4x_1_div_255   , float(Fog::MATH_1_DIV_255));
FOG_XMM_DECLARE_CONST_PS_SET  (m128f_4x_1_div_65535 , float(Fog::MATH_1_DIV_65535));

FOG_XMM_DECLARE_CONST_PS_SET  (m128f_4x_255         , 255.0f);
FOG_XMM_DECLARE_CONST_PS_SET  (m128f_4x_65535       , 65535.0f);

namespace Fog {
namespace Acc {

//! @addtogroup Fog_Acc_SSE
//! @{

// ============================================================================
// [Fog::Acc - SSE - Zero]
// ============================================================================

//! @brief Clear content of @a dst0.
//!
//! @verbatim
//! dst[0] = 0.0
//! dst[1] = 0.0
//! dst[2] = 0.0
//! dst[3] = 0.0
//! @endverbatim
static FOG_INLINE void m128fZero(__m128f& dst0)
{
  dst0 = _mm_setzero_ps();
}

// ============================================================================
// [Fog::Acc - SSE - Load/Store]
// ============================================================================

//! @brief Load scalar SP-FP value from @a srcp to @a dst.
//!
//! @verbatim
//! dst[0] = ((float*)srcp)[0]
//! dst[1] = 0.0
//! dst[2] = 0.0
//! dst[3] = 0.0
//! @endverbatim
template<typename SrcT>
static FOG_INLINE void m128fLoad4(__m128f& dst0, const SrcT* srcp)
{
  dst0 = _mm_load_ss(reinterpret_cast<const float*>(srcp));
}

//! @brief Load two SP-FP values from @a srcp to @a dst.
//!
//! @verbatim
//! dst[0] = ((float*)srcp)[0]
//! dst[1] = ((float*)srcp)[0]
//! dst[2] = {NO CHANGE}
//! dst[3] = {NO CHANGE}
//! @endverbatim
template<typename SrcT>
static FOG_INLINE void m128fLoad8Lo(__m128f& dst0, const SrcT* srcp)
{
  dst0 = _mm_loadl_pi(dst0, reinterpret_cast<const __m64*>(srcp));
}

//! @brief Load two SP-FP values from @a srcp to @a dst.
//!
//! @verbatim
//! dst[0] = {NO CHANGE}
//! dst[1] = {NO CHANGE}
//! dst[2] = ((float*)srcp)[0]
//! dst[3] = ((float*)srcp)[0]
//! @endverbatim
template<typename SrcT>
static FOG_INLINE void m128fLoad8Hi(__m128f& dst0, const SrcT* srcp)
{
  dst0 = _mm_loadh_pi(dst0, reinterpret_cast<const __m64*>(srcp));
}

//! @brief Aligned load of four packed SP-FP values.
//!
//! @verbatim
//! dst[0] = ((float*)srcp)[0]
//! dst[1] = ((float*)srcp)[1]
//! dst[2] = ((float*)srcp)[2]
//! dst[3] = ((float*)srcp)[3]
//! @endverbatim
template<typename SrcT>
static FOG_INLINE void m128fLoad16a(__m128f& dst0, const SrcT* srcp)
{
  dst0 = _mm_load_ps(reinterpret_cast<const float*>(srcp));
}

//! @brief Unaligned load of four packed SP-FP values.
//!
//! @verbatim
//! dst[0] = ((float*)srcp)[0]
//! dst[1] = ((float*)srcp)[1]
//! dst[2] = ((float*)srcp)[2]
//! dst[3] = ((float*)srcp)[3]
//! @endverbatim
template<typename SrcT>
static FOG_INLINE void m128fLoad16u(__m128f& dst0, const SrcT* srcp)
{
  dst0 = _mm_loadu_ps(reinterpret_cast<const float*>(srcp));
}

template<typename SrcT>
static FOG_INLINE void m128fLoad16uLoHi(__m128f& dst0, const SrcT* srcp)
{
  dst0 = _mm_loadl_pi(dst0, reinterpret_cast<const __m64*>(srcp) + 0);
  dst0 = _mm_loadh_pi(dst0, reinterpret_cast<const __m64*>(srcp) + 1);
}

//! @brief Store scalar SP-FP value.
//!
//! @verbatim
//! ((float*)dstp)[0] = src0[0]
//! @endverbatim
template<typename DstT>
static FOG_INLINE void m128fStore4(DstT* dstp, const __m128f& src0)
{
  _mm_store_ss(reinterpret_cast<float*>(dstp), src0);
}

//! @brief Store two packed SP-FP values (low).
//!
//! @verbatim
//! ((float*)dstp)[0] = src0[0]
//! ((float*)dstp)[1] = src0[1]
//! @endverbatim
template<typename DstT>
static FOG_INLINE void m128fStore8Lo(DstT* dstp, const __m128f& src0)
{
  _mm_storel_pi(reinterpret_cast<__m64*>(dstp), src0);
}

//! @brief Store two packed SP-FP values (high).
//!
//! @verbatim
//! ((float*)dstp)[0] = src0[2]
//! ((float*)dstp)[1] = src0[3]
//! @endverbatim
template<typename DstT>
static FOG_INLINE void m128fStore8Hi(DstT* dstp, const __m128f& src0)
{
  _mm_storeh_pi(reinterpret_cast<__m64*>(dstp), src0);
}

//! @brief Aligned store of four packed SP-FP values.
//!
//! @verbatim
//! ((float*)dstp)[0] = src0[0]
//! ((float*)dstp)[1] = src0[1]
//! ((float*)dstp)[2] = src0[2]
//! ((float*)dstp)[3] = src0[3]
//! @endverbatim
template<typename DstT>
static FOG_INLINE void m128fStore16a(DstT* dstp, const __m128f& src0)
{
  _mm_store_ps(reinterpret_cast<float*>(dstp), src0);
}

//! @brief Unaligned store of four packed SP-FP values.
//!
//! @verbatim
//! ((float*)dstp)[0] = src0[0]
//! ((float*)dstp)[1] = src0[1]
//! ((float*)dstp)[2] = src0[2]
//! ((float*)dstp)[3] = src0[3]
//! @endverbatim
template<typename DstT>
static FOG_INLINE void m128fStore16u(DstT* dstp, const __m128f& src0)
{
  _mm_storeu_ps(reinterpret_cast<float*>(dstp), src0);
}

template<typename DstT>
static FOG_INLINE void m128fStore16uLoHi(DstT* dstp, const __m128f& src0)
{
  _mm_storel_pi(reinterpret_cast<__m64*>(dstp) + 0, src0);
  _mm_storeh_pi(reinterpret_cast<__m64*>(dstp) + 1, src0);
}

//! @brief Aligned store of four packed SP-FP values (using non-thermal hint).
//!
//! @verbatim
//! ((float*)dstp)[0] = src0[0]
//! ((float*)dstp)[1] = src0[1]
//! ((float*)dstp)[2] = src0[2]
//! ((float*)dstp)[3] = src0[3]
//! @endverbatim
template<typename DstT>
static FOG_INLINE void m128fStore16nta(DstT* dstp, const __m128f& src0)
{
  _mm_stream_ps(reinterpret_cast<float*>(dstp), src0);
}

// ============================================================================
// [Fog::Acc - SSE - Copy]
// ============================================================================

static FOG_INLINE void m128fCopy(__m128f& dst, const __m128f& src)
{
  dst = src;
}

// ============================================================================
// [Fog::Acc - SSE - Shuffle]
// ============================================================================

//! @brief Shuffle SP-FP values from @a to @a dst.
//!
//! @verbatim
//! dst[0] = a[W]
//! dst[1] = a[X]
//! dst[2] = a[Y]
//! dst[3] = a[Z]
//! @endverbatim
template<int Z, int Y, int X, int W>
static FOG_INLINE void m128fShuffle(__m128f& dst, const __m128f& a)
{
#if defined(FOG_HARDCODE_SSE2)
  dst = _mm_shuffle_epi32_f(a, _MM_SHUFFLE(Z, Y, X, W));
#else
  dst = _mm_shuffle_ps(a, a, _MM_SHUFFLE(Z, Y, X, W));
#endif // FOG_HARDCODE_SSE2
}

//! @brief Shuffle SP-FP values from @a and @a b to @a dst.
//!
//! @verbatim
//! dst[0] = a[W]
//! dst[1] = a[X]
//! dst[2] = b[Y]
//! dst[3] = b[Z]
//! @endverbatim
template<int Z, int Y, int X, int W>
static FOG_INLINE void m128fShuffle(__m128f& dst, const __m128f& a, const __m128f& b)
{
  dst = _mm_shuffle_ps(a, b, _MM_SHUFFLE(Z, Y, X, W));
}

//! @brief Swap low/high two SP-FP values.
//!
//! @verbatim
//! dst[0] := a[1]
//! dst[1] := a[0]
//! dst[2] := a[3]
//! dst[3] := a[2]
//! @endverbatim
static FOG_INLINE void m128fSwapXY(__m128f& dst, const __m128f& a)
{
  m128fShuffle<2, 3, 0, 1>(dst, a);
}

//! @brief Extend SP-FP value at @a into all packed ones.
//!
//! @verbatim
//! dst[0] := a[0]
//! dst[1] := a[0]
//! dst[2] := a[0]
//! dst[3] := a[0]
//! @endverbatim
static FOG_INLINE void m128fExtendSS(__m128f& dst, const __m128f& a)
{
  m128fShuffle<0, 0, 0, 0>(dst, a);
}

//! @brief Select and interleave the lower two SP-FP values from @a a and @a b.
//!
//! @verbatim
//! dst[0] = a[0]
//! dst[1] = b[0]
//! dst[2] = a[1]
//! dst[3] = b[1]
//! @endverbatim
static FOG_INLINE void m128fUnpackLo(__m128f& dst, const __m128f& a, const __m128f& b)
{
  dst = _mm_unpacklo_ps(a, b);
}

//! @brief Select and interleave the upper two SP-FP values from @a a and @a b.
//!
//! @verbatim
//! dst[0] = a[2]
//! dst[1] = b[2]
//! dst[2] = a[3]
//! dst[3] = b[3]
//! @endverbatim
static FOG_INLINE void m128fUnpackHi(__m128f& dst, const __m128f& a, const __m128f& b)
{
  dst = _mm_unpackhi_ps(a, b);
}

//! @brief Move low SP-FP values from @a a and @a b to @a dst.
//!
//! @verbatim
//! dst[0] := b[0]
//! dst[1] := b[1]
//! dst[2] := a[0]
//! dst[3] := a[1]
//! @endverbatim
static FOG_INLINE void m128fMoveLH(__m128f& dst, const __m128f& a, const __m128f& b)
{
  dst = _mm_movelh_ps(a, b);
}

//! @brief Move high SP-FP values from @a a and @a b to @a dst.
//!
//! @verbatim
//! dst[0] := b[2]
//! dst[1] := b[3]
//! dst[2] := a[2]
//! dst[3] := a[3]
//! @endverbatim
static FOG_INLINE void m128fMoveHL(__m128f& dst, const __m128f& a, const __m128f& b)
{
  dst = _mm_movehl_ps(a, b);
}

// ============================================================================
// [Fog::Acc - SSE - Add]
// ============================================================================

//! @brief Add scalar SP-FP value of @b to @a and store the result into @a dst.
//!
//! @verbatim
//! dst[0] = a[0] + b[0]
//! dst[1] = a[1]
//! dst[2] = a[2]
//! dst[3] = a[3]
//! @endverbatim
static FOG_INLINE void m128fAddSS(__m128f& dst, const __m128f& a, const __m128f& b)
{
  dst = _mm_add_ss(a, b);
}

//! @brief Add packed SP-FP values of @b to @a and store the result into @a dst.
//!
//! @verbatim
//! dst[0] = a[0] + b[0]
//! dst[1] = a[1] + b[1]
//! dst[2] = a[2] + b[2]
//! dst[3] = a[3] + b[3]
//! @endverbatim
static FOG_INLINE void m128fAddPS(__m128f& dst, const __m128f& a, const __m128f& b)
{
  dst = _mm_add_ps(a, b);
}

// ============================================================================
// [Fog::Acc - SSE - Sub]
// ============================================================================

static FOG_INLINE void m128fSubSS(__m128f& dst, const __m128f& a, const __m128f& b)
{
  dst = _mm_sub_ss(a, b);
}

static FOG_INLINE void m128fSubPS(__m128f& dst, const __m128f& a, const __m128f& b)
{
  dst = _mm_sub_ps(a, b);
}

// ============================================================================
// [Fog::Acc - SSE - Mul]
// ============================================================================

static FOG_INLINE void m128fMulSS(__m128f& dst, const __m128f& a, const __m128f& b)
{
  dst = _mm_mul_ss(a, b);
}

static FOG_INLINE void m128fMulPS(__m128f& dst, const __m128f& a, const __m128f& b)
{
  dst = _mm_mul_ps(a, b);
}

// ============================================================================
// [Fog::Acc - SSE - Div]
// ============================================================================

static FOG_INLINE void m128fDivSS(__m128f& dst, const __m128f& a, const __m128f& b)
{
  dst = _mm_div_ss(a, b);
}

static FOG_INLINE void m128fDivPS(__m128f& dst, const __m128f& a, const __m128f& b)
{
  dst = _mm_div_ps(a, b);
}

// ============================================================================
// [Fog::Acc - SSE - Sqrt]
// ============================================================================

static FOG_INLINE void m128fSqrtSS(__m128f& dst, const __m128f& a)
{
  dst = _mm_sqrt_ss(a);
}

static FOG_INLINE void m128fSqrtPS(__m128f& dst, const __m128f& a)
{
  dst = _mm_sqrt_ps(a);
}

// ============================================================================
// [Fog::Acc - SSE - Reciprocal]
// ============================================================================

static FOG_INLINE void m128fRcpSS(__m128f& dst, const __m128f& a)
{
  dst = _mm_div_ss(_mm_load_ss((const float*)(&FOG_XMM_GET_CONST_PS(m128f_p1_p1_p1_p1))), a);
}

static FOG_INLINE void m128fRcpPS(__m128f& dst, const __m128f& a)
{
  dst = _mm_div_ps(FOG_XMM_GET_CONST_PS(m128f_p1_p1_p1_p1), a);
}

// ============================================================================
// [Fog::Acc - SSE - Sqrt-Reciprocal]
// ============================================================================

static FOG_INLINE void m128fSqrtRcpSS(__m128f& dst, const __m128f& a)
{
  dst = _mm_rsqrt_ss(a);
}

static FOG_INLINE void m128fSqrtRcpPS(__m128f& dst, const __m128f& a)
{
  dst = _mm_rsqrt_ps(a);
}

// ============================================================================
// [Fog::Acc - SSE - Min/Max]
// ============================================================================

static FOG_INLINE void m128fMinSS(__m128f& dst, const __m128f& a, const __m128f& b)
{
  dst = _mm_min_ss(a, b);
}

static FOG_INLINE void m128fMinPS(__m128f& dst, const __m128f& a, const __m128f& b)
{
  dst = _mm_min_ps(a, b);
}

static FOG_INLINE void m128fMaxSS(__m128f& dst, const __m128f& a, const __m128f& b)
{
  dst = _mm_max_ss(a, b);
}

static FOG_INLINE void m128fMaxPS(__m128f& dst, const __m128f& a, const __m128f& b)
{
  dst = _mm_max_ps(a, b);
}

// ============================================================================
// [Fog::Acc - SSE - BitOps]
// ============================================================================

//! @brief Perform a bitwise logical AND between @a and @a b and store result
//! to @a dst.
//!
//! @verbatim
//! dst[0] = a[0] & b[0]
//! dst[1] = a[1] & b[1]
//! dst[2] = a[2] & b[2]
//! dst[3] = a[3] & b[3]
//! @endverbatim
static FOG_INLINE void m128fAnd(__m128f& dst, const __m128f& a, const __m128f& b)
{
  dst = _mm_and_ps(a, b);
}

//! @brief Perform a bitwise logical ANDNOT between @a and @a b and store result
//! to @a dst.
//!
//! @verbatim
//! dst[0] = (~a[0]) & b[0]
//! dst[1] = (~a[1]) & b[1]
//! dst[2] = (~a[2]) & b[2]
//! dst[3] = (~a[3]) & b[3]
//! @endverbatim
static FOG_INLINE void m128fAndNot(__m128f& dst, const __m128f& a, const __m128f& b)
{
  dst = _mm_andnot_ps(a, b);
}

//! @brief Perform a bitwise logical OR between @a and @a b and store result
//! to @a dst.
//!
//! @verbatim
//! dst[0] = a[0] | b[0]
//! dst[1] = a[1] | b[1]
//! dst[2] = a[2] | b[2]
//! dst[3] = a[3] | b[3]
//! @endverbatim
static FOG_INLINE void m128fOr(__m128f& dst, const __m128f& a, const __m128f& b)
{
  dst = _mm_or_ps(a, b);
}

//! @brief Perform a bitwise logical XOR between @a and @a b and store result
//! to @a dst.
//!
//! @verbatim
//! dst[0] = a[0] ^ b[0]
//! dst[1] = a[1] ^ b[1]
//! dst[2] = a[2] ^ b[2]
//! dst[3] = a[3] ^ b[3]
//! @endverbatim
static FOG_INLINE void m128fXor(__m128f& dst, const __m128f& a, const __m128f& b)
{
  dst = _mm_xor_ps(a, b);
}

// ============================================================================
// [Fog::Acc - SSE - Compare]
// ============================================================================

static FOG_INLINE void m128fCmpEqSS(__m128f& dst, __m128f& a, __m128f& b) { dst = _mm_cmpeq_ss(a, b); }
static FOG_INLINE void m128fCmpEqPS(__m128f& dst, __m128f& a, __m128f& b) { dst = _mm_cmpeq_ps(a, b); }

static FOG_INLINE void m128fCmpLtSS(__m128f& dst, __m128f& a, __m128f& b) { dst = _mm_cmplt_ss(a, b); }
static FOG_INLINE void m128fCmpLtPS(__m128f& dst, __m128f& a, __m128f& b) { dst = _mm_cmplt_ps(a, b); }

static FOG_INLINE void m128fCmpLeSS(__m128f& dst, __m128f& a, __m128f& b) { dst = _mm_cmple_ss(a, b); }
static FOG_INLINE void m128fCmpLePS(__m128f& dst, __m128f& a, __m128f& b) { dst = _mm_cmple_ps(a, b); }

static FOG_INLINE void m128fCmpGtSS(__m128f& dst, __m128f& a, __m128f& b) { dst = _mm_cmpgt_ss(a, b); }
static FOG_INLINE void m128fCmpGtPS(__m128f& dst, __m128f& a, __m128f& b) { dst = _mm_cmpgt_ps(a, b); }

static FOG_INLINE void m128fCmpGeSS(__m128f& dst, __m128f& a, __m128f& b) { dst = _mm_cmpge_ss(a, b); }
static FOG_INLINE void m128fCmpGePS(__m128f& dst, __m128f& a, __m128f& b) { dst = _mm_cmpge_ps(a, b);}

static FOG_INLINE void m128fCmpNeqSS(__m128f& dst, __m128f& a, __m128f& b) { dst = _mm_cmpneq_ss(a, b); }
static FOG_INLINE void m128fCmpNeqPS(__m128f& dst, __m128f& a, __m128f& b) { dst = _mm_cmpneq_ps(a, b); }

static FOG_INLINE void m128fCmpNltSS(__m128f& dst, __m128f& a, __m128f& b) { dst = _mm_cmpnlt_ss(a, b); }
static FOG_INLINE void m128fCmpNltPS(__m128f& dst, __m128f& a, __m128f& b) { dst = _mm_cmpnlt_ps(a, b); }

static FOG_INLINE void m128fCmpNleSS(__m128f& dst, __m128f& a, __m128f& b) { dst = _mm_cmpnle_ss(a, b); }
static FOG_INLINE void m128fCmpNlePS(__m128f& dst, __m128f& a, __m128f& b) { dst = _mm_cmpnle_ps(a, b); }

static FOG_INLINE void m128fCmpNgtSS(__m128f& dst, __m128f& a, __m128f& b) { dst = _mm_cmpngt_ss(a, b); }
static FOG_INLINE void m128fCmpNgtPS(__m128f& dst, __m128f& a, __m128f& b) { dst = _mm_cmpngt_ps(a, b); }

static FOG_INLINE void m128fCmpNgeSS(__m128f& dst, __m128f& a, __m128f& b) { dst = _mm_cmpnge_ss(a, b); }
static FOG_INLINE void m128fCmpNgePS(__m128f& dst, __m128f& a, __m128f& b) { dst = _mm_cmpnge_ps(a, b); }

static FOG_INLINE void m128fCmpOrderedSS(__m128f& dst, __m128f& a, __m128f& b) { dst = _mm_cmpord_ss(a, b); }
static FOG_INLINE void m128fCmpOrderedPS(__m128f& dst, __m128f& a, __m128f& b) { dst = _mm_cmpord_ps(a, b); }

static FOG_INLINE void m128fCmpUnorderedSS(__m128f& dst, __m128f& a, __m128f& b) { dst = _mm_cmpunord_ss(a, b); }
static FOG_INLINE void m128fCmpUnorderedPS(__m128f& dst, __m128f& a, __m128f& b) { dst = _mm_cmpunord_ps(a, b); }

// ============================================================================
// [Fog::Acc - SSE - Epsilon]
// ============================================================================

//! @brief Make sure that scalar SP-FP value in a[0] is larger than epsilon
//! (Used to make a reciprocal).
//!
//! @verbatim
//! dst[0] = a[0] < 0 ? max(a[0], -epsilon) : min(a[0], epsilon)
//! dst[1] = a[1]
//! dst[2] = a[2]
//! dst[3] = a[3]
//! @endverbatim
static FOG_INLINE void m128fEpsilonSS(__m128f& dst, const __m128f& a)
{
  __m128f sgn;

  sgn = FOG_XMM_GET_CONST_PS(m128f_sn_sn_sn_sn);
  sgn = _mm_and_ps(sgn, a);

  dst = _mm_and_ps(a, FOG_XMM_GET_CONST_PS(m128f_nm_nm_nm_nm));
  dst = _mm_max_ss(dst, FOG_XMM_GET_CONST_PS(m128f_eps_eps_eps_eps));
  dst = _mm_or_ps(dst, sgn);
}

//! @brief Make sure that packed SP-FP values in a are larger than epsilon
//! (Used to make a reciprocals).
//!
//! @verbatim
//! dst[0] = a[0] < 0 ? max(a[0], -epsilon) : min(a[0], epsilon)
//! dst[1] = a[1] < 0 ? max(a[0], -epsilon) : min(a[0], epsilon)
//! dst[2] = a[2] < 0 ? max(a[0], -epsilon) : min(a[0], epsilon)
//! dst[3] = a[3] < 0 ? max(a[0], -epsilon) : min(a[0], epsilon)
//! @endverbatim
static FOG_INLINE void m128fEpsilonPS(__m128f& dst, const __m128f& a)
{
  __m128f sgn;

  sgn = FOG_XMM_GET_CONST_PS(m128f_sn_sn_sn_sn);
  sgn = _mm_and_ps(sgn, a);

  dst = _mm_and_ps(a, FOG_XMM_GET_CONST_PS(m128f_nm_nm_nm_nm));
  dst = _mm_max_ps(dst, FOG_XMM_GET_CONST_PS(m128f_eps_eps_eps_eps));
  dst = _mm_or_ps(dst, sgn);
}

// ============================================================================
// [Fog::Acc - SSE - MoveMask]
// ============================================================================

//! @brief Create a 4-bit mask from the most significant bits of the four SP-FP values.
//!
//! @verbatim
//! dst = (signbit(a[0]) << 0)
//!     | (signbit(a[1]) << 1)
//!     | (signbit(a[2]) << 2)
//!     | (signbit(a[3]) << 3)
//! @endverbatim
static FOG_INLINE void m128fMoveMask(int& dst, const __m128f& a)
{
  dst = _mm_movemask_ps(a);
}

//! @}

} // Acc namespace
} // Fog namespace

// MMX+SSE2 extensions require both, Face_MMX.h, and Face_SSE.h headers.
#if defined(_FOG_CORE_ACC_ACCMMX_H)
# include <Fog/Core/Acc/AccMmxSse.h>
#endif // _FOG_CORE_ACC_ACCMMX_H

// [Guard]
#endif // _FOG_CORE_ACC_ACCSSE_H
