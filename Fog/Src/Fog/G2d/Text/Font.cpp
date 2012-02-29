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
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Tools/Hash.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Text/Font.h>

namespace Fog {

// ============================================================================
// [Fog::FontInfo - Global]
// ============================================================================

static Static<FontInfoData> FontInfo_dNull;
static Static<FontInfo> FontInfo_oNull;

// ============================================================================
// [Fog::FontInfo - Construction / Destruction]
// ============================================================================

static void FOG_CDECL FontInfo_ctor(FontInfo* self)
{
  self->_d = FontInfo_dNull->addRef();
}

static void FOG_CDECL FontInfo_ctorCopy(FontInfo* self, const FontInfo* other)
{
  self->_d = other->_d->addRef();
}

static void FOG_CDECL FontInfo_dtor(FontInfo* self)
{
  self->_d->release();
}

// ============================================================================
// [Fog::FontInfo - Sharing]
// ============================================================================

static err_t FOG_CDECL FontInfo_detach(FontInfo* self)
{
  FontInfoData* d = self->_d;
  if (d->reference.get() == 1)
    return ERR_OK;

  FontInfoData* newd = fog_api.fontinfo_dCreate(&d->familyName, &d->fileName);
  if (FOG_IS_NULL(newd))
    return ERR_RT_OUT_OF_MEMORY;

  atomicPtrXchg(&self->_d, newd)->release();
  return ERR_OK;
}

// ============================================================================
// [Fog::FontInfo - Accessors]
// ============================================================================

static err_t FOG_CDECL FontInfo_setDefs(FontInfo* self, const FontDefs* defs)
{
  uint32_t defsPacked = defs->_packed;
  if (self->_d->defs._packed == defsPacked)
    return ERR_OK;

  FOG_RETURN_ON_ERROR(self->detach());
  FontInfoData* d = self->_d;

  d->defs._packed = defsPacked;
  return ERR_OK;
}

static err_t FOG_CDECL FontInfo_setFamilyName(FontInfo* self, const StringW* familyName)
{
  FOG_RETURN_ON_ERROR(self->detach());
  FontInfoData* d = self->_d;

  return d->familyName->set(*familyName);
}

static err_t FOG_CDECL FontInfo_setFileName(FontInfo* self, const StringW* fileName)
{
  FOG_RETURN_ON_ERROR(self->detach());
  FontInfoData* d = self->_d;

  return d->fileName->set(*fileName);
}

// ============================================================================
// [Fog::FontInfo - Reset]
// ============================================================================

static void FOG_CDECL FontInfo_reset(FontInfo* self)
{
  if (self->_d == &FontInfo_dNull)
    return;
  atomicPtrXchg(&self->_d, FontInfo_dNull->addRef())->release();
}

// ============================================================================
// [Fog::FontInfo - Copy]
// ============================================================================

static err_t FOG_CDECL FontInfo_copy(FontInfo* self, const FontInfo* other)
{
  atomicPtrXchg(&self->_d, other->_d->addRef())->release();
  return ERR_OK;
}

// ============================================================================
// [Fog::FontInfo - Eq]
// ============================================================================

static bool FOG_CDECL FontInfo_eq(const FontInfo* a, const FontInfo* b)
{
  const FontInfoData* a_d = a->_d;
  const FontInfoData* b_d = b->_d;

  if (a_d == b_d)
    return true;

  if (a_d->familyName() == b_d->familyName())
    return false;
  if (a_d->fileName() != b_d->fileName())
    return false;

  return true;
}

// ============================================================================
// [Fog::FontInfo - Compare]
// ============================================================================

static int FOG_CDECL FontInfo_compare(const FontInfo* a, const FontInfo* b)
{
  const FontInfoData* a_d = a->_d;
  const FontInfoData* b_d = b->_d;

  if (a_d == b_d)
    return true;

  int c;

  c = a_d->familyName().compare(b_d->familyName());
  if (c != 0)
    return c;

  c = a_d->fileName().compare(b_d->fileName());
  if (c != 0)
    return c;

  return true;
}

// ============================================================================
// [Fog::FontInfo - Data]
// ============================================================================

static FontInfoData* FOG_CDECL FontInfo_dCreate(const StringW* familyName, const StringW* fileName)
{
  FontInfoData* d = static_cast<FontInfoData*>(MemMgr::alloc(sizeof(FontInfoData)));

  if (FOG_IS_NULL(d))
    return NULL;

  d->reference.init(1);
  d->vType = VAR_TYPE_FONT_INFO | VAR_FLAG_NONE;
  d->defs = FontDefs(FONT_WEIGHT_NORMAL, FONT_STRETCH_NORMAL, false);
  d->familyName.initCustom1(*familyName);
  d->fileName.initCustom1(*fileName);

  return d;
}

static void FOG_CDECL FontInfo_dFree(FontInfoData* d)
{
  d->familyName.destroy();
  d->fileName.destroy();

  MemMgr::free(d);
}

// ============================================================================
// [Fog::FontCollection - Global]
// ============================================================================

static Static<FontCollectionData> FontCollection_dNull;
static Static<FontCollection> FontCollection_oNull;

// ============================================================================
// [Fog::FontCollection - Hash]
// ============================================================================

static err_t FOG_CDECL FontCollection_dUpdateHash(FontCollectionData* d)
{
  List<FontInfo>& list = d->fontList();
  Hash<StringW, size_t>& hash = d->fontHash();

  hash.clear();

  ListIterator<FontInfo> it(list);
  while (it.isValid())
  {
    size_t index = it.getIndex();
    const StringW& familyName = it.getItem().getFamilyName();

    if (hash.contains(familyName))
    {
      size_t* p = hash.usePtr(familyName);
      p[0]++;
    }
    else
    {
      FOG_RETURN_ON_ERROR(hash.put(familyName, 1, true));
    }

    it.next();
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::FontCollection - Construction / Destruction]
// ============================================================================

static void FOG_CDECL FontCollection_ctor(FontCollection* self)
{
  self->_d = FontCollection_dNull->addRef();
}

static void FOG_CDECL FontCollection_ctorCopy(FontCollection* self, const FontCollection* other)
{
  self->_d = other->_d->addRef();
}

static void FOG_CDECL FontCollection_dtor(FontCollection* self)
{
  self->_d->release();
}

// ============================================================================
// [Fog::FontCollection - Sharing]
// ============================================================================

static err_t FOG_CDECL FontCollection_detach(FontCollection* self)
{
  FontCollectionData* d = self->_d;
  if (d->reference.get() == 1)
    return ERR_OK;

  FontCollectionData* newd = fog_api.fontcollection_dCreate();
  if (FOG_IS_NULL(newd))
    return ERR_RT_OUT_OF_MEMORY;

  newd->fontList() = d->fontList();
  newd->fontHash() = d->fontHash();

  atomicPtrXchg(&self->_d, newd)->release();
  return ERR_OK;
}

// ============================================================================
// [Fog::FontCollection - Accessors]
// ============================================================================

static err_t FOG_CDECL FontCollection_setList(FontCollection* self, const List<FontInfo>* list)
{
  if (self->_d->fontList->_d == list->_d)
    return ERR_OK;

  FOG_RETURN_ON_ERROR(self->detach());
  FontCollectionData* d = self->_d;

  d->fontList() = *list;
  return ERR_OK;
}

static err_t FOG_CDECL FontCollection_addItem(FontCollection* self, const FontInfo* item)
{
  FOG_RETURN_ON_ERROR(self->detach());

  FontCollectionData* d = self->_d;
  size_t index;

  const StringW& newFamily = item->getFamilyName();

  {
    ListIterator<FontInfo> it(d->fontList());
    while (it.isValid())
    {
      const FontInfo& item = it.getItem();
      const StringW& itemFamily = item.getFamilyName();

      int cmp = itemFamily.compare(itemFamily);
      if (cmp < 0)
        break;

      if (cmp == 0)
      {
        return ERR_RT_OBJECT_ALREADY_EXISTS;
      }

      it.next();
    }

    index = it.getIndex();
  }

  err_t err = d->fontList().insert(index, *item);
  if (FOG_IS_ERROR(err))
    return err;

  if (d->fontHash().contains(newFamily))
  {
    size_t* p = d->fontHash().usePtr(newFamily);
    if (FOG_IS_NULL(p))
      err = ERR_RT_OUT_OF_MEMORY;
    else
      p[0]++;
  }
  else
  {
    err = d->fontHash().put(newFamily, 1, true);
  }

  if (FOG_IS_ERROR(err))
    d->fontList().removeAt(index);
  return err;
}

// ============================================================================
// [Fog::FontCollection - Clear]
// ============================================================================

static void FOG_CDECL FontCollection_clear(FontCollection* self)
{
  FontCollectionData* d = self->_d;
  if (d == &FontCollection_dNull)
    return;
  
  if (d->reference.get() == 1)
  {
    d->fontList().clear();
    d->fontHash().clear();
  }
  else
  {
    atomicPtrXchg(&self->_d, FontCollection_dNull->addRef())->release();
  }
}

// ============================================================================
// [Fog::FontCollection - Reset]
// ============================================================================

static void FOG_CDECL FontCollection_reset(FontCollection* self)
{
  if (self->_d == &FontCollection_dNull)
    return;
  atomicPtrXchg(&self->_d, FontCollection_dNull->addRef())->release();
}

// ============================================================================
// [Fog::FontCollection - Copy]
// ============================================================================

static err_t FOG_CDECL FontCollection_copy(FontCollection* self, const FontCollection* other)
{
  atomicPtrXchg(&self->_d, other->_d->addRef())->release();
  return ERR_OK;
}

// ============================================================================
// [Fog::FontCollection - Eq]
// ============================================================================

static bool FOG_CDECL FontCollection_eq(const FontCollection* a, const FontCollection* b)
{
  const FontCollectionData* a_d = a->_d;
  const FontCollectionData* b_d = b->_d;

  if (a_d == b_d)
    return true;

  return (a_d->fontList() == b_d->fontList());
}

// ============================================================================
// [Fog::FontCollection - Data]
// ============================================================================

static FontCollectionData* FOG_CDECL FontCollection_dCreate(void)
{
  FontCollectionData* d = static_cast<FontCollectionData*>(MemMgr::alloc(sizeof(FontCollectionData)));

  if (FOG_IS_NULL(d))
    return NULL;

  d->reference.init(1);
  d->vType = VAR_TYPE_FONT_COLLECTION | VAR_FLAG_NONE;
  d->flags = NO_FLAGS;
  d->fontList.init();
  d->fontHash.init();

  return d;
}

static void FOG_CDECL FontCollection_dFree(FontCollectionData* d)
{
  d->fontList.destroy();
  d->fontHash.destroy();
  MemMgr::free(d);
}

// ============================================================================
// [Fog::Font - Helpers]
// ============================================================================

static void Font_dInitDefaults(FontData* d)
{
  d->flags = NO_FLAGS;
  d->features.reset();
  d->matrix.reset();
}

static void Font_dScaleMetrics(FontData* d, float size)
{
  const FontMetrics& design = d->face->designMetrics;

  float scale = size / design._size;
  uint32_t flags = d->flags & ~(FONT_FLAG_IS_ALIGNED_SIZE);

  d->metrics._size = size;
  d->metrics._ascent = design._ascent * scale;
  d->metrics._descent = design._descent * scale;
  d->metrics._capHeight = design._capHeight * scale;
  d->metrics._xHeight = design._xHeight * scale;

  int alignedSize;
  if (Math::isFuzzyToInt(size, alignedSize))
    flags |= FONT_FLAG_IS_ALIGNED_SIZE;

  d->flags = flags;
  d->scale = scale;
}

// ============================================================================
// [Fog::Font - Construction / Destruction]
// ============================================================================

static void FOG_CDECL Font_ctor(Font* self)
{
  self->_d = FontEngine::getGlobal()._d->defaultFont._d->addRef();
}

static void FOG_CDECL Font_ctorCopy(Font* self, const Font* other)
{
  self->_d = other->_d->addRef();
}

static void FOG_CDECL Font_dtor(Font* self)
{
  FontData* d = self->_d;

  if (d != NULL)
    d->release();
}

// ============================================================================
// [Fog::Font - Sharing]
// ============================================================================

static err_t FOG_CDECL Font_detach(Font* self)
{
  FontData* d = self->_d;

  if (d->reference.get() == 1)
    return ERR_OK;

  FontData* newd = fog_api.font_dCreate();
  if (FOG_IS_NULL(newd))
    return ERR_RT_OUT_OF_MEMORY;

  FontFace* face = d->face;
  newd->flags = d->flags;
  newd->face = face != NULL ? face->addRef() : (FontFace*)NULL;

  newd->metrics = d->metrics;
  newd->features = d->features;
  newd->matrix = d->matrix;
  newd->scale = d->scale;

  atomicPtrXchg(&self->_d, newd)->release();
  return ERR_OK;
}

// ============================================================================
// [Fog::Font - Param]
// ============================================================================

static err_t FOG_CDECL Font_getParam(const Font* self, uint32_t id, void* dst)
{
  FontData* d = self->_d;

  switch (id)
  {
    case FONT_PARAM_SIZE:
      static_cast<float*>(dst)[0] = d->metrics.getSize();
      return ERR_OK;

    case FONT_PARAM_WEIGHT:
      static_cast<uint32_t*>(dst)[0] = d->features.getWeight();
      return ERR_OK;

    case FONT_PARAM_STRETCH:
      static_cast<uint32_t*>(dst)[0] = d->features.getStretch();
      return ERR_OK;

    case FONT_PARAM_DECORATION:
      static_cast<uint32_t*>(dst)[0] = d->features.getDecoration();
      return ERR_OK;

    case FONT_PARAM_STYLE:
      static_cast<uint32_t*>(dst)[0] = d->features.getStyle();
      return ERR_OK;

    case FONT_PARAM_KERNING:
      static_cast<uint32_t*>(dst)[0] = d->features.getKerning();
      return ERR_OK;

    case FONT_PARAM_COMMON_LIGATURES:
      static_cast<uint32_t*>(dst)[0] = d->features.getCommonLigatures();
      return ERR_OK;

    case FONT_PARAM_DISCRETIONARY_LIGATURES:
      static_cast<uint32_t*>(dst)[0] = d->features.getDiscretionaryLigatures();
      return ERR_OK;

    case FONT_PARAM_HISTORICAL_LIGATURES:
      static_cast<uint32_t*>(dst)[0] = d->features.getHistoricalLigatures();
      return ERR_OK;

    case FONT_PARAM_CAPS:
      static_cast<uint32_t*>(dst)[0] = d->features.getCaps();
      return ERR_OK;

    case FONT_PARAM_NUMERIC_FIGURE:
      static_cast<uint32_t*>(dst)[0] = d->features.getNumericFigure();
      return ERR_OK;

    case FONT_PARAM_NUMERIC_SPACING:
      static_cast<uint32_t*>(dst)[0] = d->features.getNumericSpacing();
      return ERR_OK;

    case FONT_PARAM_NUMERIC_FRACTION:
      static_cast<uint32_t*>(dst)[0] = d->features.getNumericFraction();
      return ERR_OK;

    case FONT_PARAM_NUMERIC_SLASHED_ZERO:
      static_cast<uint32_t*>(dst)[0] = d->features.getNumericSlashedZero();
      return ERR_OK;

    case FONT_PARAM_EAST_ASIAN_VARIANT:
      static_cast<uint32_t*>(dst)[0] = d->features.getEastAsianVariant();
      return ERR_OK;

    case FONT_PARAM_EAST_ASIAN_WIDTH:
      static_cast<uint32_t*>(dst)[0] = d->features.getEastAsianWidth();
      return ERR_OK;

    case FONT_PARAM_LETTER_SPACING:
      static_cast<FontSpacing*>(dst)->setSpacing(
        d->features.getLetterSpacingMode(), d->features.getLetterSpacingValue());
      return ERR_OK;

    case FONT_PARAM_WORD_SPACING:
      static_cast<FontSpacing*>(dst)->setSpacing(
        d->features.getWordSpacingMode(), d->features.getWordSpacingValue());
      return ERR_OK;

    case FONT_PARAM_SIZE_ADJUST:
      static_cast<float*>(dst)[0] = d->features.getSizeAdjust();
      return ERR_OK;

    case FONT_PARAM_FEATURES:
      static_cast<FontFeatures*>(dst)[0] = d->features;
      return ERR_OK;

    case FONT_PARAM_MATRIX:
      static_cast<FontMatrix*>(dst)[0] = d->matrix;
      return ERR_OK;

    default:
      return ERR_RT_INVALID_ARGUMENT;
  }
}

static err_t FOG_CDECL Font_setParam(Font* self, uint32_t id, const void* src)
{
  FontData* d = self->_d;

  uint32_t uVal;
  float fVal;

  // First compare the actual value with src. If the values are equal then
  // nothing will happen. This switch() also contains special case path for
  // parameters which require to change the font face (font-size, font-weight,
  // font-stretch, and font-style).
  switch (id)
  {
    case FONT_PARAM_SIZE:
      fVal = static_cast<const float*>(src)[0];
      if (d->metrics.getSize() == fVal)
        return ERR_OK;
      if (fVal <= 0.0f)
        return ERR_RT_INVALID_ARGUMENT;
      break;

    case FONT_PARAM_WEIGHT:
      uVal = static_cast<const uint32_t*>(src)[0];
      if (d->features.getWeight() == uVal)
        return ERR_OK;
      if (uVal > FONT_WEIGHT_MAX)
        return ERR_RT_INVALID_ARGUMENT;
      break;

    case FONT_PARAM_STRETCH:
      uVal = static_cast<const uint32_t*>(src)[0];
      if (d->features.getStretch() == uVal)
        return ERR_OK;
      if (uVal > FONT_STRETCH_MAX)
        return ERR_RT_INVALID_ARGUMENT;
      break;

    case FONT_PARAM_DECORATION:
      uVal = static_cast<const uint32_t*>(src)[0];
      if (d->features.getDecoration() == uVal)
        return ERR_OK;
      if (uVal > FONT_DECORATION_MAX)
        return ERR_RT_INVALID_ARGUMENT;
      break;

    case FONT_PARAM_STYLE:
      uVal = static_cast<const uint32_t*>(src)[0];
      if (d->features.getStyle() == uVal)
        return ERR_OK;
      if (uVal >= FONT_STYLE_COUNT)
        return ERR_RT_INVALID_ARGUMENT;
      break;

    case FONT_PARAM_KERNING:
      uVal = static_cast<const uint32_t*>(src)[0];
      if (d->features.getKerning() == uVal)
        return ERR_OK;
      if (uVal >= FONT_KERNING_COUNT)
        return ERR_RT_INVALID_ARGUMENT;
      break;

    case FONT_PARAM_COMMON_LIGATURES:
      uVal = static_cast<const uint32_t*>(src)[0];
      if (d->features.getCommonLigatures() == uVal)
        return ERR_OK;
      if (uVal >= 2)
        return ERR_RT_INVALID_ARGUMENT;
      break;

    case FONT_PARAM_DISCRETIONARY_LIGATURES:
      uVal = static_cast<const uint32_t*>(src)[0];
      if (d->features.getDiscretionaryLigatures() == uVal)
        return ERR_OK;
      if (uVal >= 2)
        return ERR_RT_INVALID_ARGUMENT;
      break;

    case FONT_PARAM_HISTORICAL_LIGATURES:
      uVal = static_cast<const uint32_t*>(src)[0];
      if (d->features.getHistoricalLigatures() == uVal)
        return ERR_OK;
      if (uVal >= 2)
        return ERR_RT_INVALID_ARGUMENT;
      break;

    case FONT_PARAM_CAPS:
      uVal = static_cast<const uint32_t*>(src)[0];
      if (d->features.getCaps() == uVal)
        return ERR_OK;
      if (uVal >= FONT_CAPS_COUNT)
        return ERR_RT_INVALID_ARGUMENT;
      break;

    case FONT_PARAM_NUMERIC_FIGURE:
      uVal = static_cast<const uint32_t*>(src)[0];
      if (d->features.getNumericFigure() == uVal)
        return ERR_OK;
      if (uVal >= FONT_NUMERIC_FIGURE_COUNT)
        return ERR_RT_INVALID_ARGUMENT;
      break;

    case FONT_PARAM_NUMERIC_SPACING:
      uVal = static_cast<const uint32_t*>(src)[0];
      if (d->features.getNumericSpacing() == uVal)
        return ERR_OK;
      if (uVal >= FONT_NUMERIC_SPACING_COUNT)
        return ERR_RT_INVALID_ARGUMENT;
      break;

    case FONT_PARAM_NUMERIC_FRACTION:
      uVal = static_cast<const uint32_t*>(src)[0];
      if (d->features.getNumericFraction() == uVal)
        return ERR_OK;
      if (uVal >= FONT_NUMERIC_FRACTION_COUNT)
        return ERR_RT_INVALID_ARGUMENT;
      break;

    case FONT_PARAM_NUMERIC_SLASHED_ZERO:
      uVal = static_cast<const uint32_t*>(src)[0];
      if (d->features.getNumericSlashedZero() == uVal)
        return ERR_OK;
      if (uVal >= 2)
        return ERR_RT_INVALID_ARGUMENT;
      break;

    case FONT_PARAM_EAST_ASIAN_VARIANT:
      uVal = static_cast<const uint32_t*>(src)[0];
      if (d->features.getEastAsianVariant() == uVal)
        return ERR_OK;
      if (uVal >= FONT_EAST_ASIAN_VARIANT_COUNT)
        return ERR_RT_INVALID_ARGUMENT;
      break;

    case FONT_PARAM_EAST_ASIAN_WIDTH:
      uVal = static_cast<const uint32_t*>(src)[0];
      if (d->features.getEastAsianWidth() == uVal)
        return ERR_OK;
      if (uVal >= FONT_EAST_ASIAN_WIDTH_COUNT)
        return ERR_RT_INVALID_ARGUMENT;
      break;

    case FONT_PARAM_LETTER_SPACING:
      if (d->features.getLetterSpacingMode() == static_cast<const FontSpacing*>(src)->getMode() &&
          d->features.getLetterSpacingValue() == static_cast<const FontSpacing*>(src)->getValue())
      {
        return ERR_OK;
      }

      if (static_cast<const FontSpacing*>(src)->getMode() >= FONT_SPACING_COUNT)
        return ERR_RT_INVALID_ARGUMENT;
      break;

    case FONT_PARAM_WORD_SPACING:
      if (d->features.getWordSpacingMode() == static_cast<const FontSpacing*>(src)->getMode() &&
          d->features.getWordSpacingValue() == static_cast<const FontSpacing*>(src)->getValue())
      {
        return ERR_OK;
      }

      if (static_cast<const FontSpacing*>(src)->getMode() >= FONT_SPACING_COUNT)
        return ERR_RT_INVALID_ARGUMENT;
      break;

    case FONT_PARAM_SIZE_ADJUST:
      fVal = static_cast<const float*>(src)[0];
      if (d->features.getSizeAdjust() == fVal)
        return ERR_OK;
      if (fVal <= 0.0f)
        return ERR_RT_INVALID_ARGUMENT;
      break;

    case FONT_PARAM_FEATURES:
      if (d->features == static_cast<const FontFeatures*>(src)[0])
        return ERR_OK;
      break;

    case FONT_PARAM_MATRIX:
      if (d->matrix == static_cast<const FontMatrix*>(src)[0])
        return ERR_OK;
      break;

    default:
      return ERR_RT_INVALID_ARGUMENT;
  }

  // Okay, we need to change the value in FontData.
  if (d->reference.get() != 1)
  {
    FOG_RETURN_ON_ERROR(self->_detach());
    d = self->_d;
  }

  // Set the value.
  switch (id)
  {
    case FONT_PARAM_SIZE:
      Font_dScaleMetrics(d, fVal);
      break;

    case FONT_PARAM_WEIGHT:
      d->features.setWeight(uVal);
      break;

    case FONT_PARAM_STRETCH:
      d->features.setStretch(uVal);
      break;

    case FONT_PARAM_DECORATION:
      d->features.setDecoration(uVal);
      break;

    case FONT_PARAM_STYLE:
      d->features.setStyle(uVal);
      break;

    case FONT_PARAM_KERNING:
      d->features.setKerning(uVal);
      break;

    case FONT_PARAM_COMMON_LIGATURES:
      d->features.setCommonLigatures(uVal);
      break;

    case FONT_PARAM_DISCRETIONARY_LIGATURES:
      d->features.setDiscretionaryLigatures(uVal);
      break;

    case FONT_PARAM_HISTORICAL_LIGATURES:
      d->features.setHistoricalLigatures(uVal);
      break;

    case FONT_PARAM_CAPS:
      d->features.setCaps(uVal);
      break;

    case FONT_PARAM_NUMERIC_FIGURE:
      d->features.setNumericFigure(uVal);
      break;

    case FONT_PARAM_NUMERIC_SPACING:
      d->features.setNumericSpacing(uVal);
      break;

    case FONT_PARAM_NUMERIC_FRACTION:
      d->features.setNumericFraction(uVal);
      break;

    case FONT_PARAM_NUMERIC_SLASHED_ZERO:
      d->features.setNumericSlashedZero(uVal);
      break;

    case FONT_PARAM_EAST_ASIAN_VARIANT:
      d->features.setEastAsianVariant(uVal);
      break;

    case FONT_PARAM_EAST_ASIAN_WIDTH:
      d->features.setEastAsianWidth(uVal);
      break;

    case FONT_PARAM_LETTER_SPACING:
      d->features.setLetterSpacingMode(static_cast<const FontSpacing*>(src)->getMode());
      d->features.setLetterSpacingValue(static_cast<const FontSpacing*>(src)->getValue());

      if (d->features.hasLetterSpacing())
        d->flags |= FONT_FLAG_HAS_LETTER_SPACING;
      else
        d->flags &= ~FONT_FLAG_HAS_LETTER_SPACING;
      break;

    case FONT_PARAM_WORD_SPACING:
      d->features.setWordSpacingMode(static_cast<const FontSpacing*>(src)->getMode());
      d->features.setWordSpacingValue(static_cast<const FontSpacing*>(src)->getValue());

      if (d->features.hasWordSpacing())
        d->flags |= FONT_FLAG_HAS_WORD_SPACING;
      else
        d->flags &= ~FONT_FLAG_HAS_WORD_SPACING;
      break;

    case FONT_PARAM_SIZE_ADJUST:
      fVal = static_cast<const float*>(src)[0];
      if (d->features.getSizeAdjust() == fVal)
        return ERR_OK;
      break;

    case FONT_PARAM_FEATURES:
      d->features = static_cast<const FontFeatures*>(src)[0];
      uVal = d->flags & ~(FONT_FLAG_HAS_LETTER_SPACING | FONT_FLAG_HAS_WORD_SPACING | FONT_FLAG_HAS_MATRIX);

      if (d->features.hasLetterSpacing())
        uVal |= FONT_FLAG_HAS_LETTER_SPACING;

      if (d->features.hasWordSpacing())
        uVal |= FONT_FLAG_HAS_WORD_SPACING;

      if (d->matrix.isIdentity())
        uVal |= FONT_FLAG_HAS_MATRIX;

      d->flags = uVal;
      break;

    case FONT_PARAM_MATRIX:
      if (d->matrix == static_cast<const FontMatrix*>(src)[0])
        return ERR_OK;
      
      if (d->matrix.isIdentity())
        d->flags |= FONT_FLAG_HAS_MATRIX;
      else
        d->flags &= ~FONT_FLAG_HAS_MATRIX;
      break;

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::Font - Reset]
// ============================================================================

static void FOG_CDECL Font_reset(Font* self)
{
  FOG_ASSERT(self->_d != NULL);

  FontData* newd = FontEngine::getGlobal()._d->defaultFont._d;
  atomicPtrXchg(&self->_d, newd->addRef())->release();
}

// ============================================================================
// [Fog::Font - Create]
// ============================================================================

static err_t FOG_CDECL Font_create(Font* self, const StringW* family, float size,
  const FontFeatures* features, const FontMatrix* matrix)
{
  FontData* d = self->_d;
  FOG_ASSERT(d != NULL);

  if (d->reference.get() != 1)
  {
    FOG_RETURN_ON_ERROR(fog_api.font_detach(self));
    d = self->_d;
  }

  // Try to optimize font-face creation using the current font-face.
  if (d->face->family != *family)
  {
    FontFace* face = FontEngine::getGlobal()->getFontFace(*family);
    if (FOG_IS_NULL(face))
      return ERR_FONT_NOT_MATCHED;

    face = atomicPtrXchg(&d->face, face);
    if (face != NULL) face->deref();
  }

  Font_initValues(d);
  Font_dScaleMetrics(d, size);
  return ERR_OK;
}

static err_t FOG_CDECL Font_init(Font* self, FontFace* face, float size,
  const FontFeatures* features, const FontMatrix* matrix)
{
  FontData* d = self->_d;
  FOG_ASSERT(d != NULL);

  if (d->reference.get() != 1)
  {
    FOG_RETURN_ON_ERROR(self->detach());
    d = self->_d;
  }

  face = atomicPtrXchg(&d->face, face);
  if (face != NULL)
    face->deref();

  d->features = *features;
  d->matrix = *matrix;
  Font_dScaleMetrics(d, size);

  return ERR_OK;
}

// ============================================================================
// [Fog::Font - Glyphs]
// ============================================================================

static err_t Font_getOutlineFromGlyphRunF(const Font* self,
  PathF* dst, uint32_t cntOp,
  const uint32_t* glyphList, size_t glyphAdvance,
  const PointF* positionList, size_t positionAdvance,
  size_t length)
{
  FontFace* face = self->_d->face;

  if (FOG_IS_NULL(face))
    return ERR_FONT_INVALID_FACE;

  return face->getOutlineFromGlyphRun(*dst, cntOp, glyphList, glyphAdvance, positionList, positionAdvance, length);
}

static err_t Font_getOutlineFromGlyphRunD(const Font* self,
  PathD* dst, uint32_t cntOp,
  const uint32_t* glyphList, size_t glyphAdvance,
  const PointF* positionList, size_t positionAdvance,
  size_t length)
{
  FontFace* face = self->_d->face;

  if (FOG_IS_NULL(face))
    return ERR_FONT_INVALID_FACE;

  return face->getOutlineFromGlyphRun(*dst, cntOp, glyphList, glyphAdvance, positionList, positionAdvance, length);
}

// ============================================================================
// [Fog::Font - Copy]
// ============================================================================

static err_t FOG_CDECL Font_copy(Font* self, const Font* other)
{
  FOG_ASSERT(self->_d != NULL);

  atomicPtrXchg(&self->_d, other->_d->addRef())->release();
  return ERR_OK;
}

// ============================================================================
// [Fog::Font - Equality]
// ============================================================================

static bool FOG_CDECL Font_eq(const Font* a, const Font* b)
{
  const FontData* a_d = a->_d;
  const FontData* b_d = b->_d;

  if (a_d == b_d)
    return true;

  if (a_d->face != b_d->face)
    return false;

  if (a_d->features != b_d->features)
    return false;

  if (a_d->matrix != b_d->matrix)
    return false;

  if (a_d->scale != b_d->scale)
    return false;

  return true;
}

// ============================================================================
// [Fog::Font - FontData]
// ============================================================================

static FontData* FOG_CDECL Font_dCreate(void)
{
  FontData* d = reinterpret_cast<FontData*>(MemMgr::alloc(sizeof(FontData)));
  if (FOG_IS_NULL(d)) return d;

  d->reference.init(1);
  d->vType = VAR_TYPE_FONT | VAR_FLAG_NONE;

  return d;
}

static void Font_dFree(FontData* d)
{
  if (d->face)
    d->face->deref();

  if ((d->vType & VAR_FLAG_STATIC) == 0)
    MemMgr::free(d);
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Font_init(void)
{
  // --------------------------------------------------------------------------
  // [FontInfo]
  // --------------------------------------------------------------------------

  fog_api.fontinfo_ctor = FontInfo_ctor;
  fog_api.fontinfo_ctorCopy = FontInfo_ctorCopy;
  fog_api.fontinfo_dtor = FontInfo_dtor;

  fog_api.fontinfo_detach = FontInfo_detach;
  fog_api.fontinfo_setDefs = FontInfo_setDefs;
  fog_api.fontinfo_setFamilyName = FontInfo_setFamilyName;
  fog_api.fontinfo_setFileName = FontInfo_setFileName;
  fog_api.fontinfo_reset = FontInfo_reset;
  fog_api.fontinfo_copy = FontInfo_copy;
  fog_api.fontinfo_eq = FontInfo_eq;
  fog_api.fontinfo_compare = FontInfo_compare;

  fog_api.fontinfo_dCreate = FontInfo_dCreate;
  fog_api.fontinfo_dFree = FontInfo_dFree;

  {
    FontInfoData* d = &FontInfo_dNull;

    d->reference.init(1);
    d->vType = VAR_TYPE_FONT_INFO | VAR_FLAG_NONE;
    d->defs.setWeight(FONT_WEIGHT_NORMAL);
    d->defs.setStretch(FONT_STRETCH_NORMAL);
    d->familyName->_d = fog_api.stringw_oEmpty->_d;
    d->fileName->_d = fog_api.stringw_oEmpty->_d;

    fog_api.fontinfo_oNull = FontInfo_oNull.initCustom1(d);
  }

  // --------------------------------------------------------------------------
  // [FontCollection]
  // --------------------------------------------------------------------------

  fog_api.fontcollection_ctor = FontCollection_ctor;
  fog_api.fontcollection_ctorCopy = FontCollection_ctorCopy;
  fog_api.fontcollection_dtor = FontCollection_dtor;

  fog_api.fontcollection_detach = FontCollection_detach;
  fog_api.fontcollection_setList = FontCollection_setList;
  fog_api.fontcollection_clear = FontCollection_clear;
  fog_api.fontcollection_reset = FontCollection_reset;
  fog_api.fontcollection_copy = FontCollection_copy;
  fog_api.fontcollection_eq = FontCollection_eq;

  fog_api.fontcollection_dCreate = FontCollection_dCreate;
  fog_api.fontcollection_dFree = FontCollection_dFree;

  {
    FontCollectionData* d = &FontCollection_dNull;

    d->reference.init(1);
    d->vType = VAR_TYPE_FONT_COLLECTION | VAR_FLAG_NONE;
    d->fontList->_d = fog_api.list_untyped_oEmpty->_d;
    d->fontHash->_d = fog_api.hash_unknown_unknown_oEmpty->_d;

    fog_api.fontcollection_oNull = FontCollection_oNull.initCustom1(d);
  }

  // --------------------------------------------------------------------------
  // [Font]
  // --------------------------------------------------------------------------

  fog_api.font_ctor = Font_ctor;
  fog_api.font_ctorCopy = Font_ctorCopy;
  fog_api.font_dtor = Font_dtor;

  fog_api.font_detach = Font_detach;
  fog_api.font_getParam = Font_getParam;
  fog_api.font_setParam = Font_setParam;
  fog_api.font_reset = Font_reset;

  fog_api.font_create = Font_create;
  fog_api.font_init = Font_init;

  fog_api.font_getOutlineFromGlyphRunF = Font_getOutlineFromGlyphRunF;
  fog_api.font_getOutlineFromGlyphRunD = Font_getOutlineFromGlyphRunD;

  fog_api.font_copy = Font_copy;
  fog_api.font_eq = Font_eq;

  fog_api.font_dCreate = Font_dCreate;
  fog_api.font_dFree = Font_dFree;
}

FOG_NO_EXPORT void Font_fini(void)
{
}

} // Fog namespace
