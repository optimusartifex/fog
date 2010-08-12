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
// [Fog::RasterEngine::C - Composite - Invert]
// ============================================================================

//! @internal
struct FOG_HIDDEN CompositeInvertC : public CompositeBaseFuncsC32<CompositeInvertC>
{
  enum { CHARACTERISTICS = OPERATOR_CHAR_INVERT };

  static FOG_INLINE void prgb32_op_prgb32_32b(
    Face::b32_1x2& dst0, Face::b32_1x2 a0, Face::b32_1x2 b0,
    Face::b32_1x2& dst1, Face::b32_1x2 a1, Face::b32_1x2 b1)
  {
    uint32_t aa;
    Face::b32_1x2ExpandB1(aa, a1);
    Face::b32_2x2SubB32_2x2(dst0, aa, a0, dst1, aa, a1);

    uint32_t ba = Face::b32_1x2GetB1(b1);
    Face::b32_2x2MulDiv255U(dst0, dst0, dst1, dst1, ba);

    Face::b32_2x2MulDiv255U(a0, a0, a1, a1, Face::b32_1x1Negate255(ba));
    Face::b32_2x2AddusB32_2x2(dst0, dst0, a0, dst1, dst1, a1);
  }

  static FOG_INLINE void prgb32_op_xrgb32_32b(
    Face::b32_1x2& dst0, Face::b32_1x2 a0, Face::b32_1x2 b0,
    Face::b32_1x2& dst1, Face::b32_1x2 a1, Face::b32_1x2 b1)
  {
    uint32_t aa;
    Face::b32_1x2ExpandB1(aa, a1);
    Face::b32_2x2SubB32_2x2(dst0, aa, a0, dst1, aa, a1);

    Face::b32_1x2FillB1(dst1, dst1);
  }

  static FOG_INLINE void xrgb32_op_prgb32_32b(
    Face::b32_1x2& dst0, Face::b32_1x2 a0, Face::b32_1x2 b0,
    Face::b32_1x2& dst1, Face::b32_1x2 a1, Face::b32_1x2 b1)
  {
    Face::b32_2x2Negate(dst0, a0, dst1, a1);

    uint32_t ba = Face::b32_1x2GetB1(b1);
    Face::b32_2x2MulDiv255U(dst0, dst0, dst1, dst1, ba);

    Face::b32_2x2MulDiv255U(a0, a0, a1, a1, Face::b32_1x1Negate255(ba));
    Face::b32_2x2AddusB32_2x2(dst0, dst0, a0, dst1, dst1, a1);

    Face::b32_1x2FillB1(dst1, dst1);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_32b(
    Face::b32_1x2& dst0, Face::b32_1x2 a0, Face::b32_1x2 b0,
    Face::b32_1x2& dst1, Face::b32_1x2 a1, Face::b32_1x2 b1)
  {
    Face::b32_2x2Negate(dst0, a0, dst1, a1);
    Face::b32_1x2FillB1(dst1, dst1);
  }
};

} // RasterEngine namespace
} // Fog namespace
