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
#include <Fog/Core/IO/FileSystem.h>
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/OS/UserInfo.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Tools/Hash.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Source/Color.h>
#include <Fog/G2d/Text/Font.h>
#include <Fog/G2d/Text/FontManager.h>
#include <Fog/G2d/Text/FontProvider.h>

namespace Fog {

// ============================================================================
// [Fog::Font - Helpers]
// ============================================================================

static FOG_INLINE bool Font_isSupportedUnit(uint32_t unit)
{
  // Supported units:
  //
  //   00: UNIT_NONE,
  //   01: UNIT_PX,
  //   02: UNIT_PT,
  //   03: UNIT_PC,
  //   04: UNIT_IN,
  //   05: UNIT_MM,
  //   06: UNIT_CM.

  return unit < UNIT_PERCENTAGE;
}

static void Font_initValues(FontData* d)
{
  d->letterSpacingMode = FONT_SPACING_MODE_PERCENTAGE;
  d->wordSpacingMode = FONT_SPACING_MODE_PERCENTAGE;
  d->dataFlags = NO_FLAGS;

  d->letterSpacing = 1.0f;
  d->wordSpacing = 1.0f;

  d->hints.reset();
  d->transform.reset();
  d->forceCaching = false;
}

static void Font_updateMetrics(FontData* d, float height, uint32_t unit)
{
  FOG_ASSERT(Font_isSupportedUnit(unit));

  d->unit = unit;

  const FontMetricsF& dm = d->face->designMetrics;
  float scale = height / dm._height;

  int alignedHeight;
  uint32_t dataFlags = dataFlags & ~(FONT_DATA_IS_ALIGNED | FONT_DATA_IS_PHYSICAL);

  d->metrics._height = height;
  d->metrics._ascent = dm._ascent * scale;
  d->metrics._descent = dm._descent * scale;
  d->metrics._averageWidth = dm._averageWidth * scale;
  d->metrics._maximumWidth = dm._maximumWidth * scale;

  if (Math::isFuzzyToInt(height, alignedHeight))
  {
    dataFlags |= FONT_DATA_IS_ALIGNED;
  }

  if (d->hints.getKerning() != FONT_KERNING_DETECT &&
      d->hints.getHinting() != FONT_HINTING_DETECT &&
      d->hints.getAlignMode() != FONT_ALIGN_MODE_DETECT &&
      d->hints.getQuality() != FONT_QUALITY_DETECT )
  {
    dataFlags |= FONT_DATA_IS_PHYSICAL;
  }

  d->alignedHeight = (uint32_t)alignedHeight;
  d->scale = scale;
  d->dataFlags = dataFlags;
}

// ============================================================================
// [Fog::Font - Construction / Destruction]
// ============================================================================

static void FOG_CDECL Font_ctor(Font* self)
{
  self->_d = FontManager::getGlobal()._d->defaultFont._d->addRef();
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

  FontData* newd = _api.font.dCreate();
  if (FOG_IS_NULL(newd))
    return ERR_RT_OUT_OF_MEMORY;

  newd->unit = d->unit;
  newd->letterSpacingMode = d->letterSpacingMode;
  newd->wordSpacingMode = d->wordSpacingMode;
  newd->dataFlags = d->dataFlags;
  newd->face = NULL;

  newd->letterSpacing = d->letterSpacing;
  newd->wordSpacing = d->wordSpacing;

  newd->hints = d->hints;
  newd->transform = d->transform;
  newd->metrics = d->metrics;

  newd->forceCaching = d->forceCaching;

  if (d->face != NULL)
    newd->face = d->face->addRef();

  atomicPtrXchg(&self->_d, newd)->release();
  return ERR_OK;
}

// ============================================================================
// [Fog::Font - Accessors]
// ============================================================================

static err_t FOG_CDECL Font_setHeight(Font* self, float height, uint32_t unit)
{
  FontData* d = self->_d;
  FOG_ASSERT(d != NULL);

  if (d->metrics.getHeight() == height && d->unit == unit)
    return ERR_OK;

  if (!Font_isSupportedUnit(unit))
    return ERR_RT_INVALID_ARGUMENT;

  if (d->reference.get() != 1)
  {
    FOG_RETURN_ON_ERROR(_api.font.detach(self));
    d = self->_d;
  }

  Font_updateMetrics(d, height, unit);
  return ERR_OK;
}

static err_t FOG_CDECL Font_setLetterSpacing(Font* self, float spacing, uint32_t spacingMode)
{
  FontData* d = self->_d;
  FOG_ASSERT(d != NULL);

  if (d->letterSpacing == spacing && d->letterSpacingMode == spacingMode)
    return ERR_OK;

  if (spacingMode >= FONT_SPACING_MODE_COUNT)
    return ERR_RT_INVALID_ARGUMENT;

  if (d->reference.get() != 1)
  {
    FOG_RETURN_ON_ERROR(_api.font.detach(self));
    d = self->_d;
  }

  d->dataFlags &= ~FONT_DATA_HAS_LETTER_SPACING;
  d->letterSpacing = spacing;
  d->letterSpacingMode = spacingMode;

  if (d->letterSpacing != 1.0f || spacingMode != FONT_SPACING_MODE_PERCENTAGE)
    d->dataFlags |= FONT_DATA_HAS_LETTER_SPACING;

  return ERR_OK;
}

static err_t FOG_CDECL Font_setWordSpacing(Font* self, float spacing, uint32_t spacingMode)
{
  FontData* d = self->_d;
  FOG_ASSERT(d != NULL);

  if (d->wordSpacing == spacing && d->wordSpacingMode == spacingMode)
    return ERR_OK;

  if (d->reference.get() != 1)
  {
    FOG_RETURN_ON_ERROR(_api.font.detach(self));
    d = self->_d;
  }

  d->dataFlags &= ~FONT_DATA_HAS_WORD_SPACING;
  d->wordSpacing = spacing;
  d->wordSpacingMode = spacingMode;

  if (d->letterSpacing != 1.0f || spacingMode != FONT_SPACING_MODE_PERCENTAGE)
    d->dataFlags |= FONT_DATA_HAS_WORD_SPACING;

  return ERR_OK;
}

static err_t FOG_CDECL Font_setHints(Font* self, const FontHints* hints)
{
  FontData* d = self->_d;
  FOG_ASSERT(d != NULL);

  if (d->hints == *hints)
    return ERR_OK;

  if (d->reference.get() != 1)
  {
    FOG_RETURN_ON_ERROR(_api.font.detach(self));
    d = self->_d;
  }

  d->hints = *hints;
  return ERR_OK;
}

static err_t FOG_CDECL Font_setStyle(Font* self, uint32_t style)
{
  FontData* d = self->_d;
  FOG_ASSERT(d != NULL);

  if (d->hints.getStyle() == style)
    return ERR_OK;

  FontHints newHints(d->hints);
  newHints.setStyle(style);

  return self->setHints(newHints);
}

static err_t FOG_CDECL Font_setWeight(Font* self, uint32_t weight)
{
  FontData* d = self->_d;
  FOG_ASSERT(d != NULL);

  if (d->hints.getWeight() == weight)
    return ERR_OK;

  FontHints newHints(d->hints);
  newHints.setWeight(weight);

  return self->setHints(newHints);
}

static err_t FOG_CDECL Font_setVariant(Font* self, uint32_t variant)
{
  FontData* d = self->_d;
  FOG_ASSERT(d != NULL);

  if (d->hints.getVariant() == variant)
    return ERR_OK;

  FontHints newHints(d->hints);
  newHints.setVariant(variant);

  return self->setHints(newHints);
}

static err_t FOG_CDECL Font_setDecoration(Font* self, uint32_t decoration)
{
  FontData* d = self->_d;
  FOG_ASSERT(d != NULL);

  if (d->hints.getDecoration() == decoration)
    return ERR_OK;

  FontHints newHints(d->hints);
  newHints.setDecoration(decoration);

  return self->setHints(newHints);
}

static err_t FOG_CDECL Font_setKerning(Font* self, uint32_t kerning)
{
  FontData* d = self->_d;
  FOG_ASSERT(d != NULL);

  if (d->hints.getKerning() == kerning)
    return ERR_OK;

  FontHints newHints(d->hints);
  newHints.setKerning(kerning);

  return self->setHints(newHints);
}

static err_t FOG_CDECL Font_setHinting(Font* self, uint32_t hinting)
{
  FontData* d = self->_d;
  FOG_ASSERT(d != NULL);

  if (d->hints.getHinting() == hinting)
    return ERR_OK;

  FontHints newHints(d->hints);
  newHints.setHinting(hinting);

  return self->setHints(newHints);
}

static err_t FOG_CDECL Font_setAlignMode(Font* self, uint32_t alignMode)
{
  FontData* d = self->_d;
  FOG_ASSERT(d != NULL);

  if (d->hints.getAlignMode() == alignMode)
    return ERR_OK;

  FontHints newHints(d->hints);
  newHints.setAlignMode(alignMode);

  return self->setHints(newHints);
}

static err_t FOG_CDECL Font_setTransform(Font* self, const TransformF* transform)
{
  FontData* d = self->_d;
  FOG_ASSERT(d != NULL);

  if (d->transform == *transform)
    return ERR_OK;

  uint32_t transformType = transform->getType();
  if (transformType == TRANSFORM_TYPE_DEGENERATE)
    return ERR_GEOMETRY_DEGENERATE;

  if (d->reference.get() != 1)
  {
    FOG_RETURN_ON_ERROR(self->detach());
    d = self->_d;
  }

  d->dataFlags &= ~FONT_DATA_HAS_TRANSFORM;
  d->transform = *transform;

  if (transformType != TRANSFORM_TYPE_IDENTITY)
    d->dataFlags |= FONT_DATA_HAS_TRANSFORM;

  return ERR_OK;
}

static err_t FOG_CDECL Font_setForceCaching(Font* self, bool val)
{
  FontData* d = self->_d;
  FOG_ASSERT(d != NULL);

  if (d->forceCaching == val)
    return ERR_OK;

  if (d->reference.get() != 1)
  {
    FOG_RETURN_ON_ERROR(_api.font.detach(self));
    d = self->_d;
  }

  if (val)
  {
    // Force caching on.
    d->forceCaching = true;

    // TODO: Font caching.
  }
  else
  {
    // Force caching off.
    d->forceCaching = false;

    // TODO: Font caching.
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::Font - Reset]
// ============================================================================

static void FOG_CDECL Font_reset(Font* self)
{
  FOG_ASSERT(self->_d != NULL);
  atomicPtrXchg(&self->_d, FontManager::getGlobal()._d->defaultFont._d->addRef())->release();
}

// ============================================================================
// [Fog::Font - Create]
// ============================================================================

static err_t FOG_CDECL Font_create(Font* self, const StringW* family, float height, uint32_t unit)
{
  FontData* d = self->_d;
  FOG_ASSERT(d != NULL);

  if (!Font_isSupportedUnit(unit))
    return ERR_RT_INVALID_ARGUMENT;

  if (d->reference.get() != 1)
  {
    FOG_RETURN_ON_ERROR(_api.font.detach(self));
    d = self->_d;
  }

  // Try to optimize font-face creation using the current font-face.
  if (d->face->family != *family)
  {
    FontFace* face = FontManager::getGlobal().getFontFace(*family);
    if (FOG_IS_NULL(face))
      return ERR_FONT_NOT_MATCHED;

    face = atomicPtrXchg(&d->face, face);
    if (face != NULL) face->deref();
  }

  Font_initValues(d);
  Font_updateMetrics(d, height, unit);
  return ERR_OK;
}

static err_t FOG_CDECL Font_createEx(Font* self, const StringW* family, float height, uint32_t unit,
  const FontHints* hints, const TransformF* transform)
{
  FontData* d = self->_d;
  FOG_ASSERT(d != NULL);

  if (!Font_isSupportedUnit(unit))
    return ERR_RT_INVALID_ARGUMENT;

  uint32_t transformType = transform->getType();
  if (transformType == TRANSFORM_TYPE_DEGENERATE)
    return ERR_GEOMETRY_DEGENERATE;

  if (d->reference.get() != 1)
  {
    FOG_RETURN_ON_ERROR(_api.font.detach(self));
    d = self->_d;
  }

  // Try to optimize font-face creation using the current font-face.
  if (d->face->family != *family)
  {
    FontFace* face = FontManager::getGlobal().getFontFace(*family);
    if (FOG_IS_NULL(face)) return ERR_FONT_NOT_MATCHED;

    face = atomicPtrXchg(&d->face, face);
    if (face != NULL) face->deref();
  }

  Font_initValues(d);

  d->hints = *hints;
  if (transformType != TRANSFORM_TYPE_IDENTITY)
  {
    d->transform = *transform;
    d->dataFlags |= FONT_DATA_HAS_TRANSFORM;
  }

  Font_updateMetrics(d, height, unit);
  return ERR_OK;
}

static err_t FOG_CDECL Font_fromFace(Font* self, FontFace* face, float height, uint32_t unit)
{
  FontData* d = self->_d;
  FOG_ASSERT(d != NULL);

  if (!Font_isSupportedUnit(unit))
    return ERR_RT_INVALID_ARGUMENT;

  if (d->reference.get() != 1)
  {
    FOG_RETURN_ON_ERROR(_api.font.detach(self));
    d = self->_d;
  }

  face = atomicPtrXchg(&d->face, face);
  if (face != NULL) face->deref();

  Font_initValues(d);
  Font_updateMetrics(d, height, unit);
  return ERR_OK;
}

// ============================================================================
// [Fog::Font - Methods]
// ============================================================================

static err_t FOG_CDECL Font_getTextOutlineFStubW(const Font* self, PathF* dst, uint32_t cntOp, const PointF* pt, const StubW* str)
{
  FontData* d = self->_d;
  FOG_ASSERT(d != NULL);

  if (cntOp == CONTAINER_OP_REPLACE)
    dst->clear();

  return d->face->getTextOutline(*dst, d, *pt, *str);
}

static err_t FOG_CDECL Font_getTextOutlineFStringW(const Font* self, PathF* dst, uint32_t cntOp, const PointF* pt, const StringW* str)
{
  FontData* d = self->_d;
  FOG_ASSERT(d != NULL);

  if (cntOp == CONTAINER_OP_REPLACE)
    dst->clear();

  return d->face->getTextOutline(*dst, d, *pt, StubW(str->getData(), str->getLength()));
}

static err_t FOG_CDECL Font_getTextOutlineDStubW(const Font* self, PathD* dst, uint32_t cntOp, const PointD* pt, const StubW* str)
{
  FontData* d = self->_d;
  FOG_ASSERT(d != NULL);

  if (cntOp == CONTAINER_OP_REPLACE)
    dst->clear();

  return d->face->getTextOutline(*dst, d, *pt, *str);
}

static err_t FOG_CDECL Font_getTextOutlineDStringW(const Font* self, PathD* dst, uint32_t cntOp, const PointD* pt, const StringW* str)
{
  FontData* d = self->_d;
  FOG_ASSERT(d != NULL);

  if (cntOp == CONTAINER_OP_REPLACE)
    dst->clear();

  return d->face->getTextOutline(*dst, d, *pt, StubW(str->getData(), str->getLength()));
}

static err_t FOG_CDECL Font_getTextExtentsFStubW(const Font* self, TextExtentsF* extents, const StubW* str)
{
  FontData* d = self->_d;
  FOG_ASSERT(d != NULL);

  return d->face->getTextExtents(*extents, d, *str);
}

static err_t FOG_CDECL Font_getTextExtentsFStringW(const Font* self, TextExtentsF* extents, const StringW* str)
{
  FontData* d = self->_d;
  FOG_ASSERT(d != NULL);

  return d->face->getTextExtents(*extents, d, StubW(str->getData(), str->getLength()));
}

static err_t FOG_CDECL Font_getTextExtentsDStubW(const Font* self, TextExtentsD* extents, const StubW* str)
{
  FontData* d = self->_d;
  FOG_ASSERT(d != NULL);

  return d->face->getTextExtents(*extents, d, *str);
}

static err_t FOG_CDECL Font_getTextExtentsDStringW(const Font* self, TextExtentsD* extents, const StringW* str)
{
  FontData* d = self->_d;
  FOG_ASSERT(d != NULL);

  return d->face->getTextExtents(*extents, d, StubW(str->getData(), str->getLength()));
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

  if (a_d->unit != b_d->unit)
    return false;

  if (a_d->letterSpacingMode != b_d->letterSpacingMode)
    return false;

  if (a_d->wordSpacingMode != b_d->wordSpacingMode)
    return false;
    
  if (a_d->dataFlags != b_d->dataFlags)
    return false;

  if (a_d->alignedHeight != b_d->alignedHeight)
    return false;

  if (a_d->scale != b_d->scale)
    return false;

  if (a_d->letterSpacing != b_d->letterSpacing)
    return false;

  if (a_d->wordSpacing != b_d->wordSpacing)
    return false;

  if (a_d->hints != b_d->hints)
    return false;

  if (a_d->transform != b_d->transform)
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
  // [Funcs]
  // --------------------------------------------------------------------------

  _api.font.ctor = Font_ctor;
  _api.font.ctorCopy = Font_ctorCopy;
  _api.font.dtor = Font_dtor;

  _api.font.detach = Font_detach;
  _api.font.setHeight = Font_setHeight;
  _api.font.setLetterSpacing = Font_setLetterSpacing;
  _api.font.setWordSpacing = Font_setWordSpacing;
  _api.font.setHints = Font_setHints;
  _api.font.setStyle = Font_setStyle;
  _api.font.setWeight = Font_setWeight;
  _api.font.setVariant = Font_setVariant;
  _api.font.setDecoration = Font_setDecoration;
  _api.font.setKerning = Font_setKerning;
  _api.font.setHinting = Font_setHinting;
  _api.font.setAlignMode = Font_setAlignMode;

  _api.font.setTransform = Font_setTransform;
  _api.font.setForceCaching = Font_setForceCaching;

  _api.font.reset = Font_reset;

  _api.font.create = Font_create;
  _api.font.createEx = Font_createEx;

  _api.font.fromFace = Font_fromFace;

  _api.font.getTextOutlineFStubW = Font_getTextOutlineFStubW;
  _api.font.getTextOutlineFStringW = Font_getTextOutlineFStringW;
  _api.font.getTextOutlineDStubW = Font_getTextOutlineDStubW;
  _api.font.getTextOutlineDStringW = Font_getTextOutlineDStringW;

  _api.font.getTextExtentsFStubW = Font_getTextExtentsFStubW;
  _api.font.getTextExtentsFStringW = Font_getTextExtentsFStringW;
  _api.font.getTextExtentsDStubW = Font_getTextExtentsDStubW;
  _api.font.getTextExtentsDStringW = Font_getTextExtentsDStringW;

  _api.font.copy = Font_copy;
  _api.font.eq = Font_eq;

  _api.font.dCreate = Font_dCreate;
  _api.font.dFree = Font_dFree;
}

} // Fog namespace
