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
// [Fog::RasterEngine::C - Composite - Multiply]
// ============================================================================

//! @internal
struct FOG_HIDDEN CompositeMultiplyC : public CompositeBaseFuncsC32<CompositeMultiplyC>
{
  enum { CHARACTERISTICS = OPERATOR_CHAR_MULTIPLY };

  static FOG_INLINE void prgb32_op_prgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    uint32_t aa = ByteUtil::byte1x2_hi(a1);
    uint32_t ba = ByteUtil::byte1x2_hi(b1);

    ByteUtil::byte2x2_muldiv255_byte2x2(dst0, a0, b0, dst1, a1, b1);

    ByteUtil::byte2x2_muldiv255_u(a0, a0, a1, a1, ByteUtil::scalar_neg255(ba));
    ByteUtil::byte2x2_muldiv255_u(b0, b0, b1, b1, ByteUtil::scalar_neg255(aa));

    ByteUtil::byte2x2_add_byte2x2(dst0, dst0, a0, dst1, dst1, a1);
    ByteUtil::byte2x2_addus_byte2x2(dst0, dst0, b0, dst1, dst1, b1);
  }

  static FOG_INLINE void prgb32_op_xrgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    uint32_t aa = ByteUtil::byte1x2_hi(a1);
    ByteUtil::byte2x2_muldiv255_byte2x2(dst0, a0, b0, dst1, a1, b1);

    ByteUtil::byte2x2_muldiv255_u(b0, b0, b1, b1, ByteUtil::scalar_neg255(aa));
    ByteUtil::byte2x2_add_byte2x2(dst0, dst0, a0, dst1, dst1, a1);

    ByteUtil::byte1x2_fill_hi(dst1, dst1);
  }

  static FOG_INLINE void xrgb32_op_prgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    uint32_t ba = ByteUtil::byte1x2_hi(b1);
    ByteUtil::byte2x2_muldiv255_byte2x2(dst0, a0, b0, dst1, a1, b1);

    ByteUtil::byte2x2_muldiv255_u(a0, a0, a1, a1, ByteUtil::scalar_neg255(ba));
    ByteUtil::byte2x2_add_byte2x2(dst0, dst0, a0, dst1, dst1, a1);

    ByteUtil::byte1x2_fill_hi(dst1, dst1);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    ByteUtil::byte2x2_muldiv255_byte2x2(dst0, a0, b0, dst1, a1, b1);
    ByteUtil::byte1x2_fill_hi(dst1, dst1);
  }
};

} // RasterEngine namespace
} // Fog namespace
