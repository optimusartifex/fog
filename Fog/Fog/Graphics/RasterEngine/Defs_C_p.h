// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// For some IDEs to enable code-assist.
#include <Fog/Build/Build.h>

#if defined(FOG_IDE)
#include <Fog/Graphics/RasterEngine/C_p.h>
#endif // FOG_IDE

namespace Fog {
namespace RasterEngine {

// ============================================================================
// [Fog::RasterEngine::C - Defines]
// ============================================================================

#define READ_8(ptr)  (((const uint8_t *)(ptr))[0])
#define READ_16(ptr) (((const uint16_t*)(ptr))[0])
#define READ_32(ptr) (((const uint32_t*)(ptr))[0])

  // Macros to help creating cspan_a8_scanline blitting functions.
//
// Purpose of this macro is to simplify making such functions. This inner loop
// is quite optimized so don't worry about it. You need only to create code
// that will be added after:
//
//   BLIT_CSPAN_SCANLINE_STEP2_CONST and
//   BLIT_CSPAN_SCANLINE_STEP3_MASK.
//
// Typical code looks like this:
//
//    BLIT_CSPAN_SCANLINE_STEP1_BEGIN(4)
//
//    Initialize your code for filling, unpack pixels, etc...
//
//    // Const mask.
//    BLIT_CSPAN_SCANLINE_STEP2_CONST()
//    {
//      'dst'  - Destination pointer where to write pixels.
//      'w'    - Count of pixels to write (width).
//      'msk0' - Constant pixels mask (you can check for common case that
//               is 0xFF, never check for 0x00 - this never happens here).
//    }
//    // Variable mask.
//    BLIT_CSPAN_SCANLINE_STEP3_MASK()
//    {
//      'dst'  - Destination pointer where to write pixels.
//      'w'    - Count of pixels to write (width).
//      'msk'  - Pointer to mask array (A8 array). Omit checks for nulls and
//               full opaque pixels here, this happens rarely.
//    }
//    BLIT_CSPAN_SCANLINE_STEP4_END()

#define BLIT_CSPAN_SCANLINE_STEP1_BEGIN(BPP) \
  const Scanline32::Span* span = spans; \
  uint8_t* dstBase = dst; \
  \
  for (;;) \
  { \
    sysint_t x = span->x; \
    sysint_t w = span->len; \
    \
    dst = dstBase + (sysuint_t)x * BPP;

#define BLIT_CSPAN_SCANLINE_STEP2_CONST() \
    if (FOG_UNLIKELY(w < 0)) \
    { \
      w = -w; \
      FOG_ASSERT(w > 0); \
      \
      uint32_t msk0 = (uint32_t)*(span->covers);

#define BLIT_CSPAN_SCANLINE_STEP3_MASK() \
      if (--numSpans == 0) break; \
      ++span; \
    } \
    else \
    { \
      FOG_ASSERT(w > 0); \
      \
      const uint8_t* msk = span->covers;

#define BLIT_CSPAN_SCANLINE_STEP4_END() \
      if (--numSpans == 0) break; \
      ++span; \
    } \
  }

} // RasterEngine namespace
} // Fog namespace
