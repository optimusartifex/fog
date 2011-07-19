// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_FACE_FACE_C_F32_H
#define _FOG_CORE_FACE_FACE_C_F32_H

// [Dependencies]
#include <Fog/Core/Face/Constants.h>
#include <Fog/Core/Face/Features.h>
#include <Fog/Core/Face/Face_C_P32.h>
#include <Fog/Core/Global/Global.h>

namespace Fog {
namespace Face {

//! @addtogroup Fog_Face_C
//! @{

// ============================================================================
// [Fog::Face - F32 - Types]
// ============================================================================

//! @typedef f32
//! @brief 32-bit floating point used by Fog::Face framework.

typedef float f32;

// ============================================================================
// [Fog::Face - F32 - Load / Store]
// ============================================================================

static FOG_INLINE void f32Load1(f32& dst0, const void* srcp)
{
  dst0 = ((const float*)srcp)[0];
}

static FOG_INLINE void f32Load2(f32& dst0, f32& dst1, const void* srcp)
{
  dst0 = ((const float*)srcp)[0];
  dst1 = ((const float*)srcp)[1];
}

static FOG_INLINE void f32Load3(f32& dst0, f32& dst1, f32& dst2, const void* srcp)
{
  dst0 = ((const float*)srcp)[0];
  dst1 = ((const float*)srcp)[1];
  dst2 = ((const float*)srcp)[2];
}

static FOG_INLINE void f32Load4(f32& dst0, f32& dst1, f32& dst2, f32& dst3, const void* srcp)
{
  dst0 = ((const float*)srcp)[0];
  dst1 = ((const float*)srcp)[1];
  dst2 = ((const float*)srcp)[2];
  dst3 = ((const float*)srcp)[3];
}

static FOG_INLINE void f32Store1(void* dstp, const f32& src0)
{
  ((float*)dstp)[0] = src0;
}

static FOG_INLINE void f32Store2(void* dstp, const f32& src0, const f32& src1)
{
  ((float*)dstp)[0] = src0;
  ((float*)dstp)[1] = src1;
}

static FOG_INLINE void f32Store3(void* dstp, const f32& src0, const f32& src1, const f32& src2)
{
  ((float*)dstp)[0] = src0;
  ((float*)dstp)[1] = src1;
  ((float*)dstp)[2] = src2;
}

static FOG_INLINE void f32Store4(void* dstp, const f32& src0, const f32& src1, const f32& src2, const f32& src3)
{
  ((float*)dstp)[0] = src0;
  ((float*)dstp)[1] = src1;
  ((float*)dstp)[2] = src2;
  ((float*)dstp)[3] = src3;
}

// ============================================================================
// [Fog::Face - F32 - Copy]
// ============================================================================

//! @brief Copy @a x0 to @a dst0.
//!
//! @verbatim
//! dst0.f32[0] = x0.f32[0]
//! @endverbatim
static FOG_INLINE void f32Copy(
  f32& dst0, const f32& x0)
{
  dst0 = x0;
}

//! @brief Copy @a x0/x1 to @a dst0/dst1.
//!
//! @verbatim
//! dst0.f32[0] = x0.f32[0]
//! dst1.f32[0] = x1.f32[0]
//! @endverbatim
static FOG_INLINE void f32Copy_2x(
  f32& dst0, const f32& x0,
  f32& dst1, const f32& x1)
{
  dst0 = x0;
  dst1 = x1;
}

// ============================================================================
// [Fog::Face - F32 - Add]
// ============================================================================

static FOG_INLINE void f32Add(f32& dst0, const f32& x0, const f32& y0)
{
  dst0 = x0 + y0;
}

// ============================================================================
// [Fog::Face - F32 - Sub]
// ============================================================================

static FOG_INLINE void f32Sub(f32& dst0, const f32& x0, const f32& y0)
{
  dst0 = x0 - y0;
}

// ============================================================================
// [Fog::Face - F32 - Mul]
// ============================================================================

static FOG_INLINE void f32Mul(f32& dst0, const f32& x0, const f32& y0)
{
  dst0 = x0 * y0;
}

// ============================================================================
// [Fog::Face - F32 - Div]
// ============================================================================

static FOG_INLINE void f32Div(f32& dst0, const f32& x0, const f32& y0)
{
  dst0 = x0 / y0;
}

// ============================================================================
// [Fog::Face - Cvt]
// ============================================================================

// To prevent rounding issues when converting from float into 16-bit value
// we use larger scale and then the value is shifted right (discarding the
// scale and rounding errors). The bahavior of this functions should be
// the same as when converting 16-bit component to 8-bit component.

static FOG_INLINE void f32CvtU8FromFX(p32& dst0, const f32& x0)
{
  dst0 = (uint32_t)( (int32_t)(x0 * (float)(0xFFFFFF)) ) >> 16;
}

static FOG_INLINE void f32CvtU8_PBB0_FromFX(p32& dst0, const f32& x0)
{
  dst0 = (uint32_t)( (int32_t)(x0 * (float)(0xFFFFFF)) ) >> 16;
}

static FOG_INLINE void f32CvtU8_PBB1_FromFX(p32& dst0, const f32& x0)
{
  dst0 = (uint32_t)( (int32_t)(x0 * (float)(0xFFFFFF)) ) >> 8;
  dst0 &= 0x0000FF00U;
}

static FOG_INLINE void f32CvtU8_PBB2_FromFX(p32& dst0, const f32& x0)
{
  dst0 = (uint32_t)( (int32_t)(x0 * (float)(0xFFFFFF)) );
  dst0 &= 0x00FF0000U;
}

static FOG_INLINE void f32CvtU8_PBB3_FromFX(p32& dst0, const f32& x0)
{
  dst0 = (uint32_t)( (int32_t)(x0 * (float)(0xFFFFFF)) ) << 8;
  dst0 &= 0xFF000000U;
}


static FOG_INLINE void f32CvtU16FromFX(p32& dst0, const f32& x0)
{
  dst0 = (uint32_t)( (int32_t)(x0 * (float)(0xFFFFFF)) ) >> 8;
}

static FOG_INLINE void f32CvtU16_PWW0_FromFX(p32& dst0, const f32& x0)
{
  dst0 = (uint32_t)( (int32_t)(x0 * (float)(0xFFFFFF)) ) >> 8;
}

static FOG_INLINE void f32CvtU16_PWW1_FromFX(p32& dst0, const f32& x0)
{
  dst0 = (uint32_t)( (int32_t)(x0 * (float)(0xFFFFFF)) ) << 8;
  dst0 &= 0xFFFF0000U;
}



static FOG_INLINE void f32CvtFXFromU8(f32& dst0, const p32& x0)
{
  dst0 = (float)x0 * (1.0f / 255.0f);
}

static FOG_INLINE void f32CvtFXFromU16(f32& dst0, const p32& x0)
{
  dst0 = (float)x0 * (1.0f / 65535.0f);
}



static FOG_INLINE uint32_t f32CvtU8FromFX(const f32& x0)
{
  p32 dst0;
  f32CvtU8FromFX(dst0, x0);
  return dst0;
}

static FOG_INLINE uint32_t f32CvtU16FromFX(const f32& x0)
{
  p32 dst0;
  f32CvtU16FromFX(dst0, x0);
  return dst0;
}

static FOG_INLINE float f32CvtFXFromU8(const p32& x0)
{
  float dst0;
  f32CvtFXFromU8(dst0, x0);
  return dst0;
}

static FOG_INLINE float f32CvtFXFromU16(const p32& x0)
{
  float dst0;
  f32CvtFXFromU16(dst0, x0);
  return dst0;
}

// ============================================================================
// [Fog::Face - LoadAs / StoreAs]
// ============================================================================

static FOG_INLINE void f32LoadU8AsFX(f32& dst0, const void* srcp)
{
  dst0 = (float)((int)((uint8_t*)srcp)[0]) * (1.0f/255.0f);
}

static FOG_INLINE void f32LoadU16AsFX(f32& dst0, const void* srcp)
{
  dst0 = (float)((int)((uint16_t*)srcp)[0]) * (1.0f/65535.0f);
}

static FOG_INLINE void f32StoreU8AsFX(void* dstp, f32& src0)
{
  p32 u0;

  f32CvtU8FromFX(u0, src0);
  p32Store1b(dstp, u0);
}

static FOG_INLINE void f32StoreU16AsFX(void* dstp, f32& src0)
{
  p32 u0;

  f32CvtU16FromFX(u0, src0);
  p32Store2aNative(dstp, u0);
}

//! @}

} // Face namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_FACE_FACE_C_F32_H
