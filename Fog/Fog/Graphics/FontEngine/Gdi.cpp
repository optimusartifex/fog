// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

#include <Fog/Build/Build.h>

#if defined(FOG_FONT_WINDOWS)

#include <Fog/Core/AutoLock.h>
#include <Fog/Core/Constants.h>
#include <Fog/Core/List.h>
#include <Fog/Core/Lock.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Font.h>
#include <Fog/Graphics/Matrix.h>
#include <Fog/Graphics/Geometry.h>
#include <Fog/Graphics/Path.h>
#include <Fog/Graphics/TextLayout.h>

#include <Fog/Graphics/FontEngine/Gdi.h>

#ifndef GGO_UNHINTED
#define GGO_UNHINTED 0x0100
#endif // GGO_UNHINTED

namespace Fog {

// tools

struct WinEnumFontStruct
{
  List<String>* fonts;
  HDC hdc;
  WCHAR buffer[256];
};

// ============================================================================
// [Fog::WinFontEngine]
// ============================================================================

WinFontEngine::WinFontEngine() :
  FontEngine(Ascii8("Windows"))
{
}

WinFontEngine::~WinFontEngine()
{
}

static int CALLBACK enumFontCb(CONST LOGFONTW* lplf, CONST TEXTMETRICW* lpntm, DWORD fontType, LPARAM lParam)
{
  WinEnumFontStruct* efs = (WinEnumFontStruct*)lParam;
  const WCHAR* lfFaceName = lplf->lfFaceName; 

  // Some rejects, I dislike '@'.
  if (lfFaceName[0] == L'@') return 1;
  
  // Windows will send us more fonts that we want, but usually
  // equal fonts are sent together, so we will simply copy this
  // font to buffer and compare it with previous. If this will
  // match - reject it now to save cpu cycles.
  if (wcscmp(efs->buffer, lfFaceName) == 0) return 1;
  // Font not match, so copy it to buffer.
  wcscpy_s(efs->buffer, 256, lfFaceName);

  // Many of the fonts are the same family, so temporary string
  // is here better
  TemporaryString<TEMP_LENGTH> name;
  name.set(reinterpret_cast<const Char*>(lfFaceName));

  if (!efs->fonts->contains(name)) efs->fonts->append(name);
  
  // We return 1 to continue listing...
  return 1;
}

List<String> WinFontEngine::getFontList()
{
  List<String> fonts;
  WinEnumFontStruct efs;
  efs.fonts = &fonts;
  efs.hdc = GetDC(NULL);
  efs.buffer[0] = 0;

  LOGFONT lf;
  ZeroMemory(&lf, sizeof(lf));
  lf.lfCharSet = DEFAULT_CHARSET;

  EnumFontFamiliesEx(efs.hdc, &lf, (FONTENUMPROC)enumFontCb, (LPARAM)&efs, 0);
  ReleaseDC(NULL, efs.hdc);

  return fonts;
}

FontFace* WinFontEngine::createDefaultFace()
{
  // TODO: Fix this and make it working...
  /*
  NONCLIENTMETRICSW ncm;
  ncm.cbSize = sizeof(NONCLIENTMETRICS);
  SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);

  memcpy(&gDefaultFont, &(ncm.lfMessageFont), sizeof(LOGFONT));
  */

  return createFace(Ascii8("arial"), 12, FontOptions(), FloatMatrix());
}

FontFace* WinFontEngine::createFace(
  const String& family,
  float size, 
  const FontOptions& options,
  const FloatMatrix& matrix)
{
  WinFontFace* face = NULL;

  LOGFONTW logFont;
  TEXTMETRICW textMetrics;
  HFONT hOldFont;
  HFONT hFont;
  HDC hdc;

  // Get LOGFONT.
  ZeroMemory(&logFont, sizeof(LOGFONT));

  if (family.getLength() >= FOG_ARRAY_SIZE(logFont.lfFaceName)) goto fail;

  CopyMemory(logFont.lfFaceName, reinterpret_cast<const wchar_t*>(family.getData()), (family.getLength() + 1) * sizeof(WCHAR));
  logFont.lfHeight = -(int)size;
  logFont.lfWeight = options.getWeight() * 100;
  logFont.lfItalic = options.getStyle() >= FONT_STYLE_ITALIC;

  // Create HFONT.
  if ((hFont = CreateFontIndirectW(&logFont)) == NULL) goto fail;

  // Get text metrics (need to create temporary HDC).
  if ((hdc = CreateCompatibleDC(NULL)) == NULL) goto failFreeHFONT;
  if ((hOldFont = (HFONT)SelectObject(hdc, (HGDIOBJ)hFont)) == (HFONT)GDI_ERROR) goto failFreeHFONTandHDC;

  GetTextMetricsW(hdc, &textMetrics);

  SelectObject(hdc, (HGDIOBJ)hOldFont);
  DeleteDC(hdc);

  // Everything should be OK, create new font face.
  face = new(std::nothrow) WinFontFace();
  if (!face) return NULL;

  face->family = family;
  face->family.squeeze();
  face->metrics._size = size;
  face->metrics._ascent = (float)textMetrics.tmAscent;
  face->metrics._descent = (float)textMetrics.tmDescent;
  face->metrics._averageWidth = (float)textMetrics.tmAveCharWidth;
  face->metrics._maximumWidth = (float)textMetrics.tmMaxCharWidth;
  face->metrics._height = (float)textMetrics.tmHeight;
  face->options = options;
  face->matrix = matrix;
  face->hFont = hFont;

  return face;

failFreeHFONTandHDC:
  DeleteDC(hdc);
failFreeHFONT:
  DeleteObject((HGDIOBJ)hFont);
fail:
  return NULL;
}

// ============================================================================
// [Fog::WinFontFace - Helpers]
// ============================================================================

// Identity matrix. It seems that this matrix must be always passed to 
// GetGlyphOutlineW function.
static const MAT2 mat2identity =
{
  {0, 1}, {0, 0},
  {0, 0}, {0, 1}
};

static FOG_INLINE double fxToDouble(const FIXED& p)
{
  return (double)p.value + (double)p.fract * (1.0 / 65536.0);
}

static err_t decompose_win32_glyph_outline(const uint8_t* gbuf, uint size, bool flipY, const DoubleMatrix* mtx, DoublePath& path)
{
  const uint8_t* cur_glyph = gbuf;
  const uint8_t* end_glyph = gbuf + size;
  double x, y;

  if (cur_glyph == end_glyph) return ERR_OK;

  err_t err = ERR_OK;
  sysuint_t subPathIndex = path.getLength();

  do {
    const TTPOLYGONHEADER* th = (TTPOLYGONHEADER*)cur_glyph;
    
    const uint8_t* end_poly = cur_glyph + th->cb;
    const uint8_t* cur_poly = cur_glyph + sizeof(TTPOLYGONHEADER);

    x = fxToDouble(th->pfxStart.x);
    y = fxToDouble(th->pfxStart.y);
    if (flipY) y = -y;

    path.closePolygon();
    path.moveTo(DoublePoint(x, y));

    while (cur_poly < end_poly)
    {
      const TTPOLYCURVE* pc = (const TTPOLYCURVE*)cur_poly;
      
      if (pc->wType == TT_PRIM_LINE)
      {
        int i;
        for (i = 0; i < pc->cpfx; i++)
        {
          x = fxToDouble(pc->apfx[i].x);
          y = fxToDouble(pc->apfx[i].y);
          if (flipY) y = -y;
          path.lineTo(DoublePoint(x, y));
        }
      }
      
      if (pc->wType == TT_PRIM_QSPLINE)
      {
        // Walk through points in spline.
        for (int u = 0; u < pc->cpfx - 1; u++)
        {
          // B is always the current point.
          POINTFX pnt_b = pc->apfx[u];
          POINTFX pnt_c = pc->apfx[u+1];

          // If not on last spline, compute C.
          if (u < pc->cpfx - 2)
          {
            // Midpoint (x, y).
            *(int*)&pnt_c.x = (*(int*)&pnt_b.x + *(int*)&pnt_c.x) / 2;
            *(int*)&pnt_c.y = (*(int*)&pnt_b.y + *(int*)&pnt_c.y) / 2;
          }
          
          double x2, y2;
          x  = fxToDouble(pnt_b.x);
          y  = fxToDouble(pnt_b.y);
          x2 = fxToDouble(pnt_c.x);
          y2 = fxToDouble(pnt_c.y);
          if (flipY) { y = -y; y2 = -y2; }
          path.curveTo(DoublePoint(x, y), DoublePoint(x2, y2));
        }
      }
      cur_poly += sizeof(WORD) * 2 + sizeof(POINTFX) * pc->cpfx;
    }
    cur_glyph += th->cb;
  } while (cur_glyph < end_glyph);

  path.closePolygon();

  if (mtx) path.applyMatrix(*mtx, Range(subPathIndex, DETECT_LENGTH));
  return err;
}

// ============================================================================
// [Fog::WinFontFace]
// ============================================================================

WinFontFace::WinFontFace() :
  hFont(NULL)
{
}

WinFontFace::~WinFontFace()
{
  if (hFont) DeleteObject((HGDIOBJ)hFont);
}

err_t WinFontFace::getGlyphSet(const Char* str, sysuint_t length, GlyphSet& glyphSet)
{
  err_t err;
  if ( (err = glyphSet.begin(length)) ) return err;

  AutoLock locked(lock);

  GlyphData* glyphd;
  HDC hdc = NULL;

  for (sysuint_t i = 0; i != length; i++)
  {
    uint32_t uc = str[i].ch();

    // First try cache.
    glyphd = glyphCache.get(uc);
    if (FOG_UNLIKELY(!glyphd))
    {
      // Glyph is not in cache, initialize HDC (if not initialized) and try
      // to get glyph from HFONT.
      if (hdc == NULL)
      {
        if ((hdc = CreateCompatibleDC(NULL)) == NULL) continue;
        SelectObject(hdc, (HGDIOBJ)hFont);
      }

      if ((glyphd = renderGlyph(hdc, uc))) glyphCache.set(uc, glyphd);
    }

    if (FOG_LIKELY(glyphd)) glyphSet._add(glyphd->ref());
  }

  if (hdc) DeleteDC(hdc);

  if ( (err = glyphSet.end()) ) return err;
  return ERR_OK;
}

err_t WinFontFace::getOutline(const Char* str, sysuint_t length, DoublePath& dst)
{
  AutoLock locked(lock);

  err_t err = ERR_OK;
  DoubleMatrix matrix;

  GLYPHMETRICS gm;
  ZeroMemory(&gm, sizeof(gm));

  HDC hdc = CreateCompatibleDC(NULL);
  if (hdc == NULL) return GetLastError();
  SelectObject(hdc, (HGDIOBJ)hFont);

  LocalBuffer<1024> glyphBuffer;
  DWORD glyphBufferSize = 1024;
  uint8_t *glyphData = reinterpret_cast<uint8_t*>(glyphBuffer.alloc(glyphBufferSize));

  for (sysuint_t i = 0; i < length; i++)
  {
    uint32_t uc = str[i].ch();
    uint32_t dataSize;

repeat:
    dataSize = GetGlyphOutlineW(hdc, uc, GGO_NATIVE | GGO_UNHINTED, &gm, glyphBufferSize, glyphData, &mat2identity);
    if (dataSize == GDI_ERROR) continue;

    if (dataSize > glyphBufferSize)
    {
      // Reserve space for other large glyphs...
      if (dataSize < 4000) dataSize = 4000;

      glyphBuffer.free();
      glyphData = reinterpret_cast<uint8_t*>(glyphBuffer.alloc(dataSize));
      if (!glyphData) continue;

      glyphBufferSize = dataSize;
      goto repeat;
    }

    if ((err = decompose_win32_glyph_outline(glyphData, dataSize, true, &matrix, dst))) goto end;

    matrix.translate(gm.gmCellIncX, gm.gmCellIncY);
  }

end:
  DeleteDC(hdc);
  return err;
}

err_t WinFontFace::getTextExtents(const Char* str, sysuint_t length, TextExtents& extents)
{
  TemporaryGlyphSet<128> glyphSet;
  err_t err = getGlyphSet(str, length, glyphSet);

  if (err)
  {
    ZeroMemory(&extents, sizeof(TextExtents));
    return err;
  }
  else
  {
    extents.advance = glyphSet.getAdvance();
    extents.beginWidth = 0;
    extents.endWidth = 0;
    return ERR_OK;
  }
}

GlyphData* WinFontFace::renderGlyph(HDC hdc, uint32_t uc)
{
  // renderBegin() must be called before
  FOG_ASSERT(hdc);

  GlyphData* glyphd = NULL;
  Image::Data* bitmapd = NULL;

  GLYPHMETRICS gm;
  ZeroMemory(&gm, sizeof(gm));

  uint32_t dataSize = GetGlyphOutlineW(hdc, uc, GGO_GRAY8_BITMAP, &gm, 0, NULL, &mat2identity);

  // GetGlyphOutline() fails when being called
  // for GGO_GRAY8_BITMAP and white space.
  if (dataSize == GDI_ERROR)
  {
    dataSize = GetGlyphOutlineW(hdc, uc, GGO_METRICS, &gm, 0, NULL, &mat2identity);
    if (dataSize == GDI_ERROR) return NULL;
    dataSize = 0;
  }

  glyphd = new(std::nothrow) GlyphData();
  if (glyphd == NULL) return NULL;

  // Whitespace?
  if (dataSize == 0) gm.gmBlackBoxX = gm.gmBlackBoxY = 0;

  glyphd->offset.set(gm.gmptGlyphOrigin.x, (int)metrics.getAscent() - gm.gmptGlyphOrigin.y);
  glyphd->beginWidth = 0;
  glyphd->endWidth = 0;

  glyphd->advance = gm.gmCellIncX;
  // glyphd->advanceY = -gm.gmCellIncY;

  // Whitespace? We are done
  if (dataSize == 0) return glyphd;

  // Alloc image for glyph
  if (glyphd->bitmap.create(gm.gmBlackBoxX, gm.gmBlackBoxY, PIXEL_FORMAT_A8) != ERR_OK)
  {
    delete glyphd;
    return NULL;
  }
  bitmapd = glyphd->bitmap._d;

  // Fog library should align scanlines to 32 bits like Windows does.
  FOG_ASSERT((bitmapd->stride & 0x3) == 0);
  // This should be also equal.
  FOG_ASSERT(dataSize == bitmapd->stride * bitmapd->height);

  dataSize = GetGlyphOutlineW(hdc, uc, GGO_GRAY8_BITMAP, &gm, dataSize, bitmapd->data, &mat2identity);
  // If previous call to GetGlyphOutlineW was ok, this should be also ok, but nobody knows.
  if (dataSize == GDI_ERROR)
  {
    delete glyphd;
    return NULL;
  }

  // Fog is using 256 level of antialiasing so extend the glyph provided by
  // Windows (that uses only 64 levels).
  uint32_t x, y;
  for (y = 0; y != gm.gmBlackBoxY; y++)
  {
    uint8_t* p = bitmapd->first + y * bitmapd->stride;
    for (x = 0; x < gm.gmBlackBoxX; x++)
    {
      *p++ = (*p > 63) ? 0xFF : *p << 2;
    }
  }

  return glyphd;
}

} // Fog namespace

#endif // FOG_FONT_WINDOWS
