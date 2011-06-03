// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_RENDER_RENDER_C_CBASE_P_H
#define _FOG_G2D_RENDER_RENDER_C_CBASE_P_H

// [Dependencies]
#include <Fog/G2d/Render/Render_C/Defs_p.h>

namespace Fog {
namespace Render_C {

// ============================================================================
// [Fog::Render_C - Composite - Base]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT CBase
{
  // ==========================================================================
  // [MemFill - 32]
  // ==========================================================================

  struct FOG_NO_EXPORT MemFill32Context
  {
    FOG_INLINE void init(uint32_t pattern)
    {
#if FOG_ARCH_BITS >= 64
      _pattern = (uint64_t)pattern * FOG_UINT64_C(0x0000000100000001);
#else
      _pattern = pattern;
#endif
    }

    FOG_INLINE uint8_t* fill(uint8_t* dst, int w)
    {
#if FOG_ARCH_BITS >= 64
      // Align to 64-bits when running on 64-bit mode.
      if ((size_t)dst & 0x7)
      {
        ((uint32_t*)dst)[0] = (uint32_t)_pattern;
        dst += 4;
        if (--w == 0) return dst;
      }

      while ((w -= 8) >= 0)
      {
        ((uint64_t*)dst)[0] = _pattern;
        ((uint64_t*)dst)[1] = _pattern;
        ((uint64_t*)dst)[2] = _pattern;
        ((uint64_t*)dst)[3] = _pattern;

        dst += 32;
      }
#else
      while ((w -= 8) >= 0)
      {
        ((uint32_t*)dst)[0] = _pattern;
        ((uint32_t*)dst)[1] = _pattern;
        ((uint32_t*)dst)[2] = _pattern;
        ((uint32_t*)dst)[3] = _pattern;
        ((uint32_t*)dst)[4] = _pattern;
        ((uint32_t*)dst)[5] = _pattern;
        ((uint32_t*)dst)[6] = _pattern;
        ((uint32_t*)dst)[7] = _pattern;

        dst += 32;
      }
#endif // FOG_ARCH_BITS

      w += 8;
      dst += (uint)w * 4;

      switch (w)
      {
        default: FOG_ASSERT_NOT_REACHED();

        case 7: ((uint32_t*)dst)[-7] = (uint32_t)_pattern;
        case 6: ((uint32_t*)dst)[-6] = (uint32_t)_pattern;
        case 5: ((uint32_t*)dst)[-5] = (uint32_t)_pattern;
        case 4: ((uint32_t*)dst)[-4] = (uint32_t)_pattern;
        case 3: ((uint32_t*)dst)[-3] = (uint32_t)_pattern;
        case 2: ((uint32_t*)dst)[-2] = (uint32_t)_pattern;
        case 1: ((uint32_t*)dst)[-1] = (uint32_t)_pattern;
        case 0: break;
      }

      return dst;
    }

#if FOG_ARCH_BITS >= 64
    uint64_t _pattern;
#else
    uint32_t _pattern;
#endif
  };
};

} // Render_C namespace
} // Fog namespace

// [Guard]
#endif // _FOG_G2D_RENDER_RENDER_C_CBASE_P_H
