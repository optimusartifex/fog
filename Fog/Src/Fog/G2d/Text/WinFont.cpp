// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Global/Private.h>
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/Memory/MemBufferTmp_p.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/OS/OSUtil.h>
#include <Fog/Core/Tools/StringUtil.h>
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/G2d/Text/WinFont.h>

// [Fix]
#ifndef CLEARTYPE_QUALITY
# define CLEARTYPE_QUALITY 5
#endif // CLEARTYPE_QUALITY

#ifndef GGO_GLYPH_INDEX
# define GGO_GLYPH_INDEX 0x0080
#endif // GGO_GLYPH_INDEX

#ifndef GGO_UNHINTED
# define GGO_UNHINTED 0x0100
#endif // GGO_UNHINTED

#ifndef TT_PRIM_CSPLINE
# define TT_PRIM_CSPLINE 3
#endif // TT_PRIM_CSPLINE

namespace Fog {

// ============================================================================
// [Globals]
// ============================================================================

static FaceVTable WinFace_vtable;
static FontEngineVTable WinFontEngine_vtable;

static Static<WinFontEngine> WinFontEngine_oInstance;

// ============================================================================
// [Fog::WinFontHDC]
// ============================================================================

struct FOG_NO_EXPORT WinFontHDC
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE WinFontHDC()
  {
    hdc = NULL;
  }

  FOG_INLINE ~WinFontHDC()
  {
    if (hdc != NULL)
      ::DeleteDC(hdc);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isInitialized() const
  {
    return hdc != NULL;
  }

  FOG_INLINE bool init()
  {
    FOG_ASSERT(!isInitialized());
    hdc = ::CreateCompatibleDC(NULL);

    if (hdc == NULL)
      return false;

    ::SetGraphicsMode(hdc, GM_ADVANCED);
    return true;
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE operator HDC() const { return hdc; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  HDC hdc;

private:
  _FOG_NO_COPY(WinFontHDC)
};

// ============================================================================
// [Fog::WinGetGlyphOutlineHDC]
// ============================================================================

struct FOG_NO_EXPORT WinGetGlyphOutlineHDC
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE WinGetGlyphOutlineHDC()
  {
    hdc = NULL;
    hOldFont = NULL;
  }

  FOG_INLINE ~WinGetGlyphOutlineHDC()
  {
    if (hOldFont != NULL)
      ::SelectObject(hdc, hOldFont);

    if (hdc != NULL)
      ::DeleteDC(hdc);
  }

  // --------------------------------------------------------------------------
  // [Init]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isHDCInitialized() const
  {
    return hdc != NULL;
  }

  FOG_INLINE bool isHFONTInitialized() const
  {
    return hOldFont != NULL;
  }

  FOG_INLINE err_t initHDC()
  {
    FOG_ASSERT(hdc == NULL);
    hdc = ::CreateCompatibleDC(NULL);

    if (hdc == NULL)
      return ERR_RT_OUT_OF_MEMORY;

    ::SetGraphicsMode(hdc, GM_ADVANCED);
    return ERR_OK;
  }

  FOG_INLINE err_t initHFONT(HFONT hFace)
  {
    FOG_ASSERT(hOldFont == NULL);
    hOldFont = ::SelectObject(hdc, (HGDIOBJ)hFace);

    if (hOldFont == NULL)
      return ERR_FONT_INTERNAL;

    return ERR_OK;
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE operator HDC() const { return hdc; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  HDC hdc;
  HGDIOBJ hOldFont;
};

// ============================================================================
// [Fog::WinFace - Create / Destroy]
// ============================================================================

static void FOG_CDECL WinFace_freeTable(OT_Table* table)
{
  MemMgr::free(table->getData());
}

static void FOG_CDECL WinFace_create(WinFace* self, HFONT hFace)
{
  fog_new_p(self) WinFace(&WinFace_vtable, StringW::getEmptyInstance());

  self->hFace = hFace;
  self->ot->_freeTable = WinFace_freeTable;
}

static void FOG_CDECL WinFace_destroy(Face* self_)
{
  WinFace* self = static_cast<WinFace*>(self_);

  self->~WinFace();
  MemMgr::free(self);
}

// ============================================================================
// [Fog::WinFace - GetTable / ReleaseTable]
// ============================================================================

static OT_Table* FOG_CDECL WinFace_getTable(const Face* self_, uint32_t tag)
{
  const WinFace* self = static_cast<const WinFace*>(self_);
  OT_Table* table;

  // Not needed to synchronize, because we only add into the list using atomic
  // operations.
  table = self->ot->getTable(tag);
  if (table != NULL)
    return table;

  AutoLock locked(WinFontEngine_oInstance->lock());

  // Try to get the table again in case that it was created before we acquired
  // the lock.
  table = self->ot->getTable(tag);
  if (table != NULL)
    return table;

  WinFontHDC scopedDC;
  if (!scopedDC.init())
    return NULL;

  HGDIOBJ oldFont = ::SelectObject(scopedDC, (HGDIOBJ)self->hFace);
  if (oldFont == (HGDIOBJ)GDI_ERROR)
    return NULL;

  uint8_t* data;
  DWORD length = ::GetFontData(scopedDC, MemOps::bswap32be(tag), 0, NULL, 0);

  if (length == GDI_ERROR)
    goto _End;

  data = static_cast<uint8_t*>(MemMgr::alloc(length));
  if (FOG_IS_NULL(data))
    goto _End;

  if (::GetFontData(scopedDC, MemOps::bswap32be(tag), 0, data, length) != length)
  {
    MemMgr::free(data);
    goto _End;
  }

  table = const_cast<WinFace*>(self)->ot->addTable(tag, data, length);
  if (FOG_IS_NULL(table))
  {
    MemMgr::free(data);
    goto _End;
  }

_End:
  ::SelectObject(scopedDC, oldFont);
  return table;
}

static void FOG_CDECL WinFace_releaseTable(const Face* self_, OT_Table* table)
{
  // WinFace caches all tables, there is nothing to do.
  FOG_ASSERT(static_cast<const WinFace*>(self_)->ot->hasTable(table));
}

// ============================================================================
// [Fog::WinFace - GetOutlineFromGlyphRun]
// ============================================================================

static FOG_INLINE FIXED WinFace_Fixed0() { FIXED fx; fx.value = 0; fx.fract = 0; return fx; }
static FOG_INLINE FIXED WinFace_Fixed1() { FIXED fx; fx.value = 1; fx.fract = 0; return fx; }

static FOG_INLINE FIXED WinFace_getFIXEDFrom16x16(uint32_t v16x16)
{
  FIXED fx;
  fx.value = v16x16 >> 16;
  fx.fract = v16x16 & 0xFFFF;
  return fx;
}

template<typename NumT>
static FOG_INLINE FIXED WinFace_getFIXEDFromFloat(const NumT& d)
{
  return WinFace_getFIXEDFrom16x16(Math::fixed16x16FromFloat(d));
}

template<typename NumT>
static FOG_INLINE NumT WinFace_getFloatFromFIXED(const FIXED& f)
{
  return NumT(f.value) + NumT(f.fract) * NumT(1.0 / 65536.0);
}

template<typename NumT>
static err_t WinFace_decomposeTTOutline(NumT_(Path)* path,
  const uint8_t* glyphBuffer, size_t glyphSize, bool flipY)
{
  err_t err = ERR_OK;

  const uint8_t* glyphCur = glyphBuffer;
  const uint8_t* glyphEnd = glyphBuffer + glyphSize;

  if (glyphCur == glyphEnd)
    return ERR_OK;

  NumT_(Point) p1(UNINITIALIZED);

  do {
    const TTPOLYGONHEADER* polyHeader = (TTPOLYGONHEADER*)glyphCur;

    const uint8_t* polyEnd = glyphCur + polyHeader->cb;
    const uint8_t* polyCur = glyphCur + sizeof(TTPOLYGONHEADER);

    p1.x = WinFace_getFloatFromFIXED<NumT>(polyHeader->pfxStart.x);
    p1.y = WinFace_getFloatFromFIXED<NumT>(polyHeader->pfxStart.y);
    if (flipY) p1.y = -p1.y;

    path->moveTo(p1);

    while (polyCur < polyEnd)
    {
      const TTPOLYCURVE* pc = (const TTPOLYCURVE*)polyCur;

      switch (pc->wType)
      {
        case TT_PRIM_LINE:
        {
          for (uint i = 0; i < pc->cpfx; i++)
          {
            p1.x = WinFace_getFloatFromFIXED<NumT>(pc->apfx[i].x);
            p1.y = WinFace_getFloatFromFIXED<NumT>(pc->apfx[i].y);
            if (flipY) p1.y = -p1.y;

            path->lineTo(p1);
          }
          break;
        }

        case TT_PRIM_QSPLINE:
        {
          for (uint i = 0; i < (uint)pc->cpfx - 1; i++)
          {
            NumT_(Point) p2(UNINITIALIZED);

            // B is always the current point.
            p1.x = WinFace_getFloatFromFIXED<NumT>(pc->apfx[i].x);
            p1.y = WinFace_getFloatFromFIXED<NumT>(pc->apfx[i].y);

            p2.x = WinFace_getFloatFromFIXED<NumT>(pc->apfx[i + 1].x);
            p2.y = WinFace_getFloatFromFIXED<NumT>(pc->apfx[i + 1].y);

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

            path->quadTo(p1, p2);
          }
          break;
        }

        case TT_PRIM_CSPLINE:
        {
          for (uint i = 0; i < (uint)pc->cpfx - 1; i++)
          {
            NumT_(Point) p2(UNINITIALIZED);
            NumT_(Point) p3(UNINITIALIZED);

            p1.x = WinFace_getFloatFromFIXED<NumT>(pc->apfx[i].x);
            p1.y = WinFace_getFloatFromFIXED<NumT>(pc->apfx[i].y);

            p2.x = WinFace_getFloatFromFIXED<NumT>(pc->apfx[i + 1].x);
            p2.y = WinFace_getFloatFromFIXED<NumT>(pc->apfx[i + 1].y);

            p3.x = WinFace_getFloatFromFIXED<NumT>(pc->apfx[i + 2].x);
            p3.y = WinFace_getFloatFromFIXED<NumT>(pc->apfx[i + 2].y);

            if (flipY)
            {
              p1.y = -p1.y;
              p2.y = -p2.y;
              p3.y = -p3.y;
            }

            path->cubicTo(p1, p2, p3);
          }
          break;
        }

        default:
          path->close();
          return err;
      }

      polyCur += sizeof(WORD) * 2 + sizeof(POINTFX) * pc->cpfx;
    }

    path->close();
    glyphCur += polyHeader->cb;
  } while (glyphCur < glyphEnd);

  return err;
}

template<typename NumT>
static FOG_INLINE err_t WinFace_getOutlineFromGlyphRunT(FontData* d,
  NumT_(Path)* dst, uint32_t cntOp,
  const uint32_t* glyphList, size_t glyphAdvance,
  const PointF* positionList, size_t positionAdvance,
  size_t length)
{
  WinFace* face = static_cast<WinFace*>(d->face);

  if (length == 0)
  {
    if (cntOp == CONTAINER_OP_REPLACE)
      dst->clear();
    return ERR_OK;
  }

  // Build the transform.
  NumT_(Transform) transform;

  transform.scale(
    NumT_(Point)(d->scale, d->scale));
  transform.transform(
    NumT_(Transform)(d->matrix._xx, d->matrix._yx, d->matrix._yx, d->matrix._yy, 0.0f, 0.0f));

  if (transform.getType() == TRANSFORM_TYPE_IDENTITY)
    transform._type = TRANSFORM_TYPE_TRANSLATION;

  err_t err = ERR_OK;
  size_t i;

  WinGetGlyphOutlineHDC scopedDC;
  MemBufferTmp<1024> buffer;

  FOG_RETURN_ON_ERROR(scopedDC.initHDC());
  FOG_RETURN_ON_ERROR(scopedDC.initHFONT(face->hFace));

  MAT2 mat2;
  GLYPHMETRICS gm;

  mat2.eM11 = WinFace_Fixed1(); mat2.eM12 = WinFace_Fixed0();
  mat2.eM21 = WinFace_Fixed0(); mat2.eM22 = WinFace_Fixed1();

  for (i = 0; i < length; i++)
  {
_Repeat:
    DWORD dataSize = ::GetGlyphOutlineW(scopedDC, glyphList[0],
      GGO_GLYPH_INDEX | GGO_NATIVE | GGO_UNHINTED,
      &gm, (DWORD)buffer.getCapacity(), buffer.getMem(), &mat2);

    if (dataSize == GDI_ERROR)
      return ERR_FONT_INTERNAL;

    if (static_cast<size_t>(dataSize) > buffer.getCapacity())
    {
      // If we need to realloc the temporary buffer, reserve more space so
      // we don't do that again. We don't care about memory here, because
      // it will be freed after this loop ends.
      dataSize = (dataSize + 4095) & ~4095;

      if (FOG_IS_NULL(buffer.alloc(dataSize)))
        return ERR_RT_OUT_OF_MEMORY;

      goto _Repeat;
    }

    size_t index = dst->getLength();
    FOG_RETURN_ON_ERROR(WinFace_decomposeTTOutline<NumT>(dst,
      reinterpret_cast<uint8_t*>(buffer.getMem()), dataSize, true));

    transform._20 = positionList[0].x;
    transform._21 = positionList[0].y;
    FOG_RETURN_ON_ERROR(dst->transform(transform, Range(index, DETECT_LENGTH)));

    glyphList = (const uint32_t*)((const uint8_t*)glyphList + glyphAdvance);
    positionList = (const PointF*)((const uint8_t*)positionList + positionAdvance);
  }

  return ERR_OK;
}

static err_t FOG_CDECL WinFace_getOutlineFromGlyphRunF(FontData* d,
  PathF* dst, uint32_t cntOp,
  const uint32_t* glyphList, size_t glyphAdvance,
  const PointF* positionList, size_t positionAdvance,
  size_t length)
{
  return WinFace_getOutlineFromGlyphRunT<float>(d,
    dst, cntOp, glyphList, glyphAdvance, positionList, positionAdvance, length);
}

static err_t FOG_CDECL WinFace_getOutlineFromGlyphRunD(FontData* d,
  PathD* dst, uint32_t cntOp,
  const uint32_t* glyphList, size_t glyphAdvance,
  const PointF* positionList, size_t positionAdvance,
  size_t length)
{
  return WinFace_getOutlineFromGlyphRunT<double>(d,
    dst, cntOp, glyphList, glyphAdvance, positionList, positionAdvance, length);
}

// ============================================================================
// [Fog::WinFontEngine - Create / Destroy]
// ============================================================================

static void WinFontEngine_create(WinFontEngine* self)
{
  fog_new_p(self) WinFontEngine(&WinFontEngine_vtable);
  self->defaultFaceName->setAscii8(Ascii8("Times New Roman"));
}

static void WinFontEngine_destroy(FontEngine* self_)
{
  WinFontEngine* self = static_cast<WinFontEngine*>(self_);

  if (self->defaultFont->_d != NULL)
  {
    self->defaultFont->_d->release();
    self->defaultFont->_d = NULL;
  }

  self->~WinFontEngine();
}

// ============================================================================
// [Fog::WinFontEngine - QueryFace]
// ============================================================================

static FOG_INLINE uint32_t WinFontEngine_score(uint32_t a, uint32_t b)
{
  return static_cast<uint32_t>(Math::abs(int32_t(a) - int32_t(b)));
}

static err_t FOG_CDECL WinFontEngine_queryFace(const FontEngine* self_,
  Face** dst, const StringW* family, const FaceFeatures* features)
{
  const WinFontEngine* self = static_cast<const WinFontEngine*>(self_);
  AutoLock locked(self->lock());

  WinFace* face = static_cast<WinFace*>(self->cache->getExactFace(*family, *features));
  if (face != NULL)
  {
    *dst = face;
    return ERR_OK;
  }

  Range range = self->faceCollection->getFamilyRange(*family);
  if (!range.isValid())
    return ERR_FONT_NOT_MATCHED;

  const FaceInfo* pInfo = self->faceCollection->getList().getData();
  const FaceInfo* pEnd = pInfo;

  pInfo += range.getStart();
  pEnd += range.getEnd();

  const FaceInfo* bestInfo = NULL;
  uint32_t bestDiff = UINT32_MAX;
  FaceFeatures bestFeatures;

  uint32_t isItalic = features->getItalic();

  do {
    FaceFeatures cFeatures = pInfo->_d->features;
    uint32_t cDiff = 0;

    // If the requested font is not italic, but the font in FontInfo is,
    // then we setup the biggest possible difference. The opposite way
    // is not a problem, because we can switch to oblique style.
    if (cFeatures.getItalic() && !isItalic)
      cDiff |= 0x80000000;

    // Stretch makes bigger difference than weight. We can tune this later.
    cDiff += WinFontEngine_score(cFeatures.getStretch(), features->getStretch());
    cDiff += WinFontEngine_score(cFeatures.getWeight(), features->getWeight()) * 2;

    if (cDiff < bestDiff)
    {
      bestDiff = cDiff;
      bestFeatures = cFeatures;
      bestInfo = pInfo;

      // Exact match.
      if (cDiff == 0)
        break;
    }
  } while (++pInfo != pEnd);

  if (bestInfo == NULL)
    return ERR_FONT_NOT_MATCHED;

  face = static_cast<WinFace*>(self->cache->getExactFace(*family, *features));
  if (face != NULL)
  {
    *dst = face;
    return ERR_OK;
  }

  LOGFONTW lf;
  ZeroMemory(&lf, sizeof(LOGFONTW));

  MemOps::copy(lf.lfFaceName, family->getData(),
    Math::min(family->getLength(), FOG_ARRAY_SIZE(lf.lfFaceName)));
  lf.lfItalic = bestFeatures.getItalic();
  lf.lfWeight = bestFeatures.getWeight() * 10;

  HFONT hFace = ::CreateFontIndirectW(&lf);
  if (FOG_IS_NULL(hFace))
    return OSUtil::getErrFromOSLastError();

  face = static_cast<WinFace*>(MemMgr::alloc(sizeof(WinFace)));
  if (FOG_IS_NULL(face))
  {
    ::DeleteObject(hFace);
    return ERR_RT_OUT_OF_MEMORY;
  }

  WinFace_create(face, hFace);
  self->cache->put(*family, bestFeatures, face);

  *dst = face;
  return ERR_OK;
}

// ============================================================================
// [Fog::WinFontEngine - GetAvailableFaces]
// ============================================================================

static err_t FOG_CDECL WinFontEngine_getAvailableFaces(const FontEngine* self_,
  FaceCollection* dst)
{
  const WinFontEngine* self = static_cast<const WinFontEngine*>(self_);
  AutoLock locked(self->lock());

  return dst->setCollection(self->faceCollection());
}

// ============================================================================
// [Fog::WinFontEngine - UpdateAvailableFaces]
// ============================================================================

struct FOG_NO_EXPORT WinFontEngine_UpdateFacesData
{
  FaceCollection* collection;
  FaceInfo* lastFace;
};

static int CALLBACK WinFontEngine_updateAvailableFaces_onEnumProc(
  const LOGFONTW* plf, const TEXTMETRICW* ptm, DWORD fontType, LPARAM lParam)
{
  WinFontEngine_UpdateFacesData* data = (WinFontEngine_UpdateFacesData*)lParam;

  // Accept only true-type fonts.
  if ((fontType & TRUETYPE_FONTTYPE) == 0)
    return 1;

  // Ptm can be retyped to NEWTEXTMETRICEXW structure on TrueType font.
  const NEWTEXTMETRICEXW* pntm = reinterpret_cast<const NEWTEXTMETRICEXW*>(ptm);

  // Reject '@', which is used for fonts which contain support for vertical text.
  if (plf->lfFaceName[0] == L'@')
    return 1;

  StubW faceName((const CharW*)plf->lfFaceName,
    StringUtil::nlen((const CharW*)plf->lfFaceName, FOG_ARRAY_SIZE(plf->lfFaceName)));

  FaceFeatures faceFeatures(
    plf->lfWeight / 10,
    FONT_STRETCH_NORMAL,
    (bool)plf->lfItalic);

  // Cache lookup of the same font.
  FaceCollection* collection = data->collection;
  FaceInfo* face = data->lastFace;

  if (face == NULL || face->getFamilyName() != faceName || face->getFeatures() != faceFeatures)
  {
    // Not cached, try to get the font-face if it's in the collection first.
    size_t index = collection->indexOf(faceName, faceFeatures);

    if (index != INVALID_INDEX)
    {
      face = collection->_d->faceList->getDataX() + index;
    }
    else
    {
      // Font is not in collection, we need to add it there.
      FaceInfo item;

      item.setFamilyName(StringW(faceName));
      item.setFeatures(faceFeatures);
      item.setDesignMetrics(
        FaceDesignMetrics(pntm->ntmTm.ntmSizeEM, pntm->ntmTm.ntmCellHeight));

      if (collection->addItem(item, &index) == ERR_OK)
      {
        face = collection->_d->faceList->getDataX() + index;
        data->lastFace = face;
      }
      else
      {
        // Shouldn't happen. We called functions on collection, so we must
        // ensure that 'lastFace' is clear, because it could become a dangling
        // pointer in case that addItem() caused realloc of that container.
        data->lastFace = NULL;
        return 1;
      }
    }
  }

  // Continue listing.
  return 1;
}

static void WinFontEngine_debugAvailableFaces(WinFontEngine* self)
{
  StringW buf;

  ListIterator<FaceInfo> it(self->faceCollection->getList());
  while (it.isValid())
  {
    const FaceInfo& fi = it.getItem();
    buf.appendFormat("%S [W=%u|S=%u%s]",
      fi.getFamilyName().getData(),
      fi.getFeatures().getWeight() * 10,
      fi.getFeatures().getStretch() * 10,
      fi.getFeatures().getItalic() ? "|Italic" : "");
    
    buf.append(Ascii8(", "));
    it.next();
  }

  MessageBoxW(NULL, (WCHAR*)buf.getData(), L"Fog::WinFontEngine::updateAvailableFaces() - Debug.", MB_OK);
}

static err_t FOG_CDECL WinFontEngine_updateAvailableFaces(WinFontEngine* self)
{
  AutoLock locked(self->lock());

  WinFontHDC scopedDC;
  if (!scopedDC.init())
    return ERR_RT_OUT_OF_MEMORY;

  LOGFONTW logFont;
  ZeroMemory(&logFont, sizeof(LOGFONTW));
  logFont.lfOutPrecision = OUT_OUTLINE_PRECIS;

  WinFontEngine_UpdateFacesData data;
  data.collection = &self->faceCollection;
  data.collection->clear();
  data.lastFace = NULL;

  ::EnumFontFamiliesExW(scopedDC, &logFont, 
    WinFontEngine_updateAvailableFaces_onEnumProc, (LPARAM)&data, 0); 

  // Uncomment if something goes wrong.
  WinFontEngine_debugAvailableFaces(self);

  return ERR_OK;
}

// ============================================================================
// [Fog::WinFontEngine - GetDefaultFace]
// ============================================================================

static err_t FOG_CDECL WinFontEngine_getDefaultFace(const FontEngine* self_,
  FaceInfo* dst)
{
  const WinFontEngine* self = static_cast<const WinFontEngine*>(self_);
  AutoLock locked(self->lock());

  FontData* d = self->defaultFont->_d;
  FaceFeatures features;

  features.setWeight(d->features.getWeight());
  features.setStretch(d->features.getStretch());
  features.setItalic(d->features.getStyle() == FONT_STYLE_ITALIC);

  dst->setFamilyName(d->face->family);
  dst->setFileName(StringW::getEmptyInstance());
  dst->setFeatures(features);

  return ERR_OK;
}

// ============================================================================
// [Fog::WinFontEngine - SetupDefaultFace]
// ============================================================================

static err_t FOG_CDECL WinFontEngine_setupDefaultFace(WinFontEngine* self)
{
  FaceFeatures features(FONT_WEIGHT_NORMAL, FONT_STRETCH_NORMAL, false);
  Face* face;

  StringW name = self->defaultFaceName;
  float size = 12.0f;

  FOG_RETURN_ON_ERROR(self->queryFace(&face, name, features));

  Static<Font> font;
  font.init();

  err_t err = font->_init(face, size, FontFeatures(), FontMatrix());
  if (FOG_IS_ERROR(err))
  {
    font.destroy();
    face->release();
    return err;
  }
  else
  {
    self->defaultFont->_d = font->_d;
    return ERR_OK;
  }
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Font_init_win(void)
{
  // --------------------------------------------------------------------------
  // [WinFace / WinFontEngine]
  // --------------------------------------------------------------------------

  WinFace_vtable.destroy = WinFace_destroy;
  WinFace_vtable.getTable = WinFace_getTable;
  WinFace_vtable.releaseTable = WinFace_releaseTable;
  WinFace_vtable.getOutlineFromGlyphRunF = WinFace_getOutlineFromGlyphRunF;
  WinFace_vtable.getOutlineFromGlyphRunD = WinFace_getOutlineFromGlyphRunD;

  WinFontEngine_vtable.destroy = WinFontEngine_destroy;
  WinFontEngine_vtable.getAvailableFaces = WinFontEngine_getAvailableFaces;
  WinFontEngine_vtable.getDefaultFace = WinFontEngine_getDefaultFace;
  WinFontEngine_vtable.queryFace = WinFontEngine_queryFace;
  
  WinFontEngine* engine = &WinFontEngine_oInstance;
  WinFontEngine_create(engine);

  fog_api.fontengine_oGlobal = engine;
  WinFontEngine_updateAvailableFaces(engine);
  WinFontEngine_setupDefaultFace(engine);
}

} // Fog namespace
