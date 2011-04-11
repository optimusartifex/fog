// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#include <Fog/Core/Config/Config.h>
#if defined(FOG_FONT_WINDOWS)

// [Dependencies]
#include <Fog/Core/Collection/List.h>
#include <Fog/Core/Collection/PBuffer.h>
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Math/Fixed.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Tools/StringUtil.h>
#include <Fog/G2d/Font/Font.h>
#include <Fog/G2d/Font/WinFontEngine.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Global/Constants.h>
#include <Fog/G2d/Text/TextLayout.h>

#ifndef GGO_UNHINTED
#define GGO_UNHINTED 0x0100
#endif // GGO_UNHINTED

namespace Fog {

// ============================================================================
// [Helpers]
// ============================================================================

static FOG_INLINE double FIXEDToDouble(const FIXED& f)
{
  return (double)f.value + (double)f.fract * (1.0 / 65536.0);
}

static FOG_INLINE FIXED DoubleToFIXED(const double& d)
{
  FIXED f;
  uint32_t v = Math::fixed16x16FromFloat(d);

  f.value = v >> 16;
  f.fract = v & 0xFFFF;

  return f;
}

static FOG_INLINE double FIXEDToFloat(const FIXED& f)
{
  return (float)f.value + (float)f.fract * (1.0f / 65536.0f);
}

static FOG_INLINE FIXED FloatToFIXED(const float& d)
{
  FIXED f;
  uint32_t v = Math::fixed16x16FromFloat(d);

  f.value = v >> 16;
  f.fract = v & 0xFFFF;

  return f;
}

// Identity matrix.
// 
// It seems that this matrix must be always passed to GetGlyphOutlineW function.
static const MAT2 mat2identity =
{
  {0, 1}, {0, 0},
  {0, 0}, {0, 1}
};

static err_t decompose_win32_glyph_outline(const uint8_t* gbuf, uint size, bool flipY, const TransformD* mtx, PathD& path)
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

    x = FIXEDToDouble(th->pfxStart.x);
    y = FIXEDToDouble(th->pfxStart.y);
    if (flipY) y = -y;

    path.close();
    path.moveTo(PointD(x, y));

    while (cur_poly < end_poly)
    {
      const TTPOLYCURVE* pc = (const TTPOLYCURVE*)cur_poly;
      
      if (pc->wType == TT_PRIM_LINE)
      {
        int i;
        for (i = 0; i < pc->cpfx; i++)
        {
          x = FIXEDToDouble(pc->apfx[i].x);
          y = FIXEDToDouble(pc->apfx[i].y);
          if (flipY) y = -y;
          path.lineTo(PointD(x, y));
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
          x  = FIXEDToDouble(pnt_b.x);
          y  = FIXEDToDouble(pnt_b.y);
          x2 = FIXEDToDouble(pnt_c.x);
          y2 = FIXEDToDouble(pnt_c.y);
          if (flipY) { y = -y; y2 = -y2; }
          path.quadTo(PointD(x, y), PointD(x2, y2));
        }
      }
      cur_poly += sizeof(WORD) * 2 + sizeof(POINTFX) * pc->cpfx;
    }
    cur_glyph += th->cb;
  } while (cur_glyph < end_glyph);

  path.close();

  if (mtx) path.transform(*mtx, Range(subPathIndex));
  return err;
}

// Tools.

struct WinEnumFontStruct
{
  List<String>* fonts;
  HDC hdc;
  WCHAR buffer[256];
};

static FOG_INLINE HDC winCreateDC()
{
  HDC hdc = CreateCompatibleDC(NULL);
  if (hdc != NULL) SetGraphicsMode(hdc, GM_ADVANCED);
  return hdc;
}

// ============================================================================
// [Fog::WinFontEngine]
// ============================================================================

WinFontEngine::WinFontEngine() : FontEngine(FONT_FACE_WINDOWS)
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
#if defined(FOG_CC_MSC)
  wcscpy_s(efs->buffer, 256, lfFaceName);
#else
  wcscpy(efs->buffer, lfFaceName);
#endif

  // Many of the fonts are the same family, so temporary string
  // is better here.
  TemporaryString<TEMPORARY_LENGTH> name;
  name.set(reinterpret_cast<const Char*>(lfFaceName));

  if (!efs->fonts->contains(name)) efs->fonts->append(name);
  
  // Return 1 to continue listing.
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

  return createFace(Ascii8("arial"), 12, FontOptions(), TransformF());
}

FontFace* WinFontEngine::createFace(
  const String& family,
  float size, 
  const FontOptions& options,
  const TransformF& transform)
{
  WinFontFace* face = NULL;

  LOGFONTW logFont;
  TEXTMETRICW textMetrics;
  HFONT hOldFont;
  HFONT hFont;
  HDC hdc;

  // Get LOGFONT.
  ZeroMemory(&logFont, sizeof(LOGFONT));

  if (family.getLength() >= FOG_ARRAY_SIZE(logFont.lfFaceName)) goto _Fail;

  CopyMemory(logFont.lfFaceName, reinterpret_cast<const wchar_t*>(
    family.getData()), (family.getLength() + 1) * sizeof(WCHAR));
  logFont.lfHeight = -(int)size;
  logFont.lfWeight = options.getWeight() * 100;
  logFont.lfItalic = options.getStyle() >= FONT_STYLE_ITALIC;

  // Create HFONT.
  if ((hFont = CreateFontIndirectW(&logFont)) == NULL) goto _Fail;

  // Get text metrics (need to create temporary HDC).
  if ((hdc = winCreateDC()) == NULL) goto _FailFreeHFONT;
  if ((hOldFont = (HFONT)SelectObject(hdc, (HGDIOBJ)hFont)) == (HFONT)GDI_ERROR) goto _FailFreeHFONTandHDC;

  GetTextMetricsW(hdc, &textMetrics);

  SelectObject(hdc, (HGDIOBJ)hOldFont);
  DeleteDC(hdc);

  // Everything should be OK, create new font face.
  face = fog_new WinFontFace();
  if (FOG_IS_NULL(face)) goto _FailFreeHFONT;

  face->family = family;
  face->family.squeeze();
  face->metrics._size = size;
  face->metrics._ascent = (float)textMetrics.tmAscent;
  face->metrics._descent = (float)textMetrics.tmDescent;
  face->metrics._averageWidth = (float)textMetrics.tmAveCharWidth;
  face->metrics._maximumWidth = (float)textMetrics.tmMaxCharWidth;
  face->metrics._height = (float)textMetrics.tmHeight;
  face->options = options;
  face->transform = transform;
  face->hFont = hFont;

  return face;

_FailFreeHFONTandHDC:
  DeleteDC(hdc);
_FailFreeHFONT:
  DeleteObject((HGDIOBJ)hFont);
_Fail:
  return NULL;
}

// ============================================================================
// [Fog::WinFontMaster]
// ============================================================================

WinFontMaster::WinFontMaster() :
  family(family)
{
  refCount.init(1);

  ZeroMemory(&logFont, sizeof(LOGFONTW));
  hFont = NULL;

  kerningPairs = NULL;
  kerningCount = 0;
}

WinFontMaster::~WinFontMaster()
{
}

// ============================================================================
// [Fog::WinFontFace]
// ============================================================================

WinFontFace::WinFontFace() :
  hFont(NULL)
{
  type = FONT_FACE_WINDOWS;
  master = NULL;
}

WinFontFace::~WinFontFace()
{
  if (hFont) DeleteObject((HGDIOBJ)hFont);
}

err_t WinFontFace::getGlyphSet(const Char* str, sysuint_t length, GlyphSet& glyphSet)
{
  if (length == DETECT_LENGTH) length = StringUtil::len(str);
  if (length == 0) return ERR_OK;

  // This really shouldn't happen!
  if (str[0].isTrailSurrogate()) return ERR_STRING_INVALID_UTF16;

  FOG_RETURN_ON_ERROR(glyphSet.begin(length));

  AutoLock locked(lock);

  GlyphData* glyphd;
  HDC hdc = NULL;

  err_t err = ERR_OK;
  sysuint_t remain = length;

  do {
    // Get unicode character, it's also needed to handle surrogate pairs.
    uint32_t uc = str[0].ch();
    str++;
    remain--;

    if (FOG_UNLIKELY(Char::isSurrogatePair((uint16_t)uc)))
    {
      if (!remain) { err = ERR_STRING_TRUNCATED; goto _End; }
      uc = Char::fromSurrogate((uint16_t)uc, str[0].ch());

      str++;
      remain--;
    }

    // First try cache.
    if (FOG_UNLIKELY((glyphd = glyphCache.get(uc)) == NULL))
    {
      // Glyph is not in cache.
      //
      // Initialize HDC (if not initialized) and try to get glyph from HFONT.
      if (hdc == NULL)
      {
        if ((hdc = winCreateDC()) == NULL) continue;
        SelectObject(hdc, (HGDIOBJ)hFont);
      }

      if ((glyphd = renderGlyph(hdc, uc))) glyphCache.set(uc, glyphd);
    }

    if (FOG_LIKELY(glyphd)) glyphSet._add(glyphd->ref());
  } while (remain);

_End:
  if (hdc) DeleteDC(hdc);
  if (err == ERR_OK) FOG_RETURN_ON_ERROR(glyphSet.end());

  return err;
}

err_t WinFontFace::getOutline(const Char* str, sysuint_t length, PathD& dst)
{
  AutoLock locked(lock);

  err_t err = ERR_OK;
  TransformD transform;

  GLYPHMETRICS gm;
  ZeroMemory(&gm, sizeof(gm));

  HDC hdc = winCreateDC();
  if (hdc == NULL) return GetLastError();
  SelectObject(hdc, (HGDIOBJ)hFont);

  PBuffer<1024> glyphBuffer;
  DWORD glyphBufferSize = 1024;
  uint8_t *glyphData = reinterpret_cast<uint8_t*>(glyphBuffer.alloc(glyphBufferSize));

  for (sysuint_t i = 0; i < length; i++)
  {
    uint32_t uc = str[i].ch();
    uint32_t dataSize;

_Repeat:
    dataSize = GetGlyphOutlineW(hdc, uc, GGO_NATIVE | GGO_UNHINTED, &gm, glyphBufferSize, glyphData, &mat2identity);
    if (dataSize == GDI_ERROR) continue;

    if (dataSize > glyphBufferSize)
    {
      // Reserve space for other large glyphs...
      if (dataSize < 4000) dataSize = 4000;

      glyphBuffer.reset();
      glyphData = reinterpret_cast<uint8_t*>(glyphBuffer.alloc(dataSize));
      if (FOG_IS_NULL(glyphData)) continue;

      glyphBufferSize = dataSize;
      goto _Repeat;
    }

    if ((err = decompose_win32_glyph_outline(glyphData, dataSize, true, &transform, dst))) goto _End;

    transform.translate(PointD(gm.gmCellIncX, gm.gmCellIncY));
  }

_End:
  DeleteDC(hdc);
  return err;
}

err_t WinFontFace::getTextExtents(const Char* str, sysuint_t length, TextExtents& extents)
{
  TemporaryGlyphSet<128> glyphSet;
  err_t err = getGlyphSet(str, length, glyphSet);

  if (FOG_IS_ERROR(err))
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
  // renderBegin() must be called before.
  FOG_ASSERT(hdc);

  GlyphData* glyphd = NULL;
  ImageData* bitmapd = NULL;

  GLYPHMETRICS gm;
  ZeroMemory(&gm, sizeof(gm));

  MAT2 mat2;

  if (transform.isIdentity())
  {
    mat2 = mat2identity;
  }
  else
  {
    mat2.eM11 = FloatToFIXED(transform._00);
    mat2.eM12 = FloatToFIXED(transform._01);
    mat2.eM21 = FloatToFIXED(transform._10);
    mat2.eM22 = FloatToFIXED(transform._11);
  }

  uint32_t dataSize = GetGlyphOutlineW(hdc, uc, GGO_GRAY8_BITMAP, &gm, 0, NULL, &mat2);

  // GetGlyphOutlineW() fails when being called for GGO_GRAY8_BITMAP and white-space.
  if (dataSize == GDI_ERROR)
  {
    dataSize = GetGlyphOutlineW(hdc, uc, GGO_METRICS, &gm, 0, NULL, &mat2);
    if (dataSize == GDI_ERROR) return NULL;
    dataSize = 0;
  }

  glyphd = fog_new GlyphData();
  if (glyphd == NULL) return NULL;

  // Whitespace?
  if (dataSize == 0) gm.gmBlackBoxX = gm.gmBlackBoxY = 0;

  glyphd->offset.set(gm.gmptGlyphOrigin.x, (int)metrics.getAscent() - gm.gmptGlyphOrigin.y);
  glyphd->beginWidth = 0;
  glyphd->endWidth = 0;

  glyphd->advance = gm.gmCellIncX;
  // glyphd->advanceY = -gm.gmCellIncY;

  // Whitespace? We are done.
  if (dataSize == 0) return glyphd;

  // The dataSize returned by GetGlyphOutlineW() is not always based on gm.bmBlackBoxY,
  // so it's needed to get the correct height using the stride and the returned tataSize.
  int stride = (gm.gmBlackBoxX + 3) & ~3;
  int bmWidth = gm.gmBlackBoxX;
  int bmHeight = dataSize / stride;
     
  // Alloc image for glyph.
  if (glyphd->bitmap.create(SizeI(bmWidth, bmHeight), IMAGE_FORMAT_A8) != ERR_OK)
  {
    fog_delete(glyphd);
    return NULL;
  }
  bitmapd = glyphd->bitmap._d;

  // Fog-Framework have to align stride to 32 bits like Windows does.
  FOG_ASSERT((bitmapd->stride & 0x3) == 0);
  // This should be also equal.
  FOG_ASSERT(dataSize == bitmapd->stride * bitmapd->size.h);

  dataSize = GetGlyphOutlineW(hdc, uc, GGO_GRAY8_BITMAP, &gm, dataSize, bitmapd->data, &mat2);
  // If previous call to GetGlyphOutlineW was ok, this should be also ok, but nobody knows.
  if (dataSize == GDI_ERROR)
  {
    fog_delete(glyphd);
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
      uint8_t p0 = p[0];
      *p++ = (p0 > 63) ? (0xFF) : (p0 << 2) | (p0 & 0x03);
    }
  }

  return glyphd;
}

} // Fog namespace

// [Guard]
#endif // FOG_FONT_WINDOWS
