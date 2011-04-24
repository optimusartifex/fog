// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_FACE_FACE_SSE2_H
#define _FOG_CORE_FACE_FACE_SSE2_H

// [Dependencies]
#include <Fog/Core/Cpu/Intrin_SSE2.h>
#include <Fog/Core/Face/Face_SSE.h>
#include <Fog/Core/Face/Features.h>
#include <Fog/Core/Math/Constants.h>
#include <Fog/Core/Math/Math.h>

// ============================================================================
// [Fog::Face - SSE2 - Constants]
// ============================================================================

FOG_SSE_DECLARE_CONST_PI16_VAR(0000000000000000_00000000FF000000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFF00, 0x0000);
FOG_SSE_DECLARE_CONST_PI16_VAR(0000000000000000_00000000FFFFFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF);
FOG_SSE_DECLARE_CONST_PI16_VAR(0000000000000000_00FF000000000000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00FF, 0x0000, 0x0000, 0x0000);
FOG_SSE_DECLARE_CONST_PI16_VAR(0000000000000000_00FF00FF00FF00FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x00FF, 0x00FF, 0x00FF, 0x00FF);
FOG_SSE_DECLARE_CONST_PI16_VAR(0000000000000000_0101010101010101, 0x0000, 0x0000, 0x0000, 0x0000, 0x0101, 0x0101, 0x0101, 0x0101);
FOG_SSE_DECLARE_CONST_PI16_VAR(0000000000000000_FF000000FF000000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFF00, 0x0000, 0xFF00, 0x0000);
FOG_SSE_DECLARE_CONST_PI16_VAR(0000000000000000_FFFFFFFFFFFFFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF);

FOG_SSE_DECLARE_CONST_PI16_VAR(00000000000000FF_00000000000000FF, 0x0000, 0x0000, 0x0000, 0x00FF, 0x0000, 0x0000, 0x0000, 0x00FF);
FOG_SSE_DECLARE_CONST_PI16_VAR(0080008000800080_0080008000800080, 0x0080, 0x0080, 0x0080, 0x0080, 0x0080, 0x0080, 0x0080, 0x0080);
FOG_SSE_DECLARE_CONST_PI16_VAR(000000FF000000FF_000000FF000000FF, 0x0000, 0x00FF, 0x0000, 0x00FF, 0x0000, 0x00FF, 0x0000, 0x00FF);
FOG_SSE_DECLARE_CONST_PI16_VAR(000000FF00FF00FF_000000FF00FF00FF, 0x0000, 0x00FF, 0x00FF, 0x00FF, 0x0000, 0x00FF, 0x00FF, 0x00FF);
FOG_SSE_DECLARE_CONST_PI16_VAR(000000FF00FF00FF_00FF00FF00FF00FF, 0x0000, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF);
FOG_SSE_DECLARE_CONST_PI16_VAR(0001000200020002_0001000200020002, 0x0001, 0x0002, 0x0002, 0x0002, 0x0001, 0x0002, 0x0002, 0x0002);
FOG_SSE_DECLARE_CONST_PI16_VAR(00FF000000000000_0000000000000000, 0x00FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000);
FOG_SSE_DECLARE_CONST_PI16_VAR(00FF000000000000_00FF000000000000, 0x00FF, 0x0000, 0x0000, 0x0000, 0x00FF, 0x0000, 0x0000, 0x0000);
FOG_SSE_DECLARE_CONST_PI16_VAR(00FF00FF00FF00FF_0000000000000000, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x0000, 0x0000, 0x0000, 0x0000);
FOG_SSE_DECLARE_CONST_PI16_VAR(00FF00FF00FF00FF_000000FF00FF00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x0000, 0x00FF, 0x00FF, 0x00FF);
FOG_SSE_DECLARE_CONST_PI16_VAR(00FF00FF00FF00FF_00FF00FF00FF00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF);
FOG_SSE_DECLARE_CONST_PI16_VAR(0101010101010101_0101010101010101, 0x0101, 0x0101, 0x0101, 0x0101, 0x0101, 0x0101, 0x0101, 0x0101);
FOG_SSE_DECLARE_CONST_PI16_VAR(FF000000000000FF_FF000000000000FF, 0xFF00, 0x0000, 0x0000, 0x00FF, 0xFF00, 0x0000, 0x0000, 0x00FF);
FOG_SSE_DECLARE_CONST_PI16_VAR(FF000000FF000000_FF000000FF000000, 0xFF00, 0x0000, 0xFF00, 0x0000, 0xFF00, 0x0000, 0xFF00, 0x0000);
FOG_SSE_DECLARE_CONST_PI16_VAR(FF00FF00FF00FF00_FF00FF00FF00FF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00);
FOG_SSE_DECLARE_CONST_PI16_VAR(FFFFFFFFFFFFFFFF_0000000000000000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000);
FOG_SSE_DECLARE_CONST_PI16_VAR(FFFFFFFFFFFFFFFF_FFFFFFFFFFFFFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF);
FOG_SSE_DECLARE_CONST_PI16_VAR(00FFFFFF00FFFFFF_00FFFFFF00FFFFFF, 0x00FF, 0xFFFF, 0x00FF, 0xFFFF, 0x00FF, 0xFFFF, 0x00FF, 0xFFFF);
FOG_SSE_DECLARE_CONST_PI16_VAR(0000000100000001_0000000100000001, 0x0000, 0x0001, 0x0000, 0x0001, 0x0000, 0x0001, 0x0000, 0x0001);

FOG_SSE_DECLARE_CONST_PI64_SET(m128d_sgn_mask    , FOG_UINT64_C(0x8000000000000000));
FOG_SSE_DECLARE_CONST_PI64_SET(m128d_num_mask    , FOG_UINT64_C(0x7FFFFFFFFFFFFFFF));

FOG_SSE_DECLARE_CONST_PD_SET  (m128d_one         , 1.0                             );
FOG_SSE_DECLARE_CONST_PD_VAR  (m128d_0f_1f       , 0.0, 1.0                        );
FOG_SSE_DECLARE_CONST_PD_SET  (m128d_epsilon     , Fog::MATH_EPSILON_D             );

FOG_SSE_DECLARE_CONST_PD_SET  (m128d_from_byte   , 1.0 / 255.0);
FOG_SSE_DECLARE_CONST_PD_SET  (m128d_from_word   , 1.0 / 65535.0);

FOG_SSE_DECLARE_CONST_PD_SET  (m128d_to_byte     , 255.0);
FOG_SSE_DECLARE_CONST_PD_SET  (m128d_to_word     , 65535.0);

// A constant that is subtracted from the U32 value and added back to U16 value
// to simulate unsigned saturation (this instruction is missing).
FOG_SSE_DECLARE_CONST_PI32_SET(m128i_packusdw_u32, 0x8000);
FOG_SSE_DECLARE_CONST_PI16_SET(m128i_packusdw_u16, 0x8000);

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

static FOG_INLINE void m128fCvtPSFromPI32(m128f& dst0, const m128i& src0)
{
  dst0 = _mm_cvtepi32_ps(src0);
}

static FOG_INLINE void m128dCvtPDFromPI32(m128d& dst0, const m128i& src0)
{
  dst0 = _mm_cvtepi32_pd(src0);
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

static FOG_INLINE void m128iUnpackPI64FromPI32Hi(m128i& dst0, const m128i& src0, const m128i& src1)
{
  dst0 = _mm_unpackhi_epi32(src0, src1);
}

static FOG_INLINE void m128dUnpackLo(m128d& dst, const m128d& a, const m128d& b)
{
  dst = _mm_unpacklo_pd(a, b);
}

static FOG_INLINE void m128dUnpackHi(m128d& dst, const m128d& a, const m128d& b)
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
  dst0 = _mm_sub_epi32(src0, FOG_SSE_GET_CONST_PI(m128i_packusdw_u32));
  dst0 = _mm_packs_epi32(dst0, dst0);
  dst0 = _mm_add_epi16(dst0, FOG_SSE_GET_CONST_PI(m128i_packusdw_u16));
}

static FOG_INLINE void m128iPackPU16FromPI32(m128i& dst0, const m128i& src0, const m128i& src1)
{
  dst0 = _mm_packs_epi32(
    _mm_sub_epi32(src0, FOG_SSE_GET_CONST_PI(m128i_packusdw_u32)),
    _mm_sub_epi32(src1, FOG_SSE_GET_CONST_PI(m128i_packusdw_u32)));
  dst0 = _mm_add_epi16(dst0, FOG_SSE_GET_CONST_PI(m128i_packusdw_u16));
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

// ============================================================================
// [Fog::Face - SSE2 - Epsilon]
// ============================================================================

static FOG_INLINE void m128dEpsilonSD(m128d& dst, const m128d& a)
{
  m128d sgn;
  sgn = FOG_SSE_GET_CONST_PD(m128d_sgn_mask);
  sgn = _mm_and_pd(sgn, a);

  dst = _mm_and_pd(a, FOG_SSE_GET_CONST_PD(m128d_num_mask));
  dst = _mm_max_sd(dst, FOG_SSE_GET_CONST_PD(m128d_epsilon));
  dst = _mm_or_pd(dst, sgn);
}

static FOG_INLINE void m128dEpsilonPD(m128d& dst, const m128d& a)
{
  m128d sgn;
  sgn = FOG_SSE_GET_CONST_PD(m128d_sgn_mask);
  sgn = _mm_and_pd(sgn, a);

  dst = _mm_and_pd(a, FOG_SSE_GET_CONST_PD(m128d_num_mask));
  dst = _mm_max_pd(dst, FOG_SSE_GET_CONST_PD(m128d_epsilon));
  dst = _mm_or_pd(dst, sgn);
}

// ============================================================================
// [Fog::Face - SSE2 - MoveMask]
// ============================================================================

//! @brief Create a 16-bit mask from the most significant bits of the 16 signed
//! or unsigned 8-bit integers in a and zero extend the upper bits.
static FOG_INLINE void m128iMoveMask8(int& dst, const m128i& a)
{
  dst = _mm_movemask_epi8(a);
}

//! @brief Create a 2-bit mask from the most significant bits of the four DP-FP values.
static FOG_INLINE void m128dMoveMask(int& dst, const m128d& a)
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
  dst0 = _mm_adds_epu16(dst0, FOG_SSE_GET_CONST_PI(0080008000800080_0080008000800080));
  dst0 = _mm_mulhi_epu16(dst0, FOG_SSE_GET_CONST_PI(0101010101010101_0101010101010101));
}

// ============================================================================
// [Fog::Face - SSE2 - Shift]
// ============================================================================

template<int COUNT_BITS>
static FOG_INLINE void m128iRShiftArithPI16(
  __m128i& dst0, const __m128i& a0)
{
  dst0 = _mm_srai_epi16(a0, COUNT_BITS);
}

template<int COUNT_BITS>
static FOG_INLINE void m128iRShiftArithPI32(
  __m128i& dst0, const __m128i& a0)
{
  dst0 = _mm_srai_epi32(a0, COUNT_BITS);
}

template<int COUNT_BITS>
static FOG_INLINE void m128iRShiftLogPI16(
  __m128i& dst0, const __m128i& a0)
{
  dst0 = _mm_srli_epi16(a0, COUNT_BITS);
}

template<int COUNT_BITS>
static FOG_INLINE void m128iRShiftLogPI32(
  __m128i& dst0, const __m128i& a0)
{
  dst0 = _mm_srli_epi32(a0, COUNT_BITS);
}

template<int COUNT_BITS>
static FOG_INLINE void m128iRShiftLogPI64(
  __m128i& dst0, const __m128i& a0)
{
  dst0 = _mm_srli_epi64(a0, COUNT_BITS);
}

template<int COUNT_BYTES>
static FOG_INLINE void m128iRShiftLogSI128(
  __m128i& dst0, const __m128i& a0)
{
  dst0 = _mm_srli_si128(a0, COUNT_BYTES);
}

template<int COUNT_BITS>
static FOG_INLINE void m128iLShiftPI16(
  __m128i& dst0, const __m128i& a0)
{
  dst0 = _mm_slli_epi16(a0, COUNT_BITS);
}

template<int COUNT_BITS>
static FOG_INLINE void m128iLShiftPI32(
  __m128i& dst0, const __m128i& a0)
{
  dst0 = _mm_slli_epi32(a0, COUNT_BITS);
}

template<int COUNT_BITS>
static FOG_INLINE void m128iLShiftPI64(
  __m128i& dst0, const __m128i& a0)
{
  dst0 = _mm_slli_epi64(a0, COUNT_BITS);
}

template<int COUNT_BYTES>
static FOG_INLINE void m128iLShiftSI128(
  __m128i& dst0, const __m128i& a0)
{
  dst0 = _mm_slli_si128(a0, COUNT_BYTES);
}

//! @}

} // Face namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_FACE_FACE_SSE2_H
