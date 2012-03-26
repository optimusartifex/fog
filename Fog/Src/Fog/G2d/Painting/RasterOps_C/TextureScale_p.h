// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTEROPS_C_TEXTURESCALE_P_H
#define _FOG_G2D_PAINTING_RASTEROPS_C_TEXTURESCALE_P_H

// [Dependencies]
#include <Fog/G2d/Painting/RasterOps_C/TextureBase_p.h>

namespace Fog {
namespace RasterOps_C {

} // RasterOps_C namespace
} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTEROPS_C_TEXTURESCALE_P_H







#if 0

namespace Fog {
namespace Render {

// ============================================================================
// [Fog::RasterOps_C - Scale]
// ============================================================================

#define A_VAL(p) ((uint8_t*)(p))[PIXEL_ARGB32_POS_A]
#define R_VAL(p) ((uint8_t*)(p))[PIXEL_ARGB32_POS_R]
#define G_VAL(p) ((uint8_t*)(p))[PIXEL_ARGB32_POS_G]
#define B_VAL(p) ((uint8_t*)(p))[PIXEL_ARGB32_POS_B]

#define ARGB_COMPOSE(r, g, b, a) \
  ((uint32_t)(a) << 24) | ((uint32_t)(r) << 16) | ((uint32_t)(g) << 8) | (uint32_t)(b)

#define XAP (xapoints[x])
#define YAP (yapoints[y])
#define INV_XAP (256 - xapoints[x])
#define INV_YAP (256 - yapoints[y])

//! @internal
struct FOG_NO_EXPORT PatternScaleC
{
  // --------------------------------------------------------------------------
  // [Scale]
  // --------------------------------------------------------------------------

  static FOG_NO_INLINE uint32_t** calcYPoints(uint32_t* src, int sw, int sh, int dh)
  {
    uint32_t** p;
    int i;
    int j = 0;
    int val;
    int inc;
    int rv = 0;

    if (dh < 0)
    {
      dh = -dh;
      rv = 1;
    }

    p = reinterpret_cast<uint32_t**>(MemMgr::alloc((dh + 1) * sizeof(uint32_t*)));
    if (p == NULL) return p;

    val = (dh >= sh) ? ((0x8000 * sh / dh) + 0x8000) : 0;
    inc = (sh << 16) / dh;

    for (i = 0; i < dh; i++)
    {
      p[j++] = src + Math::max(val >> 16, 0) * sw;
      val += inc;
    }

    if (rv)
    {
      for (i = dh / 2; --i >= 0;)
      {
        uint32_t* tmp = p[i];

        p[i] = p[dh - i - 1];
        p[dh - i - 1] = tmp;
      }
    }
    return p;
  }

  static FOG_NO_INLINE int* calcXPoints(int sw, int dw)
  {
    int* p;
    int i;
    int j = 0;
    int val;
    int inc;
    int rv = 0;

    if (dw < 0)
    {
      dw = -dw;
      rv = 1;
    }

    p = reinterpret_cast<int*>(MemMgr::alloc((dw + 1) * sizeof(int)));
    if (p == NULL) return p;

    val = (dw >= sw) ? ((0x8000 * sw / dw) + 0x8000) : 0;
    inc = (sw << 16) / dw;

    for (i = 0; i < dw; i++)
    {
      p[j++] = Math::max(val >> 16, 0);
      val += inc;
    }

    if (rv)
    {
      for (i = dw / 2; --i >= 0;)
      {
        int tmp = p[i];

        p[i] = p[dw - i - 1];
        p[dw - i - 1] = tmp;
      }
    }
    return p;
  }

  static FOG_NO_INLINE int* calcAPoints(int s, int d, int up)
  {
    int* p;
    int i;
    int j = 0;
    int rv = 0;
    int val;
    int inc;

    if (d < 0)
    {
      rv = 1;
      d = -d;
    }

    p = reinterpret_cast<int*>(MemMgr::alloc(d * sizeof(int)));
    if (p == NULL) return p;

    val = 0;
    inc = (s << 16) / d;

    if (up)
    {
      // Scaling up.
      val = 0x8000 * s / d - 0x8000;

      for (i = 0; i < d; i++)
      {
        int pos = val >> 16;
        if ((uint)pos >= (uint)(s - 1))
          p[j] = 0;
        else
          p[j] = (val >> 8) - ((val >> 8) & 0xFFFFFF00);
        val += inc;
        j++;
      }
    }
    else
    {
      // Scaling down.
      int ap;
      int Cp = ((d << 14) / s) + 1;

      for (i = 0; i < d; i++)
      {
        ap = ((0x100 - ((val >> 8) & 0xFF)) * Cp) >> 8;
        p[j] = ap | (Cp << 16);
        j++;
        val += inc;
      }
    }

    if (rv)
    {
      for (i = d / 2; --i >= 0;)
      {
        int tmp = p[i];

        p[i] = p[d - i - 1];
        p[d - i - 1] = tmp;
      }
    }

    return p;
  }

  static err_t FOG_FASTCALL init(RasterPattern* ctx, const Image& image, int dw, int dh, uint32_t interpolationType)
  {
    MemOps::zero(&ctx->scale, sizeof(ctx->scale));

    int sw = image.getWidth();
    int sh = image.getHeight();

    ctx->scale.sw = sw;
    ctx->scale.sh = sh;
    ctx->scale.dw = Math::abs(dw);
    ctx->scale.dh = Math::abs(dh);
    ctx->scale.xup_yup = (ctx->scale.dw >= sw) | ((ctx->scale.dh >= sh) << 1);

    ctx->scale.xpoints = calcXPoints(sw, dw);
    if (!ctx->scale.xpoints) goto fail;

    ctx->scale.ypoints = calcYPoints((uint32_t*)image.getData(), sw, sh, dh);
    if (!ctx->scale.ypoints) goto fail;

    if (interpolationType == IMAGE_INTERPOLATION_SMOOTH)
    {
      ctx->scale.xapoints = calcAPoints(sw, dw, (ctx->scale.xup_yup & 0x1) == 0x1);
      if (!ctx->scale.xapoints) goto fail;

      ctx->scale.yapoints = calcAPoints(sh, dh, (ctx->scale.xup_yup & 0x2) == 0x2);
      if (!ctx->scale.yapoints) goto fail;

      ctx->fetch = fetch_argb32_aa;
    }
    else
    {
      ctx->fetch = fetch_argb32_nn;
    }

    ctx->destroy = destroy;

    return ERR_OK;

fail:
    destroy(ctx);
    return ERR_RT_OUT_OF_MEMORY;
  }

  static void FOG_FASTCALL destroy(RasterPattern* ctx)
  {
    MemMgr::free(ctx->scale.xpoints);
    MemMgr::free(ctx->scale.ypoints);
    MemMgr::free(ctx->scale.xapoints);
    MemMgr::free(ctx->scale.yapoints);
  }

  // Scale by pixel sampling only.
  static void FOG_FASTCALL fetch_argb32_nn(
    const RasterPattern* ctx, RasterSpan* span, uint8_t* buffer, int y, uint32_t mode)
  {
    P_FETCH_SPAN8_INIT()

    int dw = ctx->scale.dw;
    int dh = ctx->scale.dh;

    int i;

    // Bound Y.
    if ((uint)y >= (uint)dh) { y %= dh; if (y < 0) y += dh; }

    // Calculate the source line we'll scan from.
    const uint32_t** ypoints = (const uint32_t**)ctx->scale.ypoints;
    const uint32_t* sptr = ypoints[y];

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBORS(4)

      // Bound X.
      if ((uint)x >= (uint)dw) { x %= dw; if (x < 0) x += dw; }

      // Go through the scanline and copy across.
      i = Math::min(dw - x, w);
      const int* xpoints = ctx->scale.xpoints + x;

      for (;;)
      {
        w -= i;

        do {
          ((uint32_t*)dst)[0] = sptr[*xpoints++];
          dst += 4;
        } while(--i);
        if (w == 0) break;

        i = Math::min(dw, w);
        xpoints = ctx->scale.xpoints;
      }

      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()
  }

  // Scale by area sampling.
  static void FOG_FASTCALL fetch_argb32_aa(
    const RasterPattern* ctx, RasterSpan* span, uint8_t* buffer, int y, uint32_t mode)
  {
    P_FETCH_SPAN8_INIT()

    int dw = ctx->scale.dw;
    int dh = ctx->scale.dh;

    // Bound Y.
    if ((uint)y >= (uint)dh) { y %= dh; if (y < 0) y += dh; }

    const uint32_t** ypoints = (const uint32_t**)ctx->scale.ypoints;
    const int* xpoints = ctx->scale.xpoints;

    const int* xapoints = ctx->scale.xapoints;
    const int* yapoints = ctx->scale.yapoints;

    const uint32_t* sptr;
    ssize_t sow = ctx->scale.sw;

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBORS(4)

      // Bounds X.
      if ((uint)x >= (uint)dw) { x %= dw; if (x < 0) x += dw; }

      // Go through the scanline and copy across.
      int i = Math::min(dw - x, w);
      int end = x + i;

      for (;;)
      {
        w -= i;

        if (ctx->scale.xup_yup == 3)
        {
          // Scaling up.

          // Calculate the source line we'll scan from.
          sptr = ypoints[y];

          // BUG: Out of bounds!
          if (YAP > 0 && y < ctx->scale.dh-1)
          {
            for (; x < end; x++)
            {
              int r, g, b, a;
              int rr, gg, bb, aa;
              const uint32_t* pix;

              if (XAP > 0)
              {
                pix = ypoints[y] + xpoints[x];
                r = R_VAL(pix) * INV_XAP;
                g = G_VAL(pix) * INV_XAP;
                b = B_VAL(pix) * INV_XAP;
                a = A_VAL(pix) * INV_XAP;

                pix++;
                r += R_VAL(pix) * XAP;
                g += G_VAL(pix) * XAP;
                b += B_VAL(pix) * XAP;
                a += A_VAL(pix) * XAP;

                pix += sow;
                rr = R_VAL(pix) * XAP;
                gg = G_VAL(pix) * XAP;
                bb = B_VAL(pix) * XAP;
                aa = A_VAL(pix) * XAP;

                pix--;
                rr += R_VAL(pix) * INV_XAP;
                gg += G_VAL(pix) * INV_XAP;
                bb += B_VAL(pix) * INV_XAP;
                aa += A_VAL(pix) * INV_XAP;

                r = ((rr * YAP) + (r * INV_YAP)) >> 16;
                g = ((gg * YAP) + (g * INV_YAP)) >> 16;
                b = ((bb * YAP) + (b * INV_YAP)) >> 16;
                a = ((aa * YAP) + (a * INV_YAP)) >> 16;

                ((uint32_t*)dst)[0] = ARGB_COMPOSE(r, g, b, a);
                dst += 4;
              }
              else
              {
                pix = ypoints[y] + xpoints[x];
                r = R_VAL(pix) * INV_YAP;
                g = G_VAL(pix) * INV_YAP;
                b = B_VAL(pix) * INV_YAP;
                a = A_VAL(pix) * INV_YAP;

                pix += sow;
                r += R_VAL(pix) * YAP;
                g += G_VAL(pix) * YAP;
                b += B_VAL(pix) * YAP;
                a += A_VAL(pix) * YAP;

                r >>= 8;
                g >>= 8;
                b >>= 8;
                a >>= 8;

                ((uint32_t*)dst)[0] = ARGB_COMPOSE(r, g, b, a);
                dst += 4;
              }
            }
          }
          else
          {
            for (; x < end; x++)
            {
              int r, g, b, a;
              const uint32_t* pix;

              if (XAP > 0)
              {
                pix = ypoints[y] + xpoints[x];
                r = R_VAL(pix) * INV_XAP;
                g = G_VAL(pix) * INV_XAP;
                b = B_VAL(pix) * INV_XAP;
                a = A_VAL(pix) * INV_XAP;

                pix++;
                r += R_VAL(pix) * XAP;
                g += G_VAL(pix) * XAP;
                b += B_VAL(pix) * XAP;
                a += A_VAL(pix) * XAP;

                r >>= 8;
                g >>= 8;
                b >>= 8;
                a >>= 8;

                ((uint32_t*)dst)[0] = ARGB_COMPOSE(r, g, b, a);
                dst += 4;
              }
              else
              {
                ((uint32_t*)dst)[0] = sptr[xpoints[x]];
                dst += 4;
              }
            }
          }
        }
        else if (ctx->scale.xup_yup == 1)
        {
          // Scaling down vertically.
          int Cy, j;
          const uint32_t* pix;
          int r, g, b, a, rr, gg, bb, aa;
          int yap;

          Cy = YAP >> 16;
          yap = YAP & 0xffff;

          for (; x < end; x++)
          {
            pix = ypoints[y] + xpoints[x];
            r = (R_VAL(pix) * yap) >> 10;
            g = (G_VAL(pix) * yap) >> 10;
            b = (B_VAL(pix) * yap) >> 10;
            a = (A_VAL(pix) * yap) >> 10;

            for (j = (1 << 14) - yap; j > Cy; j -= Cy)
            {
              pix += sow;
              r += (R_VAL(pix) * Cy) >> 10;
              g += (G_VAL(pix) * Cy) >> 10;
              b += (B_VAL(pix) * Cy) >> 10;
              a += (A_VAL(pix) * Cy) >> 10;
            }

            if (j > 0)
            {
              pix += sow;
              r += (R_VAL(pix) * j) >> 10;
              g += (G_VAL(pix) * j) >> 10;
              b += (B_VAL(pix) * j) >> 10;
              a += (A_VAL(pix) * j) >> 10;
            }

            if (XAP > 0)
            {
              pix = ypoints[y] + xpoints[x] + 1;
              rr = (R_VAL(pix) * yap) >> 10;
              gg = (G_VAL(pix) * yap) >> 10;
              bb = (B_VAL(pix) * yap) >> 10;
              aa = (A_VAL(pix) * yap) >> 10;

              for (j = (1 << 14) - yap; j > Cy; j -= Cy)
              {
                pix += sow;
                rr += (R_VAL(pix) * Cy) >> 10;
                gg += (G_VAL(pix) * Cy) >> 10;
                bb += (B_VAL(pix) * Cy) >> 10;
                aa += (A_VAL(pix) * Cy) >> 10;
              }
              if (j > 0)
              {
                pix += sow;
                rr += (R_VAL(pix) * j) >> 10;
                gg += (G_VAL(pix) * j) >> 10;
                bb += (B_VAL(pix) * j) >> 10;
                aa += (A_VAL(pix) * j) >> 10;
              }

              r = r * INV_XAP;
              g = g * INV_XAP;
              b = b * INV_XAP;
              a = a * INV_XAP;

              r = (r + ((rr * XAP))) >> 12;
              g = (g + ((gg * XAP))) >> 12;
              b = (b + ((bb * XAP))) >> 12;
              a = (a + ((aa * XAP))) >> 12;
            }
            else
            {
              r >>= 4;
              g >>= 4;
              b >>= 4;
              a >>= 4;
            }

            ((uint32_t*)dst)[0] = ARGB_COMPOSE(r, g, b, a);
            dst += 4;
          }
        }
        else if (ctx->scale.xup_yup == 2)
        {
          // Scaling down horizontally.
          int Cx, j;
          const uint32_t* pix;
          int r, g, b, a, rr, gg, bb, aa;
          int xap;

          for (; x < end; x++)
          {
            Cx = XAP >> 16;
            xap = XAP & 0xffff;

            pix = ypoints[y] + xpoints[x];
            r = (R_VAL(pix) * xap) >> 10;
            g = (G_VAL(pix) * xap) >> 10;
            b = (B_VAL(pix) * xap) >> 10;
            a = (A_VAL(pix) * xap) >> 10;

            for (j = (1 << 14) - xap; j > Cx; j -= Cx)
            {
              pix++;
              r += (R_VAL(pix) * Cx) >> 10;
              g += (G_VAL(pix) * Cx) >> 10;
              b += (B_VAL(pix) * Cx) >> 10;
              a += (A_VAL(pix) * Cx) >> 10;
            }

            if (j > 0)
            {
              pix++;
              r += (R_VAL(pix) * j) >> 10;
              g += (G_VAL(pix) * j) >> 10;
              b += (B_VAL(pix) * j) >> 10;
              a += (A_VAL(pix) * j) >> 10;
            }

            if (YAP > 0)
            {
              pix = ypoints[y] + xpoints[x] + sow;

              rr = (R_VAL(pix) * xap) >> 10;
              gg = (G_VAL(pix) * xap) >> 10;
              bb = (B_VAL(pix) * xap) >> 10;
              aa = (A_VAL(pix) * xap) >> 10;

              for (j = (1 << 14) - xap; j > Cx; j -= Cx)
              {
                pix++;
                rr += (R_VAL(pix) * Cx) >> 10;
                gg += (G_VAL(pix) * Cx) >> 10;
                bb += (B_VAL(pix) * Cx) >> 10;
                aa += (A_VAL(pix) * Cx) >> 10;
              }
              if (j > 0)
              {
                pix++;
                rr += (R_VAL(pix) * j) >> 10;
                gg += (G_VAL(pix) * j) >> 10;
                bb += (B_VAL(pix) * j) >> 10;
                aa += (A_VAL(pix) * j) >> 10;
              }

              r = r * INV_YAP;
              g = g * INV_YAP;
              b = b * INV_YAP;
              a = a * INV_YAP;

              r = (r + ((rr * YAP))) >> 12;
              g = (g + ((gg * YAP))) >> 12;
              b = (b + ((bb * YAP))) >> 12;
              a = (a + ((aa * YAP))) >> 12;
            }
            else
            {
              r >>= 4;
              g >>= 4;
              b >>= 4;
              a >>= 4;
            }

            ((uint32_t*)dst)[0] = ARGB_COMPOSE(r, g, b, a);
            dst += 4;
          }
        }
        else
        {
          // Scaling down horizontally & vertically.
          int Cx, Cy, i, j;
          const uint32_t* pix;
          int a, r, g, b, ax, rx, gx, bx;
          int xap, yap;

          Cy = YAP >> 16;
          yap = YAP & 0xffff;

          for (; x < end; x++)
          {
            Cx = XAP >> 16;
            xap = XAP & 0xffff;

            sptr = ypoints[y] + xpoints[x];
            pix = sptr;
            sptr += sow;

            rx = (R_VAL(pix) * xap) >> 9;
            gx = (G_VAL(pix) * xap) >> 9;
            bx = (B_VAL(pix) * xap) >> 9;
            ax = (A_VAL(pix) * xap) >> 9;

            pix++;

            for (i = (1 << 14) - xap; i > Cx; i -= Cx)
            {
              rx += (R_VAL(pix) * Cx) >> 9;
              gx += (G_VAL(pix) * Cx) >> 9;
              bx += (B_VAL(pix) * Cx) >> 9;
              ax += (A_VAL(pix) * Cx) >> 9;
              pix++;
            }

            if (i > 0)
            {
              rx += (R_VAL(pix) * i) >> 9;
              gx += (G_VAL(pix) * i) >> 9;
              bx += (B_VAL(pix) * i) >> 9;
              ax += (A_VAL(pix) * i) >> 9;
            }

            r = (rx * yap) >> 14;
            g = (gx * yap) >> 14;
            b = (bx * yap) >> 14;
            a = (ax * yap) >> 14;

            for (j = (1 << 14) - yap; j > Cy; j -= Cy)
            {
              pix = sptr;
              sptr += sow;
              rx = (R_VAL(pix) * xap) >> 9;
              gx = (G_VAL(pix) * xap) >> 9;
              bx = (B_VAL(pix) * xap) >> 9;
              ax = (A_VAL(pix) * xap) >> 9;
              pix++;

              for (i = (1 << 14) - xap; i > Cx; i -= Cx)
              {
                rx += (R_VAL(pix) * Cx) >> 9;
                gx += (G_VAL(pix) * Cx) >> 9;
                bx += (B_VAL(pix) * Cx) >> 9;
                ax += (A_VAL(pix) * Cx) >> 9;
                pix++;
              }

              if (i > 0)
              {
                rx += (R_VAL(pix) * i) >> 9;
                gx += (G_VAL(pix) * i) >> 9;
                bx += (B_VAL(pix) * i) >> 9;
                ax += (A_VAL(pix) * i) >> 9;
              }

              r += (rx * Cy) >> 14;
              g += (gx * Cy) >> 14;
              b += (bx * Cy) >> 14;
              a += (ax * Cy) >> 14;
            }
            if (j > 0)
            {
              pix = sptr;
              sptr += sow;
              rx = (R_VAL(pix) * xap) >> 9;
              gx = (G_VAL(pix) * xap) >> 9;
              bx = (B_VAL(pix) * xap) >> 9;
              ax = (A_VAL(pix) * xap) >> 9;
              pix++;

              for (i = (1 << 14) - xap; i > Cx; i -= Cx)
              {
                rx += (R_VAL(pix) * Cx) >> 9;
                gx += (G_VAL(pix) * Cx) >> 9;
                bx += (B_VAL(pix) * Cx) >> 9;
                ax += (A_VAL(pix) * Cx) >> 9;
                pix++;
              }

              if (i > 0)
              {
                rx += (R_VAL(pix) * i) >> 9;
                gx += (G_VAL(pix) * i) >> 9;
                bx += (B_VAL(pix) * i) >> 9;
                ax += (A_VAL(pix) * i) >> 9;
              }

              r += (rx * j) >> 14;
              g += (gx * j) >> 14;
              b += (bx * j) >> 14;
              a += (ax * j) >> 14;
            }

            R_VAL(dst) = r >> 5;
            G_VAL(dst) = g >> 5;
            B_VAL(dst) = b >> 5;
            A_VAL(dst) = a >> 5;
            dst += 4;
          }
        }

        if (w == 0) break;

        i = Math::min(dw, w);
        x = 0;
        end = i;
      };
      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()
  }
};

} // RasterOps_C namespace
} // Fog namespace

#endif
