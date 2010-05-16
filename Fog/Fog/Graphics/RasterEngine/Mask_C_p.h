// [Fog-Graphics Library - Private API]
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
// [Fog::RasterEngine::C - Mask - MaskReplaceC]
// ============================================================================

//! @internal
struct FOG_HIDDEN MaskReplaceC
{
  static void FOG_FASTCALL a8_c_op_v(uint8_t* dst, uint32_t a0, const uint8_t* b, int w)
  {
    FOG_UNUSED(a0);
    return a8_v_op_v(dst, NULL, b, w);
  }

  static void FOG_FASTCALL a8_v_op_c(uint8_t* dst, const uint8_t* a, uint32_t b0, int w)
  {
    FOG_ASSERT(w > 0);

    FOG_UNUSED(a);

    C_LOOP_8x4_INIT()

    C_LOOP_8x4_SMALL_BEGIN(bltCopy)
      dst[0] = (uint8_t)b0;
      dst += 1;
    C_LOOP_8x4_SMALL_END(bltCopy)

    b0 = RasterUtil::extendMask8(b0);

    C_LOOP_8x4_MAIN_BEGIN(bltCopy)
      ((uint32_t*)dst)[0] = b0;
      dst += 4;
    C_LOOP_8x4_MAIN_END(bltCopy)
  }

  static void FOG_FASTCALL a8_v_op_v(uint8_t* dst, const uint8_t* a, const uint8_t* b, int w)
  {
    FOG_ASSERT(w > 0);

    FOG_UNUSED(a);

    C_LOOP_8x4_INIT()

    C_LOOP_8x4_SMALL_BEGIN(bltCopy)
      dst[0] = READ_8(b);

      dst += 1;
      b += 1;
    C_LOOP_8x4_SMALL_END(bltCopy)

    C_LOOP_8x4_MAIN_BEGIN(bltCopy)
      ((uint32_t*)dst)[0] = READ_32(b);

      dst += 4;
      b += 4;
    C_LOOP_8x4_MAIN_END(bltCopy)
  }
};

// ============================================================================
// [Fog::RasterEngine::C - Mask - Intersect]
// ============================================================================

struct FOG_HIDDEN MaskIntersectC
{
  static void FOG_FASTCALL a8_c_op_v(uint8_t* dst, uint32_t a0, const uint8_t* b, int w)
  {
    a8_v_op_c(dst, b, a0, w);
  }

  static void FOG_FASTCALL a8_v_op_c(uint8_t* dst, const uint8_t* a, uint32_t b0, int w)
  {
    FOG_ASSERT(w > 0);

    if (b0 == 0x00)
    {
      C_LOOP_8x4_INIT()

      C_LOOP_8x4_SMALL_BEGIN(bltCopy)
        dst[0] = (uint8_t)b0;
        dst += 1;
      C_LOOP_8x4_SMALL_END(bltCopy)

      C_LOOP_8x4_MAIN_BEGIN(bltCopy)
        ((uint32_t*)dst)[0] = b0;
        dst += 4;
      C_LOOP_8x4_MAIN_END(bltCopy)
    }
    else
    {
      C_LOOP_8x4_INIT()

      C_LOOP_8x4_SMALL_BEGIN(bltOp)
        dst[0] = (uint8_t)ByteUtil::scalar_muldiv255(READ_8(a), b0);

        dst += 1;
        a += 1;
      C_LOOP_8x4_SMALL_END(bltOp)

      C_LOOP_8x4_MAIN_BEGIN(bltOp)
        uint32_t a0;
        uint32_t a1;

        ByteUtil::byte2x2_unpack_0213(a0, a1, READ_32(a));
        ByteUtil::byte2x2_muldiv255_u(a0, a0, b0, a1, a1, b0);
        ByteUtil::byte2x2_pack_0213(a0, a0, a1);

        ((uint32_t*)dst)[0] = a0;

        dst += 4;
        a += 4;
      C_LOOP_8x4_MAIN_END(bltOp)
    }
  }

  static void FOG_FASTCALL a8_v_op_v(uint8_t* dst, const uint8_t* a, const uint8_t* b, int w)
  {
    FOG_ASSERT(w > 0);

    C_LOOP_8x4_INIT()

    C_LOOP_8x4_SMALL_BEGIN(bltOp)
      dst[0] = (uint8_t)ByteUtil::scalar_muldiv255(READ_8(a), READ_8(b));

      dst += 1;
      a += 1;
      b += 1;
    C_LOOP_8x4_SMALL_END(bltOp)

    C_LOOP_8x4_MAIN_BEGIN(bltOp)
      uint32_t a0, a1;
      uint32_t b0, b1;

      ByteUtil::byte2x2_unpack_0213(a0, a1, READ_32(a));
      ByteUtil::byte2x2_unpack_0213(b0, b1, READ_32(b));
      ByteUtil::byte2x2_muldiv255_byte2x2(a0, a0, b0, a1, a1, b1);
      ByteUtil::byte2x2_pack_0213(a0, a0, a1);

      ((uint32_t*)dst)[0] = a0;

      dst += 4;
      a += 4;
      b += 4;
    C_LOOP_8x4_MAIN_END(bltOp)
  }
};

} // RasterEngine namespace
} // Fog namespace
