// [Fog-Graphics library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_BYTEUTIL_P_H
#define _FOG_GRAPHICS_BYTEUTIL_P_H

// [Dependencies]
#include <Fog/Build/Build.h>
#include <Fog/Graphics/Constants.h>

//! @addtogroup Fog_Graphics_Private
//! @{

namespace Fog {

//! @brief Helper typedefs and methods to work with bytes packed in 32-bit or
//! 64-bit integers.
//!
//! @internal.
namespace ByteUtil {

// ============================================================================
// [Fog::ByteUtil - Div255/256]
// ============================================================================

//! @brief Accurate division of 255. The result is equal to <code>(val / 255.0)</code>.
//!
//! @note This template is used for accure alpha blends.
template<typename T>
static FOG_INLINE T div255(T i)
{
  return (T)( (((uint32_t)i << 8U) + ((uint32_t)i + 256U)) >> 16U );
}

//! @brief Accurate division of 256. The result is equal to <code>(val / 256.0)</code>.
//!
//! @not This template is simply implemented as a right shift by 8 bits.
template<typename T>
static FOG_INLINE T div256(T i)
{
  return (T)(i >> 8U);
}

// ============================================================================
// [Fog::ByteUtil - Scalar-Byte OPS]
// ============================================================================

// Result = (x * y) / 255
static FOG_INLINE uint32_t scalar_muldiv255(uint32_t x, uint32_t y)
{
  x *= y;
  x = ((x + (x >> 8) + 0x80) >> 8);
  return x;
}

// Result = (x * y) / 256
static FOG_INLINE uint32_t scalar_muldiv256(uint32_t x, uint32_t y)
{
  return (x * y) >> 8;
}

// Result = {(x * a) + (y * (255 - a))} / 255
static FOG_INLINE uint32_t scalar_lerp255(uint32_t x, uint32_t y, uint32_t a)
{
  x *= a;
  y *= a ^ 0xFF;
  x += y;
  x = ((x + (x >> 8) + 0x80) >> 8);
  return x;
}

// Result = {(x * a) + (y * (256 - a))} / 256
static FOG_INLINE uint32_t scalar_lerp256(uint32_t x, uint32_t y, uint32_t a)
{
  x *= a;
  y *= 256 - a;
  return (x + y) >> 8;
}

// Result = 255 - x
static FOG_INLINE uint32_t scalar_neg255(uint32_t x)
{
  return x ^ 0xFF;
}

// Result = 256 - x
static FOG_INLINE uint32_t scalar_neg256(uint32_t x)
{
  return 256 - x;
}

// Result = saturate(x + y)
static FOG_INLINE uint32_t scalar_addus(uint32_t x, uint32_t y)
{
  x += y;
  x |= 0x0100U - ((x >> 8) & 0x00FFU);
  x &= 0x00FFU;
  return x;
}

// Result = x | (x << 8) | (x << 16) | (x << 24)
static FOG_INLINE uint32_t scalar_expand(uint32_t x)
{
  x |= (x << 8);
  x |= (x << 16);
  return x;
}

// ============================================================================
// [Fog::ByteUtil - Byte1x2 - 32-Bit Integer SIMD Library]
// ============================================================================

// These function allows to do SIMD with classic 32 bit integers.
//
// Terminology:
// - byte1x2 - unsigned 32 bit integer that contains two bytes at mask
//             BYTE_1x2MASK that is 0x00FF00FF.
// - byte2x2 - two byte1x2 values
// - u       - scalar at range 0 to 255 (or 0 to 256 depending to the context).
//
// Pixels are usually encoded in byte2x2 using this formula:
// - byte1x2 b0 (0x00RR00BB)
// - byte1x2 b1 (0x00AA00GG);

typedef uint32_t byte1x2;

static const uint32_t BYTE_1x1MASK          = 0x000000FFU;
static const uint32_t BYTE_1x2MASK          = 0x00FF00FFU;
static const uint32_t BYTE_1x1HALF          = 0x00000080U;
static const uint32_t BYTE_1x2HALF          = 0x00800080U;
static const uint32_t BYTE_1x1HALF_PLUS_ONE = 0x00000100U;
static const uint32_t BYTE_1x2MASK_PLUS_ONE = 0x10000100U;

// ============================================================================
// [Fog::ByteUtil - Byte1x2 - From / To / Packing / Unpacking]
// ============================================================================

static FOG_INLINE byte1x2 byte1x2_from_u(uint32_t a0)
{
  return a0 | (a0 << 16);
}

static FOG_INLINE uint32_t byte1x2_lo(byte1x2 a0)
{
  return (a0 & 0xFF);
}

static FOG_INLINE uint32_t byte1x2_hi(byte1x2 a0)
{
  return (a0 >> 16);
}

//! @brief Pack x0 and x1 into single DWORD.
static FOG_INLINE void byte2x2_pack_0213(
  byte1x2& dst0, byte1x2 a0, byte1x2 a1)
{
  dst0 = a0 | (a1 << 8);
}

//! @brief Unpack 0st and 2nd BYTE of single DWORD into one byte1x2 value.
static FOG_INLINE void byte2x2_unpack_02(
  byte1x2& dst0, uint32_t a0)
{
  dst0 = (a0 & BYTE_1x2MASK);
}

//! @brief Unpack 1st and 3rd BYTE of single DWORD into one byte1x2 value.
static FOG_INLINE void byte2x2_unpack_13(
  byte1x2& dst0, uint32_t a0)
{
  dst0 = (a0 >> 8) & BYTE_1x2MASK;
}

//! @brief Unpack all bytes of DWORD into two byte1x2 values.
static FOG_INLINE void byte2x2_unpack_0213(
  byte1x2& dst0,
  byte1x2& dst1, uint32_t a0)
{
  dst0 = a0;
  dst1 = a0 >> 8;
  dst0 &= BYTE_1x2MASK;
  dst1 &= BYTE_1x2MASK;
}

//! @brief Unpack '021' bytes of DWORD into two byte1x2 values.
static FOG_INLINE void byte2x2_unpack_021X(
  byte1x2& dst0,
  byte1x2& dst1, uint32_t a0)
{
  dst0 = a0;
  dst1 = a0 >> 8;
  dst0 &= BYTE_1x2MASK;
  dst1 &= BYTE_1x1MASK;
}

// ============================================================================
// [Fog::ByteUtil - Byte1x2 - Copy]
// ============================================================================

// dst0 = a0;
static FOG_INLINE void byte1x2_copy(
  byte1x2& dst0, byte1x2 a0)
{
  dst0 = a0;
}

// dst0 = a0;
// dst1 = a1;
static FOG_INLINE void byte2x2_copy(
  byte1x2& dst0, byte1x2 a0,
  byte1x2& dst1, byte1x2 a1)
{
  dst0 = a0;
  dst1 = a1;
}

// ============================================================================
// [Fog::ByteUtil - Byte1x2 - Zero / Fill]
// ============================================================================

static FOG_INLINE void byte1x2_zero_lo(
  byte1x2& dst0, byte1x2 a0)
{
  dst0 = a0 & 0x00FF0000;
}

static FOG_INLINE void byte1x2_zero_hi(
  byte1x2& dst0, byte1x2 a0)
{
  dst0 = a0 & 0x000000FF;
}

static FOG_INLINE void byte1x2_fill_lo(
  byte1x2& dst0, byte1x2 a0)
{
  dst0 = a0 | 0x000000FF;
}

static FOG_INLINE void byte1x2_fill_hi(
  byte1x2& dst0, byte1x2 a0)
{
  dst0 = a0 | 0x00FF0000;
}

// ============================================================================
// [Fog::ByteUtil - Byte1x2 - Set]
// ============================================================================

static FOG_INLINE void byte1x2_set_lo(
  byte1x2& dst0, byte1x2 a0, uint32_t u)
{
  dst0 = (a0 & 0x00FF0000) | u;
}

static FOG_INLINE void byte1x2_set_hi(
  byte1x2& dst0, byte1x2 a0, uint32_t u)
{
  dst0 = (a0 & 0x000000FF) | (u << 16);
}

// ============================================================================
// [Fog::ByteUtil - Byte1x2 - Expand]
// ============================================================================

// dst0 = [src0.lo, src0.lo]
static FOG_INLINE void byte1x2_expand_lo(
  byte1x2& dst0, byte1x2 src0)
{
  dst0 = (src0 << 16);
  dst0 |= (src0 & 0x000000FF);
}

// dst0 = [src0.lo, src0.lo]
// dst1 = [src0.lo, src0.lo]
static FOG_INLINE void byte2x2_expand_lo(
  byte1x2& dst0,
  byte1x2& dst1, byte1x2 src0)
{
  byte1x2_expand_lo(dst0, src0);
  dst1 = dst0;
}

// dst0 = [src0.hi, src0.hi]
static FOG_INLINE void byte1x2_expand_hi(
  byte1x2& dst0, byte1x2 src0)
{
  dst0 = (src0 >> 16);
  dst0 |= (src0 & 0x00FF0000);
}

// dst0 = [src0.hi, src0.hi]
// dst1 = [src0.hi, src0.hi]
static FOG_INLINE void byte2x2_expand_hi(
  byte1x2& dst0,
  byte1x2& dst1, byte1x2 src0)
{
  byte1x2_expand_hi(dst0, src0);
  dst1 = dst0;
}

// ============================================================================
// [Fog::ByteUtil - Byte1x2 - Saturate]
// ============================================================================

// dst0 = min(dst0, 255)
static FOG_INLINE void byte1x2_sat(
  byte1x2& dst0)
{
  dst0 |= BYTE_1x2MASK_PLUS_ONE - ((dst0 >> 8) & BYTE_1x2MASK);

  dst0 &= 0x00FF00FF;
}

// dst0 = min(dst0, 255)
// dst1 = min(dst1, 255)
static FOG_INLINE void byte2x2_sat(
  byte1x2& dst0, byte1x2& dst1)
{
  dst0 |= BYTE_1x2MASK_PLUS_ONE - ((dst0 >> 8) & BYTE_1x2MASK);
  dst1 |= BYTE_1x2MASK_PLUS_ONE - ((dst1 >> 8) & BYTE_1x2MASK);

  dst0 &= 0x00FF00FF;
  dst1 &= 0x00FF00FF;
}

// ============================================================================
// [Fog::ByteUtil - Byte1x2 - Add / Sub]
// ============================================================================

// dst0 = a0 + u
static FOG_INLINE void byte1x2_add_u(
  byte1x2& dst0, byte1x2 a0, uint32_t u)
{
  u |= u << 16;

  dst0 = a0 + u;
}

// dst0 = a0 + u
// dst1 = a1 + u
static FOG_INLINE void byte2x2_add_u(
  byte1x2& dst0, byte1x2 a0,
  byte1x2& dst1, byte1x2 a1, uint32_t u)
{
  u |= u << 16;

  dst0 = a0 + u;
  dst1 = a1 + u;
}

// dst0 = a0 + b0
static FOG_INLINE void byte1x2_add_byte1x2(
  byte1x2& dst0, byte1x2 a0, byte1x2 b0)
{
  dst0 = a0 + b0;
}

// dst0 = a0 + b0
// dst1 = a1 + b1
static FOG_INLINE void byte2x2_add_byte2x2(
  byte1x2& dst0, byte1x2 a0, byte1x2 b0,
  byte1x2& dst1, byte1x2 a1, byte1x2 b1)
{
  dst0 = a0 + b0;
  dst1 = a1 + b1;
}

// dst0 = min(a0 + u, 255)
static FOG_INLINE void byte1x2_addus_u(
  byte1x2& dst0, byte1x2 a0, uint32_t u)
{
  u |= u << 16;

  dst0 = a0 + u;
  byte1x2_sat(dst0);
}

// dst0 = min(a0 + u, 255)
// dst1 = min(a1 + u, 255)
static FOG_INLINE void byte2x2_addus_u(
  byte1x2& dst0, byte1x2 a0,
  byte1x2& dst1, byte1x2 a1, uint32_t u)
{
  u |= u << 16;

  dst0 = a0 + u;
  dst1 = a1 + u;
  byte2x2_sat(dst0, dst1);
}

// dst0 = min(a0 + b0, 255)
static FOG_INLINE void byte1x2_addus_byte1x2(
  byte1x2& dst0, byte1x2 a0, byte1x2 b0)
{
  dst0 = a0 + b0;
  byte1x2_sat(dst0);
}

// dst0 = min(a0 + b0, 255)
// dst1 = min(a1 + b1, 255)
static FOG_INLINE void byte2x2_addus_byte2x2(
  byte1x2& dst0, byte1x2 a0, byte1x2 b0,
  byte1x2& dst1, byte1x2 a1, byte1x2 b1)
{
  dst0 = a0 + b0;
  dst1 = a1 + b1;
  byte2x2_sat(dst0, dst1);
}

// dst0 = a0 - u
static FOG_INLINE void byte1x2_sub_u(
  byte1x2& dst0, byte1x2 a0, uint32_t u)
{
  u |= u << 16;

  dst0 = a0 - u;
}

// dst0 = a0 - u
// dst1 = a1 - u
static FOG_INLINE void byte2x2_sub_u(
  byte1x2& dst0, uint32_t a0,
  byte1x2& dst1, uint32_t a1, uint32_t u)
{
  u |= u << 16;

  dst0 = a0 - u;
  dst1 = a1 - u;
}

// dst0 = a0 - b0
static FOG_INLINE void byte1x2_sub_byte1x2(
  byte1x2& dst0, byte1x2 a0, byte1x2 b0)
{
  dst0 = a0 - b0;
}

// dst0 = a0 - b0
// dst1 = a1 - b1
static FOG_INLINE void byte2x2_sub_byte2x2(
  byte1x2& dst0, byte1x2 a0, byte1x2 b0,
  byte1x2& dst1, byte1x2 a1, byte1x2 b1)
{
  dst0 = a0 - b0;
  dst1 = a1 - b1;
}

// dst0 = max(a0 - u, 0)
static FOG_INLINE void byte1x2_subus_u(
  byte1x2& dst0, byte1x2 a0, uint32_t u)
{
  u |= u << 16;

  dst0 = (a0 ^ BYTE_1x2MASK) + u;
  byte1x2_sat(dst0);
  dst0 ^= BYTE_1x2MASK;
}

// dst0 = max(a0 - u, 0)
// dst1 = max(a1 - u, 0)
static FOG_INLINE void byte2x2_subus_u(
  byte1x2& dst0, byte1x2 a0,
  byte1x2& dst1, byte1x2 a1, uint32_t u)
{
  u |= u << 16;

  dst0 = (a0 ^ BYTE_1x2MASK) + u;
  dst1 = (a1 ^ BYTE_1x2MASK) + u;
  byte2x2_sat(dst0, dst1);
  dst0 ^= BYTE_1x2MASK;
  dst1 ^= BYTE_1x2MASK;
}

// dst0 = max(a0 - b0, 0)
static FOG_INLINE void byte1x2_subus_byte1x2(
  byte1x2& dst0, byte1x2 a0, byte1x2 b0)
{
  dst0 = (a0 ^ BYTE_1x2MASK) + b0;
  byte1x2_sat(dst0);
  dst0 ^= BYTE_1x2MASK;
}

// dst0 = max(a0 - b0, 0)
// dst1 = max(a1 - b1, 0)
static FOG_INLINE void byte2x2_subus_byte2x2(
  byte1x2& dst0, byte1x2 a0, byte1x2 b0,
  byte1x2& dst1, byte1x2 a1, byte1x2 b1)
{
  dst0 = (a0 ^ BYTE_1x2MASK) + b0;
  dst1 = (a1 ^ BYTE_1x2MASK) + b1;
  byte2x2_sat(dst0, dst1);
  dst0 ^= BYTE_1x2MASK;
  dst1 ^= BYTE_1x2MASK;
}

// ============================================================================
// [Fog::ByteUtil - Byte1x2 - AddSub]
// ============================================================================

// dst0 = a0 + b0 - c0
static FOG_INLINE void byte1x2_addsub_byte1x2(
  byte1x2& dst0, byte1x2 a0, byte1x2 b0, byte1x2 c0)
{
  dst0 = a0 + b0 - c0;
}

// dst0 = a0 + b0 - c0
// dst0 = a1 + b1 - c1
static FOG_INLINE void byte2x2_addsub_byte2x2(
  byte1x2& dst0, byte1x2 a0, byte1x2 b0, byte1x2 c0,
  byte1x2& dst1, byte1x2 a1, byte1x2 b1, byte1x2 c1)
{
  dst0 = a0 + b0 - c0;
  dst0 = a1 + b1 - c1;
}

// dst0 = saturate(a0 + b0 - c0)
static FOG_INLINE void byte1x2_addsubus_byte1x2(
  byte1x2& dst0, byte1x2 a0, byte1x2 b0, byte1x2 c0)
{
  dst0 = a0 + b0;

  uint32_t x0lo = (dst0 & 0x000001FF);
  uint32_t x0hi = (dst0 & 0x01FF0000) >> 16;

  x0lo -= (c0 & 0x0000FFFF);
  x0hi -= (c0 & 0xFFFF0000) >> 16;

  x0lo &= ((~x0lo) & 0xFFFF0000) >> 16;
  x0hi &= ((~x0hi) & 0xFFFF0000) >> 16;

  dst0 = x0lo | (x0hi << 16);
  byte1x2_sat(dst0);
}

// dst0 = saturate(a0 + b0 - c0)
// dst0 = saturate(a1 + b1 - c1)
static FOG_INLINE void byte2x2_addsubus_byte2x2(
  byte1x2& dst0, byte1x2 a0, byte1x2 b0, byte1x2 c0,
  byte1x2& dst1, byte1x2 a1, byte1x2 b1, byte1x2 c1)
{
  byte1x2_addsubus_byte1x2(dst0, a0, b0, c0);
  byte1x2_addsubus_byte1x2(dst1, a1, b1, c1);
}

// ============================================================================
// [Fog::ByteUtil - Byte1x2 - Mul / MulDiv]
// ============================================================================

// dst0 = a0 * u
static FOG_INLINE void byte1x2_mul_u(
  byte1x2& dst0, byte1x2 a0, uint32_t u)
{
  dst0 = a0 * u;
}

// dst0 = a0 * u
// dst1 = a1 * u
static FOG_INLINE void byte2x2_mul_u(
  byte1x2& dst0, byte1x2 a0,
  byte1x2& dst1, byte1x2 a1, uint32_t u)
{
  dst0 = a0 * u;
  dst1 = a1 * u;
}

// dst0 = (a0 * u) / 255
static FOG_INLINE void byte1x2_muldiv255_u(
  byte1x2& dst0, byte1x2 a0, uint32_t u)
{
  dst0 = a0 * u; dst0 = ((dst0 + ((dst0 >> 8) & BYTE_1x2MASK) + BYTE_1x2HALF) >> 8) & BYTE_1x2MASK;
}

// dst0 = (a0 * u) / 255
// dst1 = (a1 * u) / 255
static FOG_INLINE void byte2x2_muldiv255_u(
  byte1x2& dst0, byte1x2 a0,
  byte1x2& dst1, byte1x2 a1, uint32_t u)
{
  dst0 = a0 * u; dst0 = ((dst0 + ((dst0 >> 8) & BYTE_1x2MASK) + BYTE_1x2HALF) >> 8) & BYTE_1x2MASK;
  dst1 = a1 * u; dst1 = ((dst1 + ((dst1 >> 8) & BYTE_1x2MASK) + BYTE_1x2HALF) >> 8) & BYTE_1x2MASK;
}

// Result = pack((a0 * u) / 255, (a1 * u) / 255)
static FOG_INLINE uint32_t byte2x2_muldiv255_u_pack0213(
  byte1x2 a0,
  byte1x2 a1, uint32_t u)
{
  a0 *= u;
  a0 = ((a0 + ((a0 >> 8) & BYTE_1x2MASK) + BYTE_1x2HALF) >> 8) & 0x00FF00FF;

  a1 *= u;
  a1 = ((a1 + ((a1 >> 8) & BYTE_1x2MASK) + BYTE_1x2HALF)     ) & 0xFF00FF00;

  return a0 | a1;
}

// dst0 = (a0 * u0) / 255
// dst1 = (a1 * u1) / 255
static FOG_INLINE void byte2x2_muldiv255_u(
  byte1x2& dst0, byte1x2 a0, uint32_t u0,
  byte1x2& dst1, byte1x2 a1, uint32_t u1)
{
  dst0 = a0 * u0; dst0 = ((dst0 + ((dst0 >> 8) & BYTE_1x2MASK) + BYTE_1x2HALF) >> 8) & BYTE_1x2MASK;
  dst1 = a1 * u1; dst1 = ((dst1 + ((dst1 >> 8) & BYTE_1x2MASK) + BYTE_1x2HALF) >> 8) & BYTE_1x2MASK;
}

// Result = pack((a0 * u0) / 255, (a1 * u1) / 255)
static FOG_INLINE uint32_t byte2x2_muldiv255_u_pack0213(
  byte1x2 a0, uint32_t u0,
  byte1x2 a1, uint32_t u1)
{
  a0 *= u0;
  a0 = ((a0 + ((a0 >> 8) & BYTE_1x2MASK) + BYTE_1x2HALF) >> 8) & 0x00FF00FF;

  a1 *= u1;
  a1 = ((a1 + ((a1 >> 8) & BYTE_1x2MASK) + BYTE_1x2HALF)     ) & 0xFF00FF00;

  return a0 | a1;
}

// dst0 = saturate((a0 * b0) / 255 + c0)
static FOG_INLINE void byte1x2_muldiv255_u_addus_byte1x2(
  byte1x2& dst0, byte1x2 a0, uint32_t b0, byte1x2 c0)
{
  byte1x2_muldiv255_u(dst0, a0, b0);
  byte1x2_addus_byte1x2(dst0, dst0, c0);
}

// dst0 = saturate((a0 * b0) / 255 + c0)
// dst1 = saturate((a1 * b1) / 255 + c1)
static FOG_INLINE void byte2x2_muldiv255_u_addus_byte2x2(
  byte1x2& dst0, byte1x2 a0, uint32_t b0, byte1x2 c0,
  byte1x2& dst1, byte1x2 a1, uint32_t b1, byte1x2 c1)
{
  byte1x2_muldiv255_u(dst0, a0, b0);
  byte1x2_muldiv255_u(dst1, a1, b1);
  byte1x2_addus_byte1x2(dst0, dst0, c0);
  byte1x2_addus_byte1x2(dst1, dst1, c1);
}

// dst0 = (a0 * b0) / 255
static FOG_INLINE void byte1x2_muldiv255_byte1x2(
  byte1x2& dst0, byte1x2 a0, byte1x2 b0)
{
  dst0 = ((a0 & 0x000000FF) * (b0 & 0x000000FF)) | ((a0 & 0x00FF0000) * ((b0 & 0x00FF0000) >> 16));
  dst0 = ((dst0 + ((dst0 >> 8) & BYTE_1x2MASK) + BYTE_1x2HALF) >> 8) & BYTE_1x2MASK;
}

// dst0 = (a0 * b0) / 255
// dst1 = (a1 * b1) / 255
static FOG_INLINE void byte2x2_muldiv255_byte2x2(
  byte1x2& dst0, byte1x2 a0, byte1x2 b0,
  byte1x2& dst1, byte1x2 a1, byte1x2 b1)
{
  dst0 = ((a0 & 0x000000FF) * (b0 & 0x000000FF)) | ((a0 & 0x00FF0000) * ((b0 & 0x00FF0000) >> 16));
  dst1 = ((a1 & 0x000000FF) * (b1 & 0x000000FF)) | ((a1 & 0x00FF0000) * ((b1 & 0x00FF0000) >> 16));
  dst0 = ((dst0 + ((dst0 >> 8) & BYTE_1x2MASK) + BYTE_1x2HALF) >> 8) & BYTE_1x2MASK;
  dst1 = ((dst1 + ((dst1 >> 8) & BYTE_1x2MASK) + BYTE_1x2HALF) >> 8) & BYTE_1x2MASK;
}

// dst0 = (a0 * b0) / 255 + c0
static FOG_INLINE void byte2x2_muldiv255_byte1x2_add_byte1x2(
  byte1x2& dst0, byte1x2 a0, byte1x2 b0, byte1x2 c0)
{
  byte1x2_muldiv255_byte1x2(dst0, a0, b0);
  byte1x2_add_byte1x2(dst0, dst0, c0);
}

// dst0 = (a0 * b0) / 255 + c0
// dst1 = (a1 * b1) / 255 + c1
static FOG_INLINE void byte2x2_muldiv255_byte2x2_add_byte2x2(
  byte1x2& dst0, byte1x2 a0, byte1x2 b0, byte1x2 c0,
  byte1x2& dst1, byte1x2 a1, byte1x2 b1, byte1x2 c1)
{
  byte2x2_muldiv255_byte2x2(dst0, a0, b0, dst1, a1, b1);
  byte2x2_add_byte2x2(dst0, dst0, c0, dst1, dst1, c1);
}

// dst0 = (a0 * b0) / 255 + c0
// dst1 = (a1 * b1) / 255 + c1
static FOG_INLINE void byte1x2_muldiv255_byte1x2_addus_byte1x2(
  byte1x2& dst0, byte1x2 a0, byte1x2 b0, byte1x2 c0)
{
  byte1x2_muldiv255_byte1x2(dst0, a0, b0);
  byte1x2_addus_byte1x2(dst0, dst0, c0);
}

// dst0 = (a0 * b0) / 255 + c0
// dst1 = (a1 * b1) / 255 + c1
static FOG_INLINE void byte2x2_muldiv255_byte2x2_addus_byte2x2(
  byte1x2& dst0, byte1x2 a0, byte1x2 b0, byte1x2 c0,
  byte1x2& dst1, byte1x2 a1, byte1x2 b1, byte1x2 c1)
{
  byte2x2_muldiv255_byte2x2(dst0, a0, b0, dst1, a1, b1);
  byte2x2_addus_byte2x2(dst0, dst0, c0, dst1, dst1, c1);
}

// dst0 = (a0 * b0 + c0 * d0) / 255
static FOG_INLINE void byte1x2_muldiv255_u_2x_add(
  byte1x2& dst0, byte1x2 a0, uint32_t b0, byte1x2 c0, uint32_t d0)
{
  dst0 = a0 * b0 + c0 * d0; dst0 = ((dst0 + ((dst0 >> 8) & BYTE_1x2MASK) + BYTE_1x2HALF) >> 8) & BYTE_1x2MASK;
}

// dst0 = (a0 * b0 + c0 * d0) / 255
// dst1 = (a1 * b1 + c1 * d1) / 255
static FOG_INLINE void byte2x2_muldiv255_u_2x_add(
  byte1x2& dst0, byte1x2 a0, uint32_t b0, byte1x2 c0, uint32_t d0,
  byte1x2& dst1, byte1x2 a1, uint32_t b1, byte1x2 c1, uint32_t d1)
{
  dst0 = a0 * b0 + c0 * d0; dst0 = ((dst0 + ((dst0 >> 8) & BYTE_1x2MASK) + BYTE_1x2HALF) >> 8) & BYTE_1x2MASK;
  dst1 = a1 * b1 + c1 * d1; dst1 = ((dst1 + ((dst1 >> 8) & BYTE_1x2MASK) + BYTE_1x2HALF) >> 8) & BYTE_1x2MASK;
}

// dst0 = saturate((a0 * b0) / 255) + (c0 * d0) / 255)
static FOG_INLINE void byte1x2_muldiv255_u_2x_addus(
  byte1x2& dst0, byte1x2 a0, uint32_t b0, byte1x2 c0, uint32_t d0)
{
  byte1x2_muldiv255_u(a0, a0, b0);
  byte1x2_muldiv255_u(c0, c0, d0);

  byte1x2_addus_byte1x2(dst0, a0, c0);
}

// dst0 = saturate((a0 * b0) / 255) + (c0 * d0) / 255)
// dst1 = saturate((a1 * b1) / 255) + (c1 * d1) / 255)
static FOG_INLINE void byte2x2_muldiv255_u_2x_addus(
  byte1x2& dst0, byte1x2 a0, uint32_t b0, byte1x2 c0, uint32_t d0,
  byte1x2& dst1, byte1x2 a1, uint32_t b1, byte1x2 c1, uint32_t d1)
{
  byte2x2_muldiv255_u(a0, a0, b0, a1, a1, b1);
  byte2x2_muldiv255_u(c0, c0, d0, c1, c1, d1);

  byte2x2_addus_byte2x2(dst0, a0, c0, dst1, a1, c1);
}

// dst0 = (a0 * u) / 255
static FOG_INLINE void byte1x2_muldiv256_u(
  byte1x2& dst0, byte1x2 a0, uint32_t u)
{
  dst0 = ((a0 * u) >> 8) & BYTE_1x2MASK;
}

// dst0 = (a0 * u) / 255
// dst1 = (a1 * u) / 255
static FOG_INLINE void byte2x2_muldiv256_u(
  byte1x2& dst0, byte1x2 a0,
  byte1x2& dst1, byte1x2 a1, uint32_t u)
{
  dst0 = ((a0 * u) >> 8) & BYTE_1x2MASK;
  dst0 = ((a1 * u) >> 8) & BYTE_1x2MASK;
}

// ============================================================================
// [Fog::ByteUtil - Byte1x2 - Min / Max]
// ============================================================================

// I hope that comparison instruction will be compiled using cmov or similar.

// dst0 = min(a0, u);
static FOG_INLINE void byte1x2_min_u(
  byte1x2& dst0, byte1x2 a0, uint32_t u)
{
  uint32_t a0lo = a0 & 0x000000FF;
  uint32_t a0hi = a0 & 0x00FF0000;

  if (a0lo > u) a0lo = u;
  if (a0hi > u) a0hi = u;

  dst0 = a0lo | a0hi;
}

// dst0 = min(a0, u);
// dst1 = min(a1, u);
static FOG_INLINE void byte2x2_min_u(
  byte1x2& dst0, byte1x2 a0,
  byte1x2& dst1, byte1x2 a1, uint32_t u)
{
  byte1x2_min_u(dst0, a0, u);
  byte1x2_min_u(dst1, a1, u);
}

// dst0 = min(a0, b0);
static FOG_INLINE void byte1x2_min_byte1x2(
  byte1x2& dst0, byte1x2 a0, byte1x2 b0)
{
  uint32_t a0lo = a0 & 0x000000FF;
  uint32_t a0hi = a0 & 0x00FF0000;

  uint32_t b0lo = b0 & 0x000000FF;
  uint32_t b0hi = b0 & 0x00FF0000;

  // I hope this will be compiled using cmov...
  if (a0lo > b0lo) a0lo = b0lo;
  if (a0hi > b0hi) a0hi = b0hi;

  dst0 = a0lo | a0hi;
}

// dst0 = min(a0, b0);
// dst1 = min(a1, b1);
static FOG_INLINE void byte2x2_min_byte2x2(
  byte1x2& dst0, byte1x2 a0, byte1x2 b0,
  byte1x2& dst1, byte1x2 a1, byte1x2 b1)
{
  byte1x2_min_byte1x2(dst0, a0, b0);
  byte1x2_min_byte1x2(dst1, a1, b1);
}

// dst0 = max(a0, u);
static FOG_INLINE void byte1x2_max_u(
  byte1x2& dst0, byte1x2 a0, uint32_t u)
{
  uint32_t a0lo = a0 & 0x000000FF;
  uint32_t a0hi = a0 & 0x00FF0000;

  if (a0lo < u) a0lo = u;
  if (a0hi < u) a0hi = u;

  dst0 = a0lo | a0hi;
}

// dst0 = max(a0, u);
// dst1 = max(a1, u);
static FOG_INLINE void byte2x2_max_u(
  byte1x2& dst0, byte1x2 a0,
  byte1x2& dst1, byte1x2 a1, uint32_t u)
{
  byte1x2_max_u(dst0, a0, u);
  byte1x2_max_u(dst1, a1, u);
}

// dst0 = max(a0, b0);
static FOG_INLINE void byte1x2_max_byte1x2(
  byte1x2& dst0, byte1x2 a0, byte1x2 b0)
{
  uint32_t a0lo = a0 & 0x000000FF;
  uint32_t a0hi = a0 & 0x00FF0000;

  uint32_t b0lo = b0 & 0x000000FF;
  uint32_t b0hi = b0 & 0x00FF0000;

  // I hope this will be compiled using cmov...
  if (a0lo < b0lo) a0lo = b0lo;
  if (a0hi < b0hi) a0hi = b0hi;

  dst0 = a0lo | a0hi;
}

// dst0 = max(a0, b0);
// dst1 = max(a1, b1);
static FOG_INLINE void byte2x2_max_byte2x2(
  byte1x2& dst0, byte1x2 a0, byte1x2 b0,
  byte1x2& dst1, byte1x2 a1, byte1x2 b1)
{
  byte1x2_max_byte1x2(dst0, a0, b0);
  byte1x2_max_byte1x2(dst1, a1, b1);
}

// ============================================================================
// [Fog::ByteUtil - Byte1x2 - Shift]
// ============================================================================

static FOG_INLINE void byte1x2_slli(
  byte1x2& dst0, uint32_t a0, uint32_t u)
{
  dst0 = a0 << u;
}

static FOG_INLINE void byte2x2_slli(
  byte1x2& dst0, byte1x2& a0,
  byte1x2& dst1, byte1x2& a1, uint32_t u)
{
  dst0 = a0 << u;
  dst1 = a1 << u;
}

static FOG_INLINE void byte1x2_slli_by1(
  byte1x2& dst0, byte1x2 a0)
{
  dst0 = a0 + a0;
}

static FOG_INLINE void byte2x2_slli_by1(
  byte1x2& dst0, byte1x2 a0,
  byte1x2& dst1, byte1x2 a1)
{
  dst0 = a0 + a0;
  dst1 = a1 + a1;
}

static FOG_INLINE void byte1x2_slli_by1_lo(
  byte1x2& dst0, byte1x2 a0)
{
  dst0 = a0 + (a0 & 0x000000FF);
}

static FOG_INLINE void byte2x2_slli_by1_lo(
  byte1x2& dst0, byte1x2 a0,
  byte1x2& dst1, byte1x2 a1)
{
  dst0 = a0 + (a0 & 0x000000FF);
  dst1 = a1 + (a1 & 0x000000FF);
}

static FOG_INLINE void byte1x2_slli_by1_hi(
  byte1x2& dst0, byte1x2 a0)
{
  dst0 = a0 + (a0 & 0x00FF0000);
}

static FOG_INLINE void byte2x2_slli_by1_hi(
  byte1x2& dst0, byte1x2 a0,
  byte1x2& dst1, byte1x2 a1)
{
  dst0 = a0 + (a0 & 0x00FF0000);
  dst1 = a1 + (a1 & 0x00FF0000);
}

static FOG_INLINE void byte1x2_srli(
  byte1x2& dst0, byte1x2 a0, uint32_t u)
{
  dst0 = a0 >> u;
}

static FOG_INLINE void byte2x2_srli(
  byte1x2& dst0, byte1x2 a0,
  byte1x2& dst1, byte1x2 a1, uint32_t u)
{
  dst0 = a0 >> u;
  dst1 = a1 >> u;
}

// ============================================================================
// [Fog::ByteUtil - Byte1x2 - Negate]
// ============================================================================

static FOG_INLINE void byte1x2_negate(
  byte1x2& dst0, uint32_t a0)
{
  dst0 = a0 ^ 0x00FF00FF;
}

static FOG_INLINE void byte2x2_negate(
  byte1x2& dst0, uint32_t a0,
  byte1x2& dst1, uint32_t a1)
{
  dst0 = a0 ^ 0x00FF00FF;
  dst1 = a1 ^ 0x00FF00FF;
}

// ============================================================================
// [Fog::ByteUtil - Byte1x2 - Premultiply]
// ============================================================================

static FOG_INLINE void byte2x2_premultiply(
  byte1x2& dst0, byte1x2 a0,
  byte1x2& dst1, byte1x2 a1)
{
  uint32_t aa = byte1x2_hi(a1);
  byte2x2_muldiv255_u(dst0, a0, dst1, a1 | 0x00FF0000, aa);
}

static FOG_INLINE void byte2x2_premultiply_by(
  byte1x2& dst0, byte1x2 a0,
  byte1x2& dst1, byte1x2 a1, uint32_t by)
{
  byte2x2_muldiv255_u(dst0, a0, dst1, a1 | 0x00FF0000, by);
}

// ============================================================================
// [Fog::ByteUtil - Byte1x2 - Interpolate]
// ============================================================================

static FOG_INLINE void byte1x2_interpolate_u_255(
  byte1x2& dst0, byte1x2 a0, uint32_t b0, byte1x2 c0, uint32_t d0,
  byte1x2& dst1, byte1x2 a1, uint32_t b1, byte1x2 c1, uint32_t d1)
{
  dst0 = a0 * b0 + c0 * d0;
  dst1 = a1 * b1 + c1 * d1;

  dst0 = dst0 + ((dst0 >> 8) & 0x00FF00FF) + 0x00800080;
  dst1 = dst1 + ((dst1 >> 8) & 0x00FF00FF) + 0x00800080;

  dst0 &= BYTE_1x2MASK;
  dst1 &= BYTE_1x2MASK;
}

static FOG_INLINE uint32_t byte1x2_interpolate_u_255_pack_0213(
  byte1x2 a0, uint32_t b0, byte1x2 c0, uint32_t d0,
  byte1x2 a1, uint32_t b1, byte1x2 c1, uint32_t d1)
{
  a0 = a0 * b0 + c0 * d0;
  a1 = a1 * b1 + c1 * d1;

  a0 = (a0 + ((a0 >> 8) & 0x00FF00FF) + 0x00800080);
  a1 = (a1 + ((a1 >> 8) & 0x00FF00FF) + 0x00800080) << 8;

  a0 &= 0x00FF00FF;
  a1 &= 0xFF00FF00;

  return a0 | a1;
}

static FOG_INLINE void byte1x2_interpolate_u_256(
  byte1x2& dst0, byte1x2 a0, uint32_t b0, byte1x2 c0, uint32_t d0,
  byte1x2& dst1, byte1x2 a1, uint32_t b1, byte1x2 c1, uint32_t d1)
{
  dst0 = a0 * b0 + c0 * d0;
  dst1 = a1 * b1 + c1 * d1;

  dst0 >>= 8;
  dst1 >>= 8;

  dst0 &= BYTE_1x2MASK;
  dst1 &= BYTE_1x2MASK;
}

static FOG_INLINE uint32_t byte1x2_interpolate_u_256_pack_0213(
  byte1x2 a0, uint32_t b0, byte1x2 c0, uint32_t d0,
  byte1x2 a1, uint32_t b1, byte1x2 c1, uint32_t d1)
{
  a0 = a0 * b0 + c0 * d0;
  a1 = a1 * b1 + c1 * d1;

  a0 >>= 8;

  a0 &= 0x00FF00FF;
  a1 &= 0xFF00FF00;

  return a0 | a1;
}

// ============================================================================
// [Fog::ByteUtil - Byte1x4 - 64-Bit Integer SIMD Library]
// ============================================================================

// These function allows to do SIMD with classic 64-bit integers.
//
// Terminology:
// - byte1x4 - unsigned 64 bit integer that contains four bytes at mask
//             BYTE_1x4MASK that is 0x00FF00FF00FF00FF.
// - u       - scalar at range 0 to 255 (or 0 to 256 depending to the context).
//
// Pixels are usually encoded in byte1x4 using this formula:
// - byte1x4 b0 (0x00AA00GG00RR00BB)

typedef uint64_t byte1x4;

static const uint64_t BYTE_1x3MASK          = FOG_UINT64_C(0x000000FF00FF00FF);
static const uint64_t BYTE_1x3HALF          = FOG_UINT64_C(0x0000008000800080);
static const uint64_t BYTE_1x3MASK_PLUS_ONE = FOG_UINT64_C(0x0000010010000100);

static const uint64_t BYTE_1x4MASK          = FOG_UINT64_C(0x00FF00FF00FF00FF);
static const uint64_t BYTE_1x4HALF          = FOG_UINT64_C(0x0080008000800080);
static const uint64_t BYTE_1x4MASK_PLUS_ONE = FOG_UINT64_C(0x1000010010000100);

// ============================================================================
// [Fog::ByteUtil - Byte1x4 - From / To / Packing / Unpacking]
// ============================================================================

//! @brief Unpack all bytes of DWORD into one byte1x4 value.
static FOG_INLINE void byte1x4_unpack_0213(
  byte1x4& dst0, uint32_t a0)
{
  dst0 = ((byte1x4)a0 | ((byte1x4)a0 << 24)) & BYTE_1x4MASK;
}

//! @brief Unpack '021' bytes of DWORD into one byte1x4 value.
static FOG_INLINE void byte1x4_unpack_021X(
  byte1x4& dst0, uint32_t a0)
{
  dst0 = ((byte1x4)a0 | ((byte1x4)a0 << 24)) & BYTE_1x3MASK;
}

//! @brief Pack byte1x4 value into DWORD.
static FOG_INLINE void byte1x4_pack_0213(uint32_t& dst0, byte1x4 x0)
{
  dst0 = (uint32_t)(x0 | (x0 >> 24));
}

// ============================================================================
// [Fog::ByteUtil - Packed-Byte OPS]
// ============================================================================

// NOTE: These functions are included here, because there are somethimes useful,
// but note that each function here needs first to unpack input values, do
// operation and them pack result back. This can be in some situations overhead
// and you should consider to use pure byte1x2 or byte1x4 solution instead.

// x_c' = (x_c * a) / 255
static FOG_INLINE uint32_t packed_muldiv255(uint32_t x, uint32_t a)
{
#if FOG_ARCH_BITS == 64
  uint64_t x0 = ((uint64_t)x | ((uint64_t)x << 24)) & FOG_UINT64_C(0x00FF00FF00FF00FF);
  x0 *= a;
  x0 = (x0 + ((x0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF)) + FOG_UINT64_C(0x0080008000800080)) >> 8;
  x0 &= FOG_UINT64_C(0x00FF00FF00FF00FF);
  return (uint32_t)(x0 | (x0 >> 24));
#else
  uint32_t t0 = ((x & 0x00FF00FF)     ) * a;
  uint32_t t1 = ((x & 0xFF00FF00) >> 8) * a;

  x  = ((t0 + ((t0 >> 8) & 0x00FF00FF) + 0x00800080) >> 8) & 0x00FF00FF;
  x |= ((t1 + ((t1 >> 8) & 0x00FF00FF) + 0x00800080)     ) & 0xFF00FF00;

  return x;
#endif
}

// x_c' = x_c * a
// x_a' = 0xFF
static FOG_INLINE uint32_t packed_muldiv255_Fxxx(uint32_t x, uint32_t a)
{
#if FOG_ARCH_BITS == 64
  // TODO: I'm not sure about this.
  uint64_t x0 = ((uint64_t)x | ((uint64_t)x << 24)) & FOG_UINT64_C(0x000000FF00FF00FF);
  x0 *= a;
  x0 = (x0 + ((x0 >> 8) & FOG_UINT64_C(0x000000FF00FF00FF)) + FOG_UINT64_C(0xFF00008000800080)) >> 8;
  x0 &= FOG_UINT64_C(0x00FF00FF00FF00FF);
  return (uint32_t)(x0 | (x0 >> 24));
#else
  // TODO: I'm not sure about this.
  uint32_t t0 = ((x & 0x00FF00FF)     ) * a;
  uint32_t t1 = ((x & 0x0000FF00) >> 8) * a;

  x  = ((t0 + ((t0 >> 8) & 0x00FF00FF) + 0x00800080) >> 8) & 0x00FF00FF;
  x |= ((t1 + ((t1 >> 8) & 0x00FF00FF) + 0xFF000080)     ) & 0xFF00FF00;

  return x;
#endif
}

// x_c' = x_c * a
// x_a' = 0x00
static FOG_INLINE uint32_t packed_muldiv255_0xxx(uint32_t x, uint32_t a)
{
#if FOG_ARCH_BITS == 64
  // TODO: I'm not sure about this.
  uint64_t x0 = ((uint64_t)x | ((uint64_t)x << 24)) & FOG_UINT64_C(0x000000FF00FF00FF);
  x0 *= a;
  x0 = (x0 + ((x0 >> 8) & FOG_UINT64_C(0x000000FF00FF00FF)) + FOG_UINT64_C(0x0000008000800080)) >> 8;
  x0 &= FOG_UINT64_C(0x000000FF00FF00FF);
  return (uint32_t)(x0 | (x0 >> 24));
#else
  uint32_t t0 = ((x & 0x00FF00FF)     ) * a;
  uint32_t t1 = ((x & 0x0000FF00) >> 8) * a;

  x  = ((t0 + ((t0 >> 8) & 0x00FF00FF) + 0x00800080) >> 8) & 0x00FF00FF;
  x |= ((t1 + ((t1 >> 8) & 0x000000FF) + 0x00000080)     ) & 0x0000FF00;

  return x;
#endif
}

// x_c' = min(x_c + y_c, 255)
static FOG_INLINE uint32_t packed_addus(uint32_t x, uint32_t y)
{
#if FOG_ARCH_BITS == 64
  uint64_t x0 = ((uint64_t)x | ((uint64_t)x << 24)) & FOG_UINT64_C(0x00FF00FF00FF00FF);
  uint64_t y0 = ((uint64_t)y | ((uint64_t)y << 24)) & FOG_UINT64_C(0x00FF00FF00FF00FF);

  x0 += y0;
  x0 |= FOG_UINT64_C(0x0100010001000100) - ((x0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF));
  x0 &= FOG_UINT64_C(0x00FF00FF00FF00FF);

  return (uint32_t)(x0 | (x0 >> 24));
#else
  uint32_t t0 = (x & 0x00FF00FF);
  uint32_t t1 = (x & 0xFF00FF00) >> 8;

  t0 += (y & 0x00FF00FF);
  t1 += (y & 0xFF00FF00) >> 8;

  t0 |= 0x01000100 - ((t0 >> 8) & 0x00FF00FF);
  t1 |= 0x01000100 - ((t1 >> 8) & 0x00FF00FF);

  t0 &= 0x00FF00FF;
  t1 &= 0x00FF00FF;

  return t0 | (t1 << 8);
#endif
}

// x_c' = (x_c * a) / 255 + y
static FOG_INLINE uint32_t packed_muldiv255_addus(uint32_t x, uint32_t a, uint32_t y)
{
#if FOG_ARCH_BITS == 64
  uint64_t x0 = ((uint64_t)x | ((uint64_t)x << 24)) & FOG_UINT64_C(0x00FF00FF00FF00FF);
  uint64_t y0 = ((uint64_t)y | ((uint64_t)y << 24)) & FOG_UINT64_C(0x00FF00FF00FF00FF);

  x0 *= a;
  x0 = (x0 + ((x0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF)) + FOG_UINT64_C(0x0080008000800080)) >> 8;
  x0 &= FOG_UINT64_C(0x00FF00FF00FF00FF);

  x0 += y0;
  x0 |= FOG_UINT64_C(0x0100010001000100) - ((x0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF));
  x0 &= FOG_UINT64_C(0x00FF00FF00FF00FF);

  return (uint32_t)(x0 | (x0 >> 24));
#else
  uint32_t t0 = ((x & 0x00FF00FF)     ) * a;
  uint32_t t1 = ((x & 0xFF00FF00) >> 8) * a;

  t0 = ((t0 + ((t0 >> 8) & 0x00FF00FF) + 0x00800080) >> 8) & 0x00FF00FF;
  t1 = ((t1 + ((t1 >> 8) & 0x00FF00FF) + 0x00800080) >> 8) & 0x00FF00FF;

  t0 += (y & 0x00FF00FF);
  t1 += (y & 0xFF00FF00) >> 8;

  t0 |= 0x01000100 - ((t0 >> 8) & 0x00FF00FF);
  t1 |= 0x01000100 - ((t1 >> 8) & 0x00FF00FF);

  t0 &= 0x00FF00FF;
  t1 &= 0x00FF00FF;

  return t0 | (t1 << 8);
#endif
}

// TODO: Better name, audit
static FOG_INLINE uint32_t packed_muldiv255_2x_join(
  uint32_t x0, uint32_t a0,
  uint32_t y0, uint32_t b0)
{
  uint32_t t0 = ((x0 & 0x00FF00FF)     ) * a0 + ((y0 & 0x00FF00FF)     ) * b0;
  uint32_t t1 = ((x0 & 0xFF00FF00) >> 8) * a0 + ((y0 & 0xFF00FF00) >> 8) * b0;

  t0 = (t0 + ((t0 >> 8) & 0x00FF00FF) + 0x00800080);
  t1 = (t1 + ((t1 >> 8) & 0x00FF00FF) + 0x00800080);

  x0  = (t0 & 0xFF00FF00) >> 8;
  x0 |= (t1 & 0xFF00FF00);

  return x0;
}

// TODO: Better name, audit
static FOG_INLINE uint32_t packed_muldiv256_2x_join(
  uint32_t x0, uint32_t a0,
  uint32_t y0, uint32_t b0)
{
  uint32_t t0 = ((x0 & 0x00FF00FF)     ) * a0 + ((y0 & 0x00FF00FF)     ) * b0;
  uint32_t t1 = ((x0 & 0xFF00FF00) >> 8) * a0 + ((y0 & 0xFF00FF00) >> 8) * b0;

  x0  = (t0 & 0xFF00FF00) >> 8;
  x0 |= (t1 & 0xFF00FF00);

  return x0;
}

#if 0
// Currently not used.
// x_c = (x_c * a_c) / 255
static FOG_INLINE uint32_t bytemulC(uint32_t x, uint32_t a)
{
  uint32_t t;
  uint32_t r = (x & 0xFF) * (a & 0xFF);
  r |= (x & 0x00FF0000) * ((a >> 16) & 0xFF);
  r += 0x80;
  r = (r + ((r >> 8) & 0x00FF00FF)) >> 8;
  r &= 0x00FF00FF;

  x >>= 8;
  t = (x & 0xFF) * ((a >> 8) & 0xFF);
  t |= (x & 0x00FF0000) * (a >> 24);
  t += 0x80;
  t = t + ((t >> 8) & 0x00FF00FF);

  return r | (t & 0xFF00FF00);
}

// x_c = (x_c * a) / 255 + y
static FOG_INLINE uint32_t bytemuladdC(uint32_t x, uint32_t a, uint32_t y)
{
  uint32_t t;
  uint32_t r = (x & 0xFF) * (a & 0xFF);

  r |= (x & 0x00FF0000) * ((a >> 16) & 0xFF);
  r += 0x80;
  r = (r + ((r >> 8) & 0x00FF00FF)) >> 8;
  r &= 0x00FF00FF;
  r += y & 0x00FF00FF;
  r |= 0x01000100 - ((r >> 8) & 0x00FF00FF);
  r &= 0x00FF00FF;

  x >>= 8;
  t = (x & 0xFF) * ((a >> 8) & 0xFF);
  t |= (x & 0x00FF0000) * (a >> 24);
  t += 0x80;
  t = (t + ((t >> 8) & 0x00FF00FF)) >> 8;
  t &= 0x00FF00FF;
  t += (y >> 8) & 0x00FF00FF;
  t |= 0x01000100 - ((t >> 8) & 0x00FF00FF);
  t &= 0x00FF00FF;

  return r | (t << 8);
}

// x_c = (x_c * a_c + y_c * b) / 255
static FOG_INLINE uint32_t byteaddmulC(uint32_t x, uint32_t a, uint32_t y, uint32_t b)
{
  uint32_t t;
  uint32_t r = (x >> 24) * (a >> 24) +
               (y >> 24) * b;
  r += (r >> 8) + 0x80;
  r >>= 8;

  t = (x & 0x0000FF00) * ((a >> 8) & 0xFF) + (y & 0x0000FF00) * b;
  t += (t >> 8) + (0x80 << 8);
  t >>= 16;

  t |= r << 16;
  t |= 0x01000100 - ((t >> 8) & 0x00FF00FF);
  t &= 0x00FF00FF;
  t <<= 8;

  r = ((x >> 16) & 0xFF) * ((a >> 16) & 0xFF) +
      ((y >> 16) & 0xFF) * b + 0x80;
  r += (r >> 8);
  r >>= 8;

  x = (x & 0xFF) * (a & 0xFF) + (y & 0xFF) * b + 0x80;
  x += (x >> 8);
  x >>= 8;
  x |= r << 16;
  x |= 0x01000100 - ((x >> 8) & 0x00FF00FF);
  x &= 0x00FF00FF;

  return x | t;
}
#endif

} // ByteUtil namespace
} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_GRAPHICS_BYTEUTIL_P_H
