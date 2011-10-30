// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_FACE_FACESSE2_H
#define _FOG_CORE_FACE_FACESSE2_H

// [Dependencies]
#include <Fog/Core/C++/Base.h>
#include <Fog/Core/C++/IntrinSSE2.h>

#include <Fog/Core/Face/FaceSSE.h>
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
FOG_XMM_DECLARE_CONST_PI16_VAR(000000FF000000FF_000000FF000000FF, 0x0000, 0x00FF, 0x0000, 0x00FF, 0x0000, 0x00FF, 0x0000, 0x00FF);
FOG_XMM_DECLARE_CONST_PI16_VAR(000000FF00FF00FF_000000FF00FF00FF, 0x0000, 0x00FF, 0x00FF, 0x00FF, 0x0000, 0x00FF, 0x00FF, 0x00FF);
FOG_XMM_DECLARE_CONST_PI16_VAR(000000FF00FF00FF_00FF00FF00FF00FF, 0x0000, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF);
FOG_XMM_DECLARE_CONST_PI16_VAR(0000FFFF0000FFFF_0000FFFF0000FFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF);
FOG_XMM_DECLARE_CONST_PI16_VAR(0001000000010000_0001000000010000, 0x0001, 0x0000, 0x0001, 0x0000, 0x0001, 0x0000, 0x0001, 0x0000);
FOG_XMM_DECLARE_CONST_PI16_VAR(0001000200020002_0001000200020002, 0x0001, 0x0002, 0x0002, 0x0002, 0x0001, 0x0002, 0x0002, 0x0002);
FOG_XMM_DECLARE_CONST_PI16_VAR(0080008000800080_0080008000800080, 0x0080, 0x0080, 0x0080, 0x0080, 0x0080, 0x0080, 0x0080, 0x0080);
FOG_XMM_DECLARE_CONST_PI16_VAR(00FF000000000000_0000000000000000, 0x00FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000);
FOG_XMM_DECLARE_CONST_PI16_VAR(00FF000000000000_00FF000000000000, 0x00FF, 0x0000, 0x0000, 0x0000, 0x00FF, 0x0000, 0x0000, 0x0000);
FOG_XMM_DECLARE_CONST_PI16_VAR(00FF00FF00FF00FF_0000000000000000, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x0000, 0x0000, 0x0000, 0x0000);
FOG_XMM_DECLARE_CONST_PI16_VAR(00FF00FF00FF00FF_000000FF00FF00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x0000, 0x00FF, 0x00FF, 0x00FF);
FOG_XMM_DECLARE_CONST_PI16_VAR(00FF00FF00FF00FF_00FF00FF00FF00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF);
FOG_XMM_DECLARE_CONST_PI16_VAR(0101010101010101_0101010101010101, 0x0101, 0x0101, 0x0101, 0x0101, 0x0101, 0x0101, 0x0101, 0x0101);
FOG_XMM_DECLARE_CONST_PI16_VAR(0100010001000100_0100010001000100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100);
FOG_XMM_DECLARE_CONST_PI16_VAR(FF000000000000FF_FF000000000000FF, 0xFF00, 0x0000, 0x0000, 0x00FF, 0xFF00, 0x0000, 0x0000, 0x00FF);
FOG_XMM_DECLARE_CONST_PI16_VAR(FF000000FF000000_FF000000FF000000, 0xFF00, 0x0000, 0xFF00, 0x0000, 0xFF00, 0x0000, 0xFF00, 0x0000);
FOG_XMM_DECLARE_CONST_PI16_VAR(FF00FF00FF00FF00_FF00FF00FF00FF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00);
FOG_XMM_DECLARE_CONST_PI16_VAR(FFFFFFFFFFFFFFFF_0000000000000000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000);
FOG_XMM_DECLARE_CONST_PI16_VAR(FFFFFFFFFFFFFFFF_FFFFFFFFFFFFFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF);
FOG_XMM_DECLARE_CONST_PI16_VAR(00FFFFFF00FFFFFF_00FFFFFF00FFFFFF, 0x00FF, 0xFFFF, 0x00FF, 0xFFFF, 0x00FF, 0xFFFF, 0x00FF, 0xFFFF);
FOG_XMM_DECLARE_CONST_PI16_VAR(0000000100000001_0000000100000001, 0x0000, 0x0001, 0x0000, 0x0001, 0x0000, 0x0001, 0x0000, 0x0001);

FOG_XMM_DECLARE_CONST_PI16_VAR(0081008100810081_0081008100810081, 0x0081, 0x0081, 0x0081, 0x0081, 0x0081, 0x0081, 0x0081, 0x0081);

// A constant that is subtracted from the U32 value and added back to U16 value
// to simulate unsigned saturation (this instruction is missing).
FOG_XMM_DECLARE_CONST_PI32_SET(m128i_packusdw_u32  , 0x8000);
FOG_XMM_DECLARE_CONST_PI16_SET(m128i_packusdw_u16  , 0x8000);

FOG_XMM_DECLARE_CONST_PI64_SET(m128d_sn_sn          , FOG_UINT64_C(0x8000000000000000));
FOG_XMM_DECLARE_CONST_PI64_SET(m128d_nm_nm          , FOG_UINT64_C(0x7FFFFFFFFFFFFFFF));

FOG_XMM_DECLARE_CONST_PD_VAR  (m128d_p0_p1          , 0.0, 1.0);
FOG_XMM_DECLARE_CONST_PD_SET  (m128d_p1_p1          , 1.0);
FOG_XMM_DECLARE_CONST_PD_SET  (m128d_eps_eps        , Fog::MATH_EPSILON_D);

FOG_XMM_DECLARE_CONST_PD_SET  (m128d_p255_p255      , 255.0);
FOG_XMM_DECLARE_CONST_PD_SET  (m128d_p256_p256      , 256.0);
FOG_XMM_DECLARE_CONST_PD_SET  (m128d_p65535_p65535  , 65535.0);
FOG_XMM_DECLARE_CONST_PD_SET  (m128d_p65536_p65536  , 65536.0);

FOG_XMM_DECLARE_CONST_PD_SET  (m128d_1d255_1d255    , Fog::MATH_1_DIV_255);
FOG_XMM_DECLARE_CONST_PD_SET  (m128d_1d256_1d256    , Fog::MATH_1_DIV_256);
FOG_XMM_DECLARE_CONST_PD_SET  (m128d_1d65535_1d65535, Fog::MATH_1_DIV_65535);
FOG_XMM_DECLARE_CONST_PD_SET  (m128d_1d65536_1d65536, Fog::MATH_1_DIV_65536);

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
static FOG_INLINE void m128iStore4(DstT* dstp, const m128i& x0)
{
  reinterpret_cast<int*>(dstp)[0] = _mm_cvtsi128_si32(x0);
}

template<typename DstT>
static FOG_INLINE void m128iStore4nt(DstT* dstp, uint32_t x0)
{
  _mm_stream_si32(reinterpret_cast<int*>(dstp), (int)x0);
}

template<typename DstT>
static FOG_INLINE void m128iStore8(DstT* dstp, const m128i& x0)
{
  _mm_storel_epi64(reinterpret_cast<m128i*>(dstp), x0);
}

template<typename DstT>
static FOG_INLINE void m128dStore8(DstT* dstp, const m128d& x0)
{
  _mm_store_sd(reinterpret_cast<double*>(dstp), x0);
}

template<typename DstT>
static FOG_INLINE void m128dStore8Lo(DstT* dstp, const m128d& x0)
{
  _mm_storel_pd(reinterpret_cast<double*>(dstp), x0);
}

template<typename DstT>
static FOG_INLINE void m128dStore8Hi(DstT* dstp, const m128d& x0)
{
  _mm_storeh_pd(reinterpret_cast<double*>(dstp), x0);
}

template<typename DstT>
static FOG_INLINE void m128iStore16a(DstT* dstp, const m128i& x0)
{
  _mm_store_si128(reinterpret_cast<m128i*>(dstp), x0);
}

template<typename DstT>
static FOG_INLINE void m128dStore16a(DstT* dstp, const m128d& x0)
{
  _mm_store_pd(reinterpret_cast<double*>(dstp), x0);
}

template<typename DstT>
static FOG_INLINE void m128iStore16u(DstT* dstp, const m128i& x0)
{
  _mm_storeu_si128(reinterpret_cast<m128i*>(dstp), x0);
}

template<typename DstT>
static FOG_INLINE void m128iStore16uLoHi(DstT* dstp, const m128i& x0)
{
  m128d xd = _mm_castsi128_pd(x0);

  _mm_storel_pd(reinterpret_cast<double*>(dstp + 0), xd);
  _mm_storeh_pd(reinterpret_cast<double*>(dstp + 1), xd);
}

template<typename DstT>
static FOG_INLINE void m128dStore16u(DstT* dstp, const m128d& x0)
{
  _mm_storeu_pd(reinterpret_cast<double*>(dstp), x0);
}

template<typename DstT>
static FOG_INLINE void m128iStore16nta(DstT* dstp, const m128i& x0)
{
  _mm_stream_si128(reinterpret_cast<m128i*>(dstp), x0);
}

template<typename DstT>
static FOG_INLINE void m128dStore16nta(DstT* dstp, const m128d& x0)
{
  _mm_stream_pd(dstp, x0);
}

// ============================================================================
// [Fog::Face - SSE2 - Copy]
// ============================================================================

static FOG_INLINE void m128iCopy(
  m128i& dst0, const m128i& x0)
{
  dst0 = x0;
}

static FOG_INLINE void m128iCopy_2x(
  m128i& dst0, const m128i& x0,
  m128i& dst1, const m128i& x1)
{
  dst0 = x0;
  dst1 = x1;
}

// ============================================================================
// [Fog::Face - SSE2 - Convert]
// ============================================================================

static FOG_INLINE void m128iCvtSI128FromSI(m128i& dst0, int x0)
{
  dst0 = _mm_cvtsi32_si128(x0);
}

static FOG_INLINE void m128iCvtSIFromSI128(int& dst0, const m128i& x0)
{
  dst0 = _mm_cvtsi128_si32(x0);
}

static FOG_INLINE void m128fCvtPSFromPI32(m128f& dst0, const m128i& x0)
{
  dst0 = _mm_cvtepi32_ps(x0);
}

static FOG_INLINE void m128fCvtPSFromPD(m128f& dst0, const m128d& x0)
{
  dst0 = _mm_cvtpd_ps(x0);
}

static FOG_INLINE void m128dCvtPDFromPI32(m128d& dst0, const m128i& x0)
{
  dst0 = _mm_cvtepi32_pd(x0);
}

static FOG_INLINE void m128dCvtPDFromPS(m128d& dst0, const m128f& x0)
{
  dst0 = _mm_cvtps_pd(x0);
}

static FOG_INLINE void m128iCvtPI32FromPS(m128i& dst0, const m128f& x0)
{
  dst0 = _mm_cvtps_epi32(x0);
}

static FOG_INLINE void m128iCvtPI32FromPD(m128i& dst0, const m128d& x0)
{
  dst0 = _mm_cvtpd_epi32(x0);
}

// ============================================================================
// [Fog::Face - SSE2 - Cvt256From255]
// ============================================================================

static FOG_INLINE void m128iCvt256From255PI16(
  m128i& dst0, const m128i& x0)
{
  dst0 = _mm_mullo_epi16(x0, FOG_XMM_GET_CONST_PI(0081008100810081_0081008100810081));
  dst0 = _mm_slli_epi16(dst0, 7);
}

static FOG_INLINE void m128iCvt256From255PI16_2x(
  m128i& dst0, const m128i& x0,
  m128i& dst1, const m128i& x1)
{
  dst0 = _mm_mullo_epi16(x0, FOG_XMM_GET_CONST_PI(0081008100810081_0081008100810081));
  dst1 = _mm_mullo_epi16(x1, FOG_XMM_GET_CONST_PI(0081008100810081_0081008100810081));

  dst0 = _mm_slli_epi16(dst0, 7);
  dst1 = _mm_slli_epi16(dst1, 7);
}

// ============================================================================
// [Fog::Face - SSE2 - Trunc]
// ============================================================================

static FOG_INLINE void m128iTruncPI32FromSS(int& dst0, const m128f& x0)
{
  dst0 = _mm_cvttss_si32(x0);
}

static FOG_INLINE void m128iTruncPI32FromSD(int& dst0, const m128d& x0)
{
  dst0 = _mm_cvttsd_si32(x0);
}

static FOG_INLINE void m128iTruncPI32FromPS(m128i& dst0, const m128f& x0)
{
  dst0 = _mm_cvttps_epi32(x0);
}

static FOG_INLINE void m128iTruncPI32FromPD(m128i& dst0, const m128d& x0)
{
  dst0 = _mm_cvttpd_epi32(x0);
}

// ============================================================================
// [Fog::Face - SSE2 - Unpack]
// ============================================================================

static FOG_INLINE void m128iUnpackPI16FromPI8Lo(m128i& dst0, const m128i& x0)
{
  dst0 = _mm_unpacklo_epi8(x0, _mm_setzero_si128());
}

static FOG_INLINE void m128iUnpackPI16FromPI8Hi(m128i& dst0, const m128i& x0)
{
  dst0 = _mm_unpackhi_epi8(x0, _mm_setzero_si128());
}

static FOG_INLINE void m128iUnpackPI16FromPI8Lo(m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_unpacklo_epi8(x0, y0);
}

static FOG_INLINE void m128iUnpackPI16FromPI8Hi(m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_unpackhi_epi8(x0, y0);
}

static FOG_INLINE void m128iUnpackPI32FromPI8Lo(m128i& dst0, const m128i& x0)
{
  m128i z0 = _mm_setzero_si128();

  dst0 = _mm_unpacklo_epi8(x0, z0);
  dst0 = _mm_unpacklo_epi16(dst0, z0);
}

static FOG_INLINE void m128iUnpackPI32FromPI8Hi(m128i& dst0, const m128i& x0)
{
  m128i z0 = _mm_setzero_si128();

  dst0 = _mm_unpackhi_epi8(x0, z0);
  dst0 = _mm_unpacklo_epi16(dst0, z0);
}

static FOG_INLINE void m128iUnpackPI32FromPI16Lo(m128i& dst0, const m128i& x0)
{
  dst0 = _mm_unpacklo_epi16(x0, _mm_setzero_si128());
}

static FOG_INLINE void m128iUnpackPI32FromPI16Hi(m128i& dst0, const m128i& x0)
{
  dst0 = _mm_unpackhi_epi16(x0, _mm_setzero_si128());
}

static FOG_INLINE void m128iUnpackPI32FromPI16Lo(m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_unpacklo_epi16(x0, y0);
}

static FOG_INLINE void m128iUnpackPI32FromPI16Hi(m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_unpackhi_epi16(x0, y0);
}

static FOG_INLINE void m128iUnpackPI64FromPI32Lo(m128i& dst0, const m128i& x0)
{
  dst0 = _mm_unpacklo_epi32(x0, _mm_setzero_si128());
}

static FOG_INLINE void m128iUnpackPI64FromPI32Hi(m128i& dst0, const m128i& x0)
{
  dst0 = _mm_unpackhi_epi32(x0, _mm_setzero_si128());
}

static FOG_INLINE void m128iUnpackPI64FromPI32Lo(m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_unpacklo_epi32(x0, y0);
}

static FOG_INLINE void m128iUnpackSI128FromPI64Lo(m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_unpacklo_epi64(x0, y0);
}

static FOG_INLINE void m128iUnpackPI64FromPI32Hi(m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_unpackhi_epi32(x0, y0);
}

static FOG_INLINE void m128iUnpackSI128FromPI64Hi(m128i& dst0, const m128i& x0)
{
  dst0 = _mm_unpackhi_epi64(x0, _mm_setzero_si128());
}

static FOG_INLINE void m128dUnpackLoPD(m128d& dst0, const m128d& x0, const m128d& y0)
{
  dst0 = _mm_unpacklo_pd(x0, y0);
}

static FOG_INLINE void m128dUnpackHiPD(m128d& dst0, const m128d& x0, const m128d& y0)
{
  dst0 = _mm_unpackhi_pd(x0, y0);
}

// ============================================================================
// [Fog::Face - SSE2 - Pack]
// ============================================================================

static FOG_INLINE void m128iPackPI8FromPI16(m128i& dst0, const m128i& x0)
{
  dst0 = _mm_packs_epi16(x0, x0);
}

static FOG_INLINE void m128iPackPI8FromPI16(m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_packs_epi16(x0, y0);
}

static FOG_INLINE void m128iPackPU8FromPU16(m128i& dst0, const m128i& x0)
{
  dst0 = _mm_packus_epi16(x0, x0);
}

static FOG_INLINE void m128iPackPU8FromPU16(m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_packus_epi16(x0, y0);
}

static FOG_INLINE void m128iPackPU8FromPI32(m128i& dst0, const m128i& x0)
{
  dst0 = _mm_packs_epi32(x0, x0);
  dst0 = _mm_packus_epi16(dst0, dst0);
}

static FOG_INLINE void m128iPackPU8FromPI32(m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_packs_epi32(x0, y0);
  dst0 = _mm_packus_epi16(dst0, dst0);
}

static FOG_INLINE void m128iPackPU8FromPI32(m128i& dst0, const m128i& x0, const m128i& y0, const m128i& z0, const m128i& w0)
{
  m128i tmp0;

  tmp0 = _mm_packs_epi32(z0, w0);
  dst0 = _mm_packs_epi32(x0, y0);
  dst0 = _mm_packus_epi16(dst0, tmp0);
}

static FOG_INLINE void m128iPackPI16FromPI32(m128i& dst0, const m128i& x0)
{
  dst0 = _mm_packs_epi32(x0, x0);
}

static FOG_INLINE void m128iPackPI16FromPI32(m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_packs_epi32(x0, y0);
}

static FOG_INLINE void m128iPackPU16FromPI32(m128i& dst0, const m128i& x0)
{
  dst0 = _mm_sub_epi32(x0, FOG_XMM_GET_CONST_PI(m128i_packusdw_u32));
  dst0 = _mm_packs_epi32(dst0, dst0);
  dst0 = _mm_add_epi16(dst0, FOG_XMM_GET_CONST_PI(m128i_packusdw_u16));
}

static FOG_INLINE void m128iPackPU16FromPI32(m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_packs_epi32(
    _mm_sub_epi32(x0, FOG_XMM_GET_CONST_PI(m128i_packusdw_u32)),
    _mm_sub_epi32(y0, FOG_XMM_GET_CONST_PI(m128i_packusdw_u32)));
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
static FOG_INLINE void m128iZeroPI8i(m128i& dst0, const m128i& x0)
{
  FOG_XMM_DECLARE_CONST_PI8_VAR(Tmp,
    (I!= 0 ? 0xFF : 0x00), (I!= 1 ? 0xFF : 0x00), (I!= 2 ? 0xFF : 0x00), (I!= 3 ? 0xFF : 0x00),
    (I!= 4 ? 0xFF : 0x00), (I!= 5 ? 0xFF : 0x00), (I!= 6 ? 0xFF : 0x00), (I!= 7 ? 0xFF : 0x00),
    (I!= 8 ? 0xFF : 0x00), (I!= 9 ? 0xFF : 0x00), (I!=10 ? 0xFF : 0x00), (I!=11 ? 0xFF : 0x00),
    (I!=12 ? 0xFF : 0x00), (I!=13 ? 0xFF : 0x00), (I!=14 ? 0xFF : 0x00), (I!=15 ? 0xFF : 0x00));

  dst0 = _mm_and_si128(x0, FOG_XMM_GET_CONST_PI(Tmp));
}

template<int I>
static FOG_INLINE void m128iZeroPI16i(m128i& dst0, const m128i& x0)
{
  FOG_XMM_DECLARE_CONST_PI16_VAR(Tmp,
    (I!=0 ? 0xFFFF : 0x0000), (I!=1 ? 0xFFFF : 0x0000), (I!=2 ? 0xFFFF : 0x0000), (I!=3 ? 0xFFFF : 0x0000),
    (I!=4 ? 0xFFFF : 0x0000), (I!=5 ? 0xFFFF : 0x0000), (I!=6 ? 0xFFFF : 0x0000), (I!=7 ? 0xFFFF : 0x0000));

  dst0 = _mm_and_si128(x0, FOG_XMM_GET_CONST_PI(Tmp));
}

template<int I>
static FOG_INLINE void m128iZeroPI32i(m128i& dst0, const m128i& x0)
{
  FOG_XMM_DECLARE_CONST_PI32_VAR(Tmp,
    (I!=0 ? 0xFFFFFFFF : 0x00000000), (I!=1 ? 0xFFFFFFFF : 0x00000000),
    (I!=2 ? 0xFFFFFFFF : 0x00000000), (I!=3 ? 0xFFFFFFFF : 0x00000000));

  dst0 = _mm_and_si128(x0, FOG_XMM_GET_CONST_PI(Tmp));
}

template<int I>
static FOG_INLINE void m128iFillPI8i(m128i& dst0, const m128i& x0)
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
static FOG_INLINE void m128iFillPI16i(m128i& dst0, const m128i& x0)
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
static FOG_INLINE void m128iFillPI32i(m128i& dst0, const m128i& x0)
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
static FOG_INLINE void m128iShufflePI16(m128i& dst0, const m128i& x0)
{
  dst0 = _mm_shufflehi_epi16(x0, _MM_SHUFFLE(Z, Y, X, W));
  dst0 = _mm_shufflelo_epi16(dst0, _MM_SHUFFLE(Z, Y, X, W));
}

template<int W, int X, int Y, int Z>
static FOG_INLINE void m128iShufflePI16Lo(m128i& dst0, const m128i& x0)
{
  dst0 = _mm_shufflelo_epi16(x0, _MM_SHUFFLE(Z, Y, X, W));
}

template<int W, int X, int Y, int Z>
static FOG_INLINE void m128iShufflePI16Hi(m128i& dst0, const m128i& x0)
{
  dst0 = _mm_shufflehi_epi16(x0, _MM_SHUFFLE(Z, Y, X, W));
}

template<int Z, int Y, int X, int W>
static FOG_INLINE void m128iShufflePI32(m128i& dst0, const m128i& x0)
{
  dst0 = _mm_shuffle_epi32(x0, _MM_SHUFFLE(Z, Y, X, W));
}

// ============================================================================
// [Fog::Face - SSE2 - Swap]
// ============================================================================

static FOG_INLINE void m128iSwapPI16(m128i& dst0, const m128i& x0)
{
  m128iShufflePI16Lo<0, 1, 2, 3>(dst0, x0);
  m128iShufflePI16Hi<0, 1, 2, 3>(dst0, dst0);
}

static FOG_INLINE void m128iSwapPI16Lo(m128i& dst0, const m128i& x0)
{
  m128iShufflePI16Lo<0, 1, 2, 3>(dst0, x0);
}

static FOG_INLINE void m128iSwapPI16Hi(m128i& dst0, const m128i& x0)
{
  m128iShufflePI16Lo<0, 1, 2, 3>(dst0, x0);
}

static FOG_INLINE void m128iSwapPI32(m128i& dst0, const m128i& x0)
{
  m128iShufflePI32<0, 1, 2, 3>(dst0, x0);
}

static FOG_INLINE void m128dSwapPD(m128d& dst0, const m128d& x0)
{
  dst0 = _mm_shuffle_pd(x0, x0, _MM_SHUFFLE2(0, 1));
}

// ============================================================================
// [Fog::Face - SSE2 - Extend]
// ============================================================================

template<typename SrcT>
static FOG_INLINE void m128dExtendLo(m128d& dst0, const SrcT* srcp)
{
  dst0 = _mm_load_sd(reinterpret_cast<const double*>(srcp));
  dst0 = _mm_unpacklo_pd(dst0, dst0);
}

static FOG_INLINE void m128dExtendLo(m128d& dst0, const m128d& x0)
{
  dst0 = _mm_unpacklo_pd(x0, x0);
}

static FOG_INLINE void m128dExtendHi(m128d& dst0, const m128d& x0)
{
  dst0 = _mm_unpackhi_pd(x0, x0);
}

// ============================================================================
// [Fog::Face - SSE2 - Add]
// ============================================================================

static FOG_INLINE void m128iAddPI8(
  m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_add_epi8(x0, y0);
}

static FOG_INLINE void m128iAddPI8_2x(
  m128i& dst0, const m128i& x0, const m128i& y0,
  m128i& dst1, const m128i& x1, const m128i& y1)
{
  dst0 = _mm_add_epi8(x0, y0);
  dst1 = _mm_add_epi8(x1, y1);
}

static FOG_INLINE void m128iAddPI16(
  m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_add_epi16(x0, y0);
}

static FOG_INLINE void m128iAddPI16_2x(
  m128i& dst0, const m128i& x0, const m128i& y0,
  m128i& dst1, const m128i& x1, const m128i& y1)
{
  dst0 = _mm_add_epi16(x0, y0);
  dst1 = _mm_add_epi16(x1, y1);
}

static FOG_INLINE void m128iAddPI32(
  m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_add_epi32(x0, y0);
}

static FOG_INLINE void m128iAddPI32_2x(
  m128i& dst0, const m128i& x0, const m128i& y0,
  m128i& dst1, const m128i& x1, const m128i& y1)
{
  dst0 = _mm_add_epi32(x0, y0);
  dst1 = _mm_add_epi32(x1, y1);
}

static FOG_INLINE void m128iAddPI64(
  m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_add_epi64(x0, y0);
}

static FOG_INLINE void m128iAddPI64_2x(
  m128i& dst0, const m128i& x0, const m128i& y0,
  m128i& dst1, const m128i& x1, const m128i& y1)
{
  dst0 = _mm_add_epi64(x0, y0);
  dst1 = _mm_add_epi64(x1, y1);
}

static FOG_INLINE void m128dAddSD(
  m128d& dst0, const m128d& x0, const m128d& y0)
{
  dst0 = _mm_add_sd(x0, y0);
}

static FOG_INLINE void m128dAddSD_2x(
  m128d& dst0, const m128d& x0, const m128d& y0,
  m128d& dst1, const m128d& x1, const m128d& y1)
{
  dst0 = _mm_add_sd(x0, y0);
  dst1 = _mm_add_sd(x1, y1);
}

static FOG_INLINE void m128dAddPD(
  m128d& dst0, const m128d& x0, const m128d& y0)
{
  dst0 = _mm_add_pd(x0, y0);
}

static FOG_INLINE void m128dAddPD_2x(
  m128d& dst0, const m128d& x0, const m128d& y0,
  m128d& dst1, const m128d& x1, const m128d& y1)
{
  dst0 = _mm_add_pd(x0, y0);
  dst1 = _mm_add_pd(x1, y1);
}

// ============================================================================
// [Fog::Face - SSE2 - Add-Saturate]
// ============================================================================

static FOG_INLINE void m128iAddsPI8(
  m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_adds_epi8(x0, y0);
}

static FOG_INLINE void m128iAddsPI8_2x(
  m128i& dst0, const m128i& x0, const m128i& y0,
  m128i& dst1, const m128i& x1, const m128i& y1)
{
  dst0 = _mm_adds_epi8(x0, y0);
  dst1 = _mm_adds_epi8(x1, y1);
}

static FOG_INLINE void m128iAddusPU8(
  m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_adds_epu8(x0, y0);
}

static FOG_INLINE void m128iAddusPU8_2x(
  m128i& dst0, const m128i& x0, const m128i& y0,
  m128i& dst1, const m128i& x1, const m128i& y1)
{
  dst0 = _mm_adds_epu8(x0, y0);
  dst1 = _mm_adds_epu8(x1, y1);
}

static FOG_INLINE void m128iAddsPI16(
  m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_adds_epi16(x0, y0);
}

static FOG_INLINE void m128iAddsPI16_2x(
  m128i& dst0, const m128i& x0, const m128i& y0,
  m128i& dst1, const m128i& x1, const m128i& y1)
{
  dst0 = _mm_adds_epi16(x0, y0);
  dst1 = _mm_adds_epi16(x1, y1);
}

static FOG_INLINE void m128iAddusPU16(
  m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_adds_epu16(x0, y0);
}

static FOG_INLINE void m128iAddusPU16_2x(
  m128i& dst0, const m128i& x0, const m128i& y0,
  m128i& dst1, const m128i& x1, const m128i& y1)
{
  dst0 = _mm_adds_epu16(x0, y0);
  dst1 = _mm_adds_epu16(x1, y1);
}

// ============================================================================
// [Fog::Face - SSE2 - Sub]
// ============================================================================

static FOG_INLINE void m128iSubPI8(
  m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_sub_epi8(x0, y0);
}

static FOG_INLINE void m128iSubPI8_2x(
  m128i& dst0, const m128i& x0, const m128i& y0,
  m128i& dst1, const m128i& x1, const m128i& y1)
{
  dst0 = _mm_sub_epi8(x0, y0);
  dst1 = _mm_sub_epi8(x1, y1);
}

static FOG_INLINE void m128iSubPI16(
  m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_sub_epi16(x0, y0);
}

static FOG_INLINE void m128iSubPI16_2x(
  m128i& dst0, const m128i& x0, const m128i& y0,
  m128i& dst1, const m128i& x1, const m128i& y1)
{
  dst0 = _mm_sub_epi16(x0, y0);
  dst1 = _mm_sub_epi16(x1, y1);
}

static FOG_INLINE void m128iSubPI32(
  m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_sub_epi32(x0, y0);
}

static FOG_INLINE void m128iSubPI32_2x(
  m128i& dst0, const m128i& x0, const m128i& y0,
  m128i& dst1, const m128i& x1, const m128i& y1)
{
  dst0 = _mm_sub_epi32(x0, y0);
  dst1 = _mm_sub_epi32(x1, y1);
}

static FOG_INLINE void m128iSubPI64(
  m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_sub_epi64(x0, y0);
}

static FOG_INLINE void m128iSubPI64_2x(
  m128i& dst0, const m128i& x0, const m128i& y0,
  m128i& dst1, const m128i& x1, const m128i& y1)
{
  dst0 = _mm_sub_epi64(x0, y0);
  dst1 = _mm_sub_epi64(x1, y1);
}

static FOG_INLINE void m128dSubSD(
  m128d& dst0, const m128d& x0, const m128d& y0)
{
  dst0 = _mm_sub_sd(x0, y0);
}

static FOG_INLINE void m128dSubSD_2x(
  m128d& dst0, const m128d& x0, const m128d& y0,
  m128d& dst1, const m128d& x1, const m128d& y1)
{
  dst0 = _mm_sub_sd(x0, y0);
  dst1 = _mm_sub_sd(x1, y1);
}

static FOG_INLINE void m128dSubPD(
  m128d& dst0, const m128d& x0, const m128d& y0)
{
  dst0 = _mm_sub_pd(x0, y0);
}

static FOG_INLINE void m128dSubPD_2x(
  m128d& dst0, const m128d& x0, const m128d& y0,
  m128d& dst1, const m128d& x1, const m128d& y1)
{
  dst0 = _mm_sub_pd(x0, y0);
  dst1 = _mm_sub_pd(x1, y1);
}

// ============================================================================
// [Fog::Face - SSE2 - Sub-Saturate]
// ============================================================================

static FOG_INLINE void m128iSubsPI8(
  m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_subs_epi8(x0, y0);
}

static FOG_INLINE void m128iSubsPI8_2x(
  m128i& dst0, const m128i& x0, const m128i& y0,
  m128i& dst1, const m128i& x1, const m128i& y1)
{
  dst0 = _mm_subs_epi8(x0, y0);
  dst1 = _mm_subs_epi8(x1, y1);
}

static FOG_INLINE void m128iSubusPU8(
  m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_subs_epu8(x0, y0);
}

static FOG_INLINE void m128iSubusPU8_2x(
  m128i& dst0, const m128i& x0, const m128i& y0,
  m128i& dst1, const m128i& x1, const m128i& y1)
{
  dst0 = _mm_subs_epu8(x0, y0);
  dst1 = _mm_subs_epu8(x1, y1);
}

static FOG_INLINE void m128iSubsPI16(
  m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_subs_epi16(x0, y0);
}

static FOG_INLINE void m128iSubsPI16_2x(
  m128i& dst0, const m128i& x0, const m128i& y0,
  m128i& dst1, const m128i& x1, const m128i& y1)
{
  dst0 = _mm_subs_epi16(x0, y0);
  dst1 = _mm_subs_epi16(x1, y1);
}

static FOG_INLINE void m128iSubusPU16(
  m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_subs_epu16(x0, y0);
}

static FOG_INLINE void m128iSubusPU16_2x(
  m128i& dst0, const m128i& x0, const m128i& y0,
  m128i& dst1, const m128i& x1, const m128i& y1)
{
  dst0 = _mm_subs_epu16(x0, y0);
  dst1 = _mm_subs_epu16(x1, y1);
}

// ============================================================================
// [Fog::Face - SSE2 - Mul]
// ============================================================================

static FOG_INLINE void m128iMulLoPI16(
  m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_mullo_epi16(x0, y0);
}

static FOG_INLINE void m128iMulLoPI16_2x(
  m128i& dst0, const m128i& x0, const m128i& y0,
  m128i& dst1, const m128i& x1, const m128i& y1)
{
  dst0 = _mm_mullo_epi16(x0, y0);
  dst1 = _mm_mullo_epi16(x1, y1);
}

static FOG_INLINE void m128iMulHiPI16(
  m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_mulhi_epi16(x0, y0);
}

static FOG_INLINE void m128iMulHiPI16_2x(
  m128i& dst0, const m128i& x0, const m128i& y0,
  m128i& dst1, const m128i& x1, const m128i& y1)
{
  dst0 = _mm_mulhi_epi16(x0, y0);
  dst1 = _mm_mulhi_epi16(x1, y1);
}

static FOG_INLINE void m128iMulHiPU16(
  m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_mulhi_epu16(x0, y0);
}

static FOG_INLINE void m128iMulHiPU16_2x(
  m128i& dst0, const m128i& x0, const m128i& y0,
  m128i& dst1, const m128i& x1, const m128i& y1)
{
  dst0 = _mm_mulhi_epu16(x0, y0);
  dst1 = _mm_mulhi_epu16(x1, y1);
}

// TODO: Verify.
static FOG_INLINE void m128iMulPU32(
  m128i& dst0, const m128i& x0, const m128i& y0)
{
  m128i ta = _mm_shuffle_epi32(x0, _MM_SHUFFLE(2, 3, 0, 1));
  m128i tb = _mm_shuffle_epi32(y0, _MM_SHUFFLE(2, 3, 0, 1));

  dst0 = _mm_mul_epu32(x0, y0);
  ta = _mm_mul_epu32(ta, tb);

  dst0 = _mm_castps_si128(_mm_shuffle_ps(_mm_castsi128_ps(dst0), _mm_castsi128_ps(ta), _MM_SHUFFLE(0, 2, 0, 2)));
}

static FOG_INLINE void m128dMulSD(
  m128d& dst0, const m128d& x0, const m128d& y0)
{
  dst0 = _mm_mul_sd(x0, y0);
}

static FOG_INLINE void m128dMulSD_2x(
  m128d& dst0, const m128d& x0, const m128d& y0,
  m128d& dst1, const m128d& x1, const m128d& y1)
{
  dst0 = _mm_mul_sd(x0, y0);
  dst1 = _mm_mul_sd(x1, y1);
}

static FOG_INLINE void m128dMulPD(
  m128d& dst0, const m128d& x0, const m128d& y0)
{
  dst0 = _mm_mul_pd(x0, y0);
}

static FOG_INLINE void m128dMulPD_2x(
  m128d& dst0, const m128d& x0, const m128d& y0,
  m128d& dst1, const m128d& x1, const m128d& y1)
{
  dst0 = _mm_mul_pd(x0, y0);
  dst1 = _mm_mul_pd(x1, y1);
}

// ============================================================================
// [Fog::Face - SSE2 - EMul]
// ============================================================================

static FOG_INLINE void m128iEMulPU32(
  m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_mul_epu32(x0, y0);
}

static FOG_INLINE void m128iEMulPU32_2x(
  m128i& dst0, const m128i& x0, const m128i& y0,
  m128i& dst1, const m128i& x1, const m128i& y1)
{
  dst0 = _mm_mul_epu32(x0, y0);
  dst1 = _mm_mul_epu32(x1, y1);
}

// ============================================================================
// [Fog::Face - SSE2 - MAdd]
// ============================================================================

static FOG_INLINE void m128iMAddPI16(
  m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_madd_epi16(x0, y0);
}

static FOG_INLINE void m128iMAddPI16_2x(
  m128i& dst0, const m128i& x0, const m128i& y0,
  m128i& dst1, const m128i& x1, const m128i& y1)
{
  dst0 = _mm_madd_epi16(x0, y0);
  dst1 = _mm_madd_epi16(x1, y1);
}

// ============================================================================
// [Fog::Face - SSE2 - Div]
// ============================================================================

static FOG_INLINE void m128dDivSD(
  m128d& dst0, const m128d& x0, const m128d& y0)
{
  dst0 = _mm_div_sd(x0, y0);
}

static FOG_INLINE void m128dDivSD_2x(
  m128d& dst0, const m128d& x0, const m128d& y0,
  m128d& dst1, const m128d& x1, const m128d& y1)
{
  dst0 = _mm_div_sd(x0, y0);
  dst1 = _mm_div_sd(x1, y1);
}

static FOG_INLINE void m128dDivPD(
  m128d& dst0, const m128d& x0, const m128d& y0)
{
  dst0 = _mm_div_pd(x0, y0);
}

static FOG_INLINE void m128dDivPD_2x(
  m128d& dst0, const m128d& x0, const m128d& y0,
  m128d& dst1, const m128d& x1, const m128d& y1)
{
  dst0 = _mm_div_pd(x0, y0);
  dst1 = _mm_div_pd(x1, y1);
}

// ============================================================================
// [Fog::Face - SSE2 - Sqrt]
// ============================================================================

//! @brief Calculate square-root of @a a.
//!
//! @verbatim
//! dst[0] = sqrt(x0[0])
//! dst[1] = x0[1]
//! @endverbatim
static FOG_INLINE void m128dSqrtSD(m128d& dst0, const m128d& x0)
{
  dst0 = _mm_sqrt_sd(x0, x0);
}

//! @brief Calculate square-root of @a b.
//!
//! @verbatim
//! dst[0] = sqrt(y0[0])
//! dst[1] = x0[1]
//! @endverbatim
static FOG_INLINE void m128dSqrtSD(m128d& dst0, const m128d& x0, const m128d& y0)
{
  dst0 = _mm_sqrt_sd(x0, y0);
}

//! @brief Calculate square-root of @a a.
//!
//! @verbatim
//! dst[0] = sqrt(x0[0])
//! dst[1] = sqrt(x0[1])
//! @endverbatim
static FOG_INLINE void m128dSqrtPD(m128d& dst0, const m128d& x0)
{
  dst0 = _mm_sqrt_pd(x0);
}

// ============================================================================
// [Fog::Face - SSE2 - Reciprocal]
// ============================================================================

static FOG_INLINE void m128dRcpSD(m128d& dst0, const m128d& x0)
{
  dst0 = _mm_div_sd(_mm_load_sd(&FOG_XMM_GET_CONST_SD(m128d_p1_p1)), x0);
}

static FOG_INLINE void m128dRcpPD(m128d& dst0, const m128d& x0)
{
  dst0 = _mm_div_pd(FOG_XMM_GET_CONST_PD(m128d_p1_p1), x0);
}

// ============================================================================
// [Fog::Face - SSE2 - Min/Max]
// ============================================================================

static FOG_INLINE void m128iMinPU8(m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_min_epu8(x0, y0);
}

static FOG_INLINE void m128iMinPI16(m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_min_epi16(x0, y0);
}

static FOG_INLINE void m128dMinSD(m128d& dst0, const m128d& x0, const m128d& y0)
{
  dst0 = _mm_min_sd(x0, y0);
}

static FOG_INLINE void m128dMinPD(m128d& dst0, const m128d& x0, const m128d& y0)
{
  dst0 = _mm_min_pd(x0, y0);
}

static FOG_INLINE void m128iMaxPU8(m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_max_epu8(x0, y0);
}

static FOG_INLINE void m128iMaxPI16(m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_max_epi16(x0, y0);
}

static FOG_INLINE void m128dMaxSD(m128d& dst0, const m128d& x0, const m128d& y0)
{
  dst0 = _mm_max_sd(x0, y0);
}

static FOG_INLINE void m128dMaxPD(m128d& dst0, const m128d& x0, const m128d& y0)
{
  dst0 = _mm_max_pd(x0, y0);
}

// ============================================================================
// [Fog::Face - SSE2 - Avg]
// ============================================================================

static FOG_INLINE void m128iAvgPU8(m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_avg_epu8(x0, y0);
}

static FOG_INLINE void m128iAvgPU16(m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_avg_epu16(x0, y0);
}

// ============================================================================
// [Fog::Face - SSE2 - BitOps]
// ============================================================================

static FOG_INLINE void m128iAnd(m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_and_si128(x0, y0);
}

static FOG_INLINE void m128dAnd(m128d& dst0, const m128d& x0, const m128d& y0)
{
  dst0 = _mm_and_pd(x0, y0);
}

static FOG_INLINE void m128iAndNot(m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_andnot_si128(x0, y0);
}

static FOG_INLINE void m128dAndNot(m128d& dst0, const m128d& x0, const m128d& y0)
{
  dst0 = _mm_andnot_pd(x0, y0);
}

static FOG_INLINE void m128iOr(m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_or_si128(x0, y0);
}

static FOG_INLINE void m128dOr(m128d& dst0, const m128d& x0, const m128d& y0)
{
  dst0 = _mm_or_pd(x0, y0);
}

static FOG_INLINE void m128iXor(m128i& dst0, const m128i& x0, const m128i& y0)
{
  dst0 = _mm_xor_si128(x0, y0);
}

static FOG_INLINE void m128dXor(m128d& dst0, const m128d& x0, const m128d& y0)
{
  dst0 = _mm_xor_pd(x0, y0);
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
static FOG_INLINE void m128iLShiftPU16(m128i& dst0, const m128i& x0)
{
  dst0 = _mm_slli_epi16(x0, COUNT_BITS);
}

template<int COUNT_BITS>
static FOG_INLINE void m128iLShiftPU32(m128i& dst0, const m128i& x0)
{
  dst0 = _mm_slli_epi32(x0, COUNT_BITS);
}

template<int COUNT_BITS>
static FOG_INLINE void m128iLShiftPU64(m128i& dst0, const m128i& x0)
{
  dst0 = _mm_slli_epi64(x0, COUNT_BITS);
}

template<int COUNT_BITS>
static FOG_INLINE void m128iLShiftSU128(m128i& dst0, const m128i& x0)
{
  char COUNT_BITS_Must_Be_Divisible_By_8[COUNT_BITS % 8 == 0 ? 1 : -1];
  dst0 = _mm_slli_si128(x0, COUNT_BITS >> 3);
}

template<int COUNT_BITS>
static FOG_INLINE void m128iRShiftPU16(m128i& dst0, const m128i& x0)
{
  dst0 = _mm_srli_epi16(x0, COUNT_BITS);
}

template<int COUNT_BITS>
static FOG_INLINE void m128iRShiftPU32(m128i& dst0, const m128i& x0)
{
  dst0 = _mm_srli_epi32(x0, COUNT_BITS);
}

template<int COUNT_BITS>
static FOG_INLINE void m128iRShiftPU64(m128i& dst0, const m128i& x0)
{
  dst0 = _mm_srli_epi64(x0, COUNT_BITS);
}

template<int COUNT_BITS>
static FOG_INLINE void m128iRShiftSU128(m128i& dst0, const m128i& x0)
{
  char COUNT_BITS_Must_Be_Divisible_By_8[COUNT_BITS % 8 == 0 ? 1 : -1];
  dst0 = _mm_srli_si128(x0, COUNT_BITS >> 3);
}

template<int COUNT_BITS>
static FOG_INLINE void m128iRShiftPI16(m128i& dst0, const m128i& x0)
{
  dst0 = _mm_srai_epi16(x0, COUNT_BITS);
}

template<int COUNT_BITS>
static FOG_INLINE void m128iRShiftPI32(m128i& dst0, const m128i& x0)
{
  dst0 = _mm_srai_epi32(x0, COUNT_BITS);
}

// ============================================================================
// [Fog::Face - SSE2 - Negate255/256]
// ============================================================================

static FOG_INLINE void m128iNegate255PI8(m128i& dst0, const m128i& x0)
{
  dst0 = _mm_xor_si128(x0, FOG_XMM_GET_CONST_PI(FFFFFFFFFFFFFFFF_FFFFFFFFFFFFFFFF));
}

static FOG_INLINE void m128iNegate255PI16(m128i& dst0, const m128i& x0)
{
  dst0 = _mm_xor_si128(x0, FOG_XMM_GET_CONST_PI(00FF00FF00FF00FF_00FF00FF00FF00FF));
}

static FOG_INLINE void m128iNegate256PI16(m128i& dst0, const m128i& x0)
{
  dst0 = _mm_sub_epi16(FOG_XMM_GET_CONST_PI(0100010001000100_0100010001000100), x0);
}

static FOG_INLINE void m128iNegate65535PI16(m128i& dst0, const m128i& x0)
{
  dst0 = _mm_xor_si128(x0, FOG_XMM_GET_CONST_PI(FFFFFFFFFFFFFFFF_FFFFFFFFFFFFFFFF));
}

static FOG_INLINE void m128iNegate65535PI32(m128i& dst0, const m128i& x0)
{
  dst0 = _mm_xor_si128(x0, FOG_XMM_GET_CONST_PI(0000FFFF0000FFFF_0000FFFF0000FFFF));
}

static FOG_INLINE void m128iNegate65536PI32(m128i& dst0, const m128i& x0)
{
  dst0 = _mm_sub_epi32(FOG_XMM_GET_CONST_PI(0001000000010000_0001000000010000), x0);
}

// ============================================================================
// [Fog::Face - SSE2 - Compare]
// ============================================================================

static FOG_INLINE void m128iCmpEqPI8(m128i& dst0, const m128i& x0, const m128i& y0) { dst0 = _mm_cmpeq_epi8(x0, y0); }
static FOG_INLINE void m128iCmpEqPI16(m128i& dst0, const m128i& x0, const m128i& y0) { dst0 = _mm_cmpeq_epi16(x0, y0); }
static FOG_INLINE void m128iCmpEqPI32(m128i& dst0, const m128i& x0, const m128i& y0) { dst0 = _mm_cmpeq_epi32(x0, y0); }

static FOG_INLINE void m128iCmpGtPI8(m128i& dst0, const m128i& x0, const m128i& y0) { dst0 = _mm_cmpgt_epi8(x0, y0); }
static FOG_INLINE void m128iCmpGtPI16(m128i& dst0, const m128i& x0, const m128i& y0) { dst0 = _mm_cmpgt_epi16(x0, y0); }
static FOG_INLINE void m128iCmpGtPI32(m128i& dst0, const m128i& x0, const m128i& y0) { dst0 = _mm_cmpgt_epi32(x0, y0); }

static FOG_INLINE void m128iCmpLtPI8(m128i& dst0, const m128i& x0, const m128i& y0) { dst0 = _mm_cmplt_epi8(x0, y0); }
static FOG_INLINE void m128iCmpLtPI16(m128i& dst0, const m128i& x0, const m128i& y0) { dst0 = _mm_cmplt_epi16(x0, y0); }
static FOG_INLINE void m128iCmpLtPI32(m128i& dst0, const m128i& x0, const m128i& y0) { dst0 = _mm_cmplt_epi32(x0, y0); }

static FOG_INLINE void m128fCmpNgeSS(m128f& dst0, const m128f& x0, const m128f& y0) { dst0 = _mm_cmpnge_ss(x0, y0); }
static FOG_INLINE void m128fCmpNgePS(m128f& dst0, const m128f& x0, const m128f& y0) { dst0 = _mm_cmpnge_ps(x0, y0); }

static FOG_INLINE void m128fCmpOrderedSS(m128f& dst0, const m128f& x0, const m128f& y0) { dst0 = _mm_cmpord_ss(x0, y0); }
static FOG_INLINE void m128fCmpOrderedPS(m128f& dst0, const m128f& x0, const m128f& y0) { dst0 = _mm_cmpord_ps(x0, y0); }

static FOG_INLINE void m128fCmpUnorderedSS(m128f& dst0, const m128f& x0, const m128f& y0) { dst0 = _mm_cmpunord_ss(x0, y0); }
static FOG_INLINE void m128fCmpUnorderedPS(m128f& dst0, const m128f& x0, const m128f& y0) { dst0 = _mm_cmpunord_ps(x0, y0); }

static FOG_INLINE void m128dCmpEqSD(m128d& dst0, const m128d& x0, const m128d& y0) { dst0 = _mm_cmpeq_sd(x0, y0); }
static FOG_INLINE void m128dCmpEqPD(m128d& dst0, const m128d& x0, const m128d& y0) { dst0 = _mm_cmpeq_pd(x0, y0); }

static FOG_INLINE void m128dCmpGtSD(m128d& dst0, const m128d& x0, const m128d& y0) { dst0 = _mm_cmpgt_sd(x0, y0); }
static FOG_INLINE void m128dCmpGtPD(m128d& dst0, const m128d& x0, const m128d& y0) { dst0 = _mm_cmpgt_pd(x0, y0); }

static FOG_INLINE void m128dCmpGeSD(m128d& dst0, const m128d& x0, const m128d& y0) { dst0 = _mm_cmpge_sd(x0, y0); }
static FOG_INLINE void m128dCmpGePD(m128d& dst0, const m128d& x0, const m128d& y0) { dst0 = _mm_cmpge_pd(x0, y0);}

static FOG_INLINE void m128dCmpLtSD(m128d& dst0, const m128d& x0, const m128d& y0) { dst0 = _mm_cmplt_sd(x0, y0); }
static FOG_INLINE void m128dCmpLtPD(m128d& dst0, const m128d& x0, const m128d& y0) { dst0 = _mm_cmplt_pd(x0, y0); }

static FOG_INLINE void m128dCmpLeSD(m128d& dst0, const m128d& x0, const m128d& y0) { dst0 = _mm_cmple_sd(x0, y0); }
static FOG_INLINE void m128dCmpLePD(m128d& dst0, const m128d& x0, const m128d& y0) { dst0 = _mm_cmple_pd(x0, y0); }

static FOG_INLINE void m128dCmpNeqSD(m128d& dst0, const m128d& x0, const m128d& y0) { dst0 = _mm_cmpneq_sd(x0, y0); }
static FOG_INLINE void m128dCmpNeqPD(m128d& dst0, const m128d& x0, const m128d& y0) { dst0 = _mm_cmpneq_pd(x0, y0); }

static FOG_INLINE void m128dCmpNltSD(m128d& dst0, const m128d& x0, const m128d& y0) { dst0 = _mm_cmpnlt_sd(x0, y0); }
static FOG_INLINE void m128dCmpNltPD(m128d& dst0, const m128d& x0, const m128d& y0) { dst0 = _mm_cmpnlt_pd(x0, y0); }

static FOG_INLINE void m128dCmpNleSD(m128d& dst0, const m128d& x0, const m128d& y0) { dst0 = _mm_cmpnle_sd(x0, y0); }
static FOG_INLINE void m128dCmpNlePD(m128d& dst0, const m128d& x0, const m128d& y0) { dst0 = _mm_cmpnle_pd(x0, y0); }

static FOG_INLINE void m128dCmpNgtSD(m128d& dst0, const m128d& x0, const m128d& y0) { dst0 = _mm_cmpngt_sd(x0, y0); }
static FOG_INLINE void m128dCmpNgtPD(m128d& dst0, const m128d& x0, const m128d& y0) { dst0 = _mm_cmpngt_pd(x0, y0); }

// ============================================================================
// [Fog::Face - SSE2 - Epsilon]
// ============================================================================

static FOG_INLINE void m128dEpsilonSD(m128d& dst0, const m128d& x0)
{
  m128d sgn;
  sgn = FOG_XMM_GET_CONST_PD(m128d_sn_sn);
  sgn = _mm_and_pd(sgn, x0);

  dst0 = _mm_and_pd(x0, FOG_XMM_GET_CONST_PD(m128d_nm_nm));
  dst0 = _mm_max_sd(dst0, FOG_XMM_GET_CONST_PD(m128d_eps_eps));
  dst0 = _mm_or_pd(dst0, sgn);
}

static FOG_INLINE void m128dEpsilonPD(m128d& dst0, const m128d& x0)
{
  m128d sgn;
  sgn = FOG_XMM_GET_CONST_PD(m128d_sn_sn);
  sgn = _mm_and_pd(sgn, x0);

  dst0 = _mm_and_pd(x0, FOG_XMM_GET_CONST_PD(m128d_nm_nm));
  dst0 = _mm_max_pd(dst0, FOG_XMM_GET_CONST_PD(m128d_eps_eps));
  dst0 = _mm_or_pd(dst0, sgn);
}

// ============================================================================
// [Fog::Face - SSE2 - MoveMask]
// ============================================================================

//! @brief Create x0 16-bit mask from the most significant bits of the 16 signed
//! or unsigned 8-bit integers and zero extend the upper bits.
static FOG_INLINE void m128iMoveMaskPI8(int& dst0, const m128i& x0)
{
  dst0 = _mm_movemask_epi8(x0);
}

//! @brief Create x0 4-bit mask from the most significant bits of the 4 signed
//! or unsigned 32-bit integers and zero extend the upper bits.
static FOG_INLINE void m128iMoveMaskPI32(int& dst0, const m128i& x0)
{
  dst0 = _mm_movemask_ps(_mm_castsi128_ps(x0));
}

//! @brief Create x0 2-bit mask from the most significant bits of the 2 signed
//! or unsigned 64-bit integers and zero extend the upper bits.
static FOG_INLINE void m128iMoveMaskPI64(int& dst0, const m128i& x0)
{
  dst0 = _mm_movemask_pd(_mm_castsi128_pd(x0));
}

//! @brief Create x0 2-bit mask from the most significant bits of the four DP-FP
//! values.
static FOG_INLINE void m128dMoveMaskPD(int& dst0, const m128d& x0)
{
  dst0 = _mm_movemask_pd(x0);
}

// ============================================================================
// [Fog::Face - SSE2 - MulDiv255/256]
// ============================================================================

static FOG_INLINE void m128iMulDiv255PI16(
  __m128i& dst0, const __m128i& x0, const __m128i& y0)
{
  dst0 = _mm_mullo_epi16(x0, y0);
  dst0 = _mm_adds_epu16(dst0, FOG_XMM_GET_CONST_PI(0080008000800080_0080008000800080));
  dst0 = _mm_mulhi_epu16(dst0, FOG_XMM_GET_CONST_PI(0101010101010101_0101010101010101));
}

static FOG_INLINE void m128iMulDiv255PI16_2x(
  __m128i& dst0, const __m128i& x0, const __m128i& y0,
  __m128i& dst1, const __m128i& x1, const __m128i& y1)
{
  dst0 = _mm_mullo_epi16(x0, y0);
  dst1 = _mm_mullo_epi16(x1, y1);

  dst0 = _mm_adds_epu16(dst0, FOG_XMM_GET_CONST_PI(0080008000800080_0080008000800080));
  dst1 = _mm_adds_epu16(dst1, FOG_XMM_GET_CONST_PI(0080008000800080_0080008000800080));

  dst0 = _mm_mulhi_epu16(dst0, FOG_XMM_GET_CONST_PI(0101010101010101_0101010101010101));
  dst1 = _mm_mulhi_epu16(dst1, FOG_XMM_GET_CONST_PI(0101010101010101_0101010101010101));
}

static FOG_INLINE void m128iMulDiv256PI16(
  __m128i& dst0, const __m128i& x0, const __m128i& y0)
{
  dst0 = _mm_mullo_epi16(x0, y0);
  dst0 = _mm_slli_epi16(dst0, 8);
}

static FOG_INLINE void m128iMulDiv256PI16(
  __m128i& dst0, const __m128i& x0, const __m128i& y0,
  __m128i& dst1, const __m128i& x1, const __m128i& y1)
{
  dst0 = _mm_mullo_epi16(x0, y0);
  dst1 = _mm_mullo_epi16(x1, y1);

  dst0 = _mm_slli_epi16(dst0, 8);
  dst1 = _mm_slli_epi16(dst1, 8);
}

// ============================================================================
// [Fog::Face - SSE2 - Misc]
// ============================================================================

template<int INDEX_W>
static FOG_INLINE void m128iInsertPI16(
  __m128i& dst0, const __m128i& x0, const int& y0)
{
  dst0 = _mm_insert_epi16(x0, y0, INDEX_W);
}

template<int INDEX_W>
static FOG_INLINE void m128iExtractPI16(
  int& dst0, const __m128i& x0)
{
  dst0 = _mm_extract_epi16(x0, INDEX_W);
}

//! @}

} // Face namespace
} // Fog namespace

// MMX+SSE2 extensions require both, Face_MMX.h, and Face_SSE2.h headers.
#if defined(_FOG_CORE_FACE_FACEMMX_H)
# include <Fog/Core/Face/FaceMMXSSE2.h>
#endif // _FOG_CORE_FACE_FACEMMX_H

// [Guard]
#endif // _FOG_CORE_FACE_FACESSE2_H
