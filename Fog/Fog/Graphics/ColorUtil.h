// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_COLORUTIL_H
#define _FOG_GRAPHICS_COLORUTIL_H

// [Dependencies]
#include <Fog/Graphics/Color.h>
#include <Fog/Graphics/Constants.h>

namespace Fog {
namespace ArgbUtil {

//! @addtogroup Fog_Graphics_Other
//! @{

// ============================================================================
// [Fog::ArgbUtil - Premultiply / Demultiply]
// ============================================================================

//! @brief Return premultiplied color (@c IMAGE_FORMAT_PRGB32) in @a x given in
//! the @c IMAGE_FORMAT_ARGB.
static FOG_INLINE uint32_t premultiply(uint32_t x)
{
#if FOG_ARCH_BITS == 64
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

  return (a << 24) | t0 | t1;
#endif
}

//! @brief Return premultiplied color (@c IMAGE_FORMAT_PRGB32) in @a x given in
//! the @c IMAGE_FORMAT_ARGB.
static FOG_INLINE uint32_t premultiply(uint32_t a, uint32_t r, uint32_t g, uint32_t b)
{
#if FOG_ARCH_BITS == 64
  uint64_t x0 = ((uint64_t)b      ) | ((uint64_t)r << 16)              | 
                ((uint64_t)g << 32) | FOG_UINT64_C(0x00FF000000000000) ;
  x0 *= a;
  x0 = (x0 + ((x0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF)) + FOG_UINT64_C(0x0080008000800080)) >> 8;
  x0 &= FOG_UINT64_C(0x00FF00FF00FF00FF);
  return (uint32_t)(x0 | (x0 >> 24));
#else
  uint32_t t0 = (b | (r  << 16)) * a;
  uint32_t t1 = (g | 0x00FF0000) * a;

  t0 = ((t0 + ((t0 >> 8) & 0x00FF00FF) + 0x00800080) >> 8) & 0x00FF00FF;
  t1 = ((t1 + ((t1 >> 8) & 0x00FF00FF) + 0x00800080)     ) & 0xFF00FF00;

  return t0 | t1;
#endif
}

//! @brief Return demultiplied color (@c IMAGE_FORMAT_ARGB32) in @a x given in
//! the @c IMAGE_FORMAT_PRGB.
static FOG_INLINE uint32_t demultiply(uint32_t x)
{
  uint32_t a = (x >> 24);
  uint32_t recip = raster_demultiply_reciprocal_table_d[a];

  uint32_t r = (x >> 16) & 0xFF;
  uint32_t g = (x >>  8) & 0xFF;
  uint32_t b = (x      ) & 0xFF;

  r = ((r * recip)      ) & 0x00FF0000;
  g = ((g * recip) >>  8) & 0x0000FF00;
  b = ((b * recip) >> 16) & 0x000000FF;

  return (a << 24) | r | g | b;
}

// ============================================================================
// [Fog::ArgbUtil - ARGB <-> AHSV]
// ============================================================================

FOG_API uint32_t argbFromAhsv(float a, float h, float s, float v);

//! @}

} // ArgbUtil namespace
} // Fog namespace

// [Guard]
#endif // _FOG_GRAPHICS_COLORUTIL_H
