// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_ACC_ACCC_H
#define _FOG_G2D_ACC_ACCC_H

// [Dependencies]
#include <Fog/Core/Acc/AccC.h>
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Math/Fuzzy.h>
#include <Fog/Core/Math/Math.h>

namespace Fog {
namespace Acc {

//! @addtogroup Fog_G2d_Acc_C
//! @{

// ============================================================================
// [Fog::Acc - Raster - Documentation]
// ============================================================================

// RGB16_555:
//
//   [0RRRRRGG] [GGGBBBBB] [0RRRRRGG] [GGGBBBBB]
//   [00000000] [00000000] [00000000] [00011111] -> 0x0000001F
//   [00000000] [00000000] [00000011] [11100000] -> 0x000003E0
//   [00000000] [00000000] [01111100] [00000000] -> 0x00007C00
//   [00000000] [00000000] [01111100] [00011111] -> 0x00007C1F
//   [00000011] [11100000] [01111100] [00011111] -> 0x03E07C1F
//   [01111100] [00001111] [10000011] [11100000] -> 0x7C0F83E0
//
// RGB16_565:
//
//   [RRRRRGGG] [GGGBBBBB] [RRRRRGGG] [GGGBBBBB]
//   [00000000] [00000000] [00000000] [00011111] -> 0x0000001F
//   [00000000] [00000000] [00000111] [11100000] -> 0x000007E0
//   [00000000] [00000000] [11111000] [00000000] -> 0x0000F800
//   [00000000] [00000000] [11111000] [00011111] -> 0x0000F81F
//   [00000111] [11100000] [11111000] [00011111] -> 0x07E0F81F
//   [11111000] [00011111] [00000111] [11100000] -> 0xF81F07E0
//   [01111100] [00011111] [00000011] [11100000] -> 0x7C1F03E0

// ============================================================================
// [Fog::Acc - Raster - Helpers]
// ============================================================================

static FOG_INLINE void p32UnpackPBW256FromPBB255_2031(uint32_t& dst0_20, uint32_t& dst0_31, const uint32_t& x0)
{
  uint32_t t0 = x0;

  dst0_20 = (t0     ) & 0x00FF00FF;
  dst0_31 = (t0 >> 8) & 0x00FF00FF;

  p32Cvt256PBWFrom255PBW_2x(dst0_20, dst0_20, dst0_31, dst0_31);
}

static FOG_INLINE void p32UnpackPBW256FromPBB255_20Z1(uint32_t& dst0_20, uint32_t& dst0_31, const uint32_t& x0)
{
  uint32_t t0 = x0;

  dst0_20 = (t0     ) & 0x00FF00FF;
  dst0_31 = (t0 >> 8) & 0x000000FF;

  p32Cvt256PBWFrom255PBW_2x(dst0_20, dst0_20, dst0_31, dst0_31);
}

static FOG_INLINE void p32OpOverA8(uint32_t& dst0, const uint32_t& x0, const uint32_t& y0)
{
  uint32_t t0;
  uint32_t t1;
  
  p32Add(t0, x0, y0);
  p32MulDiv255SBW(t1, x0, y0);
  p32Sub(t0, t0, t1);
}

// ============================================================================
// [Fog::Acc - XRGB32 - From - RGB24]
// ============================================================================

static FOG_INLINE void p32XRGB32QuadFromRGB24Quad(
  uint32_t& dst0, uint32_t& dst1, uint32_t& dst2, uint32_t& dst3,
  const uint32_t& x0, const uint32_t& x1, const uint32_t& x2)
{
  uint32_t t0 = x0;
  uint32_t t1 = x1;
  uint32_t t2 = x2;

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  // [B0 G0 R0 B1] [G1 R1 B2 G2] [R2 B3 G3 R3]               - Memory
  // [B1 R0 G0 B0] [G2 B2 R1 G1] [R3 G3 B3 R2]               - CPU

  // [B0 G0 R0 X0] [B1 G1 R1 X1] [B2 G2 R2 X2] [B3 G3 R3 X3] - Memory
  // [X0 R0 G0 B0] [X1 R1 G1 B1] [X2 R2 G2 B2] [X3 R3 G3 B3] - CPU

  dst0 = (t0      );
  dst1 = (t0 >> 24) | (t1 <<  8);
  dst2 = (t1 >> 16) | (t2 << 16);
  dst3 = (t2 >>  8);
#else
  // [R0 G0 B0 R1] [G1 B1 R2 G2] [B2 R3 G3 B3]               - Memory
  // [R0 G0 B0 R1] [G1 B1 R2 G2] [B2 R3 G3 B3]               - CPU

  // [X0 R0 G0 B0] [X1 R1 G1 B1] [X2 R2 G2 B2] [X3 R3 G3 B3] - Memory
  // [X0 R0 G0 B0] [X1 R1 G1 B1] [X2 R2 G2 B2] [X3 R3 G3 B3] - CPU

  dst0 = (t0 >>  8);
  dst1 = (t0 << 16) | (t1 >> 16);
  dst2 = (t1 <<  8) | (t2 >> 24);
  dst3 = (t2      );
#endif // FOG_BYTE_ORDER
}

static FOG_INLINE void p32ZRGB32QuadFromRGB24Quad(
  uint32_t& dst0, uint32_t& dst1, uint32_t& dst2, uint32_t& dst3,
  const uint32_t& x0, const uint32_t& x1, const uint32_t& x2)
{
  p32XRGB32QuadFromRGB24Quad(dst0, dst1, dst2, dst3, x0, x1, x2);

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  dst0 &= 0x00FFFFFFU;
  dst1 &= 0x00FFFFFFU;
  dst2 &= 0x00FFFFFFU;
#else
  dst1 &= 0x00FFFFFFU;
  dst2 &= 0x00FFFFFFU;
  dst3 &= 0x00FFFFFFU;
#endif // FOG_BYTE_ORDER
}

static FOG_INLINE void p32FRGB32QuadFromRGB24Quad(
  uint32_t& dst0, uint32_t& dst1, uint32_t& dst2, uint32_t& dst3,
  const uint32_t& x0, const uint32_t& x1, const uint32_t& x2)
{
  p32XRGB32QuadFromRGB24Quad(dst0, dst1, dst2, dst3, x0, x1, x2);

  dst0 |= 0xFF000000;
  dst1 |= 0xFF000000;
  dst2 |= 0xFF000000;
  dst3 |= 0xFF000000;
}

// ============================================================================
// [Fog::Acc - XRGB32 - From - RGB16_555]
// ============================================================================

static FOG_INLINE void p32ZRGB32FromRGB16_555(uint32_t& dst0, const uint32_t& x0)
{
  //                                 // [XXXXXXXX] [XXXXXXXX] [XRRRRRGG] [GGGBBBBB]
  uint32_t t0 = x0;
  uint32_t t1;
  uint32_t t2;

  t0 = (t0 * 0x00080008U);           // [RRRGGGGG] [BBBBBXRR] [RRRGGGGG] [BBBBB000]
  t0 = (t0 & 0x1F03E0F8U);           // [000GGGGG] [000000RR] [RRR00000] [BBBBB000]
  t0 = (t0 | (t0 >> 5));             // [000GGGGG] [GGGGG0RR] [RRRRRRRR] [BBBBBBBB]

  t1 = t0 >> 13;                     // [00000000] [00000000] [GGGGGGGG] [GG0RRRRR]
  t2 = t0 << 6;                      // [GGGGGGG0] [RRRRRRRR] [RRBBBBBB] [BB000000]

  t0 = t0 & 0x000000FFU;             // [00000000] [00000000] [00000000] [BBBBBBBB]
  t1 = t1 & 0x0000FF00U;             // [00000000] [00000000] [GGGGGGGG] [00000000]
  t2 = t2 & 0x00FF0000U;             // [00000000] [RRRRRRRR] [00000000] [00000000]

  dst0 = _FOG_ACC_COMBINE_3(t0, t1, t2);
}

static FOG_INLINE void p32FRGB32FromRGB16_555(uint32_t& dst0, const uint32_t& x0)
{
  p32ZRGB32FromRGB16_555(dst0, x0);
  p32FillPBB3(dst0, dst0);
}

static FOG_INLINE void p32ZRGB32QuadFromRGB16_555Quad(uint32_t& dst0, uint32_t& dst1, uint32_t& dst2, uint32_t& dst3, const uint32_t& x0, const uint32_t& x1)
{
  uint32_t t0 = x0;                  // [XRRRRRGG] [GGGBBBBB] [XRRRRRGG] [GGGBBBBB]
  uint32_t t1 = t0;                  // [XRRRRRGG] [GGGBBBBB] [XRRRRRGG] [GGGBBBBB]

  uint32_t t2 = x1;                  // [XRRRRRGG] [GGGBBBBB] [XRRRRRGG] [GGGBBBBB]
  uint32_t t3 = t2;                  // [XRRRRRGG] [GGGBBBBB] [XRRRRRGG] [GGGBBBBB]

  t0 = (t0 & 0x03E07C1FU);           // [000000GG] [GGG00000] [0RRRRR00] [000BBBBB]
  t1 = (t1 & 0x7C0F83E0U);           // [0RRRRR00] [000BBBBB] [000000GG] [GGG00000]

  t2 = (t2 & 0x03E07C1FU);           // [000000GG] [GGG00000] [0RRRRR00] [000BBBBB]
  t3 = (t3 & 0x7C0F83E0U);           // [0RRRRR00] [000BBBBB] [000000GG] [GGG00000]

  t0 = t0 * 0x21;                    // [0GGGGGGG] [GGG0RRRR] [RRRRRRBB] [BBBBBBBB]
  t2 = t2 * 0x21;                    // [0GGGGGGG] [GGG0RRRR] [RRRRRRBB] [BBBBBBBB]

  t1 = (t1 >> 5) * 0x21;             // [0RRRRRRR] [RRRBBBBB] [BBBBB0GG] [GGGGGGGG]
  t3 = (t3 >> 5) * 0x21;             // [0RRRRRRR] [RRRBBBBB] [BBBBB0GG] [GGGGGGGG]

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  // [1][0][3][2]
  dst0 = _FOG_ACC_COMBINE_3((t0 >>  2) & 0x000000FFU, (t0 <<  4) & 0x0000FF00U, (t1 << 6) & 0x00FF0000U);
  dst1 = _FOG_ACC_COMBINE_3((t1 >> 11) & 0x000000FFU, (t0 >> 15) & 0x0000FF00U, (t1 >> 7) & 0x00FF0000U);
  dst2 = _FOG_ACC_COMBINE_3((t2 >>  2) & 0x000000FFU, (t2 <<  4) & 0x0000FF00U, (t3 << 6) & 0x00FF0000U);
  dst3 = _FOG_ACC_COMBINE_3((t3 >> 11) & 0x000000FFU, (t2 >> 15) & 0x0000FF00U, (t3 >> 7) & 0x00FF0000U);
#else
  // [0][1][2][3]
  dst0 = _FOG_ACC_COMBINE_3((t1 >>  2) & 0x000000FFU, (t1 <<  4) & 0x0000FF00U, (t0 << 6) & 0x00FF0000U);
  dst1 = _FOG_ACC_COMBINE_3((t0 >> 11) & 0x000000FFU, (t1 >> 15) & 0x0000FF00U, (t0 >> 7) & 0x00FF0000U);
  dst2 = _FOG_ACC_COMBINE_3((t3 >>  2) & 0x000000FFU, (t3 <<  4) & 0x0000FF00U, (t2 << 6) & 0x00FF0000U);
  dst3 = _FOG_ACC_COMBINE_3((t2 >> 11) & 0x000000FFU, (t3 >> 15) & 0x0000FF00U, (t2 >> 7) & 0x00FF0000U);
#endif // FOG_BYTE_ORDER
}

static FOG_INLINE void p32FRGB32QuadFromRGB16_555Quad(uint32_t& dst0, uint32_t& dst1, uint32_t& dst2, uint32_t& dst3, const uint32_t& x0, const uint32_t& x1)
{
  p32ZRGB32QuadFromRGB16_555Quad(dst0, dst1, dst2, dst3, x0, x1);
  p32FillPBB3(dst0, dst0);
  p32FillPBB3(dst1, dst1);
  p32FillPBB3(dst2, dst2);
  p32FillPBB3(dst3, dst3);
}

// ============================================================================
// [Fog::Acc - XRGB32 - From - RGB16_565]
// ============================================================================

static FOG_INLINE void p32ZRGB32FromRGB16_565(uint32_t& dst0, const uint32_t& x0)
{
  //                                 // [XXXXXXXX] [XXXXXXXX] [RRRRRGGG] [GGGBBBBB]
  uint32_t t0 = x0;
  uint32_t t1 = x0;
  uint32_t t2;

  t0 = (t0 & 0x0000F81FU);           // [00000000] [00000000] [RRRRR000] [000BBBBB]
  t1 = (t1 & 0x000007E0U);           // [00000000] [00000000] [00000GGG] [GGG00000]

  t0 = (t0 * 0x21);                  // [00000000] [000RRRRR] [RRRRR0BB] [BBBBBBBB]
  t1 = (t1 * 0x41);                  // [00000000] [0000000G] [GGGGGGGG] [GGG00000]

  t2 = (t0 << 3);                    // [00000000] [RRRRRRRR] [RR0BBBBB] [BBBBB000]
  t0 = (t0 >> 2);                    // [00000000] [00000RRR] [RRRRRRR0] [BBBBBBBB]
  t1 = (t1 >> 1);                    // [00000000] [00000000] [GGGGGGGG] [GGGG0000]

  t0 = t0 & 0x000000FFU;             // [00000000] [00000000] [00000000] [BBBBBBBB]
  t1 = t1 & 0x0000FF00U;             // [00000000] [00000000] [GGGGGGGG] [00000000]
  t2 = t2 & 0x00FF0000U;             // [00000000] [RRRRRRRR] [00000000] [00000000]

  dst0 = _FOG_ACC_COMBINE_3(t0, t1, t2);
}

static FOG_INLINE void p32FRGB32FromRGB16_565(uint32_t& dst0, const uint32_t& x0)
{
  p32ZRGB32FromRGB16_565(dst0, x0);
  p32FillPBB3(dst0, dst0);
}

static FOG_INLINE void p32ZRGB32QuadFromRGB16_565Quad(uint32_t& dst0, uint32_t& dst1, uint32_t& dst2, uint32_t& dst3, const uint32_t& x0, const uint32_t& x1)
{
  uint32_t t0 = x0;
  uint32_t t1 = x1;

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  p32ZRGB32FromRGB16_565(dst0, t0);
  t0 >>= 16;
  p32ZRGB32FromRGB16_565(dst1, t0);
  p32ZRGB32FromRGB16_565(dst2, t1);
  t1 >>= 16;
  p32ZRGB32FromRGB16_565(dst3, t1);
#else
  p32ZRGB32FromRGB16_565(dst1, t0);
  t0 >>= 16;
  p32ZRGB32FromRGB16_565(dst0, t0);
  p32ZRGB32FromRGB16_565(dst3, t1);
  t1 >>= 16;
  p32ZRGB32FromRGB16_565(dst2, t1);
#endif // FOG_BYTE_ORDER
}

static FOG_INLINE void p32FRGB32QuadFromRGB16_565Quad(uint32_t& dst0, uint32_t& dst1, uint32_t& dst2, uint32_t& dst3, const uint32_t& x0, const uint32_t& x1)
{
  p32ZRGB32QuadFromRGB16_565Quad(dst0, dst1, dst2, dst3, x0, x1);
  p32FillPBB3(dst0, dst0);
  p32FillPBB3(dst1, dst1);
  p32FillPBB3(dst2, dst2);
  p32FillPBB3(dst3, dst3);
}

// ============================================================================
// [Fog::Acc - XRGB32 - From - ARGB64]
// ============================================================================

static FOG_INLINE void p32ZRGB32FromARGB64_1032(
  uint32_t& dst0, const uint32_t& x0_10, const uint32_t& x0_32)
{
  uint32_t xa = x0_32 >> 16;

  uint32_t t0 = (x0_10 & 0xFFFF) * xa;
  uint32_t t1 = (x0_10    >> 16) * xa;
  uint32_t t2 = (x0_32 & 0xFFFF) * xa;

  t0 = (t0 + (t0 >> 16) + 0x8000U);
  t1 = (t1 + (t1 >> 16) + 0x8000U);
  t2 = (t2 + (t2 >> 16) + 0x8000U);

  dst0 = _FOG_ACC_COMBINE_3((t1 >> 16) & 0x0000FF00U, (t2 >>  8) & 0x00FF0000U, (t0 >> 24));
}

static FOG_INLINE void p32ZRGB32FromARGB64_2031(
  uint32_t& dst0, const uint32_t& x0_20, const uint32_t& x0_31)
{
  uint32_t xa = x0_31 >> 16;

  uint32_t t0 = (x0_20 & 0xFFFF) * xa;
  uint32_t t1 = (x0_31 & 0xFFFF) * xa;
  uint32_t t2 = (x0_20 >>    16) * xa;

  t0 = (t0 + (t0 >> 16) + 0x8000U);
  t1 = (t1 + (t1 >> 16) + 0x8000U);
  t2 = (t2 + (t2 >> 16) + 0x8000U);

  dst0 = _FOG_ACC_COMBINE_3((t1 >> 16) & 0x0000FF00U, (t2 >>  8) & 0x00FF0000U, (t0 >> 24));
}

// ============================================================================
// [Fog::Acc - XRGB32 - From - PRGB64]
// ============================================================================

static FOG_INLINE void p32ZRGB32FromPRGB64_1032(
  uint32_t& dst0, const uint32_t& x0_10, const uint32_t& x0_32)
{
#if defined(FOG_ARCH_HAS_FAST_MUL)
  dst0 = _FOG_ACC_COMBINE_2(((x0_10 & 0xFF00FF00U) * 0x0101U) >> 16,
                            ((x0_32 << 8) & 0x00FF0000U));
#else
  dst0 = _FOG_ACC_COMBINE_3((x0_10 >>  8) & 0x000000FFU,
                            (x0_10 >> 16) & 0x0000FF00U,
                            (x0_32 <<  8) & 0x00FF0000U);
#endif
}

static FOG_INLINE void p32ZRGB32FromPRGB64_2031(
  uint32_t& dst0, const uint32_t& x0_20, const uint32_t& x0_31)
{
  dst0 = _FOG_ACC_COMBINE_2((x0_20 >>  8) & 0x00FF00FFU,
                            (x0_31      ) & 0x0000FF00U);
}

// ============================================================================
// [Fog::Acc - XRGB32 - From - RGB48]
// ============================================================================

static FOG_INLINE void p32ZRGB32FromRGB48(
  uint32_t& dst0, const uint32_t& x0_r, const uint32_t& x0_g, const uint32_t& x0_b)
{
  dst0 = _FOG_ACC_COMBINE_2(_FOG_ACC_COMBINE_2(x0_r << 8, x0_b >> 8) & 0x00FF00FFU, x0_g & 0x0000FF00U);
}

static FOG_INLINE void p32FRGB32FromRGB48(
  uint32_t& dst0, const uint32_t& x0_r, const uint32_t& x0_g, const uint32_t& x0_b)
{
  dst0 = (_FOG_ACC_COMBINE_2(x0_r << 8, x0_b >> 8) | 0xFF00FF00U) & (x0_g | 0xFFFF00FF);
}

// ============================================================================
// [Fog::Acc - ARGB32 - IsAlphaXX]
// ============================================================================

//! @brief Get whether the ARGB32 pixel @a c0 has alpha set to 0xFF (255 in decimal).
static FOG_INLINE bool p32ARGB32IsAlphaFF(const uint32_t& c0) { return (c0 >= 0xFF000000); }

//! @brief Get whether the ARGB32 pixel @a c0 has alpha set to 0x00 (0 in decimal).
static FOG_INLINE bool p32ARGB32IsAlpha00(const uint32_t& c0) { return (c0 <= 0x00FFFFFF); }

// ============================================================================
// [Fog::Acc - ARGB32 - From - PRGB32]
// ============================================================================

static FOG_INLINE void p32ARGB32FromPRGB32_1032(
  uint32_t& dst0_10, uint32_t& dst0_32, const uint32_t& x0_10, const uint32_t& x0_32)
{
  uint32_t xa = x0_32 >> 16;
  uint32_t recip = _u8_divide_table_d[xa];

  uint32_t xr = ((x0_32 & 0xFF) * recip) >> 16;
  uint32_t xg = ((x0_10 >>  16) * recip) & 0x00FF0000;
  uint32_t xb = ((x0_10 & 0xFF) * recip) >> 16;

  xa <<= 16;

  dst0_10 = _FOG_ACC_COMBINE_2(xg, xb);
  dst0_32 = _FOG_ACC_COMBINE_2(xa, xr);
}

static FOG_INLINE void p32ARGB32FromPRGB32_2031(
  uint32_t& dst0_20, uint32_t& dst0_31, const uint32_t& x0_20, const uint32_t& x0_31)
{
  uint32_t xa = x0_31 >> 16;
  uint32_t recip = _u8_divide_table_d[xa];

  uint32_t xr = ((x0_20 >>  16) * recip) & 0x00FF0000;
  uint32_t xg = ((x0_31 & 0xFF) * recip) >> 16;
  uint32_t xb = ((x0_20 & 0xFF) * recip) >> 16;

  xa <<= 16;

  dst0_20 = _FOG_ACC_COMBINE_2(xr, xb);
  dst0_31 = _FOG_ACC_COMBINE_2(xa, xg);
}

static FOG_INLINE void p32ARGB32FromPRGB32(
  uint32_t& dst0, const uint32_t& x0)
{
  uint32_t xa = x0 >> 24;
  uint32_t recip = _u8_divide_table_d[xa];

  uint32_t xr = ( (((x0 >> 16) & 0xFF) * recip)      ) & 0x00FF0000;
  uint32_t xg = ( (((x0 >>  8) & 0xFF) * recip) >>  8) & 0x0000FF00;
  uint32_t xb = ( (((x0      ) & 0xFF) * recip) >> 16);

  dst0 = _FOG_ACC_COMBINE_4(xa << 24, xr, xg, xb);
}

// ============================================================================
// [Fog::Acc - ARGB32 - From - ARGB16_4444]
// ============================================================================

static FOG_INLINE void p32ARGB32FromARGB16_4444(
  uint32_t& dst0, const uint32_t& x0)
{
  uint32_t t0 = x0;                       // T0 = [00][00][AR][GB]
  uint32_t t1;
  uint32_t t2;

  p32LShift(t1, t0, 12);       // T1 = [0A][RG][B0][00]
  p32LShift(t2, t0, 4);        // T2 = [00][0A][RG][B0]

  p32Or(t0, t0, t1);           // T0 = [0A][RG][XR][GB]
  p32LShift(t1, t2, 4);        // T1 = [00][AR][GB][00]

  p32And(t0, t0, 0x0F00000F);  // T0 = [0A][00][00][0B]
  p32And(t1, t1, 0x000F0000);  // T1 = [00][0R][00][00]
  p32And(t2, t2, 0x00000F00);  // T2 = [00][00][0G][00]

  p32Combine(t0, t0, t1);      // T0 = [0A][0R][00][0B]
  p32Combine(t0, t0, t2);      // T0 = [0A][0R][0G][0B]

  p32Mul(dst0, t0, 17);        // T0 = [AA][RR][GG][BB]
}

static FOG_INLINE void p32ARGB32FromARGB16_4444_bs(
  uint32_t& dst0, const uint32_t& x0)
{
  uint32_t t0 = x0;                       // T0 = [00][00][BG][AR]
  uint32_t t1;
  uint32_t t2;

  p32RShift(t1, t0, 12);       // T1 = [00][00][00][0B]
  p32LShift(t2, t0, 16);       // T2 = [BG][AR][00][00]

  p32And(t0, t0, 0x00000F00);  // T0 = [00][00][0G][00]
  p32Combine(t1, t1, t2);      // T1 = [BG][AR][00][0B]
  p32LShift(t2, t2, 4);        // T2 = [GA][R0][00][00]

  p32And(t1, t1, 0x000F000F);  // T1 = [00][0R][00][0B]
  p32And(t2, t2, 0x0F000000);  // T2 = [0A][00][00][00]

  p32Combine(t0, t0, t1);      // T0 = [00][0R][0G][0B]
  p32Combine(t0, t0, t2);      // T0 = [0A][0R][0G][0B]

  p32Mul(dst0, t0, 17);        // T0 = [AA][RR][GG][BB]
}

// ============================================================================
// [Fog::Acc - ARGB32 - From - ARGB64]
// ============================================================================

static FOG_INLINE void p32ARGB32FromARGB64_1032(
  uint32_t& dst0, const uint32_t& x0_10, const uint32_t& x0_32)
{
#if defined(FOG_ARCH_HAS_FAST_MUL)
  dst0 = _FOG_ACC_COMBINE_2(((x0_10 & 0xFF00FF00U) * 0x0101U) >> 16,
                            ((x0_32 & 0xFF00FF00U) * 0x0101U) & 0xFFFF0000U);
#else
  dst0 = _FOG_ACC_COMBINE_4((x0_10 >>  8) & 0x000000FFU,
                            (x0_10 >> 16) & 0x0000FF00U,
                            (x0_32 <<  8) & 0x00FF0000U,
                            (x0_32      ) & 0xFF000000U);
#endif
}

static FOG_INLINE void p32ARGB32FromARGB64_2031(
  uint32_t& dst0, const uint32_t& x0_20, const uint32_t& x0_31)
{
  dst0 = _FOG_ACC_COMBINE_2((x0_20 >>  8) & 0x00FF00FFU,
                            (x0_31      ) & 0xFF00FF00U);
}

static FOG_INLINE void p64ARGB32FromARGB64(
  uint32_t& dst0, const __p64& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = _FOG_ACC_COMBINE_4((uint32_t)(x0 >>  8) & 0x000000FFU,
                            (uint32_t)(x0 >> 16) & 0x0000FF00U,
                            (uint32_t)(x0 >> 24) & 0x00FF0000U,
                            (uint32_t)(x0 >> 32) & 0xFF000000U);
#else
  p32ARGB32FromARGB64_1032(dst0, x0.u32Lo, x0.u32Hi);
#endif
}

// ============================================================================
// [Fog::Acc - ARGB32 - From - PRGB64]
// ============================================================================

static FOG_INLINE void p32ARGB32FromPRGB64_1032(
  uint32_t& dst0, const uint32_t& x0_10, const uint32_t& x0_32)
{
  uint32_t xa = x0_32 >> 24;
  uint32_t recip = _u8_divide_table_d[xa];

  uint32_t xr = (x0_32 & 0xFFFF) * recip;
  uint32_t xg = (x0_10    >> 16) * recip;
  uint32_t xb = (x0_10 & 0xFFFF) * recip;

  dst0 = _FOG_ACC_COMBINE_4((xa << 24),
                            (xr >>  8) & 0x00FF0000U,
                            (xg >> 16) & 0x0000FF00U,
                            (xb >> 24));
}

static FOG_INLINE void p32ARGB32FromPRGB64_2031(
  uint32_t& dst0, const uint32_t& x0_20, const uint32_t& x0_31)
{
  uint32_t xa = x0_31 >> 24;
  uint32_t recip = _u8_divide_table_d[xa];

  uint32_t xr = (x0_20    >> 16) * recip;
  uint32_t xg = (x0_31 & 0xFFFF) * recip;
  uint32_t xb = (x0_20 & 0xFFFF) * recip;

  dst0 = _FOG_ACC_COMBINE_4((xa << 24),
                            (xr >>  8) & 0x00FF0000U,
                            (xg >> 16) & 0x0000FF00U,
                            (xb >> 24));
}

// ============================================================================
// [Fog::Acc - PRGB32 - IsAlphaXX]
// ============================================================================

//! @brief Get whether the PRGB32 pixel @a c0 has alpha set to 0xFF (255 in decimal).
static FOG_INLINE bool p32PRGB32IsAlphaFF(const uint32_t& c0) { return (c0 >= 0xFF000000); }

//! @brief Get whether the PRGB32 pixel @a c0 has alpha set to 0x00 (0 in decimal).
static FOG_INLINE bool p32PRGB32IsAlpha00(const uint32_t& c0) { return (c0 == 0x00000000); }

// ============================================================================
// [Fog::Acc - PRGB32 - From - ARGB32]
// ============================================================================

static FOG_INLINE void p32PRGB32FromARGB32(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& u0)
{
#if FOG_ARCH_BITS >= 64
  uint64_t t0 = (uint64_t)x0 | ((uint64_t)(x0 | 0xFF000000) << 24);

  t0 &= FOG_UINT64_C(0x00FF00FF00FF00FF);
  t0 *= u0;
  t0 = ((t0 + ((t0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF)) + FOG_UINT64_C(0x0080008000800080)) >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF);

  dst0 = (uint32_t)_FOG_ACC_COMBINE_2(t0, t0 >> 24);
#else
  uint32_t t0 = (x0     ) & 0x00FF00FFU;
  uint32_t t1 = (x0 >> 8) & 0x00FF00FFU;

  t1 |= 0x00FF0000U;

  t0 *= u0;
  t1 *= u0;

  t0 = ((t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;
  t1 = ((t1 + ((t1 >> 8) & 0x00FF00FFU) + 0x00800080U)     ) & 0xFF00FF00U;

  dst0 = _FOG_ACC_COMBINE_2(t0, t1);
#endif // FOG_ARCH_BITS
}

static FOG_INLINE void p32PRGB32FromARGB32(
  uint32_t& dst0, const uint32_t& x0)
{
  uint32_t u0 = x0 >> 24;
  p32PRGB32FromARGB32(dst0, x0, u0);
}

static FOG_INLINE void p32ZRGB32FromARGB32(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& u0)
{
#if FOG_ARCH_BITS >= 64
  uint64_t t0 = (uint64_t)x0 | ((uint64_t)(x0) << 24);

  t0 &= FOG_UINT64_C(0x000000FF00FF00FF);
  t0 *= u0;
  t0 = ((t0 + ((t0 >> 8) & FOG_UINT64_C(0x000000FF00FF00FF)) + FOG_UINT64_C(0x0000008000800080)) >> 8) & FOG_UINT64_C(0x000000FF00FF00FF);

  dst0 = (uint32_t)_FOG_ACC_COMBINE_2(t0, t0 >> 24);
#else
  uint32_t t0 = (x0     ) & 0x00FF00FFU;
  uint32_t t1 = (x0 >> 8) & 0x000000FFU;

  t0 *= u0;
  t1 *= u0;

  t0 = ((t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;
  t1 = ((t1 + ((t1 >> 8)              ) + 0x00000080U)     ) & 0x0000FF00U;

  dst0 = _FOG_ACC_COMBINE_2(t0, t1);
#endif // FOG_ARCH_BITS
}

static FOG_INLINE void p32ZRGB32FromARGB32(
  uint32_t& dst0, const uint32_t& x0)
{
  uint32_t u0 = x0 >> 24;
  p32ZRGB32FromARGB32(dst0, x0, u0);
}

static FOG_INLINE void p32FRGB32FromARGB32(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& u0)
{
#if FOG_ARCH_BITS >= 64
  uint64_t t0 = (uint64_t)x0 | ((uint64_t)(x0) << 24);

  t0 &= FOG_UINT64_C(0x000000FF00FF00FF);
  t0 *= u0;
  t0 = ((t0 + ((t0 >> 8) & FOG_UINT64_C(0x000000FF00FF00FF)) + FOG_UINT64_C(0xFF00008000800080)) >> 8) & FOG_UINT64_C(0xFF00FF00FF00FF00);

  dst0 = (uint32_t)_FOG_ACC_COMBINE_2(t0 >> 8, t0 >> 32);
#else
  uint32_t t0 = (x0     ) & 0x00FF00FFU;
  uint32_t t1 = (x0 >> 8) & 0x000000FFU;

  t0 *= u0;
  t1 *= u0;

  t0 = ((t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;
  t1 = ((t1 + ((t1 >> 8)              ) + 0xFF000080U)     ) & 0xFF00FF00U;

  dst0 = _FOG_ACC_COMBINE_2(t0, t1);
#endif // FOG_ARCH_BITS
}

static FOG_INLINE void p32FRGB32FromARGB32(
  uint32_t& dst0, const uint32_t& x0)
{
  uint32_t u0 = x0 >> 24;
  p32FRGB32FromARGB32(dst0, x0, u0);
}

static FOG_INLINE void p32PRGB32FromARGB32_2031(
  uint32_t& dst0_20, const uint32_t& x0_20,
  uint32_t& dst0_31, const uint32_t& x0_31, const uint32_t& u0)
{
  uint32_t t0_20 = (x0_20             ) * u0;
  uint32_t t0_31 = (x0_31 | 0x00FF0000) * u0;

  dst0_20 = ((t0_20 + ((t0_20 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;
  dst0_31 = ((t0_31 + ((t0_31 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;
}

static FOG_INLINE void p32PRGB32FromARGB32_2031(
  uint32_t& dst0_20, const uint32_t& x0_20,
  uint32_t& dst0_31, const uint32_t& x0_31)
{
  uint32_t u0 = x0_31 >> 16;
  p32PRGB32FromARGB32_2031(dst0_20, x0_20, dst0_31, x0_31, u0);
}

static FOG_INLINE void p32PRGB32FromARGB32_1032(
  uint32_t& dst0_10, const uint32_t& x0_10,
  uint32_t& dst0_32, const uint32_t& x0_32, const uint32_t& u0)
{
  uint32_t t0_10 = (x0_10             ) * u0;
  uint32_t t0_32 = (x0_32 | 0x00FF0000) * u0;

  dst0_10 = ((t0_10 + ((t0_10 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;
  dst0_32 = ((t0_32 + ((t0_32 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;
}

static FOG_INLINE void p32PRGB32FromARGB32_1032(
  uint32_t& dst0_10, const uint32_t& x0_10,
  uint32_t& dst0_32, const uint32_t& x0_32)
{
  uint32_t u0 = x0_32 >> 16;
  p32PRGB32FromARGB32_2031(dst0_10, x0_10, dst0_32, x0_32, u0);
}

// ============================================================================
// [Fog::Acc - PRGB32 - From - ARGB64]
// ============================================================================

static FOG_INLINE void p32PRGB32FromARGB64_1032(
  uint32_t& dst0, const uint32_t& x0_10, const uint32_t& x0_32)
{
  uint32_t xa = x0_32 >> 16;

  uint32_t t0 = (x0_10 & 0xFFFF) * xa;
  uint32_t t1 = (x0_10    >> 16) * xa;
  uint32_t t2 = (x0_32 & 0xFFFF) * xa;

  t0 = (t0 + (t0 >> 16) + 0x8000U);
  t1 = (t1 + (t1 >> 16) + 0x8000U);
  t2 = (t2 + (t2 >> 16) + 0x8000U);

  dst0 = _FOG_ACC_COMBINE_3((_FOG_ACC_COMBINE_2((xa << 16), (t1 >> 16)) & 0xFF00FF00U), (t2 >>  8) & 0x00FF0000U, (t0 >> 24));
}

// ============================================================================
// [Fog::Acc - PRGB32 - From - PRGB64]
// ============================================================================

static FOG_INLINE void p32PRGB32FromPRGB64_1032(
  uint32_t& dst0, const uint32_t& x0_10, const uint32_t& x0_32)
{
  p32ARGB32FromARGB64_1032(dst0, x0_10, x0_32);
}

static FOG_INLINE void p64PRGB32FromPRGB64(
  uint32_t& dst0, const __p64& x0)
{
  p64ARGB32FromARGB64(dst0, x0);
}

// ============================================================================
// [Fog::Acc - RGB24 - From - XRGB32]
// ============================================================================

static FOG_INLINE void p32RGB24QuadFromXRGB32Solid(
  uint32_t& dst0, uint32_t& dst1, uint32_t& dst2,
  const uint32_t& x0)
{
  uint32_t t0 = x0 & 0x00FFFFFFU;

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN

  // [    ][    ][BGR0] - Memory
  // [    ][    ][0RGB] - CPU

  // [BGRB][GRBG][RBGR] - Memory
  // [BRGB][GBRG][RGBR] - CPU

  dst0 = _FOG_ACC_COMBINE_2(t0      , t0 << 24);
  dst1 = _FOG_ACC_COMBINE_2(t0 << 16, t0 >>  8);
  dst2 = _FOG_ACC_COMBINE_2(t0 <<  8, t0 >> 16);

#else

  // [    ][    ][0RGB] - Memory
  // [    ][    ][0RGB] - CPU

  // [RGBR][GBRG][BRGB] - Memory
  // [RGBR][GBRG][BRGB] - CPU

  dst0 = _FOG_ACC_COMBINE_2(t0 <<  8, t0 >> 16);
  dst1 = _FOG_ACC_COMBINE_2(t0 << 16, t0 >>  8);
  dst2 = _FOG_ACC_COMBINE_2(t0      , t0 << 24);

#endif // FOG_BYTE_ORDER
}

static FOG_INLINE void p32RGB24QuadFromXRGB32Quad(
  uint32_t& dst0, uint32_t& dst1, uint32_t& dst2,
  const uint32_t& x0, const uint32_t& x1, const uint32_t& x2, const uint32_t& x3)
{
  uint32_t t0 = x0;
  uint32_t t1 = x1;
  uint32_t t2 = x2;
  uint32_t t3 = x3;

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  // [B0 G0 R0 X0] [B1 G1 R1 X1] [B2 G2 R2 X2] [B3 G3 R3 X3] - Memory
  // [X0 R0 G0 B0] [X1 R1 G1 B1] [X2 R2 G2 B2] [X3 R3 G3 B3] - CPU

  // [B0 G0 R0 B1] [G1 R1 B2 G2] [R2 B3 G3 R3]               - Memory
  // [B1 R0 G0 B0] [G2 B2 R1 G1] [R3 G3 B3 R2]               - CPU

  dst0 = _FOG_ACC_COMBINE_2((t0      ) & 0x00FFFFFFU, t1 << 24);
  dst1 = _FOG_ACC_COMBINE_2((t1 >>  8) & 0x0000FFFFU, t2 << 16);
  dst2 = _FOG_ACC_COMBINE_2((t2 >> 16) & 0x000000FFU, t3 <<  8);
#else
  // [X0 R0 G0 B0] [X1 R1 G1 B1] [X2 R2 G2 B2] [X3 R3 G3 B3] - Memory
  // [X0 R0 G0 B0] [X1 R1 G1 B1] [X2 R2 G2 B2] [X3 R3 G3 B3] - CPU

  // [R0 G0 B0 R1] [G1 B1 R2 G2] [B2 R3 G3 B3]               - Memory
  // [R0 G0 B0 R1] [G1 B1 R2 G2] [B2 R3 G3 B3]               - CPU

  dst0 = _FOG_ACC_COMBINE_2((t0 <<  8), (t1 >> 16) & 0x000000FFU);
  dst1 = _FOG_ACC_COMBINE_2((t1 << 16), (t2 >>  8) & 0x0000FFFFU);
  dst2 = _FOG_ACC_COMBINE_2((t2 << 24), (t3      ) & 0x00FFFFFFU);
#endif // FOG_BYTE_ORDER
}

// ============================================================================
// [Fog::Acc - RGB24 - Lerp]
// ============================================================================

template<int M_INDEX>
static FOG_INLINE uint32_t p32ARGB32ExtractMaskT(const uint32_t& msk0_0, const uint32_t& msk0_1)
{
  if (M_INDEX == 0)
    return msk0_0 & 0xFFFF;
  else if (M_INDEX == 1)
    return msk0_0 >> 16;
  else if (M_INDEX == 2)
    return msk0_1 & 0xFFFF;
  else
    return msk0_1 >> 16;
}

template<int M_0, int M_1, int M_2, int M_3>
static FOG_INLINE void p32RGB24LerpTriadSolid_PBW256_T(
  uint32_t& dst0, uint32_t& dst1, uint32_t& dst2,
  const uint32_t& x0, const uint32_t& x1, const uint32_t& x2,
  const uint32_t& y0, const uint32_t& msk0_0, const uint32_t& msk0_1)
{
  uint32_t m0;

  uint32_t xx;
  uint32_t yy;

  uint32_t t0;
  uint32_t t1;
  uint32_t t2;
  uint32_t t3;
  uint32_t t4;

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN

  // [B0 G0 R0 B1] [G1 R1 B2 G2] [R2 B3 G3 R3]               - Pixels (Memory)
  // [B1 R0 G0 B0] [G2 B2 R1 G1] [R3 G3 B3 R2]               - Pixels (CPU)

  // [yy yy xx xx] [ww ww zz zz]                             - Mask (Memory)
  // [11 00 00 00] [22 22 11 11] [33 33 33 22]               - Mask (CPU)

  xx = x0;
  yy = y0;

  m0 = p32ARGB32ExtractMaskT<M_0>(msk0_0, msk0_1);
  t0 = (yy & 0x00FF00FFU) * m0;
  t1 = (yy & 0x0000FF00U) * m0;

  m0 = 256 - m0;
  t0 = (t0 + ((xx & 0x00FF00FFU) * m0)) & 0xFF00FF00U;
  t1 = (t1 + ((xx & 0x0000FF00U) * m0)) & 0x00FF0000U;
  t0 = _FOG_ACC_COMBINE_2(t0, t1) >> 8;

  m0 = p32ARGB32ExtractMaskT<M_1>(msk0_0, msk0_1);
  xx = _FOG_ACC_COMBINE_2(xx >> 24, x1 << 8);
  t1 = (yy & 0x00FF00FFU) * m0;
  t2 = (yy & 0x0000FF00U) * m0;

  m0 = 256 - m0;
  t1 = (t1 + ((xx & 0x00FF00FFU) * m0)) & 0xFF00FF00U;
  t2 = (t2 + ((xx & 0x0000FF00U) * m0)) & 0x00FF0000U;
  t1 = _FOG_ACC_COMBINE_2(t1, t2) >> 8;

  m0 = p32ARGB32ExtractMaskT<M_2>(msk0_0, msk0_1);
  xx = _FOG_ACC_COMBINE_2((x1 >> 16), (x2 >> 8) & 0x00FF0000);
  t2 = (yy & 0x00FF00FFU) * m0;
  t3 = (yy & 0x0000FF00U) * m0;

  m0 = 256 - m0;
  t2 = (t2 + ((xx & 0x00FF00FFU) * m0)) & 0xFF00FF00U;
  t3 = (t3 + ((xx & 0x0000FF00U) * m0)) & 0x00FF0000U;
  t2 = _FOG_ACC_COMBINE_2(t2, t3) >> 8;

  m0 = p32ARGB32ExtractMaskT<M_3>(msk0_0, msk0_1);
  xx = x2 >> 8;
  t3 = (yy & 0x00FF00FFU) * m0;
  t4 = (yy & 0x0000FF00U) * m0;

  m0 = 256 - m0;
  t3 = (t3 + ((xx & 0x00FF00FFU) * m0)) & 0xFF00FF00U;
  t4 = (t4 + ((xx & 0x0000FF00U) * m0)) & 0x00FF0000U;
  t3 = _FOG_ACC_COMBINE_2(t3, t4) >> 8;

  dst0 = _FOG_ACC_COMBINE_2(t0      , t1 << 24);
  dst1 = _FOG_ACC_COMBINE_2(t1 >>  8, t2 << 16);
  dst2 = _FOG_ACC_COMBINE_2(t2 >> 16, t3 <<  8);

#else

  // [R0 G0 B0 R1] [G1 B1 R2 G2] [B2 R3 G3 B3]               - Memory
  // [R0 G0 B0 R1] [G1 B1 R2 G2] [B2 R3 G3 B3]               - CPU

  // [00 11 22 33]                                           - Mask
  // [00 00 00 11] [11 11 22 22] [22 33 33 33]               - Expanded

  // TODO: Complete after LITTLE ENDIAN version is tested.

#endif // FOG_BYTE_ORDER
}

static FOG_INLINE void p32RGB24LerpTriadSolid_PBW256_1032(
  uint32_t& dst0, uint32_t& dst1, uint32_t& dst2,
  const uint32_t& x0, const uint32_t& x1, const uint32_t& x2,
  const uint32_t& y0, const uint32_t& msk0_10, const uint32_t& msk0_32)
{
  p32RGB24LerpTriadSolid_PBW256_T<0, 1, 2, 3>(
    dst0, dst1, dst2, x0, x1, x2, y0, msk0_10, msk0_32);
}

static FOG_INLINE void p32RGB24LerpTriadSolid_PBW256_0123(
  uint32_t& dst0, uint32_t& dst1, uint32_t& dst2,
  const uint32_t& x0, const uint32_t& x1, const uint32_t& x2,
  const uint32_t& y0, const uint32_t& msk0_10, const uint32_t& msk0_32)
{
  p32RGB24LerpTriadSolid_PBW256_T<1, 0, 3, 2>(
    dst0, dst1, dst2, x0, x1, x2, y0, msk0_10, msk0_32);
}

static FOG_INLINE void p32RGB24LerpTriadSolid_PBW256_2031(
  uint32_t& dst0, uint32_t& dst1, uint32_t& dst2,
  const uint32_t& x0, const uint32_t& x1, const uint32_t& x2,
  const uint32_t& y0, const uint32_t& msk0_20, const uint32_t& msk0_31)
{
  p32RGB24LerpTriadSolid_PBW256_T<0, 2, 1, 3>(
    dst0, dst1, dst2, x0, x1, x2, y0, msk0_20, msk0_31);
}

static FOG_INLINE void p32RGB24LerpTriadSolid_PBW256_1302(
  uint32_t& dst0, uint32_t& dst1, uint32_t& dst2,
  const uint32_t& x0, const uint32_t& x1, const uint32_t& x2,
  const uint32_t& y0, const uint32_t& msk0_20, const uint32_t& msk0_31)
{
  p32RGB24LerpTriadSolid_PBW256_T<3, 1, 2, 0>(
    dst0, dst1, dst2, x0, x1, x2, y0, msk0_20, msk0_31);
}

static FOG_INLINE void p32RGB24LerpTriadSolid_PBW256_PMA8(
  uint32_t& dst0, uint32_t& dst1, uint32_t& dst2,
  const uint32_t& x0, const uint32_t& x1, const uint32_t& x2,
  const uint32_t& y0, const uint32_t& msk0_4a, const uint32_t& msk0_4b)
{
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  // [33221100] -> [__22__00] + [__33__11]
  p32RGB24LerpTriadSolid_PBW256_2031(dst0, dst1, dst2, x0, x1, x2, y0, msk0_4a, msk0_4b);
#else
  // [00112233] -> [__11__33] + [__00__22]
  p32RGB24LerpTriadSolid_PBW256_1302(dst0, dst1, dst2, x0, x1, x2, y0, msk0_4a, msk0_4b);
#endif // FOG_BYTE_ORDER
}

static FOG_INLINE void p32RGB24LerpTriadSolid_PBW256_PMAE(
  uint32_t& dst0, uint32_t& dst1, uint32_t& dst2,
  const uint32_t& x0, const uint32_t& x1, const uint32_t& x2,
  const uint32_t& y0, const uint32_t& msk0_4a, const uint32_t& msk0_4b)
{
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  // [11110000] + [33332222] -> [__11__00] + [__33__22]
  p32RGB24LerpTriadSolid_PBW256_1032(dst0, dst1, dst2, x0, x1, x2, y0, msk0_4a, msk0_4b);
#else
  // [00001111] + [22223333] -> [__00__11] + [__22__33]
  p32RGB24LerpTriadSolid_PBW256_0123(dst0, dst1, dst2, x0, x1, x2, y0, msk0_4a, msk0_4b);
#endif // FOG_BYTE_ORDER
}

// ============================================================================
// [Fog::Acc - RGB16_555 - From - XRGB32]
// ============================================================================

static FOG_INLINE void p32RGB16_555FromXRGB32(uint32_t& dst0, const uint32_t& x0)
{
  //                                 // [XXXXXXXX] [RRRRRRRR] [GGGGGGGG] [BBBBBBBB]
  uint32_t t0;
  uint32_t t1;
  uint32_t t2;

  t0 = (x0 >> 9) & 0x00007C00U;      // [00000000] [00000000] [0RRRRR00] [00000000]
  t1 = (x0 >> 6) & 0x000003E0U;      // [00000000] [00000000] [000000GG] [GGG00000]
  t2 = (x0 >> 3) & 0x0000001FU;      // [00000000] [00000000] [00000000] [000BBBBB]

  dst0 = _FOG_ACC_COMBINE_3(t0, t1, t2);
}

// ============================================================================
// [Fog::Acc - RGB16_555 - MulDiv]
// ============================================================================

static FOG_INLINE void p32RGB16_000_555_MulDiv_SBW_5b(uint32_t& dst0, const uint32_t& x0, const uint32_t& u0_5b)
{
  uint32_t t0 = ((x0 | (x0 << 16)) & 0x03E07C1FU) * u0_5b;
  uint32_t t1 = t0;

  t0 >>= 5;
  t1 >>= 21;

  dst0 = _FOG_ACC_COMBINE_2(t0 & 0x00007C1FU, t1 & 0x000003E0U);
}

static FOG_INLINE void p32RGB16_000_555_MulDiv_SBW_8b(uint32_t& dst0, const uint32_t& x0, const uint32_t& u0_8b)
{
  uint32_t u0_5b;

  p32RShift(u0_5b, u0_8b, 3);
  p32RGB16_000_555_MulDiv_SBW_5b(dst0, x0, u0_5b);
}

static FOG_INLINE void p32RGB16_555_555_MulDiv_SBW_5b(uint32_t& dst0, const uint32_t& x0, const uint32_t& u0_5b)
{
  uint32_t t0 = (x0 & 0x03E07C1FU);
  uint32_t t1 = (x0 & 0x7C1F03E0U) >> 5;

  t0 = ((t0 * u0_5b) >> 5) & 0x03E07C1FU;
  t1 = ((t1 * u0_5b)     ) & 0x7C1F03E0U;

  dst0 = _FOG_ACC_COMBINE_2(t0, t1);
}

static FOG_INLINE void p32RGB16_555_555_MulDiv_SBW_8b(uint32_t& dst0, const uint32_t& x0, const uint32_t& u0_8b)
{
  uint32_t u0_5b;

  p32RShift(u0_5b, u0_8b, 3);
  p32RGB16_555_555_MulDiv_SBW_5b(dst0, x0, u0_5b);
}

static FOG_INLINE void p32RGB16_000_555_MulDiv_PBW_1032(uint32_t& dst0, const uint32_t& x0, const uint32_t& y0_10, const uint32_t& y0_32)
{
  uint32_t t0 = (x0 & 0x0000001FU) * (y0_10         );
  uint32_t t1 = (x0 & 0x000003E0U) * (y0_10 >> 16   );
  uint32_t t2 = (x0 & 0x00007C00U) * (y0_32 & 0xFFFF);

  t0 &= 0x0000001FU << 8;
  t1 &= 0x000003E0U << 8;
  t2 &= 0x00007C00U << 8;

  dst0 = _FOG_ACC_COMBINE_3(t0, t1, t2) >> 8;
}

static FOG_INLINE void p32RGB16_000_555_MulDiv_PBW_2031(uint32_t& dst0, const uint32_t& x0, const uint32_t& y0_20, const uint32_t& y0_31)
{
  uint32_t t0 = (x0 & 0x0000001FU) * (y0_20         );
  uint32_t t1 = (x0 & 0x000003E0U) * (y0_31 & 0xFFFF);
  uint32_t t2 = (x0 & 0x00007C00U) * (y0_20 >> 16   );

  t0 &= 0x0000001FU << 8;
  t1 &= 0x000003E0U << 8;
  t2 &= 0x00007C00U << 8;

  dst0 = _FOG_ACC_COMBINE_3(t0, t1, t2) >> 8;
}

// ============================================================================
// [Fog::Acc - RGB16_565 - From - XRGB32]
// ============================================================================

static FOG_INLINE void p32RGB16_565FromXRGB32(uint32_t& dst0, const uint32_t& x0)
{
  //                                 // [XXXXXXXX] [RRRRRRRR] [GGGGGGGG] [BBBBBBBB]
  uint32_t t0;
  uint32_t t1;
  uint32_t t2;

  t0 = (x0 >> 8) & 0x0000F800U;      // [00000000] [00000000] [RRRRR000] [00000000]
  t1 = (x0 >> 5) & 0x000007E0U;      // [00000000] [00000000] [00000GGG] [GGG00000]
  t2 = (x0 >> 3) & 0x0000001FU;      // [00000000] [00000000] [00000000] [000BBBBB]

  dst0 = _FOG_ACC_COMBINE_3(t0, t1, t2);
}

// ============================================================================
// [Fog::Acc - RGB16_565 - MulDiv]
// ============================================================================

static FOG_INLINE void p32RGB16_000_565_MulDiv_SBW_6b(uint32_t& dst0, const uint32_t& x0, const uint32_t& u0_6b)
{
  uint32_t t0 = x0 & 0x0000F81FU;         // 0000000000000000 1111100000011111.
  uint32_t t1 = x0 & 0x000007E0U;         // 0000000000000000 0000011111100000.

  t0 = ((t0 * u0_6b) >> 6) & 0x0000F81FU; // 0000000000000000 1111100000011111.
  t1 = ((t1 * u0_6b) >> 6) & 0x000007E0U; // 0000000000000000 0000011111100000.

  dst0 = _FOG_ACC_COMBINE_2(t0, t1);
}

static FOG_INLINE void p32RGB16_000_565_MulDiv_SBW_8b(uint32_t& dst0, const uint32_t& x0, const uint32_t& u0_8b)
{
  uint32_t u0_6b;

  p32RShift(u0_6b, u0_8b, 2);
  p32RGB16_000_565_MulDiv_SBW_6b(dst0, x0, u0_6b);
}

static FOG_INLINE void p32RGB16_565_565_MulDiv_SBW_6b(uint32_t& dst0, const uint32_t& x0, const uint32_t& u0_6b)
{
  uint32_t t0 = (x0 & 0x001F001FU);       // 0000000000011111 0000000000011111.
  uint32_t t1 = (x0 & 0x07E007E0U) >> 5;  // 0000011111100000 0000011111100000.
  uint32_t t2 = (x0 & 0xF800F800U) >> 6;  // 1111100000000000 1111100000000000.

  t0 = ((t0 * u0_6b) >> 6) & 0x001F001FU; // 0000000000011111 0000000000011111.
  t1 = ((t1 * u0_6b) >> 1) & 0x07E007E0U; // 0000011111100000 0000011111100000.
  t2 = ((t2 * u0_6b)     ) & 0xF800F800U; // 1111100000000000 1111100000000000.

  dst0 = _FOG_ACC_COMBINE_3(t0, t1, t2);
}

static FOG_INLINE void p32RGB16_565_565_MulDiv_SBW_8b(uint32_t& dst0, const uint32_t& x0, const uint32_t& u0_8b)
{
  uint32_t u0_6b;

  p32RShift(u0_6b, u0_8b, 2);
  p32RGB16_565_565_MulDiv_SBW_6b(dst0, x0, u0_6b);
}

static FOG_INLINE void p32RGB16_000_565_MulDiv_PBW_1032(uint32_t& dst0, const uint32_t& x0, const uint32_t& y0_10, const uint32_t& y0_32)
{
  uint32_t t0 = (x0 & 0x0000001FU) * (y0_10         );
  uint32_t t1 = (x0 & 0x000007E0U) * (y0_10 >> 16   );
  uint32_t t2 = (x0 & 0x0000F800U) * (y0_32 & 0xFFFF);

  t0 &= 0x0000001FU << 8;
  t1 &= 0x000007E0U << 8;
  t2 &= 0x0000F800U << 8;

  dst0 = _FOG_ACC_COMBINE_3(t0, t1, t2) >> 8;
}

static FOG_INLINE void p32RGB16_000_565_MulDiv_PBW_2031(uint32_t& dst0, const uint32_t& x0, const uint32_t& y0_20, const uint32_t& y0_31)
{
  uint32_t t0 = (x0 & 0x0000001FU) * (y0_20         );
  uint32_t t1 = (x0 & 0x000007E0U) * (y0_31 & 0xFFFF);
  uint32_t t2 = (x0 & 0x0000F800U) * (y0_20 >> 16   );

  t0 &= 0x0000001FU << 8;
  t1 &= 0x000003E0U << 8;
  t2 &= 0x00007C00U << 8;

  dst0 = _FOG_ACC_COMBINE_3(t0, t1, t2) >> 8;
}

// ============================================================================
// [Fog::Acc - ARGB16_4444 - From - ARGB32]
// ============================================================================

static FOG_INLINE void p32ARGB16_4444FromARGB32(uint32_t& dst0, const uint32_t& x0)
{
  //                                 // [AAAAAAAA] [RRRRRRRR] [GGGGGGGG] [BBBBBBBB]

  uint32_t t0 = (x0 >>  4) & 0x000FU;// [00000000] [00000000] [00000000] [0000BBBB]
  uint32_t t1 = (x0 >>  8) & 0x00F0U;// [00000000] [00000000] [00000000] [GGGG0000]
  uint32_t t2 = (x0 >> 12) & 0x0F00U;// [00000000] [00000000] [0000RRRR] [00000000]
  uint32_t t3 = (x0 >> 16) & 0xF000U;// [00000000] [00000000] [AAAA0000] [00000000]

  dst0 = _FOG_ACC_COMBINE_4(t0, t1, t2, t3);
}

static FOG_INLINE void p32ARGB16_4444_bsFromARGB32(uint32_t& dst0, const uint32_t& x0)
{
  //                                 // [AAAAAAAA] [RRRRRRRR] [GGGGGGGG] [BBBBBBBB]

  uint32_t t0 = (x0 <<  8) & 0xF000U;// [00000000] [00000000] [BBBB0000] [00000000]
  uint32_t t1 = (x0 >>  4) & 0x0F00U;// [00000000] [00000000] [0000GGGG] [00000000]
  uint32_t t2 = (x0 >> 16) & 0x00F0U;// [00000000] [00000000] [00000000] [RRRR0000]
  uint32_t t3 = (x0 >> 28) & 0x000FU;// [00000000] [00000000] [00000000] [0000AAAA]

  dst0 = _FOG_ACC_COMBINE_4(t0, t1, t2, t3);
}

// ============================================================================
// [Fog::Acc - RGB48 - Load / Store]
// ============================================================================

static FOG_INLINE void p32RGB48Load(
  uint32_t& dr0, uint32_t& dg0, uint32_t& db0, const void* srcp)
{
  const uint8_t* src = reinterpret_cast<const uint8_t*>(srcp);

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  p32Load2a(db0, src + 0);
  p32Load2a(dg0, src + 2);
  p32Load2a(dr0, src + 4);
#else
  p32Load2a(dr0, src + 0);
  p32Load2a(dg0, src + 2);
  p32Load2a(db0, src + 4);
#endif
}

static FOG_INLINE void p32RGB48LoadToZRGB32(
  uint32_t& dst0, const void* srcp)
{
  uint32_t dr0;
  uint32_t dg0;
  uint32_t db0;

  p32RGB48Load(dr0, dg0, db0, srcp);
  p32ZRGB32FromRGB48(dst0, dr0, dg0, db0);
}

static FOG_INLINE void p32RGB48LoadToFRGB32(
  uint32_t& dst0, const void* srcp)
{
  uint32_t dr0;
  uint32_t dg0;
  uint32_t db0;

  p32RGB48Load(dr0, dg0, db0, srcp);
  p32FRGB32FromRGB48(dst0, dr0, dg0, db0);
}

static FOG_INLINE void p32RGB48Store(
  void* dstp, const uint32_t& sr0, const uint32_t& sg0, const uint32_t& sb0)
{
  uint8_t* dst = reinterpret_cast<uint8_t*>(dstp);

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  p32Store2a(dst + 0, sb0);
  p32Store2a(dst + 2, sg0);
  p32Store2a(dst + 4, sr0);
#else
  p32Store2a(dst + 0, sr0);
  p32Store2a(dst + 2, sg0);
  p32Store2a(dst + 4, sb0);
#endif
}

static FOG_INLINE void p32RGB48StoreFromARGB32(
  void* dstp, const uint32_t& src0)
{
  uint8_t* dst = reinterpret_cast<uint8_t*>(dstp);

  uint32_t a0;
  uint32_t rb;
  uint32_t gg;

  p32ExtractPBB3(a0, src0);
  p32Cvt256SBWFrom255SBW(a0, a0);

  rb = ((src0 & 0x00FF00FFU) * a0);
  gg = ((src0 & 0x0000FF00U) * a0) >> 8;

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  p32Store2a(dst + 0, rb);
  p32Store2a(dst + 2, gg);
  p32Store2a(dst + 4, rb >> 16);
#else
  p32Store2a(dst + 0, rb >> 16);
  p32Store2a(dst + 2, gg);
  p32Store2a(dst + 4, rb);
#endif
}

static FOG_INLINE void p32RGB48StoreBSwapFromARGB32(
  void* dstp, const uint32_t& src0)
{
  uint8_t* dst = reinterpret_cast<uint8_t*>(dstp);

  uint32_t a0;
  uint32_t rb;
  uint32_t gg;

  p32ExtractPBB3(a0, src0);
  p32Cvt256SBWFrom255SBW(a0, a0);

  rb = ((src0 & 0x00FF00FFU) * a0);
  gg = ((src0 & 0x0000FF00U) * a0) >> 8;

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  p32Store2a(dst + 0, rb >> 16);
  p32Store2a(dst + 2, gg);
  p32Store2a(dst + 4, rb);
#else
  p32Store2a(dst + 0, rb);
  p32Store2a(dst + 2, gg);
  p32Store2a(dst + 4, rb >> 16);
#endif
}

static FOG_INLINE void p32RGB48StoreFromXRGB32(
  void* dstp, const uint32_t& src0)
{
  uint8_t* dst = reinterpret_cast<uint8_t*>(dstp);

  uint32_t rb = ((src0 & 0x00FF00FFU) * 0x00000101U);
  uint32_t gg = ((src0 & 0x0000FF00U) * 0x00000101U) >> 8;

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  p32Store2a(dst + 0, rb);
  p32Store2a(dst + 2, gg);
  p32Store2a(dst + 4, rb >> 16);
#else
  p32Store2a(dst + 0, rb >> 16);
  p32Store2a(dst + 2, gg);
  p32Store2a(dst + 4, rb);
#endif
}

static FOG_INLINE void p32RGB48StoreBSwapFromXRGB32(
  void* dstp, const uint32_t& src0)
{
  uint8_t* dst = reinterpret_cast<uint8_t*>(dstp);

  uint32_t rb = ((src0 & 0x00FF00FFU) * 0x00000101U);
  uint32_t gg = ((src0 & 0x0000FF00U) * 0x00000101U) >> 8;

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  p32Store2a(dst + 0, rb >> 16);
  p32Store2a(dst + 2, gg);
  p32Store2a(dst + 4, rb);
#else
  p32Store2a(dst + 0, rb);
  p32Store2a(dst + 2, gg);
  p32Store2a(dst + 4, rb >> 16);
#endif
}
// ============================================================================
// [Fog::Acc - XRGB64 - From - RGB16_555]
// ============================================================================

static FOG_INLINE void p32ZRGB64FromRGB16_555(
  uint32_t& dst0_10, uint32_t& dst0_32, const uint32_t& x0)
{
  //                                      // [XXXXXXXX] [XXXXXXXX] [XRRRRRGG] [GGGBBBBB]
  uint32_t t0 = x0 & 0x001F;              // [00000000] [00000000] [00000000] [000BBBBB]
  uint32_t t1 = x0 & 0x03E0;              // [00000000] [00000000] [000000GG] [GGG00000]
  uint32_t t2 = x0 & 0x7C00;              // [00000000] [00000000] [0RRRRR00] [00000000]

  t0 = (t0 * (0x8421));                   // [00000000] [0000BBBB] [BBBBBBBB] [BBBBBBBB]
  t1 = (t1 * (0x8421 << 7));              // [GGGGGGGG] [GGGGGGGG] [GGGG0000] [00000000]
  t2 = (t2 * (0x8421));                   // [00RRRRRR] [RRRRRRRR] [RRRRRR00] [00000000]

  t0 = (t0 >>  4);                        // [00000000] [00000000] [BBBBBBBB] [BBBBBBBB]
  t1 = (t1 & 0xFFFF0000);                 // [GGGGGGGG] [GGGGGGGG] [00000000] [00000000]
  t2 = (t2 >> 14);                        // [00000000] [00000000] [RRRRRRRR] [RRRRRRRR]

  dst0_10 = _FOG_ACC_COMBINE_2(t0, t1);   // [GGGGGGGG] [GGGGGGGG] [BBBBBBBB] [BBBBBBBB]
  dst0_32 = t2;                           // [00000000] [00000000] [RRRRRRRR] [RRRRRRRR]
}

static FOG_INLINE void p32FRGB64FromRGB16_555(
  uint32_t& dst0_10, uint32_t& dst0_32, const uint32_t& x0)
{
  p32ZRGB64FromRGB16_555(dst0_10, dst0_32, x0);
  p32FillPWW1(dst0_32, dst0_32);
}

// ============================================================================
// [Fog::Acc - XRGB64 - From - RGB16_565]
// ============================================================================

static FOG_INLINE void p32ZRGB64FromRGB16_565(
  uint32_t& dst0_10, uint32_t& dst0_32, const uint32_t& x0)
{
  //                                      // [XXXXXXXX] [XXXXXXXX] [RRRRRGGG] [GGGBBBBB]
  uint32_t t0 = x0 & 0x001F;              // [00000000] [00000000] [00000000] [000BBBBB]
  uint32_t t1 = x0 & 0x07E0;              // [00000000] [00000000] [00000GGG] [GGG00000]
  uint32_t t2 = x0 & 0xF800;              // [00000000] [00000000] [RRRRR000] [00000000]

  t0 = (t0 * (0x8421));                   // [00000000] [0000BBBB] [BBBBBBBB] [BBBBBBBB]
  t1 = (t1 * (0x1041 << 9));              // [GGGGGGGG] [GGGGGGGG] [GG000000] [00000000]
  t2 = (t2 * (0x8421));                   // [0RRRRRRR] [RRRRRRRR] [RRRRR000] [00000000]

  t0 = (t0 >>  4);                        // [00000000] [00000000] [BBBBBBBB] [BBBBBBBB]
  t1 = (t1 & 0xFFFF0000);                 // [GGGGGGGG] [GGGGGGGG] [00000000] [00000000]
  t2 = (t2 >> 15);                        // [00000000] [00000000] [RRRRRRRR] [RRRRRRRR]

  dst0_10 = _FOG_ACC_COMBINE_2(t0, t1);   // [GGGGGGGG] [GGGGGGGG] [BBBBBBBB] [BBBBBBBB]
  dst0_32 = t2;                           // [00000000] [00000000] [RRRRRRRR] [RRRRRRRR]
}

static FOG_INLINE void p32FRGB64FromRGB16_565(
  uint32_t& dst0_10, uint32_t& dst0_32, const uint32_t& x0)
{
  p32ZRGB64FromRGB16_565(dst0_10, dst0_32, x0);
  p32FillPWW1(dst0_32, dst0_32);
}

// ============================================================================
// [Fog::Acc - XRGB64 - From - ZRGB32]
// ============================================================================

static FOG_INLINE void p32ZRGB64FromZRGB32(
  uint32_t& dst0_10, uint32_t& dst0_32, const uint32_t& x0)
{
  uint32_t t0 = (x0      ) & 0xFF; // [000B]
  uint32_t t1 = (x0 >> 16)       ; // [000R]
  FOG_ASSERT(t1 <= 0xFF);

  dst0_10 = _FOG_ACC_COMBINE_2(t0, (x0 << 8) & 0x00FF0000U);
  dst0_32 = t1;

  dst0_10 = dst0_10 * 0x101;
  dst0_32 = dst0_32 * 0x101;
}

static FOG_INLINE void p32FRGB64FromZRGB32(
  uint32_t& dst0_10, uint32_t& dst0_32, const uint32_t& x0)
{
  p32ZRGB64FromZRGB32(dst0_10, dst0_32, x0);
  p32FillPWW1(dst0_32, dst0_32);
}

// ============================================================================
// [Fog::Acc - XRGB64 - From - XRGB32]
// ============================================================================

static FOG_INLINE void p32ZRGB64FromXRGB32(
  uint32_t& dst0_10, uint32_t& dst0_32, const uint32_t& x0)
{
  uint32_t t0 = (x0      ) & 0xFF; // [000B]
  uint32_t t1 = (x0 >> 16) & 0xFF; // [000R]

  dst0_10 = _FOG_ACC_COMBINE_2(t0, (x0 << 8) & 0x00FF0000U);
  dst0_32 = t1;

  dst0_10 = dst0_10 * 0x101;
  dst0_32 = dst0_32 * 0x101;
}

static FOG_INLINE void p32FRGB64FromXRGB32(
  uint32_t& dst0_10, uint32_t& dst0_32, const uint32_t& x0)
{
  p32ZRGB64FromXRGB32(dst0_10, dst0_32, x0);
  p32FillPWW1(dst0_32, dst0_32);
}

// ============================================================================
// [Fog::Acc - XRGB64 - From - ARGB64]
// ============================================================================

static FOG_INLINE void p32ZRGB64FromARGB64_1032(
  uint32_t& dst0_10, uint32_t& dst0_32, const uint32_t& x0_10, const uint32_t& x0_32)
{
  uint32_t xa = x0_32 >> 16;

  uint32_t t0 = (x0_10 & 0xFFFF) * xa;
  uint32_t t1 = (x0_10    >> 16) * xa;
  uint32_t t2 = (x0_32 & 0xFFFF) * xa;

  t0 = (t0 + (t0 >> 16) + 0x8000U);
  t1 = (t1 + (t1 >> 16) + 0x8000U);
  t2 = (t2 + (t2 >> 16) + 0x8000U);

  dst0_10 = _FOG_ACC_COMBINE_2(t0 >> 16, t1 & 0xFFFF0000);
  dst0_32 = t2 >> 16;
}

static FOG_INLINE void p32ZRGB64FromARGB64_2031(
  uint32_t& dst0_20, uint32_t& dst0_31, const uint32_t& x0_20, const uint32_t& x0_31)
{
  uint32_t xa = x0_31 >> 16;

  uint32_t t0 = (x0_20 & 0xFFFF) * xa;
  uint32_t t1 = (x0_20    >> 16) * xa;
  uint32_t t2 = (x0_31 & 0xFFFF) * xa;

  t0 = (t0 + (t0 >> 16) + 0x8000U);
  t1 = (t1 + (t1 >> 16) + 0x8000U);
  t2 = (t2 + (t2 >> 16) + 0x8000U);

  dst0_20 = _FOG_ACC_COMBINE_2(t0 >> 16, t1 & 0xFFFF0000);
  dst0_31 = t2 >> 16;
}

// ============================================================================
// [Fog::Acc - ARGB64 - IsAlphaXX]
// ============================================================================

//! @brief Get whether the ARGB64 pixel @a c0 has alpha set to 0xFFFF (65535 in decimal).
static FOG_INLINE bool p32ARGB64IsAlphaFFFF_2031(const uint32_t& c0_20, const uint32_t& c0_31)
{
  return c0_31 >= 0xFFFF0000U;
}

//! @brief Get whether the ARGB64 pixel @a c0 has alpha set to 0x0000 (0 in decimal).
static FOG_INLINE bool p32ARGB64IsAlpha0000_2031(const uint32_t& c0_20, const uint32_t& c0_31)
{
  return c0_31 <= 0x0000FFFFU;
}

//! @brief Get whether the ARGB64 pixel @a c0 has alpha set to 0xFFFF (65535 in decimal).
static FOG_INLINE bool p32ARGB64IsAlphaFFFF_1032(const uint32_t& c0_10, const uint32_t& c0_32)
{
  return c0_32 >= 0xFFFF0000U;
}

//! @brief Get whether the ARGB64 pixel @a c0 has alpha set to 0x0000 (0 in decimal).
static FOG_INLINE bool p32ARGB64IsAlpha0000_1032(const uint32_t& c0_10, const uint32_t& c0_32)
{
  return c0_32 <= 0x0000FFFFU;
}

//! @brief Get whether the ARGB64 pixel @a c0 has alpha set to 0xFFFF (65535 in decimal).
static FOG_INLINE bool p64ARGB64IsAlphaFFFF(const __p64& c0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  return c0 >= FOG_UINT64_C(0xFFFF000000000000);
#else
  return c0.u32Hi >= 0xFFFF0000U;
#endif // FOG_ARCH_NATIVE_P64
}

//! @brief Get whether the ARGB64 pixel @a c0 has alpha set to 0x0000 (0 in decimal).
static FOG_INLINE bool p64ARGB64IsAlpha0000(const __p64& c0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  return c0 <= FOG_UINT64_C(0x0000FFFFFFFFFFFF);
#else
  return c0.u32Hi <= 0x0000FFFFFU;
#endif // FOG_ARCH_NATIVE_P64
}

// ============================================================================
// [Fog::Acc - ARGB64 - From - ARGB32]
// ============================================================================

static FOG_INLINE void p32ARGB64FromARGB32(
  uint32_t& dst0_10, uint32_t& dst0_32, const uint32_t& x0)
{
  uint32_t t0 = (x0      ) & 0xFF;             // [000B]
  uint32_t t1 = (x0 >> 16) & 0xFF;             // [000R]

  dst0_10 = _FOG_ACC_COMBINE_2(t0, (x0 << 8) & 0x00FF0000U);
  dst0_32 = _FOG_ACC_COMBINE_2(t1, (x0 >> 8) & 0x00FF0000U);

  dst0_10 = dst0_10 * 0x0101;
  dst0_32 = dst0_32 * 0x0101;
}

static FOG_INLINE void p64ARGB64FromARGB32(
  __p64& dst0, const uint32_t& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = _FOG_ACC_COMBINE_4((uint64_t)(x0 & 0x000000FFU)      ,
                            (uint64_t)(x0 & 0x0000FF00U) <<  8,
                            (uint64_t)(x0 & 0x00FF0000U) << 16,
                            (uint64_t)(x0 & 0xFF000000U) << 24);
  dst0 = dst0 * FOG_UINT64_C(0x0101);
#else
  p32ARGB64FromARGB32(dst0.u32Lo, dst0.u32Hi, x0);
#endif
}

// ============================================================================
// [Fog::Acc - ARGB64 - From - ARGB16_4444]
// ============================================================================

static FOG_INLINE void p32ARGB64FromARGB16_4444(
  uint32_t& dst0_10, uint32_t& dst0_32, const uint32_t& x0)
{
  uint32_t t0 = x0;

  dst0_10 = _FOG_ACC_COMBINE_2((t0     ) & 0x0000000F, (t0 << 12) & 0x000F0000);
  dst0_32 = _FOG_ACC_COMBINE_2((t0 >> 8) & 0x0000000F, (t0 <<  4) & 0x000F0000);

  p32Mul(dst0_10, dst0_10, 0x1111);
  p32Mul(dst0_32, dst0_32, 0x1111);
}

static FOG_INLINE void p32ARGB64FromARGB16_4444_bs(
  uint32_t& dst0_10, uint32_t& dst0_32, const uint32_t& x0)
{
  uint32_t t0 = x0;

  dst0_10 = _FOG_ACC_COMBINE_2((t0 >> 8) & 0x0000000F, (t0 <<  4) & 0x000F0000);
  dst0_32 = _FOG_ACC_COMBINE_2((t0     ) & 0x0000000F, (t0 << 12) & 0x000F0000);

  p32Mul(dst0_10, dst0_10, 0x1111);
  p32Mul(dst0_32, dst0_32, 0x1111);
}

// ============================================================================
// [Fog::Acc - ARGB64 - From - PRGB64]
// ============================================================================

static FOG_INLINE void p32ARGB64FromPRGB64_2031(uint32_t& dst0_20, uint32_t& dst0_31, const uint32_t& x0_20, const uint32_t& x0_31)
{
  uint32_t t0 = x0_20;
  uint32_t t1 = x0_31;

  uint32_t xa = x0_31 >> 16;
  if (xa == 0)
  {
    dst0_20 = t0;
    dst0_31 = t1;
    return;
  }

  dst0_20 = _FOG_ACC_COMBINE_2(((t0 >> 16) / xa) << 16, (t0 & 0xFFFF) / xa);
  dst0_31 = _FOG_ACC_COMBINE_2(xa << 16               , (t1 & 0xFFFF) / xa);
}

static FOG_INLINE void p32ARGB64FromPRGB64_1032(uint32_t& dst0_10, uint32_t& dst0_32, const uint32_t& x0_10, const uint32_t& x0_32)
{
  p32ARGB64FromPRGB64_2031(dst0_10, dst0_32, x0_10, x0_32);
}

static FOG_INLINE void p64ARGB64FromPRGB64(__p64& dst0, const __p64& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = x0;

  __p64 xa = dst0 >> 48;
  if (xa == 0) return;

  dst0 = _FOG_ACC_COMBINE_4(
    ((dst0 & FOG_UINT64_C(0x0000FFFF00000000)) / xa) & FOG_UINT64_C(0x0000FFFF00000000),
    ((dst0 & FOG_UINT64_C(0x00000000FFFF0000)) / xa) & FOG_UINT64_C(0x00000000FFFF0000),
    ((dst0 & FOG_UINT64_C(0x000000000000FFFF)) / xa), xa << 48);
#else
  p32ARGB64FromPRGB64_1032(dst0.u32Lo, dst0.u32Hi, x0.u32Lo, x0.u32Hi);
#endif // FOG_ARCH_NATIVE_P64
}

static FOG_INLINE void p64ARGB64FromPRGB64_2031(__p64& dst0_20, __p64& dst0_31, const __p64& x0_20, const __p64& x0_31)
{
#if defined(FOG_ARCH_NATIVE_P64)
  __p64 xa = x0_31 >> 32;
  __p64 t0 = _FOG_ACC_COMBINE_2(
    ((x0_20 & FOG_UINT64_C(0xFFFFFFFF00000000)) / xa) & FOG_UINT64_C(0xFFFFFFFF00000000),
    ((x0_20 & FOG_UINT64_C(0x00000000FFFFFFFF)) / xa));
  __p64 t1 = _FOG_ACC_COMBINE_2(
    ((x0_20 & FOG_UINT64_C(0x00000000FFFFFFFF)) / xa), xa << 32);
  dst0_20 = t0;
  dst0_31 = t1;
#else
  uint32_t xa = x0_31.u32Hi;
  uint32_t xr = x0_20.u32Hi / xa;
  uint32_t xg = x0_31.u32Lo / xa;
  uint32_t xb = x0_20.u32Lo / xa;

  dst0_20.u32Lo = xb;
  dst0_20.u32Hi = xr;
  dst0_31.u32Lo = xg;
  dst0_31.u32Hi = xa;
#endif // FOG_ARCH_NATIVE_P64
}

static FOG_INLINE void p64ARGB64FromPRGB64_1032(__p64& dst0_10, __p64& dst0_32, const __p64& x0_10, const __p64& x0_32)
{
  p64ARGB64FromPRGB64_2031(dst0_10, dst0_32, x0_10, x0_32);
}

// ============================================================================
// [Fog::Acc - PRGB64 - IsAlphaXX]
// ============================================================================

//! @brief Get whether the PRGB64 pixel @a c0 has alpha set to 0xFFFF (65535 in decimal).
static FOG_INLINE bool p32PRGB64IsAlphaFFFF_2031(const uint32_t& c0_20, const uint32_t& c0_31)
{
  return c0_31 >= 0xFFFF0000U;
}

//! @brief Get whether the PRGB64 pixel @a c0 has alpha set to 0x0000 (0 in decimal).
static FOG_INLINE bool p32PRGB64IsAlpha0000_2031(const uint32_t& c0_20, const uint32_t& c0_31)
{
  return c0_31 == 0x00000000U;
}

//! @brief Get whether the PRGB64 pixel @a c0 has alpha set to 0xFFFF (65535 in decimal).
static FOG_INLINE bool p32PRGB64IsAlphaFFFF_1032(const uint32_t& c0_10, const uint32_t& c0_32)
{
  return c0_32 >= 0xFFFF0000U;
}

//! @brief Get whether the PRGB64 pixel @a c0 has alpha set to 0x0000 (0 in decimal).
static FOG_INLINE bool p32PRGB64IsAlpha0000_1032(const uint32_t& c0_10, const uint32_t& c0_32)
{
  return c0_32 == 0x00000000U;
}

//! @brief Get whether the PRGB64 pixel @a c0 has alpha set to 0xFFFF (65535 in decimal).
static FOG_INLINE bool p64PRGB64IsAlphaFFFF(const __p64& c0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  return c0 >= FOG_UINT64_C(0xFFFF000000000000);
#else
  return c0.u32Hi >= 0xFFFF0000U;
#endif // FOG_ARCH_NATIVE_P64
}

//! @brief Get whether the PRGB64 pixel @a c0 has alpha set to 0x0000 (0 in decimal).
static FOG_INLINE bool p64PRGB64IsAlpha0000(const __p64& c0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  return c0 == 0;
#else
  return c0.u32Hi == 0x00000000U;
#endif // FOG_ARCH_NATIVE_P64
}

// ============================================================================
// [Fog::Acc - PRGB64 - From - PRGB32]
// ============================================================================

static FOG_INLINE void p32PRGB64FromPRGB32(
  uint32_t& dst0_10, uint32_t& dst0_32, const uint32_t& x0)
{
  p32ARGB64FromARGB32(dst0_10, dst0_32, x0);
}

static FOG_INLINE void p64PRGB64FromPRGB32(
  __p64& dst0, const uint32_t& x0)
{
  p64ARGB64FromARGB32(dst0, x0);
}

// ============================================================================
// [Fog::Acc - PRGB64 - From - PRGB16_4444]
// ============================================================================

static FOG_INLINE void p32PRGB64FromPRGB16_4444(
  uint32_t& dst0_10, uint32_t& dst0_32, const uint32_t& x0)
{
  p32ARGB64FromARGB16_4444(dst0_10, dst0_32, x0);
}

// ============================================================================
// [Fog::Acc - PRGB64 - From - ARGB64]
// ============================================================================

static FOG_INLINE void p32PRGB64FromARGB64_1032(
  uint32_t& dst0_10, uint32_t& dst0_32, const uint32_t& x0_10, const uint32_t& x0_32)
{
  uint32_t xa = x0_32 >> 16;

  uint32_t t0 = (x0_10 & 0xFFFF) * xa;
  uint32_t t1 = (x0_10    >> 16) * xa;
  uint32_t t2 = (x0_32 & 0xFFFF) * xa;

  t0 = (t0 + (t0 >> 16) + 0x8000U);
  t1 = (t1 + (t1 >> 16) + 0x8000U);
  t2 = (t2 + (t2 >> 16) + 0x8000U);

  dst0_10 = _FOG_ACC_COMBINE_2(t0 >> 16, t1 & 0xFFFF0000);
  dst0_32 = _FOG_ACC_COMBINE_2(t2 >> 16, xa << 16);
}

static FOG_INLINE void p32PRGB64FromARGB64_2031(
  uint32_t& dst0_20, uint32_t& dst0_31, const uint32_t& x0_20, const uint32_t& x0_31)
{
  uint32_t xa = x0_31 >> 16;

  uint32_t t0 = (x0_20 & 0xFFFF) * xa;
  uint32_t t1 = (x0_20    >> 16) * xa;
  uint32_t t2 = (x0_31 & 0xFFFF) * xa;

  t0 = (t0 + (t0 >> 16) + 0x8000U);
  t1 = (t1 + (t1 >> 16) + 0x8000U);
  t2 = (t2 + (t2 >> 16) + 0x8000U);

  dst0_20 = _FOG_ACC_COMBINE_2(t0 >> 16, t1 & 0xFFFF0000);
  dst0_31 = _FOG_ACC_COMBINE_2(t2 >> 16, xa << 16);
}

static FOG_INLINE void p64PRGB64FromARGB64(
  __p64& dst0, const __p64& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  __p64 xa = (x0 >> 48);
  __p64 t0 = (x0      ) & FOG_UINT64_C(0x0000FFFF0000FFFF);
  __p64 t1 = (x0 >> 16) & FOG_UINT64_C(0x0000FFFF0000FFFF);

  t1 |= FOG_UINT64_C(0x0000FFFF00000000);

  t0 *= xa;
  t1 *= xa;

  t0 = ((t0 + ((t0 >> 16) & FOG_UINT64_C(0x0000FFFF0000FFFF)) + FOG_UINT64_C(0x0000800000008000)) >> 16) & FOG_UINT64_C(0x0000FFFF0000FFFF);
  t1 = ((t1 + ((t1 >> 16) & FOG_UINT64_C(0x0000FFFF0000FFFF)) + FOG_UINT64_C(0x0000800000008000))      ) & FOG_UINT64_C(0xFFFF0000FFFF0000);

  dst0 = _FOG_ACC_COMBINE_2(t0, t1);
#else
  p32PRGB64FromARGB64_1032(dst0.u32Lo, dst0.u32Hi, x0.u32Lo, x0.u32Hi);
#endif // FOG_ARCH_NATIVE_P64
}

static FOG_INLINE void p64PRGB64FromARGB64_2031(
  __p64& dst0_20, __p64& dst0_31, const __p64& x0_20, const __p64& x0_31)
{
#if defined(FOG_ARCH_NATIVE_P64)
  __p64 xa = x0_31 >> 32;

  __p64 t0_20 = x0_20;
  __p64 t0_31 = x0_31 | FOG_UINT64_C(0x0000FFFF00000000);

  t0_20 *= xa;
  t0_31 *= xa;

  t0_20 = ((t0_20 + ((t0_20 >> 16) & FOG_UINT64_C(0x0000FFFF0000FFFF)) + FOG_UINT64_C(0x0000800000008000)) >> 16) & FOG_UINT64_C(0x0000FFFF0000FFFF);
  t0_31 = ((t0_31 + ((t0_31 >> 16) & FOG_UINT64_C(0x0000FFFF0000FFFF)) + FOG_UINT64_C(0x0000800000008000)) >> 16) & FOG_UINT64_C(0x0000FFFF0000FFFF);

  dst0_20 = t0_20;
  dst0_31 = t0_31;
#else
  uint32_t xa = x0_31.u32Hi;
  uint32_t xr = x0_20.u32Hi;
  uint32_t xg = x0_31.u32Lo;
  uint32_t xb = x0_20.u32Lo;

  p32MulDiv65535SWD(dst0_20.u32Lo, xb, xa);
  p32MulDiv65535SWD(dst0_20.u32Hi, xr, xa);
  p32MulDiv65535SWD(dst0_31.u32Lo, xg, xa);
  p32Copy(dst0_31.u32Hi, xa);
#endif // FOG_ARCH_NATIVE_P64
}

static FOG_INLINE void p64PRGB64FromARGB64_1032(
  __p64& dst0_20, __p64& dst0_31, const __p64& x0_20, const __p64& x0_31)
{
  p64PRGB64FromARGB64_2031(dst0_20, dst0_31, x0_20, x0_31);
}

// ============================================================================
// [Fog::Acc - Misc]
// ============================================================================

// TODO: Categorize.

static FOG_INLINE void p32LoadPRGB32_2031_FromPRGB64(
  uint32_t& dst_20, uint32_t& dst_31, const uint8_t* mem)
{
  dst_20 = mem[PIXEL_ARGB64_BYTE_R_HI];
  dst_31 = mem[PIXEL_ARGB64_BYTE_A_HI];

  dst_20 <<= 16;
  dst_31 <<= 16;

  dst_20 |= mem[PIXEL_ARGB64_BYTE_B_HI];
  dst_31 |= mem[PIXEL_ARGB64_BYTE_G_HI];
}

static FOG_INLINE void p32LoadZRGB32_2031_FromPRGB64(
  uint32_t& dst_20, uint32_t& dst_31, const uint8_t* mem)
{
  dst_20 = mem[PIXEL_ARGB64_BYTE_R_HI];
  dst_31 = mem[PIXEL_ARGB64_BYTE_G_HI];

  dst_20 <<= 16;
  dst_20 |= mem[PIXEL_ARGB64_BYTE_B_HI];
}

static FOG_INLINE void p32LoadPRGB32FromPRGB64(
  uint32_t& dst, const uint8_t* mem)
{
#if defined(FOG_ARCH_HAS_FAST_MUL)
  uint32_t t;

  Acc::p32Load8a(dst, t, mem);
  Acc::p32PRGB32FromPRGB64_1032(dst, dst, t);
#else
  dst  = src[PIXEL_ARGB64_BYTE_A_HI]; dst <<= 8;
  dst |= src[PIXEL_ARGB64_BYTE_R_HI]; dst <<= 8;
  dst |= src[PIXEL_ARGB64_BYTE_G_HI]; dst <<= 8;
  dst |= src[PIXEL_ARGB64_BYTE_B_HI];
#endif // FOG_ARCH_HAS_FAST_MUL
}

static FOG_INLINE void p32LoadZRGB32FromPRGB64(
  uint32_t& dst, const uint8_t* mem)
{
#if defined(FOG_ARCH_HAS_FAST_MUL)
  uint32_t t;

  Acc::p32Load8a(dst, t, mem);
  Acc::p32PRGB32FromPRGB64_1032(dst, dst, t);
#else
  dst  = src[PIXEL_ARGB64_BYTE_R_HI]; dst <<= 8;
  dst |= src[PIXEL_ARGB64_BYTE_G_HI]; dst <<= 8;
  dst |= src[PIXEL_ARGB64_BYTE_B_HI];
#endif // FOG_ARCH_HAS_FAST_MUL
}

static FOG_INLINE void p32LoadZRGB32_2031_FromRGB48(
  uint32_t& dst_20, uint32_t& dst_Z1, const uint8_t* mem)
{
  dst_20 = mem[PIXEL_ARGB64_BYTE_R_HI];
  dst_Z1 = mem[PIXEL_ARGB64_BYTE_G_HI];

  dst_20 <<= 16;
  dst_20 |= mem[PIXEL_ARGB64_BYTE_B_HI];
}

//! @}

} // Acc namespace
} // Fog namespace

// [Guard]
#endif // _FOG_G2D_ACC_ACCC_H
