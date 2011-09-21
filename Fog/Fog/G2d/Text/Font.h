// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_FONT_H
#define _FOG_G2D_TEXT_FONT_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Tools/Char.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Geometry/Size.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Text/FontFace.h>
#include <Fog/G2d/Text/FontHints.h>
#include <Fog/G2d/Text/FontKerning.h>
#include <Fog/G2d/Text/FontMetrics.h>

namespace Fog {

//! @addtogroup Fog_G2d_Text
//! @{

// ============================================================================
// [Fog::FontData]
// ============================================================================

struct FOG_NO_EXPORT FontData
{
  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  FOG_INLINE FontData* addRef() const
  {
    reference.inc();
    return const_cast<FontData*>(this);
  }

  FOG_INLINE void release()
  {
    if (reference.deref())
      _api.font.dFree(this);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  // ${VAR:BEGIN}
  //
  // This data-object is binary compatible to the VarData header in the first
  // form called - "implicitly shared class". The members must be binary
  // compatible to the header below:
  //
  // +==============+============+============================================+
  // | Size         | Name       | Description / Purpose                      |
  // +==============+============+============================================+
  // | size_t       | reference  | Atomic reference count, can be managed by  |
  // |              |            | VarData without calling container specific |
  // |              |            | methods.                                   |
  // +--------------+------------+--------------------------------------------+
  // | uint32_t     | vType      | Variable type and flags.                   |
  // +==============+============+============================================+
  //
  // ${VAR:END}

  //! @brief Reference count.
  mutable Atomic<size_t> reference;

  //! @brief Variable type and flags.
  uint32_t vType;

  //! @brief Unit of all font attributes.
  uint8_t unit;
  //! @brief Letter spacing mode.
  uint8_t letterSpacingMode;
  //! @brief Word spacing mode.
  uint8_t wordSpacingMode;
  //! @brief Font data flags (various interesting flags for internal use).
  uint8_t dataFlags;

  //! @brief Font-face.
  FontFace* face;

  //! @brief Aligned height (should be used only when dataFlags member contains
  //! @c FONT_DATA_IS_ALIGNED flag).
  uint32_t alignedHeight;
  //! @brief Scale constant to get the scaled metrics from the design-metrics.
  float scale;

  //! @brief Letter spacing value.
  float letterSpacing;
  //! @brief Word spacing value.
  float wordSpacing;

  //! @brief Font hints.
  FontHints hints;
  //! @brief Font transform.
  TransformF transform;
  //! @brief Font metrics.
  FontMetricsF metrics;

  //! @brief Whether to force caching.
  //!
  //! Caching is done by font management automatically, but fonts which are
  //! not aligned to a pixel grid or rotated fonts are normally not cached.
  //! This variable can force caching on for these, but memory usage is
  //! rapidly increased.
  bool forceCaching;
};

// ============================================================================
// [Fog::Font]
// ============================================================================

struct FOG_NO_EXPORT Font
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE Font()
  {
    _api.font.ctor(this);
  }

  FOG_INLINE Font(const Font& other)
  {
    _api.font.ctorCopy(this, &other);
  }

  explicit FOG_INLINE Font(FontData* d) :
    _d(d)
  {
  }

  FOG_INLINE ~Font()
  {
    _api.font.dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getReference() const { return _d->reference.get(); }
  FOG_INLINE bool isDetached() const { return _d->reference.get() == 1; }

  FOG_INLINE err_t detach() { return isDetached() ? (err_t)ERR_OK : _detach(); }
  FOG_INLINE err_t _detach() { return _api.font.detach(this); }

  // --------------------------------------------------------------------------
  // [Flags]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isPhysical() const { return (_d->dataFlags & FONT_DATA_IS_PHYSICAL) != 0; }
  FOG_INLINE bool isAligned() const { return (_d->dataFlags & FONT_DATA_IS_ALIGNED) != 0; }

  FOG_INLINE bool hasLetterSpacing() const { return (_d->dataFlags & FONT_DATA_HAS_LETTER_SPACING) != 0; }
  FOG_INLINE bool hasWordSpacing() const { return (_d->dataFlags & FONT_DATA_HAS_WORD_SPACING) != 0; }
  FOG_INLINE bool hasTransform() const { return (_d->dataFlags & FONT_DATA_HAS_TRANSFORM) != 0; }

  // --------------------------------------------------------------------------
  // [Accessors - Font]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setFont(const Font& other)
  {
    return _api.font.copy(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Accessors - Face]
  // --------------------------------------------------------------------------

  FOG_INLINE FontFace* getFace() const { return _d->face; }
  FOG_INLINE uint32_t getFaceId() const { return _d->face->id; }

  FOG_INLINE uint32_t getFeatures() const { return _d->face->features; }
  FOG_INLINE const StringW& getFamily() const { return _d->face->family; }

  // --------------------------------------------------------------------------
  // [Accessors - Unit / Height]
  // --------------------------------------------------------------------------

  FOG_INLINE float getHeight() const
  {
    return _d->metrics.getHeight();
  }

  FOG_INLINE uint32_t getUnit() const
  {
    return _d->unit;
  }

  //! @brief Set font-height to @a height and font-units to @a unit.
  FOG_INLINE err_t setHeight(float height, uint32_t unit)
  {
    return _api.font.setHeight(this, height, unit);
  }

  // --------------------------------------------------------------------------
  // [Accessors - Spacing]
  // --------------------------------------------------------------------------

  FOG_INLINE float getLetterSpacing() const { return _d->letterSpacing; }
  FOG_INLINE float getWordSpacing() const { return _d->wordSpacing; }

  FOG_INLINE uint32_t getLetterSpacingMode() const { return _d->letterSpacingMode; }
  FOG_INLINE uint32_t getWordSpacingMode() const { return _d->wordSpacingMode; }

  FOG_INLINE err_t setLetterSpacing(float spacing, uint32_t spacingMode)
  {
    return _api.font.setLetterSpacing(this, spacing, spacingMode);
  }

  FOG_INLINE err_t setWordSpacing(float spacing, uint32_t spacingMode)
  {
    return _api.font.setWordSpacing(this, spacing, spacingMode);
  }

  // --------------------------------------------------------------------------
  // [Accessors - Hints]
  // --------------------------------------------------------------------------

  FOG_INLINE const FontHints& getHints() const { return _d->hints; }
  FOG_INLINE uint32_t getStyle() const { return _d->hints.getStyle(); }
  FOG_INLINE uint32_t getWeight() const { return _d->hints.getWeight(); }
  FOG_INLINE uint32_t getVariant() const { return _d->hints.getVariant(); }
  FOG_INLINE uint32_t getDecoration() const { return _d->hints.getDecoration(); }
  FOG_INLINE uint32_t getKerning() const { return _d->hints.getKerning(); }
  FOG_INLINE uint32_t getHinting() const { return _d->hints.getHinting(); }
  FOG_INLINE uint32_t getAlignMode() const { return _d->hints.getAlignMode(); }

  FOG_INLINE err_t setHints(const FontHints& hints)
  {
    return _api.font.setHints(this, &hints);
  }

  FOG_INLINE err_t setStyle(uint32_t style)
  {
    return _api.font.setStyle(this, style);
  }

  FOG_INLINE err_t setWeight(uint32_t weight)
  {
    return _api.font.setWeight(this, weight);
  }

  FOG_INLINE err_t setVariant(uint32_t variant)
  {
    return _api.font.setVariant(this, variant);
  }

  FOG_INLINE err_t setDecoration(uint32_t decoration)
  {
    return _api.font.setDecoration(this, decoration);
  }

  FOG_INLINE err_t setKerning(uint32_t kerning)
  {
    return _api.font.setKerning(this, kerning);
  }

  FOG_INLINE err_t setHinting(uint32_t hinting)
  {
    return _api.font.setHinting(this, hinting);
  }

  FOG_INLINE err_t setAlignMode(uint32_t alignMode)
  {
    return _api.font.setAlignMode(this, alignMode);
  }

  // --------------------------------------------------------------------------
  // [Accessors - Transform]
  // --------------------------------------------------------------------------

  FOG_INLINE const TransformF& getTransform() const
  {
    return _d->transform;
  }

  FOG_INLINE err_t setTransform(const TransformF& transform)
  {
    return _api.font.setTransform(this, &transform);
  }

  // --------------------------------------------------------------------------
  // [Accessors - Metrics]
  // --------------------------------------------------------------------------

  FOG_INLINE const FontMetricsF& getMetrics() const
  {
    return _d->metrics;
  }

  // --------------------------------------------------------------------------
  // [Accessors - Caching]
  // --------------------------------------------------------------------------

  FOG_INLINE bool getForceCaching() const
  {
    return _d->forceCaching;
  }

  FOG_INLINE err_t setForceCaching(bool val)
  {
    return _api.font.setForceCaching(this, val);
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _api.font.reset(this);
  }

  // --------------------------------------------------------------------------
  // [Create]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t create(const StringW& family, float height, uint32_t unit)
  {
    return _api.font.create(this, &family, height, unit);
  }

  FOG_INLINE err_t create(const StringW& family, float height, uint32_t unit,
    const FontHints& hints, const TransformF& transform)
  {
    return _api.font.createEx(this, &family, height, unit, &hints, &transform);
  }

  FOG_INLINE err_t _fromFace(FontFace* face, float height, uint32_t unit)
  {
    return _api.font.fromFace(this, face, height, unit);
  }

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t getTextOutline(PathF& dst, uint32_t cntOp, const PointF& pt, const StubW& str) const
  {
    return _api.font.getTextOutlineFStubW(this, &dst, cntOp, &pt, &str);
  }

  FOG_INLINE err_t getTextOutline(PathF& dst, uint32_t cntOp, const PointF& pt, const StringW& str) const
  {
    return _api.font.getTextOutlineFStringW(this, &dst, cntOp, &pt, &str);
  }

  FOG_INLINE err_t getTextOutline(PathD& dst, uint32_t cntOp, const PointD& pt, const StubW& str) const
  {
    return _api.font.getTextOutlineDStubW(this, &dst, cntOp, &pt, &str);
  }

  FOG_INLINE err_t getTextOutline(PathD& dst, uint32_t cntOp, const PointD& pt, const StringW& str) const
  {
    return _api.font.getTextOutlineDStringW(this, &dst, cntOp, &pt, &str);
  }

  FOG_INLINE err_t getTextExtents(TextExtentsF& extents, const StubW& str) const
  {
    return _api.font.getTextExtentsFStubW(this, &extents, &str);
  }

  FOG_INLINE err_t getTextExtents(TextExtentsF& extents, const StringW& str) const
  {
    return _api.font.getTextExtentsFStringW(this, &extents, &str);
  }

  FOG_INLINE err_t getTextExtents(TextExtentsD& extents, const StubW& str) const
  {
    return _api.font.getTextExtentsDStubW(this, &extents, &str);
  }

  FOG_INLINE err_t getTextExtents(TextExtentsD& extents, const StringW& str) const
  {
    return _api.font.getTextExtentsDStringW(this, &extents, &str);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const Font& other) const
  {
    return _api.font.eq(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE Font& operator=(const Font& other)
  {
    _api.font.copy(this, &other);
    return *this;
  }

  FOG_INLINE bool operator==(const Font& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const Font& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Statics - Equality]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const Font* a, const Font* b)
  {
    return _api.font.eq(a, b);
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)_api.font.eq;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(FontData)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TEXT_FONT_H
