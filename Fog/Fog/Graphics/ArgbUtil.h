// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_ARGBUTIL_H
#define _FOG_GRAPHICS_ARGBUTIL_H

// [Dependencies]
#include <Fog/Graphics/Argb.h>
#include <Fog/Graphics/Constants.h>

//! @addtogroup Fog_Graphics
//! @{

namespace Fog {
namespace ArgbUtil {

// ============================================================================
// [Fog::ArgbUtil - Demultiply Reciprocal Table]
// ============================================================================

//! @brief Reciprocal table that can be used to demultiply color value in
//! 32-bits precision.
extern FOG_API const uint32_t demultiply_reciprocal_table_d[256];

//! @brief Reciprocal table that can be used to demultiply color value in
//! 16-bits precision (used in MMX/SSE2 demultiply-code).
extern FOG_API const uint16_t demultiply_reciprocal_table_w[256*4];

//! @brief Reciprocal table that can be used to demultiply color value in
//! floating point format (for filters that needs floating point accuracy).
extern FOG_API const float demultiply_reciprocal_table_f[256];

// ============================================================================
// [Fog::ArgbUtil - isAlpha0xXX]
// ============================================================================

//! @brief Return @c true whether alpha in color @a c0 is 0xFF (255 in decimal).
//!
//! Adventage of this function is that it should expand only to 2 assembler
//! instructions.
static FOG_INLINE bool isAlpha0xFF(uint32_t c0) { return (c0 >= 0xFF000000); }

//! @brief Return @c true whether alpha in color @a c0 is 0x00 (0 in decimal).
//!
//! Adventage of this function is that it should expand only to 2 assembler
//! instructions.
static FOG_INLINE bool isAlpha0x00(uint32_t c0) { return (c0 <= 0x00FFFFFF); }

// ============================================================================
// [Fog::ArgbUtil - FromGrey / ToGrey]
// ============================================================================

static FOG_INLINE uint32_t fromARGB(uint8_t a, uint8_t r, uint8_t g, uint8_t b)
{
  return ((uint32_t)a << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
}

static FOG_INLINE uint32_t fromGrey(uint8_t a, uint8_t grey)
{
  return fromARGB(a, grey, grey, grey);
}

static FOG_INLINE uint32_t getGrey(uint32_t c)
{
  // We are using this formula:
  //  0.212671 * R + 0.715160 * G + 0.072169 * B;
  // As:
  //  (13938 * R + 46868 * G + 4730 * B) / 65536

  uint32_t grey =
    ((c >> 16) & 0xFF) * 13938 +
    ((c >>  8) & 0xFF) * 46868 +
    ((c      ) & 0xFF) *  4730 ;

  return (uint8_t)(grey >> 16);
}

// ============================================================================
// [Fog::ArgbUtil - Premultiply / Demultiply]
// ============================================================================

//! @brief Return premultiplied color (@c PIXEL_FORMAT_PRGB32) in @a x given in
//! the @c PIXEL_FORMAT_ARGB.
static FOG_INLINE uint32_t premultiply(uint32_t x)
{
#if FOG_ARCH_BITS == 64 && 0
  uint32_t a = x >> 24;
  uint64_t x0 = ((uint64_t)x | ((uint64_t)x << 24)) & FOG_UINT64_C(0x000000FF00FF00FF);
  x0 |= FOG_UINT64_C(0x00FF000000000000);
  x0 *= a;
  x0 = (x0 + ((x0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF)) + FOG_UINT64_C(0x0080008000800080)) >> 8;
  x0 &= FOG_UINT64_C(0x00FF00FF00FF00FF);
  return (uint32_t)(x0 | (x0 >> 24));
#else
  uint32_t a = x >> 24;
  uint32_t t0 = (x & 0x00FF00FF) * a;
  uint32_t t1 = (x & 0x0000FF00) * a;

  t0 = ((t0 + ((t0 >> 8) & 0x00FF00FF) + 0x00800080) >> 8) & 0x00FF00FF;
  t1 = ((t1 + ((t1 >> 8) & 0x0000FF00) + 0x00008000) >> 8) & 0x0000FF00;

  return t0 | t1 | (a << 24);
#endif
}

//! @brief Return premultiplied color (@c PIXEL_FORMAT_PRGB32) in @a x given in
//! the @c PIXEL_FORMAT_ARGB.
static FOG_INLINE uint32_t premultiply(uint a, uint r, uint g, uint b)
{
#if FOG_ARCH_BITS == 64 && 0
  uint64_t x0 = ((uint64_t)b | ((uint64_t)r << 16) ((uint64_t)g << 32));
  x0 |= FOG_UINT64_C(0x00FF000000000000);
  x0 *= a;
  x0 = (x0 + ((x0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF)) + FOG_UINT64_C(0x0080008000800080)) >> 8;
  x0 &= FOG_UINT64_C(0x00FF00FF00FF00FF);
  return (uint32_t)(x0 | (x0 >> 24));
#else
  uint32_t t0 = (b | (r << 16)) * a;
  uint32_t t1 = (g << 8) * a;

  t0 = ((t0 + ((t0 >> 8) & 0x00FF00FF) + 0x00800080) >> 8) & 0x00FF00FF;
  t1 = ((t1 + ((t1 >> 8) & 0x0000FF00) + 0x00008000) >> 8) & 0x0000FF00;

  return t0 | t1 | (a << 24);
#endif
}

//! @brief Return demultiplied color (@c PIXEL_FORMAT_ARGB32) in @a x given in
//! the @c PIXEL_FORMAT_PRGB.
static FOG_INLINE uint32_t demultiply(uint32_t x)
{
  uint32_t a = (x >> 24);
  uint32_t recip = demultiply_reciprocal_table_d[a];

  uint32_t r = (x >> 16) & 0xFF;
  uint32_t g = (x >>  8) & 0xFF;
  uint32_t b = (x      ) & 0xFF;

  r = ((r * recip)      ) & 0x00FF0000;
  g = ((g * recip) >>  8) & 0x0000FF00;
  b = ((b * recip) >> 16) & 0x000000FF;

  return r | g | b | (a << 24);
}

// ============================================================================
// [Fog::ArgbUtil - ARGB <-> AHSV]
// ============================================================================

FOG_API uint32_t argbFromAhsv(float a, float h, float s, float v);

} // ArgbUtil namespace
} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_GRAPHICS_ARGBUTIL_H
