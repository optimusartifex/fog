// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// For some IDEs to enable code-assist.
#include <Fog/Build/Build.h>

#if defined(FOG_IDE)
#include <Fog/Graphics/RasterEngine/Defs_C_p.h>
#include <Fog/Graphics/RasterEngine/Dib_C_p.h>
#endif // FOG_IDE

namespace Fog {
namespace RasterEngine {

// ============================================================================
// [Fog::RasterEngine::C - Composite - NOP]
// ============================================================================

struct FOG_HIDDEN CompositeNopC
{
  static void FOG_FASTCALL cspan(
    uint8_t* dst, const Solid* src, sysint_t w, const Closure* closure)
  {
    FOG_UNUSED(dst);
    FOG_UNUSED(src);
    FOG_UNUSED(w);
    FOG_UNUSED(closure);
  }

  static void FOG_FASTCALL cspan_a8(
    uint8_t* dst, const Solid* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    FOG_UNUSED(dst);
    FOG_UNUSED(src);
    FOG_UNUSED(msk);
    FOG_UNUSED(w);
    FOG_UNUSED(closure);
  }

  static void FOG_FASTCALL cspan_a8_const(
    uint8_t* dst, const Solid* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    FOG_UNUSED(dst);
    FOG_UNUSED(src);
    FOG_UNUSED(msk0);
    FOG_UNUSED(w);
    FOG_UNUSED(closure);
  }

  static void FOG_FASTCALL cspan_a8_scanline(
    uint8_t* dst, const Solid* src, const Scanline32::Span* spans, sysuint_t numSpans, const Closure* closure)
  {
    FOG_UNUSED(dst);
    FOG_UNUSED(src);
    FOG_UNUSED(spans);
    FOG_UNUSED(numSpans);
    FOG_UNUSED(closure);
  }
};

// ============================================================================
// [Fog::RasterEngine::C - Base Macros and Generic Implementation]
// ============================================================================

template<int CHARACTERISTICS>
struct CompositeHelpersC32
{
  static FOG_INLINE bool processDstPixel(uint32_t c0)
  {
    if (CHARACTERISTICS & OPERATOR_CHAR_NOP_IF_DST_A_ZERO)
      return !ArgbUtil::isAlpha0x00(c0);
    else if (CHARACTERISTICS & OPERATOR_CHAR_NOP_IF_DST_A_FULL)
      return !ArgbUtil::isAlpha0xFF(c0);
    else
      return true;
  }

  static FOG_INLINE bool processSrcPixel(uint32_t c0)
  {
    if (CHARACTERISTICS & OPERATOR_CHAR_NOP_IF_SRC_A_ZERO)
      return !ArgbUtil::isAlpha0x00(c0);
    else if (CHARACTERISTICS & OPERATOR_CHAR_NOP_IF_SRC_A_FULL)
      return !ArgbUtil::isAlpha0xFF(c0);
    else
      return true;
  }
};

template<typename OP>
struct CompositeBaseFuncsC32
{
  // -------------------------------------------------------------------------
  // [CompositeBaseFuncsC32 - PRGB32]
  // -------------------------------------------------------------------------

  static void FOG_FASTCALL prgb32_cspan(
    uint8_t* dst, const Solid* src, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    ByteUtil::byte1x2 src0, src1;
    ByteUtil::byte2x2_unpack_0213(src0, src1, src->prgb);

    do {
      ByteUtil::byte1x2 dst0, dst1;

      if (OP::CHARACTERISTICS & (OPERATOR_CHAR_NOP_IF_DST_A_ZERO | OPERATOR_CHAR_NOP_IF_DST_A_FULL))
      {
        dst0 = READ_32(dst);
        if (CompositeHelpersC32<OP::CHARACTERISTICS>::processDstPixel(dst0))
        {
          ByteUtil::byte2x2_unpack_0213(dst0, dst1, dst0);

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }
      }
      else
      {
        ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));

        OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

        ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
        ((uint32_t*)dst)[0] = dst0;
      }

      dst += 4;
    } while (--i);
  }

  static void FOG_FASTCALL prgb32_cspan_a8(
    uint8_t* dst, const Solid* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    ByteUtil::byte1x2 src0orig, src1orig;
    ByteUtil::byte2x2_unpack_0213(src0orig, src1orig, src->prgb);

    if (OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND)
    {
      do {
        ByteUtil::byte1x2 dst0, dst1;
        ByteUtil::byte1x2 dst0inv, dst1inv;
        uint32_t msk0;

        msk0 = READ_8(msk);
        dst0 = READ_32(dst);

        if (msk0 != 0x00)
        {
          ByteUtil::byte2x2_unpack_0213(dst0, dst1, dst0);
          ByteUtil::byte2x2_muldiv255_u(dst0inv, dst0, dst1inv, dst1, 255 - msk0);
          ByteUtil::byte2x2_pack_0213(dst0inv, dst0inv, dst1inv);

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0orig, dst1, dst1, src1orig);

          ByteUtil::byte2x2_muldiv255_u(dst0, dst0, dst1, dst1, msk0);
          ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }

        dst += 4;
        msk += 1;
      } while (--i);
    }
    else
    {
      do {
        ByteUtil::byte1x2 dst0, dst1;
        ByteUtil::byte1x2 src0, src1;
        uint32_t msk0;

        msk0 = READ_8(msk);
        dst0 = READ_32(dst);

        if (OP::CHARACTERISTICS & (OPERATOR_CHAR_NOP_IF_DST_A_ZERO | OPERATOR_CHAR_NOP_IF_DST_A_FULL))
        {
          if (msk0 != 0x00 && CompositeHelpersC32<OP::CHARACTERISTICS>::processDstPixel(dst0))
          {
            ByteUtil::byte2x2_unpack_0213(dst0, dst1, dst0);
            ByteUtil::byte2x2_muldiv255_u(src0, src0orig, src1, src1orig, msk0);

            OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

            ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
            ((uint32_t*)dst)[0] = dst0;
          }
        }
        else
        {
          if (msk0 != 0x00)
          {
            ByteUtil::byte2x2_unpack_0213(dst0, dst1, dst0);
            ByteUtil::byte2x2_muldiv255_u(src0, src0orig, src1, src1orig, msk0);

            OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

            ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
            ((uint32_t*)dst)[0] = dst0;
          }
        }

        dst += 4;
        msk += 1;
      } while (--i);
    }
  }

  static void FOG_FASTCALL prgb32_cspan_a8_const(
    uint8_t* dst, const Solid* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    ByteUtil::byte1x2 src0orig, src1orig;
    ByteUtil::byte2x2_unpack_0213(src0orig, src1orig, src->prgb);

    if (OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND)
    {
      if (OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND_MSK_IN)
      {
        ByteUtil::byte1x2 src0, src1;
        ByteUtil::byte2x2_muldiv255_u(src0, src0orig, src1, src1orig, msk0);
        msk0 = 255 - msk0;

        do {
          ByteUtil::byte1x2 dst0, dst1;
          ByteUtil::byte1x2 dst0inv, dst1inv;

          dst0 = READ_32(dst);

          ByteUtil::byte2x2_unpack_0213(dst0, dst1, dst0);
          ByteUtil::byte2x2_muldiv255_u(dst0inv, dst0, dst1inv, dst1, msk0);
          ByteUtil::byte2x2_pack_0213(dst0inv, dst0inv, dst1inv);

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;

          dst += 4;
        } while (--i);
      }
      else
      {
        do {
          ByteUtil::byte1x2 dst0, dst1;
          ByteUtil::byte1x2 dst0inv, dst1inv;

          dst0 = READ_32(dst);

          ByteUtil::byte2x2_unpack_0213(dst0, dst1, dst0);
          ByteUtil::byte2x2_muldiv255_u(dst0inv, dst0, dst1inv, dst1, 255 - msk0);
          ByteUtil::byte2x2_pack_0213(dst0inv, dst0inv, dst1inv);

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0orig, dst1, dst1, src1orig);

          ByteUtil::byte2x2_muldiv255_u(dst0, dst0, dst1, dst1, msk0);
          ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;

          dst += 4;
        } while (--i);
      }
    }
    else
    {
      ByteUtil::byte1x2 src0, src1;
      ByteUtil::byte2x2_muldiv255_u(src0, src0orig, src1, src1orig, msk0);

      do {
        ByteUtil::byte1x2 dst0, dst1;

        dst0 = READ_32(dst);

        if (OP::CHARACTERISTICS & (OPERATOR_CHAR_NOP_IF_DST_A_ZERO | OPERATOR_CHAR_NOP_IF_DST_A_FULL))
        {
          if (CompositeHelpersC32<OP::CHARACTERISTICS>::processDstPixel(dst0))
          {
            ByteUtil::byte2x2_unpack_0213(dst0, dst1, dst0);

            OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

            ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
            ((uint32_t*)dst)[0] = dst0;
          }
        }
        else
        {
          ByteUtil::byte2x2_unpack_0213(dst0, dst1, dst0);

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }

        dst += 4;
      } while (--i);
    }
  }

  static void FOG_FASTCALL prgb32_cspan_a8_scanline(
    uint8_t* dst, const Solid* src, const Scanline32::Span* spans, sysuint_t numSpans, const Closure* closure)
  {
    BLIT_CSPAN_SCANLINE_STEP1_BEGIN(4)

    ByteUtil::byte1x2 src0orig, src1orig;
    ByteUtil::byte2x2_unpack_0213(src0orig, src1orig, src->prgb);

    // Const mask.
    BLIT_CSPAN_SCANLINE_STEP2_CONST()
    {
      sysint_t i = w;
      FOG_ASSERT(w);

      ByteUtil::byte1x2 src0orig, src1orig;
      ByteUtil::byte2x2_unpack_0213(src0orig, src1orig, src->prgb);

      if (OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND)
      {
        if (OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND_MSK_IN)
        {
          ByteUtil::byte1x2 src0, src1;
          ByteUtil::byte2x2_muldiv255_u(src0, src0orig, src1, src1orig, msk0);
          msk0 = 255 - msk0;

          do {
            ByteUtil::byte1x2 dst0, dst1;
            ByteUtil::byte1x2 dst0inv, dst1inv;

            dst0 = READ_32(dst);

            ByteUtil::byte2x2_unpack_0213(dst0, dst1, dst0);
            ByteUtil::byte2x2_muldiv255_u(dst0inv, dst0, dst1inv, dst1, msk0);
            ByteUtil::byte2x2_pack_0213(dst0inv, dst0inv, dst1inv);

            OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

            ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
            ((uint32_t*)dst)[0] = dst0 + dst0inv;

            dst += 4;
          } while (--i);
        }
        else
        {
          do {
            ByteUtil::byte1x2 dst0, dst1;
            ByteUtil::byte1x2 dst0inv, dst1inv;

            dst0 = READ_32(dst);

            ByteUtil::byte2x2_unpack_0213(dst0, dst1, dst0);
            ByteUtil::byte2x2_muldiv255_u(dst0inv, dst0, dst1inv, dst1, 255 - msk0);
            ByteUtil::byte2x2_pack_0213(dst0inv, dst0inv, dst1inv);

            OP::prgb32_op_prgb32_32b(dst0, dst0, src0orig, dst1, dst1, src1orig);

            ByteUtil::byte2x2_muldiv255_u(dst0, dst0, dst1, dst1, msk0);
            ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
            ((uint32_t*)dst)[0] = dst0 + dst0inv;

            dst += 4;
          } while (--i);
        }
      }
      else
      {
        ByteUtil::byte1x2 src0, src1;
        ByteUtil::byte2x2_muldiv255_u(src0, src0orig, src1, src1orig, msk0);

        do {
          ByteUtil::byte1x2 dst0, dst1;

          dst0 = READ_32(dst);

          if (OP::CHARACTERISTICS & (OPERATOR_CHAR_NOP_IF_DST_A_ZERO | OPERATOR_CHAR_NOP_IF_DST_A_FULL))
          {
            if (CompositeHelpersC32<OP::CHARACTERISTICS>::processDstPixel(dst0))
            {
              ByteUtil::byte2x2_unpack_0213(dst0, dst1, dst0);

              OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

              ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
              ((uint32_t*)dst)[0] = dst0;
            }
          }
          else
          {
            ByteUtil::byte2x2_unpack_0213(dst0, dst1, dst0);

            OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

            ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
            ((uint32_t*)dst)[0] = dst0;
          }

          dst += 4;
        } while (--i);
      }
    }
    // Variable mask.
    BLIT_CSPAN_SCANLINE_STEP3_MASK()
    {
      sysint_t i = w;
      FOG_ASSERT(w);

      if (OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND)
      {
        do {
          ByteUtil::byte1x2 dst0, dst1;
          ByteUtil::byte1x2 dst0inv, dst1inv;
          uint32_t msk0;

          msk0 = READ_8(msk);
          dst0 = READ_32(dst);

          if (msk0 != 0x00)
          {
            ByteUtil::byte2x2_unpack_0213(dst0, dst1, dst0);
            ByteUtil::byte2x2_muldiv255_u(dst0inv, dst0, dst1inv, dst1, 255 - msk0);
            ByteUtil::byte2x2_pack_0213(dst0inv, dst0inv, dst1inv);

            OP::prgb32_op_prgb32_32b(dst0, dst0, src0orig, dst1, dst1, src1orig);

            ByteUtil::byte2x2_muldiv255_u(dst0, dst0, dst1, dst1, msk0);
            ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
            ((uint32_t*)dst)[0] = dst0 + dst0inv;
          }

          dst += 4;
          msk += 1;
        } while (--i);
      }
      else
      {
        do {
          ByteUtil::byte1x2 dst0, dst1;
          ByteUtil::byte1x2 src0, src1;
          uint32_t msk0;

          msk0 = READ_8(msk);
          dst0 = READ_32(dst);

          if (OP::CHARACTERISTICS & (OPERATOR_CHAR_NOP_IF_DST_A_ZERO | OPERATOR_CHAR_NOP_IF_DST_A_FULL))
          {
            if (msk0 != 0x00 && CompositeHelpersC32<OP::CHARACTERISTICS>::processDstPixel(dst0))
            {
              ByteUtil::byte2x2_unpack_0213(dst0, dst1, dst0);
              ByteUtil::byte2x2_muldiv255_u(src0, src0orig, src1, src1orig, msk0);

              OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

              ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
              ((uint32_t*)dst)[0] = dst0;
            }
          }
          else
          {
            if (msk0 != 0x00)
            {
              ByteUtil::byte2x2_unpack_0213(dst0, dst1, dst0);
              ByteUtil::byte2x2_muldiv255_u(src0, src0orig, src1, src1orig, msk0);

              OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

              ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
              ((uint32_t*)dst)[0] = dst0;
            }
          }

          dst += 4;
          msk += 1;
        } while (--i);
      }
    }
    BLIT_CSPAN_SCANLINE_STEP4_END()
  }

  static void FOG_FASTCALL prgb32_vspan_prgb32(
    uint8_t* dst, const uint8_t* src, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    do {
      ByteUtil::byte1x2 src0, src1;
      ByteUtil::byte1x2 dst0, dst1;

      if (OP::CHARACTERISTICS & (OPERATOR_CHAR_NOP_IF_DST_A_ZERO | OPERATOR_CHAR_NOP_IF_DST_A_FULL))
      {
        dst0 = READ_32(dst);
        if (CompositeHelpersC32<OP::CHARACTERISTICS>::processDstPixel(dst0))
        {
          ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src));
          ByteUtil::byte2x2_unpack_0213(dst0, dst1, dst0);

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }
      }
      else if (OP::CHARACTERISTICS & (OPERATOR_CHAR_NOP_IF_SRC_A_ZERO | OPERATOR_CHAR_NOP_IF_SRC_A_FULL))
      {
        src0 = READ_32(src);
        if (CompositeHelpersC32<OP::CHARACTERISTICS>::processSrcPixel(src0))
        {
          ByteUtil::byte2x2_unpack_0213(src0, src1, src0);
          ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }
      }
      else
      {
        ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));
        ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src));

        OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

        ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
        ((uint32_t*)dst)[0] = dst0;
      }

      dst += 4;
      src += 4;
    } while (--i);
  }

  static void FOG_FASTCALL prgb32_vspan_argb32(
    uint8_t* dst, const uint8_t* src, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    do {
      ByteUtil::byte1x2 src0, src1;
      ByteUtil::byte1x2 dst0, dst1;

      if (OP::CHARACTERISTICS & (OPERATOR_CHAR_NOP_IF_DST_A_ZERO | OPERATOR_CHAR_NOP_IF_DST_A_FULL))
      {
        dst0 = READ_32(dst);
        if (CompositeHelpersC32<OP::CHARACTERISTICS>::processDstPixel(dst0))
        {
          ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src));
          ByteUtil::byte2x2_unpack_0213(dst0, dst1, dst0);

          // Premultiply only if source colors are used.
          if (OP::CHARACTERISTICS & OPERATOR_CHAR_SRC_C_USED) ByteUtil::byte2x2_premultiply(src0, src0, src1, src1);
          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }
      }
      else if (OP::CHARACTERISTICS & (OPERATOR_CHAR_NOP_IF_SRC_A_ZERO | OPERATOR_CHAR_NOP_IF_SRC_A_FULL))
      {
        src0 = READ_32(src);
        if (CompositeHelpersC32<OP::CHARACTERISTICS>::processSrcPixel(src0))
        {
          ByteUtil::byte2x2_unpack_0213(src0, src1, src0);
          ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));

          // Premultiply only if source colors are used.
          if (OP::CHARACTERISTICS & OPERATOR_CHAR_SRC_C_USED) ByteUtil::byte2x2_premultiply(src0, src0, src1, src1);
          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }
      }
      else
      {
        ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));
        ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src));

        // Premultiply only if source colors are used.
        if (OP::CHARACTERISTICS & OPERATOR_CHAR_SRC_C_USED) ByteUtil::byte2x2_premultiply(src0, src0, src1, src1);
        OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

        ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
        ((uint32_t*)dst)[0] = dst0;
      }

      dst += 4;
      src += 4;
    } while (--i);
  }

  static void FOG_FASTCALL prgb32_vspan_xrgb32(
    uint8_t* dst, const uint8_t* src, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    do {
      ByteUtil::byte1x2 src0, src1;
      ByteUtil::byte1x2 dst0, dst1;

      if (OP::CHARACTERISTICS & (OPERATOR_CHAR_NOP_IF_DST_A_ZERO | OPERATOR_CHAR_NOP_IF_DST_A_FULL))
      {
        dst0 = READ_32(dst);
        if (CompositeHelpersC32<OP::CHARACTERISTICS>::processDstPixel(dst0))
        {
          ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src));
          ByteUtil::byte2x2_unpack_0213(dst0, dst1, dst0);

          OP::prgb32_op_xrgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }
      }
      else
      {
        ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));
        ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src));

        OP::prgb32_op_xrgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

        ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
        ((uint32_t*)dst)[0] = dst0;
      }

      dst += 4;
      src += 4;
    } while (--i);
  }

  static void FOG_FASTCALL prgb32_vspan_prgb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    do {
      ByteUtil::byte1x2 src0, src1;
      ByteUtil::byte1x2 dst0, dst1;
      uint32_t msk0 = READ_8(msk);

      if (msk0 != 0x00)
      {
        if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND) && OP::CHARACTERISTICS & (OPERATOR_CHAR_UNBOUND_MSK_IN))
        {
          ByteUtil::byte1x2 dst0inv, dst1inv;

          ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));
          ByteUtil::byte2x2_muldiv255_u(dst0inv, dst0, dst1inv, dst1, ByteUtil::scalar_neg255(msk0));
          ByteUtil::byte2x2_pack_0213(dst0inv, dst0inv, dst1inv);

          ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src));
          ByteUtil::byte2x2_muldiv255_u(src0, src0, src1, src1, msk0);

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
        {
          ByteUtil::byte1x2 dst0inv, dst1inv;

          ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));
          ByteUtil::byte2x2_muldiv255_u(dst0inv, dst0, dst1inv, dst1, ByteUtil::scalar_neg255(msk0));
          ByteUtil::byte2x2_pack_0213(dst0inv, dst0inv, dst1inv);

          ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src));

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);
          ByteUtil::byte2x2_muldiv255_u(dst0, dst0, dst1, dst1, msk0);
          ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else
        {
          ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));
          ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src));

          ByteUtil::byte2x2_muldiv255_u(src0, src0, msk0, src1, src1, msk0);

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }
      }

      dst += 4;
      src += 4;
      msk += 1;
    } while (--i);
  }

  static void FOG_FASTCALL prgb32_vspan_argb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    do {
      ByteUtil::byte1x2 src0, src1;
      ByteUtil::byte1x2 dst0, dst1;
      uint32_t msk0 = READ_8(msk);

      if (msk0 != 0x00)
      {
        if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND) && OP::CHARACTERISTICS & (OPERATOR_CHAR_UNBOUND_MSK_IN))
        {
          ByteUtil::byte1x2 dst0inv, dst1inv;

          ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));
          ByteUtil::byte2x2_muldiv255_u(dst0inv, dst0, dst1inv, dst1, ByteUtil::scalar_neg255(msk0));
          ByteUtil::byte2x2_pack_0213(dst0inv, dst0inv, dst1inv);

          ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src));
          ByteUtil::byte2x2_premultiply_by(src0, src0, src1, src1, ByteUtil::scalar_muldiv255(ByteUtil::byte1x2_hi(src1), msk0));

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
        {
          ByteUtil::byte1x2 dst0inv, dst1inv;

          ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));
          ByteUtil::byte2x2_muldiv255_u(dst0inv, dst0, dst1inv, dst1, ByteUtil::scalar_neg255(msk0));
          ByteUtil::byte2x2_pack_0213(dst0inv, dst0inv, dst1inv);

          ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src));
          ByteUtil::byte2x2_premultiply(src0, src0, src1, src1);

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);
          ByteUtil::byte2x2_muldiv255_u(dst0, dst0, dst1, dst1, msk0);
          ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else
        {
          ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));
          ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src));
          ByteUtil::byte2x2_premultiply_by(src0, src0, src1, src1, ByteUtil::scalar_muldiv255(ByteUtil::byte1x2_hi(src1), msk0));

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }
      }

      dst += 4;
      src += 4;
      msk += 1;
    } while (--i);
  }

  static void FOG_FASTCALL prgb32_vspan_xrgb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    do {
      ByteUtil::byte1x2 src0, src1;
      ByteUtil::byte1x2 dst0, dst1;
      uint32_t msk0 = READ_8(msk);

      if (msk0 != 0x00)
      {
        if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND) && OP::CHARACTERISTICS & (OPERATOR_CHAR_UNBOUND_MSK_IN))
        {
          ByteUtil::byte1x2 dst0inv, dst1inv;

          ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));
          ByteUtil::byte2x2_muldiv255_u(dst0inv, dst0, dst1inv, dst1, ByteUtil::scalar_neg255(msk0));
          ByteUtil::byte2x2_pack_0213(dst0inv, dst0inv, dst1inv);

          ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src) | 0xFF000000);
          ByteUtil::byte2x2_muldiv255_u(src0, src0, src1, src1, msk0);

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
        {
          ByteUtil::byte1x2 dst0inv, dst1inv;

          ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));
          ByteUtil::byte2x2_muldiv255_u(dst0inv, dst0, dst1inv, dst1, ByteUtil::scalar_neg255(msk0));
          ByteUtil::byte2x2_pack_0213(dst0inv, dst0inv, dst1inv);

          ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src));

          OP::prgb32_op_xrgb32_32b(dst0, dst0, src0, dst1, dst1, src1);
          ByteUtil::byte2x2_muldiv255_u(dst0, dst0, dst1, dst1, msk0);
          ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else
        {
          ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));
          ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src) | 0xFF000000);

          ByteUtil::byte2x2_muldiv255_u(src0, src0, msk0, src1, src1, msk0);

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }
      }

      dst += 4;
      src += 4;
      msk += 1;
    } while (--i);
  }

  static void FOG_FASTCALL prgb32_vspan_prgb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    uint32_t msk0inv;
    if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND)) msk0inv = ByteUtil::scalar_neg255(msk0);

    do {
      ByteUtil::byte1x2 src0, src1;
      ByteUtil::byte1x2 dst0, dst1;

      if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND) && OP::CHARACTERISTICS & (OPERATOR_CHAR_UNBOUND_MSK_IN))
      {
        ByteUtil::byte1x2 dst0inv, dst1inv;

        ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));
        ByteUtil::byte2x2_muldiv255_u(dst0inv, dst0, dst1inv, dst1, msk0inv);
        ByteUtil::byte2x2_pack_0213(dst0inv, dst0inv, dst1inv);

        ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src));
        ByteUtil::byte2x2_muldiv255_u(src0, src0, src1, src1, msk0);

        OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

        ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
        ((uint32_t*)dst)[0] = dst0 + dst0inv;
      }
      else if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
      {
        ByteUtil::byte1x2 dst0inv, dst1inv;

        ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));
        ByteUtil::byte2x2_muldiv255_u(dst0inv, dst0, dst1inv, dst1, msk0inv);
        ByteUtil::byte2x2_pack_0213(dst0inv, dst0inv, dst1inv);

        ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src));

        OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);
        ByteUtil::byte2x2_muldiv255_u(dst0, dst0, dst1, dst1, msk0);
        ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
        ((uint32_t*)dst)[0] = dst0 + dst0inv;
      }
      else
      {
        ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));
        ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src));

        ByteUtil::byte2x2_muldiv255_u(src0, src0, msk0, src1, src1, msk0);

        OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

        ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
        ((uint32_t*)dst)[0] = dst0;
      }

      dst += 4;
      src += 4;
    } while (--i);
  }

  static void FOG_FASTCALL prgb32_vspan_argb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    uint32_t msk0inv;
    if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND)) msk0inv = ByteUtil::scalar_neg255(msk0);

    do {
      ByteUtil::byte1x2 src0, src1;
      ByteUtil::byte1x2 dst0, dst1;

      if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND) && OP::CHARACTERISTICS & (OPERATOR_CHAR_UNBOUND_MSK_IN))
      {
        ByteUtil::byte1x2 dst0inv, dst1inv;

        ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));
        ByteUtil::byte2x2_muldiv255_u(dst0inv, dst0, dst1inv, dst1, msk0inv);
        ByteUtil::byte2x2_pack_0213(dst0inv, dst0inv, dst1inv);

        ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src));
        ByteUtil::byte2x2_premultiply_by(src0, src0, src1, src1, ByteUtil::scalar_muldiv255(ByteUtil::byte1x2_hi(src1), msk0));

        OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

        ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
        ((uint32_t*)dst)[0] = dst0 + dst0inv;
      }
      else if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
      {
        ByteUtil::byte1x2 dst0inv, dst1inv;

        ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));
        ByteUtil::byte2x2_muldiv255_u(dst0inv, dst0, dst1inv, dst1, msk0inv);
        ByteUtil::byte2x2_pack_0213(dst0inv, dst0inv, dst1inv);

        ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src));
        ByteUtil::byte2x2_premultiply(src0, src0, src1, src1);

        OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);
        ByteUtil::byte2x2_muldiv255_u(dst0, dst0, dst1, dst1, msk0);
        ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
        ((uint32_t*)dst)[0] = dst0 + dst0inv;
      }
      else
      {
        ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));
        ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src));
        ByteUtil::byte2x2_premultiply_by(src0, src0, src1, src1, ByteUtil::scalar_muldiv255(ByteUtil::byte1x2_hi(src1), msk0));

        OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

        ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
        ((uint32_t*)dst)[0] = dst0;
      }

      dst += 4;
      src += 4;
    } while (--i);
  }

  static void FOG_FASTCALL prgb32_vspan_xrgb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    uint32_t msk0inv;
    if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND)) msk0inv = ByteUtil::scalar_neg255(msk0);

    do {
      ByteUtil::byte1x2 src0, src1;
      ByteUtil::byte1x2 dst0, dst1;

      if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND) && OP::CHARACTERISTICS & (OPERATOR_CHAR_UNBOUND_MSK_IN))
      {
        ByteUtil::byte1x2 dst0inv, dst1inv;

        ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));
        ByteUtil::byte2x2_muldiv255_u(dst0inv, dst0, dst1inv, dst1, msk0inv);
        ByteUtil::byte2x2_pack_0213(dst0inv, dst0inv, dst1inv);

        ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src) | 0xFF000000);
        ByteUtil::byte2x2_muldiv255_u(src0, src0, src1, src1, msk0);

        OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

        ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
        ((uint32_t*)dst)[0] = dst0 + dst0inv;
      }
      else if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
      {
        ByteUtil::byte1x2 dst0inv, dst1inv;

        ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));
        ByteUtil::byte2x2_muldiv255_u(dst0inv, dst0, dst1inv, dst1, msk0inv);
        ByteUtil::byte2x2_pack_0213(dst0inv, dst0inv, dst1inv);

        ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src));

        OP::prgb32_op_xrgb32_32b(dst0, dst0, src0, dst1, dst1, src1);
        ByteUtil::byte2x2_muldiv255_u(dst0, dst0, dst1, dst1, msk0);
        ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
        ((uint32_t*)dst)[0] = dst0 + dst0inv;
      }
      else
      {
        ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));
        ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src) | 0xFF000000);

        ByteUtil::byte2x2_muldiv255_u(src0, src0, msk0, src1, src1, msk0);

        OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

        ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
        ((uint32_t*)dst)[0] = dst0;
      }

      dst += 4;
      src += 4;
    } while (--i);
  }

  // -------------------------------------------------------------------------
  // [CompositeBaseFuncsC - XRGB32]
  // -------------------------------------------------------------------------

  static void FOG_FASTCALL xrgb32_cspan(
    uint8_t* dst, const Solid* src, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    ByteUtil::byte1x2 src0, src1;
    ByteUtil::byte2x2_unpack_0213(src0, src1, src->prgb);

    do {
      ByteUtil::byte1x2 dst0, dst1;

      ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));
      OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);
      ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
      ((uint32_t*)dst)[0] = dst0;

      dst += 4;
    } while (--i);
  }

  static void FOG_FASTCALL xrgb32_cspan_a8(
    uint8_t* dst, const Solid* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    ByteUtil::byte1x2 src0orig, src1orig;
    ByteUtil::byte2x2_unpack_0213(src0orig, src1orig, src->prgb);

    if (OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND)
    {
      do {
        ByteUtil::byte1x2 dst0, dst1;
        ByteUtil::byte1x2 dst0inv, dst1inv;
        uint32_t msk0;

        msk0 = READ_8(msk);
        dst0 = READ_32(dst);

        if (msk0 != 0x00)
        {
          ByteUtil::byte2x2_unpack_0213(dst0, dst1, dst0);
          ByteUtil::byte2x2_muldiv255_u(dst0inv, dst0, dst1inv, dst1, 255 - msk0);
          ByteUtil::byte2x2_pack_0213(dst0inv, dst0inv, dst1inv);

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0orig, dst1, dst1, src1orig);

          ByteUtil::byte2x2_muldiv255_u(dst0, dst0, dst1, dst1, msk0);
          ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }

        dst += 4;
        msk += 1;
      } while (--i);
    }
    else
    {
      do {
        ByteUtil::byte1x2 dst0, dst1;
        ByteUtil::byte1x2 src0, src1;
        uint32_t msk0;

        msk0 = READ_8(msk);
        dst0 = READ_32(dst);

        if (OP::CHARACTERISTICS & (OPERATOR_CHAR_NOP_IF_DST_A_ZERO | OPERATOR_CHAR_NOP_IF_DST_A_FULL))
        {
          if (msk0 != 0x00 && CompositeHelpersC32<OP::CHARACTERISTICS>::processDstPixel(dst0))
          {
            ByteUtil::byte2x2_unpack_0213(dst0, dst1, dst0);
            ByteUtil::byte2x2_muldiv255_u(src0, src0orig, src1, src1orig, msk0);

            OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

            ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
            ((uint32_t*)dst)[0] = dst0;
          }
        }
        else
        {
          if (msk0 != 0x00)
          {
            ByteUtil::byte2x2_unpack_0213(dst0, dst1, dst0);
            ByteUtil::byte2x2_muldiv255_u(src0, src0orig, src1, src1orig, msk0);

            OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

            ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
            ((uint32_t*)dst)[0] = dst0;
          }
        }

        dst += 4;
        msk += 1;
      } while (--i);
    }
  }

  static void FOG_FASTCALL xrgb32_cspan_a8_const(
    uint8_t* dst, const Solid* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    ByteUtil::byte1x2 src0orig, src1orig;
    ByteUtil::byte2x2_unpack_0213(src0orig, src1orig, src->prgb);

    if (OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND)
    {
      if (OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND_MSK_IN)
      {
        ByteUtil::byte1x2 src0, src1;
        ByteUtil::byte2x2_muldiv255_u(src0, src0orig, src1, src1orig, msk0);
        msk0 = 255 - msk0;

        do {
          ByteUtil::byte1x2 dst0, dst1;
          ByteUtil::byte1x2 dst0inv, dst1inv;

          dst0 = READ_32(dst);

          ByteUtil::byte2x2_unpack_0213(dst0, dst1, dst0);
          ByteUtil::byte2x2_muldiv255_u(dst0inv, dst0, dst1inv, dst1, msk0);
          ByteUtil::byte2x2_pack_0213(dst0inv, dst0inv, dst1inv);

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;

          dst += 4;
        } while (--i);
      }
      else
      {
        do {
          ByteUtil::byte1x2 dst0, dst1;
          ByteUtil::byte1x2 dst0inv, dst1inv;

          dst0 = READ_32(dst);

          ByteUtil::byte2x2_unpack_0213(dst0, dst1, dst0);
          ByteUtil::byte2x2_muldiv255_u(dst0inv, dst0, dst1inv, dst1, 255 - msk0);
          ByteUtil::byte2x2_pack_0213(dst0inv, dst0inv, dst1inv);

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0orig, dst1, dst1, src1orig);

          ByteUtil::byte2x2_muldiv255_u(dst0, dst0, dst1, dst1, msk0);
          ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;

          dst += 4;
        } while (--i);
      }
    }
    else
    {
      ByteUtil::byte1x2 src0, src1;
      ByteUtil::byte2x2_muldiv255_u(src0, src0orig, src1, src1orig, msk0);

      do {
        ByteUtil::byte1x2 dst0, dst1;

        dst0 = READ_32(dst);

        if (OP::CHARACTERISTICS & (OPERATOR_CHAR_NOP_IF_DST_A_ZERO | OPERATOR_CHAR_NOP_IF_DST_A_FULL))
        {
          if (CompositeHelpersC32<OP::CHARACTERISTICS>::processDstPixel(dst0))
          {
            ByteUtil::byte2x2_unpack_0213(dst0, dst1, dst0);

            OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

            ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
            ((uint32_t*)dst)[0] = dst0;
          }
        }
        else
        {
          ByteUtil::byte2x2_unpack_0213(dst0, dst1, dst0);

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }

        dst += 4;
      } while (--i);
    }
  }

  static void FOG_FASTCALL xrgb32_cspan_a8_scanline(
    uint8_t* dst, const Solid* src, const Scanline32::Span* spans, sysuint_t numSpans, const Closure* closure)
  {
    BLIT_CSPAN_SCANLINE_STEP1_BEGIN(4)

    ByteUtil::byte1x2 src0orig, src1orig;
    ByteUtil::byte2x2_unpack_0213(src0orig, src1orig, src->prgb);

    // Const mask.
    BLIT_CSPAN_SCANLINE_STEP2_CONST()
    {
      sysint_t i = w;
      FOG_ASSERT(w);

      ByteUtil::byte1x2 src0orig, src1orig;
      ByteUtil::byte2x2_unpack_0213(src0orig, src1orig, src->prgb);

      if (OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND)
      {
        if (OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND_MSK_IN)
        {
          ByteUtil::byte1x2 src0, src1;
          ByteUtil::byte2x2_muldiv255_u(src0, src0orig, src1, src1orig, msk0);
          msk0 = 255 - msk0;

          do {
            ByteUtil::byte1x2 dst0, dst1;
            ByteUtil::byte1x2 dst0inv, dst1inv;

            dst0 = READ_32(dst);

            ByteUtil::byte2x2_unpack_0213(dst0, dst1, dst0);
            ByteUtil::byte2x2_muldiv255_u(dst0inv, dst0, dst1inv, dst1, msk0);
            ByteUtil::byte2x2_pack_0213(dst0inv, dst0inv, dst1inv);

            OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

            ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
            ((uint32_t*)dst)[0] = dst0 + dst0inv;

            dst += 4;
          } while (--i);
        }
        else
        {
          do {
            ByteUtil::byte1x2 dst0, dst1;
            ByteUtil::byte1x2 dst0inv, dst1inv;

            dst0 = READ_32(dst);

            ByteUtil::byte2x2_unpack_0213(dst0, dst1, dst0);
            ByteUtil::byte2x2_muldiv255_u(dst0inv, dst0, dst1inv, dst1, 255 - msk0);
            ByteUtil::byte2x2_pack_0213(dst0inv, dst0inv, dst1inv);

            OP::xrgb32_op_prgb32_32b(dst0, dst0, src0orig, dst1, dst1, src1orig);

            ByteUtil::byte2x2_muldiv255_u(dst0, dst0, dst1, dst1, msk0);
            ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
            ((uint32_t*)dst)[0] = dst0 + dst0inv;

            dst += 4;
          } while (--i);
        }
      }
      else
      {
        ByteUtil::byte1x2 src0, src1;
        ByteUtil::byte2x2_muldiv255_u(src0, src0orig, src1, src1orig, msk0);

        do {
          ByteUtil::byte1x2 dst0, dst1;

          dst0 = READ_32(dst);

          if (OP::CHARACTERISTICS & (OPERATOR_CHAR_NOP_IF_DST_A_ZERO | OPERATOR_CHAR_NOP_IF_DST_A_FULL))
          {
            if (CompositeHelpersC32<OP::CHARACTERISTICS>::processDstPixel(dst0))
            {
              ByteUtil::byte2x2_unpack_0213(dst0, dst1, dst0);

              OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

              ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
              ((uint32_t*)dst)[0] = dst0;
            }
          }
          else
          {
            ByteUtil::byte2x2_unpack_0213(dst0, dst1, dst0);

            OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

            ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
            ((uint32_t*)dst)[0] = dst0;
          }

          dst += 4;
        } while (--i);
      }
    }
    // Variable mask.
    BLIT_CSPAN_SCANLINE_STEP3_MASK()
    {
      sysint_t i = w;
      FOG_ASSERT(w);

      if (OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND)
      {
        do {
          ByteUtil::byte1x2 dst0, dst1;
          ByteUtil::byte1x2 dst0inv, dst1inv;
          uint32_t msk0;

          msk0 = READ_8(msk);
          dst0 = READ_32(dst);

          if (msk0 != 0x00)
          {
            ByteUtil::byte2x2_unpack_0213(dst0, dst1, dst0);
            ByteUtil::byte2x2_muldiv255_u(dst0inv, dst0, dst1inv, dst1, 255 - msk0);
            ByteUtil::byte2x2_pack_0213(dst0inv, dst0inv, dst1inv);

            OP::xrgb32_op_prgb32_32b(dst0, dst0, src0orig, dst1, dst1, src1orig);

            ByteUtil::byte2x2_muldiv255_u(dst0, dst0, dst1, dst1, msk0);
            ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
            ((uint32_t*)dst)[0] = dst0 + dst0inv;
          }

          dst += 4;
          msk += 1;
        } while (--i);
      }
      else
      {
        do {
          ByteUtil::byte1x2 dst0, dst1;
          ByteUtil::byte1x2 src0, src1;
          uint32_t msk0;

          msk0 = READ_8(msk);
          dst0 = READ_32(dst);

          if (OP::CHARACTERISTICS & (OPERATOR_CHAR_NOP_IF_DST_A_ZERO | OPERATOR_CHAR_NOP_IF_DST_A_FULL))
          {
            if (msk0 != 0x00 && CompositeHelpersC32<OP::CHARACTERISTICS>::processDstPixel(dst0))
            {
              ByteUtil::byte2x2_unpack_0213(dst0, dst1, dst0);
              ByteUtil::byte2x2_muldiv255_u(src0, src0orig, src1, src1orig, msk0);

              OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

              ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
              ((uint32_t*)dst)[0] = dst0;
            }
          }
          else
          {
            if (msk0 != 0x00)
            {
              ByteUtil::byte2x2_unpack_0213(dst0, dst1, dst0);
              ByteUtil::byte2x2_muldiv255_u(src0, src0orig, src1, src1orig, msk0);

              OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

              ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
              ((uint32_t*)dst)[0] = dst0;
            }
          }

          dst += 4;
          msk += 1;
        } while (--i);
      }
    }
    BLIT_CSPAN_SCANLINE_STEP4_END()
  }

  static void FOG_FASTCALL xrgb32_vspan_prgb32(
    uint8_t* dst, const uint8_t* src, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    do {
      ByteUtil::byte1x2 src0, src1;
      ByteUtil::byte1x2 dst0, dst1;

      if (OP::CHARACTERISTICS & (OPERATOR_CHAR_NOP_IF_SRC_A_ZERO | OPERATOR_CHAR_NOP_IF_SRC_A_FULL))
      {
        src0 = READ_32(src);
        if (CompositeHelpersC32<OP::CHARACTERISTICS>::processSrcPixel(src0))
        {
          ByteUtil::byte2x2_unpack_0213(src0, src1, src0);
          ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }
      }
      else
      {
        ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));
        ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src));

        OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

        ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
        ((uint32_t*)dst)[0] = dst0;
      }

      dst += 4;
      src += 4;
    } while (--i);
  }

  static void FOG_FASTCALL xrgb32_vspan_argb32(
    uint8_t* dst, const uint8_t* src, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    do {
      ByteUtil::byte1x2 src0, src1;
      ByteUtil::byte1x2 dst0, dst1;

      if (OP::CHARACTERISTICS & (OPERATOR_CHAR_NOP_IF_SRC_A_ZERO | OPERATOR_CHAR_NOP_IF_SRC_A_FULL))
      {
        src0 = READ_32(src);
        if (CompositeHelpersC32<OP::CHARACTERISTICS>::processSrcPixel(src0))
        {
          ByteUtil::byte2x2_unpack_0213(src0, src1, src0);
          ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));

          // Premultiply only if source colors are used.
          if (OP::CHARACTERISTICS & OPERATOR_CHAR_SRC_C_USED) ByteUtil::byte2x2_premultiply(src0, src0, src1, src1);

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }
      }
      else
      {
        ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));
        ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src));

        // Premultiply only if source colors are used.
        if (OP::CHARACTERISTICS & OPERATOR_CHAR_SRC_C_USED) ByteUtil::byte2x2_premultiply(src0, src0, src1, src1);

        OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

        ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
        ((uint32_t*)dst)[0] = dst0;
      }

      dst += 4;
      src += 4;
    } while (--i);
  }

  static void FOG_FASTCALL xrgb32_vspan_xrgb32(
    uint8_t* dst, const uint8_t* src, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    do {
      ByteUtil::byte1x2 src0, src1;
      ByteUtil::byte1x2 dst0, dst1;

      ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));
      ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src));

      OP::xrgb32_op_xrgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

      ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
      ((uint32_t*)dst)[0] = dst0;

      dst += 4;
      src += 4;
    } while (--i);
  }

  static void FOG_FASTCALL xrgb32_vspan_prgb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    do {
      ByteUtil::byte1x2 src0, src1;
      ByteUtil::byte1x2 dst0, dst1;
      uint32_t msk0 = READ_8(msk);

      if (msk0 != 0x00)
      {
        if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND) && OP::CHARACTERISTICS & (OPERATOR_CHAR_UNBOUND_MSK_IN))
        {
          ByteUtil::byte1x2 dst0inv, dst1inv;

          ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));
          ByteUtil::byte2x2_muldiv255_u(dst0inv, dst0, dst1inv, dst1, ByteUtil::scalar_neg255(msk0));
          ByteUtil::byte2x2_pack_0213(dst0inv, dst0inv, dst1inv);

          ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src));
          ByteUtil::byte2x2_muldiv255_u(src0, src0, src1, src1, msk0);

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
        {
          ByteUtil::byte1x2 dst0inv, dst1inv;

          ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));
          ByteUtil::byte2x2_muldiv255_u(dst0inv, dst0, dst1inv, dst1, ByteUtil::scalar_neg255(msk0));
          ByteUtil::byte2x2_pack_0213(dst0inv, dst0inv, dst1inv);

          ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src));

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);
          ByteUtil::byte2x2_muldiv255_u(dst0, dst0, dst1, dst1, msk0);
          ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else
        {
          ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));
          ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src));

          ByteUtil::byte2x2_muldiv255_u(src0, src0, msk0, src1, src1, msk0);

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }
      }

      dst += 4;
      src += 4;
      msk += 1;
    } while (--i);
  }

  static void FOG_FASTCALL xrgb32_vspan_argb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    do {
      ByteUtil::byte1x2 src0, src1;
      ByteUtil::byte1x2 dst0, dst1;
      uint32_t msk0 = READ_8(msk);

      if (msk0 != 0x00)
      {
        if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND) && OP::CHARACTERISTICS & (OPERATOR_CHAR_UNBOUND_MSK_IN))
        {
          ByteUtil::byte1x2 dst0inv, dst1inv;

          ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));
          ByteUtil::byte2x2_muldiv255_u(dst0inv, dst0, dst1inv, dst1, ByteUtil::scalar_neg255(msk0));
          ByteUtil::byte2x2_pack_0213(dst0inv, dst0inv, dst1inv);

          ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src));
          ByteUtil::byte2x2_premultiply_by(src0, src0, src1, src1, ByteUtil::scalar_muldiv255(ByteUtil::byte1x2_hi(src1), msk0));

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
        {
          ByteUtil::byte1x2 dst0inv, dst1inv;

          ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));
          ByteUtil::byte2x2_muldiv255_u(dst0inv, dst0, dst1inv, dst1, ByteUtil::scalar_neg255(msk0));
          ByteUtil::byte2x2_pack_0213(dst0inv, dst0inv, dst1inv);

          ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src));
          ByteUtil::byte2x2_premultiply(src0, src0, src1, src1);

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);
          ByteUtil::byte2x2_muldiv255_u(dst0, dst0, dst1, dst1, msk0);
          ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else
        {
          ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));
          ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src));
          ByteUtil::byte2x2_premultiply_by(src0, src0, src1, src1, ByteUtil::scalar_muldiv255(ByteUtil::byte1x2_hi(src1), msk0));

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }
      }

      dst += 4;
      src += 4;
      msk += 1;
    } while (--i);
  }

  static void FOG_FASTCALL xrgb32_vspan_xrgb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    do {
      ByteUtil::byte1x2 src0, src1;
      ByteUtil::byte1x2 dst0, dst1;
      uint32_t msk0 = READ_8(msk);

      if (msk0 != 0x00)
      {
        if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND) && OP::CHARACTERISTICS & (OPERATOR_CHAR_UNBOUND_MSK_IN))
        {
          ByteUtil::byte1x2 dst0inv, dst1inv;

          ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));
          ByteUtil::byte2x2_muldiv255_u(dst0inv, dst0, dst1inv, dst1, ByteUtil::scalar_neg255(msk0));
          ByteUtil::byte2x2_pack_0213(dst0inv, dst0inv, dst1inv);

          ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src) | 0xFF000000);
          ByteUtil::byte2x2_muldiv255_u(src0, src0, src1, src1, msk0);

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
        {
          ByteUtil::byte1x2 dst0inv, dst1inv;

          ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));
          ByteUtil::byte2x2_muldiv255_u(dst0inv, dst0, dst1inv, dst1, ByteUtil::scalar_neg255(msk0));
          ByteUtil::byte2x2_pack_0213(dst0inv, dst0inv, dst1inv);

          ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src));

          OP::xrgb32_op_xrgb32_32b(dst0, dst0, src0, dst1, dst1, src1);
          ByteUtil::byte2x2_muldiv255_u(dst0, dst0, dst1, dst1, msk0);
          ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else
        {
          ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));
          ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src) | 0xFF000000);

          ByteUtil::byte2x2_muldiv255_u(src0, src0, msk0, src1, src1, msk0);

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }
      }

      dst += 4;
      src += 4;
      msk += 1;
    } while (--i);
  }

  static void FOG_FASTCALL xrgb32_vspan_prgb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    uint32_t msk0inv;
    if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND)) msk0inv = ByteUtil::scalar_neg255(msk0);

    do {
      ByteUtil::byte1x2 src0, src1;
      ByteUtil::byte1x2 dst0, dst1;

      if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND) && OP::CHARACTERISTICS & (OPERATOR_CHAR_UNBOUND_MSK_IN))
      {
        ByteUtil::byte1x2 dst0inv, dst1inv;

        ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));
        ByteUtil::byte2x2_muldiv255_u(dst0inv, dst0, dst1inv, dst1, msk0inv);
        ByteUtil::byte2x2_pack_0213(dst0inv, dst0inv, dst1inv);

        ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src));
        ByteUtil::byte2x2_muldiv255_u(src0, src0, src1, src1, msk0);

        OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

        ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
        ((uint32_t*)dst)[0] = dst0 + dst0inv;
      }
      else if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
      {
        ByteUtil::byte1x2 dst0inv, dst1inv;

        ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));
        ByteUtil::byte2x2_muldiv255_u(dst0inv, dst0, dst1inv, dst1, msk0inv);
        ByteUtil::byte2x2_pack_0213(dst0inv, dst0inv, dst1inv);

        ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src));

        OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);
        ByteUtil::byte2x2_muldiv255_u(dst0, dst0, dst1, dst1, msk0);
        ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
        ((uint32_t*)dst)[0] = dst0 + dst0inv;
      }
      else
      {
        ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));
        ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src));

        ByteUtil::byte2x2_muldiv255_u(src0, src0, msk0, src1, src1, msk0);

        OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

        ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
        ((uint32_t*)dst)[0] = dst0;
      }

      dst += 4;
      src += 4;
    } while (--i);
  }

  static void FOG_FASTCALL xrgb32_vspan_argb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    uint32_t msk0inv;
    if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND)) msk0inv = ByteUtil::scalar_neg255(msk0);

    do {
      ByteUtil::byte1x2 src0, src1;
      ByteUtil::byte1x2 dst0, dst1;

      if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND) && OP::CHARACTERISTICS & (OPERATOR_CHAR_UNBOUND_MSK_IN))
      {
        ByteUtil::byte1x2 dst0inv, dst1inv;

        ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));
        ByteUtil::byte2x2_muldiv255_u(dst0inv, dst0, dst1inv, dst1, msk0inv);
        ByteUtil::byte2x2_pack_0213(dst0inv, dst0inv, dst1inv);

        ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src));
        ByteUtil::byte2x2_premultiply_by(src0, src0, src1, src1, ByteUtil::scalar_muldiv255(ByteUtil::byte1x2_hi(src1), msk0));

        OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

        ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
        ((uint32_t*)dst)[0] = dst0 + dst0inv;
      }
      else if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
      {
        ByteUtil::byte1x2 dst0inv, dst1inv;

        ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));
        ByteUtil::byte2x2_muldiv255_u(dst0inv, dst0, dst1inv, dst1, msk0inv);
        ByteUtil::byte2x2_pack_0213(dst0inv, dst0inv, dst1inv);

        ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src));
        ByteUtil::byte2x2_premultiply(src0, src0, src1, src1);

        OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);
        ByteUtil::byte2x2_muldiv255_u(dst0, dst0, dst1, dst1, msk0);
        ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
        ((uint32_t*)dst)[0] = dst0 + dst0inv;
      }
      else
      {
        ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));
        ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src));
        ByteUtil::byte2x2_premultiply_by(src0, src0, src1, src1, ByteUtil::scalar_muldiv255(ByteUtil::byte1x2_hi(src1), msk0));

        OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

        ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
        ((uint32_t*)dst)[0] = dst0;
      }

      dst += 4;
      src += 4;
    } while (--i);
  }

  static void FOG_FASTCALL xrgb32_vspan_xrgb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    uint32_t msk0inv;
    if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND)) msk0inv = ByteUtil::scalar_neg255(msk0);

    do {
      ByteUtil::byte1x2 src0, src1;
      ByteUtil::byte1x2 dst0, dst1;

      if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND) && OP::CHARACTERISTICS & (OPERATOR_CHAR_UNBOUND_MSK_IN))
      {
        ByteUtil::byte1x2 dst0inv, dst1inv;

        ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));
        ByteUtil::byte2x2_muldiv255_u(dst0inv, dst0, dst1inv, dst1, msk0inv);
        ByteUtil::byte2x2_pack_0213(dst0inv, dst0inv, dst1inv);

        ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src) | 0xFF000000);
        ByteUtil::byte2x2_muldiv255_u(src0, src0, src1, src1, msk0);

        OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

        ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
        ((uint32_t*)dst)[0] = dst0 + dst0inv;
      }
      else if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
      {
        ByteUtil::byte1x2 dst0inv, dst1inv;

        ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));
        ByteUtil::byte2x2_muldiv255_u(dst0inv, dst0, dst1inv, dst1, msk0inv);
        ByteUtil::byte2x2_pack_0213(dst0inv, dst0inv, dst1inv);

        ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src));

        OP::xrgb32_op_xrgb32_32b(dst0, dst0, src0, dst1, dst1, src1);
        ByteUtil::byte2x2_muldiv255_u(dst0, dst0, dst1, dst1, msk0);
        ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
        ((uint32_t*)dst)[0] = dst0 + dst0inv;
      }
      else
      {
        ByteUtil::byte2x2_unpack_0213(dst0, dst1, READ_32(dst));
        ByteUtil::byte2x2_unpack_0213(src0, src1, READ_32(src) | 0xFF000000);

        ByteUtil::byte2x2_muldiv255_u(src0, src0, msk0, src1, src1, msk0);

        OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

        ByteUtil::byte2x2_pack_0213(dst0, dst0, dst1);
        ((uint32_t*)dst)[0] = dst0;
      }

      dst += 4;
      src += 4;
    } while (--i);
  }
};

// ############################################################################
// Implementation starts from here.
// ############################################################################

// ============================================================================
// [Fog::RasterEngine::C - Composite - Src]
// ============================================================================

struct CompositeSrcC /* : public CompositeBaseFuncsC32<CompositeSrcC> */
{
  enum { CHARACTERISTICS = OPERATOR_SRC };

  // -------------------------------------------------------------------------
  // [CompositeSrcC - PRGB32]
  // -------------------------------------------------------------------------

  static void FOG_FASTCALL prgb32_cspan(
    uint8_t* dst, const Solid* src, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    uint32_t srcp = src->prgb;

    do {
      ((uint32_t*)dst)[0] = srcp;
      dst += 4;
    } while (--i);
  }

  static void FOG_FASTCALL prgb32_cspan_a8(
    uint8_t* dst, const Solid* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    uint32_t srcp = src->prgb;
    uint32_t src0 = srcp;

    do {
      uint32_t msk0 = READ_8(msk);
      uint32_t dst0;

      if (msk0 == 0x00) goto skip;
      if (msk0 == 0xFF) goto fill;

      dst0 = READ_32(dst);
      src0 = ByteUtil::packed_muldiv255(src0, msk0);
      dst0 = ByteUtil::packed_muldiv255(dst0, ByteUtil::scalar_neg255(msk0));
      src0 += dst0;

fill:
      ((uint32_t*)dst)[0] = src0;
      src0 = srcp;

skip:
      dst += 4;
      msk += 1;
    } while (--i);
  }

  static void FOG_FASTCALL prgb32_cspan_a8_const(
    uint8_t* dst, const Solid* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    uint32_t srcm = ByteUtil::packed_muldiv255(src->prgb, msk0);
    uint32_t msk0inv = ByteUtil::scalar_neg255(msk0);

    do {
      ((uint32_t*)dst)[0] = ByteUtil::packed_muldiv255(READ_32(dst), msk0inv) + srcm;

      dst += 4;
    } while (--i);
  }

  static void FOG_FASTCALL prgb32_cspan_a8_scanline(
    uint8_t* dst, const Solid* src, const Scanline32::Span* spans, sysuint_t numSpans, const Closure* closure)
  {
    uint32_t srcp = src->prgb;

    ByteUtil::byte1x2 src0orig, src1orig;
    ByteUtil::byte2x2_unpack_0213(src0orig, src1orig, srcp);

    BLIT_CSPAN_SCANLINE_STEP1_BEGIN(4)

    // Const mask.
    BLIT_CSPAN_SCANLINE_STEP2_CONST()
    {
      sysint_t i = w;
      FOG_ASSERT(w);

      if (msk0 == 255)
      {
        do {
          ((uint32_t*)dst)[0] = srcp;
          dst += 4;
        } while (--i);
      }
      else
      {
        uint32_t srcm = ByteUtil::byte2x2_muldiv255_u_pack0213(src0orig, src1orig, msk0);
        uint32_t msk0inv = ByteUtil::scalar_neg255(msk0);

        do {
          ((uint32_t*)dst)[0] = ByteUtil::packed_muldiv255(READ_32(dst), msk0inv) + srcm;

          dst += 4;
        } while (--i);
      }
    }
    // Variable mask.
    BLIT_CSPAN_SCANLINE_STEP3_MASK()
    {
      sysint_t i = w;
      FOG_ASSERT(w);

      do {
        uint32_t msk0 = READ_8(msk);
        uint32_t dst0;
        uint32_t src0;

        dst0 = READ_32(dst);
        src0 = ByteUtil::byte2x2_muldiv255_u_pack0213(src0orig, src1orig, msk0);
        dst0 = ByteUtil::packed_muldiv255(dst0, ByteUtil::scalar_neg255(msk0));

        ((uint32_t*)dst)[0] = src0 + dst0;

        dst += 4;
        msk += 1;
      } while (--i);
    }
    BLIT_CSPAN_SCANLINE_STEP4_END()
  }

  static void FOG_FASTCALL prgb32_vspan_prgb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    do {
      uint32_t dst0;
      uint32_t src0;
      uint32_t msk0 = READ_8(msk);

      if (msk0 == 0x00) goto skip;

      src0 = READ_32(src);
      if (msk0 == 0xFF) goto fill;

      dst0 = READ_32(dst);
      src0 = ByteUtil::packed_muldiv255(src0, msk0);
      src0 += ByteUtil::packed_muldiv255(dst0, ByteUtil::scalar_neg255(msk0));

fill:
      ((uint32_t*)dst)[0] = src0;

skip:
      dst += 4;
      src += 4;
      msk += 1;
    } while (--i);
  }

  static void FOG_FASTCALL prgb32_vspan_argb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    do {
      uint32_t dst0;
      uint32_t src0;
      uint32_t msk0 = READ_8(msk);

      if (msk0 == 0x00) goto skip;

      src0 = READ_32(src);
      dst0 = READ_32(dst);
      src0 = ByteUtil::packed_muldiv255(src0 | 0xFF000000, ByteUtil::scalar_muldiv255(msk0, (src0 >> 24)));
      src0 += ByteUtil::packed_muldiv255(dst0, ByteUtil::scalar_neg255(msk0));

      ((uint32_t*)dst)[0] = src0;

skip:
      dst += 4;
      src += 4;
      msk += 1;
    } while (--i);
  }

  static void FOG_FASTCALL prgb32_vspan_xrgb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    do {
      uint32_t dst0;
      uint32_t src0;
      uint32_t msk0 = READ_8(msk);

      if (msk0 == 0x00) goto skip;

      src0 = READ_32(src) | 0xFF000000;
      if (msk0 == 0xFF) goto fill;

      dst0 = READ_32(dst);
      src0 = ByteUtil::packed_muldiv255(src0, msk0);
      src0 += ByteUtil::packed_muldiv255(dst0, ByteUtil::scalar_neg255(msk0));

fill:
      ((uint32_t*)dst)[0] = src0;

skip:
      dst += 4;
      src += 4;
      msk += 1;
    } while (--i);
  }

  static void FOG_FASTCALL prgb32_vspan_a8_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    do {
      uint32_t dst0;
      uint32_t src0;
      uint32_t msk0 = READ_8(msk);

      if (msk0 == 0x00) goto skip;

      src0 = READ_8(src);
      dst0 = READ_32(dst);

      src0 = ByteUtil::scalar_muldiv255(src0, msk0) << 24;
      src0 += ByteUtil::packed_muldiv255(dst0, ByteUtil::scalar_neg255(msk0));

      ((uint32_t*)dst)[0] = src0;

skip:
      dst += 4;
      src += 1;
      msk += 1;
    } while (--i);
  }

  static void FOG_FASTCALL prgb32_vspan_i8_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    const Argb* pal = closure->srcPalette;

    do {
      uint32_t dst0;
      uint32_t src0;
      uint32_t msk0 = READ_8(msk);

      if (msk0 == 0x00) goto skip;

      src0 = pal[READ_8(src)];
      if (msk0 == 0xFF) goto fill;

      dst0 = READ_32(dst);
      src0 = ByteUtil::packed_muldiv255(src0, msk0);
      src0 += ByteUtil::packed_muldiv255(dst0, ByteUtil::scalar_neg255(msk0));

fill:
      ((uint32_t*)dst)[0] = src0;

skip:
      dst += 4;
      src += 1;
      msk += 1;
    } while (--i);
  }

  static void FOG_FASTCALL prgb32_vspan_prgb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    uint32_t msk0inv = ByteUtil::scalar_neg255(msk0);

    do {
      uint32_t dst0 = READ_32(dst);
      uint32_t src0 = READ_32(src);

      ((uint32_t*)dst)[0] = ByteUtil::packed_muldiv255_2x_join(src0, msk0, dst0, msk0inv);

      dst += 4;
      src += 4;
    } while (--i);
  }

  static void FOG_FASTCALL prgb32_vspan_argb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    uint32_t msk0inv = ByteUtil::scalar_neg255(msk0);

    do {
      uint32_t dst0 = READ_32(dst);
      uint32_t src0 = READ_32(src);

      ((uint32_t*)dst)[0] = ByteUtil::packed_muldiv255_2x_join(src0 | 0xFF000000, ByteUtil::scalar_muldiv255(msk0, src0 >> 24), dst0, msk0inv);

      dst += 4;
      src += 4;
    } while (--i);
  }

  static void FOG_FASTCALL prgb32_vspan_xrgb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    uint32_t msk0inv = ByteUtil::scalar_neg255(msk0);

    do {
      uint32_t dst0 = READ_32(dst);
      uint32_t src0 = READ_32(src) | 0xFF000000;

      ((uint32_t*)dst)[0] = ByteUtil::packed_muldiv255_2x_join(src0, msk0, dst0, msk0inv);

      dst += 4;
      src += 4;
    } while (--i);
  }

  static void FOG_FASTCALL prgb32_vspan_a8_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    uint32_t msk0inv = ByteUtil::scalar_neg255(msk0);

    do {
      uint32_t dst0 = READ_32(dst);
      uint32_t src0 = ByteUtil::scalar_muldiv255(READ_8(src), msk0) << 24;
      ((uint32_t*)dst)[0] = src0 + ByteUtil::packed_muldiv255(dst0, msk0inv);

      dst += 4;
      src += 1;
    } while (--i);
  }

  static void FOG_FASTCALL prgb32_vspan_i8_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    const Argb* pal = closure->srcPalette;
    uint32_t msk0inv = ByteUtil::scalar_neg255(msk0);

    do {
      ByteUtil::byte1x2 dst0 = READ_32(dst);
      ByteUtil::byte1x2 src0 = pal[READ_8(src)];
      ((uint32_t*)dst)[0] = ByteUtil::packed_muldiv255_2x_join(src0, msk0, dst0, msk0inv);

      dst += 4;
      src += 1;
    } while (--i);
  }

  // -------------------------------------------------------------------------
  // [CompositeSrcC - XRGB32]
  // -------------------------------------------------------------------------

  static void FOG_FASTCALL xrgb32_cspan(
    uint8_t* dst, const Solid* src, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    uint32_t srcp = src->prgb | 0xFF000000;

    do {
      ((uint32_t*)dst)[0] = srcp;
      dst += 4;
    } while (--i);
  }

  static void FOG_FASTCALL xrgb32_cspan_a8(
    uint8_t* dst, const Solid* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    uint32_t srcp = src->prgb | 0xFF000000;
    uint32_t src0 = srcp;

    do {
      uint32_t msk0 = READ_8(msk);
      uint32_t dst0;

      if (msk0 == 0x00) goto skip;
      if (msk0 == 0xFF) goto fill;

      dst0 = READ_32(dst);
      src0 = ByteUtil::packed_muldiv255(src0, msk0);
      dst0 = ByteUtil::packed_muldiv255(dst0, ByteUtil::scalar_neg255(msk0));
      src0 += dst0;

fill:
      ((uint32_t*)dst)[0] = src0;
      src0 = srcp;

skip:
      dst += 4;
      msk += 1;
    } while (--i);
  }

  static void FOG_FASTCALL xrgb32_cspan_a8_const(
    uint8_t* dst, const Solid* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    uint32_t srcm = ByteUtil::packed_muldiv255(src->prgb | 0xFF000000, msk0);
    uint32_t msk0inv = ByteUtil::scalar_neg255(msk0);

    do {
      ((uint32_t*)dst)[0] = ByteUtil::packed_muldiv255(READ_32(dst), msk0inv) + srcm;

      dst += 4;
    } while (--i);
  }

  static void FOG_FASTCALL xrgb32_cspan_a8_scanline(
    uint8_t* dst, const Solid* src, const Scanline32::Span* spans, sysuint_t numSpans, const Closure* closure)
  {
    uint32_t srcp = src->prgb | 0xFF000000;

    ByteUtil::byte1x2 src0orig, src1orig;
    ByteUtil::byte2x2_unpack_0213(src0orig, src1orig, srcp);

    BLIT_CSPAN_SCANLINE_STEP1_BEGIN(4)

    // Const mask.
    BLIT_CSPAN_SCANLINE_STEP2_CONST()
    {
      sysint_t i = w;
      FOG_ASSERT(w);

      if (msk0 == 255)
      {
        do {
          ((uint32_t*)dst)[0] = srcp;
          dst += 4;
        } while (--i);
      }
      else
      {
        uint32_t srcm = ByteUtil::byte2x2_muldiv255_u_pack0213(src0orig, src1orig, msk0);
        uint32_t msk0inv = ByteUtil::scalar_neg255(msk0);

        do {
          ((uint32_t*)dst)[0] = ByteUtil::packed_muldiv255(READ_32(dst), msk0inv) + srcm;

          dst += 4;
        } while (--i);
      }
    }
    // Variable mask.
    BLIT_CSPAN_SCANLINE_STEP3_MASK()
    {
      sysint_t i = w;
      FOG_ASSERT(w);

      uint32_t src0 = srcp;

      do {
        uint32_t msk0 = READ_8(msk);
        uint32_t dst0;

        dst0 = READ_32(dst);
        src0 = ByteUtil::byte2x2_muldiv255_u_pack0213(src0orig, src1orig, msk0);
        dst0 = ByteUtil::packed_muldiv255(dst0, ByteUtil::scalar_neg255(msk0));

        ((uint32_t*)dst)[0] = src0 + dst0;

        dst += 4;
        msk += 1;
      } while (--i);
    }
    BLIT_CSPAN_SCANLINE_STEP4_END()
  }

  static void FOG_FASTCALL xrgb32_vspan_prgb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    do {
      uint32_t dst0;
      uint32_t src0;
      uint32_t msk0 = READ_8(msk);

      if (msk0 == 0x00) goto skip;

      src0 = READ_32(src);
      if (msk0 == 0xFF) goto fill;

      dst0 = READ_32(dst);
      src0 = ByteUtil::packed_muldiv255_Fxxx(src0, msk0);
      src0 += ByteUtil::packed_muldiv255_0xxx(dst0, ByteUtil::scalar_neg255(msk0));

fill:
      ((uint32_t*)dst)[0] = src0;

skip:
      dst += 4;
      src += 4;
      msk += 1;
    } while (--i);
  }

  static void FOG_FASTCALL xrgb32_vspan_argb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    do {
      uint32_t dst0;
      uint32_t src0;
      uint32_t msk0 = READ_8(msk);

      if (msk0 == 0x00) goto skip;

      src0 = READ_32(src);
      dst0 = READ_32(dst);
      src0 = ByteUtil::packed_muldiv255_Fxxx(src0, ByteUtil::scalar_muldiv255(msk0, (src0 >> 24)));
      src0 += ByteUtil::packed_muldiv255_0xxx(dst0, ByteUtil::scalar_neg255(msk0));

      ((uint32_t*)dst)[0] = src0;

skip:
      dst += 4;
      src += 4;
      msk += 1;
    } while (--i);
  }

  static void FOG_FASTCALL xrgb32_vspan_xrgb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    do {
      uint32_t dst0;
      uint32_t src0;
      uint32_t msk0 = READ_8(msk);

      if (msk0 == 0x00) goto skip;

      src0 = READ_32(src) | 0xFF000000;
      if (msk0 == 0xFF) goto fill;

      dst0 = READ_32(dst);
      src0 = ByteUtil::packed_muldiv255_Fxxx(src0, msk0);
      src0 += ByteUtil::packed_muldiv255_0xxx(dst0, ByteUtil::scalar_neg255(msk0));

fill:
      ((uint32_t*)dst)[0] = src0;

skip:
      dst += 4;
      src += 4;
      msk += 1;
    } while (--i);
  }

  static void FOG_FASTCALL xrgb32_vspan_i8_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    const Argb* pal = closure->srcPalette;

    do {
      uint32_t dst0;
      uint32_t src0;
      uint32_t msk0 = READ_8(msk);

      if (msk0 == 0x00) goto skip;

      src0 = pal[READ_8(src)];
      if (msk0 == 0xFF) goto fill;

      dst0 = READ_32(dst);
      src0 = ByteUtil::packed_muldiv255_Fxxx(src0, msk0);
      src0 += ByteUtil::packed_muldiv255_0xxx(dst0, ByteUtil::scalar_neg255(msk0));

fill:
      ((uint32_t*)dst)[0] = src0;

skip:
      dst += 4;
      src += 1;
      msk += 1;
    } while (--i);
  }

  static void FOG_FASTCALL xrgb32_vspan_prgb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    uint32_t msk0inv = ByteUtil::scalar_neg255(msk0);

    do {
      uint32_t dst0 = READ_32(dst);
      uint32_t src0 = READ_32(src);

      ((uint32_t*)dst)[0] =
        ByteUtil::packed_muldiv255_Fxxx(src0, msk0) +
        ByteUtil::packed_muldiv255_0xxx(dst0, msk0inv);

      dst += 4;
      src += 4;
    } while (--i);
  }

  static void FOG_FASTCALL xrgb32_vspan_argb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    uint32_t msk0inv = ByteUtil::scalar_neg255(msk0);

    do {
      uint32_t dst0 = READ_32(dst);
      uint32_t src0 = READ_32(src);

      ((uint32_t*)dst)[0] =
        ByteUtil::packed_muldiv255_Fxxx(src0, ByteUtil::scalar_muldiv255(msk0, src0 >> 24)) +
        ByteUtil::packed_muldiv255_0xxx(dst0, msk0inv);

      dst += 4;
      src += 4;
    } while (--i);
  }

  static void FOG_FASTCALL xrgb32_vspan_xrgb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    uint32_t msk0inv = ByteUtil::scalar_neg255(msk0);

    do {
      uint32_t dst0 = READ_32(dst);
      uint32_t src0 = READ_32(src);

      ((uint32_t*)dst)[0] = 
        ByteUtil::packed_muldiv255_Fxxx(src0, msk0) +
        ByteUtil::packed_muldiv255_0xxx(dst0, msk0inv);

      dst += 4;
      src += 4;
    } while (--i);
  }

  static void FOG_FASTCALL xrgb32_vspan_i8_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    uint32_t msk0inv = ByteUtil::scalar_neg255(msk0);
    const Argb* pal = closure->srcPalette;

    do {
      ByteUtil::byte1x2 dst0 = READ_32(dst);
      ByteUtil::byte1x2 src0 = pal[READ_8(src)];

      ((uint32_t*)dst)[0] =
        ByteUtil::packed_muldiv255_Fxxx(src0, msk0) +
        ByteUtil::packed_muldiv255_0xxx(dst0, msk0inv);

      dst += 4;
      src += 1;
    } while (--i);
  }
};









































// ============================================================================
// [Fog::RasterEngine::C - Composite - Clear]
// ============================================================================

struct CompositeClearC : public CompositeBaseFuncsC32<CompositeClearC>
{
  enum { CHARACTERISTICS = OPERATOR_CLEAR };

  static FOG_INLINE void prgb32_op_prgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    dst0 = 0;
    dst1 = 0;
  }

  static FOG_INLINE void prgb32_op_xrgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    dst0 = 0;
    dst1 = 0;
  }

  static FOG_INLINE void xrgb32_op_prgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    dst0 = 0;
    dst1 = 0;
  }

  static FOG_INLINE void xrgb32_op_xrgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    dst0 = 0;
    dst1 = 0;
  }
};




















// ============================================================================
// [Fog::RasterEngine::C - Composite - SrcOver]
// ============================================================================

struct CompositeSrcOverC /* : public CompositeBaseFuncsC32<CompositeSrcOverC> */
{
  enum { CHARACTERISTICS = OPERATOR_SRC_OVER };

  // -------------------------------------------------------------------------
  // [CompositeSrcC - PRGB32]
  // -------------------------------------------------------------------------

  static void FOG_FASTCALL prgb32_cspan(
    uint8_t* dst, const Solid* src, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    uint32_t srcp = src->prgb;
    if (ArgbUtil::isAlpha0xFF(srcp))
    {
      do {
        ((uint32_t*)dst)[0] = srcp;
        dst += 4;
      } while (--i);
    }
    else
    {
      uint32_t srcainv = ByteUtil::scalar_neg255(srcp >> 24);

      do {
        ((uint32_t*)dst)[0] = ByteUtil::packed_muldiv255(READ_32(dst), srcainv) + srcp;
        dst += 4;
      } while (--i);
    }
  }

  static void FOG_FASTCALL prgb32_cspan_a8(
    uint8_t* dst, const Solid* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    uint32_t srcp = src->prgb;

    ByteUtil::byte1x2 src0orig, src1orig;
    ByteUtil::byte2x2_unpack_0213(src0orig, src1orig, srcp);

    if (ArgbUtil::isAlpha0xFF(srcp))
    {
      sysint_t i = w;
      FOG_ASSERT(w);

      uint32_t src0 = srcp;

      do {
        uint32_t msk0 = READ_8(msk);
        uint32_t dst0;

        if (msk0 == 0x00) goto opaqueSkip;
        if (msk0 == 0xFF) goto opaqueFill;

        dst0 = READ_32(dst);
        src0 = ByteUtil::byte2x2_muldiv255_u_pack0213(src0orig, src1orig, msk0);
        dst0 = ByteUtil::packed_muldiv255(dst0, ByteUtil::scalar_neg255(msk0));
        src0 += dst0;

opaqueFill:
        ((uint32_t*)dst)[0] = src0;
        src0 = srcp;

opaqueSkip:
        dst += 4;
        msk += 1;
      } while (--i);
    }
    else
    {
      uint32_t dst0 = srcp;

      do {
        uint32_t msk0 = READ_8(msk);
        uint32_t srcm;

        if (msk0 == 0x00) goto transSkip;

        srcm = ByteUtil::byte2x2_muldiv255_u_pack0213(src0orig, src1orig, msk0);
        if (srcm == 0x00000000) goto transSkip;

        dst0 = ByteUtil::packed_muldiv255(READ_32(dst), ByteUtil::scalar_neg255(srcm >> 24));
        dst0 += srcm;

        ((uint32_t*)dst)[0] = dst0;

transSkip:
        dst += 4;
        msk += 1;
      } while (--i);
    }
  }

  static void FOG_FASTCALL prgb32_cspan_a8_const(
    uint8_t* dst, const Solid* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    uint32_t src0 = ByteUtil::packed_muldiv255(src->prgb, msk0);
    if (!src0) return;

    msk0 = ByteUtil::scalar_neg255(src0 >> 24);

    do {
      ((uint32_t*)dst)[0] = ByteUtil::packed_muldiv255(READ_32(dst), msk0) + src0;

      dst += 4;
    } while (--i);
  }

  static void FOG_FASTCALL prgb32_cspan_a8_scanline(
    uint8_t* dst, const Solid* src, const Scanline32::Span* spans, sysuint_t numSpans, const Closure* closure)
  {
    uint32_t srcp = src->prgb;

    ByteUtil::byte1x2 src0orig, src1orig;
    ByteUtil::byte2x2_unpack_0213(src0orig, src1orig, srcp);

    if (ArgbUtil::isAlpha0xFF(srcp))
    {
      BLIT_CSPAN_SCANLINE_STEP1_BEGIN(4)

      // Const mask.
      BLIT_CSPAN_SCANLINE_STEP2_CONST()
      {
        sysint_t i = w;
        FOG_ASSERT(w);

        if (msk0 == 255)
        {
          do {
            ((uint32_t*)dst)[0] = srcp;
            dst += 4;
          } while (--i);
        }
        else
        {
          uint32_t src0 = ByteUtil::packed_muldiv255(src->prgb, msk0);
          if (!src0) return;

          msk0 = ByteUtil::scalar_neg255(src0 >> 24);

          do {
            ((uint32_t*)dst)[0] = ByteUtil::packed_muldiv255(READ_32(dst), msk0) + src0;

            dst += 4;
          } while (--i);
        }
      }
      // Variable mask.
      BLIT_CSPAN_SCANLINE_STEP3_MASK()
      {
        sysint_t i = w;
        FOG_ASSERT(w);

        do {
          uint32_t msk0 = READ_8(msk);
          uint32_t dst0;
          uint32_t src0;

          dst0 = READ_32(dst);
          src0 = ByteUtil::byte2x2_muldiv255_u_pack0213(src0orig, src1orig, msk0);
          dst0 = ByteUtil::packed_muldiv255(dst0, ByteUtil::scalar_neg255(msk0));

          ((uint32_t*)dst)[0] = src0 + dst0;

          dst += 4;
          msk += 1;
        } while (--i);
      }
      BLIT_CSPAN_SCANLINE_STEP4_END()
    }
    else
    {
      BLIT_CSPAN_SCANLINE_STEP1_BEGIN(4)

      // Const mask.
      BLIT_CSPAN_SCANLINE_STEP2_CONST()
      {
        sysint_t i = w;
        FOG_ASSERT(w);

        uint32_t srcm = srcp;
        if (msk0 != 0xFF) srcm = ByteUtil::byte2x2_muldiv255_u_pack0213(src0orig, src1orig, msk0);

        msk0 = ByteUtil::scalar_neg255(srcm >> 24);

        do {
          ((uint32_t*)dst)[0] = ByteUtil::packed_muldiv255(READ_32(dst), msk0) + srcm;

          dst += 4;
        } while (--i);
      }
      // Variable mask.
      BLIT_CSPAN_SCANLINE_STEP3_MASK()
      {
        sysint_t i = w;
        FOG_ASSERT(w);

        do {
          uint32_t dst0 = READ_32(dst);
          uint32_t src0;
          uint32_t msk0 = READ_8(msk);

          src0 = ByteUtil::byte2x2_muldiv255_u_pack0213(src0orig, src1orig, msk0);
          dst0 = ByteUtil::packed_muldiv255(dst0, ByteUtil::scalar_neg255(src0 >> 24));

          ((uint32_t*)dst)[0] = dst0 + src0;

          dst += 4;
          msk += 1;
        } while (--i);
      }
      BLIT_CSPAN_SCANLINE_STEP4_END()
    }
  }

  static void FOG_FASTCALL prgb32_vspan_prgb32(
    uint8_t* dst, const uint8_t* src, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    do {
      uint32_t dst0;
      uint32_t src0;

      src0 = READ_32(src);
      if (src0 == 0x00000000) goto skip;
      if (ArgbUtil::isAlpha0xFF(src0)) goto fill;

      dst0 = READ_32(dst);
      dst0 = ByteUtil::packed_muldiv255(dst0, ByteUtil::scalar_neg255(src0 >> 24));

      src0 += dst0;

fill:
      ((uint32_t*)dst)[0] = src0;

skip:
      dst += 4;
      src += 4;
    } while (--i);
  }

  static void FOG_FASTCALL prgb32_vspan_argb32(
    uint8_t* dst, const uint8_t* src, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    do {
      uint32_t dst0;
      uint32_t src0;
      uint32_t srca;

      src0 = READ_32(src);
      if (ArgbUtil::isAlpha0x00(src0)) goto skip;
      if (ArgbUtil::isAlpha0xFF(src0)) goto fill;

      srca = src0 >> 24;
      dst0 = READ_32(dst);

      src0 = ByteUtil::packed_muldiv255_0xxx(src0, srca) | (srca << 24);
      dst0 = ByteUtil::packed_muldiv255(dst0, ByteUtil::scalar_neg255(srca));

      src0 += dst0;

fill:
      ((uint32_t*)dst)[0] = src0;

skip:
      dst += 4;
      src += 4;
    } while (--i);
  }

  static void FOG_FASTCALL prgb32_vspan_i8(
    uint8_t* dst, const uint8_t* src, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    const Argb* pal = closure->srcPalette;

    do {
      uint32_t dst0;
      uint32_t src0;

      src0 = pal[READ_8(src)];
      if (src0 != 0x00000000) goto skip;
      if (ArgbUtil::isAlpha0xFF(src0)) goto fill;

      dst0 = READ_32(dst);
      dst0 = ByteUtil::packed_muldiv255(dst0, ByteUtil::scalar_neg255(src0 >> 24));

      src0 += dst0;

fill:
      ((uint32_t*)dst)[0] = src0;

skip:
      dst += 4;
      src += 1;
    } while (--i);
  }

  static void FOG_FASTCALL prgb32_vspan_prgb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    do {
      uint32_t dst0;
      uint32_t src0;
      uint32_t msk0 = READ_8(msk);

      if (msk0 != 0x00) goto skip;

      src0 = READ_32(src);
      if (msk0 == 0xFF && ArgbUtil::isAlpha0xFF(src0)) goto fill;

      dst0 = READ_32(dst);

      src0 = ByteUtil::packed_muldiv255(src0, msk0);
      dst0 = ByteUtil::packed_muldiv255(dst0, ByteUtil::scalar_neg255(src0 >> 24));

      src0 += dst0;

fill:
      ((uint32_t*)dst)[0] = src0;

skip:
      dst += 4;
      src += 4;
      msk += 1;
    } while (--i);
  }

  static void FOG_FASTCALL prgb32_vspan_argb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    do {
      uint32_t dst0;
      uint32_t src0;
      uint32_t msk0 = READ_8(msk);

      if (msk0 != 0x00) goto skip;

      src0 = READ_32(src);
      if (msk0 == 0xFF && ArgbUtil::isAlpha0xFF(src0)) goto fill;

      dst0 = READ_32(dst);

      src0 = ByteUtil::packed_muldiv255(src0 | 0xFF000000, ByteUtil::scalar_muldiv255(src0 >> 24, msk0));
      dst0 = ByteUtil::packed_muldiv255(dst0, ByteUtil::scalar_neg255(src0 >> 24));

      src0 += dst0;

fill:
      ((uint32_t*)dst)[0] = src0;

skip:
      dst += 4;
      src += 4;
      msk += 1;
    } while (--i);
  }

  static void FOG_FASTCALL prgb32_vspan_i8_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    const Argb* pal = closure->srcPalette;

    do {
      uint32_t dst0;
      uint32_t src0;
      uint32_t msk0 = READ_8(msk);

      if (msk0 != 0x00) goto skip;

      src0 = pal[READ_8(src)];
      if (msk0 == 0xFF && ArgbUtil::isAlpha0xFF(src0)) goto fill;

      dst0 = READ_32(dst);

      src0 = ByteUtil::packed_muldiv255(src0, msk0);
      dst0 = ByteUtil::packed_muldiv255(dst0, ByteUtil::scalar_neg255(src0 >> 24));

      src0 += dst0;

fill:
      ((uint32_t*)dst)[0] = src0;

skip:
      dst += 4;
      src += 1;
      msk += 1;
    } while (--i);
  }

  static void FOG_FASTCALL prgb32_vspan_prgb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    do {
      uint32_t dst0 = READ_32(dst);
      uint32_t src0 = READ_32(src);

      src0 = ByteUtil::packed_muldiv255(src0, msk0);
      dst0 = ByteUtil::packed_muldiv255(dst0, ByteUtil::scalar_neg255(src0 >> 24));
      ((uint32_t*)dst)[0] = src0 + dst0;

      dst += 4;
      src += 4;
    } while (--i);
  }

  static void FOG_FASTCALL prgb32_vspan_argb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    do {
      uint32_t dst0 = READ_32(dst);
      uint32_t src0 = READ_32(src);
      uint32_t srca = ByteUtil::scalar_muldiv255(src0 >> 24, msk0);

      src0 = ByteUtil::packed_muldiv255_0xxx(src0, srca) | (srca << 24);
      dst0 = ByteUtil::packed_muldiv255(dst0, ByteUtil::scalar_neg255(srca));
      ((uint32_t*)dst)[0] = src0 + dst0;

      dst += 4;
      src += 4;
    } while (--i);
  }

  static void FOG_FASTCALL prgb32_vspan_i8_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    const Argb* pal = closure->srcPalette;

    do {
      uint32_t dst0 = READ_32(dst);
      uint32_t src0 = pal[READ_8(src)];

      src0 = ByteUtil::packed_muldiv255(src0, msk0);
      dst0 = ByteUtil::packed_muldiv255(dst0, ByteUtil::scalar_neg255(src0 >> 24));
      ((uint32_t*)dst)[0] = src0 + dst0;

      dst += 4;
      src += 1;
    } while (--i);
  }

  // -------------------------------------------------------------------------
  // [CompositeSrcC - XRGB32]
  // -------------------------------------------------------------------------

  static void FOG_FASTCALL xrgb32_cspan(
    uint8_t* dst, const Solid* src, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    uint32_t srcp = src->prgb;
    if (ArgbUtil::isAlpha0xFF(srcp))
    {
      do {
        ((uint32_t*)dst)[0] = srcp;
        dst += 4;
      } while (--i);
    }
    else
    {
      uint32_t srcainv = ByteUtil::scalar_neg255(srcp >> 24);
      srcp |= 0xFF000000;

      do {
        ((uint32_t*)dst)[0] = ByteUtil::packed_muldiv255(READ_32(dst), srcainv) + srcp;
        dst += 4;
      } while (--i);
    }
  }

  static void FOG_FASTCALL xrgb32_cspan_a8(
    uint8_t* dst, const Solid* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    uint32_t srcp = src->prgb;

    ByteUtil::byte1x2 src0orig, src1orig;
    ByteUtil::byte2x2_unpack_0213(src0orig, src1orig, srcp);

    if (ArgbUtil::isAlpha0xFF(srcp))
    {
      sysint_t i = w;
      FOG_ASSERT(w);

      uint32_t src0 = srcp;

      do {
        uint32_t msk0 = READ_8(msk);
        uint32_t dst0;

        if (msk0 == 0x00) goto opaqueSkip;
        if (msk0 == 0xFF) goto opaqueFill;

        dst0 = READ_32(dst);
        src0 = ByteUtil::byte2x2_muldiv255_u_pack0213(src0orig, src1orig, msk0);
        dst0 = ByteUtil::packed_muldiv255(dst0, ByteUtil::scalar_neg255(msk0));
        src0 += dst0;

opaqueFill:
        ((uint32_t*)dst)[0] = src0;
        src0 = srcp;

opaqueSkip:
        dst += 4;
        msk += 1;
      } while (--i);
    }
    else
    {
      uint32_t dst0 = srcp;

      do {
        uint32_t msk0 = READ_8(msk);
        uint32_t srcm;

        if (msk0 == 0x00) goto transSkip;

        srcm = ByteUtil::byte2x2_muldiv255_u_pack0213(src0orig, src1orig, msk0);
        if (srcm == 0x00000000) goto transSkip;

        dst0 = ByteUtil::packed_muldiv255(READ_32(dst), ByteUtil::scalar_neg255(srcm >> 24));
        dst0 += srcm;
        dst0 |= 0xFF000000;

        ((uint32_t*)dst)[0] = dst0;

transSkip:
        dst += 4;
        msk += 1;
      } while (--i);
    }
  }

  static void FOG_FASTCALL xrgb32_cspan_a8_const(
    uint8_t* dst, const Solid* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    uint32_t src0 = ByteUtil::packed_muldiv255(src->prgb, msk0);
    if (!src0) return;

    msk0 = ByteUtil::scalar_neg255(src0 >> 24);
    src0 |= 0xFF000000;

    do {
      ((uint32_t*)dst)[0] = ByteUtil::packed_muldiv255_0xxx(READ_32(dst), msk0) + src0;

      dst += 4;
    } while (--i);
  }

  static void FOG_FASTCALL xrgb32_cspan_a8_scanline(
    uint8_t* dst, const Solid* src, const Scanline32::Span* spans, sysuint_t numSpans, const Closure* closure)
  {
    uint32_t srcp = src->prgb;

    ByteUtil::byte1x2 src0orig, src1orig;
    ByteUtil::byte2x2_unpack_0213(src0orig, src1orig, srcp);

    if (ArgbUtil::isAlpha0xFF(srcp))
    {
      BLIT_CSPAN_SCANLINE_STEP1_BEGIN(4)

      // Const mask.
      BLIT_CSPAN_SCANLINE_STEP2_CONST()
      {
        sysint_t i = w;
        FOG_ASSERT(w);

        if (msk0 == 255)
        {
          do {
            ((uint32_t*)dst)[0] = srcp;
            dst += 4;
          } while (--i);
        }
        else
        {
          uint32_t src0 = ByteUtil::packed_muldiv255(src->prgb, msk0);
          if (!src0) return;

          msk0 = ByteUtil::scalar_neg255(src0 >> 24);

          do {
            ((uint32_t*)dst)[0] = ByteUtil::packed_muldiv255(READ_32(dst), msk0) + src0;

            dst += 4;
          } while (--i);
        }
      }
      // Variable mask.
      BLIT_CSPAN_SCANLINE_STEP3_MASK()
      {
        sysint_t i = w;
        FOG_ASSERT(w);

        do {
          uint32_t msk0 = READ_8(msk);
          uint32_t dst0;
          uint32_t src0;

          dst0 = READ_32(dst);
          src0 = ByteUtil::byte2x2_muldiv255_u_pack0213(src0orig, src1orig, msk0);
          dst0 = ByteUtil::packed_muldiv255(dst0, ByteUtil::scalar_neg255(msk0));

          ((uint32_t*)dst)[0] = src0 + dst0;

          dst += 4;
          msk += 1;
        } while (--i);
      }
      BLIT_CSPAN_SCANLINE_STEP4_END()
    }
    else
    {
      BLIT_CSPAN_SCANLINE_STEP1_BEGIN(4)

      // Const mask.
      BLIT_CSPAN_SCANLINE_STEP2_CONST()
      {
        sysint_t i = w;
        FOG_ASSERT(w);

        uint32_t srcm = srcp;
        if (msk0 != 0xFF) srcm = ByteUtil::byte2x2_muldiv255_u_pack0213(src0orig, src1orig, msk0);

        msk0 = ByteUtil::scalar_neg255(srcm >> 24);
        srcm |= 0xFF000000;

        do {
          ((uint32_t*)dst)[0] = ByteUtil::packed_muldiv255_0xxx(READ_32(dst), msk0) + srcm;

          dst += 4;
        } while (--i);
      }
      // Variable mask.
      BLIT_CSPAN_SCANLINE_STEP3_MASK()
      {
        sysint_t i = w;
        FOG_ASSERT(w);

        do {
          uint32_t dst0 = READ_32(dst);
          uint32_t src0;
          uint32_t msk0 = READ_8(msk);

          src0 = ByteUtil::byte2x2_muldiv255_u_pack0213(src0orig, src1orig, msk0);
          dst0 = ByteUtil::packed_muldiv255(dst0, ByteUtil::scalar_neg255(src0 >> 24));

          ((uint32_t*)dst)[0] = (dst0 + src0) | 0xFF000000;

          dst += 4;
          msk += 1;
        } while (--i);
      }
      BLIT_CSPAN_SCANLINE_STEP4_END()
    }
  }

  static void FOG_FASTCALL xrgb32_vspan_prgb32(
    uint8_t* dst, const uint8_t* src, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    do {
      uint32_t dst0;
      uint32_t src0;

      src0 = READ_32(src);
      if (src0 == 0x00000000) goto skip;
      if (ArgbUtil::isAlpha0xFF(src0)) goto fill;

      dst0 = READ_32(dst);
      dst0 = ByteUtil::packed_muldiv255(dst0, ByteUtil::scalar_neg255(src0 >> 24));

      src0 += dst0;
      src0 |= 0xFF000000;

fill:
      ((uint32_t*)dst)[0] = src0;

skip:
      dst += 4;
      src += 4;
    } while (--i);
  }

  static void FOG_FASTCALL xrgb32_vspan_argb32(
    uint8_t* dst, const uint8_t* src, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    do {
      uint32_t dst0;
      uint32_t src0;
      uint32_t srca;

      src0 = READ_32(src);
      if (ArgbUtil::isAlpha0x00(src0)) goto skip;
      if (ArgbUtil::isAlpha0xFF(src0)) goto fill;

      srca = src0 >> 24;
      dst0 = READ_32(dst);

      src0 = ByteUtil::packed_muldiv255_0xxx(src0, srca) | (srca << 24);
      dst0 = ByteUtil::packed_muldiv255(dst0, ByteUtil::scalar_neg255(srca));

      src0 += dst0;
      src0 |= 0xFF000000;

fill:
      ((uint32_t*)dst)[0] = src0;

skip:
      dst += 4;
      src += 4;
    } while (--i);
  }

  static void FOG_FASTCALL xrgb32_vspan_i8(
    uint8_t* dst, const uint8_t* src, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    const Argb* pal = closure->srcPalette;

    do {
      uint32_t dst0;
      uint32_t src0;

      src0 = pal[READ_8(src)];
      if (src0 != 0x00000000) goto skip;
      if (ArgbUtil::isAlpha0xFF(src0)) goto fill;

      dst0 = READ_32(dst);
      dst0 = ByteUtil::packed_muldiv255(dst0, ByteUtil::scalar_neg255(src0 >> 24));

      src0 += dst0;
      src0 |= 0xFF000000;

fill:
      ((uint32_t*)dst)[0] = src0;

skip:
      dst += 4;
      src += 1;
    } while (--i);
  }

  static void FOG_FASTCALL xrgb32_vspan_prgb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    do {
      uint32_t dst0;
      uint32_t src0;
      uint32_t msk0 = READ_8(msk);

      if (msk0 != 0x00) goto skip;

      src0 = READ_32(src);
      if (msk0 == 0xFF && ArgbUtil::isAlpha0xFF(src0)) goto fill;

      dst0 = READ_32(dst);

      src0 = ByteUtil::packed_muldiv255(src0, msk0);
      dst0 = ByteUtil::packed_muldiv255(dst0, ByteUtil::scalar_neg255(src0 >> 24));

      src0 += dst0;
      src0 |= 0xFF000000;

fill:
      ((uint32_t*)dst)[0] = src0;

skip:
      dst += 4;
      src += 4;
      msk += 1;
    } while (--i);
  }

  static void FOG_FASTCALL xrgb32_vspan_argb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    do {
      uint32_t dst0;
      uint32_t src0;
      uint32_t msk0 = READ_8(msk);

      if (msk0 != 0x00) goto skip;

      src0 = READ_32(src);
      if (msk0 == 0xFF && ArgbUtil::isAlpha0xFF(src0)) goto fill;

      dst0 = READ_32(dst);

      src0 = ByteUtil::packed_muldiv255(src0 | 0xFF000000, ByteUtil::scalar_muldiv255(src0 >> 24, msk0));
      dst0 = ByteUtil::packed_muldiv255(dst0, ByteUtil::scalar_neg255(src0 >> 24));

      src0 += dst0;
      src0 |= 0xFF000000;

fill:
      ((uint32_t*)dst)[0] = src0;

skip:
      dst += 4;
      src += 4;
      msk += 1;
    } while (--i);
  }

  static void FOG_FASTCALL xrgb32_vspan_i8_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    const Argb* pal = closure->srcPalette;

    do {
      uint32_t dst0;
      uint32_t src0;
      uint32_t msk0 = READ_8(msk);

      if (msk0 != 0x00) goto skip;

      src0 = pal[READ_8(src)];
      if (msk0 == 0xFF && ArgbUtil::isAlpha0xFF(src0)) goto fill;

      dst0 = READ_32(dst);

      src0 = ByteUtil::packed_muldiv255(src0, msk0);
      dst0 = ByteUtil::packed_muldiv255(dst0, ByteUtil::scalar_neg255(src0 >> 24));

      src0 += dst0;
      src0 |= 0xFF000000;

fill:
      ((uint32_t*)dst)[0] = src0;

skip:
      dst += 4;
      src += 1;
      msk += 1;
    } while (--i);
  }

  static void FOG_FASTCALL xrgb32_vspan_prgb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    do {
      uint32_t dst0 = READ_32(dst);
      uint32_t src0 = READ_32(src);

      src0 = ByteUtil::packed_muldiv255(src0, msk0);
      dst0 = ByteUtil::packed_muldiv255_0xxx(dst0, ByteUtil::scalar_neg255(src0 >> 24));
      ((uint32_t*)dst)[0] = (src0 + dst0) | 0xFF000000;

      dst += 4;
      src += 4;
    } while (--i);
  }

  static void FOG_FASTCALL xrgb32_vspan_argb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    do {
      uint32_t dst0 = READ_32(dst);
      uint32_t src0 = READ_32(src);
      uint32_t srca = ByteUtil::scalar_muldiv255(src0 >> 24, msk0);

      src0 = ByteUtil::packed_muldiv255_Fxxx(src0, srca);
      dst0 = ByteUtil::packed_muldiv255_0xxx(dst0, ByteUtil::scalar_neg255(srca));
      ((uint32_t*)dst)[0] = src0 + dst0;

      dst += 4;
      src += 4;
    } while (--i);
  }

  static void FOG_FASTCALL xrgb32_vspan_i8_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    const Argb* pal = closure->srcPalette;

    do {
      uint32_t dst0 = READ_32(dst);
      uint32_t src0 = pal[READ_8(src)];

      src0 = ByteUtil::packed_muldiv255_Fxxx(src0, msk0);
      dst0 = ByteUtil::packed_muldiv255_0xxx(dst0, ByteUtil::scalar_neg255(src0 >> 24));
      ((uint32_t*)dst)[0] = src0 + dst0;

      dst += 4;
      src += 1;
    } while (--i);
  }
};

// ============================================================================
// [Fog::RasterEngine::C - Composite - DstOver]
// ============================================================================

struct CompositeDstOverC : public CompositeBaseFuncsC32<CompositeDstOverC>
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

// ============================================================================
// [Fog::RasterEngine::C - Composite - SrcIn]
// ============================================================================

struct CompositeSrcInC : public CompositeBaseFuncsC32<CompositeSrcInC>
{
  enum { CHARACTERISTICS = OPERATOR_CHAR_SRC_IN };

  static FOG_INLINE void prgb32_op_prgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    ByteUtil::byte2x2_muldiv255_u(dst0, b0, dst1, b1, ByteUtil::byte1x2_hi(a1));
  }

  static FOG_INLINE void prgb32_op_xrgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    ByteUtil::byte2x2_muldiv255_u(dst0, b0, dst1, b1 | 0x00FF0000, ByteUtil::byte1x2_hi(a1));
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

// ============================================================================
// [Fog::RasterEngine::C - Composite - DstIn]
// ============================================================================

struct CompositeDstInC : public CompositeBaseFuncsC32<CompositeDstInC>
{
  enum { CHARACTERISTICS = OPERATOR_CHAR_DST_IN };

  static FOG_INLINE void prgb32_op_prgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    ByteUtil::byte2x2_muldiv255_u(dst0, a0, dst1, a1, ByteUtil::byte1x2_hi(b1));
  }

  static FOG_INLINE void prgb32_op_xrgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    // NOT USED!
    FOG_ASSERT_NOT_REACHED();
  }

  static FOG_INLINE void xrgb32_op_prgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    ByteUtil::byte2x2_muldiv255_u(dst0, a0, dst1, a1, ByteUtil::byte1x2_hi(b1));
  }

  static FOG_INLINE void xrgb32_op_xrgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    // NOT USED!
    FOG_ASSERT_NOT_REACHED();
  }

  static void FOG_FASTCALL prgb32_vspan_prgb32_or_argb32(
    uint8_t* dst, const uint8_t* src, sysint_t w, const Closure* closure)
  {
    // TODO
  }
  static void FOG_FASTCALL prgb32_vspan_prgb32_or_argb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    // TODO
  }

  static void FOG_FASTCALL prgb32_vspan_prgb32_or_argb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    // TODO
  }
};

// ============================================================================
// [Fog::RasterEngine::C - Composite - SrcOut]
// ============================================================================

struct CompositeSrcOutC : public CompositeBaseFuncsC32<CompositeSrcOutC>
{
  enum { CHARACTERISTICS = OPERATOR_CHAR_SRC_OUT };

  static FOG_INLINE void prgb32_op_prgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    ByteUtil::byte2x2_muldiv255_u(dst0, b0, dst1, b1, ByteUtil::scalar_neg255(ByteUtil::byte1x2_hi(a1)));
  }

  static FOG_INLINE void prgb32_op_xrgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    ByteUtil::byte2x2_muldiv255_u(dst0, b0, dst1, b1 | 0x00FF0000, ByteUtil::scalar_neg255(ByteUtil::byte1x2_hi(a1)));
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

// ============================================================================
// [Fog::RasterEngine::C - Composite - DstOut]
// ============================================================================

struct CompositeDstOutC : public CompositeBaseFuncsC32<CompositeDstOutC>
{
  enum { CHARACTERISTICS = OPERATOR_CHAR_DST_OUT };

  static FOG_INLINE void prgb32_op_prgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    ByteUtil::byte2x2_muldiv255_u(dst0, a0, dst1, a1, ByteUtil::scalar_neg255(ByteUtil::byte1x2_hi(b1)));
  }

  static FOG_INLINE void prgb32_op_xrgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    // NOT USED!
    FOG_ASSERT_NOT_REACHED();
  }

  static FOG_INLINE void xrgb32_op_prgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    ByteUtil::byte2x2_muldiv255_u(dst0, a0, dst1, a1, ByteUtil::scalar_neg255(ByteUtil::byte1x2_hi(b1)));
  }

  static FOG_INLINE void xrgb32_op_xrgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    // NOT USED!
    FOG_ASSERT_NOT_REACHED();
  }

  static void FOG_FASTCALL prgb32_vspan_prgb32_or_argb32(
    uint8_t* dst, const uint8_t* src, sysint_t w, const Closure* closure)
  {
    // TODO
  }

  static void FOG_FASTCALL prgb32_vspan_a8(
    uint8_t* dst, const uint8_t* src, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    do {
      uint32_t dst0;
      uint32_t src0;
      uint32_t srca;

      srca = READ_8(src);

      if (srca == 0x00) goto skip;
      if (srca == 0xFF) goto fill;

      src0 = srca << 24;
      dst0 = READ_32(dst);

      dst0 = ByteUtil::packed_muldiv255(dst0, ByteUtil::scalar_neg255(srca));
      src0 += dst0;

fill:
      ((uint32_t*)dst)[0] = src0;

skip:
      dst += 4;
      src += 1;
    } while (--i);
  }

  static void FOG_FASTCALL prgb32_vspan_prgb32_or_argb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    // TODO
  }

  static void FOG_FASTCALL prgb32_vspan_a8_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    do {
      ByteUtil::byte1x2 dst0;
      ByteUtil::byte1x2 src0 = READ_8(src);
      uint32_t msk0 = READ_8(msk);

      if (msk0 == 0x00) goto skip;

      dst0 = READ_32(dst);
      msk0 = ByteUtil::scalar_neg255(ByteUtil::scalar_muldiv255(msk0, src0));

      ((uint32_t*)dst)[0] = ByteUtil::packed_muldiv255(dst0, msk0);

skip:
      dst += 4;
      src += 1;
      msk += 1;
    } while (--i);
  }

  static void FOG_FASTCALL prgb32_vspan_prgb32_or_argb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    // TODO
  }

  static void FOG_FASTCALL prgb32_vspan_a8_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    do {
      ByteUtil::byte1x2 dst0 = READ_32(dst);
      ByteUtil::byte1x2 src0 = READ_8(src);

      uint32_t msk0inv = ByteUtil::scalar_neg255(ByteUtil::scalar_muldiv255(msk0, src0));
      ((uint32_t*)dst)[0] = ByteUtil::packed_muldiv255(dst0, msk0inv);

      dst += 4;
      src += 1;
    } while (--i);
  }
};

// ============================================================================
// [Fog::RasterEngine::C - Composite - SrcAtop]
// ============================================================================

struct CompositeSrcAtopC : public CompositeBaseFuncsC32<CompositeSrcAtopC>
{
  enum { CHARACTERISTICS = OPERATOR_CHAR_SRC_ATOP };

  static FOG_INLINE void prgb32_op_prgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    uint32_t aa = ByteUtil::byte1x2_hi(a1);
    uint32_t bainv = ByteUtil::scalar_neg255(ByteUtil::byte1x2_hi(b1));

    ByteUtil::byte2x2_muldiv255_u_2x_add(
      dst0, a0, bainv, b0, aa,
      dst1, a1, bainv, b1, aa);
  }

  static FOG_INLINE void prgb32_op_xrgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    // NOT USED!
    FOG_ASSERT_NOT_REACHED();
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

// ============================================================================
// [Fog::RasterEngine::C - Composite - DstAtop]
// ============================================================================

struct CompositeDstAtopC : public CompositeBaseFuncsC32<CompositeDstAtopC>
{
  enum { CHARACTERISTICS = OPERATOR_CHAR_DST_ATOP };

  static FOG_INLINE void prgb32_op_prgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    uint32_t aainv = ByteUtil::scalar_neg255(ByteUtil::byte1x2_hi(a1));
    uint32_t ba = ByteUtil::byte1x2_hi(b1);

    ByteUtil::byte2x2_muldiv255_u_2x_add(
      dst0, a0, ba, b0, aainv,
      dst1, a1, ba, b1, aainv);
  }

  static FOG_INLINE void prgb32_op_xrgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    // NOT USED!
    FOG_ASSERT_NOT_REACHED();
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

// ============================================================================
// [Fog::RasterEngine::C - Composite - Xor]
// ============================================================================

struct CompositeXorC : public CompositeBaseFuncsC32<CompositeXorC>
{
  enum { CHARACTERISTICS = OPERATOR_CHAR_XOR };

  static FOG_INLINE void prgb32_op_prgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    uint32_t aainv = ByteUtil::scalar_neg255(ByteUtil::byte1x2_hi(a1));
    uint32_t bainv = ByteUtil::scalar_neg255(ByteUtil::byte1x2_hi(b1));

    ByteUtil::byte2x2_muldiv255_u_2x_add(
      dst0, a0, bainv, b0, aainv,
      dst1, a1, bainv, b1, aainv);
  }

  static FOG_INLINE void prgb32_op_xrgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    // NOT USED!
    FOG_ASSERT_NOT_REACHED();
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

// ============================================================================
// [Fog::RasterEngine::C - Composite - Add]
// ============================================================================

struct CompositeAddC : public CompositeBaseFuncsC32<CompositeAddC>
{
  enum { CHARACTERISTICS = OPERATOR_CHAR_ADD };

  static FOG_INLINE void prgb32_op_prgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    ByteUtil::byte2x2_addus_byte2x2(dst0, a0, b0, dst1, a1, b1);
  }

  static FOG_INLINE void prgb32_op_xrgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    ByteUtil::byte2x2_addus_byte2x2(dst0, a0, b0, dst1, a1, b1);
    ByteUtil::byte1x2_fill_hi(dst1, dst1);
  }

  static FOG_INLINE void xrgb32_op_prgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    ByteUtil::byte2x2_addus_byte2x2(dst0, a0, b0, dst1, a1, b1);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    ByteUtil::byte2x2_addus_byte2x2(dst0, a0, b0, dst1, a1, b1);
  }
};

// ============================================================================
// [Fog::RasterEngine::C - Composite - Subtract]
// ============================================================================

struct CompositeSubtractC : public CompositeBaseFuncsC32<CompositeSubtractC>
{
  enum { CHARACTERISTICS = OPERATOR_CHAR_SUBTRACT };

  static FOG_INLINE void prgb32_op_prgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    uint32_t aa = ByteUtil::byte1x2_hi(a1);
    uint32_t ba = ByteUtil::byte1x2_hi(b1);

    ByteUtil::byte2x2_subus_byte2x2(dst0, a0, b0, dst1, a1, b1);
    ByteUtil::byte1x2_set_hi(dst1, dst1, (aa + ba - ByteUtil::scalar_muldiv255(aa, ba)));
  }

  static FOG_INLINE void prgb32_op_xrgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    ByteUtil::byte2x2_subus_byte2x2(dst0, a0, b0, dst1, a1, b1);
    ByteUtil::byte1x2_fill_hi(dst1, dst1);
  }

  static FOG_INLINE void xrgb32_op_prgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    ByteUtil::byte2x2_subus_byte2x2(dst0, a0, b0, dst1, a1, b1);
    ByteUtil::byte1x2_fill_hi(dst1, dst1);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    ByteUtil::byte2x2_subus_byte2x2(dst0, a0, b0, dst1, a1, b1);
    ByteUtil::byte1x2_fill_hi(dst1, dst1);
  }
};

// ============================================================================
// [Fog::RasterEngine::C - Composite - Multiply]
// ============================================================================

struct CompositeMultiplyC : public CompositeBaseFuncsC32<CompositeMultiplyC>
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

// ============================================================================
// [Fog::RasterEngine::C - Composite - Screen]
// ============================================================================

struct CompositeScreenC : public CompositeBaseFuncsC32<CompositeScreenC>
{
  enum { CHARACTERISTICS = OPERATOR_CHAR_SCREEN };

  static FOG_INLINE void prgb32_op_prgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    uint32_t b0inv, b1inv;

    ByteUtil::byte2x2_negate(b0inv, b0, b1inv, b1);
    ByteUtil::byte2x2_muldiv255_byte2x2(dst0, a0, b0inv, dst1, a1, b1inv);
    ByteUtil::byte2x2_add_byte2x2(dst0, dst0, b0, dst1, dst1, b1);
  }

  static FOG_INLINE void prgb32_op_xrgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    uint32_t b0inv, b1inv;

    ByteUtil::byte2x2_negate(b0inv, b0, b1inv, b1);
    ByteUtil::byte2x2_muldiv255_byte2x2(dst0, a0, b0inv, dst1, a1, b1inv);
    ByteUtil::byte2x2_add_byte2x2(dst0, dst0, b0, dst1, dst1, b1);
    ByteUtil::byte1x2_fill_hi(dst1, dst1);
  }

  static FOG_INLINE void xrgb32_op_prgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    uint32_t b0inv, b1inv;

    ByteUtil::byte2x2_negate(b0inv, b0, b1inv, b1);
    ByteUtil::byte2x2_muldiv255_byte2x2(dst0, a0, b0inv, dst1, a1, b1inv);
    ByteUtil::byte2x2_add_byte2x2(dst0, dst0, b0, dst1, dst1, b1);
    ByteUtil::byte1x2_fill_hi(dst1, dst1);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    uint32_t b0inv, b1inv;

    ByteUtil::byte2x2_negate(b0inv, b0, b1inv, b1);
    ByteUtil::byte2x2_muldiv255_byte2x2(dst0, a0, b0inv, dst1, a1, b1inv);
    ByteUtil::byte2x2_add_byte2x2(dst0, dst0, b0, dst1, dst1, b1);
    ByteUtil::byte1x2_fill_hi(dst1, dst1);
  }
};

// ============================================================================
// [Fog::RasterEngine::C - Composite - Darken]
// ============================================================================

struct CompositeDarkenC : public CompositeBaseFuncsC32<CompositeDarkenC>
{
  enum { CHARACTERISTICS = OPERATOR_CHAR_DARKEN };

  static FOG_INLINE void prgb32_op_prgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    uint32_t aa = ByteUtil::byte1x2_hi(a1);
    uint32_t ba = ByteUtil::byte1x2_hi(b1);

    uint32_t at0, at1;
    uint32_t bt0, bt1;

    ByteUtil::byte2x2_muldiv255_u(at0, a0, at1, a1, ba);
    ByteUtil::byte2x2_muldiv255_u(bt0, b0, bt1, b1, aa);

    ByteUtil::byte2x2_min_byte2x2(dst0, at0, bt0, dst1, at1, bt1);

    ByteUtil::byte2x2_sub_byte2x2(at0, a0, at0, at1, a1, at1);
    ByteUtil::byte2x2_sub_byte2x2(bt0, b0, bt0, bt1, b1, bt1);

    ByteUtil::byte2x2_add_byte2x2(dst0, dst0, at0, dst1, dst1, at1);
    ByteUtil::byte2x2_add_byte2x2(dst0, dst0, bt0, dst1, dst1, bt1);
  }

  static FOG_INLINE void prgb32_op_xrgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    uint32_t aa = ByteUtil::byte1x2_hi(a1);
    uint32_t bt0, bt1;

    ByteUtil::byte2x2_muldiv255_u(bt0, b0, aa, bt1, b1, aa);

    ByteUtil::byte2x2_min_byte2x2(dst0, a0, bt0, dst1, a1, bt1);
    ByteUtil::byte2x2_sub_byte2x2(bt0, b0, bt0, bt1, b1, bt1);
    ByteUtil::byte2x2_add_byte2x2(dst0, dst0, bt0, dst1, dst1, bt1);

    ByteUtil::byte1x2_fill_hi(dst1, dst1);
  }

  static FOG_INLINE void xrgb32_op_prgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    uint32_t ba = ByteUtil::byte1x2_hi(b1);
    uint32_t at0, at1;

    ByteUtil::byte2x2_muldiv255_u(at0, a0, ba, at1, a1, ba);

    ByteUtil::byte2x2_min_byte2x2(dst0, at0, b0, dst1, at1, b1);
    ByteUtil::byte2x2_sub_byte2x2(at0, a0, at0, at1, a1, at1);
    ByteUtil::byte2x2_add_byte2x2(dst0, dst0, at0, dst1, dst1, at1);

    ByteUtil::byte1x2_fill_hi(dst1, dst1);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    ByteUtil::byte2x2_min_byte2x2(dst0, a0, b0, dst1, a1, b1);
    ByteUtil::byte1x2_fill_hi(dst1, dst1);
  }
};

// ============================================================================
// [Fog::RasterEngine::C - Composite - Lighten]
// ============================================================================

struct CompositeLightenC : public CompositeBaseFuncsC32<CompositeLightenC>
{
  enum { CHARACTERISTICS = OPERATOR_CHAR_LIGHTEN };

  static FOG_INLINE void prgb32_op_prgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    uint32_t aa = ByteUtil::byte1x2_hi(a1);
    uint32_t ba = ByteUtil::byte1x2_hi(b1);

    uint32_t at0, at1;
    uint32_t bt0, bt1;

    ByteUtil::byte2x2_muldiv255_u(at0, a0, ba, at1, a1, ba);
    ByteUtil::byte2x2_muldiv255_u(bt0, b0, aa, bt1, b1, aa);

    ByteUtil::byte2x2_max_byte2x2(dst0, at0, bt0, dst1, at1, bt1);

    ByteUtil::byte2x2_sub_byte2x2(at0, a0, at0, at1, a1, at1);
    ByteUtil::byte2x2_sub_byte2x2(bt0, b0, bt0, bt1, b1, bt1);

    ByteUtil::byte2x2_add_byte2x2(dst0, dst0, at0, dst1, dst1, at1);
    ByteUtil::byte2x2_add_byte2x2(dst0, dst0, bt0, dst1, dst1, bt1);
  }

  static FOG_INLINE void prgb32_op_xrgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    uint32_t aa = ByteUtil::byte1x2_hi(a1);
    uint32_t bt0, bt1;

    ByteUtil::byte2x2_muldiv255_u(bt0, b0, aa, bt1, b1, aa);

    ByteUtil::byte2x2_max_byte2x2(dst0, a0, bt0, dst1, a1, bt1);
    ByteUtil::byte2x2_sub_byte2x2(bt0, b0, bt0, bt1, b1, bt1);
    ByteUtil::byte2x2_add_byte2x2(dst0, dst0, bt0, dst1, dst1, bt1);

    ByteUtil::byte1x2_fill_hi(dst1, dst1);
  }

  static FOG_INLINE void xrgb32_op_prgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    uint32_t ba = ByteUtil::byte1x2_hi(b1);
    uint32_t at0, at1;

    ByteUtil::byte2x2_muldiv255_u(at0, a0, ba, at1, a1, ba);

    ByteUtil::byte2x2_max_byte2x2(dst0, at0, b0, dst1, at1, b1);
    ByteUtil::byte2x2_sub_byte2x2(at0, a0, at0, at1, a1, at1);
    ByteUtil::byte2x2_add_byte2x2(dst0, dst0, at0, dst1, dst1, at1);

    ByteUtil::byte1x2_fill_hi(dst1, dst1);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    ByteUtil::byte2x2_max_byte2x2(dst0, a0, b0, dst1, a1, b1);
    ByteUtil::byte1x2_fill_hi(dst1, dst1);
  }
};

// ============================================================================
// [Fog::RasterEngine::C - Composite - Difference]
// ============================================================================

struct CompositeDifferenceC : public CompositeBaseFuncsC32<CompositeDifferenceC>
{
  enum { CHARACTERISTICS = OPERATOR_CHAR_DIFFERENCE };

  static FOG_INLINE void prgb32_op_prgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    uint32_t aa = ByteUtil::byte1x2_hi(a1);
    uint32_t ba = ByteUtil::byte1x2_hi(b1);

    uint32_t at0, at1;
    uint32_t bt0, bt1;

    ByteUtil::byte2x2_muldiv255_u(at0, a0, at1, a1, ba);
    ByteUtil::byte2x2_muldiv255_u(bt0, b0, bt1, b1, aa);

    ByteUtil::byte2x2_min_byte2x2(at0, at0, bt0, at1, at1, bt1);

    ByteUtil::byte2x2_add_byte2x2(dst0, a0, b0, dst1, a1, b1);
    ByteUtil::byte2x2_sub_byte2x2(dst0, dst0, at0, dst1, dst1, at1);
    ByteUtil::byte1x2_zero_hi(at1, at1);
    ByteUtil::byte2x2_subus_byte2x2(dst0, dst0, at0, dst1, dst1, at1);
  }

  static FOG_INLINE void prgb32_op_xrgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    uint32_t aa = ByteUtil::byte1x2_hi(a1);
    uint32_t bt0, bt1;

    ByteUtil::byte2x2_muldiv255_u(bt0, b0, bt1, b1, aa);
    ByteUtil::byte2x2_min_byte2x2(bt0, bt0, a0, bt1, bt1, a1);

    ByteUtil::byte2x2_add_byte2x2(dst0, a0, b0, dst1, a1, b1);
    ByteUtil::byte2x2_sub_byte2x2(dst0, dst0, bt0, dst1, dst1, bt1);
    ByteUtil::byte2x2_subus_byte2x2(dst0, dst0, bt0, dst1, dst1, bt1);

    ByteUtil::byte1x2_fill_hi(dst1, dst1);
  }

  static FOG_INLINE void xrgb32_op_prgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    uint32_t ba = ByteUtil::byte1x2_hi(b1);
    uint32_t at0, at1;

    ByteUtil::byte2x2_muldiv255_u(at0, a0, at1, a1, ba);
    ByteUtil::byte2x2_min_byte2x2(at0, at0, b0, at1, at1, b1);

    ByteUtil::byte2x2_add_byte2x2(dst0, a0, b0, dst1, a1, b1);
    ByteUtil::byte2x2_sub_byte2x2(dst0, dst0, at0, dst1, dst1, at1);
    ByteUtil::byte2x2_subus_byte2x2(dst0, dst0, at0, dst1, dst1, at1);

    ByteUtil::byte1x2_fill_hi(dst1, dst1);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    ByteUtil::byte2x2_add_byte2x2(dst0, a0, b0, dst1, a1, b1);
    ByteUtil::byte2x2_min_byte2x2(a0, a0, b0, a1, a1, b1);
    ByteUtil::byte2x2_sub_byte2x2(dst0, dst0, a0, dst1, dst1, a1);
    ByteUtil::byte2x2_subus_byte2x2(dst0, dst0, a0, dst1, dst1, a1);

    ByteUtil::byte1x2_fill_hi(dst1, dst1);
  }
};

// ============================================================================
// [Fog::RasterEngine::C - Composite - Exclusion]
// ============================================================================

struct CompositeExclusionC : public CompositeBaseFuncsC32<CompositeExclusionC>
{
  enum { CHARACTERISTICS = OPERATOR_CHAR_EXCLUSION };

  static FOG_INLINE void prgb32_op_prgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    uint32_t aa = ByteUtil::byte1x2_hi(a1);

    ByteUtil::byte2x2_muldiv255_u(dst0, b0, dst1, b1, aa);
    ByteUtil::byte2x2_add_byte2x2(dst0, dst0, a0, dst1, dst1, a1);

    ByteUtil::byte2x2_muldiv255_byte2x2(a0, a0, b0, a1, a1, b1);

    ByteUtil::byte2x2_sub_byte2x2(dst0, dst0, a0, dst1, dst1, a1);
    ByteUtil::byte2x2_subus_byte2x2(dst0, dst0, a0, dst1, dst1, a1);
  }

  static FOG_INLINE void prgb32_op_xrgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    uint32_t aa = ByteUtil::byte1x2_hi(a1);

    ByteUtil::byte2x2_muldiv255_u(dst0, b0, dst1, b1, aa);
    ByteUtil::byte2x2_add_byte2x2(dst0, dst0, a0, dst1, dst1, a1);

    ByteUtil::byte2x2_muldiv255_byte2x2(a0, a0, b0, a1, a1, b1);

    ByteUtil::byte2x2_sub_byte2x2(dst0, dst0, a0, dst1, dst1, a1);
    ByteUtil::byte2x2_subus_byte2x2(dst0, dst0, a0, dst1, dst1, a1);

    ByteUtil::byte1x2_fill_hi(dst1, dst1);
  }

  static FOG_INLINE void xrgb32_op_prgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    ByteUtil::byte2x2_add_byte2x2(dst0, a0, b0, dst1, a1, b1);
    ByteUtil::byte2x2_muldiv255_byte2x2(a0, a0, b0, a1, a1, b1);

    ByteUtil::byte2x2_sub_byte2x2(dst0, dst0, a0, dst1, dst1, a1);
    ByteUtil::byte2x2_subus_byte2x2(dst0, dst0, a0, dst1, dst1, a1);

    ByteUtil::byte1x2_fill_hi(dst1, dst1);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    ByteUtil::byte2x2_add_byte2x2(dst0, a0, b0, dst1, a1, b1);
    ByteUtil::byte2x2_muldiv255_byte2x2(a0, a0, b0, a1, a1, b1);

    ByteUtil::byte2x2_sub_byte2x2(dst0, dst0, a0, dst1, dst1, a1);
    ByteUtil::byte2x2_subus_byte2x2(dst0, dst0, a0, dst1, dst1, a1);

    ByteUtil::byte1x2_fill_hi(dst1, dst1);
  }
};

// ============================================================================
// [Fog::RasterEngine::C - Composite - Invert]
// ============================================================================

struct CompositeInvertC : public CompositeBaseFuncsC32<CompositeInvertC>
{
  enum { CHARACTERISTICS = OPERATOR_CHAR_INVERT };

  static FOG_INLINE void prgb32_op_prgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    uint32_t aa;
    ByteUtil::byte1x2_expand_hi(aa, a1);
    ByteUtil::byte2x2_sub_byte2x2(dst0, aa, a0, dst1, aa, a1);

    uint32_t ba = ByteUtil::byte1x2_hi(b1);
    ByteUtil::byte2x2_muldiv255_u(dst0, dst0, dst1, dst1, ba);

    ByteUtil::byte2x2_muldiv255_u(a0, a0, a1, a1, ByteUtil::scalar_neg255(ba));
    ByteUtil::byte2x2_addus_byte2x2(dst0, dst0, a0, dst1, dst1, a1);
  }

  static FOG_INLINE void prgb32_op_xrgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    uint32_t aa;
    ByteUtil::byte1x2_expand_hi(aa, a1);
    ByteUtil::byte2x2_sub_byte2x2(dst0, aa, a0, dst1, aa, a1);

    ByteUtil::byte1x2_fill_hi(dst1, dst1);
  }

  static FOG_INLINE void xrgb32_op_prgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    ByteUtil::byte2x2_negate(dst0, a0, dst1, a1);

    uint32_t ba = ByteUtil::byte1x2_hi(b1);
    ByteUtil::byte2x2_muldiv255_u(dst0, dst0, dst1, dst1, ba);

    ByteUtil::byte2x2_muldiv255_u(a0, a0, a1, a1, ByteUtil::scalar_neg255(ba));
    ByteUtil::byte2x2_addus_byte2x2(dst0, dst0, a0, dst1, dst1, a1);

    ByteUtil::byte1x2_fill_hi(dst1, dst1);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    ByteUtil::byte2x2_negate(dst0, a0, dst1, a1);
    ByteUtil::byte1x2_fill_hi(dst1, dst1);
  }
};

// ============================================================================
// [Fog::RasterEngine::C - Composite - InvertRgb]
// ============================================================================

struct CompositeInvertRgbC : public CompositeBaseFuncsC32<CompositeInvertRgbC>
{
  enum { CHARACTERISTICS = OPERATOR_CHAR_INVERT_RGB };

  static FOG_INLINE void prgb32_op_prgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    uint32_t aa;
    ByteUtil::byte1x2_expand_hi(aa, a1);
    ByteUtil::byte2x2_sub_byte2x2(dst0, aa, a0, dst1, aa, a1);
    ByteUtil::byte1x2_fill_hi(dst1, dst1);

    ByteUtil::byte2x2_muldiv255_byte2x2(dst0, dst0, b0, dst1, dst1, b1);

    uint32_t ba = ByteUtil::scalar_neg255(ByteUtil::byte1x2_hi(b1));
    ByteUtil::byte2x2_muldiv255_u(a0, a0, a1, a1, ba);

    ByteUtil::byte2x2_addus_byte2x2(dst0, dst0, a0, dst1, dst1, a1);
  }

  static FOG_INLINE void prgb32_op_xrgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    uint32_t aa;
    ByteUtil::byte1x2_expand_hi(aa, a1);
    ByteUtil::byte2x2_sub_byte2x2(dst0, aa, a0, dst1, aa, a1);

    ByteUtil::byte2x2_muldiv255_byte2x2(dst0, dst0, b0, dst1, dst1, b1);
    ByteUtil::byte1x2_fill_hi(dst1, dst1);
  }

  static FOG_INLINE void xrgb32_op_prgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    ByteUtil::byte2x2_negate(dst0, a0, dst1, a1);
    ByteUtil::byte1x2_fill_hi(dst1, dst1);

    ByteUtil::byte2x2_muldiv255_byte2x2(dst0, dst0, b0, dst1, dst1, b1);

    uint32_t ba = ByteUtil::scalar_neg255(ByteUtil::byte1x2_hi(b1));
    ByteUtil::byte2x2_muldiv255_u(a0, a0, a1, a1, ba);

    ByteUtil::byte2x2_addus_byte2x2(dst0, dst0, a0, dst1, dst1, a1);
    ByteUtil::byte1x2_fill_hi(dst1, dst1);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_32b(
    ByteUtil::byte1x2& dst0, ByteUtil::byte1x2 a0, ByteUtil::byte1x2 b0,
    ByteUtil::byte1x2& dst1, ByteUtil::byte1x2 a1, ByteUtil::byte1x2 b1)
  {
    ByteUtil::byte2x2_negate(dst0, a0, dst1, a1);
    ByteUtil::byte1x2_fill_hi(dst1, dst1);

    ByteUtil::byte2x2_muldiv255_byte2x2(dst0, dst0, b0, dst1, dst1, b1);
    ByteUtil::byte1x2_fill_hi(dst1, dst1);
  }
};

} // RasterEngine namespace
} // Fog namespace
