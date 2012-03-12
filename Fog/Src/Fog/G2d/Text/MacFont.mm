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
#include <Fog/Core/Tools/Logger.h>
#include <Fog/Core/Tools/StringUtil.h>
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/G2d/Text/MacFont.h>
#include <Fog/G2d/Text/OpenType/OTHead.h>
#include <Fog/G2d/Text/OpenType/OTHHea.h>

namespace Fog {

// ============================================================================
// [Globals]
// ============================================================================

static FaceVTable MacFace_vtable;
static FontEngineVTable MacFontEngine_vtable;

static Static<MacFontEngine> MacFontEngine_oInstance;

// ============================================================================
// [Fog::MacFace - Create / Destroy]
// ============================================================================

static void FOG_CDECL MacFace_freeTableData(OTTable* table)
{
  if (table->_handle != NULL)
  {
    CFRelease((CTFontRef)table->_handle);

    table->_handle = NULL;
    table->_data = NULL;
    table->_dataLength = 0;
  }
}

static void FOG_CDECL MacFace_create(MacFace* self, CTFontRef ctFont)
{
  fog_new_p(self) MacFace(&MacFace_vtable, StringW::getEmptyInstance());

  self->ctFont = ctFont;
  self->ot->_freeTableDataFunc = MacFace_freeTableData;
  self->ot->initCoreTables();
}

static void FOG_CDECL MacFace_destroy(Face* self_)
{
  MacFace* self = static_cast<MacFace*>(self_);

  self->~MacFace();
  MemMgr::free(self);
}

// ============================================================================
// [Fog::MacFace - GetTable / ReleaseTable]
// ============================================================================

static OTFace* FOG_CDECL MacFace_getOTFace(const Face* self_)
{
  const MacFace* self = static_cast<const MacFace*>(self_);
  return const_cast<OTFace*>(&self->ot);
}

static OTTable* FOG_CDECL MacFace_getOTTable(const Face* self_, uint32_t tag)
{
  const MacFace* self = static_cast<const MacFace*>(self_);
  OTTable* table;

  // Not needed to synchronize, because we only add into the list using atomic
  // operations.
  table = self->ot->getTable(tag);
  if (table != NULL)
    return table;

  AutoLock locked(MacFontEngine_oInstance->lock());

  // Try to get the table again in case that it was created before we acquired
  // the lock.
  table = self->ot->getTable(tag);
  if (table != NULL)
    return table;

#if defined(FOG_OT_DEBUG)
  Logger::info("Fog::MacFace", "getOTTable",
    "Requested table '%c%c%c%c' not found in the cache, trying to load.",
      (tag >> 24) & 0xFF,
      (tag >> 16) & 0xFF,
      (tag >>  8) & 0xFF,
      (tag      ) & 0xFF);
#endif // FOG_OT_DEBUG

  CFDataRef cfData = CTFontCopyTable(self->ctFont, tag, NULL);
  if (cfData == NULL)
  {
#if defined(FOG_OT_DEBUG)
    Logger::info("Fog::MacFace", "getOTTable",
      "Requested table '%c%c%c%c' not found in the font.",
        (tag >> 24) & 0xFF,
        (tag >> 16) & 0xFF,
        (tag >>  8) & 0xFF,
        (tag      ) & 0xFF);
#endif // FOG_OT_DEBUG
    return NULL;
  }
  else
  {
#if defined(FOG_OT_DEBUG)
    Logger::info("Fog::MacFace", "getOTTable",
      "Requested table '%c%c%c%c' found in the font.",
        (tag >> 24) & 0xFF,
        (tag >> 16) & 0xFF,
        (tag >>  8) & 0xFF,
        (tag      ) & 0xFF);
#endif // FOG_OT_DEBUG
  }

  uint8_t* data = (uint8_t*)CFDataGetBytePtr(cfData);
  uint32_t length = CFDataGetLength(cfData);

  table = const_cast<MacFace*>(self)->ot->addTable(tag, data, length);
  if (FOG_IS_NULL(table))
  {
#if defined(FOG_OT_DEBUG)
    Logger::info("Fog::MacFace", "getOTTable",
      "Failed to add table '%c%c%c%c' to OTFace.",
        (tag >> 24) & 0xFF,
        (tag >> 16) & 0xFF,
        (tag >>  8) & 0xFF,
        (tag      ) & 0xFF);
#endif // FOG_OT_DEBUG

    CFRelease(cfData);
    return NULL;
  }

  table->_handle = (void*)cfData;
  return table;
}

// ============================================================================
// [Fog::MacFace - GetOutlineFromGlyphRun]
// ============================================================================

template<typename NumT>
struct MacFace_CGPathApplierInfo
{
  NumT_(Path)* dst;

  NumT_(Point) scale;
  NumT_(Point) pos;
};

template<typename NumT>
static void MacFace_cgPathApplier(void* info_, const CGPathElement* element)
{
  MacFace_CGPathApplierInfo<NumT>* info = reinterpret_cast< MacFace_CGPathApplierInfo<NumT>* >(info_);

  NumT sx = info->scale.x;
  NumT sy = info->scale.y;

  NumT px = info->pos.x;
  NumT py = info->pos.y;

  switch (element->type)
  {
    case kCGPathElementMoveToPoint:
      info->dst->moveTo(
        NumT(element->points[0].x) * sx + px, NumT(element->points[0].y) * sy + py);
      break;

    case kCGPathElementAddLineToPoint:
      info->dst->lineTo(
        NumT(element->points[0].x) * sx + px, NumT(element->points[0].y) * sy + py);
      break;
      
    case kCGPathElementAddQuadCurveToPoint:
      info->dst->quadTo(
        NumT(element->points[0].x) * sx + px, NumT(element->points[0].y) * sy + py,
        NumT(element->points[1].x) * sx + px, NumT(element->points[1].y) * sy + py);
      break;
      
    case kCGPathElementAddCurveToPoint:
      info->dst->cubicTo(
        NumT(element->points[0].x) * sx + px, NumT(element->points[0].y) * sy + py,
        NumT(element->points[1].x) * sx + px, NumT(element->points[1].y) * sy + py,
        NumT(element->points[2].x) * sx + px, NumT(element->points[2].y) * sy + py);
      break;

    case kCGPathElementCloseSubpath:
      info->dst->close();
      break;
      
    default:
      FOG_ASSERT_NOT_REACHED();
  }
}

template<typename NumT>
static FOG_INLINE err_t MacFace_getOutlineFromGlyphRunT(FontData* d,
  NumT_(Path)* dst, uint32_t cntOp, const NumT_(Point)* pt,
  const uint32_t* glyphList, size_t glyphAdvance,
  const PointF* positionList, size_t positionAdvance,
  size_t length)
{
  CTFontRef ctFont = static_cast<MacFace*>(d->face)->ctFont;
  float scale = d->scale;

  MacFace_CGPathApplierInfo<NumT> info;

  info.dst = dst;
  info.scale.set(NumT(scale), -NumT(scale));

  size_t i;
  for (i = 0; i < length; i++)
  {
    CGGlyph glyphId = glyphList[0];
    info.pos.set(NumT(positionList[0].x) + pt->x, NumT(positionList[0].y) + pt->y);

    CGPathRef cgPath = CTFontCreatePathForGlyph(ctFont, glyphId, NULL);
    if (cgPath != NULL)
    {
      CGPathApply(cgPath, &info, MacFace_cgPathApplier<NumT>);
      CGPathRelease(cgPath);
    }
    
    glyphList = reinterpret_cast<const uint32_t*>((const uint8_t*)glyphList + glyphAdvance);
    positionList = reinterpret_cast<const PointF*>((const uint8_t*)positionList + positionAdvance);
  }
  
  return ERR_OK;
}

static err_t FOG_CDECL MacFace_getOutlineFromGlyphRunF(FontData* d,
  PathF* dst, uint32_t cntOp, const PointF* pt,
  const uint32_t* glyphList, size_t glyphAdvance,
  const PointF* positionList, size_t positionAdvance,
  size_t length)
{
  return MacFace_getOutlineFromGlyphRunT<float>(d,
    dst, cntOp, pt, glyphList, glyphAdvance, positionList, positionAdvance, length);
}

static err_t FOG_CDECL MacFace_getOutlineFromGlyphRunD(FontData* d,
  PathD* dst, uint32_t cntOp, const PointD* pt,
  const uint32_t* glyphList, size_t glyphAdvance,
  const PointF* positionList, size_t positionAdvance,
  size_t length)
{
  return MacFace_getOutlineFromGlyphRunT<double>(d,
    dst, cntOp, pt, glyphList, glyphAdvance, positionList, positionAdvance, length);
}

// ============================================================================
// [Fog::MacFontEngine - Create / Destroy]
// ============================================================================

static void MacFontEngine_create(MacFontEngine* self)
{
  fog_new_p(self) MacFontEngine(&MacFontEngine_vtable);

  self->defaultFaceName->setAscii8(Ascii8("Lucida Grande"));
  self->defaultFont->_d = fog_api.font_oNull->_d->addRef();
}

static void MacFontEngine_destroy(FontEngine* self_)
{
  MacFontEngine* self = static_cast<MacFontEngine*>(self_);

  if (self->defaultFont->_d != NULL)
  {
    self->defaultFont->_d->release();
    self->defaultFont->_d = NULL;
  }

  self->~MacFontEngine();
}

// ============================================================================
// [Fog::MacFontEngine - QueryFace]
// ============================================================================

static FOG_INLINE uint32_t MacFontEngine_score(uint32_t a, uint32_t b)
{
  return static_cast<uint32_t>(Math::abs(int32_t(a) - int32_t(b)));
}

static err_t FOG_CDECL MacFontEngine_queryFace(const FontEngine* self_,
  Face** dst, const StringW* family, const FaceFeatures* features)
{
  *dst = NULL;

  const MacFontEngine* self = static_cast<const MacFontEngine*>(self_);
  AutoLock locked(self->lock());

  MacFace* face = static_cast<MacFace*>(self->cache->getExactFace(*family, *features));
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
    cDiff += MacFontEngine_score(cFeatures.getStretch(), features->getStretch());
    cDiff += MacFontEngine_score(cFeatures.getWeight(), features->getWeight()) * 2;

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

  face = static_cast<MacFace*>(self->cache->getExactFace(*family, *features));
  if (face != NULL)
  {
    *dst = face;
    return ERR_OK;
  }

  CFStringRef ctFaceFamily;
  FOG_RETURN_ON_ERROR(family->toCFString(&ctFaceFamily));

  CTFontRef ctFace = CTFontCreateWithName(ctFaceFamily, 128.0f, NULL);
  if (ctFace == NULL)
  {
    CFRelease(ctFaceFamily);
    return ERR_FONT_NOT_MATCHED;
  }

  // Because we don't know the design metrics of the font, we read the font
  // table where the metrics is stored and then recreate the font using design
  // units. I found that this is the best solution to deal with fonts on MAC.
  CFDataRef ctFaceHead = CTFontCopyTable(ctFace, kCTFontTableHead, NULL);
  int unitsPerEm = 0;

  // If the 'head' section hasn't been found or it's corrupted (I don't know
  // whether it's possible when loading through MAC) then we don't allow to
  // create it.
  if (ctFaceHead == NULL || CFDataGetLength(ctFaceHead) < sizeof(OTHeadHeader))
  {
    CFRelease(ctFaceHead);
    CFRelease(ctFaceFamily);
    CFRelease(ctFace);

    return ERR_FONT_NOT_MATCHED;
  }
  else
  {
    const OTHeadHeader* header = reinterpret_cast<const OTHeadHeader*>(
      CFDataGetBytePtr(ctFaceHead));

    unitsPerEm = header->unitsPerEm.getValueA();
    
    CFRelease(ctFaceHead);
    CFRelease(ctFace);
  }

  // Okay, now we can create CTFontRef which we can store and use.
  ctFace = CTFontCreateWithName(ctFaceFamily, float(unitsPerEm), NULL);
  CFRelease(ctFaceFamily);

  if (ctFace == NULL)
    return ERR_FONT_NOT_MATCHED;

  CFStringRef cfFontName = CTFontCopyFullName(ctFace);
  StringW fontNameW;

  err_t err = fontNameW.fromCFString(cfFontName);
  CFRelease(cfFontName);

  if (FOG_IS_ERROR(err))
  {
    CFRelease(ctFace);
    return err;
  }

  face = static_cast<MacFace*>(MemMgr::alloc(sizeof(MacFace)));
  if (FOG_IS_NULL(face))
  {
    CFRelease(ctFace);
    return ERR_RT_OUT_OF_MEMORY;
  }

  MacFace_create(face, ctFace);
  FontMetrics& fm = face->designMetrics;
  
  OTHHea* hhea = face->ot->getHHea();
  if (hhea != NULL)
  {
    const OTHHeaHeader* header = hhea->getHeader();

    fm._size = float(unitsPerEm);
    fm._ascent = float(header->ascender.getValueA());
    fm._descent = -float(header->descender.getValueA());
    fm._lineGap =float(header->lineGap.getValueA());
    fm._lineSpacing = fm._ascent + fm._descent + fm._lineGap;
  }

  self->cache->put(*family, bestFeatures, face);

  *dst = face;
  return ERR_OK;
}

// ============================================================================
// [Fog::MacFontEngine - GetAvailableFaces]
// ============================================================================

static err_t FOG_CDECL MacFontEngine_getAvailableFaces(const FontEngine* self_,
  FaceCollection* dst)
{
  const MacFontEngine* self = static_cast<const MacFontEngine*>(self_);
  AutoLock locked(self->lock());

  return dst->setCollection(self->faceCollection());
}

// ============================================================================
// [Fog::MacFontEngine - UpdateAvailableFaces]
// ============================================================================

static err_t FOG_CDECL MacFontEngine_updateAvailableFaces(MacFontEngine* self)
{
  AutoLock locked(self->lock());

  FaceCollection* collection = &self->faceCollection;
  collection->clear();

#if defined(FOG_OS_IOS)
  NSArray* nsArray = [UIFont familyNames];
  err_t err = ERR_OK;

  for (NSString* familyName in nsArray)
  {
    StringW familyNameW;
    err = familyNameW.fromNSString(familyName);
    
    if (FOG_IS_ERROR(err))
      break;

    FaceInfo faceInfo;
    faceInfo.setFamilyName(familyNameW);

    collection->addItem(faceInfo);
  }

  return err;
#else
  CFArrayRef cfArray = CTFontManagerCopyAvailableFontFamilyNames();
  if (cfArray == NULL)
    return ERR_RT_OUT_OF_MEMORY;

  CFIndex i, length = CFArrayGetCount(cfArray);
  err_t err = ERR_OK;

  for (i = 0; i < length; i++)
  {
    StringW familyNameW;
    err = familyNameW.fromCFString((CFStringRef)CFArrayGetValueAtIndex(cfArray, i));
    
    if (FOG_IS_ERROR(err))
      break;

    FaceInfo faceInfo;
    faceInfo.setFamilyName(familyNameW);

    collection->addItem(faceInfo);
  }

  CFRelease(cfArray);
  return err;
#endif // FOG_OS_IOS

  return ERR_OK;
}

// ============================================================================
// [Fog::MacFontEngine - GetDefaultFace]
// ============================================================================

static err_t FOG_CDECL MacFontEngine_getDefaultFace(const FontEngine* self_,
  FaceInfo* dst)
{
  const MacFontEngine* self = static_cast<const MacFontEngine*>(self_);
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
// [Fog::MacFontEngine - SetupDefaultFace]
// ============================================================================

static err_t FOG_CDECL MacFontEngine_setupDefaultFace(MacFontEngine* self)
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

FOG_NO_EXPORT void Font_init_mac(void)
{
  // --------------------------------------------------------------------------
  // [MacFace / MacFontEngine]
  // --------------------------------------------------------------------------

  MacFace_vtable.destroy = MacFace_destroy;
  MacFace_vtable.getOTFace = MacFace_getOTFace;
  MacFace_vtable.getOTTable = MacFace_getOTTable;
  MacFace_vtable.getOutlineFromGlyphRunF = MacFace_getOutlineFromGlyphRunF;
  MacFace_vtable.getOutlineFromGlyphRunD = MacFace_getOutlineFromGlyphRunD;

  MacFontEngine_vtable.destroy = MacFontEngine_destroy;
  MacFontEngine_vtable.getAvailableFaces = MacFontEngine_getAvailableFaces;
  MacFontEngine_vtable.getDefaultFace = MacFontEngine_getDefaultFace;
  MacFontEngine_vtable.queryFace = MacFontEngine_queryFace;
  
  MacFontEngine* engine = &MacFontEngine_oInstance;
  MacFontEngine_create(engine);

  fog_api.fontengine_oGlobal = engine;
  MacFontEngine_updateAvailableFaces(engine);
  MacFontEngine_setupDefaultFace(engine);
}

} // Fog namespace
