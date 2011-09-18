// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_FACE_FACE_C_P32_H
#define _FOG_CORE_FACE_FACE_C_P32_H

// [Dependencies]
#include <Fog/Core/Face/Constants.h>
#include <Fog/Core/Face/Features.h>
#include <Fog/Core/Face/Face_C_Types.h>
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/BSwap.h>

namespace Fog {
namespace Face {

//! @addtogroup Fog_Face_C
//! @{

// ============================================================================
// [Fog::Face - P32 - Load / Store]
// ============================================================================

static FOG_INLINE void p32Load1b(p32& dst0, const void* srcp) { dst0 = ((const uint8_t *)srcp)[0]; }
static FOG_INLINE void p32Store1b(void* dstp, const p32& src0) { ((uint8_t *)dstp)[0] = (uint8_t )(src0); }

static FOG_INLINE void p32Load2aNative(p32& dst0, const void* srcp) { dst0 = ((const uint16_t*)srcp)[0]; }
static FOG_INLINE void p32Load2uNative(p32& dst0, const void* srcp) { dst0 = ((const uint16_t*)srcp)[0]; }

static FOG_INLINE void p32Load2aSwap(p32& dst0, const void* srcp) { dst0 = Memory::bswap16( ((const uint16_t*)srcp)[0] ); }
static FOG_INLINE void p32Load2uSwap(p32& dst0, const void* srcp) { dst0 = Memory::bswap16( ((const uint16_t*)srcp)[0] ); }

static FOG_INLINE void p32Store2aNative(void* dstp, const p32& src0) { ((uint16_t*)dstp)[0] = (uint16_t)(src0); }
static FOG_INLINE void p32Store2uNative(void* dstp, const p32& src0) { ((uint16_t*)dstp)[0] = (uint16_t)(src0); }

static FOG_INLINE void p32Store2aSwap(void* dstp, const p32& src0) { ((uint16_t*)dstp)[0] = Memory::bswap16( (uint16_t)(src0) ); }
static FOG_INLINE void p32Store2uSwap(void* dstp, const p32& src0) { ((uint16_t*)dstp)[0] = Memory::bswap16( (uint16_t)(src0) ); }

static FOG_INLINE void p32Load4aNative(p32& dst0, const void* srcp) { dst0 = ((const uint32_t*)srcp)[0]; }
static FOG_INLINE void p32Load4uNative(p32& dst0, const void* srcp) { dst0 = ((const uint32_t*)srcp)[0]; }

static FOG_INLINE void p32Load4aSwap(p32& dst0, const void* srcp) { dst0 = Memory::bswap32( ((const uint32_t*)srcp)[0] ); }
static FOG_INLINE void p32Load4uSwap(p32& dst0, const void* srcp) { dst0 = Memory::bswap32( ((const uint32_t*)srcp)[0] ); }

static FOG_INLINE void p32Store4aNative(void* dstp, const p32& src0) { ((uint32_t*)dstp)[0] = (uint32_t)(src0); }
static FOG_INLINE void p32Store4uNative(void* dstp, const p32& src0) { ((uint32_t*)dstp)[0] = (uint32_t)(src0); }

static FOG_INLINE void p32Store4aSwap(void* dstp, const p32& src0) { ((uint32_t*)dstp)[0] = Memory::bswap32( (uint32_t)(src0) ); }
static FOG_INLINE void p32Store4uSwap(void* dstp, const p32& src0) { ((uint32_t*)dstp)[0] = Memory::bswap32( (uint32_t)(src0) ); }

static FOG_INLINE void p32Load3bNative(p32& dst0, const void* srcp)
{
  const uint8_t* src8 = reinterpret_cast<const uint8_t*>(srcp);

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  dst0 = _FOG_FACE_COMBINE_2( (uint32_t)((const uint16_t*)(src8 + 0))[0],
                              (uint32_t)((const uint8_t *)(src8 + 2))[0] << 16);
#else
  dst0 = _FOG_FACE_COMBINE_2( (uint32_t)((const uint8_t *)(src8 + 0))[0] << 16),
                              (uint32_t)((const uint16_t*)(src8 + 1))[0]);
#endif // FOG_BYTE_ORDER
}

static FOG_INLINE void p32Load3bSwap(p32& dst0, const void* srcp)
{
  const uint8_t* src8 = reinterpret_cast<const uint8_t*>(srcp);

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  dst0 = _FOG_FACE_COMBINE_3( (uint32_t)((const uint8_t *)(src8 + 0))[0]      ,
                              (uint32_t)((const uint8_t *)(src8 + 1))[0] <<  8,
                              (uint32_t)((const uint8_t *)(src8 + 2))[0] << 16);
#else
  dst0 = _FOG_FACE_COMBINE_3( (uint32_t)((const uint8_t *)(src8 + 0))[0] << 16,
                              (uint32_t)((const uint8_t *)(src8 + 1))[0] <<  8,
                              (uint32_t)((const uint8_t *)(src8 + 2))[0]      );
#endif // FOG_BYTE_ORDER
}

static FOG_INLINE void p32Store3bNative(void* dstp, const p32& src0)
{
  uint8_t* dst8 = reinterpret_cast<uint8_t*>(dstp);

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  ((uint16_t*)(dst8 + 0))[0] = (uint16_t)(src0      );
  ((uint8_t *)(dst8 + 2))[0] = (uint8_t )(src0 >> 16);
#else
  ((uint8_t *)(dst8 + 0))[0] = (uint8_t )(src0 >> 16);
  ((uint16_t*)(dst8 + 1))[0] = (uint16_t)(src0      );
#endif // FOG_BYTE_ORDER
}

static FOG_INLINE void p32Store3bSwap(void* dstp, const p32& src0)
{
  uint8_t* dst8 = reinterpret_cast<uint8_t*>(dstp);

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  ((uint8_t *)(dst8 + 0))[0] = (uint8_t)(src0      );
  ((uint8_t *)(dst8 + 1))[0] = (uint8_t)(src0 >>  8);
  ((uint8_t *)(dst8 + 2))[0] = (uint8_t)(src0 >> 16);
#else
  ((uint8_t *)(dst8 + 0))[0] = (uint8_t)(src0 >> 16);
  ((uint8_t *)(dst8 + 1))[0] = (uint8_t)(src0 >>  8);
  ((uint8_t *)(dst8 + 2))[0] = (uint8_t)(src0      );
#endif // FOG_BYTE_ORDER
}

// [00 11 22 33]
// [LO LO HI HI]

// [00 11 22 33]
// [LO LO HI HI]

static FOG_INLINE void p32Load8aNative(p32& dst0Lo, p32& dst0Hi, const void* srcp)
{
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  // [7 6 5 4|3 2 1 0]
  dst0Lo = ((const uint32_t*)srcp)[0];
  dst0Hi = ((const uint32_t*)srcp)[1];
#else
  // [0 1 2 3|4 5 6 7]
  dst0Hi = ((const uint32_t*)srcp)[0];
  dst0Lo = ((const uint32_t*)srcp)[1];
#endif // FOG_BYTE_ORDER
}

static FOG_INLINE void p32Load8uNative(p32& dst0Lo, p32& dst0Hi, const void* srcp)
{
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  // [7 6 5 4|3 2 1 0]
  dst0Lo = ((const uint32_t*)srcp)[0];
  dst0Hi = ((const uint32_t*)srcp)[1];
#else
  // [0 1 2 3|4 5 6 7]
  dst0Hi = ((const uint32_t*)srcp)[0];
  dst0Lo = ((const uint32_t*)srcp)[1];
#endif // FOG_BYTE_ORDER
}

static FOG_INLINE void p32Store8aNative(void* dstp, const p32& src0Lo, const p32& src0Hi)
{
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  // [7 6 5 4|3 2 1 0]
  ((uint32_t*)dstp)[0] = src0Lo;
  ((uint32_t*)dstp)[1] = src0Hi;
#else
  // [0 1 2 3|4 5 6 7]
  ((uint32_t*)dstp)[0] = src0Hi;
  ((uint32_t*)dstp)[1] = src0Lo;
#endif // FOG_BYTE_ORDER
}

static FOG_INLINE void p32Store8uNative(void* dstp, const p32& src0Lo, const p32& src0Hi)
{
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  // [7 6 5 4|3 2 1 0]
  ((uint32_t*)dstp)[0] = src0Lo;
  ((uint32_t*)dstp)[1] = src0Hi;
#else
  // [0 1 2 3|4 5 6 7]
  ((uint32_t*)dstp)[0] = src0Hi;
  ((uint32_t*)dstp)[1] = src0Lo;
#endif // FOG_BYTE_ORDER
}

static FOG_INLINE void p32Load8aRaw(p32& dst0Lo, p32& dst0Hi, const void* srcp)
{
  dst0Lo = ((const uint32_t*)srcp)[0];
  dst0Hi = ((const uint32_t*)srcp)[1];
}

static FOG_INLINE void p32Load8uRaw(p32& dst0Lo, p32& dst0Hi, const void* srcp)
{
  dst0Lo = ((const uint32_t*)srcp)[0];
  dst0Hi = ((const uint32_t*)srcp)[1];
}

static FOG_INLINE void p32Store8aRaw(void* dstp, const p32& src0Lo, const p32& src0Hi)
{
  ((uint32_t*)dstp)[0] = src0Lo;
  ((uint32_t*)dstp)[1] = src0Hi;
}

static FOG_INLINE void p32Store8uRaw(void* dstp, const p32& src0Lo, const p32& src0Hi)
{
  ((uint32_t*)dstp)[0] = src0Lo;
  ((uint32_t*)dstp)[1] = src0Hi;
}

// ============================================================================
// [Fog::Face - P32 - Clear]
// ============================================================================

//! @brief Clear the @a dst0.
//!
//! @verbatim
//! dst0.u32[0] = 0
//! @endverbatim
static FOG_INLINE void p32Clear(p32& dst0)
{
  dst0 = 0;
}

//! @brief Clear the @a dst0/dst1.
//!
//! @verbatim
//! dst0.u32[0] = 0
//! dst1.u32[0] = 0
//! @endverbatim
static FOG_INLINE void p32Clear_2x(p32& dst0, p32& dst1)
{
  dst0 = 0;
  dst1 = 0;
}

// ============================================================================
// [Fog::Face - P32 - Copy]
// ============================================================================

//! @brief Copy @a x0 to @a dst0.
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0]
//! @endverbatim
static FOG_INLINE void p32Copy(
  p32& dst0, const p32& x0)
{
  dst0 = x0;
}

//! @brief Copy @a x0/x1 to @a dst0/dst1.
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0]
//! dst1.u32[0] = x1.u32[0]
//! @endverbatim
static FOG_INLINE void p32Copy_2x(
  p32& dst0, const p32& x0,
  p32& dst1, const p32& x1)
{
  dst0 = x0;
  dst1 = x1;
}

// ============================================================================
// [Fog::Face - P32 - Extract]
// ============================================================================

static FOG_INLINE void p32ExtractPBB0(p32& dst0, const p32& x0) { dst0 = (x0      ) & 0xFFU; }
static FOG_INLINE void p32ExtractPBB1(p32& dst0, const p32& x0) { dst0 = (x0 >>  8) & 0xFFU; }
static FOG_INLINE void p32ExtractPBB2(p32& dst0, const p32& x0) { dst0 = (x0 >> 16) & 0xFFU; }
static FOG_INLINE void p32ExtractPBB3(p32& dst0, const p32& x0) { dst0 = (x0 >> 24);         }

static FOG_INLINE void p32ExtractPBW0(p32& dst0, const p32& x0) { dst0 = (x0      ) & 0xFFU; }
static FOG_INLINE void p32ExtractPBW1(p32& dst0, const p32& x0) { dst0 = (x0 >> 16);         }

static FOG_INLINE void p32ExtractPWW0(p32& dst0, const p32& x0) { dst0 = (x0      ) & 0xFFFFU; }
static FOG_INLINE void p32ExtractPWW1(p32& dst0, const p32& x0) { dst0 = (x0 >> 16);           }

// ============================================================================
// [Fog::Face - P32 - As]
// ============================================================================

static FOG_INLINE uint32_t p32PBB0AsU32(const p32& x0) { return (x0      ) & 0xFFU; }
static FOG_INLINE uint32_t p32PBB1AsU32(const p32& x0) { return (x0 >>  8) & 0xFFU; }
static FOG_INLINE uint32_t p32PBB2AsU32(const p32& x0) { return (x0 >> 16) & 0xFFU; }
static FOG_INLINE uint32_t p32PBB3AsU32(const p32& x0) { return (x0 >> 24) & 0xFFU; }

static FOG_INLINE uint32_t p32PBW0AsU32(const p32& x0) { return (x0      ) & 0xFFU; }
static FOG_INLINE uint32_t p32PBW1AsU32(const p32& x0) { return (x0 >> 16) & 0xFFU; }

static FOG_INLINE uint32_t p32PWW0AsU32(const p32& x0) { return (x0      ) & 0xFFFFU; }
static FOG_INLINE uint32_t p32PWW1AsU32(const p32& x0) { return (x0 >> 16) & 0xFFFFU; }

// ============================================================================
// [Fog::Face - P32 - Pack]
// ============================================================================

//! @brief Pack x0_31 and x0_20 into single P32 value.
//!
//! @verbatim
//! dst0.u8[0] = x0_20.u16[0] or x0_20.u8[0]
//! dst0.u8[1] = x0_31.u16[0] or x0_31.u8[0]
//! dst0.u8[2] = x0_20.u16[1] or x0_20.u8[2]
//! dst0.u8[3] = x0_31.u16[1] or x0_31.u8[2]
//! @endverbatim
static FOG_INLINE void p32PackPBB2031FromPBW(
  p32& dst0, const p32& x0_20, const p32& x0_31)
{
  dst0 = (x0_31 << 8) | x0_20;
}

// ============================================================================
// [Fog::Face - P32 - Unpack]
// ============================================================================

//! @brief Unpack 2nd and 0st BYTE of a single DWORD into the p32.
//!
//! @verbatim
//! dst0.u16[0] = x0_20.u8[0]
//! dst0.u16[1] = x0_20.u8[2]
//! @endverbatim
static FOG_INLINE void p32UnpackPBWFromPBB_20(
  p32& dst0, const p32& x0_20)
{
  dst0 = x0_20 & 0x00FF00FFU;
}

//! @brief Unpack 3rd and 1st BYTE of a single DWORD into the p32.
//!
//! @verbatim
//! dst0.u16[0] = x0_31.u8[1]
//! dst0.u16[1] = x0_31.u8[3]
//! @endverbatim
static FOG_INLINE void p32UnpackPBWFromPBB_31(
  p32& dst0, const p32& x0_31)
{
  dst0 = (x0_31 >> 8) & 0x00FF00FFU;
}

//! @brief Unpack all bytes of DWORD into two p32 values.
//!
//! @verbatim
//! dst0_20.u16[0] = x0.u8[0]
//! dst0_20.u16[1] = x0.u8[2]
//! dst0_31.u16[0] = x0.u8[1]
//! dst0_31.u16[1] = x0.u8[3]
//! @endverbatim
static FOG_INLINE void p32UnpackPBWFromPBB_2031(
  p32& dst0_20, p32& dst0_31, const p32& x0)
{
  uint32_t t0 = x0;

  dst0_20 = t0;
  dst0_31 = t0 >> 8;

  dst0_20 &= 0x00FF00FFU;
  dst0_31 &= 0x00FF00FFU;
}

//! @brief Unpack '20_1' bytes of DWORD into two p32 values.
//!
//! @verbatim
//! dst0_20.u16[0] = x0.u8[0]
//! dst0_20.u16[1] = x0.u8[2]
//! dst0_31.u16[0] = x0.u8[1]
//! dst0_31.u16[1] = 0x00
//! @endverbatim
static FOG_INLINE void p32UnpackPBWFromPBB_20Z1(
  p32& dst0_20, p32& dst0_Z1, const p32& x0)
{
  uint32_t t0 = x0;

  dst0_20 = t0;
  dst0_Z1 = t0 >> 8;

  dst0_20 &= 0x00FF00FFU;
  dst0_Z1 &= 0x000000FFU;
}

// ============================================================================
// [Fog::Face - P32 - Zero / Fill]
// ============================================================================

static FOG_INLINE void p32ZeroPBB0(p32& dst0, const p32& x0) { dst0 = x0 & 0xFFFFFF00U; }
static FOG_INLINE void p32ZeroPBB1(p32& dst0, const p32& x0) { dst0 = x0 & 0xFFFF00FFU; }
static FOG_INLINE void p32ZeroPBB2(p32& dst0, const p32& x0) { dst0 = x0 & 0xFF00FFFFU; }
static FOG_INLINE void p32ZeroPBB3(p32& dst0, const p32& x0) { dst0 = x0 & 0x00FFFFFFU; }

static FOG_INLINE void p32ZeroPBW0(p32& dst0, const p32& x0) { dst0 = x0 & 0xFFFFFF00U; }
static FOG_INLINE void p32ZeroPBW1(p32& dst0, const p32& x0) { dst0 = x0 & 0xFF00FFFFU; }

static FOG_INLINE void p32ZeroPWW0(p32& dst0, const p32& x0) { dst0 = x0 & 0xFFFF0000U; }
static FOG_INLINE void p32ZeroPWW1(p32& dst0, const p32& x0) { dst0 = x0 & 0x0000FFFFU; }

static FOG_INLINE void p32FillPBB0(p32& dst0, const p32& x0) { dst0 = x0 | 0x000000FFU; }
static FOG_INLINE void p32FillPBB1(p32& dst0, const p32& x0) { dst0 = x0 | 0x0000FF00U; }
static FOG_INLINE void p32FillPBB2(p32& dst0, const p32& x0) { dst0 = x0 | 0x00FF0000U; }
static FOG_INLINE void p32FillPBB3(p32& dst0, const p32& x0) { dst0 = x0 | 0xFF000000U; }

static FOG_INLINE void p32FillPBW0(p32& dst0, const p32& x0) { dst0 = x0 | 0x00FF0000U; }
static FOG_INLINE void p32FillPBW1(p32& dst0, const p32& x0) { dst0 = x0 | 0x000000FFU; }

static FOG_INLINE void p32FillPWW0(p32& dst0, const p32& x0) { dst0 = x0 | 0x0000FFFFU; }
static FOG_INLINE void p32FillPWW1(p32& dst0, const p32& x0) { dst0 = x0 | 0xFFFF0000U; }

// ============================================================================
// [Fog::Face - P32 - Replace]
// ============================================================================

static FOG_INLINE void p32ReplacePBB0(p32& dst0, const p32& x0, const p32& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & 0x000000FFU, u0      ); }
static FOG_INLINE void p32ReplacePBB1(p32& dst0, const p32& x0, const p32& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & 0x0000FF00U, u0 <<  8); }
static FOG_INLINE void p32ReplacePBB2(p32& dst0, const p32& x0, const p32& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & 0x00FF0000U, u0 << 16); }
static FOG_INLINE void p32ReplacePBB3(p32& dst0, const p32& x0, const p32& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & 0xFF000000U, u0 << 24); }

static FOG_INLINE void p32ReplacePBW0(p32& dst0, const p32& x0, const p32& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & 0x000000FFU, u0      ); }
static FOG_INLINE void p32ReplacePBW1(p32& dst0, const p32& x0, const p32& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & 0x00FF0000U, u0 << 16); }

static FOG_INLINE void p32ReplacePWW0(p32& dst0, const p32& x0, const p32& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & 0x000000FFU, u0      ); }
static FOG_INLINE void p32ReplacePWW1(p32& dst0, const p32& x0, const p32& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & 0x00FF0000U, u0 << 16); }

// ============================================================================
// [Fog::Face - P32 - Logical / Arithmetic]
// ============================================================================

//! @brief Scalar combine (AND or OR, depends on platform).
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] +| y0.u32[0]
//! @endverbatim
static FOG_INLINE void p32Combine(
  p32& dst0, const p32& x0, const p32& y0)
{
  dst0 = _FOG_FACE_COMBINE_2(x0, y0);
}

//! @brief Scalar combine (AND or OR, depends on platform) (2x).
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] +| y0.u32[0]
//! dst1.u32[0] = x1.u32[0] +| y1.u32[0]
//! @endverbatim
static FOG_INLINE void p32Combine_2x(
  p32& dst0, const p32& x0, const p32& y0,
  p32& dst1, const p32& x1, const p32& y1)
{
  uint32_t t0 = _FOG_FACE_COMBINE_2(x0, y0);
  uint32_t t1 = _FOG_FACE_COMBINE_2(x1, y1);

  dst0 = t0;
  dst1 = t1;
}

//! @brief Scalar AND.
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] & y0.u32[0]
//! @endverbatim
static FOG_INLINE void p32And(
  p32& dst0, const p32& x0, const p32& y0)
{
  dst0 = x0 & y0;
}

//! @brief Scalar AND (2x).
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] & y0.u32[0]
//! dst1.u32[0] = x1.u32[0] & y1.u32[0]
//! @endverbatim
static FOG_INLINE void p32And_2x(
  p32& dst0, const p32& x0, const p32& y0,
  p32& dst1, const p32& x1, const p32& y1)
{
  uint32_t t0 = x0 & y0;
  uint32_t t1 = x1 & y1;

  dst0 = t0;
  dst1 = t1;
}

//! @brief Scalar OR.
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] | y0.u32[0]
//! @endverbatim
static FOG_INLINE void p32Or(
  p32& dst0, const p32& x0, const p32& y0)
{
  dst0 = x0 | y0;
}

//! @brief Scalar OR (2x).
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] | y0.u32[0]
//! dst1.u32[0] = x1.u32[0] | y1.u32[0]
//! @endverbatim
static FOG_INLINE void p32Or_2x(
  p32& dst0, const p32& x0, const p32& y0,
  p32& dst1, const p32& x1, const p32& y1)
{
  uint32_t t0 = x0 | y0;
  uint32_t t1 = x1 | y1;

  dst0 = t0;
  dst1 = t1;
}

//! @brief Scalar XOR.
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] ^ y0.u32[0]
//! @endverbatim
static FOG_INLINE void p32Xor(
  p32& dst0, const p32& x0, const p32& y0)
{
  dst0 = x0 ^ y0;
}

//! @brief Scalar XOR (2x).
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] ^ y0.u32[0]
//! dst1.u32[0] = x1.u32[0] ^ y1.u32[0]
//! @endverbatim
static FOG_INLINE void p32Xor_2x(
  p32& dst0, const p32& x0, const p32& y0,
  p32& dst1, const p32& x1, const p32& y1)
{
  uint32_t t0 = x0 ^ y0;
  uint32_t t1 = x1 ^ y1;

  dst0 = t0;
  dst1 = t1;
}

//! @brief Scalar negate.
//!
//! @verbatim
//! dst0.u32[0] = ~x0.u32[0]
//! @endverbatim
static FOG_INLINE void p32Neg(
  p32& dst0, const p32& x0)
{
  dst0 = ~x0;
}

//! @brief Scalar negate (2x).
//!
//! @verbatim
//! dst0.u32[0] = ~x0.u32[0]
//! dst1.u32[0] = ~x1.u32[0]
//! @endverbatim
static FOG_INLINE void p32Neg_2x(
  p32& dst0, const p32& x0,
  p32& dst1, const p32& x1)
{
  uint32_t t0 = ~x0;
  uint32_t t1 = ~x1;

  dst0 = t0;
  dst1 = t1;
}

//! @brief Scalar add.
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] + y0.u32[0]
//! @endverbatim
static FOG_INLINE void p32Add(
  p32& dst0, const p32& x0, const p32& y0)
{
  dst0 = x0 + y0;
}

//! @brief Scalar add (2x).
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] + y0.u32[0]
//! dst1.u32[0] = x1.u32[0] + y1.u32[0]
//! @endverbatim
static FOG_INLINE void p32Add_2x(
  p32& dst0, const p32& x0, const p32& y0,
  p32& dst1, const p32& x1, const p32& y1)
{
  uint32_t t0 = x0 + y0;
  uint32_t t1 = x1 + y1;

  dst0 = t0;
  dst1 = t1;
}

//! @brief Scalar subtract.
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] - y0.u32[0]
//! @endverbatim
static FOG_INLINE void p32Sub(
  p32& dst0, const p32& x0, const p32& y0)
{
  dst0 = x0 - y0;
}

//! @brief Scalar subtract (2x).
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] - y0.u32[0]
//! dst1.u32[0] = x1.u32[0] - y1.u32[0]
//! @endverbatim
static FOG_INLINE void p32Sub_2x(
  p32& dst0, const p32& x0, const p32& y0,
  p32& dst1, const p32& x1, const p32& y1)
{
  uint32_t t0 = x0 - y0;
  uint32_t t1 = x1 - y1;

  dst0 = t0;
  dst1 = t1;
}

//! @brief Scalar multiply.
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] * y0.u32[0]
//! @endverbatim
static FOG_INLINE void p32Mul(
  p32& dst0, const p32& x0, const p32& y0)
{
  dst0 = x0 * y0;
}

//! @brief Scalar multiply (2x).
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] * y0.u32[0]
//! dst1.u32[0] = x1.u32[0] * y1.u32[0]
//! @endverbatim
static FOG_INLINE void p32Mul_2x(
  p32& dst0, const p32& x0, const p32& y0,
  p32& dst1, const p32& x1, const p32& y1)
{
  uint32_t t0 = x0 * y0;
  uint32_t t1 = x1 * y1;

  dst0 = t0;
  dst1 = t1;
}

//! @brief Scalar divide.
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] / y0.u32[0]
//! @endverbatim
static FOG_INLINE void p32Div(
  p32& dst0, const p32& x0, const p32& y0)
{
  dst0 = x0 / y0;
}

//! @brief Scalar divide (2x).
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] / y0.u32[0]
//! dst1.u32[0] = x1.u32[0] / y1.u32[0]
//! @endverbatim
static FOG_INLINE void p32Div_2x(
  p32& dst0, const p32& x0, const p32& y0,
  p32& dst1, const p32& x1, const p32& y1)
{
  dst0 = x0 / y0;
  dst1 = x1 / y1;
}

// ============================================================================
// [Fog::Face - P32 - Cvt]
// ============================================================================

static FOG_INLINE void p32Cvt256SBWFrom255SBW(
  p32& dst0, const p32& x0)
{
  dst0 = x0 + (x0 > 127);
}

static FOG_INLINE void p32Cvt256PBWFrom255PBW(
  p32& dst0, const p32& x0)
{
  dst0 = x0 + ((x0 >> 7) & 0x00010001U);
}

static FOG_INLINE void p32Cvt256PBWFrom255PBW_2x(
  p32& dst0, const p32& x0,
  p32& dst1, const p32& x1)
{
  uint32_t t0 = x0 + ((x0 >> 7) & 0x00010001U);
  uint32_t t1 = x1 + ((x1 >> 7) & 0x00010001U);

  dst0 = t0;
  dst1 = t1;
}

static FOG_INLINE void p32Cvt255SBWFrom256SBW(
  p32& dst0, const p32& x0)
{
  dst0 = x0 - (x0 > 127);
}

static FOG_INLINE void p32Cvt255PBWFrom256PBW(
  p32& dst0, const p32& x0)
{
  dst0 = x0 - ((x0 >> 7) & 0x00010001U);
}

static FOG_INLINE void p32Cvt255PBWFrom256PBW_2x(
  p32& dst0, const p32& x0,
  p32& dst1, const p32& x1)
{
  uint32_t t0 = x0 - ((x0 >> 7) & 0x00010001U);
  uint32_t t1 = x1 - ((x1 >> 7) & 0x00010001U);

  dst0 = t0;
  dst1 = t1;
}

// ============================================================================
// [Fog::Face - P32 - Extend]
// ============================================================================

//! @brief Extend the LO byte in @c x0 and copy result into @c dst0.
//!
//! @verbatim
//! dst0 = (x0 << 24) | (x0 << 16) | (x0 << 8) | x0
//! @endverbatim
static FOG_INLINE void p32ExtendPBBFromSBB(
  p32& dst0, const p32& x0)
{
#if defined(FOG_FACE_HAS_FAST_MUL)
  dst0 = x0 * 0x01010101U;
#else
  dst0 = x0;
  dst0 = _FOG_FACE_COMBINE_2(dst0, dst0 << 8);
  dst0 = _FOG_FACE_COMBINE_2(dst0, dst0 << 16);
#endif
}

static FOG_INLINE void p32ExtendPBBFromSBB_ZZ10(
  p32& dst0, const p32& x0)
{
#if defined(FOG_FACE_HAS_FAST_MUL)
  dst0 = x0 * 0x00000101U;
#else
  dst0 = _FOG_FACE_COMBINE_2(x0  , x0 <<  8);
#endif
}

static FOG_INLINE void p32ExtendPBBFromSBB_Z210(
  p32& dst0, const p32& x0)
{
#if defined(FOG_FACE_HAS_FAST_MUL)
  dst0 = x0 * 0x00010101U;
#else
  uint32_t t0 = x0;
  dst0 = _FOG_FACE_COMBINE_2(t0  , t0 <<  8);
  dst0 = _FOG_FACE_COMBINE_2(dst0, t0 << 16);
#endif
}

//! @brief Extend src0.B0 to dst0, src0.B1 must be zero before call.
//!
//! @verbatim
//! dst0.u16[0] = x0.u16[0]
//! dst0.u16[1] = x0.u16[0]
//! @endverbatim
static FOG_INLINE void p32ExtendPBWFromSBW(
  p32& dst0, const p32& x0)
{
#if defined(FOG_FACE_HAS_FAST_MUL)
  dst0 = x0 * 0x00010001U;
#else
  dst0 = _FOG_FACE_COMBINE_2(x0, x0 << 16);
#endif
}

// ============================================================================
// [Fog::Face - P32 - Expand]
// ============================================================================

static FOG_INLINE void p32ExpandPBBFromPBB0(
  p32& dst0, const p32& x0)
{
  dst0 = x0 & 0xFFU;
  p32ExtendPBBFromSBB(dst0, dst0);
}

static FOG_INLINE void p32ExpandPBBFromPBB1(
  p32& dst0, const p32& x0)
{
  dst0 = (x0 >> 8) & 0xFFU;
  p32ExtendPBBFromSBB(dst0, dst0);
}

static FOG_INLINE void p32ExpandPBBFromPBB2(
  p32& dst0, const p32& x0)
{
  dst0 = (x0 >> 16) & 0xFFU;
  p32ExtendPBBFromSBB(dst0, dst0);
}

static FOG_INLINE void p32ExpandPBBFromPBB3(
  p32& dst0, const p32& x0)
{
  dst0 = x0 >> 24;
  p32ExtendPBBFromSBB(dst0, dst0);
}

//! @brief Expand scalar byte into p32.
//!
//! @verbatim
//! dst0.u16[0] = x0.u16[0]
//! dst0.u16[1] = x0.u16[0]
//! @endverbatim
static FOG_INLINE void p32ExpandPBWFromPBW0(
  p32& dst0, const p32& x0)
{
#if defined(FOG_FACE_HAS_FAST_MUL)
  dst0 = (x0 & 0xFFU) * 0x00010001U;
#else
  dst0 = _FOG_FACE_COMBINE_2(x0 & 0xFFU, x0 << 16);
#endif
}

//! @brief Expand src0.B0 to [dst0.B0, dst0.B1] and [dst1.B0, dst1.B1].
//!
//! @verbatim
//! dst0.u16[0] = x0.u16[0]
//! dst0.u16[1] = x0.u16[0]
//! dst1.u16[0] = x1.u16[0]
//! dst1.u16[1] = x1.u16[0]
//! @endverbatim
static FOG_INLINE void p32ExpandPBWFromPBW0_2x(
  p32& dst0, const p32& x0,
  p32& dst1, const p32& x1)
{
  p32ExpandPBWFromPBW0(dst0, x0);
  p32ExpandPBWFromPBW0(dst1, x1);
}

//! @brief Expand src0.B1 to [dst0.B0, dst0.B1].
//!
//! @verbatim
//! dst0.u16[0] = x0.u16[1]
//! dst0.u16[1] = x0.u16[1]
//! @endverbatim
static FOG_INLINE void p32ExpandPBWFromPBW1(
  p32& dst0, const p32& x0)
{
#if defined(FOG_FACE_HAS_FAST_MUL)
  dst0 = (x0 >> 16) * 0x00010001U;
#else
  dst0 = _FOG_FACE_COMBINE_2(x0 & 0x00FF0000U, x0 >> 16);
#endif
}

//! @brief Expand src0.B1 to [dst0.B0, dst0.B1] and [dst1.B0, dst1.B1].
//!
//! @verbatim
//! dst0.u16[0] = x0.u16[1]
//! dst0.u16[1] = x0.u16[1]
//! dst1.u16[0] = x1.u16[1]
//! dst1.u16[1] = x1.u16[1]
//! @endverbatim
static FOG_INLINE void p32ExpandPBWFromPBW1_2x(
  p32& dst0, const p32& x0,
  p32& dst1, const p32& x1)
{
  p32ExpandPBWFromPBW1(dst0, x0);
  p32ExpandPBWFromPBW1(dst1, x1);
}

// ============================================================================
// [Fog::Face - P32 - LShift / RShift]
// ============================================================================

static FOG_INLINE void p32LShift(
  p32& dst0, const p32& x0, uint32_t s0)
{
  dst0 = x0 << s0;
}

static FOG_INLINE void p32LShift_2x(
  p32& dst0, const p32& x0, uint32_t s0,
  p32& dst1, const p32& x1, uint32_t s1)
{
  dst0 = x0 << s0;
  dst1 = x1 << s1;
}

static FOG_INLINE void p32LShiftTruncatePBW(
  p32& dst0, const p32& x0, uint32_t s0)
{
  FOG_ASSERT(s0 <= 8);

  dst0 = (x0 << s0) & 0x00FF00FFU;
}

static FOG_INLINE void p32LShiftTruncatePBW_2x(
  p32& dst0, const p32& x0, uint32_t s0,
  p32& dst1, const p32& x1, uint32_t s1)
{
  FOG_ASSERT(s0 <= 8);
  FOG_ASSERT(s1 <= 8);

  dst0 = (x0 << s0) & 0x00FF00FFU;
  dst1 = (x1 << s1) & 0x00FF00FFU;
}

static FOG_INLINE void p32LShiftBy1(
  p32& dst0, const p32& x0)
{
#if defined(FOG_FACE_HAS_FAST_ADD)
  dst0 = x0 + x0;
#else
  dst0 = x0 << 1;
#endif // FOG_FACE_HAS_FAST_ADD
}

static FOG_INLINE void p32LShiftBy1_2x(
  p32& dst0, const p32& x0,
  p32& dst1, const p32& x1)
{
#if defined(FOG_FACE_HAS_FAST_ADD)
  dst0 = x0 + x0;
  dst1 = x1 + x1;
#else
  dst0 = x0 << 1;
  dst1 = x1 << 1;
#endif // FOG_FACE_HAS_FAST_ADD
}

static FOG_INLINE void p32LShiftBy1PBW0(
  p32& dst0, const p32& x0)
{
  dst0 = x0 + (x0 & 0x000000FFU);
}

static FOG_INLINE void p32LShiftBy1PBW0_2x(
  p32& dst0, const p32& x0,
  p32& dst1, const p32& x1)
{
  dst0 = x0 + (x0 & 0x000000FFU);
  dst1 = x1 + (x1 & 0x000000FFU);
}

static FOG_INLINE void p32LShiftBy1PBW1(
  p32& dst0, const p32& x0)
{
  dst0 = x0 + (x0 & 0x00FF0000U);
}

static FOG_INLINE void p32LShiftBy1PBW1_2x(
  p32& dst0, const p32& x0,
  p32& dst1, const p32& x1)
{
  dst0 = x0 + (x0 & 0x00FF0000U);
  dst1 = x1 + (x1 & 0x00FF0000U);
}

static FOG_INLINE void p32RShift(
  p32& dst0, const p32& x0, uint32_t s0)
{
  dst0 = x0 >> s0;
}

static FOG_INLINE void p32RShift_2x(
  p32& dst0, const p32& x0, uint32_t s0,
  p32& dst1, const p32& x1, uint32_t s1)
{
  dst0 = x0 >> s0;
  dst1 = x1 >> s1;
}

static FOG_INLINE void p32RShiftTruncatePBW(
  p32& dst0, const p32& x0, uint32_t s0)
{
  FOG_ASSERT(s0 <= 8);

  dst0 = (x0 >> s0) & 0x00FF00FFU;
}

static FOG_INLINE void p32RShiftTruncatePBW_2x(
  p32& dst0, const p32& x0, uint32_t s0,
  p32& dst1, const p32& x1, uint32_t s1)
{
  FOG_ASSERT(s0 <= 8);
  FOG_ASSERT(s1 <= 8);

  dst0 = (x0 >> s0) & 0x00FF00FFU;
  dst1 = (x1 >> s1) & 0x00FF00FFU;
}

// ============================================================================
// [Fog::Face - P32 - Negate255/256]
// ============================================================================

//! @verbatim
//! dst0.u16[0] = 255 - x0.u16[0]
//! @endverbatim
static FOG_INLINE void p32Negate255SBW(
  p32& dst0, const p32& x0)
{
  dst0 = x0 ^ 0xFFU;
}

//! @verbatim
//! dst0.u16[0] = 255 - x0.u16[0]
//! dst1.u16[0] = 255 - x1.u16[0]
//! @endverbatim
static FOG_INLINE void p32Negate255SBW_2x(
  p32& dst0, const p32& x0,
  p32& dst1, const p32& x1)
{
  p32Negate255SBW(dst0, x0);
  p32Negate255SBW(dst1, x1);
}

//! @verbatim
//! dst0.u16[0] = 256 - x0.u16[0]
//! @endverbatim
static FOG_INLINE void p32Negate256SBW(
  p32& dst0, const p32& x0)
{
  dst0 = 256U - x0;
}

//! @verbatim
//! dst0.u16[0] = 256 - x0.u16[0]
//! dst1.u16[0] = 256 - x1.u16[0]
//! @endverbatim
static FOG_INLINE void p32Negate256SBW_2x(
  p32& dst0, const p32& x0,
  p32& dst1, const p32& x1)
{
  p32Negate256SBW(dst0, x0);
  p32Negate256SBW(dst1, x1);
}

//! @verbatim
//! dst0.u8[0] = 255 - x0.u8[0]
//! dst0.u8[1] = 255 - x0.u8[1]
//! dst0.u8[2] = 255 - x0.u8[2]
//! dst0.u8[3] = 255 - x0.u8[3]
//! @endverbatim
static FOG_INLINE void p32Negate255PBB(
  p32& dst0, const p32& x0)
{
  dst0 = ~x0;
}

//! @verbatim
//! dst0.u8[0] = 255 - x0.u8[0]
//! dst0.u8[1] = 255 - x0.u8[1]
//! dst0.u8[2] = 255 - x0.u8[2]
//! dst0.u8[3] = 255 - x0.u8[3]
//! dst1.u8[0] = 255 - x1.u8[0]
//! dst1.u8[1] = 255 - x1.u8[1]
//! dst1.u8[2] = 255 - x1.u8[2]
//! dst1.u8[3] = 255 - x1.u8[3]
//! @endverbatim
static FOG_INLINE void p32Negate255PBB_2x(
  p32& dst0, const p32& x0,
  p32& dst1, const p32& x1)
{
  dst0 = ~x0;
  dst1 = ~x1;
}

//! @verbatim
//! dst0.u16[0] = 255 - x0.u16[0]
//! dst0.u16[1] = 255 - x0.u16[1]
//! @endverbatim
static FOG_INLINE void p32Negate255PBW(
  p32& dst0, const p32& x0)
{
  dst0 = x0 ^ 0x00FF00FFU;
}

//! @verbatim
//! dst0.u16[0] = 255 - x0.u16[0]
//! dst0.u16[1] = 255 - x0.u16[1]
//! dst1.u16[0] = 255 - x1.u16[0]
//! dst1.u16[1] = 255 - x1.u16[1]
//! @endverbatim
static FOG_INLINE void p32Negate255PBW_2x(
  p32& dst0, const p32& x0,
  p32& dst1, const p32& x1)
{
  p32Negate255PBW(dst0, x0);
  p32Negate255PBW(dst1, x1);
}

//! @verbatim
//! dst0.u16[0] = 256 - x0.u16[0]
//! dst0.u16[1] = 256 - x0.u16[1]
//! @endverbatim
static FOG_INLINE void p32Negate256PBW(
  p32& dst0, const p32& x0)
{
  dst0 = 0x01000100U - x0;
}

//! @verbatim
//! dst0.u16[0] = 256 - x0.u16[0]
//! dst0.u16[1] = 256 - x0.u16[1]
//! dst1.u16[0] = 256 - x1.u16[0]
//! dst1.u16[1] = 256 - x1.u16[1]
//! @endverbatim
static FOG_INLINE void p32Negate256PBW_2x(
  p32& dst0, const p32& x0,
  p32& dst1, const p32& x1)
{
  dst0 = 0x01000100U - x0;
  dst1 = 0x01000100U - x1;
}

static FOG_INLINE void p32Negate255PBW0(
  p32& dst0, const p32& x0)
{
  dst0 = x0 ^ 0x000000FFU;
}

static FOG_INLINE void p32Negate255PBW0_2x(
  p32& dst0, const p32& x0,
  p32& dst1, const p32& x1)
{
  dst0 = x0 ^ 0x000000FFU;
  dst1 = x1 ^ 0x000000FFU;
}

static FOG_INLINE void p32Negate255PBW1(
  p32& dst0, const p32& x0)
{
  dst0 = x0 ^ 0x00FF0000U;
}

static FOG_INLINE void p32Negate255PBW1_2x(
  p32& dst0, const p32& x0,
  p32& dst1, const p32& x1)
{
  dst0 = x0 ^ 0x00FF0000U;
  dst1 = x1 ^ 0x00FF0000U;
}

// ============================================================================
// [Fog::Face - P32 - Min]
// ============================================================================

//! @brief Take smaller value from @a u0 and @a u and store it to @a dst0.
//!
//! @verbatim
//! dst0 = min(x0, u0)
//! @endverbatim
static FOG_INLINE void p32MinPBW_SBW(
  p32& dst0, const p32& x0, const p32& u0)
{
  uint32_t x0Lo = x0 & 0xFFU;
  uint32_t x0Hi = x0 >> 16;

  if (x0Lo > u0) x0Lo = u0;
  if (x0Hi > u0) x0Hi = u0;

  dst0 = _FOG_FACE_COMBINE_2(x0Lo, x0Hi << 16);
}

//! @brief Take smaller value from @a x0/u0 and @a x1/u1 and store it to @a dst0/dst1.
//!
//! @verbatim
//! dst0 = min(x0, u0)
//! dst1 = min(x1, u1)
//! @endverbatim
static FOG_INLINE void p32MinPBW_SBW_2x(
  p32& dst0, const p32& x0, const p32& u0,
  p32& dst1, const p32& x1, const p32& u1)
{
  uint32_t x0Lo = x0 & 0x000000FFU;
  uint32_t x1Lo = x1 & 0x000000FFU;

  if (x0Lo > u0) x0Lo = u0;
  if (x1Lo > u1) x1Lo = u1;

  uint32_t x0Hi = x0 & 0x00FF0000U;
  uint32_t x1Hi = x1 & 0x00FF0000U;

  uint32_t t0Hi = u0 << 16;
  uint32_t t1Hi = u1 << 16;

  if (x0Hi > t0Hi) x0Hi = t0Hi;
  if (x1Hi > t1Hi) x1Hi = t1Hi;

  dst0 = _FOG_FACE_COMBINE_2(x0Lo, x0Hi);
  dst1 = _FOG_FACE_COMBINE_2(x1Lo, x1Hi);
}

//! @brief Take smaller value from @a x0/y0 and store it to @a dst0.
//!
//! @verbatim
//! dst0 = min(x0, y0)
//! @endverbatim
static FOG_INLINE void p32MinPBW(
  p32& dst0, const p32& x0, const p32& y0)
{
  uint32_t x0Lo = x0 & 0x000000FFU;
  uint32_t y0Lo = y0 & 0x000000FFU;

  uint32_t x0Hi = x0 & 0x00FF0000U;
  uint32_t y0Hi = y0 & 0x00FF0000U;

  if (x0Lo > y0Lo) x0Lo = y0Lo;
  if (x0Hi > y0Hi) x0Hi = y0Hi;

  dst0 = _FOG_FACE_COMBINE_2(x0Lo, x0Hi);
}

//! @brief Take smaller value from @a x0/y0 and @c x1/y1 and store it to @a dst0/dst1.
//!
//! @verbatim
//! dst0 = min(x0, y0)
//! dst1 = min(x1, y1)
//! @endverbatim
static FOG_INLINE void p32MinPBW_2x(
  p32& dst0, const p32& x0, const p32& y0,
  p32& dst1, const p32& x1, const p32& y1)
{
  p32MinPBW(dst0, x0, y0);
  p32MinPBW(dst1, x1, y1);
}

// ============================================================================
// [Fog::Face - P32 - Max]
// ============================================================================

//! @brief Take larger value from @a x0/u0 and store it to @a dst0.
//!
//! @verbatim
//! dst0 = max(x0, u0)
//! @endverbatim
static FOG_INLINE void p32MaxPBW_SBW(
  p32& dst0, const p32& x0, const p32& u0)
{
  uint32_t x0Lo = x0 & 0x000000FFU;
  uint32_t x0Hi = x0 >> 16;

  if (x0Lo < u0) x0Lo = u0;
  if (x0Hi < u0) x0Hi = u0;

  dst0 = _FOG_FACE_COMBINE_2(x0Lo, x0Hi << 16);
}

//! @brief Take larger value from @a x0/u0 and @a x1/u1 and store it to @a dst0/dst1.
//!
//! @verbatim
//! dst0 = max(x0, u0)
//! dst1 = max(x1, u1)
//! @endverbatim
static FOG_INLINE void p32MaxPBW_SBW_2x(
  p32& dst0, const p32& x0, const p32& u0,
  p32& dst1, const p32& x1, const p32& u1)
{
  uint32_t x0Lo = x0 & 0x000000FFU;
  uint32_t x1Lo = x1 & 0x000000FFU;

  if (x0Lo < u0) x0Lo = u0;
  if (x1Lo < u1) x1Lo = u1;

  uint32_t x0Hi = x0 & 0x00FF0000U;
  uint32_t x1Hi = x1 & 0x00FF0000U;

  uint32_t t0Hi = u0 << 16;
  uint32_t t1Hi = u1 << 16;

  if (x0Hi < t0Hi) x0Hi = t0Hi;
  if (x1Hi < t1Hi) x1Hi = t1Hi;

  dst0 = _FOG_FACE_COMBINE_2(x0Lo, x0Hi);
  dst1 = _FOG_FACE_COMBINE_2(x1Lo, x1Hi);
}

//! @brief Take larger value from @a x0/y0 and store it to @a dst0.
//!
//! @verbatim
//! dst0 = max(x0, y0)
//! @endverbatim
static FOG_INLINE void p32MaxPBW(
  p32& dst0, const p32& x0, const p32& y0)
{
  uint32_t x0Lo = x0 & 0x000000FFU;
  uint32_t x0Hi = x0 & 0x00FF0000U;

  uint32_t y0Lo = y0 & 0x000000FFU;
  uint32_t y0Hi = y0 & 0x00FF0000U;

  if (x0Lo < y0Lo) x0Lo = y0Lo;
  if (x0Hi < y0Hi) x0Hi = y0Hi;

  dst0 = _FOG_FACE_COMBINE_2(x0Lo, x0Hi);
}

//! @brief Take larger value from @a x0/y0 and @a x1/y1 and store it to @a dst0/dst1.
//!
//! @verbatim
//! dst0 = max(x0, y0)
//! dst1 = max(x1, y1)
//! @endverbatim
static FOG_INLINE void p32MaxPBW_2x(
  p32& dst0, const p32& x0, const p32& y0,
  p32& dst1, const p32& x1, const p32& y1)
{
  p32MaxPBW(dst0, x0, y0);
  p32MaxPBW(dst1, x1, y1);
}

// ============================================================================
// [Fog::Face - P32 - Saturate]
// ============================================================================

static FOG_INLINE void p32Saturate255SBW(
  p32& dst0, const p32& x0)
{
#if defined(FOG_FACE_HAS_CMOV)
  dst0 = x0;
  if (dst0 > 0xFF) dst0 = 0xFF;
#else
  dst0 = (x0 | (0x0100U - ((x0 >> 8)))) & 0xFFU;
#endif
}

//! @brief Saturate @a dst0. Can be used after two BYTEs were added and
//! resulting value may be larger than BYTE.
//!
//! @verbatim
//! dst0 = min(dst0, 255)
//! @endverbatim
static FOG_INLINE void p32Saturate255PBW(
  p32& dst0, const p32& x0)
{
  // NOTE: The following code will work (and is more precise) too:
  //
  // dst0 = (x0 | (0x01000100U - ((x0 >> 8) & 0x00010001U))) & 0x00FF00FFU;
  //
  // The reason why the 0x00FF00FF is also used to mask (x0 >> 8) is that the
  // mask can be stored in register so it will be reused by another computation.

  dst0 = (x0 | (0x01000100U - ((x0 >> 8) & 0x00FF00FFU))) & 0x00FF00FFU;
}

//! @brief Saturate @a dst0/dst1. Can be used after two BYTEs were added
//! and resulting value may be larger than BYTE.
//!
//! @verbatim
//! dst0 = min(dst0, 255)
//! dst1 = min(dst1, 255)
//! @endverbatim
static FOG_INLINE void p32Saturate255PBW_2x(
  p32& dst0, const p32& x0,
  p32& dst1, const p32& x1)
{
  p32Saturate255PBW(dst0, x0);
  p32Saturate255PBW(dst1, x1);
}

//! @brief Saturate @a dst0. Can be used after two BYTEs were added and
//! resulting value may be larger than BYTE.
//!
//! @verbatim
//! dst0 = min(dst0, 511)
//! @endverbatim
static FOG_INLINE void p32Saturate511PBW(
  p32& dst0, const p32& x0)
{
  dst0 = (x0 | (0x02000200U - ((x0 >> 9) & 0x00010001U))) & 0x01FF01FFU;
}

//! @brief Saturate @a dst0/dst1. Can be used after two BYTEs were added
//! and resulting value may be larger than BYTE.
//!
//! @verbatim
//! dst0 = min(dst0, 511)
//! dst1 = min(dst1, 511)
//! @endverbatim
static FOG_INLINE void p32Saturate511PBW_2x(
  p32& dst0, const p32& x0,
  p32& dst1, const p32& x1)
{
  p32Saturate511PBW(dst0, x0);
  p32Saturate511PBW(dst1, x1);
}

// ============================================================================
// [Fog::Face - P32 - Add / Addus]
// ============================================================================

static FOG_INLINE void p32AddPBW_SBW(
  p32& dst0, const p32& x0, const p32& u0)
{
  p32 t0;
  p32ExtendPBWFromSBW(t0, u0);
  p32Add(dst0, x0, t0);
}

static FOG_INLINE void p32AddPBW_SBW_2x(
  p32& dst0, const p32& x0,
  p32& dst1, const p32& x1, const p32& u01)
{
  p32 t01;
  p32ExtendPBWFromSBW(t01, u01);
  p32Add_2x(dst0, x0, t01, dst1, x1, t01);
}

static FOG_INLINE void p32AddPBW_SBW_2x(
  p32& dst0, const p32& x0, const p32& u0,
  p32& dst1, const p32& x1, const p32& u1)
{
  p32 t0, t1;
  p32ExtendPBWFromSBW(t0, u0);
  p32ExtendPBWFromSBW(t1, u1);
  p32Add_2x(dst0, x0, t0, dst1, x1, t1);
}

static FOG_INLINE void p32Addus255SBW(
  p32& dst0, const p32& x0, const p32& y0)
{
  p32Add(dst0, x0, y0);
  p32Saturate255SBW(dst0, dst0);
}

static FOG_INLINE void p32Addus255PBB(
  p32& dst0, const p32& x0, const p32& y0)
{
#if defined(FOG_FACE_HAS_64BIT)
  uint64_t t0 = ((uint64_t)x0 | ((uint64_t)x0 << 24)) & FOG_UINT64_C(0x00FF00FF00FF00FF);
  uint64_t t1 = ((uint64_t)y0 | ((uint64_t)y0 << 24)) & FOG_UINT64_C(0x00FF00FF00FF00FF);

  t0 += t1;
  t0 |= FOG_UINT64_C(0x0100010001000100) - ((t0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF));
  t0 &= FOG_UINT64_C(0x00FF00FF00FF00FF);

  dst0 = (uint32_t)(_FOG_FACE_COMBINE_2(x0, x0 >> 24));
#else
  uint32_t t0 = (x0     ) & 0x00FF00FFU;
  uint32_t t1 = (x0 >> 8) & 0x00FF00FFU;

  t0 += (y0     ) & 0x00FF00FFU;
  t1 += (y0 >> 8) & 0x00FF00FFU;

  t0 |= 0x01000100U - ((t0 >> 8) & 0x00FF00FFU);
  t1 |= 0x01000100U - ((t1 >> 8) & 0x00FF00FFU);

  t0 = (t0 & 0x00FF00FFU);
  t1 = (t1 & 0x00FF00FFU) << 8;

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
#endif
}

static FOG_INLINE void p32Addus255PBW(
  p32& dst0, const p32& x0, const p32& y0)
{
  p32Add(dst0, x0, y0);
  p32Saturate255PBW(dst0, dst0);
}

static FOG_INLINE void p32Addus255PBW_2x(
  p32& dst0, const p32& x0, const p32& y0,
  p32& dst1, const p32& x1, const p32& y1)
{
  p32Add_2x(dst0, x0, y0, dst1, x1, y1);
  p32Saturate255PBW_2x(dst0, dst0, dst1, dst1);
}

static FOG_INLINE void p32Addus255PBW_SBW(
  p32& dst0, const p32& x0, const p32& u0)
{
  p32 t0;
  p32ExtendPBWFromSBW(t0, u0);
  p32Addus255PBW(dst0, x0, t0);
}

static FOG_INLINE void p32Addus255PBW_SBW_2x(
  p32& dst0, const p32& x0, const p32& u0,
  p32& dst1, const p32& x1, const p32& u1)
{
  p32 t0, t1;
  p32ExtendPBWFromSBW(t0, u0);
  p32ExtendPBWFromSBW(t1, u1);
  p32Addus255PBW_2x(dst0, x0, t0, dst1, x1, t1);
}

// ============================================================================
// [Fog::Face - P32 - Sub / Subus]
// ============================================================================

static FOG_INLINE void p32SubPBW_SBW(
  p32& dst0, const p32& x0, const p32& u0)
{
  p32 t0;
  p32ExtendPBWFromSBW(t0, u0);
  p32Sub(dst0, x0, t0);
}

static FOG_INLINE void p32SubPBW_SBW_2x(
  p32& dst0, const p32& x0,
  p32& dst1, const p32& x1, const p32& u01)
{
  p32 t01;
  p32ExtendPBWFromSBW(t01, u01);
  p32Sub_2x(dst0, x0, t01, dst1, x1, t01);
}

static FOG_INLINE void p32SubPBW_SBW_2x(
  p32& dst0, const p32& x0, const p32& u0,
  p32& dst1, const p32& x1, const p32& u1)
{
  p32 t0, t1;
  p32ExtendPBWFromSBW(t0, u0);
  p32ExtendPBWFromSBW(t1, u1);
  p32Sub_2x(dst0, x0, t0, dst1, x1, t1);
}

static FOG_INLINE void p32Subus255PBW(
  p32& dst0, const p32& x0, const p32& y0)
{
  p32Negate255PBW(dst0, x0);
  p32Add(dst0, dst0, y0);
  p32Saturate255PBW(dst0, dst0);
  p32Negate255PBW(dst0, dst0);
}

static FOG_INLINE void p32Subus255PBW_2x(
  p32& dst0, const p32& x0, const p32& y0,
  p32& dst1, const p32& x1, const p32& y1)
{
  p32Negate255PBW_2x(dst0, x0, dst1, x1);
  p32Add_2x(dst0, dst0, y0, dst1, dst1, y1);
  p32Saturate255PBW_2x(dst0, dst0, dst1, dst1);
  p32Negate255PBW_2x(dst0, dst0, dst1, dst1);
}

static FOG_INLINE void p32Subus255SBW(
  p32& dst0, const p32& x0, const p32& y0)
{
  dst0 = x0 - y0;

#if defined(FOG_FACE_HAS_CMOV)
  if ((int32_t)dst0 < 0) dst0 = 0;
#else
  dst0 &= (dst0 >> 24) ^ 0xFFU;
#endif
}

static FOG_INLINE void p32Subus255PBW_SBW(
  p32& dst0, const p32& x0, const p32& u0)
{
  p32 t0;
  p32ExtendPBWFromSBW(t0, u0);
  p32Subus255PBW(dst0, x0, t0);
}

static FOG_INLINE void p32Subus255PBW_SBW_2x(
  p32& dst0, const p32& x0, const p32& u0,
  p32& dst1, const p32& x1, const p32& u1)
{
  p32 t0, t1;
  p32ExtendPBWFromSBW(t0, u0);
  p32ExtendPBWFromSBW(t1, u1);
  p32Subus255PBW_2x(dst0, x0, t0, dst1, x1, t1);
}

// ============================================================================
// [Fog::Face - P32 - AddSub / AddSubus]
// ============================================================================

//! @brief Scalar add and subtract.
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] + y0.u32[0] - z0.u32[0]
//! @endverbatim
static FOG_INLINE void p32AddSub(
  p32& dst0, const p32& x0, const p32& y0, const p32& z0)
{
  dst0 = x0 + y0 - z0;
}

//! @brief Scalar add and subtract (2x).
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] + y0.u32[0] - z0.u32[0]
//! dst1.u32[0] = x1.u32[0] + y1.u32[0] - z1.u32[0]
//! @endverbatim
static FOG_INLINE void p32AddSub_2x(
  p32& dst0, const p32& x0, const p32& y0, const p32& z0,
  p32& dst1, const p32& x1, const p32& y1, const p32& z1)
{
  uint32_t t0 = x0 + y0 - z0;
  uint32_t t1 = x1 + y1 - z1;

  dst0 = t0;
  dst1 = t1;
}

//! @brief Scalar add, subtract and saturate.
//!
//! @verbatim
//! dst0.u16[0] = saturate255(x0.u16[0] + y0.u16[0] - z0.u16[0])
//! dst0.u16[1] = saturate255(x0.u16[1] + y0.u16[1] - z0.u16[1])
//! @endverbatim
static FOG_INLINE void p32AddSubus255PBW(
  p32& dst0, const p32& x0, const p32& y0, const p32& z0)
{
  p32 t0;

  p32Add(t0, x0, y0);
  p32Xor(t0, t0, 0x01FF01FFU);
  p32Add(t0, t0, z0);
  p32Saturate511PBW(t0, t0);
  p32Xor(t0, t0, 0x01FF01FFU);
  p32Saturate255PBW(t0, t0);
}

//! @brief Scalar add, subtract and saturate (2x).
//!
//! @verbatim
//! dst0.u16[0] = saturate255(x0.u16[0] + y0.u16[0] - z0.u16[0])
//! dst0.u16[1] = saturate255(x0.u16[1] + y0.u16[1] - z0.u16[1])
//! dst1.u16[0] = saturate255(x1.u16[0] + y1.u16[0] - z1.u16[0])
//! dst1.u16[1] = saturate255(x1.u16[1] + y1.u16[1] - z1.u16[1])
//! @endverbatim
static FOG_INLINE void p32AddSubus255PBW_2x(
  p32& dst0, const p32& x0, const p32& y0, const p32& z0,
  p32& dst1, const p32& x1, const p32& y1, const p32& z1)
{
  p32 t0, t1;
  p32AddSubus255PBW(t0, x0, y0, z0);
  p32AddSubus255PBW(t1, x1, y1, z1);

  dst0 = t0;
  dst1 = t1;
}

// ============================================================================
// [Fog::Face - P32 - Div]
// ============================================================================

//! @brief Scalar divide by 255.
//!
//! @verbatim
//! dst0.u32[0] = (x0.u32[0] / 255).
//! @endverbatim
static FOG_INLINE void p32Div255SBW(
  p32& dst0, const p32& x0)
{
  dst0 = ((x0 << 8U) + x0 + 256U) >> 16U;
}

//! @brief Scalar Divide by 256.
//!
//! @verbatim
//! dst0.u32[0] = (x0.u32[0] / 256).
//! @endverbatim
static FOG_INLINE void p32Div256SBW(
  p32& dst0, const p32& x0)
{
  dst0 = (x0 >> 8);
}

//! @brief Scalar Divide by 65535.
//!
//! @verbatim
//! dst0.u32[0] = (x0.u32[0] / 65535).
//! @endverbatim
static FOG_INLINE void p32Div65535SWD(
  p32& dst0, const p32& x0)
{
  dst0 = ((x0 + (x0 >> 16) + 0x8000U) >> 16);
}

//! @brief Scalar Divide by 65536.
//!
//! @verbatim
//! dst0.u32[0] = (x0.u32[0] / 65536).
//! @endverbatim
static FOG_INLINE void p32Div65536SWD(
  p32& dst0, const p32& x0)
{
  dst0 = (x0 >> 16);
}

// ============================================================================
// [Fog::Face - P32 - MulDiv]
// ============================================================================

//! @brief Scalar Multiply and divide by 255.
//!
//! @verbatim
//! dst0.u32[0] = (x0.u32[0] * u0.u32[0]) / 255
//! @endverbatim
static FOG_INLINE void p32MulDiv255SBW(
  p32 &dst0, const p32& x0, const p32& u0)
{
  dst0 = x0 * u0;
  dst0 = ((dst0 + (dst0 >> 8) + 0x80U) >> 8);
}

//! @brief Scalar Multiply and divide by 256.
//!
//! @verbatim
//! dst0 = (x0.u32[0] * u0.u32[0]) / 256
//! @endverbatim
static FOG_INLINE void p32MulDiv256SBW(
  p32& dst0, const p32& x0, const p32& u0)
{
  dst0 = (x0 * u0) >> 8;
}

//! @brief Scalar Multiply and divide by 65535.
//!
//! @verbatim
//! dst0 = (x0.u32[0] * u0.u32[0]) / 65535
//! @endverbatim
static FOG_INLINE void p32MulDiv65535SWD(
  p32& dst0, const p32& x0, const p32& u0)
{
  dst0 = x0 * u0;
  dst0 = ((dst0 + (dst0 >> 16) + 0x8000U) >> 16);
}

//! @brief Scalar Multiply and divide by 65536.
//!
//! @verbatim
//! dst0 = (x0.u32[0] * u0.u32[0]) / 65536
//! @endverbatim
static FOG_INLINE void p32MulDiv65536SWD(
  p32& dst0, const p32& x0, const p32& u0)
{
  dst0 = (x0 * u0) >> 16;
}

//! @brief Packed<-Scalar multiply and divide by 255.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * u0.p32[0]) / 255
//! dst0.u8[1] = (x0.u8[1] * u0.p32[0]) / 255
//! dst0.u8[2] = (x0.u8[2] * u0.p32[0]) / 255
//! dst0.u8[3] = (x0.u8[3] * u0.p32[0]) / 255
//! @endverbatim
static FOG_INLINE void p32MulDiv255PBB_SBW(
  p32& dst0, const p32& x0, const p32& u0)
{
#if defined(FOG_FACE_HAS_64BIT)
  uint64_t t0 = ((uint64_t)x0 | ((uint64_t)x0 << 24)) & FOG_UINT64_C(0x00FF00FF00FF00FF);
  t0 *= u0;
  t0 = (t0 + ((t0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF)) + FOG_UINT64_C(0x0080008000800080)) >> 8;
  t0 &= FOG_UINT64_C(0x00FF00FF00FF00FF);
  dst0 = (uint32_t)(_FOG_FACE_COMBINE_2(t0, t0 >> 24));
#else
  uint32_t t0 = ((x0     ) & 0x00FF00FFU) * u0;
  uint32_t t1 = ((x0 >> 8) & 0x00FF00FFU) * u0;

  t0 = ((t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U) & 0xFF00FF00U) >> 8;
  t1 = ((t1 + ((t1 >> 8) & 0x00FF00FFU) + 0x00800080U) & 0xFF00FF00U);

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
#endif
}

//! @brief Packed<-Scalar multiply and divide by 256.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * u0.p32[0]) / 256
//! dst0.u8[1] = (x0.u8[1] * u0.p32[0]) / 256
//! dst0.u8[2] = (x0.u8[2] * u0.p32[0]) / 256
//! dst0.u8[3] = (x0.u8[3] * u0.p32[0]) / 256
//! @endverbatim
static FOG_INLINE void p32MulDiv256PBB_SBW(
  p32& dst0, const p32& x0, const p32& u0)
{
  uint32_t t0 = ((x0     ) & 0x00FF00FFU) * u0;
  uint32_t t1 = ((x0 >> 8) & 0x00FF00FFU) * u0;

  t0 = (t0 & 0xFF00FF00U) >> 8;
  t1 = (t1 & 0xFF00FF00U);

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
}

//! @brief Packed<-Scalar multiply and divide by 255.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * u0.p32[0]) / 255
//! dst0.u8[1] = (x0.u8[1] * u0.p32[0]) / 255
//! dst0.u8[2] = (x0.u8[2] * u0.p32[0]) / 255
//! dst0.u8[3] = 0x00
//! @endverbatim
static FOG_INLINE void p32MulDiv255PBB_SBW_Z210(
  p32& dst0, const p32& x0, const p32& u0)
{
#if defined(FOG_FACE_HAS_64BIT)
  uint64_t t0 = ((uint64_t)x0 | ((uint64_t)x0 << 24)) & FOG_UINT64_C(0x000000FF00FF00FF);
  t0 *= u0;
  t0 = (t0 + ((t0 >> 8) & FOG_UINT64_C(0x000000FF00FF00FF)) + FOG_UINT64_C(0x0000008000800080)) >> 8;
  t0 &= FOG_UINT64_C(0x000000FF00FF00FF);
  dst0 = (uint32_t)(_FOG_FACE_COMBINE_2(t0, t0 >> 24));
#else
  uint32_t t0 = ((x0     ) & 0x00FF00FFU) * u0;
  uint32_t t1 = ((x0 >> 8) & 0x000000FFU) * u0;

  t0 = ((t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U) & 0xFF00FF00U) >> 8;
  t1 = ((t1 + ((t1 >> 8)              ) + 0x00000080U) & 0xFF00FF00U);

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
#endif
}

//! @brief Packed<-Scalar multiply and divide by 256.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * u0.p32[0]) / 256
//! dst0.u8[1] = (x0.u8[1] * u0.p32[0]) / 256
//! dst0.u8[2] = (x0.u8[2] * u0.p32[0]) / 256
//! dst0.u8[3] = 0x00
//! @endverbatim
static FOG_INLINE void p32MulDiv256PBB_SBW_Z210(
  p32& dst0, const p32& x0, const p32& u0)
{
  uint32_t t0 = (x0 & 0x00FF00FFU) * u0;
  uint32_t t1 = (x0 & 0x0000FF00U) * u0;

  t0 = (t0 & 0xFF00FF00U);
  t1 = (t1 & 0x00FF0000U);

  dst0 = _FOG_FACE_COMBINE_2(t0, t1) >> 8;
}

//! @brief Packed<-Scalar multiply and divide by 255.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * u0.p32[0]) / 255
//! dst0.u8[1] = (x0.u8[1] * u0.p32[0]) / 255
//! dst0.u8[2] = (x0.u8[2] * u0.p32[0]) / 255
//! dst0.u8[3] = 0xFF
//! @endverbatim
static FOG_INLINE void p32MulDiv255PBB_SBW_F210(
  p32& dst0, const p32& x0, const p32& u0)
{
#if defined(FOG_FACE_HAS_64BIT)
  uint64_t t0 = ((uint64_t)x0 | ((uint64_t)x0 << 24)) & FOG_UINT64_C(0x000000FF00FF00FF);
  t0 *= u0;
  t0 = (t0 + ((t0 >> 8) & FOG_UINT64_C(0x000000FF00FF00FF)) + FOG_UINT64_C(0xFF00008000800080)) >> 8;
  t0 &= FOG_UINT64_C(0x00FF00FF00FF00FF);
  dst0 = (uint32_t)_FOG_FACE_COMBINE_2(t0, t0 >> 24);
#else
  uint32_t t0 = ((x0     ) & 0x00FF00FFU) * u0;
  uint32_t t1 = ((x0 >> 8) & 0x000000FFU) * u0;

  t0 = ((t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U) & 0xFF00FF00U) >> 8;
  t1 = ((t1 + ((t1 >> 8)              ) + 0xFF000080U) & 0xFF00FF00U);

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
#endif
}

//! @brief Packed<-Scalar multiply and divide by 256.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * u0.p32[0]) / 256
//! dst0.u8[1] = (x0.u8[1] * u0.p32[0]) / 256
//! dst0.u8[2] = (x0.u8[2] * u0.p32[0]) / 256
//! dst0.u8[3] = 0xFF
//! @endverbatim
static FOG_INLINE void p32MulDiv256PBB_SBW_F210(
  p32& dst0, const p32& x0, const p32& u0)
{
  uint32_t t0 = (x0 & 0x00FF00FFU) * u0;
  uint32_t t1 = (x0 & 0x0000FF00U) * u0;

  t0 = (t0 & 0xFF00FF00U);
  t1 = (t1 & 0x00FF0000U);

  dst0 = (_FOG_FACE_COMBINE_2(t0, t1) >> 8) | 0xFF000000;
}

//! @brief Packed multiply and divide by 255.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * y0.u8[0]) / 255
//! dst0.u8[1] = (x0.u8[1] * y0.u8[1]) / 255
//! dst0.u8[2] = (x0.u8[2] * y0.u8[2]) / 255
//! dst0.u8[3] = (x0.u8[3] * y0.u8[3]) / 255
//! @endverbatim
static FOG_INLINE void p32MulDiv255PBB(
  p32& dst0, const p32& x0, const p32& y0)
{
#if defined(FOG_FACE_HAS_64BIT)
  uint64_t t0 = (uint64_t)(x0 & 0x000000FFU) * (uint64_t)(y0 & 0x000000FFU); // 0x000000000000____;
  uint64_t t1 = (uint64_t)(x0 & 0x0000FF00U) * (uint64_t)(y0 & 0x0000FF00U); // 0x00000000____0000;
  uint64_t t2 = (uint64_t)(x0 & 0x00FF0000U) * (uint64_t)(y0 & 0x00FF0000U); // 0x0000____00000000;
  uint64_t t3 = (uint64_t)(x0 & 0xFF000000U) * (uint64_t)(y0 & 0xFF000000U); // 0x____000000000000;

  t0 = _FOG_FACE_COMBINE_2(t0, t1);
  t2 = _FOG_FACE_COMBINE_2(t2, t3);
  t0 = _FOG_FACE_COMBINE_2(t0, t2);

  t0 = t0 + ((t0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF)) + FOG_UINT64_C(0x0080008000800080);

  dst0 = ((uint32_t)(t0 >>  8) & 0x000000FFU) |
         ((uint32_t)(t0 >> 16) & 0x0000FF00U) |
         ((uint32_t)(t0 >> 24) & 0x00FF0000U) |
         ((uint32_t)(t0 >> 32) & 0xFF000000U) ;
#else
  uint32_t t0 = (x0 & 0x000000FFU) * (y0 & 0x000000FFU);
  uint32_t t1 = (x0 & 0x0000FF00U) * (y0 & 0x0000FF00U);

  uint32_t t2 = (x0 & 0x00FF0000U) * ((y0 >> 16) & 0x000000FFU);
  uint32_t t3 = (x0 >> 24        ) * ((y0 >> 24)              );

  t0 = _FOG_FACE_COMBINE_2(t0, t1);
  t2 = _FOG_FACE_COMBINE_2(t2, t3);

  t0 = t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U;
  t2 = t2 + ((t2 >> 8) & 0x00FF00FFU) + 0x00800080U;

  dst0 = _FOG_FACE_COMBINE_2(t0 >> 16   , (t2 << 16)) & 0xFF00FF00U;
  dst0 = _FOG_FACE_COMBINE_2(dst0, (t0 >>  8) & 0x000000FFU);
  dst0 = _FOG_FACE_COMBINE_2(dst0, (t2 >>  8) & 0x00FF0000U);
#endif // FOG_FACE_HAS_64BIT
}

//! @brief Packed multiply and divide by 255.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * y0.u8[0]) / 255
//! dst0.u8[1] = (x0.u8[1] * y0.u8[1]) / 255
//! dst0.u8[2] = (x0.u8[2] * y0.u8[2]) / 255
//! dst0.u8[3] = 0x00
//! @endverbatim
static FOG_INLINE void p32MulDiv255PBB_Z210(
  p32& dst0, const p32& x0, const p32& y0)
{
#if defined(FOG_FACE_HAS_64BIT)
  uint64_t t0 = (uint64_t)(x0 & 0x000000FFU) * (uint64_t)(y0 & 0x000000FFU); // 0x000000000000____;
  uint64_t t1 = (uint64_t)(x0 & 0x0000FF00U) * (uint64_t)(y0 & 0x0000FF00U); // 0x00000000____0000;
  uint64_t t2 = (uint64_t)(x0 & 0x00FF0000U) * (uint64_t)(y0 & 0x00FF0000U); // 0x0000____00000000;

  t0 = _FOG_FACE_COMBINE_2(t0, t1);
  t0 = _FOG_FACE_COMBINE_2(t0, t2);

  t0 = t0 + ((t0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF)) + FOG_UINT64_C(0x0080008000800080);

  dst0 = ((uint32_t)(t0 >>  8) & 0x000000FFU) |
         ((uint32_t)(t0 >> 16) & 0x0000FF00U) |
         ((uint32_t)(t0 >> 24) & 0x00FF0000U) ;
#else
  uint32_t t0 = (x0 & 0x000000FFU) * ((y0      ) & 0xFFU);
  uint32_t t1 = (x0 & 0x00FF0000U) * ((y0 >> 16) & 0xFFU);
  uint32_t t2 = ((x0 >> 8) & 0xFFU)* ((y0 >>  8) & 0xFFU);

  t0 = _FOG_FACE_COMBINE_2(t0, t1);

  t0 = t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U;
  t2 = t2 + ((t2 >> 8)              ) + 0x00000080U;

  dst0 = _FOG_FACE_COMBINE_2((t0 >> 8) & 0x00FF00FF, t2 & 0x0000FF00U);
#endif // FOG_FACE_HAS_64BIT
}

//! @brief Packed multiply and divide by 256.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * y0_10.u16[0]) / 256
//! dst0.u8[1] = (x0.u8[1] * y0_10.u16[1]) / 256
//! dst0.u8[2] = (x0.u8[2] * y0_32.u16[0]) / 256
//! dst0.u8[3] = (x0.u8[3] * y0_32.u16[1]) / 256
//! @endverbatim
static FOG_INLINE void p32MulDiv256PBB_PBW_1032(
  p32& dst0, const p32& x0, const p32& y0_10, const p32& y0_32)
{
  uint32_t xm = x0;
  uint32_t t0 = (xm & 0xFFU) * y0_10; xm >>= 8;
  uint32_t t1 = (xm & 0xFFU) * y0_10; xm >>= 8;
  uint32_t t2 = (xm & 0xFFU) * y0_32; xm >>= 8;
  uint32_t t3 = (xm        ) * y0_32;

  dst0 = _FOG_FACE_COMBINE_4(((t0 >>  8) & 0x000000FFU), ((t1 >> 16) & 0x0000FF00U) ,
                             ((t2 >>  8) & 0x00FF0000U), ((t3      ) & 0xFF000000U) );
}

//! @brief Packed multiply and divide by 256.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * y0_10.u16[0]) / 256
//! dst0.u8[1] = (x0.u8[1] * y0_10.u16[1]) / 256
//! dst0.u8[2] = (x0.u8[2] * y0_32.u16[0]) / 256
//! dst0.u8[3] = 0x00
//! @endverbatim
static FOG_INLINE void p32MulDiv256PBB_PBW_10Z2(
  p32& dst0, const p32& x0, const p32& y0_10, const p32& y0_32)
{
  uint32_t xm = x0;
  uint32_t t0 = (xm & 0xFFU) * y0_10; xm >>= 8;
  uint32_t t1 = (xm & 0xFFU) * y0_10; xm >>= 8;
  uint32_t t2 = (xm & 0xFFU) * y0_32;

  dst0 = _FOG_FACE_COMBINE_3(((t0 >>  8) & 0x000000FFU),
                             ((t1 >> 16) & 0x0000FF00U),
                             ((t2 <<  8) & 0x00FF0000U));
}

//! @brief Packed multiply and divide by 256.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * y0_10.u16[0]) / 256
//! dst0.u8[1] = (x0.u8[1] * y0_10.u16[1]) / 256
//! dst0.u8[2] = (x0.u8[2] * y0_32.u16[0]) / 256
//! dst0.u8[3] = 0xFF
//! @endverbatim
static FOG_INLINE void p32MulDiv256PBB_PBW_10F2(
  p32& dst0, const p32& x0, const p32& y0_10, const p32& y0_32)
{
  p32MulDiv256PBB_PBW_10Z2(dst0, x0, y0_10, y0_32);
  p32FillPBB3(dst0, dst0);
}

//! @brief Packed multiply and divide by 255.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * y0_20.u16[0]) / 255
//! dst0.u8[1] = (x0.u8[1] * y0_31.u16[1]) / 255
//! dst0.u8[2] = (x0.u8[2] * y0_20.u16[0]) / 255
//! dst0.u8[3] = (x0.u8[3] * y0_31.u16[1]) / 255
//! @endverbatim
static FOG_INLINE void p32MulDiv255PBB_PBW_2031(
  p32& dst0, const p32& x0, const p32& y0_20, const p32& y0_31)
{
  // abc
  uint32_t xm = x0;
  uint32_t t0 = (xm & 0xFFU) * y0_20; xm >>= 8; // 0 0x0000FF00
  uint32_t t1 = (xm & 0xFFU) * y0_31; xm >>= 8; // 1 0x0000FF00
  uint32_t t2 = (xm & 0xFFU) * y0_20; xm >>= 8; // 2 0xFF000000
  uint32_t t3 = (xm        ) * y0_31;           // 3 0xFF000000

  t0 = (t0 & 0x0000FFFF) | (t1 & 0x0000FFFF);
  t2 = (t2 & 0x0000FFFF) | (t3 & 0x0000FFFF);

  t0 = t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U;
  t2 = t2 + ((t2 >> 8) & 0x00FF00FFU) + 0x00800080U;

  dst0 = _FOG_FACE_COMBINE_2((t0 >> 8) & 0x00FF00FF, t2 & 0xFF00FF00U);
}

//! @brief Packed multiply and divide by 256.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * y0_20.u16[0]) / 256
//! dst0.u8[1] = (x0.u8[1] * y0_31.u16[1]) / 256
//! dst0.u8[2] = (x0.u8[2] * y0_20.u16[0]) / 256
//! dst0.u8[3] = (x0.u8[3] * y0_31.u16[1]) / 256
//! @endverbatim
static FOG_INLINE void p32MulDiv256PBB_PBW_2031(
  p32& dst0, const p32& x0, const p32& y0_20, const p32& y0_31)
{
  uint32_t xm = x0;
  uint32_t t0 = (xm & 0xFFU) * y0_20; xm >>= 8;
  uint32_t t1 = (xm & 0xFFU) * y0_31; xm >>= 8;
  uint32_t t2 = (xm & 0xFFU) * y0_20; xm >>= 8;
  uint32_t t3 = (xm        ) * y0_31;

  dst0 = _FOG_FACE_COMBINE_4(((t0 >>  8) & 0x000000FFU), (t1 & 0x0000FF00U) ,
                             ((t2 >>  8) & 0x00FF0000U), (t3 & 0xFF000000U) );
}

//! @brief Packed multiply and divide by 255.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * y0_20.u16[0]) / 255
//! dst0.u8[1] = (x0.u8[1] * y0_31.u16[1]) / 255
//! dst0.u8[2] = (x0.u8[2] * y0_20.u16[0]) / 255
//! dst0.u8[3] = 0x00
//! @endverbatim
static FOG_INLINE void p32MulDiv255PBB_PBW_20Z1(
  p32& dst0, const p32& x0, const p32& y0_20, const p32& y0_31)
{
  // abc
  uint32_t xm = x0;
  uint32_t t0 = (xm & 0xFFU) * y0_20; xm >>= 8; // 0 0x0000FF00
  uint32_t t1 = (xm & 0xFFU) * y0_31; xm >>= 8; // 1 0x0000FF00
  uint32_t t2 = (xm & 0xFFU) * y0_20; xm >>= 8; // 2 0xFF000000

  t0 = (t0 & 0x0000FFFF) | (t1 & 0x0000FFFF);

  t0 = t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U;
  t2 = t2 + ((t2 >> 8) & 0x00FF00FFU) + 0x00800080U;

  dst0 = _FOG_FACE_COMBINE_2((t0 >> 8) & 0x00FF00FF, t2 & 0x0000FF00U);
}

//! @brief Packed multiply and divide by 256.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * y0_20.u16[0]) / 256
//! dst0.u8[1] = (x0.u8[1] * y0_31.u16[0]) / 256
//! dst0.u8[2] = (x0.u8[2] * y0_20.u16[1]) / 256
//! dst0.u8[3] = 0x00
//! @endverbatim
static FOG_INLINE void p32MulDiv256PBB_PBW_20Z1(
  p32& dst0, const p32& x0, const p32& y0_20, const p32& y0_31)
{
  uint32_t xm = x0;
  uint32_t t0 = (xm & 0xFFU) * y0_20; xm >>= 8;
  uint32_t t1 = (xm & 0xFFU) * y0_31; xm >>= 8;
  uint32_t t2 = (xm        ) * y0_20;

  dst0 = _FOG_FACE_COMBINE_3(((t0 >>  8) & 0x000000FFU),
                             ((t1      ) & 0x0000FF00U),
                             ((t2      ) & 0x00FF0000U));
}

//! @brief Packed multiply and divide by 256.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * y0_20.u16[0]) / 256
//! dst0.u8[1] = (x0.u8[1] * y0_31.u16[0]) / 256
//! dst0.u8[2] = (x0.u8[2] * y0_20.u16[1]) / 256
//! dst0.u8[3] = 0xFF
//! @endverbatim
static FOG_INLINE void p32MulDiv256PBB_PBW_20F1(
  p32& dst0, const p32& x0, const p32& y0_20, const p32& y0_31)
{
  p32MulDiv256PBB_PBW_20Z1(dst0, x0, y0_20, y0_31);
  p32FillPBB3(dst0, dst0);
}

//! @brief Packed<-Scalar multiply and divide by 255.
//!
//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * u0.u32[0]) / 255
//! dst0.u16[1] = (x0.u16[1] * u0.u32[0]) / 255
//! @endverbatim
static FOG_INLINE void p32MulDiv255PBW_SBW(
  p32& dst0, const p32& x0, const p32& u0)
{
  dst0 = x0 * u0;
  dst0 = ((dst0 + ((dst0 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;
}

//! @brief Packed<-Scalar multiply and divide by 255 (2x).
//!
//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * u0.u32[0]) / 255
//! dst0.u16[1] = (x0.u16[1] * u0.u32[0]) / 255
//! dst1.u16[0] = (x1.u16[0] * u1.u32[0]) / 255
//! dst1.u16[1] = (x1.u16[1] * u1.u32[0]) / 255
//! @endverbatim
static FOG_INLINE void p32MulDiv255PBW_SBW_2x(
  p32& dst0, const p32& x0, const p32& u0,
  p32& dst1, const p32& x1, const p32& u1)
{
  uint32_t t0 = x0 * u0;
  uint32_t t1 = x1 * u1;

  dst0 = ((t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;
  dst1 = ((t1 + ((t1 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;
}

//! @brief Packed<-Scalar multiply and divide by 256.
//!
//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * u0.u32[0]) / 256
//! dst0.u16[1] = (x0.u16[1] * u0.u32[0]) / 256
//! @endverbatim
static FOG_INLINE void p32MulDiv256PBW_SBW(
  p32& dst0, const p32& x0, const p32& u0)
{
  dst0 = ((x0 * u0) >> 8) & 0x00FF00FFU;
}

//! @brief Packed<-Scalar multiply and divide by 256 (2x).
//!
//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * u0.u32[0]) / 256
//! dst0.u16[1] = (x0.u16[1] * u0.u32[0]) / 256
//! dst1.u16[0] = (x1.u16[0] * u1.u32[0]) / 256
//! dst1.u16[1] = (x1.u16[1] * u1.u32[0]) / 256
//! @endverbatim
static FOG_INLINE void p32MulDiv256PBW_SBW_2x(
  p32& dst0, const p32& x0, const p32& u0,
  p32& dst1, const p32& x1, const p32& u1)
{
  uint32_t t0 = ((x0 * u0) >> 8) & 0x00FF00FFU;
  uint32_t t1 = ((x1 * u1) >> 8) & 0x00FF00FFU;

  dst0 = t0;
  dst1 = t1;
}

//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * y0.u16[0]) / 255
//! dst0.u16[1] = (x0.u16[1] * y0.u16[1]) / 255
//! @endverbatim
static FOG_INLINE void p32MulDiv255PBW(
  p32& dst0, const p32& x0, const p32& y0)
{
  dst0 = ((x0 & 0x000000FFU) * (y0 & 0x000000FFU)) | ((x0 & 0x00FF0000U) * (y0 >> 16));
  dst0 = ((dst0 + ((dst0 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;
}

//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * y0.u16[0]) / 255
//! dst0.u16[1] = (x0.u16[1] * y0.u16[1]) / 255
//! dst1.u16[0] = (x1.u16[0] * y1.u16[0]) / 255
//! dst1.u16[1] = (x1.u16[1] * y1.u16[1]) / 255
//! @endverbatim
static FOG_INLINE void p32MulDiv255PBW_2x(
  p32& dst0, const p32& x0, const p32& y0,
  p32& dst1, const p32& x1, const p32& y1)
{
  uint32_t t0 = ((x0 & 0x000000FFU) * (y0 & 0x000000FFU)) | ((x0 & 0x00FF0000U) * (y0 >> 16));
  uint32_t t1 = ((x1 & 0x000000FFU) * (y1 & 0x000000FFU)) | ((x1 & 0x00FF0000U) * (y1 >> 16));

  dst0 = ((t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;
  dst1 = ((t1 + ((t1 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;
}

//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * y0.u16[0]) / 256
//! dst0.u16[1] = (x0.u16[1] * y0.u16[1]) / 256
//! @endverbatim
static FOG_INLINE void p32MulDiv256PBW(
  p32& dst0, const p32& x0, const p32& y0)
{
  dst0 = ((((x0 & 0x0000FFFFU) * (y0 & 0x0000FFFFU)) | ((x0 & 0xFFFF0000U) * (y0 >> 16))) >> 8) & 0x00FF00FFU;
}

//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * y0.u16[0]) / 256
//! dst0.u16[1] = (x0.u16[1] * y0.u16[1]) / 256
//! dst1.u16[0] = (x1.u16[0] * y1.u16[0]) / 256
//! dst1.u16[1] = (x1.u16[1] * y1.u16[1]) / 256
//! @endverbatim
static FOG_INLINE void p32MulDiv256PBW_2x(
  p32& dst0, const p32& x0, const p32& y0,
  p32& dst1, const p32& x1, const p32& y1)
{
  uint32_t t0 = ((((x0 & 0x0000FFFFU) * (y0 & 0x0000FFFFU)) | ((x0 & 0xFFFF0000U) * (y0 >> 16))) >> 8) & 0x00FF00FFU;
  uint32_t t1 = ((((x1 & 0x0000FFFFU) * (y1 & 0x0000FFFFU)) | ((x1 & 0xFFFF0000U) * (y1 >> 16))) >> 8) & 0x00FF00FFU;

  dst0 = t0;
  dst1 = t1;
}

static FOG_INLINE void p32MulDiv256PBW_2x_Pack_2031(
  p32& dst0,
  const p32& x0, const p32& y0,
  const p32& x1, const p32& y1)
{
  uint32_t t0 = ((((x0 & 0x0000FFFFU) * (y0 & 0x0000FFFFU)) | ((x0 & 0xFFFF0000U) * (y0 >> 16))) >> 8) & 0x00FF00FFU;
  uint32_t t1 = ((((x1 & 0x0000FFFFU) * (y1 & 0x0000FFFFU)) | ((x1 & 0xFFFF0000U) * (y1 >> 16)))     ) & 0xFF00FF00U;

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
}

static FOG_INLINE void p32MulDiv256PBW_2x_Pack_20Z1(
  p32& dst0,
  const p32& x0, const p32& y0,
  const p32& x1, const p32& y1)
{
  uint32_t t0 = ((((x0 & 0x0000FFFFU) * (y0 & 0x0000FFFFU)) | ((x0 & 0xFFFF0000U) * (y0 >> 16))) >> 8) & 0x00FF00FFU;
  uint32_t t1 = ((x1 & 0x0000FFFFU) * (y1 & 0x0000FFFFU)) & 0xFF00FF00U;

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
}

static FOG_INLINE void p32MulDiv256PBW_2x_Pack_20F1(
  p32& dst0,
  const p32& x0, const p32& y0,
  const p32& x1, const p32& y1)
{
  p32MulDiv256PBW_2x_Pack_20Z1(dst0, x0, y0, x1, y1);
  p32FillPBB3(dst0, dst0);
}

static FOG_INLINE void p32MulDiv65535PWW_SWD(
  p32& dst0, const p32& x0, const p32& u0)
{
  uint32_t t0 = (x0 & 0xFFFF) * u0;
  uint32_t t1 = (x0    >> 16) * u0;

  dst0 = _FOG_FACE_COMBINE_2((t0 + (t0 >> 16) + 0x8000U) >> 16,
                             (t1 + (t1 >> 16) + 0x8000U) & 0xFFFF0000U);
}

// ============================================================================
// [Fog::Face - P32 - MulDiv_Pack]
// ============================================================================

//! @brief Packed<-Scalar multiply and divide by 255 (2x) and pack (2031).
//!
//! @verbatim
//! dst0 = pack2031((x0 * u0) / 255, (x1 * u1) / 255)
//! @endverbatim
static FOG_INLINE void p32MulDiv255PBW_SBW_2x_Pack_2031(
  p32& dst0,
  const p32& x0, const p32& u0,
  const p32& x1, const p32& u1)
{
  uint32_t t0 = x0 * u0;
  uint32_t t1 = x1 * u1;

  t0 = ((t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;
  t1 = ((t1 + ((t1 >> 8) & 0x00FF00FFU) + 0x00800080U)     ) & 0xFF00FF00U;

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
}

//! @brief Packed<-Scalar multiply and divide by 256 (2x) and pack (2031).
//!
//! @verbatim
//! dst0 = pack2031((x0 * u0) / 256, (x1 * u1) / 256)
//! @endverbatim
static FOG_INLINE void p32MulDiv256PBW_SBW_2x_Pack_2031(
  p32& dst0,
  const p32& x0, const p32& u0,
  const p32& x1, const p32& u1)
{
  uint32_t t0 = ((x0 * u0) >> 8) & 0x00FF00FFU;
  uint32_t t1 = ((x1 * u1)     ) & 0xFF00FF00U;

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
}

//! @brief Packed<-Scalar multiply and divide by 256 (2x) and pack (20Z1).
//!
//! @verbatim
//! dst0 = pack20Z1((x0 * u0) / 256, (x1 * u1) / 256)
//! @endverbatim
static FOG_INLINE void p32MulDiv256PBW_SBW_2x_Pack_20Z1(
  p32& dst0,
  const p32& x0, const p32& u0,
  const p32& x1, const p32& u1)
{
  uint32_t t0 = ((x0 * u0) >> 8) & 0x00FF00FFU;
  uint32_t t1 = ((x1 * u1)     ) & 0x0000FF00U;

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
}

//! @brief Packed<-Scalar multiply and divide by 256 (2x) and pack (20F1).
//!
//! @verbatim
//! dst0 = pack20F1((x0 * u0) / 256, (x1 * u1) / 256)
//! @endverbatim
static FOG_INLINE void p32MulDiv256PBW_SBW_2x_Pack_20F1(
  p32& dst0,
  const p32& x0, const p32& u0,
  const p32& x1, const p32& u1)
{
  p32MulDiv256PBW_SBW_2x_Pack_20Z1(dst0, x0, u0, x1, u1);
  p32FillPBB3(dst0, dst0);
}

//! @brief Packed<-Scalar multiply and divide by 256 (2x) and pack (1032).
static FOG_INLINE void p32MulDiv256PBW_SBW_2x_Pack_1032(
  p32& dst0,
  const p32& x0, const p32& u0,
  const p32& x1, const p32& u1)
{
  uint32_t t0 = x0 * u0;
  uint32_t t1 = x1 * u1;

  dst0 = _FOG_FACE_COMBINE_4((t0 >>  8) & 0x000000FF, (t0 >> 16) & 0x0000FF00,
                             (t1 <<  8) & 0x00FF0000, (t1      ) & 0xFF000000);
}

//! @brief Packed<-Scalar multiply and divide by 256 (2x) and pack (10Z2).
static FOG_INLINE void p32MulDiv256PBW_SBW_2x_Pack_10Z2(
  p32& dst0,
  const p32& x0, const p32& u0,
  const p32& x1, const p32& u1)
{
  uint32_t t0 = x0 * u0;
  uint32_t t1 = x1 * u1;

  dst0 = _FOG_FACE_COMBINE_3((t0 >>  8) & 0x000000FF, (t0 >> 16) & 0x0000FF00,
                             (t1 <<  8) & 0x00FF0000);
}

//! @brief Packed<-Scalar multiply and divide by 256 (2x) and pack (10Z2).
static FOG_INLINE void p32MulDiv256PBW_SBW_2x_Pack_10F2(
  p32& dst0,
  const p32& x0, const p32& u0,
  const p32& x1, const p32& u1)
{
  uint32_t t0 = x0 * u0;
  uint32_t t1 = x1 * u1;

  dst0 = _FOG_FACE_COMBINE_4((t0 >>  8) & 0x000000FF, (t0 >> 16) & 0x0000FF00,
                             (t1 <<  8) & 0x00FF0000, 0xFF000000);
}

//! @brief Packed<-Scalar multiply and divide by 255 (2x) and pack (20Z1).
//!
//! @verbatim
//! dst0 = pack((x0 * u0) / 255, (x1 * u1) / 255) & 0x00FFFFFF
//! @endverbatim
static FOG_INLINE void p32MulDiv255PBW_SBW_2x_Pack_20Z1(
  p32& dst0,
  const p32& x0, const p32& u0,
  const p32& x1, const p32& u1)
{
  uint32_t t0 = x0 * u0;
  uint32_t t1 = x1 * u1;

  t0 = ((t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;
  t1 = ((t1 + ((t1 >> 8) & 0x00FF00FFU) + 0x00800080U)     ) & 0x0000FF00U;

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
}

// ============================================================================
// [Fog::Face - P32 - MulDiv_Add]
// ============================================================================

//! @verbatim
//! dst0 = (x0 * y0) / 255 + z0
//! @endverbatim
static FOG_INLINE void p32MulDiv255PBW_Add(
  p32& dst0, const p32& x0, const p32& y0, const p32& z0)
{
  p32MulDiv255PBW(dst0, x0, y0);
  p32Add(dst0, dst0, z0);
}

//! @verbatim
//! dst0 = (x0 * y0) / 255 + z0
//! dst1 = (x1 * y1) / 255 + z1
//! @endverbatim
static FOG_INLINE void p32MulDiv255PBW_Add_2x(
  p32& dst0, const p32& x0, const p32& y0, const p32& z0,
  p32& dst1, const p32& x1, const p32& y1, const p32& z1)
{
  p32MulDiv255PBW_2x(dst0, x0, y0, dst1, x1, y1);
  p32Add_2x(dst0, dst0, z0, dst1, dst1, z1);
}

//! @verbatim
//! dst0 = (x0 * y0) / 256 + z0
//! @endverbatim
static FOG_INLINE void p32MulDiv256PBW_Add(
  p32& dst0, const p32& x0, const p32& y0, const p32& z0)
{
  p32MulDiv256PBW(dst0, x0, y0);
  p32Add(dst0, dst0, z0);
}

//! @verbatim
//! dst0 = (x0 * y0) / 256 + z0
//! dst1 = (x1 * y1) / 256 + z1
//! @endverbatim
static FOG_INLINE void p32MulDiv256PBW_Add_2x(
  p32& dst0, const p32& x0, const p32& y0, const p32& z0,
  p32& dst1, const p32& x1, const p32& y1, const p32& z1)
{
  p32MulDiv256PBW_2x(dst0, x0, y0, dst1, x1, y1);
  p32Add_2x(dst0, dst0, z0, dst1, dst1, z1);
}

//! @verbatim
//! dst0.u8[0] = min((x0.u8[0] * y0.u32[0]) / 255 + z0.u8[0], 255)
//! dst0.u8[1] = min((x0.u8[1] * y0.u32[1]) / 255 + z0.u8[1], 255)
//! dst0.u8[2] = min((x0.u8[2] * y0.u32[2]) / 255 + z0.u8[2], 255)
//! dst0.u8[3] = min((x0.u8[3] * y0.u32[3]) / 255 + z0.u8[3], 255)
//! @endverbatim
static FOG_INLINE void p32MulDiv255_PBB_SBB_Addus255PBB(
  p32& dst0, const p32& x0, const p32& y0, const p32& z0)
{
#if defined(FOG_FACE_HAS_64BIT)
  uint64_t t0 = ((uint64_t)x0 | ((uint64_t)x0 << 24)) & FOG_UINT64_C(0x00FF00FF00FF00FF);
  uint64_t t1 = ((uint64_t)z0 | ((uint64_t)z0 << 24)) & FOG_UINT64_C(0x00FF00FF00FF00FF);

  t0 *= y0;
  t0 = (t0 + ((t0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF)) + FOG_UINT64_C(0x0080008000800080)) >> 8;
  t0 &= FOG_UINT64_C(0x00FF00FF00FF00FF);

  t0 += t1;
  t0 |= FOG_UINT64_C(0x0100010001000100) - ((t0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF));
  t0 &= FOG_UINT64_C(0x00FF00FF00FF00FF);

  dst0 = (uint32_t)(_FOG_FACE_COMBINE_2(t0, t0 >> 24));
#else
  uint32_t t0 = ((x0     ) & 0x00FF00FFU) * y0;
  uint32_t t1 = ((x0 >> 8) & 0x00FF00FFU) * y0;

  t0 = ((t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;
  t1 = ((t1 + ((t1 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;

  t0 += (z0     ) & 0x00FF00FFU;
  t1 += (z0 >> 8) & 0x00FF00FFU;

  t0 |= 0x01000100U - ((t0 >> 8) & 0x00FF00FFU);
  t1 |= 0x01000100U - ((t1 >> 8) & 0x00FF00FFU);

  t0 = (t0 & 0x00FF00FFU);
  t1 = (t1 & 0x00FF00FFU) << 8;

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
#endif
}

//! @verbatim
//! dst0 = min((x0 * y0) / 255 + z0, 255)
//! @endverbatim
static FOG_INLINE void p32MulDiv255PBW_SBW_AddusPBW(
  p32& dst0, const p32& x0, const p32& y0, const p32& z0)
{
  p32MulDiv255PBW_SBW(dst0, x0, y0);
  p32Addus255PBW(dst0, dst0, z0);
}

//! @verbatim
//! dst0 = min((x0 * y0) / 255 + z0, 255)
//! dst1 = min((x1 * y1) / 255 + z1, 255)
//! @endverbatim
static FOG_INLINE void p32MulDiv255PBW_SBW_AddusPBW_2x(
  p32& dst0, const p32& x0, const p32& y0, const p32& z0,
  p32& dst1, const p32& x1, const p32& y1, const p32& z1)
{
  p32MulDiv255PBW_SBW_2x(dst0, x0, y0, dst1, x1, y1);
  p32Addus255PBW_2x(dst0, dst0, z1, dst1, dst1, z1);
}

//! @verbatim
//! dst0 = min((x0 * y0) / 256 + z0, 255)
//! @endverbatim
static FOG_INLINE void p32MulDiv256PBW_SBW_AddusPBW(
  p32& dst0, const p32& x0, const p32& y0, const p32& z0)
{
  p32MulDiv256PBW_SBW(dst0, x0, y0);
  p32Addus255PBW(dst0, dst0, z0);
}

//! @verbatim
//! dst0 = min((z0 * y0) / 256 + z0, 255)
//! dst1 = min((z1 * y1) / 256 + z1, 255)
//! @endverbatim
static FOG_INLINE void p32MulDiv256PBW_SBW_AddusPBW_2x(
  p32& dst0, const p32& x0, const p32& y0, const p32& z0,
  p32& dst1, const p32& x1, const p32& y1, const p32& z1)
{
  p32MulDiv256PBW_SBW_2x(dst0, x0, y0, dst1, x1, y1);
  p32Addus255PBW_2x(dst0, dst0, z1, dst1, dst1, z1);
}

//! @verbatim
//! dst0 = (x0 * y0) / 255 + z0
//! @endverbatim
static FOG_INLINE void p32MulDiv255PBW_Addus255PBW(
  p32& dst0, const p32& x0, const p32& y0, const p32& z0)
{
  p32MulDiv255PBW(dst0, x0, y0);
  p32Addus255PBW(dst0, dst0, z0);
}

//! @verbatim
//! dst0 = (x0 * y0) / 255 + z0
//! dst1 = (x1 * y1) / 255 + z1
//! @endverbatim
static FOG_INLINE void p32MulDiv255PBW_Addus255PBW_2x(
  p32& dst0, const p32& x0, const p32& y0, const p32& z0,
  p32& dst1, const p32& x1, const p32& y1, const p32& z1)
{
  p32MulDiv255PBW_2x(dst0, x0, y0, dst1, x1, y1);
  p32Addus255PBW_2x(dst0, dst0, z0, dst1, dst1, z1);
}

//! @verbatim
//! dst0 = (x0 * y0) / 256 + z0
//! @endverbatim
static FOG_INLINE void p32MulDiv256PBW_Addus255PBW(
  p32& dst0, const p32& x0, const p32& y0, const p32& z0)
{
  p32MulDiv256PBW(dst0, x0, y0);
  p32Addus255PBW(dst0, dst0, z0);
}

//! @verbatim
//! dst0 = (x0 * y0) / 256 + z0
//! dst1 = (x1 * y1) / 256 + z1
//! @endverbatim
static FOG_INLINE void p32MulDiv256PBW_Addus255PBW_2x(
  p32& dst0, const p32& x0, const p32& y0, const p32& z0,
  p32& dst1, const p32& x1, const p32& y1, const p32& z1)
{
  p32MulDiv256PBW_2x(dst0, x0, y0, dst1, x1, y1);
  p32Addus255PBW_2x(dst0, dst0, z0, dst1, dst1, z1);
}

// ============================================================================
// [Fog::Face - P32 - Lerp]
// ============================================================================

//! @brief Scalar interpolate at interval [0, 255].
//!
//! @verbatim
//! dst0 = ( (x0 * z0) + (y0 * (255 - z0)) ) / 255
//! @endverbatim
static FOG_INLINE void p32Lerp255SBW(
  p32& dst0, const p32& x0, const p32& y0, const p32& z0)
{
  dst0 = x0 * z0 + y0 * (z0 ^ 0xFFU);
  dst0 = (dst0 + (dst0 >> 8) + 0x80U) >> 8;
}

//! @brief Scalar interpolate at interval [0, 255].
//!
//! @verbatim
//! dst0 = ( (x0 * z0) + (y0 * w0) ) / 255
//! @endverbatim
static FOG_INLINE void p32Lerp255SBW(
  p32& dst0, const p32& x0, const p32& y0, const p32& z0, const p32& w0)
{
  dst0 = x0 * z0 + y0 * w0;
  dst0 = (dst0 + (dst0 >> 8) + 0x80U) >> 8;
}

//! @brief Scalar interpolate at interval [0, 256].
//!
//! @verbatim
//! dst0 = ( (x0 * z0) + (y0 * (256 - w0)) ) / 256
//! @endverbatim
static FOG_INLINE void p32Lerp256SBW(
  p32& dst0, const p32& x0, const p32& y0, const p32& z0)
{
  dst0 = x0 * z0 + y0 * (256U - z0);
  dst0 = dst0 >> 8;
}

//! @brief Scalar interpolate at interval [0, 256].
//!
//! @verbatim
//! dst0 = ( (x0 * z0) + (y0 * w0) ) / 256
//! @endverbatim
static FOG_INLINE void p32Lerp256SBW(
  p32& dst0, const p32& x0, const p32& y0, const p32& z0, const p32& w0)
{
  dst0 = x0 * z0 + y0 * w0;
  dst0 = dst0 >> 8;
}

//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * z0.u32[0] + y0.u8[0] * (255 - z0.u32[0])) / 255
//! dst0.u8[1] = (x0.u8[1] * z0.u32[0] + y0.u8[1] * (255 - z0.u32[0])) / 255
//! dst0.u8[2] = (x0.u8[2] * z0.u32[0] + y0.u8[2] * (255 - z0.u32[0])) / 255
//! dst0.u8[3] = (x0.u8[3] * z0.u32[0] + y0.u8[3] * (255 - z0.u32[0])) / 255
//! @endverbatim
static FOG_INLINE void p32Lerp255PBB_SBB(
  p32& dst0, const p32& x0, const p32& y0, const p32& z0)
{
  uint32_t zm = z0;

  uint32_t t0 = ((x0     ) & 0x00FF00FFU) * zm;
  uint32_t t1 = ((x0 >> 8) & 0x00FF00FFU) * zm;

  zm ^= 0xFF;
  t0 += ((y0     ) & 0x00FF00FFU) * zm;
  t1 += ((y0 >> 8) & 0x00FF00FFU) * zm;

  t0 = ((t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U) & 0xFF00FF00U) >> 8;
  t1 = ((t1 + ((t1 >> 8) & 0x00FF00FFU) + 0x00800080U) & 0xFF00FF00U);

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
}

//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * z0.u32[0] + y0.u8[0] * w0.u32[0]) / 255
//! dst0.u8[1] = (x0.u8[1] * z0.u32[0] + y0.u8[1] * w0.u32[0]) / 255
//! dst0.u8[2] = (x0.u8[2] * z0.u32[0] + y0.u8[2] * w0.u32[0]) / 255
//! dst0.u8[3] = (x0.u8[3] * z0.u32[0] + y0.u8[3] * w0.u32[0]) / 255
//! @endverbatim
static FOG_INLINE void p32Lerp255PBB_SBB(
  p32& dst0, const p32& x0, const p32& y0, const p32& z0, const p32& w0)
{
  uint32_t t0 = ((x0     ) & 0x00FF00FFU) * z0 + ((y0     ) & 0x00FF00FFU) * w0;
  uint32_t t1 = ((x0 >> 8) & 0x00FF00FFU) * z0 + ((y0 >> 8) & 0x00FF00FFU) * w0;

  t0 = ((t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U) & 0xFF00FF00U) >> 8;
  t1 = ((t1 + ((t1 >> 8) & 0x00FF00FFU) + 0x00800080U) & 0xFF00FF00U);

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
}

//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * z0.u32[0] + y0.u8[0] * (256 - z0.u32[0])) / 256
//! dst0.u8[1] = (x0.u8[1] * z0.u32[0] + y0.u8[1] * (256 - z0.u32[0])) / 256
//! dst0.u8[2] = (x0.u8[2] * z0.u32[0] + y0.u8[2] * (256 - z0.u32[0])) / 256
//! dst0.u8[3] = (x0.u8[3] * z0.u32[0] + y0.u8[3] * (256 - z0.u32[0])) / 256
//! @endverbatim
static FOG_INLINE void p32Lerp256PBB_SBW(
  p32& dst0, const p32& x0, const p32& y0, const p32& z0)
{
  uint32_t zm = z0;

  uint32_t t0 = ((x0     ) & 0x00FF00FFU) * zm;
  uint32_t t1 = ((x0 >> 8) & 0x00FF00FFU) * zm;

  zm = 256 - zm;
  t0 += ((y0     ) & 0x00FF00FFU) * zm;
  t1 += ((y0 >> 8) & 0x00FF00FFU) * zm;

  t0 = (t0 & 0xFF00FF00U) >> 8;
  t1 = (t1 & 0xFF00FF00U);

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
}

//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * z0.u32[0] + y0.u8[0] * (256 - z0.u32[0])) / 256
//! dst0.u8[1] = (x0.u8[1] * z0.u32[0] + y0.u8[1] * (256 - z0.u32[0])) / 256
//! dst0.u8[2] = (x0.u8[2] * z0.u32[0] + y0.u8[2] * (256 - z0.u32[0])) / 256
//! dst0.u8[3] = 0x00
//! @endverbatim
static FOG_INLINE void p32Lerp256PBB_SBW_10Z2(
  p32& dst0, const p32& x0, const p32& y0, const p32& z0)
{
  uint32_t zm = z0;

  uint32_t t0 = (x0 & 0x00FF00FFU) * zm;
  uint32_t t1 = (x0 & 0x0000FF00U) * zm;

  zm = 256 - zm;
  t0 += (y0 & 0x00FF00FFU) * zm;
  t1 += (y0 & 0x0000FF00U) * zm;

  t0 &= 0xFF00FF00U;
  t1 &= 0x00FF0000U;

  dst0 = _FOG_FACE_COMBINE_2(t0, t1) >> 8;
}

//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * z0.u32[0] + y0.u8[0] * (256 - z0.u32[0])) / 256
//! dst0.u8[1] = (x0.u8[1] * z0.u32[0] + y0.u8[1] * (256 - z0.u32[0])) / 256
//! dst0.u8[2] = (x0.u8[2] * z0.u32[0] + y0.u8[2] * (256 - z0.u32[0])) / 256
//! dst0.u8[3] = 0xFF
//! @endverbatim
static FOG_INLINE void p32Lerp256PBB_SBW_10F2(
  p32& dst0, const p32& x0, const p32& y0, const p32& z0)
{
  p32Lerp256PBB_SBW_10Z2(dst0, x0, y0, z0);
  p32FillPBB3(dst0, dst0);
}

//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * z0.u32[0] + y0.u8[0] * w0.u32[0]) / 256
//! dst0.u8[1] = (x0.u8[1] * z0.u32[0] + y0.u8[1] * w0.u32[0]) / 256
//! dst0.u8[2] = (x0.u8[2] * z0.u32[0] + y0.u8[2] * w0.u32[0]) / 256
//! dst0.u8[3] = (x0.u8[3] * z0.u32[0] + y0.u8[3] * w0.u32[0]) / 256
//! @endverbatim
static FOG_INLINE void p32Lerp256PBB_SBW(
  p32& dst0, const p32& x0, const p32& y0, const p32& z0, const p32& w0)
{
  uint32_t t0 = ((x0     ) & 0x00FF00FFU) * z0 + ((y0     ) & 0x00FF00FFU) * w0;
  uint32_t t1 = ((x0 >> 8) & 0x00FF00FFU) * z0 + ((y0 >> 8) & 0x00FF00FFU) * w0;

  t0 = (t0 & 0xFF00FF00U) >> 8;
  t1 = (t1 & 0xFF00FF00U);

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
}

//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * z0.u32[0] + y0.u8[0] * w0.u32[0]) / 256
//! dst0.u8[1] = (x0.u8[1] * z0.u32[0] + y0.u8[1] * w0.u32[0]) / 256
//! dst0.u8[2] = (x0.u8[2] * z0.u32[0] + y0.u8[2] * w0.u32[0]) / 256
//! dst0.u8[3] = 0x00
//! @endverbatim
static FOG_INLINE void p32Lerp256PBB_SBW_10Z2(
  p32& dst0, const p32& x0, const p32& y0, const p32& z0, const p32& w0)
{
  uint32_t t0 = (x0 & 0x00FF00FFU) * z0 + (y0 & 0x00FF00FFU) * w0;
  uint32_t t1 = (x0 & 0x0000FF00U) * z0 + (y0 & 0x0000FF00U) * w0;

  t0 &= 0xFF00FF00U;
  t1 &= 0x00FF0000U;

  dst0 = _FOG_FACE_COMBINE_2(t0, t1) >> 8;
}

//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * z0.u32[0] + y0.u8[0] * w0.u32[0]) / 256
//! dst0.u8[1] = (x0.u8[1] * z0.u32[0] + y0.u8[1] * w0.u32[0]) / 256
//! dst0.u8[2] = (x0.u8[2] * z0.u32[0] + y0.u8[2] * w0.u32[0]) / 256
//! dst0.u8[3] = 0xFF
//! @endverbatim
static FOG_INLINE void p32Lerp256PBB_SBW_10F2(
  p32& dst0, const p32& x0, const p32& y0, const p32& z0, const p32& w0)
{
  p32Lerp256PBB_SBW_10Z2(dst0, x0, y0, z0, w0);
  p32FillPBB3(dst0, dst0);
}

//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * z0.u32[0] + y0.u16[0] * w0.u32[0]) / 255
//! dst0.u16[1] = (x0.u16[1] * z0.u32[0] + y0.u16[1] * w0.u32[0]) / 255
//! @endverbatim
static FOG_INLINE void p32Lerp255PBW_SBW(
  p32& dst0, const p32& x0, const p32& y0, const p32& z0, const p32& w0)
{
  dst0 = (x0 * z0) + (y0 * w0);
  dst0 = ((dst0 + ((dst0 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;
}

//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * z0.u32[0] + y0.u16[0] * w0.u32[0]) / 255
//! dst0.u16[1] = (x0.u16[1] * z0.u32[0] + y0.u16[1] * w0.u32[0]) / 255
//! dst1.u16[0] = (x1.u16[0] * z1.u32[0] + y1.u16[0] * w1.u32[0]) / 255
//! dst1.u16[1] = (x1.u16[1] * z1.u32[0] + y1.u16[1] * w1.u32[0]) / 255
//! @endverbatim
static FOG_INLINE void p32Lerp255PBW_SBW_2x(
  p32& dst0, const p32& x0, const p32& y0, const p32& z0, const p32& w0,
  p32& dst1, const p32& x1, const p32& y1, const p32& z1, const p32& w1)
{
  uint32_t t0 = (x0 * z0) + (y0 * w0);
  uint32_t t1 = (x1 * z1) + (y1 * w1);

  dst0 = ((t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;
  dst1 = ((t1 + ((t1 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;
}

//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * z0.u32[0] + y0.u16[0] * w0.u32[0]) / 256
//! dst0.u16[1] = (x0.u16[1] * z0.u32[0] + y0.u16[1] * w0.u32[0]) / 256
//! @endverbatim
static FOG_INLINE void p32Lerp256PBW_SBW(
  p32& dst0, const p32& x0, const p32& y0, const p32& z0, const p32& w0)
{
  dst0 = (x0 * z0) + (y0 * w0);
  dst0 = (dst0 >> 8) & 0x00FF00FFU;
}

//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * z0.u32[0] + y0.u16[0] * w0.u32[0]) / 256
//! dst0.u16[1] = (x0.u16[1] * z0.u32[0] + y0.u16[1] * w0.u32[0]) / 256
//! dst1.u16[0] = (x1.u16[0] * z1.u32[0] + y1.u16[0] * w1.u32[0]) / 256
//! dst1.u16[1] = (x1.u16[1] * z1.u32[0] + y1.u16[1] * w1.u32[0]) / 256
//! @endverbatim
static FOG_INLINE void p32Lerp256PBW_SBW_2x(
  p32& dst0, const p32& x0, const p32& y0, const p32& z0, const p32& w0,
  p32& dst1, const p32& x1, const p32& y1, const p32& z1, const p32& w1)
{
  uint32_t t0 = (x0 * z0) + (y0 * w0);
  uint32_t t1 = (x1 * z1) + (y1 * w1);

  dst0 = (t0 >> 8) & 0x00FF00FFU;
  dst1 = (t1 >> 8) & 0x00FF00FFU;
}

//! @verbatim
//! dst0.u8[0] = ( x0.u8[0] * z0_10.u16[0] + y0.u8[0] * (256 - z0_10.u16[0]) ) / 256
//! dst0.u8[1] = ( x0.u8[1] * z0_10.u16[1] + y0.u8[1] * (256 - z0_10.u16[1]) ) / 256
//! dst0.u8[2] = ( x0.u8[2] * z0_32.u16[0] + y0.u8[2] * (256 - z0_32.u16[0]) ) / 256
//! dst0.u8[3] = ( x0.u8[3] * z0_32.u16[1] + y0.u8[3] * (256 - z0_32.u16[1]) ) / 256
//! @endverbatim
static FOG_INLINE void p32Lerp256PBB_PBW_1032(
  p32& dst0, const p32& x0, const p32& y0, const p32& z0_10, const p32& z0_32)
{
  uint32_t xm = x0;
  uint32_t ym = y0;

  uint32_t t0;
  uint32_t t1;

  t0 = (uint32_t)( ((((xm & 0x00FF) - (ym & 0x00FF)) * (z0_10 & 0xFFFF)) >> 8) + (ym & 0x00FF) );
  t1 = (uint32_t)( ((((xm & 0xFF00) - (ym & 0xFF00)) * (z0_10 >> 16   )) >> 8) + (ym & 0xFF00) );
  t0 = _FOG_FACE_COMBINE_2(t0, t1 & 0xFF00);

  xm >>= 16;
  ym >>= 16;

  t1 = (uint32_t)( ((((xm & 0x00FF) - (ym & 0x00FF)) * (z0_32 & 0xFFFF)) >> 8) + (ym & 0x00FF) );
  t0 = _FOG_FACE_COMBINE_2(t0, (t1 << 16));

  t1 = (uint32_t)( ((((xm & 0xFF00) - (ym & 0xFF00)) * (z0_32 >> 16   )) >> 8) + (ym & 0xFF00) );
  t0 = _FOG_FACE_COMBINE_2(t0, (t1 & 0xFF00) << 16);

  dst0 = t0;
}

//! @verbatim
//! dst0.u8[0] = ( x0.u8[0] * z0_10.u16[0] + y0.u8[0] * (256 - z0_10.u16[0]) ) / 256
//! dst0.u8[1] = ( x0.u8[1] * z0_10.u16[1] + y0.u8[1] * (256 - z0_10.u16[1]) ) / 256
//! dst0.u8[2] = ( x0.u8[2] * z0_32.u16[0] + y0.u8[2] * (256 - z0_32.u16[0]) ) / 256
//! dst0.u8[3] = 0x00
//! @endverbatim
static FOG_INLINE void p32Lerp256PBB_PBW_10Z2(
  p32& dst0, const p32& x0, const p32& y0, const p32& z0_10, const p32& z0_32)
{
  uint32_t xm = x0;
  uint32_t ym = y0;

  uint32_t t0;
  uint32_t t1;

  t0 = (uint32_t)( ((((xm & 0x00FF) - (ym & 0x00FF)) * (z0_10 & 0xFFFF)) >> 8) + (ym & 0x00FF) );
  t1 = (uint32_t)( ((((xm & 0xFF00) - (ym & 0xFF00)) * (z0_10 >> 16   )) >> 8) + (ym & 0xFF00) );
  t0 = _FOG_FACE_COMBINE_2(t0, t1 & 0xFF00);

  xm >>= 16;
  ym >>= 16;

  t1 = (uint32_t)( ((((xm & 0x00FF) - (ym & 0x00FF)) * (z0_32 & 0xFFFF)) >> 8) + (ym & 0x00FF) );
  t0 = _FOG_FACE_COMBINE_2(t0, (t1 << 16));

  dst0 = t0;
}

//! @verbatim
//! dst0.u8[0] = ( x0.u8[0] * z0_10.u16[0] + y0.u8[0] * (256 - z0_10.u16[0]) ) / 256
//! dst0.u8[1] = ( x0.u8[1] * z0_10.u16[1] + y0.u8[1] * (256 - z0_10.u16[1]) ) / 256
//! dst0.u8[2] = ( x0.u8[2] * z0_32.u16[0] + y0.u8[2] * (256 - z0_32.u16[0]) ) / 256
//! dst0.u8[3] = 0xFF
//! @endverbatim
static FOG_INLINE void p32Lerp256PBB_PBW_10F2(
  p32& dst0, const p32& x0, const p32& y0, const p32& z0_10, const p32& z0_32)
{
  p32Lerp256PBB_PBW_10Z2(dst0, x0, y0, z0_10, z0_32);
  p32FillPBB3(dst0, dst0);
}

//! @verbatim
//! dst0.u8[0] = ( x0.u8[0] * z0_10.u16[0] + y0.u8[0] * (256 - z0_10.u16[0]) ) / 256
//! dst0.u8[1] = ( x0.u8[1] * z0_10.u16[1] + y0.u8[1] * (256 - z0_10.u16[1]) ) / 256
//! dst0.u8[2] = ( x0.u8[2] * z0_32.u16[0] + y0.u8[2] * (256 - z0_32.u16[0]) ) / 256
//! dst0.u8[3] = ( x0.u8[3] * z0_32.u16[1] + y0.u8[3] * (256 - z0_32.u16[1]) ) / 256
//! @endverbatim
static FOG_INLINE void p32Lerp256PBB_PBW_2031(
  p32& dst0, const p32& x0, const p32& y0, const p32& z0_20, const p32& z0_31)
{
  uint32_t xm = x0;
  uint32_t ym = y0;

  uint32_t t0;
  uint32_t t1;

  t0 = (uint32_t)( ((((xm & 0x00FF) - (ym & 0x00FF)) * (z0_20 & 0xFFFF)) >> 8) + (ym & 0x00FF) );
  t1 = (uint32_t)( ((((xm & 0xFF00) - (ym & 0xFF00)) * (z0_31 & 0xFFFF)) >> 8) + (ym & 0xFF00) );
  t0 = _FOG_FACE_COMBINE_2(t0, t1 & 0xFF00);

  xm >>= 16;
  ym >>= 16;

  t1 = (uint32_t)( ((((xm & 0x00FF) - (ym & 0x00FF)) * (z0_20 >> 16)) >> 8) + (ym & 0x00FF) );
  t0 = _FOG_FACE_COMBINE_2(t0, (t1 << 16));

  t1 = (uint32_t)( ((((xm & 0xFF00) - (ym & 0xFF00)) * (z0_31 >> 16)) >> 8) + (ym & 0xFF00) );
  t0 = _FOG_FACE_COMBINE_2(t0, (t1 & 0xFF00) << 16);

  dst0 = t0;
}

//! @verbatim
//! dst0.u8[0] = ( x0.u8[0] * z0_10.u16[0] + y0.u8[0] * (256 - z0_10.u16[0]) ) / 256
//! dst0.u8[1] = ( x0.u8[1] * z0_10.u16[1] + y0.u8[1] * (256 - z0_10.u16[1]) ) / 256
//! dst0.u8[2] = ( x0.u8[2] * z0_32.u16[0] + y0.u8[2] * (256 - z0_32.u16[0]) ) / 256
//! dst0.u8[3] = 0x00
//! @endverbatim
static FOG_INLINE void p32Lerp256PBB_PBW_20Z1(
  p32& dst0, const p32& x0, const p32& y0, const p32& z0_20, const p32& z0_31)
{
  uint32_t xm = x0;
  uint32_t ym = y0;

  uint32_t t0;
  uint32_t t1;

  t0 = (uint32_t)( ((((xm & 0x00FF) - (ym & 0x00FF)) * (z0_20 & 0xFFFF)) >> 8) + (ym & 0x00FF) );
  t1 = (uint32_t)( ((((xm & 0xFF00) - (ym & 0xFF00)) * (z0_31 & 0xFFFF)) >> 8) + (ym & 0xFF00) );
  t0 = _FOG_FACE_COMBINE_2(t0, t1 & 0xFF00);

  xm >>= 16;
  ym >>= 16;

  t1 = (uint32_t)( ((((xm & 0x00FF) - (ym & 0x00FF)) * (z0_20 >> 16)) >> 8) + (ym & 0x00FF) );
  t0 = _FOG_FACE_COMBINE_2(t0, (t1 << 16));

  dst0 = t0;
}

//! @verbatim
//! dst0.u8[0] = ( x0.u8[0] * z0_10.u16[0] + y0.u8[0] * (256 - z0_10.u16[0]) ) / 256
//! dst0.u8[1] = ( x0.u8[1] * z0_10.u16[1] + y0.u8[1] * (256 - z0_10.u16[1]) ) / 256
//! dst0.u8[2] = ( x0.u8[2] * z0_32.u16[0] + y0.u8[2] * (256 - z0_32.u16[0]) ) / 256
//! dst0.u8[3] = 0x00
//! @endverbatim
static FOG_INLINE void p32Lerp256PBB_PBW_20F1(
  p32& dst0, const p32& x0, const p32& y0, const p32& z0_20, const p32& z0_31)
{
  p32Lerp256PBB_PBW_20Z1(dst0, x0, y0, z0_20, z0_31);
  p32FillPBB3(dst0, dst0);
}

//! @verbatim
//! dst0.u16[0] = min((x0.u16[0] * z0.u32[0] + y0.u16[0] * w0.u32[0]) / 255, 255)
//! dst0.u16[1] = min((x0.u16[1] * z0.u32[0] + y0.u16[1] * w0.u32[0]) / 255, 255)
//! @endverbatim
static FOG_INLINE void p32Lerpus255PBW_SBW(
  p32& dst0, const p32& x0, const p32& y0, const p32& z0, const p32& w0)
{
  p32 t0;

  p32MulDiv255PBW_SBW(t0  , x0, z0);
  p32MulDiv255PBW_SBW(dst0, y0, z0);
  p32Addus255PBW(dst0, dst0, t0);
}

//! @verbatim
//! dst0.u16[0] = min((x0.u16[0] * z0.u32[0] + y0.u16[0] * w0.u32[0]) / 255, 255)
//! dst0.u16[1] = min((x0.u16[1] * z0.u32[0] + y0.u16[1] * w0.u32[0]) / 255, 255)
//! dst1.u16[0] = min((x1.u16[0] * z1.u32[0] + y1.u16[0] * w1.u32[0]) / 255, 255)
//! dst1.u16[1] = min((x1.u16[1] * z1.u32[0] + y1.u16[1] * w1.u32[0]) / 255, 255)
//! @endverbatim
static FOG_INLINE void p32Lerpus255PBW_SBW_2x(
  p32& dst0, const p32& x0, const p32& y0, const p32& z0, const p32& w0,
  p32& dst1, const p32& x1, const p32& y1, const p32& z1, const p32& w1)
{
  p32 t0, t1;

  p32MulDiv255PBW_SBW_2x(t0, x0, z0, t1, x1, z1);
  p32MulDiv255PBW_SBW_2x(dst0, y0, z0, dst1, y1, z1);
  p32Addus255PBW_2x(dst0, dst0, t0, dst1, dst1, t1);
}

//! @verbatim
//! dst0.u16[0] = min((x0.u16[0] * z0.u32[0] + y0.u16[0] * w0.u32[0]) / 256, 255)
//! dst0.u16[1] = min((x0.u16[1] * z0.u32[0] + y0.u16[1] * w0.u32[0]) / 256, 255)
//! @endverbatim
static FOG_INLINE void p32Lerpus256PBW_SBW(
  p32& dst0, const p32& x0, const p32& y0, const p32& z0, const p32& w0)
{
  uint32_t t0 = ((x0 * z0) >> 8) & 0x00FF00FFU;
  uint32_t t1 = ((y0 * w0) >> 8) & 0x00FF00FFU;

  p32Addus255PBW(dst0, t0, t1);
}

//! @verbatim
//! dst0.u16[0] = min((x0.u16[0] * z0.u32[0] + y0.u16[0] * w0.u32[0]) / 256, 255)
//! dst0.u16[1] = min((x0.u16[1] * z0.u32[0] + y0.u16[1] * w0.u32[0]) / 256, 255)
//! dst1.u16[0] = min((x1.u16[0] * z1.u32[0] + y1.u16[0] * w1.u32[0]) / 256, 255)
//! dst1.u16[1] = min((x1.u16[1] * z1.u32[0] + y1.u16[1] * w1.u32[0]) / 256, 255)
//! @endverbatim
static FOG_INLINE void p32Lerpus256PBW_SBW_2x(
  p32& dst0, const p32& x0, const p32& y0, const p32& z0, const p32& w0,
  p32& dst1, const p32& x1, const p32& y1, const p32& z1, const p32& w1)
{
  uint32_t t0 = ((x0 * z0) >> 8) & 0x00FF00FFU;
  uint32_t t1 = ((x1 * z1) >> 8) & 0x00FF00FFU;

  dst0 = ((y0 * w0) >> 8) & 0x00FF00FFU;
  dst1 = ((y1 * w1) >> 8) & 0x00FF00FFU;

  p32Addus255PBW_2x(dst0, dst0, t0, dst1, dst1, t1);
}

// ============================================================================
// [Fog::Face - P32 - Lerp_Pack]
// ============================================================================

static FOG_INLINE void p32Lerp255_Pack2031(
  p32& dst0,
  const p32& x0, const p32& y0, const p32& z0, const p32& w0,
  const p32& x1, const p32& y1, const p32& z1, const p32& w1)
{
  uint32_t t0 = x0 * z0 + y0 * w0;
  uint32_t t1 = x1 * z1 + y1 * w1;

  t0 = (t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U);
  t1 = (t1 + ((t1 >> 8) & 0x00FF00FFU) + 0x00800080U) << 8;

  t0 &= 0x00FF00FFU;
  t1 &= 0xFF00FF00U;

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
}

static FOG_INLINE void p32Lerp255_Pack20Z1(
  p32& dst0,
  const p32& x0, const p32& y0, const p32& z0, const p32& w0,
  const p32& x1, const p32& y1, const p32& z1, const p32& w1)
{
  uint32_t t0 = x0 * z0 + y0 * w0;
  uint32_t t1 = x1 * z1 + y1 * w1;

  t0 = (t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U);
  t1 = (t1 + ((t1 >> 8) & 0x00FF00FFU) + 0x00800080U) << 8;

  t0 &= 0x00FF00FFU;
  t1 &= 0x0000FF00U;

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
}

static FOG_INLINE void p32Lerp256_Pack2031(
  p32& dst0,
  const p32& x0, const p32& y0, const p32& z0, const p32& w0,
  const p32& x1, const p32& y1, const p32& z1, const p32& w1)
{
  uint32_t t0 = x0 * z0 + y0 * w0;
  uint32_t t1 = x1 * z1 + y1 * w1;

  t0 >>= 8;

  t0 &= 0x00FF00FFU;
  t1 &= 0xFF00FF00U;

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
}

static FOG_INLINE void p32Lerp256_Pack20Z1(
  p32& dst0,
  const p32& x0, const p32& y0, const p32& z0, const p32& w0,
  const p32& x1, const p32& y1, const p32& z1, const p32& w1)
{
  uint32_t t0 = x0 * z0 + y0 * w0;
  uint32_t t1 = x1 * z1 + y1 * w1;

  t0 >>= 8;

  t0 &= 0x00FF00FFU;
  t1 &= 0x0000FF00U;

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
}

// ============================================================================
// [Fog::Face - P32 - Special]
// ============================================================================

static FOG_INLINE void p32MulDiv256PBB_3Z1Z_(
  p32& dst0, const p32& x0, const p32& u0)
{
  dst0 = (((x0 & 0xFF00FF00U) >> 8) * u0) & 0xFF00FF00U;
}

static FOG_INLINE void p32MulDiv256PBB_Z2Z0_(
  p32& dst0, const p32& x0, const p32& u0)
{
  dst0 = (((x0 & 0x00FF00FFU) * u0) >> 8) & 0x00FF00FFU;
}

//! @}

} // Face namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_FACE_FACE_C_P32_H
