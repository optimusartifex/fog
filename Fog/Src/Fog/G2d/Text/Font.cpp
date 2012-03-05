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
#include <Fog/G2d/Text/Font.h>

namespace Fog {

// ============================================================================
// [Fog::FaceInfo - Global]
// ============================================================================

static Static<FaceInfoData> FaceInfo_dNull;
static Static<FaceInfo> FaceInfo_oNull;

// ============================================================================
// [Fog::FaceInfo - Construction / Destruction]
// ============================================================================

static void FOG_CDECL FaceInfo_ctor(FaceInfo* self)
{
  self->_d = FaceInfo_dNull->addRef();
}

static void FOG_CDECL FaceInfo_ctorCopy(FaceInfo* self, const FaceInfo* other)
{
  self->_d = other->_d->addRef();
}

static void FOG_CDECL FaceInfo_dtor(FaceInfo* self)
{
  self->_d->release();
}

// ============================================================================
// [Fog::FaceInfo - Sharing]
// ============================================================================

static err_t FOG_CDECL FaceInfo_detach(FaceInfo* self)
{
  FaceInfoData* d = self->_d;
  if (d->reference.get() == 1)
    return ERR_OK;

  FaceInfoData* newd = fog_api.faceinfo_dCreate(&d->familyName, &d->fileName);
  if (FOG_IS_NULL(newd))
    return ERR_RT_OUT_OF_MEMORY;

  atomicPtrXchg(&self->_d, newd)->release();
  return ERR_OK;
}

// ============================================================================
// [Fog::FaceInfo - Accessors]
// ============================================================================

static err_t FOG_CDECL FaceInfo_setFeatures(FaceInfo* self, const FaceFeatures* features)
{
  uint32_t featuresPacked = features->_packed;
  if (self->_d->features._packed == featuresPacked)
    return ERR_OK;

  FOG_RETURN_ON_ERROR(self->detach());
  FaceInfoData* d = self->_d;

  d->features._packed = featuresPacked;
  return ERR_OK;
}

static err_t FOG_CDECL FaceInfo_setFamilyName(FaceInfo* self, const StringW* familyName)
{
  FOG_RETURN_ON_ERROR(self->detach());
  FaceInfoData* d = self->_d;

  return d->familyName->set(*familyName);
}

static err_t FOG_CDECL FaceInfo_setFileName(FaceInfo* self, const StringW* fileName)
{
  FOG_RETURN_ON_ERROR(self->detach());
  FaceInfoData* d = self->_d;

  return d->fileName->set(*fileName);
}

static err_t FOG_CDECL FaceInfo_setMetrics(FaceInfo* self, const FaceInfoMetrics* metrics)
{
  FOG_RETURN_ON_ERROR(self->detach());
  FaceInfoData* d = self->_d;

  d->metrics = *metrics;
  return ERR_OK;
}

// ============================================================================
// [Fog::FaceInfo - Reset]
// ============================================================================

static void FOG_CDECL FaceInfo_reset(FaceInfo* self)
{
  if (self->_d == &FaceInfo_dNull)
    return;
  atomicPtrXchg(&self->_d, FaceInfo_dNull->addRef())->release();
}

// ============================================================================
// [Fog::FaceInfo - Copy]
// ============================================================================

static err_t FOG_CDECL FaceInfo_copy(FaceInfo* self, const FaceInfo* other)
{
  atomicPtrXchg(&self->_d, other->_d->addRef())->release();
  return ERR_OK;
}

// ============================================================================
// [Fog::FaceInfo - Eq]
// ============================================================================

static bool FOG_CDECL FaceInfo_eq(const FaceInfo* a, const FaceInfo* b)
{
  const FaceInfoData* a_d = a->_d;
  const FaceInfoData* b_d = b->_d;

  if (a_d == b_d)
    return true;

  if (a_d->familyName() == b_d->familyName())
    return false;
  if (a_d->fileName() != b_d->fileName())
    return false;

  return true;
}

// ============================================================================
// [Fog::FaceInfo - Compare]
// ============================================================================

static int FOG_CDECL FaceInfo_compare(const FaceInfo* a, const FaceInfo* b)
{
  const FaceInfoData* a_d = a->_d;
  const FaceInfoData* b_d = b->_d;

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
// [Fog::FaceInfo - Data]
// ============================================================================

static FaceInfoData* FOG_CDECL FaceInfo_dCreate(const StringW* familyName, const StringW* fileName)
{
  FaceInfoData* d = static_cast<FaceInfoData*>(MemMgr::alloc(sizeof(FaceInfoData)));

  if (FOG_IS_NULL(d))
    return NULL;

  d->reference.init(1);
  d->vType = VAR_TYPE_FACE_INFO | VAR_FLAG_NONE;
  d->features = FaceFeatures(FONT_WEIGHT_NORMAL, FONT_STRETCH_NORMAL, false);
  d->familyName.initCustom1(*familyName);
  d->fileName.initCustom1(*fileName);
  d->metrics.reset();

  return d;
}

static void FOG_CDECL FaceInfo_dFree(FaceInfoData* d)
{
  d->familyName.destroy();
  d->fileName.destroy();

  MemMgr::free(d);
}

// ============================================================================
// [Fog::FaceCollection - Global]
// ============================================================================

static Static<FaceCollectionData> FaceCollection_dNull;
static Static<FaceCollection> FaceCollection_oNull;

// ============================================================================
// [Fog::FaceCollection - Hash]
// ============================================================================

static err_t FOG_CDECL FaceCollection_dUpdateHash(FaceCollectionData* d)
{
  List<FaceInfo>& list = d->faceList();
  Hash<StringW, size_t>& hash = d->faceHash();

  hash.clear();

  ListIterator<FaceInfo> it(list);
  while (it.isValid())
  {
    size_t index = it.getIndex();
    const StringW& familyName = it.getItem().getFamilyName();

    if (hash.contains(familyName))
    {
      size_t* p = hash.usePtr(familyName, NULL);
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
// [Fog::FaceCollection - Construction / Destruction]
// ============================================================================

static void FOG_CDECL FaceCollection_ctor(FaceCollection* self)
{
  self->_d = FaceCollection_dNull->addRef();
}

static void FOG_CDECL FaceCollection_ctorCopy(FaceCollection* self, const FaceCollection* other)
{
  self->_d = other->_d->addRef();
}

static void FOG_CDECL FaceCollection_dtor(FaceCollection* self)
{
  self->_d->release();
}

// ============================================================================
// [Fog::FaceCollection - Sharing]
// ============================================================================

static err_t FOG_CDECL FaceCollection_detach(FaceCollection* self)
{
  FaceCollectionData* d = self->_d;
  if (d->reference.get() == 1)
    return ERR_OK;

  FaceCollectionData* newd = fog_api.facecollection_dCreate();
  if (FOG_IS_NULL(newd))
    return ERR_RT_OUT_OF_MEMORY;

  newd->faceList() = d->faceList();
  newd->faceHash() = d->faceHash();

  atomicPtrXchg(&self->_d, newd)->release();
  return ERR_OK;
}

// ============================================================================
// [Fog::FaceCollection - Accessors]
// ============================================================================

static err_t FOG_CDECL FaceCollection_setList(FaceCollection* self, const List<FaceInfo>* list)
{
  if (self->_d->faceList->_d == list->_d)
    return ERR_OK;

  FOG_RETURN_ON_ERROR(self->detach());
  FaceCollectionData* d = self->_d;

  d->faceList() = *list;
  return ERR_OK;
}

// ============================================================================
// [Fog::FaceCollection - GetFamilyRange]
// ============================================================================

static bool FaceCollection_getFamilyRangeStringW(const FaceCollection* self,
  const StringW* family, Range* dst)
{
  const List<FaceInfo>& faceList = self->_d->faceList();

  const FaceInfo* base = faceList.getData();
  size_t length = faceList.getLength();

  for (size_t lim = length; lim != 0; lim >>= 1)
  {
    const FaceInfo* cur = base + (lim >> 1);
    int cmp = StringW::compare(&cur->_d->familyName, family);

    // Right.
    if (cmp < 0)
    {
      base = cur + 1;
      lim--;
      continue;
    }

    // Left.
    if (cmp > 0)
      continue;

    // Expand range.
    {
      const FaceInfo* stop = faceList.getData();
      const FaceInfo* p = cur;

      // Left.
      for (;;)
      {
        if (p == stop)
          break;

        if (p[-1].getFamilyName() != *family)
          break;

        p--;
      }
      dst->setStart((size_t)(p - stop));

      // Right.
      stop += length;
      p = cur;

      while (++p != stop)
      {
        if (p->getFamilyName() != *family)
          break;
      }
      dst->setEnd((size_t)(p - faceList.getData()));
    }
    return true;
  }

  dst->setRange(INVALID_INDEX, INVALID_INDEX);
  return false;
}

static bool FaceCollection_getFamilyRangeStubW(const FaceCollection* self,
  const StubW* family_, Range* dst)
{
  const List<FaceInfo>& faceList = self->_d->faceList();
  StubW family(family_->getData(), family_->getComputedLength());

  const FaceInfo* base = faceList.getData();
  size_t length = faceList.getLength();

  for (size_t lim = length; lim != 0; lim >>= 1)
  {
    const FaceInfo* cur = base + (lim >> 1);
    int cmp = cur->_d->familyName->compare(family);

    // Right.
    if (cmp < 0)
    {
      base = cur + 1;
      lim--;
      continue;
    }

    // Left.
    if (cmp > 0)
      continue;

    // Expand range.
    {
      const FaceInfo* stop = faceList.getData();
      const FaceInfo* p = cur;

      // Left.
      for (;;)
      {
        if (p == stop)
          break;

        if (p[-1].getFamilyName() != family)
          break;

        p--;
      }
      dst->setStart((size_t)(p - stop));

      // Right.
      stop += length;
      p = cur;

      while (++p != stop)
      {
        if (p->getFamilyName() != family)
          break;
      }
      dst->setEnd((size_t)(p - faceList.getData()));
    }
    return true;
  }

  dst->setRange(INVALID_INDEX, INVALID_INDEX);
  return false;
}

// ============================================================================
// [Fog::FaceCollection - IndexOf]
// ============================================================================

static FOG_INLINE size_t FaceCollection_indexOfFeature(const FaceCollection* self, const Range* range, const FaceFeatures* features)
{
  const List<FaceInfo>& faceList = self->_d->faceList();

  const FaceInfo* pBase = faceList.getData();
  const FaceInfo* pCur = pBase;
  const FaceInfo* pEnd = pBase;

  pCur += range->getStart();
  pEnd += range->getEnd();

  uint32_t featuresPacked = features->_packed;

  while (pCur != pEnd)
  {
    if (pCur->_d->features._packed == featuresPacked)
      return (size_t)(pCur - pBase);
    pCur++;
  }

  return INVALID_INDEX;
}

static size_t FOG_CDECL FaceCollection_indexOfStringW(const FaceCollection* self, const StringW* family, const FaceFeatures* features)
{
  Range range(UNINITIALIZED);
  if (!fog_api.facecollection_getFamilyRangeStringW(self, family, &range))
    return INVALID_INDEX;
  return FaceCollection_indexOfFeature(self, &range, features);
}

static size_t FOG_CDECL FaceCollection_indexOfStubW(const FaceCollection* self, const StubW* family, const FaceFeatures* features)
{
  Range range(UNINITIALIZED);
  if (!fog_api.facecollection_getFamilyRangeStubW(self, family, &range))
    return INVALID_INDEX;
  return FaceCollection_indexOfFeature(self, &range, features);
}

// ============================================================================
// [Fog::FaceCollection - Manipulation]
// ============================================================================

static err_t FOG_CDECL FaceCollection_addItem(FaceCollection* self, const FaceInfo* item, size_t* dstIndex)
{
  FOG_RETURN_ON_ERROR(self->detach());

  FaceCollectionData* d = self->_d;
  size_t index;

  const StringW& newFamily = item->getFamilyName();
  FaceFeatures newFeatures = item->getFeatures();

  {
    ListIterator<FaceInfo> it(d->faceList());
    while (it.isValid())
    {
      const FaceInfo& fi = it.getItem();
      const StringW& fiFamily = fi.getFamilyName();

      int cmp = newFamily.compare(fiFamily);
      if (cmp < 0)
        break;

      if (cmp == 0)
      {
        if (newFeatures._packed < fi.getFeatures()._packed)
          break;

        if (newFeatures._packed == fi.getFeatures()._packed)
        {
          if (dstIndex != NULL)
            *dstIndex = it.getIndex();
          return ERR_RT_OBJECT_ALREADY_EXISTS;
        }
      }

      it.next();
    }

    index = it.getIndex();
  }

  err_t err = d->faceList().insert(index, *item);
  if (FOG_IS_ERROR(err))
    return err;

  if (d->faceHash().contains(newFamily))
  {
    size_t* p = d->faceHash().usePtr(newFamily, NULL);
    if (FOG_IS_NULL(p))
      err = ERR_RT_OUT_OF_MEMORY;
    else
      p[0]++;
  }
  else
  {
    err = d->faceHash().put(newFamily, 1, true);
  }

  if (FOG_IS_ERROR(err))
    d->faceList().removeAt(index);
  else if (dstIndex != NULL)
    *dstIndex = index;

  return err;
}

// ============================================================================
// [Fog::FaceCollection - Clear]
// ============================================================================

static void FOG_CDECL FaceCollection_clear(FaceCollection* self)
{
  FaceCollectionData* d = self->_d;
  if (d == &FaceCollection_dNull)
    return;
  
  if (d->reference.get() == 1)
  {
    d->faceList().clear();
    d->faceHash().clear();
  }
  else
  {
    atomicPtrXchg(&self->_d, FaceCollection_dNull->addRef())->release();
  }
}

// ============================================================================
// [Fog::FaceCollection - Reset]
// ============================================================================

static void FOG_CDECL FaceCollection_reset(FaceCollection* self)
{
  if (self->_d == &FaceCollection_dNull)
    return;
  atomicPtrXchg(&self->_d, FaceCollection_dNull->addRef())->release();
}

// ============================================================================
// [Fog::FaceCollection - Copy]
// ============================================================================

static err_t FOG_CDECL FaceCollection_copy(FaceCollection* self, const FaceCollection* other)
{
  atomicPtrXchg(&self->_d, other->_d->addRef())->release();
  return ERR_OK;
}

// ============================================================================
// [Fog::FaceCollection - Eq]
// ============================================================================

static bool FOG_CDECL FaceCollection_eq(const FaceCollection* a, const FaceCollection* b)
{
  const FaceCollectionData* a_d = a->_d;
  const FaceCollectionData* b_d = b->_d;

  if (a_d == b_d)
    return true;

  return (a_d->faceList() == b_d->faceList());
}

// ============================================================================
// [Fog::FaceCollection - Data]
// ============================================================================

static FaceCollectionData* FOG_CDECL FaceCollection_dCreate(void)
{
  FaceCollectionData* d = static_cast<FaceCollectionData*>(MemMgr::alloc(sizeof(FaceCollectionData)));

  if (FOG_IS_NULL(d))
    return NULL;

  d->reference.init(1);
  d->vType = VAR_TYPE_FACE_COLLECTION | VAR_FLAG_NONE;
  d->flags = NO_FLAGS;
  d->faceList.init();
  d->faceHash.init();

  return d;
}

static void FOG_CDECL FaceCollection_dFree(FaceCollectionData* d)
{
  d->faceList.destroy();
  d->faceHash.destroy();
  MemMgr::free(d);
}

// ============================================================================
// [Fog::FaceCache - Construction / Destruction]
// ============================================================================

static void FOG_CDECL FaceCache_ctor(FaceCache* self)
{
  self->data.init();
}

static void FOG_CDECL FaceCache_dtor(FaceCache* self)
{
  self->reset();
  self->data.destroy();
}

// ============================================================================
// [Fog::FaceCache - Reset]
// ============================================================================

static void FOG_CDECL FaceCache_reset(FaceCache* self)
{
  Hash< StringW, List<Face*> > copy;
  swap(copy, self->data());

  HashIterator< StringW, List<Face*> > cacheIterator(copy);
  while (cacheIterator.isValid())
  {
    const List<Face*>& faceList = cacheIterator.getItem();
    ListIterator<Face*> faceIterator(faceList);

    while (faceIterator.isValid())
    {
      faceIterator.getItem()->release();
      faceIterator.next();
    }

    cacheIterator.next();
  }
}

// ============================================================================
// [Fog::FaceCache - Methods]
// ============================================================================

static Face* FOG_CDECL FaceCache_getExactFace(const FaceCache* self, const StringW* family, const FaceFeatures* features)
{
  const List<Face*>* faceList = self->data().getPtr(*family, NULL);
  if (FOG_IS_NULL(faceList))
    return NULL;

  ListIterator<Face*> faceIterator(*faceList);
  uint32_t featuresPacked = features->_packed;

  while (faceIterator.isValid())
  {
    Face* face = faceIterator.getItem();

    if (face->features._packed == featuresPacked)
      return face->addRef();

    faceIterator.next();
  }

  return NULL;
}

static err_t FOG_CDECL FaceCache_getAllFaces(const FaceCache* self, const StringW* family, List<Face*>* dst)
{
  const List<Face*>* faceList = self->data().getPtr(*family, NULL);

  if (faceList != NULL)
    return dst->setList(*faceList);

  dst->clear();
  return ERR_OK;
}

static err_t FOG_CDECL FaceCache_put(FaceCache* self, const StringW* family, const FaceFeatures* features, Face* face)
{
  Hash< StringW, List<Face*> >& data = self->data();
  List<Face*>* list = data.usePtr(*family, NULL);

  if (list != NULL)
  {
    FOG_RETURN_ON_ERROR(list->append(face));
  }
  else
  {
    List<Face*> list;
    FOG_RETURN_ON_ERROR(list.append(face));
    FOG_RETURN_ON_ERROR(data.put(*family, list, false));
  }

  face->reference.inc();
  return ERR_OK;
}

static err_t FOG_CDECL FaceCache_remove(FaceCache* self, const StringW* family, const FaceFeatures* features, Face* face)
{
  Hash< StringW, List<Face*> >& data = self->data();
  List<Face*>* list = data.usePtr(*family, NULL);

  if (list != NULL)
  {
    ListIterator<Face*> listIterator(*list);
    size_t index = INVALID_INDEX;

    while (listIterator.isValid())
    {
      if (listIterator.getItem() == face)
      {
        index = listIterator.getIndex();
        break;
      }
      listIterator.next();
    }

    if (index == INVALID_INDEX)
      return ERR_RT_OBJECT_NOT_FOUND;

    FOG_RETURN_ON_ERROR(list->removeAt(index));
    if (list->getLength() == 0)
      data.remove(*family);

    face->release();
    return ERR_OK;
  }
  else
  {
    return ERR_RT_OBJECT_NOT_FOUND;
  }
}

// ============================================================================
// [Fog::Font - Global]
// ============================================================================

static Static<FontData> Font_dNull;
static Static<Font> Font_oNull;

// ============================================================================
// [Fog::Font - Helpers]
// ============================================================================

static void Font_dScaleMetrics(FontData* d, float size)
{
  const FontMetrics& design = d->face->designMetrics;

  float scale = size / design._size;
  uint32_t flags = d->flags & ~(FONT_FLAG_IS_ALIGNED_SIZE);

  d->metrics._size = size;
  d->metrics._ascent = design._ascent * scale;
  d->metrics._descent = design._descent * scale;
  d->metrics._lineGap = design._lineGap * scale;
  d->metrics._lineSpacing = design._lineSpacing * scale;
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
  const Font& defaultFont = FontEngine::getGlobal()->defaultFont();
  FOG_ASSERT_X(defaultFont._d != NULL, 
    "Fog::Font() - No default font in global FontEngine.");

  self->_d = defaultFont._d->addRef();
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

  Face* face = d->face;
  newd->flags = d->flags;
  newd->face = face != NULL ? face->addRef() : (Face*)NULL;

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
  const Font& defaultFont = FontEngine::getGlobal()->defaultFont();

  FOG_ASSERT_X(self->_d != NULL,
    "Fog::Font::reset() - Called on not-initialized instance.");
  FOG_ASSERT_X(defaultFont._d != NULL, 
    "Fog::Font::reset() - No default font in global FontEngine.");

  atomicPtrXchg(&self->_d, defaultFont._d->addRef())->release();
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

  // If these are not given then we fake them using default values.
  Static<FontFeatures> staticFeatures;
  Static<FontMatrix> staticMatrix;

  if (features == NULL)
  {
    staticFeatures->reset();
    features = &staticFeatures;
  }

  if (matrix == NULL)
  {
    staticMatrix->reset();
    matrix = &staticMatrix;
  }

  // It is expensive to query font-face, so we try to do it only if it's really
  // necessary. In case that the family is the same, there is high probability
  // that we don't do query at all, just compare requested features.
  bool doFaceQuery = d->face->family != *family;

  // Font family is the same -> compare weight, stretch, and style. If these
  // are the same then we don't need to do a real query.
  if (doFaceQuery == false)
  {
    doFaceQuery = (d->features.getWeight()  != features->getWeight() ) ||
                  (d->features.getStretch() != features->getStretch()) ||
                  (d->features.getStyle()   != features->getStyle()  ) ;
  }

  if (doFaceQuery)
  {
    Face* face;
    FaceFeatures faceFeatures(features->getWeight(), features->getStretch(), features->getStyle() == FONT_STYLE_ITALIC);

    err_t err = FontEngine::getGlobal()->queryFace(&face, *family, faceFeatures);
    if (FOG_IS_ERROR(err))
      return err;

    face = atomicPtrXchg(&d->face, face);
    if (face != NULL)
      face->release();
  }

  d->features = *features;  
  d->matrix = *matrix;
  Font_dScaleMetrics(d, size);
  return ERR_OK;
}

static err_t FOG_CDECL Font_init(Font* self, Face* face, float size,
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
    face->release();

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
  Face* face = self->_d->face;

  if (FOG_IS_NULL(face))
    return ERR_FONT_INVALID_FACE;

  return face->vtable->getOutlineFromGlyphRunF(self->_d, dst, cntOp, glyphList, glyphAdvance, positionList, positionAdvance, length);
}

static err_t Font_getOutlineFromGlyphRunD(const Font* self,
  PathD* dst, uint32_t cntOp,
  const uint32_t* glyphList, size_t glyphAdvance,
  const PointF* positionList, size_t positionAdvance,
  size_t length)
{
  Face* face = self->_d->face;

  if (FOG_IS_NULL(face))
    return ERR_FONT_INVALID_FACE;

  return face->vtable->getOutlineFromGlyphRunD(self->_d, dst, cntOp, glyphList, glyphAdvance, positionList, positionAdvance, length);
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
    d->face->release();

  if ((d->vType & VAR_FLAG_STATIC) == 0)
    MemMgr::free(d);
}

// ============================================================================
// [Fog::NullFace]
// ============================================================================

static Static<Face> NullFace_oInstance;
static FaceVTable NullFace_vtable;

static void FOG_CDECL NullFace_create(Face* self)
{
  fog_new_p(self) Face(&NullFace_vtable, StringW::getEmptyInstance());
}

static void FOG_CDECL NullFace_destroy(Face* self)
{
  self->~Face();
}

static OT_Table* FOG_CDECL NullFace_getTable(const Face* self, uint32_t tag)
{
  return NULL;
}

static void FOG_CDECL NullFace_releaseTable(const Face* self, OT_Table* table)
{
  FOG_ASSERT_NOT_REACHED();
}

static err_t FOG_CDECL NullFace_getOutlineFromGlyphRunF(FontData* d,
  PathF* dst, uint32_t cntOp,
  const uint32_t* glyphList, size_t itemAdvance,
  const PointF* positionList, size_t positionAdvance,
  size_t length)
{
  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL NullFace_getOutlineFromGlyphRunD(FontData* d,
  PathD* dst, uint32_t cntOp,
  const uint32_t* glyphList, size_t glyphAdvance,
  const PointF* positionList, size_t positionAdvance,
  size_t length)
{
  return ERR_RT_INVALID_STATE;
}

// ============================================================================
// [Fog::NullFontEngine]
// ============================================================================

static Static<FontEngine> NullFontEngine_oInstance;
static FontEngineVTable NullFontEngine_vtable;

static void NullFontEngine_create(FontEngine* self)
{
  fog_new_p(self) FontEngine(&NullFontEngine_vtable);
  self->defaultFont->_d = Font_oNull->_d;
}

static void NullFontEngine_destroy(FontEngine* self)
{
  self->~FontEngine();
  FOG_UNUSED(self);
}

static err_t FOG_CDECL NullFontEngine_queryFace(const FontEngine* self,
  Face** dst, const StringW* family, const FaceFeatures* features)
{
  FOG_UNUSED(self);
  FOG_UNUSED(dst);
  FOG_UNUSED(family);
  FOG_UNUSED(features);

  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL NullFontEngine_getAvailableFaces(const FontEngine* self,
  FaceCollection* dst)
{
  FOG_UNUSED(self);

  dst->clear();
  return ERR_OK;
}

static err_t FOG_CDECL NullFontEngine_getDefaultFace(const FontEngine* self,
  FaceInfo* dst)
{
  
  return ERR_OK;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

#if defined(FOG_FONT_WINDOWS)
FOG_NO_EXPORT void Font_init_win(void);
#endif // FOG_FONT_WINDOWS

#if defined(FOG_FONT_MAC)
FOG_NO_EXPORT void Font_init_mac(void);
#endif // FOG_FONT_MAC

#if defined(FOG_FONT_FREETYPE)
FOG_NO_EXPORT void Font_init_freetype(void);
#endif // FOG_FONT_FREETYPE

FOG_NO_EXPORT void Font_init(void)
{
  // --------------------------------------------------------------------------
  // [FaceInfo]
  // --------------------------------------------------------------------------

  fog_api.faceinfo_ctor = FaceInfo_ctor;
  fog_api.faceinfo_ctorCopy = FaceInfo_ctorCopy;
  fog_api.faceinfo_dtor = FaceInfo_dtor;

  fog_api.faceinfo_detach = FaceInfo_detach;
  fog_api.faceinfo_setFeatures = FaceInfo_setFeatures;
  fog_api.faceinfo_setFamilyName = FaceInfo_setFamilyName;
  fog_api.faceinfo_setFileName = FaceInfo_setFileName;
  fog_api.faceinfo_setMetrics = FaceInfo_setMetrics;
  fog_api.faceinfo_reset = FaceInfo_reset;
  fog_api.faceinfo_copy = FaceInfo_copy;
  fog_api.faceinfo_eq = FaceInfo_eq;
  fog_api.faceinfo_compare = FaceInfo_compare;

  fog_api.faceinfo_dCreate = FaceInfo_dCreate;
  fog_api.faceinfo_dFree = FaceInfo_dFree;

  {
    FaceInfoData* d = &FaceInfo_dNull;

    d->reference.init(1);
    d->vType = VAR_TYPE_FACE_INFO | VAR_FLAG_NONE;
    d->features.setWeight(FONT_WEIGHT_NORMAL);
    d->features.setStretch(FONT_STRETCH_NORMAL);
    d->familyName->_d = fog_api.stringw_oEmpty->_d;
    d->fileName->_d = fog_api.stringw_oEmpty->_d;

    fog_api.faceinfo_oNull = FaceInfo_oNull.initCustom1(d);
  }

  // --------------------------------------------------------------------------
  // [FaceCollection]
  // --------------------------------------------------------------------------

  fog_api.facecollection_ctor = FaceCollection_ctor;
  fog_api.facecollection_ctorCopy = FaceCollection_ctorCopy;
  fog_api.facecollection_dtor = FaceCollection_dtor;

  fog_api.facecollection_detach = FaceCollection_detach;
  fog_api.facecollection_setList = FaceCollection_setList;
  fog_api.facecollection_getFamilyRangeStringW = FaceCollection_getFamilyRangeStringW;
  fog_api.facecollection_getFamilyRangeStubW = FaceCollection_getFamilyRangeStubW;
  fog_api.facecollection_indexOfStringW = FaceCollection_indexOfStringW;
  fog_api.facecollection_indexOfStubW = FaceCollection_indexOfStubW;
  fog_api.facecollection_addItem = FaceCollection_addItem;
  fog_api.facecollection_clear = FaceCollection_clear;
  fog_api.facecollection_reset = FaceCollection_reset;
  fog_api.facecollection_copy = FaceCollection_copy;
  fog_api.facecollection_eq = FaceCollection_eq;

  fog_api.facecollection_dCreate = FaceCollection_dCreate;
  fog_api.facecollection_dFree = FaceCollection_dFree;

  {
    FaceCollectionData* d = &FaceCollection_dNull;

    d->reference.init(1);
    d->vType = VAR_TYPE_FACE_COLLECTION | VAR_FLAG_NONE;
    d->faceList->_d = fog_api.list_untyped_oEmpty->_d;
    d->faceHash->_d = fog_api.hash_unknown_unknown_oEmpty->_d;

    fog_api.facecollection_oNull = FaceCollection_oNull.initCustom1(d);
  }

  // --------------------------------------------------------------------------
  // [FaceCache]
  // --------------------------------------------------------------------------

  fog_api.facecache_ctor = FaceCache_ctor;
  fog_api.facecache_dtor = FaceCache_dtor;

  fog_api.facecache_reset = FaceCache_reset;
  fog_api.facecache_getExactFace = FaceCache_getExactFace;
  fog_api.facecache_getAllFaces = FaceCache_getAllFaces;
  fog_api.facecache_put = FaceCache_put;
  fog_api.facecache_remove = FaceCache_remove;

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

  {
    FontData* d = &Font_dNull;

    d->reference.init(1);
    d->vType = VAR_TYPE_FONT;
    d->flags = NO_FLAGS;
    d->face = &NullFace_oInstance;
    d->metrics.reset();
    d->features.reset();
    d->matrix.reset();
    d->scale = 0.0f;

    fog_api.font_oNull = Font_oNull.initCustom1(d);
  }

  // --------------------------------------------------------------------------
  // [NullFace / NullFontEngine]
  // --------------------------------------------------------------------------

  NullFace_vtable.destroy = NullFace_destroy;
  NullFace_vtable.getTable = NullFace_getTable;
  NullFace_vtable.releaseTable = NullFace_releaseTable;
  NullFace_vtable.getOutlineFromGlyphRunF = NullFace_getOutlineFromGlyphRunF;
  NullFace_vtable.getOutlineFromGlyphRunD = NullFace_getOutlineFromGlyphRunD;
  NullFace_create(&NullFace_oInstance);

  NullFontEngine_vtable.destroy = NullFontEngine_destroy;
  NullFontEngine_vtable.getAvailableFaces = NullFontEngine_getAvailableFaces;
  NullFontEngine_vtable.getDefaultFace = NullFontEngine_getDefaultFace;
  NullFontEngine_vtable.queryFace = NullFontEngine_queryFace;
  NullFontEngine_create(&NullFontEngine_oInstance);

  fog_api.fontengine_oGlobal = &NullFontEngine_oInstance;

  // --------------------------------------------------------------------------
  // [Initialize Native FontEngine]
  // --------------------------------------------------------------------------

#if defined(FOG_FONT_WINDOWS)
  Font_init_win();
#endif // FOG_FONT_WINDOWS

#if defined(FOG_FONT_MAC)
  Font_init_mac();
#endif // FOG_FONT_MAC

#if defined(FOG_FONT_FREETYPE)
  Font_init_freetype();
#endif // FOG_FONT_FREETYPE
}

FOG_NO_EXPORT void Font_fini(void)
{
  FontEngine* engine = fog_api.fontengine_oGlobal;
  if (engine != &NullFontEngine_oInstance)
  {
    engine->destroy();
    fog_api.fontengine_oGlobal = &NullFontEngine_oInstance;
  }
}

} // Fog namespace
