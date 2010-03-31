// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_RASTERENGINE_C_DEPRECATED_H
#define _FOG_GRAPHICS_RASTERENGINE_C_DEPRECATED_H

// [Dependencies]
#include <Fog/Build/Build.h>
#include <Fog/Graphics/Argb.h>
#include <Fog/Graphics/ArgbUtil.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Geometry.h>
#include <Fog/Graphics/Image.h>
#include <Fog/Graphics/RasterEngine_p.h>

//! @addtogroup Fog_Graphics_Raster
//! @{

// This file is included from all Fog-Graphics/RasterEngine/RasterEngine_.h includes and
// .cpp files, so in future it may contain generic code for these modules.

namespace Fog {
namespace RasterEngine {






















// ============================================================================
// [Fog::Raster - Copy]
// ============================================================================

// TODO: Ideally move to Fog::Memory...

static FOG_INLINE void copy1(uint8_t* dest, const uint8_t* src)
{
  *dest = *src;
}

static FOG_INLINE void copy2(uint8_t* dest, const uint8_t* src)
{
  ((uint16_t *)dest)[0] = ((uint16_t *)src)[0];
}

static FOG_INLINE void copy3(uint8_t* dest, const uint8_t* src)
{
  ((uint16_t *)dest)[0] = ((uint16_t *)src)[0];
  ((uint8_t  *)dest)[2] = ((uint8_t  *)src)[2];
}

static FOG_INLINE void copy4(uint8_t* dest, const uint8_t* src)
{
  ((uint32_t *)dest)[0] = ((uint32_t *)src)[0];
}

static FOG_INLINE void copy8(uint8_t* dest, const uint8_t* src)
{
#if FOG_ARCH_BITS == 64
  ((uint64_t *)dest)[0] = ((uint64_t *)src)[0];
#else
  ((uint32_t *)dest)[0] = ((uint32_t *)src)[0];
  ((uint32_t *)dest)[1] = ((uint32_t *)src)[1];
#endif
}

static FOG_INLINE void copy12(uint8_t* dest, const uint8_t* src)
{
#if FOG_ARCH_BITS == 64
  ((uint64_t *)dest)[0] = ((uint64_t *)src)[0];
  ((uint32_t *)dest)[2] = ((uint32_t *)src)[2];
#else
  ((uint32_t *)dest)[0] = ((uint32_t *)src)[0];
  ((uint32_t *)dest)[1] = ((uint32_t *)src)[1];
  ((uint32_t *)dest)[2] = ((uint32_t *)src)[2];
#endif
}

static FOG_INLINE void copy16(uint8_t* dest, const uint8_t* src)
{
#if defined(FOG_HARDCODE_SSE2)
  _mm_storeu_si128(&((__m128i *)dest)[0], _mm_loadu_si128(&((__m128i *)src)[0]));
#elif FOG_ARCH_BITS == 64
  ((uint64_t *)dest)[0] = ((uint64_t *)src)[0];
  ((uint64_t *)dest)[1] = ((uint64_t *)src)[1];
#else
  ((uint32_t *)dest)[0] = ((uint32_t *)src)[0];
  ((uint32_t *)dest)[1] = ((uint32_t *)src)[1];
  ((uint32_t *)dest)[2] = ((uint32_t *)src)[2];
  ((uint32_t *)dest)[3] = ((uint32_t *)src)[3];
#endif
}

static FOG_INLINE void copy24(uint8_t* dest, const uint8_t* src)
{
  copy16(dest, src);
  copy8(dest + 16, src + 16);
}

static FOG_INLINE void copy32(uint8_t* dest, const uint8_t* src)
{
  copy16(dest, src);
  copy16(dest + 16, src + 16);
}

// ============================================================================
// [Fog::Raster - Set]
// ============================================================================

// TODO: Ideally move to Fog::Memory...

static FOG_INLINE void set4(uint8_t* dest, uint32_t pattern)
{
  ((uint32_t *)dest)[0] = pattern;
}

static FOG_INLINE void set8(uint8_t* dest, uint32_t pattern)
{
  ((uint32_t *)dest)[0] = pattern;
  ((uint32_t *)dest)[1] = pattern;
}

static FOG_INLINE void set12(uint8_t* dest, uint32_t pattern)
{
  ((uint32_t *)dest)[0] = pattern;
  ((uint32_t *)dest)[1] = pattern;
  ((uint32_t *)dest)[2] = pattern;
}

static FOG_INLINE void set16(uint8_t* dest, uint32_t pattern)
{
  ((uint32_t *)dest)[0] = pattern;
  ((uint32_t *)dest)[1] = pattern;
  ((uint32_t *)dest)[2] = pattern;
  ((uint32_t *)dest)[3] = pattern;
}

static FOG_INLINE void set24(uint8_t* dest, uint32_t pattern)
{
  ((uint32_t *)dest)[0] = pattern;
  ((uint32_t *)dest)[1] = pattern;
  ((uint32_t *)dest)[2] = pattern;
  ((uint32_t *)dest)[3] = pattern;
  ((uint32_t *)dest)[4] = pattern;
  ((uint32_t *)dest)[5] = pattern;
}

static FOG_INLINE void set32(uint8_t* dest, uint32_t pattern)
{
  ((uint32_t *)dest)[0] = pattern;
  ((uint32_t *)dest)[1] = pattern;
  ((uint32_t *)dest)[2] = pattern;
  ((uint32_t *)dest)[3] = pattern;
  ((uint32_t *)dest)[4] = pattern;
  ((uint32_t *)dest)[5] = pattern;
  ((uint32_t *)dest)[6] = pattern;
  ((uint32_t *)dest)[7] = pattern;
}

static FOG_INLINE void set8(uint8_t* dest, uint64_t pattern)
{
  ((uint64_t *)dest)[0] = pattern;
}

static FOG_INLINE void set12(uint8_t* dest, uint64_t pattern)
{
  ((uint64_t *)dest)[0] = pattern;
  ((uint32_t *)dest)[2] = (uint32_t)pattern;
}

static FOG_INLINE void set16(uint8_t* dest, uint64_t pattern)
{
  ((uint64_t *)dest)[0] = pattern;
  ((uint64_t *)dest)[1] = pattern;
}

static FOG_INLINE void set24(uint8_t* dest, uint64_t pattern)
{
  ((uint64_t *)dest)[0] = pattern;
  ((uint64_t *)dest)[1] = pattern;
  ((uint64_t *)dest)[2] = pattern;
}

static FOG_INLINE void set32(uint8_t* dest, uint64_t pattern)
{
  ((uint64_t *)dest)[0] = pattern;
  ((uint64_t *)dest)[1] = pattern;
  ((uint64_t *)dest)[2] = pattern;
  ((uint64_t *)dest)[3] = pattern;
}

























// ============================================================================
// [Fog::Raster - PixFmt_ARGB32]
// ============================================================================

struct FOG_HIDDEN PixFmt_ARGB32
{
  enum { BytesPerPixel = 4 };

  static FOG_INLINE uint32_t fetch(const uint8_t* p)
  {
    return ((const uint32_t *)p)[0];
  }

  static FOG_INLINE void store(uint8_t* p, uint32_t s0)
  {
    ((uint32_t *)p)[0] = s0;
  }
};


// ============================================================================
// [Fog::Raster - PixFmt_A8]
// ============================================================================

struct FOG_HIDDEN PixFmt_A8
{
  enum { BytesPerPixel = 2 };

  static FOG_INLINE uint32_t fetch(const uint8_t* p)
  {
    return p[0];
  }

  static FOG_INLINE void store(uint8_t* p, uint32_t s0)
  {
    p[0] = (uint8_t)s0;
  }
};





















} // RasterEngine namespace
} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_GRAPHICS_RASTERENGINE_C_DEPRECATED_H
