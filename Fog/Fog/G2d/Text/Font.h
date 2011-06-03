// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_FONT_H
#define _FOG_G2D_TEXT_FONT_H

// [Dependencies]
#include <Fog/Core/Global/Class.h>
#include <Fog/Core/Global/Swap.h>
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Memory/MemoryAlloc.h>
#include <Fog/Core/Memory/MemoryOps.h>
#include <Fog/Core/Tools/Char.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Geometry/Size.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Global/Constants.h>
#include <Fog/G2d/Text/FontFace.h>
#include <Fog/G2d/Text/FontHints.h>
#include <Fog/G2d/Text/FontKerning.h>
#include <Fog/G2d/Text/FontMetrics.h>

namespace Fog {

//! @addtogroup Fog_G2d_Text
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct Font;
struct FontData;
struct FontManager;

struct GlyphSet;
struct PathF;
struct PathD;
struct TextExtents;

// ============================================================================
// [Fog::FontData]
// ============================================================================

struct FOG_NO_EXPORT FontData
{
  //! @brief Reference count.
  mutable Atomic<size_t> refCount;

  //! @brief Font-face.
  FontFace* face;

  //! @brief Unit of all font attributes.
  uint8_t unit;
  //! @brief Letter spacing mode.
  uint8_t letterSpacingMode;
  //! @brief Word spacing mode.
  uint8_t wordSpacingMode;
  //! @brief Font data flags (various interesting flags for internal use).
  uint8_t dataFlags;

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

struct FOG_API Font
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  Font();
  Font(const Font& other);
  ~Font();

  explicit FOG_INLINE Font(FontData* d) : _d(d) {}

  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getReference() const { return _d->refCount.get(); }

  FOG_INLINE bool isDetached() const { return _d->refCount.get() == 1; }
  FOG_INLINE err_t detach() { return isDetached() ? (err_t)ERR_OK : _detach(); }

  err_t _detach();

  // --------------------------------------------------------------------------
  // [Flags]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isPhysical() const { return (_d->dataFlags & FONT_DATA_IS_PHYSICAL) != 0; }
  FOG_INLINE bool isAligned() const { return (_d->dataFlags & FONT_DATA_IS_ALIGNED) != 0; }

  FOG_INLINE bool hasLetterSpacing() const { return (_d->dataFlags & FONT_DATA_HAS_LETTER_SPACING) != 0; }
  FOG_INLINE bool hasWordSpacing() const { return (_d->dataFlags & FONT_DATA_HAS_WORD_SPACING) != 0; }
  FOG_INLINE bool hasTransform() const { return (_d->dataFlags & FONT_DATA_HAS_TRANSFORM) != 0; }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  // Font.

  err_t setFont(const Font& other);

  // Face.

  FOG_INLINE FontFace* getFace() const { return _d->face; }
  FOG_INLINE uint32_t getFaceId() const { return _d->face->id; }

  FOG_INLINE uint32_t getFeatures() const { return _d->face->features; }
  FOG_INLINE const String& getFamily() const { return _d->face->family; }

  // Unit / Height.

  FOG_INLINE uint32_t getUnit() const { return _d->unit; }
  FOG_INLINE float getHeight() const { return _d->metrics.getHeight(); }

  //! @brief Set font-height to @a height and font-units to @a unit.
  err_t setHeight(float height, uint32_t unit);

  // Spacing.

  FOG_INLINE float getLetterSpacing() const { return _d->letterSpacing; }
  FOG_INLINE float getWordSpacing() const { return _d->wordSpacing; }

  FOG_INLINE uint32_t getLetterSpacingMode() const { return _d->letterSpacingMode; }
  FOG_INLINE uint32_t getWordSpacingMode() const { return _d->wordSpacingMode; }

  err_t setLetterSpacing(float spacing, uint32_t spacingMode);
  err_t setWordSpacing(float spacing, uint32_t spacingMode);

  // Hints.

  FOG_INLINE const FontHints& getHints() const { return _d->hints; }
  err_t setHints(const FontHints& hints);

  FOG_INLINE uint32_t getStyle() const { return _d->hints.getStyle(); }
  FOG_INLINE uint32_t getWeight() const { return _d->hints.getWeight(); }
  FOG_INLINE uint32_t getVariant() const { return _d->hints.getVariant(); }
  FOG_INLINE uint32_t getDecoration() const { return _d->hints.getDecoration(); }
  FOG_INLINE uint32_t getKerning() const { return _d->hints.getKerning(); }
  FOG_INLINE uint32_t getHinting() const { return _d->hints.getHinting(); }
  FOG_INLINE uint32_t getAlignMode() const { return _d->hints.getAlignMode(); }

  err_t setStyle(uint32_t style);
  err_t setWeight(uint32_t weight);
  err_t setVariant(uint32_t variant);
  err_t setDecoration(uint32_t decoration);
  err_t setKerning(uint32_t kerning);
  err_t setHinting(uint32_t hinting);
  err_t setAlignMode(uint32_t alignMode);

  // Transform.

  FOG_INLINE const TransformF& getTransform() const { return _d->transform; }
  err_t setTransform(const TransformF& transform);

  // Metrics.

  FOG_INLINE const FontMetricsF& getMetrics() const { return _d->metrics; }

  // Caching.

  FOG_INLINE bool getForceCaching() const { return _d->forceCaching; }
  err_t setForceCaching(bool val);

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  void reset();

  // --------------------------------------------------------------------------
  // [Create]
  // --------------------------------------------------------------------------

  err_t create(const String& family, float height, uint32_t unit);
  err_t create(const String& family, float height, uint32_t unit,
    const FontHints& hints, const TransformF& transform);

  err_t _fromFace(FontFace* face, float height, uint32_t unit);

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  err_t getTextOutline(PathF& dst, const PointF& pt, const String& str, uint32_t cntOp = CONTAINER_OP_REPLACE) const;
  err_t getTextOutline(PathF& dst, const PointF& pt, const Utf16& str, uint32_t cntOp = CONTAINER_OP_REPLACE) const;

  err_t getTextOutline(PathD& dst, const PointD& pt, const String& str, uint32_t cntOp = CONTAINER_OP_REPLACE) const;
  err_t getTextOutline(PathD& dst, const PointD& pt, const Utf16& str, uint32_t cntOp = CONTAINER_OP_REPLACE) const;

  err_t getTextExtents(TextExtents& extents, const String& str) const;
  err_t getTextExtents(TextExtents& extents, const Utf16& str) const;

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE Font& operator=(const Font& other)
  {
    setFont(other);
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(FontData)
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPEINFO_DECLARE(Fog::Font, Fog::TYPEINFO_MOVABLE)

// ============================================================================
// [Fog::Swap]
// ============================================================================

_FOG_SWAP_D(Fog::Font)

// [Guard]
#endif // _FOG_G2D_TEXT_FONT_H
