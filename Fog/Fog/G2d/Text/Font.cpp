// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Collection/Hash.h>
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Global/Debug.h>
#include <Fog/Core/Global/Static.h>
#include <Fog/Core/IO/FileSystem.h>
#include <Fog/Core/Memory/Alloc.h>
#include <Fog/Core/OS/OS.h>
#include <Fog/Core/OS/UserInfo.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Global/Constants.h>
#include <Fog/G2d/Source/Color.h>
#include <Fog/G2d/Text/Font.h>
#include <Fog/G2d/Text/FontManager.h>
#include <Fog/G2d/Text/FontProvider.h>

namespace Fog {

// ============================================================================
// [Fog::Font - Helpers]
// ============================================================================

static FOG_INLINE bool _Font_isSupportedUnit(uint32_t unit)
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

static FOG_INLINE FontData* _Font_dalloc()
{
  FontData* d = reinterpret_cast<FontData*>(Memory::alloc(sizeof(FontData)));
  if (FOG_IS_NULL(d)) return d;

  d->refCount.init(1);
  return d;
}

static FOG_INLINE void _Font_dfree(FontData* d)
{
  if (d->face) d->face->deref();
  Memory::free(d);
}

static FOG_INLINE FontData* _Font_ref(FontData* d)
{
  d->refCount.inc();
  return d;
}

static FOG_INLINE void _Font_deref(FontData* d)
{
  if (d->refCount.deref()) _Font_dfree(d);
}

static void _Font_initValues(FontData* d)
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

static FOG_INLINE void _Font_updateMetrics(FontData* d, float height, uint32_t unit)
{
  FOG_ASSERT(_Font_isSupportedUnit(unit));

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

Font::Font()
{
  _d = _Font_ref(FontManager::getGlobal()._d->defaultFont._d);
}

Font::Font(const Font& other)
{
  _d = _Font_ref(other._d);
}

Font::~Font()
{
  if (FOG_IS_NULL(_d)) return;
  _Font_deref(_d);
}

// ============================================================================
// [Fog::Font - Sharing]
// ============================================================================

err_t Font::_detach()
{
  if (isDetached()) return ERR_OK;

  FontData* newd = _Font_dalloc();
  if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

  newd->face = NULL;

  newd->unit = _d->unit;
  newd->letterSpacingMode = _d->letterSpacingMode;
  newd->wordSpacingMode = _d->wordSpacingMode;
  newd->dataFlags = _d->dataFlags;

  newd->letterSpacing = _d->letterSpacing;
  newd->wordSpacing = _d->wordSpacing;

  newd->hints = _d->hints;
  newd->transform = _d->transform;
  newd->metrics = _d->metrics;

  newd->forceCaching = _d->forceCaching;

  if (_d->face != NULL)
    newd->face = _d->face->ref();

  _Font_deref(atomicPtrXchg(&_d, newd));
  return ERR_OK;
}

// ============================================================================
// [Fog::Font - Accessors]
// ============================================================================

err_t Font::setFont(const Font& other)
{
  FOG_ASSERT(_d != NULL);

  _Font_deref(atomicPtrXchg(&_d, _Font_ref(other._d)));
  return ERR_OK;
}

err_t Font::setHeight(float height, uint32_t unit)
{
  FOG_ASSERT(_d != NULL);

  if (_d->metrics.getHeight() == height && _d->unit == unit)
    return ERR_OK;

  if (!_Font_isSupportedUnit(unit))
    return ERR_RT_INVALID_ARGUMENT;
  FOG_RETURN_ON_ERROR(detach());

  _Font_updateMetrics(_d, height, unit);
  return ERR_OK;
}

err_t Font::setLetterSpacing(float spacing, uint32_t spacingMode)
{
  FOG_ASSERT(_d != NULL);

  if (_d->letterSpacing == spacing && _d->letterSpacingMode == spacingMode)
    return ERR_OK;

  if (spacingMode >= FONT_SPACING_MODE_COUNT)
    return ERR_RT_INVALID_ARGUMENT;

  FOG_RETURN_ON_ERROR(detach());
  _d->dataFlags &= ~FONT_DATA_HAS_LETTER_SPACING;
  _d->letterSpacing = spacing;
  _d->letterSpacingMode = spacingMode;

  if (_d->letterSpacing != 1.0f || spacingMode != FONT_SPACING_MODE_PERCENTAGE)
    _d->dataFlags |= FONT_DATA_HAS_LETTER_SPACING;

  return ERR_OK;
}

err_t Font::setWordSpacing(float spacing, uint32_t spacingMode)
{
  FOG_ASSERT(_d != NULL);

  if (_d->wordSpacing == spacing && _d->wordSpacingMode == spacingMode)
    return ERR_OK;

  FOG_RETURN_ON_ERROR(detach());
  _d->dataFlags &= ~FONT_DATA_HAS_WORD_SPACING;
  _d->wordSpacing = spacing;
  _d->wordSpacingMode = spacingMode;

  if (_d->letterSpacing != 1.0f || spacingMode != FONT_SPACING_MODE_PERCENTAGE)
    _d->dataFlags |= FONT_DATA_HAS_WORD_SPACING;

  return ERR_OK;
}

err_t Font::setHints(const FontHints& hints)
{
  FOG_ASSERT(_d != NULL);

  if (_d->hints == hints)
    return ERR_OK;

  FOG_RETURN_ON_ERROR(detach());
  _d->hints = hints;

  return ERR_OK;
}

err_t Font::setStyle(uint32_t style)
{
  FOG_ASSERT(_d != NULL);

  if (_d->hints.getStyle() == style)
    return ERR_OK;

  FontHints newHints(_d->hints);
  newHints.setStyle(style);

  return setHints(newHints);
}

err_t Font::setWeight(uint32_t weight)
{
  FOG_ASSERT(_d != NULL);

  if (_d->hints.getWeight() == weight)
    return ERR_OK;

  FontHints newHints(_d->hints);
  newHints.setWeight(weight);

  return setHints(newHints);
}

err_t Font::setVariant(uint32_t variant)
{
  FOG_ASSERT(_d != NULL);

  if (_d->hints.getVariant() == variant)
    return ERR_OK;

  FontHints newHints(_d->hints);
  newHints.setVariant(variant);

  return setHints(newHints);
}

err_t Font::setDecoration(uint32_t decoration)
{
  FOG_ASSERT(_d != NULL);

  if (_d->hints.getDecoration() == decoration)
    return ERR_OK;

  FontHints newHints(_d->hints);
  newHints.setDecoration(decoration);

  return setHints(newHints);
}

err_t Font::setKerning(uint32_t kerning)
{
  FOG_ASSERT(_d != NULL);

  if (_d->hints.getKerning() == kerning)
    return ERR_OK;

  FontHints newHints(_d->hints);
  newHints.setKerning(kerning);

  return setHints(newHints);
}

err_t Font::setHinting(uint32_t hinting)
{
  FOG_ASSERT(_d != NULL);

  if (_d->hints.getHinting() == hinting)
    return ERR_OK;

  FontHints newHints(_d->hints);
  newHints.setHinting(hinting);

  return setHints(newHints);
}

err_t Font::setAlignMode(uint32_t alignMode)
{
  FOG_ASSERT(_d != NULL);

  if (_d->hints.getAlignMode() == alignMode)
    return ERR_OK;

  FontHints newHints(_d->hints);
  newHints.setAlignMode(alignMode);

  return setHints(newHints);
}

err_t Font::setTransform(const TransformF& transform)
{
  FOG_ASSERT(_d != NULL);

  if (_d->transform == transform)
    return ERR_OK;

  uint32_t transformType = transform.getType();
  if (transformType == TRANSFORM_TYPE_DEGENERATE)
    return ERR_GEOMETRY_DEGENERATE;

  FOG_RETURN_ON_ERROR(detach());
  _d->dataFlags &= ~FONT_DATA_HAS_TRANSFORM;
  _d->transform = transform;

  if (transformType != TRANSFORM_TYPE_IDENTITY)
    _d->dataFlags |= FONT_DATA_HAS_TRANSFORM;

  return ERR_OK;
}

err_t Font::setForceCaching(bool val)
{
  FOG_ASSERT(_d != NULL);

  if (_d->forceCaching == val)
    return ERR_OK;

  FOG_RETURN_ON_ERROR(detach());

  if (val)
  {
    // Force caching on.
    _d->forceCaching = true;

    // TODO:
  }
  else
  {
    // Force caching off.
    _d->forceCaching = false;

    // TODO:
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::Font - Reset]
// ============================================================================

void Font::reset()
{
  FOG_ASSERT(_d != NULL);

  _Font_deref(_d);
  _d = _Font_ref(FontManager::getGlobal()._d->defaultFont._d);
}

// ============================================================================
// [Fog::Font - Create]
// ============================================================================

err_t Font::create(const String& family, float height, uint32_t unit)
{
  FOG_ASSERT(_d != NULL);

  if (!_Font_isSupportedUnit(unit))
    return ERR_RT_INVALID_ARGUMENT;

  FOG_RETURN_ON_ERROR(detach());

  // Try to optimize font-face creation using the current font-face.
  if (_d->face->family != family)
  {
    FontFace* face = FontManager::getGlobal().getFontFace(family);
    if (FOG_IS_NULL(face)) return ERR_FONT_NOT_MATCHED;

    if (FOG_IS_ERROR(detach()))
    {
      face->deref();
      return ERR_RT_OUT_OF_MEMORY;
    }

    face = atomicPtrXchg(&_d->face, face);
    if (face != NULL) face->deref();
  }

  _Font_initValues(_d);
  _Font_updateMetrics(_d, height, unit);
  return ERR_OK;
}

err_t Font::create(const String& family, float height, uint32_t unit,
  const FontHints& hints, const TransformF& transform)
{
  if (!_Font_isSupportedUnit(unit))
    return ERR_RT_INVALID_ARGUMENT;

  uint32_t transformType = transform.getType();
  if (transformType == TRANSFORM_TYPE_DEGENERATE)
    return ERR_GEOMETRY_DEGENERATE;

  FOG_RETURN_ON_ERROR(detach());

  // Try to optimize font-face creation using the current font-face.
  if (_d->face->family != family)
  {
    FontFace* face = FontManager::getGlobal().getFontFace(family);
    if (FOG_IS_NULL(face)) return ERR_FONT_NOT_MATCHED;

    if (FOG_IS_ERROR(detach()))
    {
      face->deref();
      return ERR_RT_OUT_OF_MEMORY;
    }

    face = atomicPtrXchg(&_d->face, face);
    if (face != NULL) face->deref();
  }

  _Font_initValues(_d);

  _d->hints = hints;
  if (transformType != TRANSFORM_TYPE_IDENTITY)
  {
    _d->transform = transform;
    _d->dataFlags |= FONT_DATA_HAS_TRANSFORM;
  }

  _Font_updateMetrics(_d, height, unit);
  return ERR_OK;
}

err_t Font::_fromFace(FontFace* face, float height, uint32_t unit)
{
  FOG_ASSERT(_d != NULL);

  if (!_Font_isSupportedUnit(unit))
    return ERR_RT_INVALID_ARGUMENT;

  FOG_RETURN_ON_ERROR(detach());

  face = atomicPtrXchg(&_d->face, face);
  if (face != NULL) face->deref();

  _Font_initValues(_d);
  _Font_updateMetrics(_d, height, unit);
  return ERR_OK;
}

// ============================================================================
// [Fog::Font - Methods]
// ============================================================================

err_t Font::getTextOutline(PathF& dst, const PointF& pt, const String& str, uint32_t cntOp) const
{
  FOG_ASSERT(_d != NULL);

  if (cntOp == CONTAINER_OP_REPLACE) dst.clear();
  return _d->face->getTextOutline(dst, _d, pt, Utf16(str.getData(), str.getLength()));
}

err_t Font::getTextOutline(PathF& dst, const PointF& pt, const Utf16& str, uint32_t cntOp) const
{
  FOG_ASSERT(_d != NULL);

  if (cntOp == CONTAINER_OP_REPLACE) dst.clear();
  return _d->face->getTextOutline(dst, _d, pt, str);
}

err_t Font::getTextOutline(PathD& dst, const PointD& pt, const String& str, uint32_t cntOp) const
{
  FOG_ASSERT(_d != NULL);

  if (cntOp == CONTAINER_OP_REPLACE) dst.clear();
  return _d->face->getTextOutline(dst, _d, pt, Utf16(str.getData(), str.getLength()));
}

err_t Font::getTextOutline(PathD& dst, const PointD& pt, const Utf16& str, uint32_t cntOp) const
{
  FOG_ASSERT(_d != NULL);

  if (cntOp == CONTAINER_OP_REPLACE) dst.clear();
  return _d->face->getTextOutline(dst, _d, pt, str);
}

err_t Font::getTextExtents(TextExtents& extents, const String& str) const
{
  FOG_ASSERT(_d != NULL);

  return _d->face->getTextExtents(extents, _d, Utf16(str.getData(), str.getLength()));
}

err_t Font::getTextExtents(TextExtents& extents, const Utf16& str) const
{
  FOG_ASSERT(_d != NULL);

  return _d->face->getTextExtents(extents, _d, str);
}

} // Fog namespace
