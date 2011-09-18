// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_FACE_FACE_SSE2_H
#define _FOG_CORE_FACE_FACE_SSE2_H

// [Dependencies]
#include <Fog/Core/C++/Base.h>
#include <Fog/Core/C++/IntrinSSE2.h>

#include <Fog/Core/Face/Face_SSE.h>
#include <Fog/Core/Face/Features.h>
#include <Fog/Core/Math/Constants.h>
#include <Fog/Core/Math/Math.h>

// ============================================================================
// [Fog::Face - SSE2 - Constants]
// ============================================================================

FOG_XMM_DECLARE_CONST_PI16_VAR(0000000000000000_00000000FF000000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFF00, 0x0000);
FOG_XMM_DECLARE_CONST_PI16_VAR(0000000000000000_00000000FFFFFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF);
FOG_XMM_DECLARE_CONST_PI16_VAR(0000000000000000_00FF000000000000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00FF, 0x0000, 0x0000, 0x0000);
FOG_XMM_DECLARE_CONST_PI16_VAR(0000000000000000_00FF00FF00FF00FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x00FF, 0x00FF, 0x00FF, 0x00FF);
FOG_XMM_DECLARE_CONST_PI16_VAR(0000000000000000_0101010101010101, 0x0000, 0x0000, 0x0000, 0x0000, 0x0101, 0x0101, 0x0101, 0x0101);
FOG_XMM_DECLARE_CONST_PI16_VAR(0000000000000000_FF000000FF000000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFF00, 0x0000, 0xFF00, 0x0000);
FOG_XMM_DECLARE_CONST_PI16_VAR(0000000000000000_FFFFFFFFFFFFFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF);

FOG_XMM_DECLARE_CONST_PI16_VAR(00000000000000FF_00000000000000FF, 0x0000, 0x0000, 0x0000, 0x00FF, 0x0000, 0x0000, 0x0000, 0x00FF);
FOG_XMM_DECLARE_CONST_PI16_VAR(0080008000800080_0080008000800080, 0x0080, 0x0080, 0x0080, 0x0080, 0x0080, 0x0080, 0x0080, 0x0080);
FOG_XMM_DECLARE_CONST_PI16_VAR(000000FF000000FF_000000FF000000FF, 0x0000, 0x00FF, 0x0000, 0x00FF, 0x0000, 0x00FF, 0x0000, 0x00FF);
FOG_XMM_DECLARE_CONST_PI16_VAR(000000FF00FF00FF_000000FF00FF00FF, 0x0000, 0x00FF, 0x00FF, 0x00FF, 0x0000, 0x00FF, 0x00FF, 0x00FF);
FOG_XMM_DECLARE_CONST_PI16_VAR(000000FF00FF00FF_00FF00FF00FF00FF, 0x0000, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF);
FOG_XMM_DECLARE_CONST_PI16_VAR(0001000200020002_0001000200020002, 0x0001, 0x0002, 0x0002, 0x0002, 0x0001, 0x0002, 0x0002, 0x0002);
FOG_XMM_DECLARE_CONST_PI16_VAR(00FF000000000000_0000000000000000, 0x00FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000);
FOG_XMM_DECLARE_CONST_PI16_VAR(00FF000000000000_00FF000000000000, 0x00FF, 0x0000, 0x0000, 0x0000, 0x00FF, 0x0000, 0x0000, 0x0000);
FOG_XMM_DECLARE_CONST_PI16_VAR(00FF00FF00FF00FF_0000000000000000, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x0000, 0x0000, 0x0000, 0x0000);
FOG_XMM_DECLARE_CONST_PI16_VAR(00FF00FF00FF00FF_000000FF00FF00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x0000, 0x00FF, 0x00FF, 0x00FF);
FOG_XMM_DECLARE_CONST_PI16_VAR(00FF00FF00FF00FF_00FF00FF00FF00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF);
FOG_XMM_DECLARE_CONST_PI16_VAR(0101010101010101_0101010101010101, 0x0101, 0x0101, 0x0101, 0x0101, 0x0101, 0x0101, 0x0101, 0x0101);
FOG_XMM_DECLARE_CONST_PI16_VAR(FF000000000000FF_FF000000000000FF, 0xFF00, 0x0000, 0x0000, 0x00FF, 0xFF00, 0x0000, 0x0000, 0x00FF);
FOG_XMM_DECLARE_CONST_PI16_VAR(FF000000FF000000_FF000000FF000000, 0xFF00, 0x0000, 0xFF00, 0x0000, 0xFF00, 0x0000, 0xFF00, 0x0000);
FOG_XMM_DECLARE_CONST_PI16_VAR(FF00FF00FF00FF00_FF00FF00FF00FF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00);
FOG_XMM_DECLARE_CONST_PI16_VAR(FFFFFFFFFFFFFFFF_0000000000000000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000);
FOG_XMM_DECLARE_CONST_PI16_VAR(FFFFFFFFFFFFFFFF_FFFFFFFFFFFFFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF);
FOG_XMM_DECLARE_CONST_PI16_VAR(00FFFFFF00FFFFFF_00FFFFFF00FFFFFF, 0x00FF, 0xFFFF, 0x00FF, 0xFFFF, 0x00FF, 0xFFFF, 0x00FF, 0xFFFF);
FOG_XMM_DECLARE_CONST_PI16_VAR(0000000100000001_0000000100000001, 0x0000, 0x0001, 0x0000, 0x0001, 0x0000, 0x0001, 0x0000, 0x0001);

FOG_XMM_DECLARE_CONST_PI64_SET(m128d_2x_sn         , FOG_UINT64_C(0x8000000000000000));
FOG_XMM_DECLARE_CONST_PI64_SET(m128d_2x_nm         , FOG_UINT64_C(0x7FFFFFFFFFFFFFFF));

FOG_XMM_DECLARE_CONST_PD_SET  (m128d_2x_one        , 1.0);
FOG_XMM_DECLARE_CONST_PD_SET  (m128d_2x_eps        , Fog::MATH_EPSILON_D);

FOG_XMM_DECLARE_CONST_PD_SET  (m128d_2x_1_div_255  , Fog::MATH_1_DIV_255);
FOG_XMM_DECLARE_CONST_PD_SET  (m128d_2x_1_div_65535, Fog::MATH_1_DIV_65535);

FOG_XMM_DECLARE_CONST_PD_SET  (m128d_2x_255        , 255.0);
FOG_XMM_DECLARE_CONST_PD_SET  (m128d_2x_65535      , 65535.0);

FOG_XMM_DECLARE_CONST_PD_VAR  (m128d_p0_p1         , 0.0, 1.0);

// A constant that is subtracted from the U32 value and added back to U16 value
// to simulate unsigned saturation (this instruction is missing).
FOG_XMM_DECLARE_CONST_PI32_SET(m128i_packusdw_u32  , 0x8000);
FOG_XMM_DECLARE_CONST_PI16_SET(m128i_packusdw_u16  , 0x8000);

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
// [Fog::Face - SSE2 - Load]
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
static FOG_INLINE void m128dLoad8CvtFrom2xPS(m128d& dst0, const SrcT* srcp)
{
  dst0 = _mm_load_sd(reinterpret_cast<const double*>(srcp));
  dst0 = _mm_cvtps_pd(_mm_castpd_ps(dst0));
}

template<typename SrcT>
static FOG_INLINE void m128dLoad8Lo(m128d& dst0, const SrcT* srcp)
{
  dst0 = _mm_loadl_pd(dst0, reinterpret_cast<const double*>(srcp));
}

template<typename SrcT>
static FOG_INLINE void m128dLoad8Hi(m128d& dst0, const SrcT* srcp)
{
  dst0 = _mm_loadh_pd(dst0, reinterpret_cast<const double*>(srcp));
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
  dst0 = _mm_loadu_si128(reinterpret_cast<const m128i*>(srcp));
}

template<typename SrcT>
static FOG_INLINE void m128iLoad16uLoHi(m128i& dst0, const SrcT* srcp)
{
  dst0 = _mm_loadl_epi64((m128i*)(srcp));
  dst0 = _mm_castps_si128(_mm_loadh_pi(_mm_castsi128_ps(dst0), ((const __m64*)srcp) + 1));
}

template<typename SrcT>
static FOG_INLINE void m128dLoad16u(m128d& dst0, const SrcT* srcp)
{
  dst0 = _mm_loadu_pd(reinterpret_cast<const double*>(srcp));
}

// ============================================================================
// [Fog::Face - SSE2 - Store]
// ============================================================================

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
static FOG_INLINE void m128dStore8Lo(DstT* dstp, const m128d& src0)
{
  _mm_storel_pd(reinterpret_cast<double*>(dstp), src0);
}

template<typename DstT>
static FOG_INLINE void m128dStore8Hi(DstT* dstp, const m128d& src0)
{
  _mm_storeh_pd(reinterpret_cast<double*>(dstp), src0);
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
static FOG_INLINE void m128iStore16uLoHi(DstT* dstp, const m128i& src0)
{
  m128d srcd = _mm_castsi128_pd(src0);

  _mm_storel_pd(reinterpret_cast<double*>(dstp + 0), srcd);
  _mm_storeh_pd(reinterpret_cast<double*>(dstp + 1), srcd);
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
  _mm_stream_pd(dstp, src0);
}

// ============================================================================
// [Fog::Face - SSE2 - Convert]
// ============================================================================

static FOG_INLINE void m128iCvtSI128FromSI(m128i& dst0, int src0)
{
  dst0 = _mm_cvtsi32_si128(src0);
}

static FOG_INLINE void m128iCvtSIFromSI128(int& dst0, const m128i& src0)
{
  dst0 = _mm_cvtsi128_si32(src0);
}

static FOG_INLINE void m128fCvtPSFromPI32(m128f& dst0, const m128i& src0)
{
  dst0 = _mm_cvtepi32_ps(src0);
}

static FOG_INLINE void m128fCvtPSFromPD(m128f& dst0, const m128d& src0)
{
  dst0 = _mm_cvtpd_ps(src0);
}

static FOG_INLINE void m128dCvtPDFromPI32(m128d& dst0, const m128i& src0)
{
  dst0 = _mm_cvtepi32_pd(src0);
}

static FOG_INLINE void m128dCvtPDFromPS(m128d& dst0, const m128f& src0)
{
  dst0 = _mm_cvtps_pd(src0);
}

static FOG_INLINE void m128iCvtPI32FromPS(m128i& dst0, const m128f& src0)
{
  dst0 = _mm_cvtps_epi32(src0);
}

static FOG_INLINE void m128iCvtPI32FromPD(m128i& dst0, const m128d& src0)
{
  dst0 = _mm_cvtpd_epi32(src0);
}

// ============================================================================
// [Fog::Face - SSE2 - Prefetch]
// ============================================================================

template<typename DstT>
static FOG_INLINE void prefetchT0(const DstT* dstp)
{
  _mm_prefetch(reinterpret_cast<const char*>(dstp), _MM_HINT_T0);
}

template<typename DstT>
static FOG_INLINE void prefetchT1(const DstT* dstp)
{
  _mm_prefetch(reinterpret_cast<const char*>(dstp), _MM_HINT_T1);
}

template<typename DstT>
static FOG_INLINE void prefetchT2(const DstT* dstp)
{
  _mm_prefetch(reinterpret_cast<const char*>(dstp), _MM_HINT_T2);
}

template<typename DstT>
static FOG_INLINE void prefetchNTA(const DstT* dstp)
{
  _mm_prefetch(reinterpret_cast<const char*>(dstp), _MM_HINT_NTA);
}

// ============================================================================
// [Fog::Face - SSE2 - Unpack]
// ============================================================================

static FOG_INLINE void m128iUnpackPI16FromPI8Lo(m128i& dst0, const m128i& src0)
{
  dst0 = _mm_unpacklo_epi8(src0, _mm_setzero_si128());
}

static FOG_INLINE void m128iUnpackPI16FromPI8Hi(m128i& dst0, const m128i& src0)
{
  dst0 = _mm_unpackhi_epi8(src0, _mm_setzero_si128());
}

static FOG_INLINE void m128iUnpackPI16FromPI8Lo(m128i& dst0, const m128i& src0, const m128i& src1)
{
  dst0 = _mm_unpacklo_epi8(src0, src1);
}

static FOG_INLINE void m128iUnpackPI16FromPI8Hi(m128i& dst0, const m128i& src0, const m128i& src1)
{
  dst0 = _mm_unpackhi_epi8(src0, src1);
}

static FOG_INLINE void m128iUnpackPI32FromPI8Lo(m128i& dst0, const m128i& src0)
{
  m128i zero = _mm_setzero_si128();

  dst0 = _mm_unpacklo_epi8(src0, zero);
  dst0 = _mm_unpacklo_epi16(dst0, zero);
}

static FOG_INLINE void m128iUnpackPI32FromPI8Hi(m128i& dst0, const m128i& src0)
{
  m128i zero = _mm_setzero_si128();

  dst0 = _mm_unpackhi_epi8(src0, zero);
  dst0 = _mm_unpacklo_epi16(dst0, zero);
}

static FOG_INLINE void m128iUnpackPI32FromPI16Lo(m128i& dst0, const m128i& src0)
{
  dst0 = _mm_unpacklo_epi16(src0, _mm_setzero_si128());
}

static FOG_INLINE void m128iUnpackPI32FromPI16Hi(m128i& dst0, const m128i& src0)
{
  dst0 = _mm_unpackhi_epi16(src0, _mm_setzero_si128());
}

static FOG_INLINE void m128iUnpackPI32FromPI16Lo(m128i& dst0, const m128i& src0, const m128i& src1)
{
  dst0 = _mm_unpacklo_epi16(src0, src1);
}

static FOG_INLINE void m128iUnpackPI32FromPI16Hi(m128i& dst0, const m128i& src0, const m128i& src1)
{
  dst0 = _mm_unpackhi_epi16(src0, src1);
}

static FOG_INLINE void m128iUnpackPI64FromPI32Lo(m128i& dst0, const m128i& src0)
{
  dst0 = _mm_unpacklo_epi32(src0, _mm_setzero_si128());
}

static FOG_INLINE void m128iUnpackPI64FromPI32Hi(m128i& dst0, const m128i& src0)
{
  dst0 = _mm_unpackhi_epi32(src0, _mm_setzero_si128());
}

static FOG_INLINE void m128iUnpackPI64FromPI32Lo(m128i& dst0, const m128i& src0, const m128i& src1)
{
  dst0 = _mm_unpacklo_epi32(src0, src1);
}

static FOG_INLINE void m128iUnpackSI128FromPI64Lo(m128i& dst0, const m128i& src0, const m128i& src1)
{
  dst0 = _mm_unpacklo_epi64(src0, src1);
}

static FOG_INLINE void m128iUnpackPI64FromPI32Hi(m128i& dst0, const m128i& src0, const m128i& src1)
{
  dst0 = _mm_unpackhi_epi32(src0, src1);
}

static FOG_INLINE void m128iUnpackSI128FromPI64Hi(m128i& dst0, const m128i& src0)
{
  dst0 = _mm_unpackhi_epi64(src0, _mm_setzero_si128());
}

static FOG_INLINE void m128dUnpackLoPD(m128d& dst, const m128d& a, const m128d& b)
{
  dst = _mm_unpacklo_pd(a, b);
}

static FOG_INLINE void m128dUnpackHiPD(m128d& dst, const m128d& a, const m128d& b)
{
  dst = _mm_unpackhi_pd(a, b);
}

// ============================================================================
// [Fog::Face - SSE2 - Pack]
// ============================================================================

static FOG_INLINE void m128iPackPI8FromPI16(m128i& dst0, const m128i& src0)
{
  dst0 = _mm_packs_epi16(src0, src0);
}

static FOG_INLINE void m128iPackPI8FromPI16(m128i& dst0, const m128i& src0, const m128i& src1)
{
  dst0 = _mm_packs_epi16(src0, src1);
}

static FOG_INLINE void m128iPackPU8FromPU16(m128i& dst0, const m128i& src0)
{
  dst0 = _mm_packus_epi16(src0, src0);
}

static FOG_INLINE void m128iPackPU8FromPU16(m128i& dst0, const m128i& src0, const m128i& src1)
{
  dst0 = _mm_packus_epi16(src0, src1);
}

static FOG_INLINE void m128iPackPU8FromPI32(m128i& dst0, const m128i& src0)
{
  dst0 = _mm_packs_epi32(src0, src0);
  dst0 = _mm_packus_epi16(dst0, dst0);
}

static FOG_INLINE void m128iPackPU8FromPI32(m128i& dst0, const m128i& src0, const m128i& src1)
{
  dst0 = _mm_packs_epi32(src0, src1);
  dst0 = _mm_packus_epi16(dst0, dst0);
}

static FOG_INLINE void m128iPackPU8FromPI32(m128i& dst0, const m128i& src0, const m128i& src1, const m128i& src2, const m128i& src3)
{
  m128i tmp0;
  tmp0 = _mm_packs_epi32(src2, src3);
  dst0 = _mm_packs_epi32(src0, src1);
  dst0 = _mm_packus_epi16(dst0, tmp0);
}

static FOG_INLINE void m128iPackPI16FromPI32(m128i& dst0, const m128i& src0)
{
  dst0 = _mm_packs_epi32(src0, src0);
}

static FOG_INLINE void m128iPackPI16FromPI32(m128i& dst0, const m128i& src0, const m128i& src1)
{
  dst0 = _mm_packs_epi32(src0, src1);
}

static FOG_INLINE void m128iPackPU16FromPI32(m128i& dst0, const m128i& src0)
{
  dst0 = _mm_sub_epi32(src0, FOG_XMM_GET_CONST_PI(m128i_packusdw_u32));
  dst0 = _mm_packs_epi32(dst0, dst0);
  dst0 = _mm_add_epi16(dst0, FOG_XMM_GET_CONST_PI(m128i_packusdw_u16));
}

static FOG_INLINE void m128iPackPU16FromPI32(m128i& dst0, const m128i& src0, const m128i& src1)
{
  dst0 = _mm_packs_epi32(
    _mm_sub_epi32(src0, FOG_XMM_GET_CONST_PI(m128i_packusdw_u32)),
    _mm_sub_epi32(src1, FOG_XMM_GET_CONST_PI(m128i_packusdw_u32)));
  dst0 = _mm_add_epi16(dst0, FOG_XMM_GET_CONST_PI(m128i_packusdw_u16));
}

// ============================================================================
// [Fog::Face - P32 - Zero / Fill]
// ============================================================================

static FOG_INLINE void m128iZero(m128i& dst0)
{
  dst0 = _mm_setzero_si128();
}

static FOG_INLINE void m128iFill(m128i& dst0)
{
  _mm_ext_fill_si128(dst0);
}

template<int I>
static FOG_INLINE void m128iZeroPBBi(m128i& dst0, const m128i& x0)
{
  FOG_XMM_DECLARE_CONST_PI8_VAR(Tmp,
    (I!= 0 ? 0xFF : 0x00), (I!= 1 ? 0xFF : 0x00), (I!= 2 ? 0xFF : 0x00), (I!= 3 ? 0xFF : 0x00),
    (I!= 4 ? 0xFF : 0x00), (I!= 5 ? 0xFF : 0x00), (I!= 6 ? 0xFF : 0x00), (I!= 7 ? 0xFF : 0x00),
    (I!= 8 ? 0xFF : 0x00), (I!= 9 ? 0xFF : 0x00), (I!=10 ? 0xFF : 0x00), (I!=11 ? 0xFF : 0x00),
    (I!=12 ? 0xFF : 0x00), (I!=13 ? 0xFF : 0x00), (I!=14 ? 0xFF : 0x00), (I!=15 ? 0xFF : 0x00));

  dst0 = _mm_and_si128(x0, FOG_XMM_GET_CONST_PI(Tmp));
}

template<int I>
static FOG_INLINE void m128iZeroPBWi(m128i& dst0, const m128i& x0)
{
  FOG_XMM_DECLARE_CONST_PI16_VAR(Tmp,
    (I!=0 ? 0xFFFF : 0x0000), (I!=1 ? 0xFFFF : 0x0000), (I!=2 ? 0xFFFF : 0x0000), (I!=3 ? 0xFFFF : 0x0000),
    (I!=4 ? 0xFFFF : 0x0000), (I!=5 ? 0xFFFF : 0x0000), (I!=6 ? 0xFFFF : 0x0000), (I!=7 ? 0xFFFF : 0x0000));

  dst0 = _mm_and_si128(x0, FOG_XMM_GET_CONST_PI(Tmp));
}

template<int I>
static FOG_INLINE void m128iZeroPWWi(m128i& dst0, const m128i& x0)
{
  m128iZeroPBWi<I>(dst0, x0);
}

template<int I>
static FOG_INLINE void m128iZeroPWDi(m128i& dst0, const m128i& x0)
{
  FOG_XMM_DECLARE_CONST_PI32_VAR(Tmp,
    (I!=0 ? 0xFFFFFFFF : 0x00000000), (I!=1 ? 0xFFFFFFFF : 0x00000000),
    (I!=2 ? 0xFFFFFFFF : 0x00000000), (I!=3 ? 0xFFFFFFFF : 0x00000000));

  dst0 = _mm_and_si128(x0, FOG_XMM_GET_CONST_PI(Tmp));
}

template<int I>
static FOG_INLINE void m128iZeroPDDi(m128i& dst0, const m128i& x0)
{
  m128iZeroPWDi<I>(dst0, x0);
}

template<int I>
static FOG_INLINE void m128iFillPBBi(m128i& dst0, const m128i& x0)
{
  FOG_XMM_DECLARE_CONST_PI8_VAR(Tmp,
    (I== 0 ? 0xFF : 0x00), (I== 1 ? 0xFF : 0x00), (I== 2 ? 0xFF : 0x00), (I== 3 ? 0xFF : 0x00),
    (I== 4 ? 0xFF : 0x00), (I== 5 ? 0xFF : 0x00), (I== 6 ? 0xFF : 0x00), (I== 7 ? 0xFF : 0x00),
    (I== 8 ? 0xFF : 0x00), (I== 9 ? 0xFF : 0x00), (I==10 ? 0xFF : 0x00), (I==11 ? 0xFF : 0x00),
    (I==12 ? 0xFF : 0x00), (I==13 ? 0xFF : 0x00), (I==14 ? 0xFF : 0x00), (I==15 ? 0xFF : 0x00));

  dst0 = _mm_or_si128(x0, FOG_XMM_GET_CONST_PI(Tmp));
}

template<int I>
static FOG_INLINE void m128iFillPBWi(m128i& dst0, const m128i& x0)
{
  FOG_XMM_DECLARE_CONST_PI16_VAR(Tmp,
    (I==7 ? 0x00FF : 0x0000), (I==6 ? 0x00FF : 0x0000), (I==5 ? 0x00FF : 0x0000), (I==4 ? 0x00FF : 0x0000),
    (I==3 ? 0x00FF : 0x0000), (I==2 ? 0x00FF : 0x0000), (I==1 ? 0x00FF : 0x0000), (I==0 ? 0x00FF : 0x0000));

  dst0 = _mm_or_si128(x0, FOG_XMM_GET_CONST_PI(Tmp));
}

template<int I>
static FOG_INLINE void m128iFillPWWi(m128i& dst0, const m128i& x0)
{
  FOG_XMM_DECLARE_CONST_PI16_VAR(Tmp,
    (I==0 ? 0xFFFF : 0x0000), (I==1 ? 0xFFFF : 0x0000), (I==2 ? 0xFFFF : 0x0000), (I==3 ? 0xFFFF : 0x0000),
    (I==4 ? 0xFFFF : 0x0000), (I==5 ? 0xFFFF : 0x0000), (I==6 ? 0xFFFF : 0x0000), (I==7 ? 0xFFFF : 0x0000));

  dst0 = _mm_or_si128(x0, FOG_XMM_GET_CONST_PI(Tmp));
}

template<int I>
static FOG_INLINE void m128iFillPWDi(m128i& dst0, const m128i& x0)
{
  FOG_XMM_DECLARE_CONST_PI32_VAR(Tmp,
    (I==0 ? 0x0000FFFF : 0x00000000), (I==1 ? 0x0000FFFF : 0x00000000),
    (I==2 ? 0x0000FFFF : 0x00000000), (I==3 ? 0x0000FFFF : 0x00000000));

  dst0 = _mm_or_si128(x0, FOG_XMM_GET_CONST_PI(Tmp));
}

template<int I>
static FOG_INLINE void m128iFillPDDi(m128i& dst0, const m128i& x0)
{
  FOG_XMM_DECLARE_CONST_PI32_VAR(Tmp,
    (I==0 ? 0xFFFFFFFF : 0x00000000), (I==1 ? 0xFFFFFFFF : 0x00000000),
    (I==2 ? 0xFFFFFFFF : 0x00000000), (I==3 ? 0xFFFFFFFF : 0x00000000));

  dst0 = _mm_or_si128(x0, FOG_XMM_GET_CONST_PI(Tmp));
}

// ============================================================================
// [Fog::Face - SSE2 - Shuffle]
// ============================================================================

template<int W, int X, int Y, int Z>
static FOG_INLINE void m128iShufflePI16(m128i& dst0, const m128i& src0)
{
  dst0 = _mm_shufflehi_epi16(src0, _MM_SHUFFLE(Z, Y, X, W));
  dst0 = _mm_shufflelo_epi16(dst0, _MM_SHUFFLE(Z, Y, X, W));
}

template<int W, int X, int Y, int Z>
static FOG_INLINE void m128iShufflePI16Lo(m128i& dst0, const m128i& src0)
{
  dst0 = _mm_shufflelo_epi16(src0, _MM_SHUFFLE(Z, Y, X, W));
}

template<int W, int X, int Y, int Z>
static FOG_INLINE void m128iShufflePI16Hi(m128i& dst0, const m128i& src0)
{
  dst0 = _mm_shufflehi_epi16(src0, _MM_SHUFFLE(Z, Y, X, W));
}

template<int Z, int Y, int X, int W>
static FOG_INLINE void m128iShufflePI32(m128i& dst0, const m128i& src0)
{
  dst0 = _mm_shuffle_epi32(src0, _MM_SHUFFLE(Z, Y, X, W));
}

// ============================================================================
// [Fog::Face - SSE2 - Swap]
// ============================================================================

static FOG_INLINE void m128iSwapPI16(m128i& dst0, const m128i& src0)
{
  m128iShufflePI16Lo<0, 1, 2, 3>(dst0, src0);
  m128iShufflePI16Hi<0, 1, 2, 3>(dst0, dst0);
}

static FOG_INLINE void m128iSwapPI16Lo(m128i& dst0, const m128i& src0)
{
  m128iShufflePI16Lo<0, 1, 2, 3>(dst0, src0);
}

static FOG_INLINE void m128iSwapPI16Hi(m128i& dst0, const m128i& src0)
{
  m128iShufflePI16Lo<0, 1, 2, 3>(dst0, src0);
}

static FOG_INLINE void m128iSwapPI32(m128i& dst0, const m128i& src0)
{
  m128iShufflePI32<0, 1, 2, 3>(dst0, src0);
}

static FOG_INLINE void m128dSwapPD(m128d& dst0, const m128d& src0)
{
  dst0 = _mm_shuffle_pd(src0, src0, _MM_SHUFFLE2(0, 1));
}

// ============================================================================
// [Fog::Face - SSE2 - Extend]
// ============================================================================

template<typename SrcT>
static FOG_INLINE void m128dExtendLo(m128d& dst, const SrcT* srcp)
{
  dst = _mm_load_sd(reinterpret_cast<const double*>(srcp));
  dst = _mm_unpacklo_pd(dst, dst);
}

static FOG_INLINE void m128dExtendLo(m128d& dst, const m128d& a)
{
  dst = _mm_unpacklo_pd(a, a);
}

static FOG_INLINE void m128dExtendHi(m128d& dst, const m128d& a)
{
  dst = _mm_unpackhi_pd(a, a);
}

// ============================================================================
// [Fog::Face - SSE2 - Add]
// ============================================================================

static FOG_INLINE void m128iAddPI8(m128i& dst, const m128i& a, const m128i& b)
{
  dst = _mm_add_epi8(a, b);
}

static FOG_INLINE void m128iAddPI16(m128i& dst, const m128i& a, const m128i& b)
{
  dst = _mm_add_epi16(a, b);
}

static FOG_INLINE void m128iAddPI32(m128i& dst, const m128i& a, const m128i& b)
{
  dst = _mm_add_epi32(a, b);
}

static FOG_INLINE void m128iAddPI64(m128i& dst, const m128i& a, const m128i& b)
{
  dst = _mm_add_epi64(a, b);
}

static FOG_INLINE void m128dAddSD(m128d& dst, const m128d& a, const m128d& b)
{
  dst = _mm_add_sd(a, b);
}

static FOG_INLINE void m128dAddPD(m128d& dst, const m128d& a, const m128d& b)
{
  dst = _mm_add_pd(a, b);
}

// ============================================================================
// [Fog::Face - SSE2 - Add-Saturate]
// ============================================================================

static FOG_INLINE void m128iAddsPI8(m128i& dst, const m128i& a, const m128i& b)
{
  dst = _mm_adds_epi8(a, b);
}

static FOG_INLINE void m128iAddusPU8(m128i& dst, const m128i& a, const m128i& b)
{
  dst = _mm_adds_epu8(a, b);
}

static FOG_INLINE void m128iAddsPI16(m128i& dst, const m128i& a, const m128i& b)
{
  dst = _mm_adds_epi16(a, b);
}

static FOG_INLINE void m128iAddusPU16(m128i& dst, const m128i& a, const m128i& b)
{
  dst = _mm_adds_epu16(a, b);
}

// ============================================================================
// [Fog::Face - SSE2 - Sub]
// ============================================================================

static FOG_INLINE void m128iSubPI8(m128i& dst, const m128i& a, const m128i& b)
{
  dst = _mm_sub_epi8(a, b);
}

static FOG_INLINE void m128iSubPI16(m128i& dst, const m128i& a, const m128i& b)
{
  dst = _mm_sub_epi16(a, b);
}

static FOG_INLINE void m128iSubPI32(m128i& dst, const m128i& a, const m128i& b)
{
  dst = _mm_sub_epi32(a, b);
}

static FOG_INLINE void m128iSubPI64(m128i& dst, const m128i& a, const m128i& b)
{
  dst = _mm_sub_epi64(a, b);
}

static FOG_INLINE void m128dSubSD(m128d& dst, const m128d& a, const m128d& b)
{
  dst = _mm_sub_sd(a, b);
}

static FOG_INLINE void m128dSubPD(m128d& dst, const m128d& a, const m128d& b)
{
  dst = _mm_sub_pd(a, b);
}

// ============================================================================
// [Fog::Face - SSE2 - Sub-Saturate]
// ============================================================================

static FOG_INLINE void m128iSubsPI8(m128i& dst, const m128i& a, const m128i& b)
{
  dst = _mm_subs_epi8(a, b);
}

static FOG_INLINE void m128iSubusPU8(m128i& dst, const m128i& a, const m128i& b)
{
  dst = _mm_subs_epu8(a, b);
}

static FOG_INLINE void m128iSubsPI16(m128i& dst, const m128i& a, const m128i& b)
{
  dst = _mm_subs_epi16(a, b);
}

static FOG_INLINE void m128iSubusPU16(m128i& dst, const m128i& a, const m128i& b)
{
  dst = _mm_subs_epu16(a, b);
}

// ============================================================================
// [Fog::Face - SSE2 - Mul]
// ============================================================================

static FOG_INLINE void m128iMulLoPI16(m128i& dst, const m128i& a, const m128i& b)
{
  dst = _mm_mullo_epi16(a, b);
}

static FOG_INLINE void m128iMulHiPI16(m128i& dst, const m128i& a, const m128i& b)
{
  dst = _mm_mulhi_epi16(a, b);
}

static FOG_INLINE void m128iMulHiPU16(m128i& dst, const m128i& a, const m128i& b)
{
  dst = _mm_mulhi_epu16(a, b);
}

// TODO: Verify.
static FOG_INLINE void m128iMulPU32(m128i& dst, const m128i& a, const m128i& b)
{
  m128i ta = _mm_shuffle_epi32(a, _MM_SHUFFLE(2, 3, 0, 1));
  m128i tb = _mm_shuffle_epi32(b, _MM_SHUFFLE(2, 3, 0, 1));

  dst = _mm_mul_epu32(a, b);
  ta  = _mm_mul_epu32(ta, tb);

  dst = _mm_castps_si128(_mm_shuffle_ps(_mm_castsi128_ps(dst), _mm_castsi128_ps(ta), _MM_SHUFFLE(0, 2, 0, 2)));
}

static FOG_INLINE void m128dMulSD(m128d& dst, const m128d& a, const m128d& b)
{
  dst = _mm_mul_sd(a, b);
}

static FOG_INLINE void m128dMulPD(m128d& dst, const m128d& a, const m128d& b)
{
  dst = _mm_mul_pd(a, b);
}

// ============================================================================
// [Fog::Face - SSE2 - EMul]
// ============================================================================

static FOG_INLINE void m128iEMulPU32(m128i& dst, const m128i& a, const m128i& b)
{
  dst = _mm_mul_epu32(a, b);
}

// ============================================================================
// [Fog::Face - SSE2 - MAdd]
// ============================================================================

static FOG_INLINE void m128iMAddPI16(m128i& dst, const m128i& a, const m128i& b)
{
  dst = _mm_madd_epi16(a, b);
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
  dst = _mm_div_sd(_mm_load_sd(&FOG_XMM_GET_CONST_SD(m128d_2x_one)), a);
}

static FOG_INLINE void m128dRcpPD(m128d& dst, const m128d& a)
{
  dst = _mm_div_pd(FOG_XMM_GET_CONST_PD(m128d_2x_one), a);
}

// ============================================================================
// [Fog::Face - SSE2 - Min/Max]
// ============================================================================

static FOG_INLINE void m128iMinPU8(m128i& dst, const m128i& a, const m128i& b)
{
  dst = _mm_min_epu8(a, b);
}

static FOG_INLINE void m128iMinPI16(m128i& dst, const m128i& a, const m128i& b)
{
  dst = _mm_min_epi16(a, b);

}
static FOG_INLINE void m128dMinSD(m128d& dst, const m128d& a, const m128d& b)
{
  dst = _mm_min_sd(a, b);
}

static FOG_INLINE void m128dMinPD(m128d& dst, const m128d& a, const m128d& b)
{
  dst = _mm_min_pd(a, b);
}

static FOG_INLINE void m128iMaxPU8(m128i& dst, const m128i& a, const m128i& b)
{
  dst = _mm_max_epu8(a, b);
}

static FOG_INLINE void m128iMaxPI16(m128i& dst, const m128i& a, const m128i& b)
{
  dst = _mm_max_epi16(a, b);
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
// [Fog::Face - SSE2 - Avg]
// ============================================================================

static FOG_INLINE void m128iAvgPU8(m128i& dst, const m128i& a, const m128i& b)
{
  dst = _mm_avg_epu8(a, b);
}

static FOG_INLINE void m128iAvgPU16(m128i& dst, const m128i& a, const m128i& b)
{
  dst = _mm_avg_epu16(a, b);
}

// ============================================================================
// [Fog::Face - SSE2 - BitOps]
// ============================================================================

static FOG_INLINE void m128iAnd(m128i& dst, const m128i& a, const m128i& b)
{
  dst = _mm_and_si128(a, b);
}

static FOG_INLINE void m128dAnd(m128d& dst, const m128d& a, const m128d& b)
{
  dst = _mm_and_pd(a, b);
}

static FOG_INLINE void m128iAndNot(m128i& dst, const m128i& a, const m128i& b)
{
  dst = _mm_andnot_si128(a, b);
}

static FOG_INLINE void m128dAndNot(m128d& dst, const m128d& a, const m128d& b)
{
  dst = _mm_andnot_pd(a, b);
}

static FOG_INLINE void m128iOr(m128i& dst, const m128i& a, const m128i& b)
{
  dst = _mm_or_si128(a, b);
}

static FOG_INLINE void m128dOr(m128d& dst, const m128d& a, const m128d& b)
{
  dst = _mm_or_pd(a, b);
}

static FOG_INLINE void m128iXor(m128i& dst, const m128i& a, const m128i& b)
{
  dst = _mm_xor_si128(a, b);
}

static FOG_INLINE void m128dXor(m128d& dst, const m128d& a, const m128d& b)
{
  dst = _mm_xor_pd(a, b);
}

// ============================================================================
// [Fog::Face - SSE2 - Extend]
// ============================================================================

static FOG_INLINE void m128iExtendPI8FromSI8(
  m128i& dst0, const m128i& x0)
{
  dst0 = _mm_mul_epu32(x0, FOG_XMM_GET_CONST_PI(0101010101010101_0101010101010101));
  dst0 = _mm_shuffle_epi32(dst0, _MM_SHUFFLE(0, 0, 0, 0));
}

static FOG_INLINE void m128iExtendPI16FromSI16(
  m128i& dst0, const m128i& x0)
{
  dst0 = _mm_shufflelo_epi16(x0, _MM_SHUFFLE(0, 0, 0, 0));
  dst0 = _mm_shuffle_epi32(dst0, _MM_SHUFFLE(0, 0, 0, 0));
}

static FOG_INLINE void m128iExtendPI16FromSI16Lo(
  m128i& dst0, const m128i& x0)
{
  dst0 = _mm_shufflelo_epi16(x0, _MM_SHUFFLE(0, 0, 0, 0));
}

static FOG_INLINE void m128iExtendPI16FromSI16Hi(
  m128i& dst0, const m128i& x0)
{
  dst0 = _mm_shufflehi_epi16(x0, _MM_SHUFFLE(0, 0, 0, 0));
}

static FOG_INLINE void m128iExtendPI32FromSI32(
  m128i& dst0, const m128i& x0)
{
  dst0 = _mm_shuffle_epi32(x0, _MM_SHUFFLE(0, 0, 0, 0));
}

// ============================================================================
// [Fog::Face - SSE2 - Expand]
// ============================================================================

static FOG_INLINE void m128iExpandPI8FromSI8(
  m128i& dst0, const m128i& x0)
{
  dst0 = _mm_unpacklo_epi8(x0, x0);
  dst0 = _mm_shufflelo_epi16(dst0, _MM_SHUFFLE(0, 0, 0, 0));
  dst0 = _mm_shuffle_epi32(dst0, _MM_SHUFFLE(0, 0, 0, 0));
}

static FOG_INLINE void m128iExpandPI8FromSI8Lo(
  m128i& dst0, const m128i& x0)
{
  dst0 = _mm_unpacklo_epi8(x0, x0);
  dst0 = _mm_shufflelo_epi16(x0, _MM_SHUFFLE(0, 0, 0, 0));
}

static FOG_INLINE void m128iExpandPI16FromSI16(
  m128i& dst0, const m128i& x0)
{
  dst0 = _mm_shufflelo_epi16(x0, _MM_SHUFFLE(0, 0, 0, 0));
  dst0 = _mm_shuffle_epi32(dst0, _MM_SHUFFLE(0, 0, 0, 0));
}

static FOG_INLINE void m128iExpandPI16FromSI16Lo(
  m128i& dst0, const m128i& x0)
{
  dst0 = _mm_shufflelo_epi16(x0, _MM_SHUFFLE(0, 0, 0, 0));
}

static FOG_INLINE void m128iExpandPI16FromSI16Hi(
  m128i& dst0, const m128i& x0)
{
  dst0 = _mm_shufflehi_epi16(x0, _MM_SHUFFLE(0, 0, 0, 0));
}

static FOG_INLINE void m128iExpandPI32FromSI32(
  m128i& dst0, const m128i& x0)
{
  dst0 = _mm_shuffle_epi32(x0, _MM_SHUFFLE(0, 0, 0, 0));
}

static FOG_INLINE void m128iExpandPI32FromSI32Lo(
  m128i& dst0, const m128i& x0)
{
  dst0 = _mm_shuffle_epi32(x0, _MM_SHUFFLE(3, 2, 0, 0));
}

static FOG_INLINE void m128iExpandPI32FromSI32Hi(
  m128i& dst0, const m128i& x0)
{
  dst0 = _mm_shuffle_epi32(x0, _MM_SHUFFLE(2, 2, 1, 0));
}

// ============================================================================
// [Fog::Face - SSE2 - LShift / RShift]
// ============================================================================

template<int COUNT_BITS>
static FOG_INLINE void m128iLShiftPU16(m128i& dst, const m128i& a)
{
  dst = _mm_slli_epi16(a, COUNT_BITS);
}

template<int COUNT_BITS>
static FOG_INLINE void m128iLShiftPU32(m128i& dst, const m128i& a)
{
  dst = _mm_slli_epi32(a, COUNT_BITS);
}

template<int COUNT_BITS>
static FOG_INLINE void m128iLShiftPU64(m128i& dst, const m128i& a)
{
  dst = _mm_slli_epi64(a, COUNT_BITS);
}

template<int COUNT_BITS>
static FOG_INLINE void m128iLShiftSU128(m128i& dst, const m128i& a)
{
  const char COUNT_BITS_Must_Be_Divisible_By_8[COUNT_BITS % 8 == 0 ? 1 : -1];
  dst = _mm_slli_si128(a, COUNT_BITS >> 3);
}

template<int COUNT_BITS>
static FOG_INLINE void m128iRShiftPU16(m128i& dst, const m128i& a)
{
  dst = _mm_srli_epi16(a, COUNT_BITS);
}

template<int COUNT_BITS>
static FOG_INLINE void m128iRShiftPU32(m128i& dst, const m128i& a)
{
  dst = _mm_srli_epi32(a, COUNT_BITS);
}

template<int COUNT_BITS>
static FOG_INLINE void m128iRShiftPU64(m128i& dst, const m128i& a)
{
  dst = _mm_srli_epi64(a, COUNT_BITS);
}

template<int COUNT_BITS>
static FOG_INLINE void m128iRShiftSU128(m128i& dst, const m128i& a)
{
  const char COUNT_BITS_Must_Be_Divisible_By_8[COUNT_BITS % 8 == 0 ? 1 : -1];
  dst = _mm_srli_si128(a, COUNT_BITS >> 3);
}

template<int COUNT_BITS>
static FOG_INLINE void m128iRShiftPI16(m128i& dst, const m128i& a)
{
  dst = _mm_srai_epi16(a, COUNT_BITS);
}

template<int COUNT_BITS>
static FOG_INLINE void m128iRShiftPI32(m128i& dst, const m128i& a)
{
  dst = _mm_srai_epi32(a, COUNT_BITS);
}

// ============================================================================
// [Fog::Face - SSE2 - Compare]
// ============================================================================

static FOG_INLINE void m128iCmpEqPI8(m128i& dst, const m128i& a, const m128i& b) { dst = _mm_cmpeq_epi8(a, b); }
static FOG_INLINE void m128iCmpEqPI16(m128i& dst, const m128i& a, const m128i& b) { dst = _mm_cmpeq_epi16(a, b); }
static FOG_INLINE void m128iCmpEqPI32(m128i& dst, const m128i& a, const m128i& b) { dst = _mm_cmpeq_epi32(a, b); }

static FOG_INLINE void m128iCmpGtPI8(m128i& dst, const m128i& a, const m128i& b) { dst = _mm_cmpgt_epi8(a, b); }
static FOG_INLINE void m128iCmpGtPI16(m128i& dst, const m128i& a, const m128i& b) { dst = _mm_cmpgt_epi16(a, b); }
static FOG_INLINE void m128iCmpGtPI32(m128i& dst, const m128i& a, const m128i& b) { dst = _mm_cmpgt_epi32(a, b); }

static FOG_INLINE void m128iCmpLtPI8(m128i& dst, const m128i& a, const m128i& b) { dst = _mm_cmplt_epi8(a, b); }
static FOG_INLINE void m128iCmpLtPI16(m128i& dst, const m128i& a, const m128i& b) { dst = _mm_cmplt_epi16(a, b); }
static FOG_INLINE void m128iCmpLtPI32(m128i& dst, const m128i& a, const m128i& b) { dst = _mm_cmplt_epi32(a, b); }

static FOG_INLINE void m128fCmpNgeSS(m128f& dst, const m128f& a, const m128f& b) { dst = _mm_cmpnge_ss(a, b); }
static FOG_INLINE void m128fCmpNgePS(m128f& dst, const m128f& a, const m128f& b) { dst = _mm_cmpnge_ps(a, b); }

static FOG_INLINE void m128fCmpOrderedSS(m128f& dst, const m128f& a, const m128f& b) { dst = _mm_cmpord_ss(a, b); }
static FOG_INLINE void m128fCmpOrderedPS(m128f& dst, const m128f& a, const m128f& b) { dst = _mm_cmpord_ps(a, b); }

static FOG_INLINE void m128fCmpUnorderedSS(m128f& dst, const m128f& a, const m128f& b) { dst = _mm_cmpunord_ss(a, b); }
static FOG_INLINE void m128fCmpUnorderedPS(m128f& dst, const m128f& a, const m128f& b) { dst = _mm_cmpunord_ps(a, b); }

static FOG_INLINE void m128dCmpEqSD(m128d& dst, const m128d& a, const m128d& b) { dst = _mm_cmpeq_sd(a, b); }
static FOG_INLINE void m128dCmpEqPD(m128d& dst, const m128d& a, const m128d& b) { dst = _mm_cmpeq_pd(a, b); }

static FOG_INLINE void m128dCmpGtSD(m128d& dst, const m128d& a, const m128d& b) { dst = _mm_cmpgt_sd(a, b); }
static FOG_INLINE void m128dCmpGtPD(m128d& dst, const m128d& a, const m128d& b) { dst = _mm_cmpgt_pd(a, b); }

static FOG_INLINE void m128dCmpGeSD(m128d& dst, const m128d& a, const m128d& b) { dst = _mm_cmpge_sd(a, b); }
static FOG_INLINE void m128dCmpGePD(m128d& dst, const m128d& a, const m128d& b) { dst = _mm_cmpge_pd(a, b);}

static FOG_INLINE void m128dCmpLtSD(m128d& dst, const m128d& a, const m128d& b) { dst = _mm_cmplt_sd(a, b); }
static FOG_INLINE void m128dCmpLtPD(m128d& dst, const m128d& a, const m128d& b) { dst = _mm_cmplt_pd(a, b); }

static FOG_INLINE void m128dCmpLeSD(m128d& dst, const m128d& a, const m128d& b) { dst = _mm_cmple_sd(a, b); }
static FOG_INLINE void m128dCmpLePD(m128d& dst, const m128d& a, const m128d& b) { dst = _mm_cmple_pd(a, b); }

static FOG_INLINE void m128dCmpNeqSD(m128d& dst, const m128d& a, const m128d& b) { dst = _mm_cmpneq_sd(a, b); }
static FOG_INLINE void m128dCmpNeqPD(m128d& dst, const m128d& a, const m128d& b) { dst = _mm_cmpneq_pd(a, b); }

static FOG_INLINE void m128dCmpNltSD(m128d& dst, const m128d& a, const m128d& b) { dst = _mm_cmpnlt_sd(a, b); }
static FOG_INLINE void m128dCmpNltPD(m128d& dst, const m128d& a, const m128d& b) { dst = _mm_cmpnlt_pd(a, b); }

static FOG_INLINE void m128dCmpNleSD(m128d& dst, const m128d& a, const m128d& b) { dst = _mm_cmpnle_sd(a, b); }
static FOG_INLINE void m128dCmpNlePD(m128d& dst, const m128d& a, const m128d& b) { dst = _mm_cmpnle_pd(a, b); }

static FOG_INLINE void m128dCmpNgtSD(m128d& dst, const m128d& a, const m128d& b) { dst = _mm_cmpngt_sd(a, b); }
static FOG_INLINE void m128dCmpNgtPD(m128d& dst, const m128d& a, const m128d& b) { dst = _mm_cmpngt_pd(a, b); }

// ============================================================================
// [Fog::Face - SSE2 - Epsilon]
// ============================================================================

static FOG_INLINE void m128dEpsilonSD(m128d& dst, const m128d& a)
{
  m128d sgn;
  sgn = FOG_XMM_GET_CONST_PD(m128d_2x_sn);
  sgn = _mm_and_pd(sgn, a);

  dst = _mm_and_pd(a, FOG_XMM_GET_CONST_PD(m128d_2x_nm));
  dst = _mm_max_sd(dst, FOG_XMM_GET_CONST_PD(m128d_2x_eps));
  dst = _mm_or_pd(dst, sgn);
}

static FOG_INLINE void m128dEpsilonPD(m128d& dst, const m128d& a)
{
  m128d sgn;
  sgn = FOG_XMM_GET_CONST_PD(m128d_2x_sn);
  sgn = _mm_and_pd(sgn, a);

  dst = _mm_and_pd(a, FOG_XMM_GET_CONST_PD(m128d_2x_nm));
  dst = _mm_max_pd(dst, FOG_XMM_GET_CONST_PD(m128d_2x_eps));
  dst = _mm_or_pd(dst, sgn);
}

// ============================================================================
// [Fog::Face - SSE2 - MoveMask]
// ============================================================================

//! @brief Create a 16-bit mask from the most significant bits of the 16 signed
//! or unsigned 8-bit integers and zero extend the upper bits.
static FOG_INLINE void m128iMoveMaskPI8(int& dst, const m128i& a)
{
  dst = _mm_movemask_epi8(a);
}

//! @brief Create a 4-bit mask from the most significant bits of the 4 signed
//! or unsigned 32-bit integers and zero extend the upper bits.
static FOG_INLINE void m128iMoveMaskPI32(int& dst, const m128i& a)
{
  dst = _mm_movemask_ps(_mm_castsi128_ps(a));
}

//! @brief Create a 2-bit mask from the most significant bits of the 2 signed
//! or unsigned 64-bit integers and zero extend the upper bits.
static FOG_INLINE void m128iMoveMaskPI64(int& dst, const m128i& a)
{
  dst = _mm_movemask_pd(_mm_castsi128_pd(a));
}

//! @brief Create a 2-bit mask from the most significant bits of the four DP-FP
//! values.
static FOG_INLINE void m128dMoveMaskPD(int& dst, const m128d& a)
{
  dst = _mm_movemask_pd(a);
}

// ============================================================================
// [Fog::Face - SSE2 - MulDiv255]
// ============================================================================

static FOG_INLINE void m128iMulDiv255PBW(
  __m128i& dst0, const __m128i& data0, const __m128i& alpha0)
{
  dst0 = _mm_mullo_epi16(data0, alpha0);
  dst0 = _mm_adds_epu16(dst0, FOG_XMM_GET_CONST_PI(0080008000800080_0080008000800080));
  dst0 = _mm_mulhi_epu16(dst0, FOG_XMM_GET_CONST_PI(0101010101010101_0101010101010101));
}

// ============================================================================
// [Fog::Face - SSE2 - Misc]
// ============================================================================

template<int INDEX_W>
static FOG_INLINE void m128iInsertPI16(
  __m128i& dst, const __m128i& a, const int& b)
{
  dst = _mm_insert_epi16(a, b, INDEX_W);
}

template<int INDEX_W>
static FOG_INLINE void m128iExtractPI16(
  int& dst, const __m128i& a)
{
  dst = _mm_extract_epi16(a, INDEX_W);
}

//! @}

} // Face namespace
} // Fog namespace

// MMX+SSE2 extensions require both, Face_MMX.h, and Face_SSE2.h headers.
#if defined(_FOG_CORE_FACE_FACE_MMX_H)
# include <Fog/Core/Face/Face_MMX_SSE2.h>
#endif // _FOG_CORE_FACE_FACE_MMX_H

// [Guard]
#endif // _FOG_CORE_FACE_FACE_SSE2_H
