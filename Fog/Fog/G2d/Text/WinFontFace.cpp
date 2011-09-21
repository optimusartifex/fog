// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Private.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/MemBufferTmp_p.h>
#include <Fog/Core/Tools/TextIterator.h>
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/G2d/Text/WinFontFace.h>
#include <Fog/G2d/Text/WinFontProvider.h>
#include <Fog/G2d/Text/WinFontUtil_p.h>

// Define if missing.
#ifndef GGO_UNHINTED
#define GGO_UNHINTED 0x0100
#endif // GGO_UNHINTED

#ifndef CLEARTYPE_QUALITY
#define CLEARTYPE_QUALITY 5
#endif // CLEARTYPE_QUALITY

#ifndef TT_PRIM_CSPLINE
#define TT_PRIM_CSPLINE 3
#endif // TT_PRIM_CSPLINE

namespace Fog {

// ============================================================================
// [Fog::WinFontFace - Helpers - FIXED]
// ============================================================================

static FOG_INLINE FIXED _WinFIXEDZero()
{
  FIXED fx;
  fx.value = 0;
  fx.fract = 0;
  return fx;
}

static FOG_INLINE FIXED _WinFIXEDOne()
{
  FIXED fx;
  fx.value = 1;
  fx.fract = 0;
  return fx;
}

static FOG_INLINE FIXED _WinFIXEDFrom16x16(uint32_t v16x16)
{
  FIXED fx;
  fx.value = v16x16 >> 16;
  fx.fract = v16x16 & 0xFFFF;
  return fx;
}

template<typename NumT>
static FOG_INLINE FIXED _WinFIXEDFromFloat(const NumT& d)
{
  return _WinFIXEDFrom16x16(Math::fixed16x16FromFloat(d));
}

template<typename NumT>
static FOG_INLINE NumT _WinFloatFromFIXED(const FIXED& f)
{
  return NumT(f.value) + NumT(f.fract) * NumT(1.0 / 65536.0);
}

// ============================================================================
// [Fog::WinFontFace - Context]
// ============================================================================

struct FOG_NO_EXPORT WinFontContext
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  WinFontContext();
  ~WinFontContext();

  // --------------------------------------------------------------------------
  // [Validity]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isInitialized() const { return hOldFont != NULL; }

  // --------------------------------------------------------------------------
  // [Create]
  // --------------------------------------------------------------------------

  err_t prepare(HFONT hFont);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  HDC hDC;
  HGDIOBJ hOldFont;

  GLYPHMETRICS gm;
  MAT2 mat;

  MemBufferTmp<2048> buffer;
};

WinFontContext::WinFontContext()
{
  hDC = NULL;
  hOldFont = NULL;
}

WinFontContext::~WinFontContext()
{
  if (hOldFont != NULL) SelectObject(hDC, hOldFont);
  if (hDC      != NULL) DeleteDC(hDC);
}

err_t WinFontContext::prepare(HFONT hFont)
{
  FOG_ASSERT(hDC == NULL);

  if (hDC == NULL)
  {
    hDC = CreateCompatibleDC(NULL);
    if (hDC == NULL) return ERR_RT_OUT_OF_MEMORY;
    SetGraphicsMode(hDC, GM_ADVANCED);
  }

  hOldFont = SelectObject(hDC, (HGDIOBJ)hFont);
  if (hOldFont == NULL) return ERR_FONT_INTERNAL;

  ZeroMemory(&gm, sizeof(gm));
  mat.eM11 = _WinFIXEDOne();
  mat.eM12 = _WinFIXEDZero();
  mat.eM21 = _WinFIXEDZero();
  mat.eM22 = _WinFIXEDOne();

  return ERR_OK;
}

template<typename NumT>
static err_t _WinDecomposeTTOutline(NumT_(Path)& path,
  const uint8_t* glyphBuffer, size_t glyphSize, bool flipY)
{
  err_t err = ERR_OK;

  const uint8_t* glyphCur = glyphBuffer;
  const uint8_t* glyphEnd = glyphBuffer + glyphSize;
  if (glyphCur == glyphEnd) return ERR_OK;

  NumT_(Point) p1(UNINITIALIZED);

  do {
    const TTPOLYGONHEADER* polyHeader = (TTPOLYGONHEADER*)glyphCur;

    const uint8_t* polyEnd = glyphCur + polyHeader->cb;
    const uint8_t* polyCur = glyphCur + sizeof(TTPOLYGONHEADER);

    p1.x = _WinFloatFromFIXED<NumT>(polyHeader->pfxStart.x);
    p1.y = _WinFloatFromFIXED<NumT>(polyHeader->pfxStart.y);
    if (flipY) p1.y = -p1.y;

    path.moveTo(p1);

    while (polyCur < polyEnd)
    {
      const TTPOLYCURVE* pc = (const TTPOLYCURVE*)polyCur;

      switch (pc->wType)
      {
        case TT_PRIM_LINE:
        {
          for (uint i = 0; i < pc->cpfx; i++)
          {
            p1.x = _WinFloatFromFIXED<NumT>(pc->apfx[i].x);
            p1.y = _WinFloatFromFIXED<NumT>(pc->apfx[i].y);
            if (flipY) p1.y = -p1.y;

            path.lineTo(p1);
          }
          break;
        }

        case TT_PRIM_QSPLINE:
        {
          for (uint i = 0; i < (uint)pc->cpfx - 1; i++)
          {
            NumT_(Point) p2(UNINITIALIZED);

            // B is always the current point.
            p1.x = _WinFloatFromFIXED<NumT>(pc->apfx[i].x);
            p1.y = _WinFloatFromFIXED<NumT>(pc->apfx[i].y);

            p2.x = _WinFloatFromFIXED<NumT>(pc->apfx[i + 1].x);
            p2.y = _WinFloatFromFIXED<NumT>(pc->apfx[i + 1].y);

            // If not on last spline, compute C (midpoint).
            if (i < (uint)pc->cpfx - 2)
            {
              p2.x = (p1.x + p2.x) * 0.5f;
              p2.y = (p1.y + p2.y) * 0.5f;
            }

            if (flipY)
            {
              p1.y = -p1.y;
              p2.y = -p2.y;
            }

            path.quadTo(p1, p2);
          }
          break;
        }

        case TT_PRIM_CSPLINE:
        {
          for (uint i = 0; i < (uint)pc->cpfx - 1; i++)
          {
            NumT_(Point) p2(UNINITIALIZED);
            NumT_(Point) p3(UNINITIALIZED);

            p1.x = _WinFloatFromFIXED<NumT>(pc->apfx[i].x);
            p1.y = _WinFloatFromFIXED<NumT>(pc->apfx[i].y);

            p2.x = _WinFloatFromFIXED<NumT>(pc->apfx[i + 1].x);
            p2.y = _WinFloatFromFIXED<NumT>(pc->apfx[i + 1].y);

            p3.x = _WinFloatFromFIXED<NumT>(pc->apfx[i + 2].x);
            p3.y = _WinFloatFromFIXED<NumT>(pc->apfx[i + 2].y);

            if (flipY)
            {
              p1.y = -p1.y;
              p2.y = -p2.y;
              p3.y = -p3.y;
            }

            path.cubicTo(p1, p2, p3);
          }
          break;
        }

        default:
          path.close();
          return err;
      }

      polyCur += sizeof(WORD) * 2 + sizeof(POINTFX) * pc->cpfx;
    }

    path.close();

    glyphCur += polyHeader->cb;
  } while (glyphCur < glyphEnd);

  return err;
}

// ============================================================================
// [Fog::WinFontFace - Handle]
// ============================================================================

static FOG_INLINE void _WinFontHandle_init(WinFontHandle& handle, HFONT hFont, FontKerningTableF* kerningTable)
{
  handle.hFont = hFont;
  handle.kerningTable = kerningTable;
}

static FOG_INLINE void _WinFontHandle_free(WinFontHandle& handle)
{
  if (handle.hFont)
  {
    DeleteObject((HGDIOBJ)handle.hFont);
    handle.hFont = NULL;
  }

  if (handle.kerningTable)
  {
    MemMgr::free(handle.kerningTable);
    handle.kerningTable = NULL;
  }
}

static FOG_INLINE void _WinFontHandle_zero(WinFontHandle& handle)
{
  handle.hFont = NULL;
  handle.kerningTable = NULL;
}

// ============================================================================
// [Fog::WinFontFace - Helpers - Metrics]
// ============================================================================

static err_t _WinGetMetricsF(HDC hdc, FontMetricsF& metrics)
{
  TEXTMETRICW tm;
  if (!GetTextMetricsW(hdc, &tm)) return ERR_FONT_INTERNAL;

  metrics.setHeight(float(tm.tmHeight));
  metrics.setAscent(float(tm.tmAscent));
  metrics.setDescent(float(tm.tmDescent));
  metrics.setInternalLeading(float(tm.tmInternalLeading));
  metrics.setExternalLeading(float(tm.tmExternalLeading));
  metrics.setAverageWidth(float(tm.tmAveCharWidth));
  metrics.setMaximumWidth(float(tm.tmMaxCharWidth));

  return ERR_OK;
}

// ============================================================================
// [Fog::WinFontFace - Helpers - Kerning]
// ============================================================================

static err_t _WinGetKerningF(HDC hdc, FontKerningTableF** out)
{
  DWORD kerningPairsLength = GetKerningPairsW(hdc, 0, NULL);
  KERNINGPAIR* kerningWin = NULL;

  if (kerningPairsLength == 0)
  {
    *out = NULL;
    return ERR_OK;
  }

  FontKerningTableF* kerningTable = reinterpret_cast<FontKerningTableF*>(
    MemMgr::alloc(FontKerningTableF::getSizeOf(kerningPairsLength)));
  if (FOG_IS_NULL(kerningTable)) return ERR_RT_OUT_OF_MEMORY;

  kerningTable->length = kerningPairsLength;

  // This is quite hacky, but it saves one memory allocation and some cached
  // space. I don't know if it's possible that the size of the KERNINGPAIR is
  // different to the Fog::FontKerningPairI/F, but it's implemented.
  if (sizeof(FontKerningPairF) == sizeof(KERNINGPAIR))
  {
    kerningWin = reinterpret_cast<KERNINGPAIR*>(kerningTable->pairs);
  }
  else
  {
    kerningWin = reinterpret_cast<KERNINGPAIR*>(
      MemMgr::alloc(kerningPairsLength * sizeof(KERNINGPAIR)));
  }

  if (FOG_IS_NULL(kerningWin))
  {
    MemMgr::free(kerningTable);
    return ERR_RT_OUT_OF_MEMORY;
  }

  bool ok = (GetKerningPairsW(hdc, kerningPairsLength, kerningWin) == kerningPairsLength);

  if (ok)
  {
    for (uint32_t i = 0; i < kerningPairsLength; i++)
    {
      FontKerningChars chars;

      chars.first = kerningWin[i].wFirst;
      chars.second = kerningWin[i].wSecond;
      float amount = float(kerningWin[i].iKernAmount);

      kerningTable->pairs[i].chars = chars;
      kerningTable->pairs[i].amount = amount;
    }
  }

  if ((void*)kerningWin != (void*)kerningTable->pairs)
  {
    MemMgr::free(kerningWin);
  }

  if (!ok)
  {
    MemMgr::free(kerningTable);
    return ERR_FONT_INTERNAL;
  }

  *out = kerningTable;
  return ERR_OK;
}

// ============================================================================
// [Fog::WinFontFace - Construction / Destruction]
// ============================================================================

WinFontFace::WinFontFace(WinFontProviderData* pd_)
{
  FOG_ASSERT(pd_ != NULL);

  id = FONT_FACE_WINDOWS;
  pd = pd_;

  _WinFontHandle_zero(hMaster);
  for (uint i = 0; i < HFONT_CACHE_SIZE; i++)
    _WinFontHandle_zero(hCache[i]);
}

WinFontFace::~WinFontFace()
{
  _reset();
}

// ============================================================================
// [Fog::WinFontFace - Interface]
// ============================================================================

err_t WinFontFace::getTextOutline(PathF& dst, const FontData* d, const PointF& pt, const StubW& str)
{
  AutoLock locked(pd->lock);

  WinFontContext ctx;
  return _getTextOutline(dst, &outlineCache, d, pt, str, &ctx);
}

err_t WinFontFace::getTextOutline(PathD& dst, const FontData* d, const PointD& pt, const StubW& str)
{
  AutoLock locked(pd->lock);

  WinFontContext ctx;
  return _getTextOutline(dst, &outlineCache, d, pt, str, &ctx);
}

err_t WinFontFace::getTextExtents(TextExtentsF& extents, const FontData* d, const StubW& str)
{
  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t WinFontFace::getTextExtents(TextExtentsD& extents, const FontData* d, const StubW& str)
{
  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

FontKerningTableF* WinFontFace::getKerningTable(const FontData* d)
{
  return hMaster.kerningTable;
}

// ============================================================================
// [Fog::WinFontFace - Private]
// ============================================================================

template<typename NumT>
static FOG_INLINE err_t _WinFontFace_renderGlyphOutline(WinFontFace* self,
  NumT_(Path)& dst, GlyphMetricsF& metrics, const FontData* d, uint32_t uc, WinFontContext* ctx)
{
  if (!ctx->isInitialized())
    FOG_RETURN_ON_ERROR(ctx->prepare(self->hMaster.hFont));

  for (;;)
  {
    DWORD dataSize = GetGlyphOutlineW(ctx->hDC, uc, GGO_NATIVE | GGO_UNHINTED, &ctx->gm, (DWORD)ctx->buffer.getCapacity(), ctx->buffer.getMem(), &ctx->mat);
    if (dataSize == GDI_ERROR) return ERR_FONT_INTERNAL;

    if ((size_t)dataSize > ctx->buffer.getCapacity())
    {
      // Reserve space for large glyph(s) and align it.
      dataSize = (dataSize + 4095) & ~4095;

      if (FOG_IS_NULL(ctx->buffer.alloc(dataSize)))
        return ERR_RT_OUT_OF_MEMORY;

      continue;
    }

    FOG_RETURN_ON_ERROR(_WinDecomposeTTOutline<NumT>(dst,
      reinterpret_cast<uint8_t*>(ctx->buffer.getMem()), dataSize, true));

    metrics._horizontalAdvance.set(PointF(float(ctx->gm.gmCellIncX), float(ctx->gm.gmCellIncY)));
    metrics._verticalAdvance.reset();

    return ERR_OK;
  }
}

err_t WinFontFace::_renderGlyphOutline(PathF& dst, GlyphMetricsF& metrics, const FontData* d, uint32_t uc, void* ctx)
{
  return _WinFontFace_renderGlyphOutline<float>(this, dst, metrics, d, uc, reinterpret_cast<WinFontContext*>(ctx));
}

err_t WinFontFace::_renderGlyphOutline(PathD& dst, GlyphMetricsF& metrics, const FontData* d, uint32_t uc, void* ctx)
{
  return _WinFontFace_renderGlyphOutline<double>(this, dst, metrics, d, uc, reinterpret_cast<WinFontContext*>(ctx));
}

// ============================================================================
// [Fog::WinFontFace - Methods]
// ============================================================================

err_t WinFontFace::_init(const LOGFONTW* logFont)
{
  err_t err = ERR_FONT_INTERNAL;
  FontKerningTableF* kerningTable = NULL;

  HDC hDC;
  HFONT hFont, hFontOld;

  // --------------------------------------------------------------------------
  // [HDC]
  // --------------------------------------------------------------------------

  hDC = CreateCompatibleDC(NULL);
  if (hDC == NULL) return ERR_RT_OUT_OF_MEMORY;
  SetGraphicsMode(hDC, GM_ADVANCED);

  // --------------------------------------------------------------------------
  // [HFONT]
  // --------------------------------------------------------------------------

  hFont = CreateFontIndirectW(logFont);
  if (FOG_IS_NULL(hFont)) goto _EndDeleteDC;

  hFontOld = (HFONT)SelectObject(hDC, (HGDIOBJ)hFont);
  if (hFontOld == (HFONT)GDI_ERROR) goto _EndDeleteFont;

  err = _WinGetMetricsF(hDC, designMetrics);
  if (FOG_IS_ERROR(err)) goto _EndSelect;

  err = _WinGetKerningF(hDC, &kerningTable);
  if (FOG_IS_ERROR(err)) goto _EndSelect;

  family.setWChar(logFont->lfFaceName);
  designEm = designMetrics.getHeight();

  // --------------------------------------------------------------------------
  // [Success]
  // --------------------------------------------------------------------------

  // Keep the FONT handle.
  _WinFontHandle_init(hMaster, hFont, kerningTable);
  hFont = NULL;

  // Update the flags.
  features = FONT_FEATURE_RASTER  |
             FONT_FEATURE_OUTLINE |
             FONT_FEATURE_HINTING ;

  if (kerningTable)
    features |= FONT_FEATURE_KERNING;

  // --------------------------------------------------------------------------
  // [Cleanup]
  // --------------------------------------------------------------------------

_EndSelect:
  SelectObject(hDC, (HGDIOBJ)hFontOld);
_EndDeleteFont:
  if (hFont) DeleteObject((HGDIOBJ)hFont);
_EndDeleteDC:
  DeleteDC(hDC);
  return err;
}

void WinFontFace::_reset()
{
  _WinFontHandle_free(hMaster);

  for (uint i = 0; i < HFONT_CACHE_SIZE; i++)
    _WinFontHandle_free(hCache[i]);
}

} // Fog namespace
