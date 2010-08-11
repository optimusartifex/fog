// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// For some IDEs to enable code-assist.
#include <Fog/Core/Build.h>

#if defined(FOG_IDE)
#include <Fog/Graphics/RasterEngine/Defs_C_p.h>
#endif // FOG_IDE

namespace Fog {
namespace RasterEngine {

// ============================================================================
// [Fog::RasterEngine::C - Composite - DstAtop]
// ============================================================================

//! @internal
struct FOG_HIDDEN CompositeDstAtopC : public CompositeBaseFuncsC32<CompositeDstAtopC>
{
  enum { CHARACTERISTICS = OPERATOR_CHAR_DST_ATOP };

  static FOG_INLINE void prgb32_op_prgb32_32b(
    ByteSIMD::b32_1x2& dst0, ByteSIMD::b32_1x2 a0, ByteSIMD::b32_1x2 b0,
    ByteSIMD::b32_1x2& dst1, ByteSIMD::b32_1x2 a1, ByteSIMD::b32_1x2 b1)
  {
    uint32_t aainv = ByteSIMD::u32Negate255(ByteSIMD::b32_1x2GetB1(a1));
    uint32_t ba = ByteSIMD::b32_1x2GetB1(b1);

    ByteSIMD::b32_2x2MulDiv255U_2x_Add(
      dst0, a0, ba, b0, aainv,
      dst1, a1, ba, b1, aainv);
  }

  static FOG_INLINE void prgb32_op_xrgb32_32b(
    ByteSIMD::b32_1x2& dst0, ByteSIMD::b32_1x2 a0, ByteSIMD::b32_1x2 b0,
    ByteSIMD::b32_1x2& dst1, ByteSIMD::b32_1x2 a1, ByteSIMD::b32_1x2 b1)
  {
    // NOT USED!
    FOG_ASSERT_NOT_REACHED();
  }

  static FOG_INLINE void xrgb32_op_prgb32_32b(
    ByteSIMD::b32_1x2& dst0, ByteSIMD::b32_1x2 a0, ByteSIMD::b32_1x2 b0,
    ByteSIMD::b32_1x2& dst1, ByteSIMD::b32_1x2 a1, ByteSIMD::b32_1x2 b1)
  {
    // NOT USED!
    FOG_ASSERT_NOT_REACHED();
  }

  static FOG_INLINE void xrgb32_op_xrgb32_32b(
    ByteSIMD::b32_1x2& dst0, ByteSIMD::b32_1x2 a0, ByteSIMD::b32_1x2 b0,
    ByteSIMD::b32_1x2& dst1, ByteSIMD::b32_1x2 a1, ByteSIMD::b32_1x2 b1)
  {
    // NOT USED!
    FOG_ASSERT_NOT_REACHED();
  }
};

} // RasterEngine namespace
} // Fog namespace
