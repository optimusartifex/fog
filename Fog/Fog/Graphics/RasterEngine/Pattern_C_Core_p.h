// [Fog-Graphics]
//
// [License]
// MIT, See COPYING file in package

// For some IDEs to enable code-assist.
#include <Fog/Core/Build.h>

#if defined(FOG_IDE)
#include <Fog/Graphics/RasterEngine/Defs_C_p.h>
#include <Fog/Graphics/RasterEngine/Dib_C_p.h>
#endif // FOG_IDE

namespace Fog {
namespace RasterEngine {

// ============================================================================
// [Fog::RasterEngine::C - Pattern - Core]
// ============================================================================

//! @internal
struct FOG_HIDDEN PatternCoreC
{
  // --------------------------------------------------------------------------
  // [Pattern - Core - Helpers - For making repeat patterns and fills]
  // --------------------------------------------------------------------------

  static uint8_t* FOG_FASTCALL repeat32(uint8_t* dst, sysuint_t linew, sysint_t w)
  {
    FOG_ASSERT(w > 0);

    uint8_t* base = dst - (linew * 4);
    uint8_t* src = base;

    sysint_t i;

    for (;;)
    {
      i = Math::min((sysint_t)linew, w);
      w -= i;

      i -= 4;
      while (i >= 0)
      {
        Memory::copy16B(dst, src);

        dst += 16;
        src += 16;
        i -= 4;
      }
      i += 4;

      while (i)
      {
        ((uint32_t*)dst)[0] = READ_32(src);

        dst += 4;
        src += 4;
        i--;
      }
      if (!w) break;

      src = base;
      linew <<= 1;
    }
    return dst;
  }

  static void FOG_FASTCALL repeat8(uint8_t* dst, sysuint_t linew, sysint_t w)
  {
    FOG_ASSERT(w > 0);

    uint8_t* base = dst - (linew * 4);

    uint8_t* srcCur = base;
    uint8_t* dstCur = dst;

    sysint_t i;

    for (;;)
    {
      i = Math::min((sysint_t)linew, w);
      w -= i;

      memcpy(dstCur, srcCur, (sysuint_t)i);
      dstCur += i;
      if (!w) break;

      srcCur = base;
      linew <<= 1;
    }
  }

  static FOG_INLINE uint8_t* fill32(uint8_t* dst, uint32_t c0, sysint_t w)
  {
    FOG_ASSERT(w > 0);

#if FOG_ARCH_BITS == 64
    uint64_t c0_64 = (uint64_t)c0 + ((uint64_t)c0 << 32);
#endif // FOG_ARCH_BITS

    while ((w -= 4) >= 0)
    {
#if FOG_ARCH_BITS == 64
      ((uint64_t*)dst)[0] = c0_64;
      ((uint64_t*)dst)[1] = c0_64;
#else
      ((uint32_t*)dst)[0] = c0;
      ((uint32_t*)dst)[1] = c0;
      ((uint32_t*)dst)[2] = c0;
      ((uint32_t*)dst)[3] = c0;
#endif // FOG_ARCH_BITS

      dst += 16;
    }
    w += 4;

    while (w)
    {
      ((uint32_t*)dst)[0] = c0;

      dst += 4;
      w--;
    }
    return dst;
  }

  static FOG_INLINE uint8_t* fill8(uint8_t* dst, uint32_t c0, sysint_t w)
  {
    FOG_ASSERT(w > 0);

    memset(dst, (int)c0, (sysuint_t)w);
    return dst + w;
  }

  // --------------------------------------------------------------------------
  // [Pattern - Core - Fetch, Solid]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL fetch_solid(
    Span* span, uint8_t* buffer, uint32_t mode, uint32_t src0)
  {
    if (mode == PATTERN_FETCH_CAN_USE_SRC)
    {
      // If the buffer will not be modified then we can fetch only width needed
      // by the largest span here. If there is only one span then this is 
      // without effect, if there are many small spans then we can save some
      // CPU cycles.
      sysint_t filledWidth = 0;

      P_FETCH_SPAN8_INIT()
      P_FETCH_SPAN8_BEGIN()
        P_FETCH_SPAN8_SET_CUSTOM(buffer)

        if (filledWidth < w)
        {
          dst = PatternCoreC::fill32(dst, src0, w - filledWidth);
          filledWidth = w;
        }

        P_FETCH_SPAN8_NEXT()
      P_FETCH_SPAN8_END()
    }
    else
    {
      P_FETCH_SPAN8_INIT()
      P_FETCH_SPAN8_BEGIN()
        P_FETCH_SPAN8_SET_CURRENT()
        dst = PatternCoreC::fill32(dst, src0, w);
        P_FETCH_SPAN8_NEXT()
      P_FETCH_SPAN8_END()
    }
  }
};

} // RasterEngine namespace
} // Fog namespace
