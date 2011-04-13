// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_RENDER_RENDER_C_PHELPERS_P_H
#define _FOG_G2D_RENDER_RENDER_C_PHELPERS_P_H

// [Dependencies]
#include <Fog/G2d/Render/Render_C/CBase_p.h>
#include <Fog/G2d/Render/Render_C/Defs_p.h>

namespace Fog {
namespace Render_C {

// ============================================================================
// [Fog::Render_C - Helpers]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT Helpers
{
  // --------------------------------------------------------------------------
  // [Helpers - Pattern - Any - Repeat]
  // --------------------------------------------------------------------------

  static uint8_t* FOG_FASTCALL p_repeat_prgb32(uint8_t* dst, int baseWidth, int repeatWidth)
  {
    uint8_t* base = dst - ((uint)baseWidth * 4);
    uint8_t* src = base;

    for (;;)
    {
      int i = Math::min(baseWidth, repeatWidth);
      repeatWidth -= i;

      i -= 4;
      while (i >= 0)
      {
        Memory::copy_16(dst, src);

        dst += 16;
        src += 16;
        i -= 4;
      }
      i += 4;

      while (i)
      {
        Memory::copy_4(dst, src);

        dst += 4;
        src += 4;
        i--;
      }
      if (!repeatWidth) break;

      src = base;
      baseWidth <<= 1;
    }

    return dst;
  }

  static uint8_t* FOG_FASTCALL p_repeat_a8(uint8_t* dst, int baseWidth, int repeatWidth)
  {
    uint8_t* base = dst - ((uint)baseWidth);
    uint8_t* src = base;

    for (;;)
    {
      int i = Math::min(baseWidth, repeatWidth);
      repeatWidth -= i;

      memcpy(dst, src, i);
      dst += i;
      if (!repeatWidth) break;

      src = base;
      baseWidth <<= 1;
    }

    return dst;
  }

  // --------------------------------------------------------------------------
  // [Helpers - Pattern - Any - Fill]
  // --------------------------------------------------------------------------

  static FOG_INLINE uint8_t* p_fill_prgb32(uint8_t* dst, uint32_t c0, int w)
  {
    CBase::MemFill32Context ctx;
    ctx.init(c0);
    return ctx.fill(dst, w);
  }

  static FOG_INLINE uint8_t* p_fill_a8(uint8_t* dst, uint32_t c0, int w)
  {
    memset(dst, (int)c0, (sysuint_t)w);
    return dst + w;
  }

  // ==========================================================================
  // [Helpers - Pattern - Solid - Create / Destroy]
  // ==========================================================================

  static err_t FOG_FASTCALL p_solid_create(
    RenderPatternContext* ctx, uint32_t dstFormat, const RenderSolid& solid)
  {
    ctx->_initDst(dstFormat);
    ctx->_destroy = _g2d_render.solid.destroy;
    ctx->_prepare = _g2d_render.solid.prepare;

    switch (dstFormat)
    {
      case IMAGE_FORMAT_PRGB32:
      case IMAGE_FORMAT_XRGB32:
      case IMAGE_FORMAT_RGB24:
      {
        if (Face::p32PRGB32IsAlphaFF(solid.prgb32.p32)) 
        {
          ctx->_srcFormat = IMAGE_FORMAT_XRGB32;
          ctx->_srcBPP = 4;
        }
        else
        {
          ctx->_srcFormat = IMAGE_FORMAT_PRGB32;
          ctx->_srcBPP = 4;
        }
        break;
      }

      case IMAGE_FORMAT_A8:
      {
        ctx->_srcFormat = IMAGE_FORMAT_A8;
        ctx->_srcBPP = 1;
        break;
      }

      case IMAGE_FORMAT_PRGB64:
      case IMAGE_FORMAT_RGB48:
      {
        if (Face::p64PRGB64IsAlphaFFFF(solid.prgb64.p64)) 
        {
          ctx->_srcFormat = IMAGE_FORMAT_RGB48;
          ctx->_srcBPP = 6;
        }
        else
        {
          ctx->_srcFormat = IMAGE_FORMAT_PRGB64;
          ctx->_srcBPP = 8;
        }
        break;
      }

      case IMAGE_FORMAT_A16:
      {
        ctx->_srcFormat = IMAGE_FORMAT_A16;
        ctx->_srcBPP = 2;
        break;
      }

      default:
        FOG_ASSERT_NOT_REACHED();
    }

    return ERR_OK;
  }

  static void FOG_FASTCALL p_solid_destroy(
    RenderPatternContext* ctx)
  {
    FOG_ASSERT(ctx->isInitialized());
    ctx->reset();
  }

  // ==========================================================================
  // [Helpers - Pattern - Solid - Prepare]
  // ==========================================================================

  static void FOG_FASTCALL p_solid_prepare(
    const RenderPatternContext* ctx, RenderPatternFetcher* fetcher, int y, int delta, uint32_t mode)
  {
    fetcher->_ctx = ctx;
    fetcher->_fetch = _g2d_render.solid.fetch[ctx->_srcFormat];
    fetcher->_skip = p_solid_skip;
    fetcher->_mode = mode;
  }

  // ==========================================================================
  // [Helpers - Pattern - Solid - Fetch]
  // ==========================================================================

  // --------------------------------------------------------------------------
  // [Helpers - Pattern - Solid - Fetch]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL p_solid_fetch_helper_prgb32(
    Span* span, uint8_t* buffer, uint32_t mode, const RenderSolid& solid)
  {
    uint32_t c0 = solid.prgb32.u32;

    if (mode == RENDER_FETCH_REFERENCE)
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
          dst = p_fill_prgb32(dst, c0, w - filledWidth);
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
        dst = p_fill_prgb32(dst, c0, w);
        P_FETCH_SPAN8_NEXT()
      P_FETCH_SPAN8_END()
    }
  }

  static void FOG_FASTCALL p_solid_fetch_prgb32_xrgb32(
    RenderPatternFetcher* fetcher, Span* span, uint8_t* buffer)
  {
    p_solid_fetch_helper_prgb32(span, buffer, fetcher->getMode(), fetcher->getContext()->_d.solid);
  }

  static void FOG_FASTCALL p_solid_fetch_a8(
    RenderPatternFetcher* fetcher, Span* span, uint8_t* buffer)
  {
    // TODO: A8 support.
  }

  static void FOG_FASTCALL p_solid_fetch_prgb64(
    RenderPatternFetcher* fetcher, Span* span, uint8_t* buffer)
  {
    // TODO: PRGB64 support.
  }

  static void FOG_FASTCALL p_solid_fetch_rgb48(
    RenderPatternFetcher* fetcher, Span* span, uint8_t* buffer)
  {
    // TODO: PRGB48 support.
  }

  static void FOG_FASTCALL p_solid_fetch_a16(
    RenderPatternFetcher* fetcher, Span* span, uint8_t* buffer)
  {
    // TODO: A16 support.
  }

  // ==========================================================================
  // [Helpers - Pattern - Solid - Skip]
  // ==========================================================================

  static void FOG_FASTCALL p_solid_skip(
    RenderPatternFetcher* fetcher, int count)
  {
  }
};

} // Render_C namespace
} // Fog namespace

// [Guard]
#endif // _FOG_G2D_RENDER_RENDER_C_PHELPERS_P_H
