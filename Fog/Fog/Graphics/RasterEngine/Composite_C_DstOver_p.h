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
// [Fog::RasterEngine::C - Composite - DstOver]
// ============================================================================

//! @internal
struct FOG_HIDDEN CompositeDstOverC : public CompositeBaseFuncsC32<CompositeDstOverC>
{
  enum { CHARACTERISTICS = OPERATOR_CHAR_DST_OVER };

  static FOG_INLINE void prgb32_op_prgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    ByteUtil::byte2x2_muldiv255_u(b0, b0, b1, b1, ByteUtil::scalar_neg255(ByteUtil::byte1x2_hi(a1)));
    ByteUtil::byte2x2_add_byte2x2(dst0, a0, b0, dst1, a1, b1);
  }

  static FOG_INLINE void prgb32_op_xrgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    ByteUtil::byte2x2_muldiv255_u(b0, b0, b1, b1, ByteUtil::scalar_neg255(ByteUtil::byte1x2_hi(a1)));
    ByteUtil::byte2x2_add_byte2x2(dst0, a0, b0, dst1, a1, b1);

    ByteUtil::byte1x2_fill_hi(dst1, dst1);
  }

  static FOG_INLINE void xrgb32_op_prgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    // NOT USED!
    FOG_ASSERT_NOT_REACHED();
  }

  static FOG_INLINE void xrgb32_op_xrgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    // NOT USED!
    FOG_ASSERT_NOT_REACHED();
  }
};

} // RasterEngine namespace
} // Fog namespace
