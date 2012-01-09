// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTEROPS_C_TEXTURESIMPLE_P_H
#define _FOG_G2D_PAINTING_RASTEROPS_C_TEXTURESIMPLE_P_H

// [Dependencies]
#include <Fog/G2d/Painting/RasterOps_C/TextureBase_p.h>

namespace Fog {
namespace RasterOps_C {

// ============================================================================
// [Fog::RasterOps_C - PTextureSimple]
// ============================================================================

struct FOG_NO_EXPORT PTextureSimple
{
  // --------------------------------------------------------------------------
  // [Fetch - Align - Pad]
  // --------------------------------------------------------------------------

  template<typename Accessor>
  static void FOG_FASTCALL fetch_align_pad(
    RasterPatternFetcher* fetcher, RasterSpan* span, uint8_t* buffer)
  {
    const RasterPattern* ctx = fetcher->getContext();
    Accessor accessor(ctx);

    // ------------------------------------------------------------------------
    // [Prepare]
    // ------------------------------------------------------------------------

    int tw = ctx->_d.texture.base.w;
    int th = ctx->_d.texture.base.h;

    const uint8_t* srcLine;
    typename Accessor::Pixel c0;

    P_FETCH_SPAN8_INIT()
    int y = fetcher->_d.texture.simple.py;
    if (y < 0)
      y = 0;
    else if (y >= th)
      y = th - 1;

    srcLine = ctx->_d.texture.base.pixels + y * ctx->_d.texture.base.stride;

    // ------------------------------------------------------------------------
    // [Loop]
    // ------------------------------------------------------------------------

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()
      x += ctx->_d.texture.simple.tx;

      const uint8_t* src = srcLine;

      // ----------------------------------------------------------------------
      // [Pad]
      // ----------------------------------------------------------------------

      if (x < 0)
      {
        int i = Math::min(-x, w);
        x = 0;
        w -= i;

        accessor.fetchNorm(c0, src);
        dst = accessor.fill(dst, c0, i);
        if (w == 0) goto _FetchSkip;
      }

      // ----------------------------------------------------------------------
      // [Reference]
      // ----------------------------------------------------------------------

      else if (Accessor::FETCH_REFERENCE && fetcher->_mode == RASTER_FETCH_REFERENCE && x < tw && w < tw - x)
      {
        P_FETCH_SPAN8_SET_CUSTOM(src + (uint)x * Accessor::SRC_BPP);
        goto _FetchSkip;
      }

      // ----------------------------------------------------------------------
      // [Fetch]
      // ----------------------------------------------------------------------

      if (x < tw)
      {
        int i = Math::min(tw - x, w);
        src += (uint)x * Accessor::SRC_BPP;
        w -= i;

        do {
          accessor.fetchNorm(c0, src);
          accessor.store(dst, c0);

          dst += Accessor::DST_BPP;
          src += Accessor::SRC_BPP;
        } while (--i);
        if (w == 0) goto _FetchSkip;
      }

      // ----------------------------------------------------------------------
      // [Pad]
      // ----------------------------------------------------------------------

      accessor.fetchNorm(c0, srcLine + (uint)tw * Accessor::SRC_BPP - Accessor::SRC_BPP);
      goto _FetchSolidLoop;

_FetchSkip:
      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()
    goto _FetchEnd;

    // ------------------------------------------------------------------------
    // [Solid]
    // ------------------------------------------------------------------------

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()
_FetchSolidLoop:
      dst = accessor.fill(dst, c0, w);
      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()

    // ------------------------------------------------------------------------
    // [Advance]
    // ------------------------------------------------------------------------

_FetchEnd:
    fetcher->_d.texture.simple.py += fetcher->_d.texture.simple.dy;
  }

  // --------------------------------------------------------------------------
  // [Fetch - SubX0 - Pad]
  // --------------------------------------------------------------------------

  template<typename Accessor>
  static void FOG_FASTCALL fetch_subx0_pad(
    RasterPatternFetcher* fetcher, RasterSpan* span, uint8_t* buffer)
  {
    const RasterPattern* ctx = fetcher->getContext();
    Accessor accessor(ctx);

    // ------------------------------------------------------------------------
    // [Prepare]
    // ------------------------------------------------------------------------

    int tw = ctx->_d.texture.base.w;
    int th = ctx->_d.texture.base.h;

    uint fY0X0 = ctx->_d.texture.simple.fY1X0;
    uint fY0X1 = ctx->_d.texture.simple.fY1X1;

    const uint8_t* srcLine;
    typename Accessor::Pixel c0;
    typename Accessor::Pixel c1;

    P_FETCH_SPAN8_INIT()
    int y = fetcher->_d.texture.simple.py;
    if (y < 0)
      y = 0;
    else if (y >= th)
      y = th - 1;

    srcLine = ctx->_d.texture.base.pixels + y * ctx->_d.texture.base.stride;

    // ------------------------------------------------------------------------
    // [Loop]
    // ------------------------------------------------------------------------

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBORS(Accessor::DST_BPP)
      x += ctx->_d.texture.simple.tx;

      const uint8_t* src = srcLine;
      int i;

      // ----------------------------------------------------------------------
      // [Pad]
      // ----------------------------------------------------------------------

      if (x < 0)
      {
        typename Accessor::Pixel norm0;

        i = Math::min(-x, w);
        w -= i;
        x = 1;

        accessor.fetchRaw(c0, src);
        accessor.normalize(norm0, c0);

        dst = accessor.fill(dst, norm0, i);
        src += Accessor::SRC_BPP;

        if (w == 0) goto _FetchSkip;
        goto _FetchFill;
      }

      // ----------------------------------------------------------------------
      // [Fetch]
      // ----------------------------------------------------------------------

      else if (x < tw)
      {
        src += (uint)x * Accessor::SRC_BPP;
        accessor.fetchRaw(c0, src);
        src += Accessor::SRC_BPP;

        if (++x == tw) goto _FetchSolid;

_FetchFill:
        i = Math::min(tw - x, w);
        w -= i;

        do {
          accessor.fetchRaw(c1, src);
          accessor.interpolateRaw_2(c0, c0, fY0X0, c1, fY0X1);
          accessor.normalize(c0, c0);
          accessor.store(dst, c0);

          c0 = c1;
          dst += Accessor::DST_BPP;
          src += Accessor::SRC_BPP;
        } while (--i);

        if (w == 0) goto _FetchSkip;
      }

      // ----------------------------------------------------------------------
      // [Pad]
      // ----------------------------------------------------------------------

      accessor.fetchNorm(c0, srcLine + tw * Accessor::SRC_BPP - Accessor::SRC_BPP);
      goto _FetchSolidLoop;

_FetchSkip:
      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()
    goto _FetchEnd;

    // ------------------------------------------------------------------------
    // [Solid]
    // ------------------------------------------------------------------------

_FetchSolid:
    accessor.normalize(c0, c0);

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()
_FetchSolidLoop:
      dst = accessor.fill(dst, c0, w);
      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()

    // ------------------------------------------------------------------------
    // [Advance]
    // ------------------------------------------------------------------------

_FetchEnd:
    fetcher->_d.texture.simple.py += fetcher->_d.texture.simple.dy;
  }

  // --------------------------------------------------------------------------
  // [Fetch - Sub0Y - Pad]
  // --------------------------------------------------------------------------

  template<typename Accessor>
  static void FOG_FASTCALL fetch_sub0y_pad(
    RasterPatternFetcher* fetcher, RasterSpan* span, uint8_t* buffer)
  {
    const RasterPattern* ctx = fetcher->getContext();
    Accessor accessor(ctx);

    // ------------------------------------------------------------------------
    // [Prepare]
    // ------------------------------------------------------------------------

    int tw = ctx->_d.texture.base.w;
    int th = ctx->_d.texture.base.h;

    uint fY0X0 = ctx->_d.texture.simple.fY0X1;
    uint fY1X0 = ctx->_d.texture.simple.fY1X1;

    typename Accessor::Pixel c0;
    typename Accessor::Pixel c1;

    P_FETCH_SPAN8_INIT()
    int y = fetcher->_d.texture.simple.py;

    // ------------------------------------------------------------------------
    // [Loop - Aligned]
    // ------------------------------------------------------------------------

    if (y < 0 || y >= th - 1)
    {
      y = (y < 0) ? 0 : th - 1;
      const uint8_t* srcLine = ctx->_d.texture.base.pixels + y * ctx->_d.texture.base.stride;

      P_FETCH_SPAN8_BEGIN()
        // Can't use MERGE_NEIGHBOURS, because of P_FETCH_SPAN8_SET_CUSTOM.
        P_FETCH_SPAN8_SET_CURRENT()
        x += ctx->_d.texture.simple.tx;

        // --------------------------------------------------------------------
        // [Pad]
        // --------------------------------------------------------------------

        if (x < 0)
        {
          int i = Math::min(-x, w);
          w -= i;
          x = 0;

          accessor.fetchNorm(c0, srcLine);
          dst = accessor.fill(dst, c0, i);
          if (w == 0) goto _FetchAlignedSkip;
        }

        // --------------------------------------------------------------------
        // [Reference]
        // --------------------------------------------------------------------

        else if (Accessor::FETCH_REFERENCE && fetcher->_mode == RASTER_FETCH_REFERENCE && x < tw && w < tw - x)
        {
          P_FETCH_SPAN8_SET_CUSTOM(srcLine + (uint)x * Accessor::SRC_BPP);
          goto _FetchAlignedSkip;
        }

        // --------------------------------------------------------------------
        // [Fetch]
        // --------------------------------------------------------------------

        if (x < tw)
        {
          int i = Math::min(tw - x, w);
          const uint8_t* src = srcLine + (uint)x * Accessor::SRC_BPP;
          w -= i;

          do {
            accessor.fetchNorm(c0, src);
            accessor.store(dst, c0);

            dst += Accessor::DST_BPP;
            src += Accessor::SRC_BPP;
          } while (--i);
          if (w == 0) goto _FetchAlignedSkip;
        }

        // --------------------------------------------------------------------
        // [Pad]
        // --------------------------------------------------------------------

        accessor.fetchNorm(c0, srcLine + (uint)tw * Accessor::SRC_BPP - Accessor::SRC_BPP);
        goto _FetchSolidLoop;

_FetchAlignedSkip:
        P_FETCH_SPAN8_NEXT()
      P_FETCH_SPAN8_END()
    }

    // ------------------------------------------------------------------------
    // [Loop - Sub0Y]
    // ------------------------------------------------------------------------

    else
    {
      const uint8_t* srcLine0 = ctx->_d.texture.base.pixels + y * ctx->_d.texture.base.stride;
      const uint8_t* srcLine1 = srcLine0 + ctx->_d.texture.base.stride;

      P_FETCH_SPAN8_BEGIN()
        P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBORS(Accessor::DST_BPP)
        x += ctx->_d.texture.simple.tx;

        // --------------------------------------------------------------------
        // [Pad]
        // --------------------------------------------------------------------

        if (x < 0)
        {
          int i = Math::min(-x, w);
          x = 0;
          w -= i;

          accessor.fetchRaw(c0, srcLine0);
          accessor.fetchRaw(c1, srcLine1);
          accessor.interpolateRaw_2(c0, c0, fY0X0, c1, fY1X0);
          accessor.normalize(c0, c0);

          dst = accessor.fill(dst, c0, i);
          if (w == 0) goto _FetchSub0YSkip;
        }

        // --------------------------------------------------------------------
        // [Fetch]
        // --------------------------------------------------------------------

        if (x < tw)
        {
          const uint8_t* srcCur0 = srcLine0 + (uint)x * Accessor::SRC_BPP;
          const uint8_t* srcCur1 = srcLine1 + (uint)x * Accessor::SRC_BPP;

          int i = Math::min(tw - x, w);
          w -= i;

          do {
            accessor.fetchRaw(c0, srcCur0);
            accessor.fetchRaw(c1, srcCur1);
            accessor.interpolateRaw_2(c0, c0, fY0X0, c1, fY1X0);
            accessor.normalize(c0, c0);
            accessor.store(dst, c0);

            dst     += Accessor::DST_BPP;
            srcCur0 += Accessor::SRC_BPP;
            srcCur1 += Accessor::SRC_BPP;
          } while (--i);
          if (w == 0) goto _FetchSub0YSkip;
        }

        // --------------------------------------------------------------------
        // [Pad]
        // --------------------------------------------------------------------

        accessor.fetchRaw(c0, srcLine0 + (uint)tw * Accessor::SRC_BPP - Accessor::SRC_BPP);
        accessor.fetchRaw(c1, srcLine1 + (uint)tw * Accessor::SRC_BPP - Accessor::SRC_BPP);
        accessor.interpolateRaw_2(c0, c0, fY0X0, c1, fY1X0);
        accessor.normalize(c0, c0);
        goto _FetchSolidLoop;

_FetchSub0YSkip:
        P_FETCH_SPAN8_NEXT()
      P_FETCH_SPAN8_END()
    }
    goto _FetchEnd;

    // ------------------------------------------------------------------------
    // [Solid]
    // ------------------------------------------------------------------------

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()
_FetchSolidLoop:
      dst = accessor.fill(dst, c0, w);
      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()

    // ------------------------------------------------------------------------
    // [Advance]
    // ------------------------------------------------------------------------

_FetchEnd:
    fetcher->_d.texture.simple.py += fetcher->_d.texture.simple.dy;
  }

  // --------------------------------------------------------------------------
  // [Fetch - SubXY - Pad]
  // --------------------------------------------------------------------------

  template<typename Accessor>
  static void FOG_FASTCALL fetch_subxy_pad(
    RasterPatternFetcher* fetcher, RasterSpan* span, uint8_t* buffer)
  {
    const RasterPattern* ctx = fetcher->getContext();
    Accessor accessor(ctx);

    // ------------------------------------------------------------------------
    // [Prepare]
    // ------------------------------------------------------------------------

    int tw = ctx->_d.texture.base.w;
    int th = ctx->_d.texture.base.h;

    uint fY0X0 = ctx->_d.texture.simple.fY0X0;
    uint fY0X1 = ctx->_d.texture.simple.fY0X1;
    uint fY1X0 = ctx->_d.texture.simple.fY1X0;
    uint fY1X1 = ctx->_d.texture.simple.fY1X1;

    typename Accessor::Pixel back0;
    typename Accessor::Pixel back1;

    P_FETCH_SPAN8_INIT()
    int y = fetcher->_d.texture.simple.py;

    // ------------------------------------------------------------------------
    // [Loop - Border]
    // ------------------------------------------------------------------------

    if ((y < 0) | (y >= th - 1))
    {
      y = (y < 0) ? 0 : th - 1;

      fY0X0 += fY1X0;
      fY0X1 += fY1X1;

      const uint8_t* srcLine = ctx->_d.texture.base.pixels + y * ctx->_d.texture.base.stride;

      P_FETCH_SPAN8_BEGIN()
        P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBORS(Accessor::DST_BPP)
        x += ctx->_d.texture.simple.tx;

        const uint8_t* src = srcLine;
        int i;

        // --------------------------------------------------------------------
        // [Pad]
        // --------------------------------------------------------------------

        if (x < 0)
        {
          i = Math::min(-x, w);
          w -= i;
          x = 1;

          accessor.fetchNorm(back0, src);
          dst = accessor.fill(dst, back0, i);
          src += Accessor::SRC_BPP;

          if (w == 0) goto _FetchBorderSkip;

          accessor.fetchRaw(back0, src);
          goto _FetchBorderFill;
        }

        // --------------------------------------------------------------------
        // [Fetch]
        // --------------------------------------------------------------------

        else if (x < tw)
        {
          src += (uint)x * Accessor::SRC_BPP;
          accessor.fetchRaw(back0, src);
          src += Accessor::SRC_BPP;
          if (++x == tw) goto _FetchBorderAfter;

_FetchBorderFill:
          i = Math::min(tw - x, w);
          w -= i;

          do {
            accessor.fetchRaw(back1, src);
            accessor.interpolateRaw_2(back0, back0, fY0X0, back1, fY0X1);
            accessor.normalize(back0, back0);
            accessor.store(dst, back0);

            back0 = back1;
            dst += Accessor::DST_BPP;
            src += Accessor::SRC_BPP;
          } while (--i);
          if (w == 0) goto _FetchBorderSkip;
        }

        // --------------------------------------------------------------------
        // [Pad]
        // --------------------------------------------------------------------

_FetchBorderAfter:
        accessor.fetchNorm(back0, srcLine + (uint)tw * Accessor::SRC_BPP - Accessor::SRC_BPP);
        goto _FetchSolidLoop;

_FetchBorderSkip:
        P_FETCH_SPAN8_NEXT()
      P_FETCH_SPAN8_END()
    }

    // ------------------------------------------------------------------------
    // [Loop - Inside]
    // ------------------------------------------------------------------------

    else
    {
      const uint8_t* srcLine0 = ctx->_d.texture.base.pixels + y * ctx->_d.texture.base.stride;
      const uint8_t* srcLine1 = srcLine0 + ctx->_d.texture.base.stride;

      P_FETCH_SPAN8_BEGIN()
        P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBORS(Accessor::DST_BPP)
        x += ctx->_d.texture.simple.tx;

        const uint8_t* srcCur0 = srcLine0;
        const uint8_t* srcCur1 = srcLine1;

        int i;

        // --------------------------------------------------------------------
        // [Pad]
        // --------------------------------------------------------------------

        if (x < 0)
        {
          i = Math::min(-x, w);
          x = 0;
          w -= i;

          accessor.fetchRaw(back0, srcLine0);
          accessor.fetchRaw(back1, srcLine1);

          typename Accessor::Pixel c0;
          accessor.interpolateRaw_2(c0, back0, fY0X0 + fY0X1, back1, fY1X0 + fY1X1);
          accessor.normalize(c0, c0);
          dst = accessor.fill(dst, c0, i);
          if (w == 0) goto _FetchInsideSkip;

          goto _FetchInsideFill;
        }

        // --------------------------------------------------------------------
        // [Fetch]
        // --------------------------------------------------------------------

        else if (x < tw - 1)
        {
          srcCur0 += (uint)x * Accessor::SRC_BPP;
          srcCur1 += (uint)x * Accessor::SRC_BPP;

          accessor.fetchRaw(back0, srcCur0);
          accessor.fetchRaw(back1, srcCur1);

_FetchInsideFill:
          srcCur0 += Accessor::SRC_BPP;
          srcCur1 += Accessor::SRC_BPP;

          i = Math::min(tw - 1 - x, w);
          w -= i;

          do {
            typename Accessor::Pixel back0_1;
            typename Accessor::Pixel back1_1;

            accessor.fetchRaw(back0_1, srcCur0);
            accessor.fetchRaw(back1_1, srcCur1);
            accessor.interpolateRaw_4(back0, back0, fY0X0, back1, fY1X0, back0_1, fY0X1, back1_1, fY1X1);
            accessor.normalize(back0, back0);
            accessor.store(dst, back0);

            back0 = back0_1;
            back1 = back1_1;
            dst     += Accessor::DST_BPP;
            srcCur0 += Accessor::SRC_BPP;
            srcCur1 += Accessor::SRC_BPP;
          } while (--i);

          if (w == 0) goto _FetchInsideSkip;
        }
        else
        {
          accessor.fetchRaw(back0, srcLine0 + (uint)tw * Accessor::SRC_BPP - Accessor::SRC_BPP);
          accessor.fetchRaw(back1, srcLine1 + (uint)tw * Accessor::SRC_BPP - Accessor::SRC_BPP);
        }

        // --------------------------------------------------------------------
        // [Pad]
        // --------------------------------------------------------------------

        accessor.interpolateRaw_2(back0, back0, fY0X0 + fY0X1, back1, fY1X0 + fY1X1);
        accessor.normalize(back0, back0);
        goto _FetchSolidLoop;

_FetchInsideSkip:
        P_FETCH_SPAN8_NEXT()
      P_FETCH_SPAN8_END()
    }
    goto _FetchEnd;

    // ------------------------------------------------------------------------
    // [Solid]
    // ------------------------------------------------------------------------

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()
_FetchSolidLoop:
      dst = accessor.fill(dst, back0, w);
      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()

    // ------------------------------------------------------------------------
    // [Advance]
    // ------------------------------------------------------------------------

_FetchEnd:
    fetcher->_d.texture.simple.py += fetcher->_d.texture.simple.dy;
  }

  // --------------------------------------------------------------------------
  // [Fetch - Align - Repeat]
  // --------------------------------------------------------------------------

  template<typename Accessor>
  static void FOG_FASTCALL fetch_align_repeat(
    RasterPatternFetcher* fetcher, RasterSpan* span, uint8_t* buffer)
  {
    const RasterPattern* ctx = fetcher->getContext();
    Accessor accessor(ctx);

    // ------------------------------------------------------------------------
    // [Prepare]
    // ------------------------------------------------------------------------

    int tw = ctx->_d.texture.base.w;
    int th = ctx->_d.texture.base.h;

    const uint8_t* srcLine;
    const uint8_t* src;

    typename Accessor::Pixel c0;

    P_FETCH_SPAN8_INIT()
    int y = fetcher->_d.texture.simple.py;
    FOG_ASSERT(y >= 0 && y < th);

    x = Helpers::p_repeat_integer(x + ctx->_d.texture.simple.tx, tw);

    srcLine = ctx->_d.texture.base.pixels + y * ctx->_d.texture.base.stride;
    src = srcLine + (uint)x * Accessor::SRC_BPP;

    // ------------------------------------------------------------------------
    // [Loop]
    // ------------------------------------------------------------------------

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()

      int i = Math::min(tw - x, w);

      // ----------------------------------------------------------------------
      // [Reference]
      // ----------------------------------------------------------------------

      if (Accessor::FETCH_REFERENCE && fetcher->_mode == RASTER_FETCH_REFERENCE && w <= tw - x)
      {
        P_FETCH_SPAN8_SET_CUSTOM(src);
        src += (uint)w * Accessor::SRC_BPP;

        if ((x += w) == tw) { x = 0; src = srcLine; }
        goto _FetchSkip;
      }

      // ----------------------------------------------------------------------
      // [Fetch]
      // ----------------------------------------------------------------------

      for (;;)
      {
        w -= i;
        x += i;

        do {
          accessor.fetchNorm(c0, src);
          accessor.store(dst, c0);

          dst += Accessor::DST_BPP;
          src += Accessor::SRC_BPP;
        } while (--i);

        if (x == tw) { x = 0; src = srcLine; }
        if (!w) break;

        i = Math::min(tw, w);
      }

_FetchSkip:
      P_FETCH_SPAN8_HOLE(
      {
        x += hole;
        if (x >= tw) x %= tw;

        src = srcLine + (uint)x * Accessor::SRC_BPP;
      })
    P_FETCH_SPAN8_END()

    // ------------------------------------------------------------------------
    // [Advance]
    // ------------------------------------------------------------------------

    y += fetcher->_d.texture.simple.dy;
    if (y >= th) y -= th;
    fetcher->_d.texture.simple.py = y;
  }

  // --------------------------------------------------------------------------
  // [Fetch - SubX0 - Repeat]
  // --------------------------------------------------------------------------

  template<typename Accessor>
  static void FOG_FASTCALL fetch_subx0_repeat(
    RasterPatternFetcher* fetcher, RasterSpan* span, uint8_t* buffer)
  {
    const RasterPattern* ctx = fetcher->getContext();
    Accessor accessor(ctx);

    // ------------------------------------------------------------------------
    // [Prepare]
    // ------------------------------------------------------------------------

    int tw = ctx->_d.texture.base.w;
    int th = ctx->_d.texture.base.h;

    uint fY0X0 = ctx->_d.texture.simple.fY1X0;
    uint fY0X1 = ctx->_d.texture.simple.fY1X1;

    P_FETCH_SPAN8_INIT()
    int y = fetcher->_d.texture.simple.py;
    FOG_ASSERT(y >= 0 && y < th);

    const uint8_t* srcLine = ctx->_d.texture.base.pixels + y * ctx->_d.texture.base.stride;

    // ------------------------------------------------------------------------
    // [Loop]
    // ------------------------------------------------------------------------

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBORS(Accessor::DST_BPP)
      x = Helpers::p_repeat_integer(x + ctx->_d.texture.simple.tx, tw);

      const uint8_t* src = srcLine + (uint)x * Accessor::SRC_BPP;

      typename Accessor::Pixel back0;
      typename Accessor::Pixel back1;

      accessor.fetchRaw(back0, src);
      src += Accessor::SRC_BPP;

      if (++x == tw)
      {
        x = 0;
        src = srcLine;
      }

      int r = Math::min(w, tw);
      int i = Math::min(tw - x, r);

      w -= r;

      // ----------------------------------------------------------------------
      // [Fetch - Texture Width]
      // ----------------------------------------------------------------------

      for (;;)
      {
        r -= i;

        do {
          accessor.fetchRaw(back1, src);
          accessor.interpolateRaw_2(back0, back0, fY0X0, back1, fY0X1);
          accessor.normalize(back0, back0);
          accessor.store(dst, back0);

          back0 = back1;
          dst += Accessor::DST_BPP;
          src += Accessor::SRC_BPP;
        } while (--i);
        if (!r) break;

        i = Math::min(r, tw);
        src = srcLine;
      }

      // ----------------------------------------------------------------------
      // [Fetch - Repeat]
      // ----------------------------------------------------------------------

      if (w) dst = accessor.repeat(dst, tw, w);

      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()

    // ------------------------------------------------------------------------
    // [Advance]
    // ------------------------------------------------------------------------

    y += fetcher->_d.texture.simple.dy;
    if (y >= th) y -= th;
    fetcher->_d.texture.simple.py = y;
  }

  // --------------------------------------------------------------------------
  // [Fetch - Sub0Y - Repeat]
  // --------------------------------------------------------------------------

  template<typename Accessor>
  static void FOG_FASTCALL fetch_sub0y_repeat(
    RasterPatternFetcher* fetcher, RasterSpan* span, uint8_t* buffer)
  {
    const RasterPattern* ctx = fetcher->getContext();
    Accessor accessor(ctx);

    // ------------------------------------------------------------------------
    // [Prepare]
    // ------------------------------------------------------------------------

    int tw = ctx->_d.texture.base.w;
    int th = ctx->_d.texture.base.h;

    uint fY0X0 = ctx->_d.texture.simple.fY0X1;
    uint fY1X0 = ctx->_d.texture.simple.fY1X1;

    P_FETCH_SPAN8_INIT()
    int y = fetcher->_d.texture.simple.py;
    FOG_ASSERT(y >= 0 && y < th);

    const uint8_t* srcLine0 = ctx->_d.texture.base.pixels + y * ctx->_d.texture.base.stride;
    const uint8_t* srcLine1 = srcLine0 + ctx->_d.texture.base.stride;
    if (y + 1 >= th) srcLine1 = ctx->_d.texture.base.pixels;

    // ------------------------------------------------------------------------
    // [Loop]
    // ------------------------------------------------------------------------

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBORS(Accessor::DST_BPP)
      x = Helpers::p_repeat_integer(x + ctx->_d.texture.simple.tx, tw);

      const uint8_t* srcCur0 = srcLine0 + (uint)x * Accessor::SRC_BPP;
      const uint8_t* srcCur1 = srcLine1 + (uint)x * Accessor::SRC_BPP;

      int r = Math::min(w, tw);
      int i = Math::min(tw - x, r);

      w -= r;

      // ----------------------------------------------------------------------
      // [Fetch - Texture Width]
      // ----------------------------------------------------------------------

      for (;;)
      {
        r -= i;

        do {
          typename Accessor::Pixel c0;
          typename Accessor::Pixel c1;

          accessor.fetchRaw(c0, srcCur0);
          accessor.fetchRaw(c1, srcCur1);
          accessor.interpolateRaw_2(c0, c0, fY0X0, c1, fY1X0);
          accessor.normalize(c0, c0);
          accessor.store(dst, c0);

          dst     += Accessor::DST_BPP;
          srcCur0 += Accessor::SRC_BPP;
          srcCur1 += Accessor::SRC_BPP;
        } while (--i);
        if (!r) break;

        i = Math::min(r, tw);
        srcCur0 = srcLine0;
        srcCur1 = srcLine1;
      }

      // ----------------------------------------------------------------------
      // [Fetch - Repeat]
      // ----------------------------------------------------------------------

      if (w) dst = accessor.repeat(dst, tw, w);

      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()

    // ------------------------------------------------------------------------
    // [Advance]
    // ------------------------------------------------------------------------

    y += fetcher->_d.texture.simple.dy;
    if (y >= th) y -= th;
    fetcher->_d.texture.simple.py = y;
  }

  // --------------------------------------------------------------------------
  // [Fetch - SubXY - Repeat]
  // --------------------------------------------------------------------------

  template<typename Accessor>
  static void FOG_FASTCALL fetch_subxy_repeat(
    RasterPatternFetcher* fetcher, RasterSpan* span, uint8_t* buffer)
  {
    const RasterPattern* ctx = fetcher->getContext();
    Accessor accessor(ctx);

    // ------------------------------------------------------------------------
    // [Prepare]
    // ------------------------------------------------------------------------

    int tw = ctx->_d.texture.base.w;
    int th = ctx->_d.texture.base.h;

    uint fY0X0 = ctx->_d.texture.simple.fY0X0;
    uint fY0X1 = ctx->_d.texture.simple.fY0X1;
    uint fY1X0 = ctx->_d.texture.simple.fY1X0;
    uint fY1X1 = ctx->_d.texture.simple.fY1X1;

    P_FETCH_SPAN8_INIT()
    int y = fetcher->_d.texture.simple.py;
    FOG_ASSERT(y >= 0 && y < th);

    const uint8_t* srcLine0 = ctx->_d.texture.base.pixels + y * ctx->_d.texture.base.stride;
    const uint8_t* srcLine1 = srcLine0 + ctx->_d.texture.base.stride;
    if (y + 1 >= th) srcLine1 = ctx->_d.texture.base.pixels;

    // ------------------------------------------------------------------------
    // [Loop]
    // ------------------------------------------------------------------------

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBORS(Accessor::DST_BPP)
      x = Helpers::p_repeat_integer(x + ctx->_d.texture.simple.tx, tw);

      const uint8_t* srcCur0 = srcLine0 + (uint)x * Accessor::SRC_BPP;
      const uint8_t* srcCur1 = srcLine1 + (uint)x * Accessor::SRC_BPP;

      typename Accessor::Pixel back0;
      typename Accessor::Pixel back1;

      accessor.fetchRaw(back0, srcCur0);
      accessor.fetchRaw(back1, srcCur1);

      srcCur0 += Accessor::SRC_BPP;
      srcCur1 += Accessor::SRC_BPP;

      if (++x == tw)
      {
        x = 0;
        srcCur0 = srcLine0;
        srcCur1 = srcLine1;
      }

      int r = Math::min(w, tw);
      int i = Math::min(tw - x, r);

      w -= r;

      // ----------------------------------------------------------------------
      // [Fetch - Texture Width]
      // ----------------------------------------------------------------------

      for (;;)
      {
        r -= i;

        do {
          typename Accessor::Pixel back0_1;
          typename Accessor::Pixel back1_1;

          accessor.fetchRaw(back0_1, srcCur0);
          accessor.fetchRaw(back1_1, srcCur1);
          accessor.interpolateRaw_4(back0, back0, fY0X0, back1, fY1X0, back0_1, fY0X1, back1_1, fY1X1);
          accessor.normalize(back0, back0);
          accessor.store(dst, back0);

          back0 = back0_1;
          back1 = back1_1;

          dst     += Accessor::DST_BPP;
          srcCur0 += Accessor::SRC_BPP;
          srcCur1 += Accessor::SRC_BPP;
        } while (--i);
        if (!r) break;

        i = Math::min(r, tw);
        srcCur0 = srcLine0;
        srcCur1 = srcLine1;
      }

      // ----------------------------------------------------------------------
      // [Fetch - Texture Repeat]
      // ----------------------------------------------------------------------

      if (w) dst = accessor.repeat(dst, tw, w);

      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()

    // ------------------------------------------------------------------------
    // [Advance]
    // ------------------------------------------------------------------------

    y += fetcher->_d.texture.simple.dy;
    if (y >= th) y -= th;
    fetcher->_d.texture.simple.py = y;
  }

  // --------------------------------------------------------------------------
  // [Fetch - Align - Reflect]
  // --------------------------------------------------------------------------

  template<typename Accessor>
  static void FOG_FASTCALL fetch_align_reflect(
    RasterPatternFetcher* fetcher, RasterSpan* span, uint8_t* buffer)
  {
    const RasterPattern* ctx = fetcher->getContext();
    Accessor accessor(ctx);

    // ------------------------------------------------------------------------
    // [Prepare]
    // ------------------------------------------------------------------------

    int tw = ctx->_d.texture.base.w;
    int th = ctx->_d.texture.base.h;

    int tw2 = tw << 1;
    int th2 = th << 1;

    const uint8_t* srcLine;
    typename Accessor::Pixel c0;

    P_FETCH_SPAN8_INIT()
    int y = fetcher->_d.texture.simple.py;
    FOG_ASSERT(y >= 0 && y < th2);

    // Modify Y if reflected (if it lies in reflected section).
    if (y >= th) y = th2 - y - 1;
    srcLine = ctx->_d.texture.base.pixels + y * ctx->_d.texture.base.stride;

    x = Helpers::p_repeat_integer(x + ctx->_d.texture.simple.tx, tw2);

    // ------------------------------------------------------------------------
    // [Loop]
    // ------------------------------------------------------------------------

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()

      // ----------------------------------------------------------------------
      // [Reference]
      // ----------------------------------------------------------------------

      if (Accessor::FETCH_REFERENCE && fetcher->_mode == RASTER_FETCH_REFERENCE && x <= tw && w < tw - x)
      {
        P_FETCH_SPAN8_SET_CUSTOM(srcLine + (uint)x * Accessor::SRC_BPP);

        x += w;
        goto _FetchSkip;
      }

      do {
        // --------------------------------------------------------------------
        // [Reflect]
        // --------------------------------------------------------------------

        if (x >= tw)
        {
          int i = Math::min(tw2 - x, w);
          const uint8_t* src = srcLine + (tw2 - x - 1) * Accessor::SRC_BPP;

          w -= i;
          x += i;
          if (x == tw2) x = 0;

          do {
            accessor.fetchNorm(c0, src);
            accessor.store(dst, c0);

            dst += Accessor::DST_BPP;
            src -= Accessor::SRC_BPP;
          } while (--i);
        }

        // --------------------------------------------------------------------
        // [Repeat]
        // --------------------------------------------------------------------

        else
        {
          int i = Math::min(tw - x, w);
          const uint8_t* src = srcLine + x * Accessor::SRC_BPP;

          w -= i;
          x += i;

          do {
            accessor.fetchNorm(c0, src);
            accessor.store(dst, c0);

            dst += Accessor::DST_BPP;
            src += Accessor::SRC_BPP;
          } while (--i);
        }
      } while (w);

_FetchSkip:
      P_FETCH_SPAN8_HOLE(
      {
        x = Helpers::p_repeat_integer(x + hole, tw2);
      })
    P_FETCH_SPAN8_END()

    // ------------------------------------------------------------------------
    // [Advance]
    // ------------------------------------------------------------------------

    y = fetcher->_d.texture.simple.py + fetcher->_d.texture.simple.dy;
    if (y >= th2) y -= th2;
    fetcher->_d.texture.simple.py = y;
  }

  // --------------------------------------------------------------------------
  // [Fetch - SubX0 - Reflect]
  // --------------------------------------------------------------------------

  template<typename Accessor>
  static void FOG_FASTCALL fetch_subx0_reflect(
    RasterPatternFetcher* fetcher, RasterSpan* span, uint8_t* buffer)
  {
    const RasterPattern* ctx = fetcher->getContext();
    Accessor accessor(ctx);

    // ------------------------------------------------------------------------
    // [Prepare]
    // ------------------------------------------------------------------------

    int tw = ctx->_d.texture.base.w;
    int th = ctx->_d.texture.base.h;

    int tw2 = tw << 1;
    int th2 = th << 1;

    uint fY0X0 = ctx->_d.texture.simple.fY1X0;
    uint fY0X1 = ctx->_d.texture.simple.fY1X1;

    P_FETCH_SPAN8_INIT()
    int y = fetcher->_d.texture.simple.py;
    FOG_ASSERT(y >= 0 && y < th2);

    // Modify Y if reflected (if it lies in second section).
    if (y >= th) y = th2 - y - 1;

    const uint8_t* srcLine = ctx->_d.texture.base.pixels + y * ctx->_d.texture.base.stride;
    const uint8_t* src;

    // ------------------------------------------------------------------------
    // [Loop]
    // ------------------------------------------------------------------------

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBORS(Accessor::DST_BPP)
      x = Helpers::p_repeat_integer(x + ctx->_d.texture.simple.tx, tw2);

      if (x >= tw)
      {
        // Reflect mode.
        src = srcLine + (uint)(tw2 - x - 1) * Accessor::SRC_BPP;
      }
      else
      {
        // Repeat mode.
        src = srcLine + (uint)x * Accessor::SRC_BPP;
      }

      typename Accessor::Pixel back0;
      typename Accessor::Pixel back1;

      accessor.fetchRaw(back0, src);

      if (++x >= tw2) x -= tw2;
      int r = Math::min(w, tw2);
      w -= r;

      // ----------------------------------------------------------------------
      // [Fetch - Texture Width * 2]
      // ----------------------------------------------------------------------

      do {
        // Reflect mode.
        if (x >= tw)
        {
          int i = Math::min(tw2 - x, r);
          src = srcLine + (tw2 - x - 1) * Accessor::SRC_BPP;

          r -= i;
          x = 0;

          do {
            accessor.fetchRaw(back1, src);
            accessor.interpolateRaw_2(back0, back0, fY0X0, back1, fY0X1);
            accessor.normalize(back0, back0);
            accessor.store(dst, back0);

            back0 = back1;
            dst += Accessor::DST_BPP;
            src -= Accessor::SRC_BPP;
          } while (--i);
        }
        // Repeat mode.
        else
        {
          int i = Math::min(tw - x, r);
          src = srcLine + (uint)x * Accessor::SRC_BPP;

          r -= i;
          x += i;

          do {
            accessor.fetchRaw(back1, src);
            accessor.interpolateRaw_2(back0, back0, fY0X0, back1, fY0X1);
            accessor.normalize(back0, back0);
            accessor.store(dst, back0);

            back0 = back1;
            dst += Accessor::DST_BPP;
            src += Accessor::SRC_BPP;
          } while (--i);
        }
      } while (r);

      // ----------------------------------------------------------------------
      // [Fetch - Repeat / Reflect]
      // ----------------------------------------------------------------------

      if (w) dst = accessor.repeat(dst, tw2, w);

      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()

    // ------------------------------------------------------------------------
    // [Advance]
    // ------------------------------------------------------------------------

    y = fetcher->_d.texture.simple.py + fetcher->_d.texture.simple.dy;
    if (y >= th2) y -= th2;
    fetcher->_d.texture.simple.py = y;
  }

  // --------------------------------------------------------------------------
  // [Fetch - Sub0Y - Reflect]
  // --------------------------------------------------------------------------

  template<typename Accessor>
  static void FOG_FASTCALL fetch_sub0y_reflect(
    RasterPatternFetcher* fetcher, RasterSpan* span, uint8_t* buffer)
  {
    const RasterPattern* ctx = fetcher->getContext();
    Accessor accessor(ctx);

    // ------------------------------------------------------------------------
    // [Prepare]
    // ------------------------------------------------------------------------

    int tw = ctx->_d.texture.base.w;
    int th = ctx->_d.texture.base.h;

    int tw2 = tw << 1;
    int th2 = th << 1;

    uint fY0X0 = ctx->_d.texture.simple.fY0X1;
    uint fY1X0 = ctx->_d.texture.simple.fY1X1;

    P_FETCH_SPAN8_INIT()
    int y = fetcher->_d.texture.simple.py;
    FOG_ASSERT(y >= 0 && y < th2);
    int y1 = y + 1;

    // Modify Y if reflected (if it lies in second section).
    if (y >= th)
    {
      y = th2 - y - 1;

      y1 = y - 1;
      if (y1 < 0) y1 = 0;
    }
    else
    {
      if (y1 >= th) y1 = th2 - y1 - 1;
    }

    const uint8_t* srcLine0 = ctx->_d.texture.base.pixels + y * ctx->_d.texture.base.stride;
    const uint8_t* srcLine1 = ctx->_d.texture.base.pixels + y1 * ctx->_d.texture.base.stride;

    // ------------------------------------------------------------------------
    // [Loop]
    // ------------------------------------------------------------------------

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBORS(Accessor::DST_BPP)
      x = Helpers::p_repeat_integer(x + ctx->_d.texture.simple.tx, tw2);

      const uint8_t* srcCur0 = srcLine0 + (uint)x * Accessor::SRC_BPP;
      const uint8_t* srcCur1 = srcLine1 + (uint)x * Accessor::SRC_BPP;

      int r = Math::min(w, tw2);
      w -= r;

      // ----------------------------------------------------------------------
      // [Fetch - Texture Width * 2]
      // ----------------------------------------------------------------------

      do {
        // Reflect mode.
        if (x >= tw)
        {
          int i = Math::min(tw2 - x, r);

          srcCur0 = srcLine0 + (uint)(tw2 - x - 1) * Accessor::SRC_BPP;
          srcCur1 = srcLine1 + (uint)(tw2 - x - 1) * Accessor::SRC_BPP;

          r -= i;
          x = 0;

          do {
            typename Accessor::Pixel c0;
            typename Accessor::Pixel c1;

            accessor.fetchRaw(c0, srcCur0);
            accessor.fetchRaw(c1, srcCur1);
            accessor.interpolateRaw_2(c0, c0, fY0X0, c1, fY1X0);
            accessor.normalize(c0, c0);
            accessor.store(dst, c0);

            dst     += Accessor::DST_BPP;
            srcCur0 -= Accessor::SRC_BPP;
            srcCur1 -= Accessor::SRC_BPP;
          } while (--i);
        }
        // Repeat mode.
        else
        {
          int i = Math::min(tw - x, r);

          srcCur0 = srcLine0 + (uint)x * Accessor::SRC_BPP;
          srcCur1 = srcLine1 + (uint)x * Accessor::SRC_BPP;

          r -= i;
          x += i;

          do {
            typename Accessor::Pixel c0;
            typename Accessor::Pixel c1;

            accessor.fetchRaw(c0, srcCur0);
            accessor.fetchRaw(c1, srcCur1);
            accessor.interpolateRaw_2(c0, c0, fY0X0, c1, fY1X0);
            accessor.normalize(c0, c0);
            accessor.store(dst, c0);

            dst     += Accessor::DST_BPP;
            srcCur0 += Accessor::SRC_BPP;
            srcCur1 += Accessor::SRC_BPP;
          } while (--i);
        }
      } while (r);

      // ----------------------------------------------------------------------
      // [Fetch - Repeat / Reflect]
      // ----------------------------------------------------------------------

      if (w) dst = accessor.repeat(dst, tw2, w);

      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()

    // ------------------------------------------------------------------------
    // [Advance]
    // ------------------------------------------------------------------------

    y = fetcher->_d.texture.simple.py + fetcher->_d.texture.simple.dy;
    if (y >= th2) y -= th2;
    fetcher->_d.texture.simple.py = y;
  }

  // --------------------------------------------------------------------------
  // [Fetch - SubXY - Reflect]
  // --------------------------------------------------------------------------

  template<typename Accessor>
  static void FOG_FASTCALL fetch_subxy_reflect(
    RasterPatternFetcher* fetcher, RasterSpan* span, uint8_t* buffer)
  {
    const RasterPattern* ctx = fetcher->getContext();
    Accessor accessor(ctx);

    // ------------------------------------------------------------------------
    // [Prepare]
    // ------------------------------------------------------------------------

    int tw = ctx->_d.texture.base.w;
    int th = ctx->_d.texture.base.h;

    int tw2 = tw << 1;
    int th2 = th << 1;

    uint fY0X0 = ctx->_d.texture.simple.fY0X0;
    uint fY0X1 = ctx->_d.texture.simple.fY0X1;
    uint fY1X0 = ctx->_d.texture.simple.fY1X0;
    uint fY1X1 = ctx->_d.texture.simple.fY1X1;

    P_FETCH_SPAN8_INIT()
    int y = fetcher->_d.texture.simple.py;
    FOG_ASSERT(y >= 0 && y < th2);
    int y1 = y + 1;

    // Modify Y if reflected (if it lies in second section).
    if (y >= th)
    {
      y = th2 - y - 1;

      y1 = y - 1;
      if (y1 < 0) y1 = 0;
    }
    else
    {
      if (y1 >= th) y1 = th2 - y1 - 1;
    }

    const uint8_t* srcLine0 = ctx->_d.texture.base.pixels + y * ctx->_d.texture.base.stride;
    const uint8_t* srcLine1 = ctx->_d.texture.base.pixels + y1 * ctx->_d.texture.base.stride;

    // ------------------------------------------------------------------------
    // [Loop]
    // ------------------------------------------------------------------------

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBORS(Accessor::DST_BPP)
      x = Helpers::p_repeat_integer(x + ctx->_d.texture.simple.tx, tw2);

      const uint8_t* srcCur0;
      const uint8_t* srcCur1;

      typename Accessor::Pixel back0;
      typename Accessor::Pixel back1;

      if (x >= tw)
      {
        // Reflect mode.
        srcCur0 = srcLine0 + (uint)(tw2 - x - 1) * Accessor::SRC_BPP;
        srcCur1 = srcLine1 + (uint)(tw2 - x - 1) * Accessor::SRC_BPP;
      }
      else
      {
        // Repeat mode.
        srcCur0 = srcLine0 + (uint)x * Accessor::SRC_BPP;
        srcCur1 = srcLine1 + (uint)x * Accessor::SRC_BPP;
      }

      accessor.fetchRaw(back0, srcCur0);
      accessor.fetchRaw(back1, srcCur1);

      if (++x >= tw2) x -= tw2;

      int r = Math::min(w, tw2);
      w -= r;

      // ----------------------------------------------------------------------
      // [Fetch - Texture Width * 2]
      // ----------------------------------------------------------------------

      do {
        // Reflect mode.
        if (x >= tw)
        {
          int i = Math::min(tw2 - x, r);

          srcCur0 = srcLine0 + (uint)(tw2 - x - 1) * Accessor::SRC_BPP;
          srcCur1 = srcLine1 + (uint)(tw2 - x - 1) * Accessor::SRC_BPP;

          r -= i;
          x = 0;

          do {
            typename Accessor::Pixel back0_1;
            typename Accessor::Pixel back1_1;

            accessor.fetchRaw(back0_1, srcCur0);
            accessor.fetchRaw(back1_1, srcCur1);
            accessor.interpolateRaw_4(back0, back0, fY0X0, back1, fY1X0, back0_1, fY0X1, back1_1, fY1X1);
            accessor.normalize(back0, back0);
            accessor.store(dst, back0);

            back0 = back0_1;
            back1 = back1_1;

            dst     += Accessor::DST_BPP;
            srcCur0 -= Accessor::SRC_BPP;
            srcCur1 -= Accessor::SRC_BPP;
          } while (--i);
        }
        // Repeat mode.
        else
        {
          int i = Math::min(tw - x, r);

          srcCur0 = srcLine0 + (uint)x * Accessor::SRC_BPP;
          srcCur1 = srcLine1 + (uint)x * Accessor::SRC_BPP;

          r -= i;
          x += i;

          do {
            typename Accessor::Pixel back0_1;
            typename Accessor::Pixel back1_1;

            accessor.fetchRaw(back0_1, srcCur0);
            accessor.fetchRaw(back1_1, srcCur1);
            accessor.interpolateRaw_4(back0, back0, fY0X0, back1, fY1X0, back0_1, fY0X1, back1_1, fY1X1);
            accessor.normalize(back0, back0);
            accessor.store(dst, back0);

            back0 = back0_1;
            back1 = back1_1;

            dst     += Accessor::DST_BPP;
            srcCur0 += Accessor::SRC_BPP;
            srcCur1 += Accessor::SRC_BPP;
          } while (--i);
        }
      } while (r);

      // ----------------------------------------------------------------------
      // [Fetch - Repeat / Reflect]
      // ----------------------------------------------------------------------

      if (w) dst = accessor.repeat(dst, tw2, w);

      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()

    // ------------------------------------------------------------------------
    // [Advance]
    // ------------------------------------------------------------------------

    y = fetcher->_d.texture.simple.py + fetcher->_d.texture.simple.dy;
    if (y >= th2) y -= th2;
    fetcher->_d.texture.simple.py = y;
  }

  // --------------------------------------------------------------------------
  // [Fetch - Align - Clamp]
  // --------------------------------------------------------------------------

  template<typename Accessor>
  static void FOG_FASTCALL fetch_align_clamp(
    RasterPatternFetcher* fetcher, RasterSpan* span, uint8_t* buffer)
  {
    const RasterPattern* ctx = fetcher->getContext();
    Accessor accessor(ctx);

    // ------------------------------------------------------------------------
    // [Prepare]
    // ------------------------------------------------------------------------

    int tw = ctx->_d.texture.base.w;
    int th = ctx->_d.texture.base.h;

    const uint8_t* srcLine;
    typename Accessor::Pixel c0;

    P_FETCH_SPAN8_INIT()
    int y = fetcher->_d.texture.simple.py;
    if ((uint)y >= (uint)th)
    {
      accessor.fetchSolid(c0, ctx->_d.texture.base.clamp);
      goto _FetchSolid;
    }

    srcLine = ctx->_d.texture.base.pixels + y * ctx->_d.texture.base.stride;

    // ------------------------------------------------------------------------
    // [Loop]
    // ------------------------------------------------------------------------

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()
      x += ctx->_d.texture.simple.tx;

      const uint8_t* src = srcLine;

      // ----------------------------------------------------------------------
      // [Clamp]
      // ----------------------------------------------------------------------

      if (x < 0)
      {
        int i = Math::min(-x, w);
        w -= i;

        accessor.fetchSolid(c0, ctx->_d.texture.base.clamp);
        dst = accessor.fill(dst, c0, i);
        if (w == 0) goto _FetchSkip;

        x = 0;
      }

      // ----------------------------------------------------------------------
      // [Reference]
      // ----------------------------------------------------------------------

      else if (Accessor::FETCH_REFERENCE && fetcher->_mode == RASTER_FETCH_REFERENCE && x < tw && w < tw - x)
      {
        P_FETCH_SPAN8_SET_CUSTOM(src + (uint)x * Accessor::SRC_BPP);
        goto _FetchSkip;
      }

      // ----------------------------------------------------------------------
      // [Fetch]
      // ----------------------------------------------------------------------

      if (x < tw)
      {
        int i = Math::min(tw - x, w);
        src += (uint)x * Accessor::SRC_BPP;
        w -= i;

        do {
          accessor.fetchNorm(c0, src);
          accessor.store(dst, c0);

          dst += Accessor::DST_BPP;
          src += Accessor::SRC_BPP;
        } while (--i);
        if (w == 0) goto _FetchSkip;
      }

      // ----------------------------------------------------------------------
      // [Clamp]
      // ----------------------------------------------------------------------

      accessor.fetchSolid(c0, ctx->_d.texture.base.clamp);
      goto _FetchSolidLoop;

_FetchSkip:
      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()
    goto _FetchEnd;

    // ------------------------------------------------------------------------
    // [Solid]
    // ------------------------------------------------------------------------

_FetchSolid:
    accessor.fetchSolid(c0, ctx->_d.texture.base.clamp);

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()
_FetchSolidLoop:
      dst = accessor.fill(dst, c0, w);
      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()

    // ------------------------------------------------------------------------
    // [Advance]
    // ------------------------------------------------------------------------

_FetchEnd:
    fetcher->_d.texture.simple.py += fetcher->_d.texture.simple.dy;
  }

  // --------------------------------------------------------------------------
  // [Fetch - SubX0 - Clamp]
  // --------------------------------------------------------------------------

  template<typename Accessor>
  static void FOG_FASTCALL fetch_subx0_clamp(
    RasterPatternFetcher* fetcher, RasterSpan* span, uint8_t* buffer)
  {
    const RasterPattern* ctx = fetcher->getContext();
    Accessor accessor(ctx);

    // ------------------------------------------------------------------------
    // [Prepare]
    // ------------------------------------------------------------------------

    int tw = ctx->_d.texture.base.w;
    int th = ctx->_d.texture.base.h;

    uint fY0X0 = ctx->_d.texture.simple.fY1X0;
    uint fY0X1 = ctx->_d.texture.simple.fY1X1;

    const uint8_t* srcLine;

    typename Accessor::Pixel back0;
    typename Accessor::Pixel back1;

    P_FETCH_SPAN8_INIT()
    int y = fetcher->_d.texture.simple.py;
    if ((uint)y >= (uint)th) goto _FetchSolid;

    srcLine = ctx->_d.texture.base.pixels + y * ctx->_d.texture.base.stride;

    // ------------------------------------------------------------------------
    // [Loop]
    // ------------------------------------------------------------------------

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBORS(Accessor::DST_BPP)
      x += ctx->_d.texture.simple.tx;

      const uint8_t* src = srcLine;
      int i;

      // ----------------------------------------------------------------------
      // [Clamp / Border]
      // ----------------------------------------------------------------------

      if (x < -1)
      {
        i = Math::min(-1-x, w);
        w -= i;
        x = -1;

        accessor.fetchSolid(back0, ctx->_d.texture.base.clamp);
        dst = accessor.fill(dst, back0, i);

        if (w == 0) goto _FetchSkip;
        goto _FetchFirst;
      }

      // ----------------------------------------------------------------------
      // [Fetch]
      // ----------------------------------------------------------------------

      else if (x < tw)
      {
        if (x == -1)
        {
          accessor.fetchSolid(back0, ctx->_d.texture.base.clamp);
_FetchFirst:
          accessor.fetchNorm(back1, src);
          accessor.interpolateNorm_2(back0, back0, fY0X0, back1, fY0X1);
          accessor.store(dst, back0);

          dst += Accessor::DST_BPP;
          x++;
          w--;
          if (w == 0) goto _FetchSkip;
        }
        else
        {
          src += (uint)x * Accessor::SRC_BPP;
        }

        accessor.fetchRaw(back0, src);
        src += Accessor::SRC_BPP;

        i = Math::min(tw - 1 - x, w);
        w -= i;

        while (i)
        {
          accessor.fetchRaw(back1, src);
          accessor.interpolateRaw_2(back0, back0, fY0X0, back1, fY0X1);
          accessor.normalize(back0, back0);
          accessor.store(dst, back0);

          back0 = back1;
          dst += Accessor::DST_BPP;
          src += Accessor::SRC_BPP;
          i--;
        }

        // Interpolate last pixel on the row.
        if (w == 0) goto _FetchSkip;

        accessor.normalize(back0, back0);
        accessor.fetchSolid(back1, ctx->_d.texture.base.clamp);
        accessor.interpolateNorm_2(back0, back0, fY0X0, back1, fY0X1);
        accessor.store(dst, back0);

        back0 = back1;
        dst += Accessor::DST_BPP;
        w--;
        if (w == 0) goto _FetchSkip;
      }

      // ----------------------------------------------------------------------
      // [Clamp]
      // ----------------------------------------------------------------------

      accessor.fetchSolid(back0, ctx->_d.texture.base.clamp);
      goto _FetchSolidLoop;

_FetchSkip:
      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()
    goto _FetchEnd;

    // ------------------------------------------------------------------------
    // [Solid]
    // ------------------------------------------------------------------------

_FetchSolid:
    accessor.fetchSolid(back0, ctx->_d.texture.base.clamp);

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()
_FetchSolidLoop:
      dst = accessor.fill(dst, back0, w);
      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()

    // ------------------------------------------------------------------------
    // [Advance]
    // ------------------------------------------------------------------------

_FetchEnd:
    fetcher->_d.texture.simple.py += fetcher->_d.texture.simple.dy;
  }

  // --------------------------------------------------------------------------
  // [Fetch - Sub0Y - Clamp]
  // --------------------------------------------------------------------------

  template<typename Accessor>
  static void FOG_FASTCALL fetch_sub0y_clamp(
    RasterPatternFetcher* fetcher, RasterSpan* span, uint8_t* buffer)
  {
    const RasterPattern* ctx = fetcher->getContext();
    Accessor accessor(ctx);

    // ------------------------------------------------------------------------
    // [Prepare]
    // ------------------------------------------------------------------------

    int tw = ctx->_d.texture.base.w;
    int th = ctx->_d.texture.base.h;

    uint fY0X0 = ctx->_d.texture.simple.fY0X1;
    uint fY1X0 = ctx->_d.texture.simple.fY1X1;

    typename Accessor::Pixel c0;
    typename Accessor::Pixel c1;

    P_FETCH_SPAN8_INIT()
    int y = fetcher->_d.texture.simple.py;
    if ((y < -1) | (y >= th)) goto _FetchSolid;

    // -----------------------------------------------------------------------
    // [Loop - Border]
    // ------------------------------------------------------------------------

    if (y == -1 || y == th - 1)
    {
      // Swap weight if (y == -1).
      if (y < 0) { swap(fY0X0, fY1X0); y++; }
      const uint8_t* srcLine = ctx->_d.texture.base.pixels + y * ctx->_d.texture.base.stride;

      accessor.fetchSolid(c1, ctx->_d.texture.base.clamp);

      P_FETCH_SPAN8_BEGIN()
        P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBORS(Accessor::DST_BPP)
        x += ctx->_d.texture.simple.tx;

        // --------------------------------------------------------------------
        // [Clamp]
        // --------------------------------------------------------------------

        if (x < 0)
        {
          int i = Math::min(-x, w);
          x = 0;
          w -= i;

          dst = accessor.fill(dst, c1, i);
          if (w == 0) goto _FetchBorderSkip;
        }

        // --------------------------------------------------------------------
        // [Fetch]
        // --------------------------------------------------------------------

        if (x < tw)
        {
          const uint8_t* src = srcLine + (uint)x * Accessor::SRC_BPP;
          int i = Math::min(tw - x, w);
          w -= i;

          do {
            accessor.fetchRaw(c0, src);
            accessor.interpolateRaw_2(c0, c0, fY0X0, c1, fY1X0);
            accessor.normalize(c0, c0);
            accessor.store(dst, c0);

            dst += Accessor::DST_BPP;
            src += Accessor::SRC_BPP;
          } while (--i);
          if (w == 0) goto _FetchBorderSkip;
        }

        // --------------------------------------------------------------------
        // [Clamp]
        // --------------------------------------------------------------------

        accessor.fetchSolid(c1, ctx->_d.texture.base.clamp);
        goto _FetchSolidLoop;

_FetchBorderSkip:
        P_FETCH_SPAN8_NEXT()
      P_FETCH_SPAN8_END()
    }

    // -----------------------------------------------------------------------
    // [Loop - Inside]
    // ------------------------------------------------------------------------

    else
    {
      const uint8_t* srcLine0 = ctx->_d.texture.base.pixels + y * ctx->_d.texture.base.stride;
      const uint8_t* srcLine1 = srcLine0 + ctx->_d.texture.base.stride;

      P_FETCH_SPAN8_BEGIN()
        P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBORS(Accessor::DST_BPP)
        x += ctx->_d.texture.simple.tx;

        // --------------------------------------------------------------------
        // [Clamp]
        // --------------------------------------------------------------------

        if (x < 0)
        {
          int i = Math::min(-x, w);
          x = 0;
          w -= i;

          accessor.fetchSolid(c1, ctx->_d.texture.base.clamp);
          dst = accessor.fill(dst, c1, i);
          if (w == 0) goto _FetchInsideSkip;
        }

        // --------------------------------------------------------------------
        // [Fetch]
        // --------------------------------------------------------------------

        if (x < tw)
        {
          const uint8_t* srcCur0 = srcLine0 + (uint)x * Accessor::SRC_BPP;
          const uint8_t* srcCur1 = srcLine1 + (uint)x * Accessor::SRC_BPP;

          int i = Math::min(tw - x, w);
          w -= i;

          do {
            accessor.fetchRaw(c0, srcCur0);
            accessor.fetchRaw(c1, srcCur1);
            accessor.interpolateRaw_2(c0, c0, fY0X0, c1, fY1X0);
            accessor.normalize(c0, c0);
            accessor.store(dst, c0);

            dst += Accessor::DST_BPP;
            srcCur0 += Accessor::SRC_BPP;
            srcCur1 += Accessor::SRC_BPP;
          } while (--i);
          if (w == 0) goto _FetchInsideSkip;
        }

        // --------------------------------------------------------------------
        // [Clamp]
        // --------------------------------------------------------------------

        accessor.fetchSolid(c1, ctx->_d.texture.base.clamp);
        goto _FetchSolidLoop;

_FetchInsideSkip:
        P_FETCH_SPAN8_NEXT()
      P_FETCH_SPAN8_END()
    }
    goto _FetchEnd;

    // ------------------------------------------------------------------------
    // [Solid]
    // ------------------------------------------------------------------------

_FetchSolid:
    accessor.fetchSolid(c1, ctx->_d.texture.base.clamp);

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()
_FetchSolidLoop:
      dst = accessor.fill(dst, c1, w);
      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()

    // ------------------------------------------------------------------------
    // [Advance]
    // ------------------------------------------------------------------------

_FetchEnd:
    fetcher->_d.texture.simple.py += fetcher->_d.texture.simple.dy;
  }

  // --------------------------------------------------------------------------
  // [Fetch - SubXY - Clamp]
  // --------------------------------------------------------------------------

  template<typename Accessor>
  static void FOG_FASTCALL fetch_subxy_clamp(
    RasterPatternFetcher* fetcher, RasterSpan* span, uint8_t* buffer)
  {
    const RasterPattern* ctx = fetcher->getContext();
    Accessor accessor(ctx);

    // ------------------------------------------------------------------------
    // [Prepare]
    // ------------------------------------------------------------------------

    int tw = ctx->_d.texture.base.w;
    int th = ctx->_d.texture.base.h;

    uint fY0X0 = ctx->_d.texture.simple.fY0X0;
    uint fY0X1 = ctx->_d.texture.simple.fY0X1;
    uint fY1X0 = ctx->_d.texture.simple.fY1X0;
    uint fY1X1 = ctx->_d.texture.simple.fY1X1;

    typename Accessor::Pixel back0;
    typename Accessor::Pixel back1;

    P_FETCH_SPAN8_INIT()
    int y = fetcher->_d.texture.simple.py;
    if ((y < -1) | (y >= th)) goto _FetchSolid;

    // ------------------------------------------------------------------------
    // [Loop - Border]
    // ------------------------------------------------------------------------

    if ((uint)y >= (uint)(th - 1))
    {
      if (y < 0) { y++; swap(fY0X0, fY1X0); swap(fY0X1, fY1X1); }

      P_FETCH_SPAN8_BEGIN()
        P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBORS(Accessor::DST_BPP)
        x += ctx->_d.texture.simple.tx;

        const uint8_t* srcLine = ctx->_d.texture.base.pixels + y * ctx->_d.texture.base.stride;
        const uint8_t* src = srcLine;
        int i;

        accessor.fetchSolid(back0, ctx->_d.texture.base.clamp);

        // Precompute the clamped pixel
        typename Accessor::Pixel clamp;
        clamp = back0;
        accessor._cmul(clamp, clamp, fY1X0 + fY1X1);

        // --------------------------------------------------------------------
        // [Clamp]
        // --------------------------------------------------------------------

        if (x < -1)
        {
          i = Math::min(-1-x, w);
          w -= i;
          x = -1;

          dst = accessor.fill(dst, back0, i);

          if (w == 0) goto _FetchBorderSkip;
          goto _FetchBorderFirst;
        }

        // --------------------------------------------------------------------
        // [Fetch]
        // --------------------------------------------------------------------

        else if (x < tw)
        {
          if (x >= 0)
          {
            src += (uint)x * Accessor::SRC_BPP;
            accessor.fetchNorm(back0, src - Accessor::SRC_BPP);
          }

_FetchBorderFirst:
          i = Math::min(tw - 1 - x, w);
          w -= i;

          while (i)
          {
            accessor.fetchNorm(back1, src);
            accessor.interpolateNorm_2(back0, back0, fY0X0, back1, fY0X1);
            accessor._cadd(back0, back0, clamp);
            accessor.store(dst, back0);

            back0 = back1;
            dst += Accessor::DST_BPP;
            src += Accessor::SRC_BPP;
            i--;
          }

          // Interpolate the last pixel on the row.
          if (w == 0) goto _FetchBorderSkip;

          accessor.fetchSolid(back1, ctx->_d.texture.base.clamp);
          accessor.interpolateNorm_2(back0, back0, fY0X0, back1, fY0X1 + fY1X0 + fY1X1);
          accessor.store(dst, back0);

          dst += Accessor::DST_BPP;
          w--;
          if (w == 0) goto _FetchBorderSkip;
        }

        // --------------------------------------------------------------------
        // [Clamp]
        // --------------------------------------------------------------------

        accessor.fetchSolid(back0, ctx->_d.texture.base.clamp);
        goto _FetchSolidLoop;

_FetchBorderSkip:
        P_FETCH_SPAN8_NEXT()
      P_FETCH_SPAN8_END()
    }

    // ------------------------------------------------------------------------
    // [Loop - Inside]
    // ------------------------------------------------------------------------

    else
    {
      const uint8_t* srcLine = ctx->_d.texture.base.pixels + y * ctx->_d.texture.base.stride;
      const uint8_t* srcCur0 = srcLine;
      const uint8_t* srcCur1 = srcLine + ctx->_d.texture.base.stride;

      P_FETCH_SPAN8_BEGIN()
        P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBORS(Accessor::DST_BPP)
        x += ctx->_d.texture.simple.tx;

        int i;

        // --------------------------------------------------------------------
        // [Clamp]
        // --------------------------------------------------------------------

        if (x < -1)
        {
          i = Math::min(-1-x, w);
          w -= i;
          x = -1;

          accessor.fetchSolid(back0, ctx->_d.texture.base.clamp);
          dst = accessor.fill(dst, back0, i);

          if (w == 0) goto _FetchInsideSkip;
          goto _FetchInsideFirst;
        }

        // --------------------------------------------------------------------
        // [Fetch]
        // --------------------------------------------------------------------

        else if (x < tw)
        {
          if (x == -1)
          {
_FetchInsideFirst:
            // Interpolate the first pixel with the border (clamp) pixel. This
            // is needed to use RAW pixels in the main loop.
            accessor.fetchNorm(back0, srcCur0);
            accessor.fetchNorm(back1, srcCur1);
            accessor.interpolateNorm_2(back0, back0, fY0X1, back1, fY1X1);

            accessor.fetchSolid(back1, ctx->_d.texture.base.clamp);
            accessor._cmul(back1, back1, fY0X0 + fY1X0);
            accessor._cadd(back0, back0, back1);
            accessor.store(dst, back0);

            dst += Accessor::DST_BPP;
            x++;
            w--;
            if (w == 0) goto _FetchInsideSkip;
          }
          else
          {
            srcCur0 += (uint)x * Accessor::SRC_BPP;
            srcCur1 += (uint)x * Accessor::SRC_BPP;
          }

          accessor.fetchRaw(back0, srcCur0);
          accessor.fetchRaw(back1, srcCur1);
          srcCur0 += Accessor::SRC_BPP;
          srcCur1 += Accessor::SRC_BPP;

          i = Math::min(tw - 1 - x, w);
          w -= i;

          while (i)
          {
            typename Accessor::Pixel back0_1;
            typename Accessor::Pixel back1_1;

            accessor.fetchRaw(back0_1, srcCur0);
            accessor.fetchRaw(back1_1, srcCur1);
            accessor.interpolateRaw_4(back0, back0, fY0X0, back0_1, fY0X1, back1, fY1X0, back1_1, fY1X1);
            accessor.normalize(back0, back0);
            accessor.store(dst, back0);

            back0 = back0_1;
            back1 = back1_1;
            dst     += Accessor::DST_BPP;
            srcCur0 += Accessor::SRC_BPP;
            srcCur1 += Accessor::SRC_BPP;
            i--;
          }

          // Interpolate last pixel on the row.
          if (w == 0) goto _FetchInsideSkip;

          accessor.normalize(back0, back0);
          accessor.normalize(back1, back1);
          accessor.interpolateNorm_2(back0, back0, fY0X0, back1, fY1X0);

          accessor.fetchSolid(back1, ctx->_d.texture.base.clamp);
          accessor._cmul(back1, back1, fY0X1 + fY1X1);
          accessor._cadd(back0, back0, back1);
          accessor.store(dst, back0);

          dst += Accessor::DST_BPP;
          if (!--w) goto _FetchInsideSkip;
        }

        // --------------------------------------------------------------------
        // [Clamp]
        // --------------------------------------------------------------------

        accessor.fetchSolid(back0, ctx->_d.texture.base.clamp);
        goto _FetchSolidLoop;

_FetchInsideSkip:
        P_FETCH_SPAN8_NEXT()
      P_FETCH_SPAN8_END()
    }
    goto _FetchEnd;

    // ------------------------------------------------------------------------
    // [Solid]
    // ------------------------------------------------------------------------

_FetchSolid:
    accessor.fetchSolid(back0, ctx->_d.texture.base.clamp);

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()
_FetchSolidLoop:
      dst = accessor.fill(dst, back0, w);
      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()

    // ------------------------------------------------------------------------
    // [Advance]
    // ------------------------------------------------------------------------

_FetchEnd:
    fetcher->_d.texture.simple.py += fetcher->_d.texture.simple.dy;
  }
};

} // RasterOps_C namespace
} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTEROPS_C_TEXTURESIMPLE_P_H
