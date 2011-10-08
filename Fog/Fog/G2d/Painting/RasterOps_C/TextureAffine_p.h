// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTEROPS_C_TEXTUREAFFINE_P_H
#define _FOG_G2D_PAINTING_RASTEROPS_C_TEXTUREAFFINE_P_H

// [Dependencies]
#include <Fog/G2d/Painting/RasterOps_C/TextureBase_p.h>

namespace Fog {
namespace RasterOps_C {

// ============================================================================
// [Fog::RasterOps_C - PTextureAffine]
// ============================================================================

struct FOG_NO_EXPORT PTextureAffine
{
  // --------------------------------------------------------------------------
  // [Constants]
  // --------------------------------------------------------------------------

  enum { MAX_FIXED_STEP = 128 };

  // --------------------------------------------------------------------------
  // [Fetch - Affine (Nearest) - Pad]
  // --------------------------------------------------------------------------

  template<typename Accessor>
  static void FOG_FASTCALL fetch_affine_nearest_pad(
    RasterPatternFetcher* fetcher, RasterSpan* span, uint8_t* buffer)
  {
    const RasterPattern* ctx = fetcher->getContext();
    Accessor accessor(ctx);

    // ------------------------------------------------------------------------
    // [Prepare]
    // ------------------------------------------------------------------------

    int tw = ctx->_d.texture.base.w;
    int th = ctx->_d.texture.base.h;

    double xx = ctx->_d.texture.affine.xx;
    double xy = ctx->_d.texture.affine.xy;

    double offx = fetcher->_d.texture.affine.px;
    double offy = fetcher->_d.texture.affine.py;

    const uint8_t* srcPixels = ctx->_d.texture.base.pixels;
    ssize_t srcStride = ctx->_d.texture.base.stride;

    P_FETCH_SPAN8_INIT()

    tw--;
    th--;

    // ------------------------------------------------------------------------
    // [Loop - FixedPoint]
    // ------------------------------------------------------------------------

    if (ctx->_d.texture.affine.safeFixedPoint)
    {
      int xx16x16 = ctx->_d.texture.affine.xx16x16;
      int xy16x16 = ctx->_d.texture.affine.xy16x16;

      if (ctx->_d.texture.affine.xyZero)
      {
        int py0 = Math::bound<int>((int)offy, 0, th);
        srcPixels += py0 * srcStride;

        P_FETCH_SPAN8_BEGIN()
          P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBORS(Accessor::DST_BPP)
          double _x = (double)x;

          for (;;)
          {
            int i = Math::min<int>(w, MAX_FIXED_STEP);
            int px = Math::fixed16x16FromFloat(offx + _x * xx);

            w -= i;

            do {
              int px0 = Math::bound<int>(px >> 16, 0, tw);

              typename Accessor::Pixel pix;
              accessor.fetchNorm(pix, srcPixels + px0 * Accessor::SRC_BPP);
              accessor.store(dst, pix);

              dst += Accessor::DST_BPP;
              px += xx16x16;
            } while (--i);

            if (w == 0) break;
            _x += (double)MAX_FIXED_STEP;
          }

          P_FETCH_SPAN8_NEXT()
        P_FETCH_SPAN8_END()
      }
      else
      {
        P_FETCH_SPAN8_BEGIN()
          P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBORS(Accessor::DST_BPP)
          double _x = (double)x;

          for (;;)
          {
            int i = Math::min<int>(w, MAX_FIXED_STEP);
            int px = Math::fixed16x16FromFloat(offx + _x * xx);
            int py = Math::fixed16x16FromFloat(offy + _x * xy);

            w -= i;

            do {
              int px0 = Math::bound<int>(px >> 16, 0, tw);
              int py0 = Math::bound<int>(py >> 16, 0, th);

              typename Accessor::Pixel pix;
              accessor.fetchNorm(pix, srcPixels + (ssize_t)py0 * srcStride + px0 * Accessor::SRC_BPP);
              accessor.store(dst, pix);

              dst += Accessor::DST_BPP;
              px += xx16x16;
              py += xy16x16;
            } while (--i);

            if (w == 0) break;
            _x += (double)MAX_FIXED_STEP;
          }

          P_FETCH_SPAN8_NEXT()
        P_FETCH_SPAN8_END()
      }
    }

    // ------------------------------------------------------------------------
    // [Loop - Float]
    // ------------------------------------------------------------------------

    else
    {
      P_FETCH_SPAN8_BEGIN()
        P_FETCH_SPAN8_SET_CURRENT()
        double _x = (double)x;

        double px = offx + _x * xx;
        double py = offy + _x * xy;

        do {
          int px0 = Math::bound<int>((int)px, 0, tw);
          int py0 = Math::bound<int>((int)py, 0, th);

          typename Accessor::Pixel pix;
          accessor.fetchNorm(pix, srcPixels + py0 * srcStride + px0 * Accessor::SRC_BPP);
          accessor.store(dst, pix);

          dst += Accessor::DST_BPP;
          px += xx;
          py += xy;
        } while (--w);

        P_FETCH_SPAN8_NEXT()
      P_FETCH_SPAN8_END()
    }

    // ------------------------------------------------------------------------
    // [Advance]
    // ------------------------------------------------------------------------

    fetcher->_d.texture.affine.px += fetcher->_d.texture.affine.dx;
    fetcher->_d.texture.affine.py += fetcher->_d.texture.affine.dy;
  }

  // --------------------------------------------------------------------------
  // [Fetch - Affine (Bilinear) - Pad]
  // --------------------------------------------------------------------------

  template<typename Accessor>
  static void FOG_FASTCALL fetch_affine_bilinear_pad(
    RasterPatternFetcher* fetcher, RasterSpan* span, uint8_t* buffer)
  {
    const RasterPattern* ctx = fetcher->getContext();
    Accessor accessor(ctx);

    // ------------------------------------------------------------------------
    // [Prepare]
    // ------------------------------------------------------------------------

    int tw = ctx->_d.texture.base.w;
    int th = ctx->_d.texture.base.h;

    double xx = ctx->_d.texture.affine.xx;
    double xy = ctx->_d.texture.affine.xy;

    double offx = fetcher->_d.texture.affine.px;
    double offy = fetcher->_d.texture.affine.py;

    const uint8_t* srcPixels = ctx->_d.texture.base.pixels;
    ssize_t srcStride = ctx->_d.texture.base.stride;

    P_FETCH_SPAN8_INIT()

    tw--;
    th--;

    // ------------------------------------------------------------------------
    // [Loop - FixedPoint]
    // ------------------------------------------------------------------------

    if (ctx->_d.texture.affine.safeFixedPoint)
    {
      int xx16x16 = ctx->_d.texture.affine.xx16x16;
      int xy16x16 = ctx->_d.texture.affine.xy16x16;

      if (ctx->_d.texture.affine.xyZero)
      {
        int py0 = Math::bound<int>((int)offy, 0, th);

        uint32_t wy = (uint)(Math::fixed24x8FromFloat(offy) & 0xFF);
        uint32_t inv_wy = 0x100 - wy;

        const uint8_t* srcLine0 = srcPixels + py0 * srcStride;
        if (++py0 > th) py0 = th;
        const uint8_t* srcLine1 = srcPixels + py0 * srcStride;

        P_FETCH_SPAN8_BEGIN()
          P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBORS(Accessor::DST_BPP)
          double _x = (double)x;

          for (;;)
          {
            int i = Math::min<int>(w, MAX_FIXED_STEP);
            int px = Math::fixed16x16FromFloat(offx + _x * xx);

            w -= i;

            do {
              int px0 = px >> 16;

              typename Accessor::Pixel pix_x0y0;
              typename Accessor::Pixel pix_x1y0;
              typename Accessor::Pixel pix_x0y1;
              typename Accessor::Pixel pix_x1y1;

              if (FOG_LIKELY((uint)px0 < (uint)tw))
              {
                accessor.fetchRaw(pix_x0y0, srcLine0 + (uint)px0 * Accessor::SRC_BPP);
                accessor.fetchRaw(pix_x1y0, srcLine0 + (uint)px0 * Accessor::SRC_BPP + Accessor::SRC_BPP);
                accessor.fetchRaw(pix_x0y1, srcLine1 + (uint)px0 * Accessor::SRC_BPP);
                accessor.fetchRaw(pix_x1y1, srcLine1 + (uint)px0 * Accessor::SRC_BPP + Accessor::SRC_BPP);
              }
              else
              {
                if (px0 < 0) px0 = 0; else px0 = tw;
                accessor.fetchRaw(pix_x0y0, srcLine0 + (uint)px0 * Accessor::SRC_BPP);
                accessor.fetchRaw(pix_x0y1, srcLine1 + (uint)px0 * Accessor::SRC_BPP);

                pix_x1y0 = pix_x0y0;
                pix_x1y1 = pix_x0y1;
              }

              uint32_t wx = (uint)(px >> 8) & 0xFF;

              accessor.interpolateRaw_4(pix_x0y0,
                pix_x0y0, ((0x100 - wx) * (inv_wy)) >> 8,
                pix_x1y0, ((wx        ) * (inv_wy)) >> 8,
                pix_x0y1, ((0x100 - wx) * (wy    )) >> 8,
                pix_x1y1, ((wx        ) * (wy    )) >> 8);
              accessor.normalize(pix_x0y0, pix_x0y0);
              accessor.store(dst, pix_x0y0);

              dst += Accessor::DST_BPP;
              px += xx16x16;
            } while (--i);

            if (w == 0) break;
            _x += (double)MAX_FIXED_STEP;
          }

          P_FETCH_SPAN8_NEXT()
        P_FETCH_SPAN8_END()
      }
      else
      {
        P_FETCH_SPAN8_BEGIN()
          P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBORS(Accessor::DST_BPP)
          double _x = (double)x;

          for (;;)
          {
            int i = Math::min<int>(w, MAX_FIXED_STEP);
            int px = Math::fixed16x16FromFloat(offx + _x * xx);
            int py = Math::fixed16x16FromFloat(offy + _x * xy);

            w -= i;

            do {
              int px0 = px >> 16;
              int py0 = py >> 16;

              typename Accessor::Pixel pix_x0y0;
              typename Accessor::Pixel pix_x1y0;
              typename Accessor::Pixel pix_x0y1;
              typename Accessor::Pixel pix_x1y1;

              if (FOG_LIKELY(((uint)px0 < (uint)tw) & ((uint)py0 < (uint)th)))
              {
                const uint8_t* srcLine = srcPixels + py0 * srcStride + (uint)px0 * Accessor::SRC_BPP;

                accessor.fetchRaw(pix_x0y0, srcLine);
                accessor.fetchRaw(pix_x1y0, srcLine + Accessor::SRC_BPP);
                srcLine += srcStride;
                accessor.fetchRaw(pix_x0y1, srcLine);
                accessor.fetchRaw(pix_x1y1, srcLine + Accessor::SRC_BPP);
              }
              else
              {
                int px1 = px0 + 1;
                int py1 = py0 + 1;

                if (px0 < 0) { px0 = px1 = 0; } else if (px0 >= tw) { px0 = px1 = tw; }
                if (py0 < 0) { py0 = py1 = 0; } else if (py0 >= th) { py0 = py1 = th; }

                const uint8_t* srcLine0 = srcPixels + (uint)py0 * srcStride;
                const uint8_t* srcLine1 = srcPixels + (uint)py1 * srcStride;

                accessor.fetchRaw(pix_x0y0, srcLine0 + px0 * Accessor::SRC_BPP);
                accessor.fetchRaw(pix_x1y0, srcLine0 + px1 * Accessor::SRC_BPP);
                accessor.fetchRaw(pix_x0y1, srcLine1 + px0 * Accessor::SRC_BPP);
                accessor.fetchRaw(pix_x1y1, srcLine1 + px1 * Accessor::SRC_BPP);
              }

              uint32_t wx = (uint)(px >> 8) & 0xFF;
              uint32_t wy = (uint)(py >> 8) & 0xFF;

              accessor.interpolateRaw_4(pix_x0y0,
                pix_x0y0, ((0x100 - wx) * (0x100 - wy)) >> 8,
                pix_x1y0, ((wx        ) * (0x100 - wy)) >> 8,
                pix_x0y1, ((0x100 - wx) * (wy        )) >> 8,
                pix_x1y1, ((wx        ) * (wy        )) >> 8);
              accessor.normalize(pix_x0y0, pix_x0y0);
              accessor.store(dst, pix_x0y0);

              dst += Accessor::DST_BPP;
              px += xx16x16;
              py += xy16x16;
            } while (--i);

            if (w == 0) break;
            _x += (double)MAX_FIXED_STEP;
          }

          P_FETCH_SPAN8_NEXT()
        P_FETCH_SPAN8_END()
      }
    }

    // ------------------------------------------------------------------------
    // [Loop - Float]
    // ------------------------------------------------------------------------

    else
    {
      P_FETCH_SPAN8_BEGIN()
        P_FETCH_SPAN8_SET_CURRENT()
        double _x = (double)x;

        double px = offx + _x * xx;
        double py = offy + _x * xy;

        do {
          int px0 = (int)px;
          int py0 = (int)py;

          typename Accessor::Pixel pix_x0y0;
          typename Accessor::Pixel pix_x1y0;
          typename Accessor::Pixel pix_x0y1;
          typename Accessor::Pixel pix_x1y1;

          if (FOG_LIKELY(((uint)px0 < (uint)tw) & ((uint)py0 < (uint)th)))
          {
            const uint8_t* srcLine = srcPixels + py0 * srcStride + (uint)px0 * Accessor::SRC_BPP;

            accessor.fetchRaw(pix_x0y0, srcLine);
            accessor.fetchRaw(pix_x1y0, srcLine + Accessor::SRC_BPP);
            srcLine += srcStride;
            accessor.fetchRaw(pix_x0y1, srcLine);
            accessor.fetchRaw(pix_x1y1, srcLine + Accessor::SRC_BPP);
          }
          else
          {
            int px1 = px0 + 1;
            int py1 = py0 + 1;

            if (px0 < 0) { px0 = px1 = 0; } else if (px0 >= tw) { px0 = px1 = tw; }
            if (py0 < 0) { py0 = py1 = 0; } else if (py0 >= th) { py0 = py1 = th; }

            const uint8_t* srcLine0 = srcPixels + (uint)py0 * srcStride;
            const uint8_t* srcLine1 = srcPixels + (uint)py1 * srcStride;

            accessor.fetchRaw(pix_x0y0, srcLine0 + px0 * Accessor::SRC_BPP);
            accessor.fetchRaw(pix_x1y0, srcLine0 + px1 * Accessor::SRC_BPP);
            accessor.fetchRaw(pix_x0y1, srcLine1 + px0 * Accessor::SRC_BPP);
            accessor.fetchRaw(pix_x1y1, srcLine1 + px1 * Accessor::SRC_BPP);
          }

          uint32_t wx = Math::fixed24x8FromFloat(px) & 0xFF;
          uint32_t wy = Math::fixed24x8FromFloat(py) & 0xFF;

          accessor.interpolateRaw_4(pix_x0y0,
            pix_x0y0, ((0x100 - wx) * (0x100 - wy)) >> 8,
            pix_x1y0, ((wx        ) * (0x100 - wy)) >> 8,
            pix_x0y1, ((0x100 - wx) * (wy        )) >> 8,
            pix_x1y1, ((wx        ) * (wy        )) >> 8);
          accessor.normalize(pix_x0y0, pix_x0y0);
          accessor.store(dst, pix_x0y0);

          dst += Accessor::DST_BPP;
          px += xx;
          py += xy;
        } while (--w);
        P_FETCH_SPAN8_NEXT()
      P_FETCH_SPAN8_END()
    }

    // ------------------------------------------------------------------------
    // [Advance]
    // ------------------------------------------------------------------------

    fetcher->_d.texture.affine.px += fetcher->_d.texture.affine.dx;
    fetcher->_d.texture.affine.py += fetcher->_d.texture.affine.dy;
  }

  // --------------------------------------------------------------------------
  // [Fetch - Affine (Nearest) - Repeat]
  // --------------------------------------------------------------------------

  template<typename Accessor>
  static void FOG_FASTCALL fetch_affine_nearest_repeat(
    RasterPatternFetcher* fetcher, RasterSpan* span, uint8_t* buffer)
  {
    const RasterPattern* ctx = fetcher->getContext();
    Accessor accessor(ctx);

    // ------------------------------------------------------------------------
    // [Prepare]
    // ------------------------------------------------------------------------

    int tw = ctx->_d.texture.base.w;
    int th = ctx->_d.texture.base.h;

    double xx = ctx->_d.texture.affine.xx;
    double xy = ctx->_d.texture.affine.xy;

    double offx = fetcher->_d.texture.affine.px;
    double offy = fetcher->_d.texture.affine.py;

    const uint8_t* srcPixels = ctx->_d.texture.base.pixels;
    ssize_t srcStride = ctx->_d.texture.base.stride;

    P_FETCH_SPAN8_INIT()

    tw--;
    th--;

    // ------------------------------------------------------------------------
    // [Loop - FixedPoint]
    // ------------------------------------------------------------------------

    if (ctx->_d.texture.affine.safeFixedPoint)
    {
      int xx16x16 = ctx->_d.texture.affine.xx16x16;
      int xy16x16 = ctx->_d.texture.affine.xy16x16;

      int mx16x16 = ctx->_d.texture.affine.mx16x16;
      int my16x16 = ctx->_d.texture.affine.my16x16;

      int rx16x16 = ctx->_d.texture.affine.rx16x16;
      int ry16x16 = ctx->_d.texture.affine.ry16x16;

      if (ctx->_d.texture.affine.xyZero)
      {
        int py0 = (int)(offy);
        FOG_ASSERT(py0 >= 0 && py0 <= th);
        srcPixels += py0 * srcStride;

        P_FETCH_SPAN8_BEGIN()
          P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBORS(Accessor::DST_BPP)
          double _x = (double)x;

          for (;;)
          {
            int i = Math::min<int>(w, MAX_FIXED_STEP);
            int px = Helpers::p_repeat_integer(Math::fixed16x16FromFloat(offx + _x * xx), mx16x16);

            w -= i;

            do {
              int px0 = px >> 16;

              typename Accessor::Pixel pix;
              accessor.fetchNorm(pix, srcPixels + px0 * Accessor::SRC_BPP);
              accessor.store(dst, pix);

              dst += Accessor::DST_BPP;
              px += xx16x16;
              if ((uint)px >= (uint)mx16x16) px += rx16x16;
            } while (--i);

            if (w == 0) break;
            _x += (double)MAX_FIXED_STEP;
          }

          P_FETCH_SPAN8_NEXT()
        P_FETCH_SPAN8_END()
      }
      else
      {
        P_FETCH_SPAN8_BEGIN()
          P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBORS(Accessor::DST_BPP)
          double _x = (double)x;

          for (;;)
          {
            int i = Math::min<int>(w, MAX_FIXED_STEP);
            int px = Helpers::p_repeat_integer(Math::fixed16x16FromFloat(offx + _x * xx), mx16x16);
            int py = Helpers::p_repeat_integer(Math::fixed16x16FromFloat(offy + _x * xy), my16x16);

            w -= i;

            do {
              int px0 = px >> 16;
              int py0 = py >> 16;

              typename Accessor::Pixel pix;
              accessor.fetchNorm(pix, srcPixels + (ssize_t)py0 * srcStride + px0 * Accessor::SRC_BPP);
              accessor.store(dst, pix);

              dst += Accessor::DST_BPP;
              px += xx16x16;
              py += xy16x16;

              if ((uint)px >= (uint)mx16x16) px += rx16x16;
              if ((uint)py >= (uint)my16x16) py += ry16x16;
            } while (--i);

            if (w == 0) break;
            _x += (double)MAX_FIXED_STEP;
          }

          P_FETCH_SPAN8_NEXT()
        P_FETCH_SPAN8_END()
      }
    }

    // ------------------------------------------------------------------------
    // [Loop - Float]
    // ------------------------------------------------------------------------

    else
    {
      double mx = ctx->_d.texture.affine.mx;
      double my = ctx->_d.texture.affine.my;

      double rx = ctx->_d.texture.affine.rx;
      double ry = ctx->_d.texture.affine.ry;

      P_FETCH_SPAN8_BEGIN()
        P_FETCH_SPAN8_SET_CURRENT()
        double _x = (double)x;

        double px = Math::repeat(offx + _x * xx, mx);
        double py = Math::repeat(offy + _x * xy, my);

        do {
          int px0 = (int)px;
          int py0 = (int)py;

          typename Accessor::Pixel pix;
          accessor.fetchNorm(pix, srcPixels + py0 * srcStride + px0 * Accessor::SRC_BPP);
          accessor.store(dst, pix);

          dst += Accessor::DST_BPP;
          px += xx; if ((mx < 0.0) | (px >= mx)) px += rx;
          py += xy; if ((my < 0.0) | (py >= my)) py += ry;
        } while (--w);

        P_FETCH_SPAN8_NEXT()
      P_FETCH_SPAN8_END()
    }

    // ------------------------------------------------------------------------
    // [Advance]
    // ------------------------------------------------------------------------

    fetcher->_d.texture.affine.px += fetcher->_d.texture.affine.dx;
    fetcher->_d.texture.affine.py += fetcher->_d.texture.affine.dy;

    if (fetcher->_d.texture.affine.py < 0.0)
      fetcher->_d.texture.affine.py += ctx->_d.texture.affine.my;
    if (fetcher->_d.texture.affine.py >= ctx->_d.texture.affine.my)
      fetcher->_d.texture.affine.py -= ctx->_d.texture.affine.my;
  }

  // --------------------------------------------------------------------------
  // [Fetch - Affine (Bilinear) - Repeat]
  // --------------------------------------------------------------------------

  template<typename Accessor>
  static void FOG_FASTCALL fetch_affine_bilinear_repeat(
    RasterPatternFetcher* fetcher, RasterSpan* span, uint8_t* buffer)
  {
    const RasterPattern* ctx = fetcher->getContext();
    Accessor accessor(ctx);

    // ------------------------------------------------------------------------
    // [Prepare]
    // ------------------------------------------------------------------------

    int tw = ctx->_d.texture.base.w;
    int th = ctx->_d.texture.base.h;

    double xx = ctx->_d.texture.affine.xx;
    double xy = ctx->_d.texture.affine.xy;

    double offx = fetcher->_d.texture.affine.px;
    double offy = fetcher->_d.texture.affine.py;

    const uint8_t* srcPixels = ctx->_d.texture.base.pixels;
    ssize_t srcStride = ctx->_d.texture.base.stride;

    P_FETCH_SPAN8_INIT()

    tw--;
    th--;

    // ------------------------------------------------------------------------
    // [Loop - FixedPoint]
    // ------------------------------------------------------------------------

    if (ctx->_d.texture.affine.safeFixedPoint)
    {
      int xx16x16 = ctx->_d.texture.affine.xx16x16;
      int xy16x16 = ctx->_d.texture.affine.xy16x16;

      int mx16x16 = ctx->_d.texture.affine.mx16x16;
      int my16x16 = ctx->_d.texture.affine.my16x16;

      int rx16x16 = ctx->_d.texture.affine.rx16x16;
      int ry16x16 = ctx->_d.texture.affine.ry16x16;

      if (ctx->_d.texture.affine.xyZero)
      {
        int py0 = (int)(offy);
        FOG_ASSERT(py0 >= 0 && py0 <= th);

        uint32_t wy = (uint)(Math::fixed24x8FromFloat(offy) & 0xFF);
        uint32_t inv_wy = 0x100 - wy;

        const uint8_t* srcLine0 = srcPixels + py0 * srcStride;
        if (++py0 > th) py0 = 0;
        const uint8_t* srcLine1 = srcPixels + py0 * srcStride;

        P_FETCH_SPAN8_BEGIN()
          P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBORS(Accessor::DST_BPP)
          double _x = (double)x;

          for (;;)
          {
            int i = Math::min<int>(w, MAX_FIXED_STEP);
            int px = Helpers::p_repeat_integer(Math::fixed16x16FromFloat(offx + _x * xx), mx16x16);

            w -= i;

            do {
              int px0 = px >> 16;

              typename Accessor::Pixel pix_x0y0;
              typename Accessor::Pixel pix_x1y0;
              typename Accessor::Pixel pix_x0y1;
              typename Accessor::Pixel pix_x1y1;

              accessor.fetchRaw(pix_x0y0, srcLine0 + px0 * Accessor::SRC_BPP);
              accessor.fetchRaw(pix_x0y1, srcLine1 + px0 * Accessor::SRC_BPP);

              if (++px0 > tw) px0 = 0;

              accessor.fetchRaw(pix_x1y0, srcLine0 + px0 * Accessor::SRC_BPP);
              accessor.fetchRaw(pix_x1y1, srcLine1 + px0 * Accessor::SRC_BPP);

              uint32_t wx = (uint)(px >> 8) & 0xFF;

              accessor.interpolateRaw_4(pix_x0y0,
                pix_x0y0, ((0x100 - wx) * (inv_wy)) >> 8,
                pix_x1y0, ((wx        ) * (inv_wy)) >> 8,
                pix_x0y1, ((0x100 - wx) * (wy    )) >> 8,
                pix_x1y1, ((wx        ) * (wy    )) >> 8);
              accessor.normalize(pix_x0y0, pix_x0y0);
              accessor.store(dst, pix_x0y0);

              dst += Accessor::DST_BPP;
              px += xx16x16;
              if ((uint)px >= (uint)mx16x16) px += rx16x16;
            } while (--i);

            if (w == 0) break;
            _x += (double)MAX_FIXED_STEP;
          }

          P_FETCH_SPAN8_NEXT()
        P_FETCH_SPAN8_END()
      }
      else
      {
        P_FETCH_SPAN8_BEGIN()
          P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBORS(Accessor::DST_BPP)
          double _x = (double)x;

          for (;;)
          {
            int i = Math::min<int>(w, MAX_FIXED_STEP);
            int px = Helpers::p_repeat_integer(Math::fixed16x16FromFloat(offx + _x * xx), mx16x16);
            int py = Helpers::p_repeat_integer(Math::fixed16x16FromFloat(offy + _x * xy), my16x16);

            w -= i;

            do {
              int px0 = px >> 16;
              int py0 = py >> 16;

              typename Accessor::Pixel pix_x0y0;
              typename Accessor::Pixel pix_x1y0;
              typename Accessor::Pixel pix_x0y1;
              typename Accessor::Pixel pix_x1y1;

              const uint8_t* srcLine0 = srcPixels + (uint)py0 * srcStride;
              if (++py0 > th) py0 = 0;
              const uint8_t* srcLine1 = srcPixels + (uint)py0 * srcStride;

              accessor.fetchRaw(pix_x0y0, srcLine0 + (uint)px0 * Accessor::SRC_BPP);
              accessor.fetchRaw(pix_x0y1, srcLine1 + (uint)px0 * Accessor::SRC_BPP);

              if (++px0 > tw) px0 = 0;

              accessor.fetchRaw(pix_x1y0, srcLine0 + (uint)px0 * Accessor::SRC_BPP);
              accessor.fetchRaw(pix_x1y1, srcLine1 + (uint)px0 * Accessor::SRC_BPP);

              uint32_t wx = (uint)(px >> 8) & 0xFF;
              uint32_t wy = (uint)(py >> 8) & 0xFF;

              accessor.interpolateRaw_4(pix_x0y0,
                pix_x0y0, ((0x100 - wx) * (0x100 - wy)) >> 8,
                pix_x1y0, ((wx        ) * (0x100 - wy)) >> 8,
                pix_x0y1, ((0x100 - wx) * (wy        )) >> 8,
                pix_x1y1, ((wx        ) * (wy        )) >> 8);
              accessor.normalize(pix_x0y0, pix_x0y0);
              accessor.store(dst, pix_x0y0);

              dst += Accessor::DST_BPP;
              px += xx16x16;
              py += xy16x16;

              if ((uint)px >= (uint)mx16x16) px += rx16x16;
              if ((uint)py >= (uint)my16x16) py += ry16x16;
            } while (--i);

            if (w == 0) break;
            _x += (double)MAX_FIXED_STEP;
          }

          P_FETCH_SPAN8_NEXT()
        P_FETCH_SPAN8_END()
      }
    }

    // ------------------------------------------------------------------------
    // [Loop - Float]
    // ------------------------------------------------------------------------

    else
    {
      double mx = ctx->_d.texture.affine.mx;
      double my = ctx->_d.texture.affine.my;

      double rx = ctx->_d.texture.affine.rx;
      double ry = ctx->_d.texture.affine.ry;

      P_FETCH_SPAN8_BEGIN()
        P_FETCH_SPAN8_SET_CURRENT()
        double _x = (double)x;

        double px = Math::repeat(offx + _x * xx, mx);
        double py = Math::repeat(offy + _x * xy, my);

        do {
          int px0 = (int)px;
          int py0 = (int)py;

          typename Accessor::Pixel pix_x0y0;
          typename Accessor::Pixel pix_x1y0;
          typename Accessor::Pixel pix_x0y1;
          typename Accessor::Pixel pix_x1y1;

          const uint8_t* srcLine0 = srcPixels + (ssize_t)(uint)py0 * srcStride;
          if (++py0 > th) py0 = 0;
          const uint8_t* srcLine1 = srcPixels + (ssize_t)(uint)py0 * srcStride;

          accessor.fetchRaw(pix_x0y0, srcLine0 + (uint)px0 * Accessor::SRC_BPP);
          accessor.fetchRaw(pix_x0y1, srcLine1 + (uint)px0 * Accessor::SRC_BPP);

          if (++px0 > tw) px0 = 0;

          accessor.fetchRaw(pix_x1y0, srcLine0 + (uint)px0 * Accessor::SRC_BPP);
          accessor.fetchRaw(pix_x1y1, srcLine1 + (uint)px0 * Accessor::SRC_BPP);

          uint32_t wx = (uint)Math::fixed24x8FromFloat(px) & 0xFF;
          uint32_t wy = (uint)Math::fixed24x8FromFloat(py) & 0xFF;

          accessor.interpolateRaw_4(pix_x0y0,
            pix_x0y0, ((0x100 - wx) * (0x100 - wy)) >> 8,
            pix_x1y0, ((wx        ) * (0x100 - wy)) >> 8,
            pix_x0y1, ((0x100 - wx) * (wy        )) >> 8,
            pix_x1y1, ((wx        ) * (wy        )) >> 8);
          accessor.normalize(pix_x0y0, pix_x0y0);
          accessor.store(dst, pix_x0y0);

          dst += Accessor::DST_BPP;
          px += xx; if ((px < 0.0) | (px >= mx)) px += rx;
          py += xy; if ((py < 0.0) | (py >= my)) py += ry;
        } while (--w);

        P_FETCH_SPAN8_NEXT()
      P_FETCH_SPAN8_END()
    }

    // ------------------------------------------------------------------------
    // [Advance]
    // ------------------------------------------------------------------------

    fetcher->_d.texture.affine.px += fetcher->_d.texture.affine.dx;
    fetcher->_d.texture.affine.py += fetcher->_d.texture.affine.dy;

    if (fetcher->_d.texture.affine.py < 0.0)
      fetcher->_d.texture.affine.py += ctx->_d.texture.affine.my;
    if (fetcher->_d.texture.affine.py >= ctx->_d.texture.affine.my)
      fetcher->_d.texture.affine.py -= ctx->_d.texture.affine.my;
  }

  // --------------------------------------------------------------------------
  // [Fetch - Affine (Nearest) - Reflect]
  // --------------------------------------------------------------------------

  template<typename Accessor>
  static void FOG_FASTCALL fetch_affine_nearest_reflect(
    RasterPatternFetcher* fetcher, RasterSpan* span, uint8_t* buffer)
  {
    const RasterPattern* ctx = fetcher->getContext();
    Accessor accessor(ctx);

    // ------------------------------------------------------------------------
    // [Prepare]
    // ------------------------------------------------------------------------

    int tw = ctx->_d.texture.base.w;
    int th = ctx->_d.texture.base.h;

    int tw2 = tw * 2 - 1;
    int th2 = th * 2 - 1;

    double xx = ctx->_d.texture.affine.xx;
    double xy = ctx->_d.texture.affine.xy;

    double offx = fetcher->_d.texture.affine.px;
    double offy = fetcher->_d.texture.affine.py;

    const uint8_t* srcPixels = ctx->_d.texture.base.pixels;
    ssize_t srcStride = ctx->_d.texture.base.stride;

    P_FETCH_SPAN8_INIT()

    tw--;
    th--;

    // ------------------------------------------------------------------------
    // [Loop - FixedPoint]
    // ------------------------------------------------------------------------

    if (ctx->_d.texture.affine.safeFixedPoint)
    {
      int xx16x16 = ctx->_d.texture.affine.xx16x16;
      int xy16x16 = ctx->_d.texture.affine.xy16x16;

      int mx16x16 = ctx->_d.texture.affine.mx16x16;
      int my16x16 = ctx->_d.texture.affine.my16x16;

      int rx16x16 = ctx->_d.texture.affine.rx16x16;
      int ry16x16 = ctx->_d.texture.affine.ry16x16;

      if (ctx->_d.texture.affine.xyZero)
      {
        int py0 = (int)(offy);
        if (py0 > th) py0 = th2 - py0;
        FOG_ASSERT(py0 >= 0 && py0 <= th);
        srcPixels += py0 * srcStride;

        P_FETCH_SPAN8_BEGIN()
          P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBORS(Accessor::DST_BPP)
          double _x = (double)x;

          for (;;)
          {
            int i = Math::min<int>(w, MAX_FIXED_STEP);
            int px = Helpers::p_repeat_integer(Math::fixed16x16FromFloat(offx + _x * xx), mx16x16);

            w -= i;

            do {
              int px0 = px >> 16;
              if (px0 > tw) px0 = tw2 - px0;

              typename Accessor::Pixel pix;
              accessor.fetchNorm(pix, srcPixels + px0 * Accessor::SRC_BPP);
              accessor.store(dst, pix);

              dst += Accessor::DST_BPP;
              px += xx16x16;
              if ((uint)px >= (uint)mx16x16) px += rx16x16;
            } while (--i);

            if (w == 0) break;
            _x += (double)MAX_FIXED_STEP;
          }

          P_FETCH_SPAN8_NEXT()
        P_FETCH_SPAN8_END()
      }
      else
      {
        P_FETCH_SPAN8_BEGIN()
          P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBORS(Accessor::DST_BPP)
          double _x = (double)x;

          for (;;)
          {
            int i = Math::min<int>(w, MAX_FIXED_STEP);
            int px = Helpers::p_repeat_integer(Math::fixed16x16FromFloat(offx + _x * xx), mx16x16);
            int py = Helpers::p_repeat_integer(Math::fixed16x16FromFloat(offy + _x * xy), my16x16);

            w -= i;

            do {
              int px0 = px >> 16;
              int py0 = py >> 16;

              if (px0 > tw) px0 = tw2 - px0;
              if (py0 > th) py0 = th2 - py0;

              typename Accessor::Pixel pix;
              accessor.fetchNorm(pix, srcPixels + (ssize_t)py0 * srcStride + px0 * Accessor::SRC_BPP);
              accessor.store(dst, pix);

              dst += Accessor::DST_BPP;
              px += xx16x16;
              py += xy16x16;

              if ((uint)px >= (uint)mx16x16) px += rx16x16;
              if ((uint)py >= (uint)my16x16) py += ry16x16;
            } while (--i);

            if (w == 0) break;
            _x += (double)MAX_FIXED_STEP;
          }

          P_FETCH_SPAN8_NEXT()
        P_FETCH_SPAN8_END()
      }
    }

    // ------------------------------------------------------------------------
    // [Loop - Float]
    // ------------------------------------------------------------------------

    else
    {
      double mx = ctx->_d.texture.affine.mx;
      double my = ctx->_d.texture.affine.my;

      double rx = ctx->_d.texture.affine.rx;
      double ry = ctx->_d.texture.affine.ry;

      P_FETCH_SPAN8_BEGIN()
        P_FETCH_SPAN8_SET_CURRENT()
        double _x = (double)x;

        double px = Math::repeat(offx + _x * xx, mx);
        double py = Math::repeat(offy + _x * xy, my);

        do {
          int px0 = (int)px;
          int py0 = (int)py;

          if (px0 > tw) px0 = tw2 - px0;
          if (py0 > th) py0 = th2 - py0;

          typename Accessor::Pixel pix;
          accessor.fetchNorm(pix, srcPixels + py0 * srcStride + px0 * Accessor::SRC_BPP);
          accessor.store(dst, pix);

          dst += Accessor::DST_BPP;
          px += xx; if ((mx < 0.0) | (px >= mx)) px += rx;
          py += xy; if ((my < 0.0) | (py >= my)) py += ry;
        } while (--w);

        P_FETCH_SPAN8_NEXT()
      P_FETCH_SPAN8_END()
    }

    // ------------------------------------------------------------------------
    // [Advance]
    // ------------------------------------------------------------------------

    fetcher->_d.texture.affine.px += fetcher->_d.texture.affine.dx;
    fetcher->_d.texture.affine.py += fetcher->_d.texture.affine.dy;

    if (fetcher->_d.texture.affine.py < 0.0)
      fetcher->_d.texture.affine.py += ctx->_d.texture.affine.my;
    if (fetcher->_d.texture.affine.py >= ctx->_d.texture.affine.my)
      fetcher->_d.texture.affine.py -= ctx->_d.texture.affine.my;
  }

  // --------------------------------------------------------------------------
  // [Fetch - Affine (Bilinear) - Reflect]
  // --------------------------------------------------------------------------

  template<typename Accessor>
  static void FOG_FASTCALL fetch_affine_bilinear_reflect(
    RasterPatternFetcher* fetcher, RasterSpan* span, uint8_t* buffer)
  {
    const RasterPattern* ctx = fetcher->getContext();
    Accessor accessor(ctx);

    // ------------------------------------------------------------------------
    // [Prepare]
    // ------------------------------------------------------------------------

    int tw = ctx->_d.texture.base.w;
    int th = ctx->_d.texture.base.h;

    int tw2 = tw * 2 - 1;
    int th2 = th * 2 - 1;

    double xx = ctx->_d.texture.affine.xx;
    double xy = ctx->_d.texture.affine.xy;

    double offx = fetcher->_d.texture.affine.px;
    double offy = fetcher->_d.texture.affine.py;

    const uint8_t* srcPixels = ctx->_d.texture.base.pixels;
    ssize_t srcStride = ctx->_d.texture.base.stride;

    P_FETCH_SPAN8_INIT()

    tw--;
    th--;

    // ------------------------------------------------------------------------
    // [Loop - FixedPoint]
    // ------------------------------------------------------------------------

    if (ctx->_d.texture.affine.safeFixedPoint)
    {
      int xx16x16 = ctx->_d.texture.affine.xx16x16;
      int xy16x16 = ctx->_d.texture.affine.xy16x16;

      int mx16x16 = ctx->_d.texture.affine.mx16x16;
      int my16x16 = ctx->_d.texture.affine.my16x16;

      int rx16x16 = ctx->_d.texture.affine.rx16x16;
      int ry16x16 = ctx->_d.texture.affine.ry16x16;

      if (ctx->_d.texture.affine.xyZero)
      {
        int py0 = (int)(offy);
        int py1;
        Helpers::p_reflect_integer(py0, py1, th, th2);

        uint32_t wy = (uint)(Math::fixed24x8FromFloat(offy) & 0xFF);
        uint32_t inv_wy = 0x100 - wy;

        const uint8_t* srcLine0 = srcPixels + py0 * srcStride;
        const uint8_t* srcLine1 = srcPixels + py1 * srcStride;

        P_FETCH_SPAN8_BEGIN()
          P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBORS(Accessor::DST_BPP)
          double _x = (double)x;

          for (;;)
          {
            int i = Math::min<int>(w, MAX_FIXED_STEP);
            int px = Helpers::p_repeat_integer(Math::fixed16x16FromFloat(offx + _x * xx), mx16x16);

            w -= i;

            do {
              int px0 = px >> 16, px1;
              Helpers::p_reflect_integer(px0, px1, tw, tw2);

              typename Accessor::Pixel pix_x0y0;
              typename Accessor::Pixel pix_x1y0;
              typename Accessor::Pixel pix_x0y1;
              typename Accessor::Pixel pix_x1y1;

              accessor.fetchRaw(pix_x0y0, srcLine0 + px0 * Accessor::SRC_BPP);
              accessor.fetchRaw(pix_x1y0, srcLine0 + px1 * Accessor::SRC_BPP);

              accessor.fetchRaw(pix_x0y1, srcLine1 + px0 * Accessor::SRC_BPP);
              accessor.fetchRaw(pix_x1y1, srcLine1 + px1 * Accessor::SRC_BPP);

              uint32_t wx = (uint)(px >> 8) & 0xFF;

              accessor.interpolateRaw_4(pix_x0y0,
                pix_x0y0, ((0x100 - wx) * (inv_wy)) >> 8,
                pix_x1y0, ((wx        ) * (inv_wy)) >> 8,
                pix_x0y1, ((0x100 - wx) * (wy    )) >> 8,
                pix_x1y1, ((wx        ) * (wy    )) >> 8);
              accessor.normalize(pix_x0y0, pix_x0y0);
              accessor.store(dst, pix_x0y0);

              dst += Accessor::DST_BPP;
              px += xx16x16;
              if ((uint)px >= (uint)mx16x16) px += rx16x16;
            } while (--i);

            if (w == 0) break;
            _x += (double)MAX_FIXED_STEP;
          }

          P_FETCH_SPAN8_NEXT()
        P_FETCH_SPAN8_END()
      }
      else
      {
        P_FETCH_SPAN8_BEGIN()
          P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBORS(Accessor::DST_BPP)
          double _x = (double)x;

          for (;;)
          {
            int i = Math::min<int>(w, MAX_FIXED_STEP);
            int px = Helpers::p_repeat_integer(Math::fixed16x16FromFloat(offx + _x * xx), mx16x16);
            int py = Helpers::p_repeat_integer(Math::fixed16x16FromFloat(offy + _x * xy), my16x16);

            w -= i;

            do {
              int px0 = px >> 16, px1;
              int py0 = py >> 16, py1;

              Helpers::p_reflect_integer(px0, px1, tw, tw2);
              Helpers::p_reflect_integer(py0, py1, th, th2);

              typename Accessor::Pixel pix_x0y0;
              typename Accessor::Pixel pix_x1y0;
              typename Accessor::Pixel pix_x0y1;
              typename Accessor::Pixel pix_x1y1;

              const uint8_t* srcLine0 = srcPixels + (uint)py0 * srcStride;
              const uint8_t* srcLine1 = srcPixels + (uint)py1 * srcStride;

              accessor.fetchRaw(pix_x0y0, srcLine0 + (uint)px0 * Accessor::SRC_BPP);
              accessor.fetchRaw(pix_x1y0, srcLine0 + (uint)px1 * Accessor::SRC_BPP);
              accessor.fetchRaw(pix_x0y1, srcLine1 + (uint)px0 * Accessor::SRC_BPP);
              accessor.fetchRaw(pix_x1y1, srcLine1 + (uint)px1 * Accessor::SRC_BPP);

              uint32_t wx = (uint)(px >> 8) & 0xFF;
              uint32_t wy = (uint)(py >> 8) & 0xFF;

              accessor.interpolateRaw_4(pix_x0y0,
                pix_x0y0, ((0x100 - wx) * (0x100 - wy)) >> 8,
                pix_x1y0, ((wx        ) * (0x100 - wy)) >> 8,
                pix_x0y1, ((0x100 - wx) * (wy        )) >> 8,
                pix_x1y1, ((wx        ) * (wy        )) >> 8);
              accessor.normalize(pix_x0y0, pix_x0y0);
              accessor.store(dst, pix_x0y0);

              dst += Accessor::DST_BPP;
              px += xx16x16;
              py += xy16x16;

              if ((uint)px >= (uint)mx16x16) px += rx16x16;
              if ((uint)py >= (uint)my16x16) py += ry16x16;
            } while (--i);

            if (w == 0) break;
            _x += (double)MAX_FIXED_STEP;
          }

          P_FETCH_SPAN8_NEXT()
        P_FETCH_SPAN8_END()
      }
    }

    // ------------------------------------------------------------------------
    // [Loop - Float]
    // ------------------------------------------------------------------------

    else
    {
      double mx = ctx->_d.texture.affine.mx;
      double my = ctx->_d.texture.affine.my;

      double rx = ctx->_d.texture.affine.rx;
      double ry = ctx->_d.texture.affine.ry;

      P_FETCH_SPAN8_BEGIN()
        P_FETCH_SPAN8_SET_CURRENT()
        double _x = (double)x;

        double px = Math::repeat(offx + _x * xx, mx);
        double py = Math::repeat(offy + _x * xy, my);

        do {
          int px0 = (int)px, px1;
          int py0 = (int)py, py1;

          Helpers::p_reflect_integer(px0, px1, tw, tw2);
          Helpers::p_reflect_integer(py0, py1, th, th2);

          typename Accessor::Pixel pix_x0y0;
          typename Accessor::Pixel pix_x1y0;
          typename Accessor::Pixel pix_x0y1;
          typename Accessor::Pixel pix_x1y1;

          const uint8_t* srcLine0 = srcPixels + (ssize_t)(uint)py0 * srcStride;
          const uint8_t* srcLine1 = srcPixels + (ssize_t)(uint)py1 * srcStride;

          accessor.fetchRaw(pix_x0y0, srcLine0 + (uint)px0 * Accessor::SRC_BPP);
          accessor.fetchRaw(pix_x1y0, srcLine0 + (uint)px1 * Accessor::SRC_BPP);
          accessor.fetchRaw(pix_x0y1, srcLine1 + (uint)px0 * Accessor::SRC_BPP);
          accessor.fetchRaw(pix_x1y1, srcLine1 + (uint)px1 * Accessor::SRC_BPP);

          uint32_t wx = (uint)Math::fixed24x8FromFloat(px) & 0xFF;
          uint32_t wy = (uint)Math::fixed24x8FromFloat(py) & 0xFF;

          accessor.interpolateRaw_4(pix_x0y0,
            pix_x0y0, ((0x100 - wx) * (0x100 - wy)) >> 8,
            pix_x1y0, ((wx        ) * (0x100 - wy)) >> 8,
            pix_x0y1, ((0x100 - wx) * (wy        )) >> 8,
            pix_x1y1, ((wx        ) * (wy        )) >> 8);
          accessor.normalize(pix_x0y0, pix_x0y0);
          accessor.store(dst, pix_x0y0);

          dst += Accessor::DST_BPP;
          px += xx; if ((px < 0.0) | (px >= mx)) px += rx;
          py += xy; if ((py < 0.0) | (py >= my)) py += ry;
        } while (--w);

        P_FETCH_SPAN8_NEXT()
      P_FETCH_SPAN8_END()
    }

    // ------------------------------------------------------------------------
    // [Advance]
    // ------------------------------------------------------------------------

    fetcher->_d.texture.affine.px += fetcher->_d.texture.affine.dx;
    fetcher->_d.texture.affine.py += fetcher->_d.texture.affine.dy;

    if (fetcher->_d.texture.affine.py < 0.0)
      fetcher->_d.texture.affine.py += ctx->_d.texture.affine.my;
    if (fetcher->_d.texture.affine.py >= ctx->_d.texture.affine.my)
      fetcher->_d.texture.affine.py -= ctx->_d.texture.affine.my;
  }

  // --------------------------------------------------------------------------
  // [Fetch - Affine (Nearest) - Clamp]
  // --------------------------------------------------------------------------

  template<typename Accessor>
  static void FOG_FASTCALL fetch_affine_nearest_clamp(
    RasterPatternFetcher* fetcher, RasterSpan* span, uint8_t* buffer)
  {
    const RasterPattern* ctx = fetcher->getContext();
    Accessor accessor(ctx);

    // ------------------------------------------------------------------------
    // [Prepare]
    // ------------------------------------------------------------------------

    int tw = ctx->_d.texture.base.w;
    int th = ctx->_d.texture.base.h;

    double xx = ctx->_d.texture.affine.xx;
    double xy = ctx->_d.texture.affine.xy;

    double offx = fetcher->_d.texture.affine.px;
    double offy = fetcher->_d.texture.affine.py;

    const uint8_t* srcPixels = ctx->_d.texture.base.pixels;
    ssize_t srcStride = ctx->_d.texture.base.stride;

    typename Accessor::Pixel clamp;
    accessor.fetchSolid(clamp, ctx->_d.texture.base.clamp);

    P_FETCH_SPAN8_INIT()

    tw--;
    th--;

    // ------------------------------------------------------------------------
    // [Loop - FixedPoint]
    // ------------------------------------------------------------------------

    if (ctx->_d.texture.affine.safeFixedPoint)
    {
      int xx16x16 = ctx->_d.texture.affine.xx16x16;
      int xy16x16 = ctx->_d.texture.affine.xy16x16;

      if (ctx->_d.texture.affine.xyZero)
      {
        int py0 = (int)offy;
        if ((uint)py0 > (uint)th) goto _FetchSolid;

        srcPixels += py0 * srcStride;

        P_FETCH_SPAN8_BEGIN()
          P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBORS(Accessor::DST_BPP)
          double _x = (double)x;

          for (;;)
          {
            int i = Math::min<int>(w, MAX_FIXED_STEP);
            int px = Math::fixed16x16FromFloat(offx + _x * xx);

            w -= i;

            do {
              int px0 = px >> 16;

              if ((uint)px0 <= (uint)tw)
              {
                typename Accessor::Pixel pix;
                accessor.fetchNorm(pix, srcPixels + px0 * Accessor::SRC_BPP);
                accessor.store(dst, pix);
              }
              else
              {
                accessor.store(dst, clamp);
              }

              dst += Accessor::DST_BPP;
              px += xx16x16;
            } while (--i);

            if (w == 0) break;
            _x += (double)MAX_FIXED_STEP;
          }

          P_FETCH_SPAN8_NEXT()
        P_FETCH_SPAN8_END()
      }
      else
      {
        P_FETCH_SPAN8_BEGIN()
          P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBORS(Accessor::DST_BPP)
          double _x = (double)x;

          for (;;)
          {
            int i = Math::min<int>(w, MAX_FIXED_STEP);
            int px = Math::fixed16x16FromFloat(offx + _x * xx);
            int py = Math::fixed16x16FromFloat(offy + _x * xy);

            w -= i;

            do {
              int px0 = px >> 16;
              int py0 = py >> 16;

              if (((uint)px0 <= (uint)tw) & ((uint)py0 <= (uint)th))
              {
                typename Accessor::Pixel pix;
                accessor.fetchNorm(pix, srcPixels + (ssize_t)py0 * srcStride + px0 * Accessor::SRC_BPP);
                accessor.store(dst, pix);
              }
              else
              {
                accessor.store(dst, clamp);
              }

              dst += Accessor::DST_BPP;
              px += xx16x16;
              py += xy16x16;
            } while (--i);

            if (w == 0) break;
            _x += (double)MAX_FIXED_STEP;
          }

          P_FETCH_SPAN8_NEXT()
        P_FETCH_SPAN8_END()
      }
    }

    // ------------------------------------------------------------------------
    // [Loop - Float]
    // ------------------------------------------------------------------------

    else
    {
      P_FETCH_SPAN8_BEGIN()
        P_FETCH_SPAN8_SET_CURRENT()
        double _x = (double)x;

        double px = offx + _x * xx;
        double py = offy + _x * xy;

        do {
          int px0 = (int)px;
          int py0 = (int)py;

          if (((uint)px0 <= (uint)tw) & ((uint)py0 <= (uint)th))
          {
            typename Accessor::Pixel pix;
            accessor.fetchNorm(pix, srcPixels + (ssize_t)py0 * srcStride + px0 * Accessor::SRC_BPP);
            accessor.store(dst, pix);
          }
          else
          {
            accessor.store(dst, clamp);
          }

          dst += Accessor::DST_BPP;
          px += xx;
          py += xy;
        } while (--w);

        P_FETCH_SPAN8_NEXT()
      P_FETCH_SPAN8_END()
    }
    goto _FetchEnd;

    // ------------------------------------------------------------------------
    // [Solid]
    // ------------------------------------------------------------------------

_FetchSolid:
    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()
      dst = accessor.fill(dst, clamp, w);
      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()

    // ------------------------------------------------------------------------
    // [Advance]
    // ------------------------------------------------------------------------

_FetchEnd:
    fetcher->_d.texture.affine.px += fetcher->_d.texture.affine.dx;
    fetcher->_d.texture.affine.py += fetcher->_d.texture.affine.dy;
  }

  // --------------------------------------------------------------------------
  // [Fetch - Affine (Bilinear) - Clamp]
  // --------------------------------------------------------------------------

  template<typename Accessor>
  static void FOG_FASTCALL fetch_affine_bilinear_clamp(
    RasterPatternFetcher* fetcher, RasterSpan* span, uint8_t* buffer)
  {
    const RasterPattern* ctx = fetcher->getContext();
    Accessor accessor(ctx);

    // ------------------------------------------------------------------------
    // [Prepare]
    // ------------------------------------------------------------------------

    int tw = ctx->_d.texture.base.w;
    int th = ctx->_d.texture.base.h;

    double xx = ctx->_d.texture.affine.xx;
    double xy = ctx->_d.texture.affine.xy;

    double offx = fetcher->_d.texture.affine.px;
    double offy = fetcher->_d.texture.affine.py;

    const uint8_t* srcPixels = ctx->_d.texture.base.pixels;
    ssize_t srcStride = ctx->_d.texture.base.stride;

    typename Accessor::Pixel clamp;
    accessor.fetchSolid(clamp, ctx->_d.texture.base.clamp);

    P_FETCH_SPAN8_INIT()

    tw--;
    th--;

    // ------------------------------------------------------------------------
    // [Loop - FixedPoint]
    // ------------------------------------------------------------------------

    if (ctx->_d.texture.affine.safeFixedPoint)
    {
      int xx16x16 = ctx->_d.texture.affine.xx16x16;
      int xy16x16 = ctx->_d.texture.affine.xy16x16;

      if (ctx->_d.texture.affine.xyZero && offy >= 0.0 && offy < (double)th)
      {
        int py0 = (int)offy;
        FOG_ASSERT(py0 >= 0 && py0 < th);

        uint32_t wy = (uint)(Math::fixed24x8FromFloat(offy) & 0xFF);
        uint32_t inv_wy = 0x100 - wy;

        const uint8_t* srcLine0 = srcPixels + py0 * srcStride;
        const uint8_t* srcLine1 = srcLine0 + srcStride;

        P_FETCH_SPAN8_BEGIN()
          P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBORS(Accessor::DST_BPP)
          double _x = (double)x;

          for (;;)
          {
            int i = Math::min<int>(w, MAX_FIXED_STEP);
            int px = Math::fixed16x16FromFloat(offx + _x * xx);

            w -= i;

            do {
              int px0 = px >> 16;
              uint32_t wx = (uint)(px >> 8) & 0xFF;

              typename Accessor::Pixel pix_x0y0;
              typename Accessor::Pixel pix_x1y0;
              typename Accessor::Pixel pix_x0y1;
              typename Accessor::Pixel pix_x1y1;

              if (FOG_LIKELY((uint)px0 < (uint)tw))
              {
                accessor.fetchRaw(pix_x0y0, srcLine0 + (uint)px0 * Accessor::SRC_BPP);
                accessor.fetchRaw(pix_x1y0, srcLine0 + (uint)px0 * Accessor::SRC_BPP + Accessor::SRC_BPP);
                accessor.fetchRaw(pix_x0y1, srcLine1 + (uint)px0 * Accessor::SRC_BPP);
                accessor.fetchRaw(pix_x1y1, srcLine1 + (uint)px0 * Accessor::SRC_BPP + Accessor::SRC_BPP);

                accessor.interpolateRaw_4(pix_x0y0,
                  pix_x0y0, ((0x100 - wx) * (inv_wy)) >> 8,
                  pix_x1y0, ((wx        ) * (inv_wy)) >> 8,
                  pix_x0y1, ((0x100 - wx) * (wy    )) >> 8,
                  pix_x1y1, ((wx        ) * (wy    )) >> 8);
                accessor.normalize(pix_x0y0, pix_x0y0);
                accessor.store(dst, pix_x0y0);
              }
              else
              {
                if (px0 == -1)
                {
                  pix_x0y0 = clamp;
                  pix_x0y1 = clamp;

                  accessor.fetchNorm(pix_x1y0, srcLine0);
                  accessor.fetchNorm(pix_x1y1, srcLine1);
                  goto _FixedScaleInterpolate;
                }
                else if (px0 == tw)
                {
                  accessor.fetchNorm(pix_x0y0, srcLine0 + tw * Accessor::SRC_BPP);
                  accessor.fetchNorm(pix_x0y1, srcLine1 + tw * Accessor::SRC_BPP);

                  pix_x1y0 = clamp;
                  pix_x1y1 = clamp;

_FixedScaleInterpolate:
                  accessor.interpolateNorm_4(pix_x0y0,
                    pix_x0y0, ((0x100 - wx) * (inv_wy)) >> 8,
                    pix_x1y0, ((wx        ) * (inv_wy)) >> 8,
                    pix_x0y1, ((0x100 - wx) * (wy    )) >> 8,
                    pix_x1y1, ((wx        ) * (wy    )) >> 8);
                  accessor.store(dst, pix_x0y0);
                }
                else
                {
                  accessor.store(dst, clamp);
                }
              }

              dst += Accessor::DST_BPP;
              px += xx16x16;
            } while (--i);

            if (w == 0) break;
            _x += (double)MAX_FIXED_STEP;
          }

          P_FETCH_SPAN8_NEXT()
        P_FETCH_SPAN8_END()
      }
      else
      {
        P_FETCH_SPAN8_BEGIN()
          P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBORS(Accessor::DST_BPP)
          double _x = (double)x;

          for (;;)
          {
            int i = Math::min<int>(w, MAX_FIXED_STEP);
            int px = Math::fixed16x16FromFloat(offx + _x * xx);
            int py = Math::fixed16x16FromFloat(offy + _x * xy);

            w -= i;

            do {
              int px0 = px >> 16;
              int py0 = py >> 16;

              typename Accessor::Pixel pix_x0y0;
              typename Accessor::Pixel pix_x1y0;
              typename Accessor::Pixel pix_x0y1;
              typename Accessor::Pixel pix_x1y1;

              uint32_t wx = (uint)(px >> 8) & 0xFF;
              uint32_t wy = (uint)(py >> 8) & 0xFF;

              if (FOG_LIKELY(((uint)px0 < (uint)tw) & ((uint)py0 < (uint)th)))
              {
                const uint8_t* srcLine = srcPixels + py0 * srcStride + (uint)px0 * Accessor::SRC_BPP;

                accessor.fetchRaw(pix_x0y0, srcLine);
                accessor.fetchRaw(pix_x1y0, srcLine + Accessor::SRC_BPP);
                srcLine += srcStride;
                accessor.fetchRaw(pix_x0y1, srcLine);
                accessor.fetchRaw(pix_x1y1, srcLine + Accessor::SRC_BPP);

                accessor.interpolateRaw_4(pix_x0y0,
                  pix_x0y0, ((0x100 - wx) * (0x100 - wy)) >> 8,
                  pix_x1y0, ((wx        ) * (0x100 - wy)) >> 8,
                  pix_x0y1, ((0x100 - wx) * (wy        )) >> 8,
                  pix_x1y1, ((wx        ) * (wy        )) >> 8);
                accessor.normalize(pix_x0y0, pix_x0y0);
              }
              else
              {
                int px1 = px0 + 1;
                int py1 = py0 + 1;

                pix_x0y0 = clamp;

                if ((uint)px1 <= (uint)tw+1 && (uint)py1 <= (uint)th+1)
                {
                  const uint8_t* srcLine0 = srcPixels + (uint)py0 * srcStride;
                  const uint8_t* srcLine1 = srcPixels + (uint)py1 * srcStride;

                  pix_x1y0 = clamp;
                  pix_x0y1 = clamp;
                  pix_x1y1 = clamp;

                  if ((uint)px0 <= (uint)tw && (uint)py0 <= (uint)th) accessor.fetchNorm(pix_x0y0, srcLine0 + px0 * Accessor::SRC_BPP);
                  if ((uint)px1 <= (uint)tw && (uint)py0 <= (uint)th) accessor.fetchNorm(pix_x1y0, srcLine0 + px1 * Accessor::SRC_BPP);
                  if ((uint)px0 <= (uint)tw && (uint)py1 <= (uint)th) accessor.fetchNorm(pix_x0y1, srcLine1 + px0 * Accessor::SRC_BPP);
                  if ((uint)px1 <= (uint)tw && (uint)py1 <= (uint)th) accessor.fetchNorm(pix_x1y1, srcLine1 + px1 * Accessor::SRC_BPP);

                  accessor.interpolateNorm_4(pix_x0y0,
                    pix_x0y0, ((0x100 - wx) * (0x100 - wy)) >> 8,
                    pix_x1y0, ((wx        ) * (0x100 - wy)) >> 8,
                    pix_x0y1, ((0x100 - wx) * (wy        )) >> 8,
                    pix_x1y1, ((wx        ) * (wy        )) >> 8);
                }
              }

              accessor.store(dst, pix_x0y0);
              dst += Accessor::DST_BPP;
              px += xx16x16;
              py += xy16x16;
            } while (--i);

            if (w == 0) break;
            _x += (double)MAX_FIXED_STEP;
          }

          P_FETCH_SPAN8_NEXT()
        P_FETCH_SPAN8_END()
      }
    }

    // ------------------------------------------------------------------------
    // [Loop - Float]
    // ------------------------------------------------------------------------

    else
    {
      P_FETCH_SPAN8_BEGIN()
        P_FETCH_SPAN8_SET_CURRENT()
        double _x = (double)x;

        double px = offx + _x * xx;
        double py = offy + _x * xy;

        do {
          int px0 = (int)px;
          int py0 = (int)py;

          typename Accessor::Pixel pix_x0y0;
          typename Accessor::Pixel pix_x1y0;
          typename Accessor::Pixel pix_x0y1;
          typename Accessor::Pixel pix_x1y1;

          if (FOG_LIKELY(((uint)px0 < (uint)tw) & ((uint)py0 < (uint)th)))
          {
            const uint8_t* srcLine = srcPixels + py0 * srcStride + (uint)px0 * Accessor::SRC_BPP;

            accessor.fetchRaw(pix_x0y0, srcLine);
            accessor.fetchRaw(pix_x1y0, srcLine + Accessor::SRC_BPP);
            srcLine += srcStride;
            accessor.fetchRaw(pix_x0y1, srcLine);
            accessor.fetchRaw(pix_x1y1, srcLine + Accessor::SRC_BPP);

            uint32_t wx = Math::fixed24x8FromFloat(px) & 0xFF;
            uint32_t wy = Math::fixed24x8FromFloat(py) & 0xFF;

            accessor.interpolateRaw_4(pix_x0y0,
              pix_x0y0, ((0x100 - wx) * (0x100 - wy)) >> 8,
              pix_x1y0, ((wx        ) * (0x100 - wy)) >> 8,
              pix_x0y1, ((0x100 - wx) * (wy        )) >> 8,
              pix_x1y1, ((wx        ) * (wy        )) >> 8);
            accessor.normalize(pix_x0y0, pix_x0y0);
          }
          else
          {
            int px1 = px0 + 1;
            int py1 = py0 + 1;

            pix_x0y0 = clamp;

            if ((uint)px1 <= (uint)tw+1 && (uint)py1 <= (uint)th+1)
            {
              const uint8_t* srcLine0 = srcPixels + (uint)py0 * srcStride;
              const uint8_t* srcLine1 = srcPixels + (uint)py1 * srcStride;

              pix_x1y0 = clamp;
              pix_x0y1 = clamp;
              pix_x1y1 = clamp;

              uint32_t wx = Math::fixed24x8FromFloat(px) & 0xFF;
              uint32_t wy = Math::fixed24x8FromFloat(py) & 0xFF;

              if ((uint)px0 <= (uint)tw && (uint)py0 <= (uint)th) accessor.fetchNorm(pix_x0y0, srcLine0 + px0 * Accessor::SRC_BPP);
              if ((uint)px1 <= (uint)tw && (uint)py0 <= (uint)th) accessor.fetchNorm(pix_x1y0, srcLine0 + px1 * Accessor::SRC_BPP);
              if ((uint)px0 <= (uint)tw && (uint)py1 <= (uint)th) accessor.fetchNorm(pix_x0y1, srcLine1 + px0 * Accessor::SRC_BPP);
              if ((uint)px1 <= (uint)tw && (uint)py1 <= (uint)th) accessor.fetchNorm(pix_x1y1, srcLine1 + px1 * Accessor::SRC_BPP);

              accessor.interpolateNorm_4(pix_x0y0,
                pix_x0y0, ((0x100 - wx) * (0x100 - wy)) >> 8,
                pix_x1y0, ((wx        ) * (0x100 - wy)) >> 8,
                pix_x0y1, ((0x100 - wx) * (wy        )) >> 8,
                pix_x1y1, ((wx        ) * (wy        )) >> 8);
            }
          }

          accessor.store(dst, pix_x0y0);
          dst += Accessor::DST_BPP;
          px += xx;
          py += xy;
        } while (--w);
        P_FETCH_SPAN8_NEXT()
      P_FETCH_SPAN8_END()
    }

    goto _FetchEnd;

    // ------------------------------------------------------------------------
    // [Solid]
    // ------------------------------------------------------------------------

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()
      dst = accessor.fill(dst, clamp, w);
      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()

    // ------------------------------------------------------------------------
    // [Advance]
    // ------------------------------------------------------------------------

_FetchEnd:
    fetcher->_d.texture.affine.px += fetcher->_d.texture.affine.dx;
    fetcher->_d.texture.affine.py += fetcher->_d.texture.affine.dy;
  }
};

} // RasterOps_C namespace
} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTEROPS_C_TEXTUREAFFINE_P_H
