// [Fog-Graphics]
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
// [Fog::RasterEngine::C - Composite - Subtract]
// ============================================================================

//! @internal
struct FOG_HIDDEN CompositeSubtractC : public CompositeBaseFuncsC32<CompositeSubtractC>
{
  enum { CHARACTERISTICS = OPERATOR_CHAR_SUBTRACT };

  static FOG_INLINE void prgb32_op_prgb32_32b(
    Face::b32_1x2& dst0, Face::b32_1x2 a0, Face::b32_1x2 b0,
    Face::b32_1x2& dst1, Face::b32_1x2 a1, Face::b32_1x2 b1)
  {
    uint32_t aa = Face::b32_1x2GetB1(a1);
    uint32_t ba = Face::b32_1x2GetB1(b1);

    Face::b32_2x2SubusB32_2x2(dst0, a0, b0, dst1, a1, b1);
    Face::b32_1x2SetB1(dst1, dst1, (aa + ba - Face::b32_1x1MulDiv255(aa, ba)));
  }

  static FOG_INLINE void prgb32_op_xrgb32_32b(
    Face::b32_1x2& dst0, Face::b32_1x2 a0, Face::b32_1x2 b0,
    Face::b32_1x2& dst1, Face::b32_1x2 a1, Face::b32_1x2 b1)
  {
    Face::b32_2x2SubusB32_2x2(dst0, a0, b0, dst1, a1, b1);
    Face::b32_1x2FillB1(dst1, dst1);
  }

  static FOG_INLINE void xrgb32_op_prgb32_32b(
    Face::b32_1x2& dst0, Face::b32_1x2 a0, Face::b32_1x2 b0,
    Face::b32_1x2& dst1, Face::b32_1x2 a1, Face::b32_1x2 b1)
  {
    Face::b32_2x2SubusB32_2x2(dst0, a0, b0, dst1, a1, b1);
    Face::b32_1x2FillB1(dst1, dst1);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_32b(
    Face::b32_1x2& dst0, Face::b32_1x2 a0, Face::b32_1x2 b0,
    Face::b32_1x2& dst1, Face::b32_1x2 a1, Face::b32_1x2 b1)
  {
    Face::b32_2x2SubusB32_2x2(dst0, a0, b0, dst1, a1, b1);
    Face::b32_1x2FillB1(dst1, dst1);
  }
};

} // RasterEngine namespace
} // Fog namespace
