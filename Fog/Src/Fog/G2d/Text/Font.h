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

namespace Fog {

//! @addtogroup Fog_G2d_Text
//! @{

// ============================================================================
// [Fog::FontMetrics]
// ============================================================================

struct FOG_NO_EXPORT FontMetrics
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------
  
  FOG_INLINE FontMetrics()
  {
    MemOps::zero_t<FontMetrics>(this);
  }

  explicit FOG_INLINE FontMetrics(_Uninitialized) {}

  FOG_INLINE FontMetrics(const FontMetrics& other)
  {
    MemOps::copy_t<FontMetrics>(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE float getSize() const { return _size; }
  FOG_INLINE void setSize(float size) { _size = size; }

  FOG_INLINE float getAscent() const { return _ascent; }
  FOG_INLINE void setAscent(float ascent) { _ascent = ascent; }

  FOG_INLINE float getDescent() const { return _descent; }
  FOG_INLINE void setDescent(float descent) { _descent = descent; }

  FOG_INLINE float getCapHeight() const { return _capHeight; }
  FOG_INLINE void setCapHeight(float capHeight) { _capHeight = capHeight; }

  FOG_INLINE float getXHeight() const { return _xHeight; }
  FOG_INLINE void setXHeight(float xHeight) { _xHeight = xHeight; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    MemOps::zero_t<FontMetrics>(this);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE FontMetrics& operator=(const FontMetrics& other)
  {
    MemOps::copy_t<FontMetrics>(this, &other);
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Font size (difference between two base-lines).
  float _size;
  //! @brief Font ascent (positive).
  float _ascent;
  //! @brief Font descent (positive).
  float _descent;
  //! @brief Capital letter height (positive).
  float _capHeight;
  //! @brief Small 'x' letter height (positive).
  float _xHeight;
};

// ============================================================================
// [Fog::FontFeatures]
// ============================================================================

struct FOG_NO_EXPORT FontFeatures
{
  // --------------------------------------------------------------------------
  // [FontFeatures]
  // --------------------------------------------------------------------------

  FOG_INLINE FontFeatures()
  {
    reset();
  }

  explicit FOG_INLINE FontFeatures(_Uninitialized) {}

  FOG_INLINE FontFeatures(const FontFeatures& other)
  {
    MemOps::copy_t<FontFeatures>(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getKerning() const { return _kerning; }
  FOG_INLINE void setKerning(uint32_t kerning) { _kerning = kerning; }

  FOG_INLINE uint32_t getCommonLigatures() const { return _commonLigatures; }
  FOG_INLINE void setCommonLigatures(uint32_t commonLigatures) { _commonLigatures = commonLigatures; }

  FOG_INLINE uint32_t getDiscretionaryLigatures() const { return _discretionaryLigatures; }
  FOG_INLINE void setDiscretionaryLigatures(uint32_t discretionaryLigatures) { _discretionaryLigatures = discretionaryLigatures; }

  FOG_INLINE uint32_t getHistoricalLigatures() const { return _historicalLigatures; }
  FOG_INLINE void setHistoricalLigatures(uint32_t historicalLigatures) { _historicalLigatures = historicalLigatures; }

  FOG_INLINE uint32_t getCaps() const { return _caps; }
  FOG_INLINE void setCaps(uint32_t caps) { _caps = caps; }

  FOG_INLINE uint32_t getNumericFigure() const { return _numericFigure; }
  FOG_INLINE void setx(uint32_t numericFigure) { _numericFigure = numericFigure; }

  FOG_INLINE uint32_t getNumericSpacing() const { return _numericSpacing; }
  FOG_INLINE void setNumericSpacing(uint32_t numericSpacing) { _numericSpacing = numericSpacing; }

  FOG_INLINE uint32_t getNumericFraction() const { return _numericFraction; }
  FOG_INLINE void setNumericFraction(uint32_t numericFraction) { _numericFraction = numericFraction; }

  FOG_INLINE uint32_t getNumericSlashedZero() const { return _numericSlashedZero; }
  FOG_INLINE void setNumericSlashedZero(uint32_t numericSlashedZero) { _numericSlashedZero = numericSlashedZero; }

  FOG_INLINE uint32_t getEastAsianVariant() const { return _eastAsianVariant; }
  FOG_INLINE void setEastAsianVariant(uint32_t eastAsianVariant) { _eastAsianVariant = eastAsianVariant; }

  FOG_INLINE uint32_t getEastAsianWidth() const { return _eastAsianWidth; }
  FOG_INLINE void setEastAsianWidth(uint32_t eastAsianWidth) { _eastAsianWidth = eastAsianWidth; }

  FOG_INLINE uint32_t getLetterSpacingMode() const { return _letterSpacingMode; }
  FOG_INLINE void setLetterSpacingMode(uint32_t letterSpacingMode) { _letterSpacingMode = letterSpacingMode; }

  FOG_INLINE uint32_t getWordSpacingMode() const { return _wordSpacingMode; }
  FOG_INLINE void setWordSpacingMode(uint32_t wordSpacingMode) { _wordSpacingMode = wordSpacingMode; }

  FOG_INLINE float getLetterSpacingValue() const { return _letterSpacingValue; }
  FOG_INLINE void setLetterSpacingValue(float val) { _letterSpacingValue = val; }

  FOG_INLINE float getWordSpacingValue() const { return _wordSpacingValue; }
  FOG_INLINE void setWordSpacingValue(float val) { _wordSpacingValue = val; }

  FOG_INLINE float getSizeAdjust() const { return _sizeAdjust; }
  FOG_INLINE void setSizeAdjust(float val) { _sizeAdjust = val; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _packed[0] = 0;
    _packed[1] = 0;
    _letterSpacingValue = 0.0f;
    _wordSpacingValue = 0.0f;
    _sizeAdjust = 0.0f;
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE FontFeatures& operator=(const FontFeatures& other)
  {
    MemOps::copy_t<FontFeatures>(this, &other);
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  union
  {
    struct
    {
      //! @brief Whether to use kerning (if supported).
      uint32_t _kerning : 1;

      //! @brief Whether to use common ligatures (if supported).
      uint32_t _commonLigatures : 1;
      //! @brief Whether to use discretionary ligatures (if supported).
      uint32_t _discretionaryLigatures : 1;
      //! @brief Whether to use historical ligatures (if supported).
      uint32_t _historicalLigatures : 1;

      //! @brief Caps.
      uint32_t _caps : 4;

      //! @brief Numeric figure variant.
      uint32_t _numericFigure : 2;
      //! @brief Numeric proportional variant.
      uint32_t _numericSpacing : 2;
      //! @brief Numeric fraction variant.
      uint32_t _numericFraction : 2;
      //! @brief Whether to use slashed zero.
      uint32_t _numericSlashedZero : 1;

      //! @brief East asian variant
      uint32_t _eastAsianVariant : 3;
      //! @brief East asian width.
      uint32_t _eastAsianWidth : 2;

      //! @brief Letter spacing mode.
      uint32_t _letterSpacingMode : 2;
      //! @brief Word spacing mode.
      uint32_t _wordSpacingMode : 2;

      //! @brief Weight.
      uint32_t _weight : 8;
      //! @brief Stretch.
      uint32_t _stretch : 8;
      //! @brief Decoration.
      uint32_t _decoration : 8;
      //! @brief Style.
      uint32_t _style : 2;

      //! @brief Reserved for future use, initialized to zero.
      uint32_t _reserved : 14;
    };

    uint32_t _packed[2];
  };

  //! @brief Letter spacing value.
  float _letterSpacingValue;
  //! @brief Word spacing value.
  float _wordSpacingValue;

  //! @brief Size adjust.
  float _sizeAdjust;
};

// ============================================================================
// [Fog::FontMatrix]
// ============================================================================

//! @brief Transformation matrix used by @ref Font.
struct FOG_NO_EXPORT FontMatrix
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FontMatrix()
  {
    _xx = 1.0f; _xy = 0.0f;
    _yx = 0.0f; _yy = 1.0f;
  }

  explicit FOG_INLINE FontMatrix(_Uninitialized) {}

  FOG_INLINE FontMatrix(float xx, float xy, float yx, float yy)
  {
    _xx = xx; _xy = xy;
    _yx = yx; _yy = yy;
  }

  FOG_INLINE FontMatrix(const FontMatrix& other)
  {
    MemOps::copy_t<FontMatrix>(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE FontMatrix& operator=(const FontMatrix& other)
  {
    MemOps::copy_t<FontMatrix>(this, &other);
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  union
  {
    struct
    {
      float _xx, _xy;
      float _yx, _yy;
    };

    float _m[4];
  };
};

// ============================================================================
// [Fog::GlyphItem]
// ============================================================================

struct FOG_NO_EXPORT GlyphItem
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------
  
  uint32_t _glyphIndex;
  uint32_t _properties;
  uint32_t _cluster;
  uint16_t _component;
  uint16_t _ligatureId;
};

// ============================================================================
// [Fog::GlyphPosition]
// ============================================================================

struct FOG_NO_EXPORT GlyphPosition
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  PointF _position;
  PointF _advance;

  uint32_t _newAdvance : 1;
  uint32_t _back : 15;
  int32_t _cursiveChain : 16;
};

// ============================================================================
// [Fog::GlyphRun]
// ============================================================================

struct FOG_NO_EXPORT GlyphRun
{
  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  List<GlyphItem> _glyphs;
};































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
      fog_api.font_dFree(this);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  // ${VAR:BEGIN}
  //
  // This data-object is binary compatible with the VarData header in the first
  // form called - "implicitly shared class". The members must be binary
  // compatible with the header below:
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
  //! @brief Font flags.
  uint32_t flags;

  //! @brief Font-face.
  FontFace* face;
  //! @brief Custom transformation matrix to apply to glyphs.
  FontMatrix matrix;
  //! @brief Scaled font metrics.
  FontMetrics metrics;
  //! @brief Font features.
  FontFeatures features;
  //! @brief Scale constant to get the scaled metrics from the design-metrics.
  float scale;
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
    fog_api.font_ctor(this);
  }

  FOG_INLINE Font(const Font& other)
  {
    fog_api.font_ctorCopy(this, &other);
  }

#if defined(FOG_CC_HAS_RVALUE)
  FOG_INLINE Font(Font&& other) : _d(other._d) { other._d = NULL; }
#endif // FOG_CC_HAS_RVALUE

  explicit FOG_INLINE Font(FontData* d) :
    _d(d)
  {
  }

  FOG_INLINE ~Font()
  {
    fog_api.font_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getReference() const { return _d->reference.get(); }
  FOG_INLINE bool isDetached() const { return _d->reference.get() == 1; }

  FOG_INLINE err_t detach() { return isDetached() ? (err_t)ERR_OK : _detach(); }
  FOG_INLINE err_t _detach() { return fog_api.font_detach(this); }

  // --------------------------------------------------------------------------
  // [Accessors - Font/Face]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setFont(const Font& other)
  {
    return fog_api.font_copy(this, &other);
  }

  FOG_INLINE FontFace* getFace() const { return _d->face; }
  FOG_INLINE const StringW& getFamily() const { return _d->face->family; }

  // --------------------------------------------------------------------------
  // [Accessors - Size]
  // --------------------------------------------------------------------------

  //! @brief Get font size.
  FOG_INLINE float getSize() const { return _d->metrics._size; }
  //! @brief Set font size to @a size.
  FOG_INLINE err_t setSize(float size) { return fog_api.font_setSize(this, size); }

  // --------------------------------------------------------------------------
  // [Accessors - Metrics]
  // --------------------------------------------------------------------------

  FOG_INLINE const FontMetrics& getMetrics() const { return _d->metrics; }

  // --------------------------------------------------------------------------
  // [Accessors - Spacing]
  // --------------------------------------------------------------------------

  FOG_INLINE bool hasLetterSpacing() const { return (_d->flags & FONT_FLAG_HAS_LETTER_SPACING) != 0; }
  FOG_INLINE bool hasWordSpacing() const { return (_d->flags & FONT_FLAG_HAS_WORD_SPACING) != 0; }

  FOG_INLINE uint32_t getLetterSpacingMode() const { return _d->features._letterSpacingMode; }
  FOG_INLINE uint32_t getWordSpacingMode() const { return _d->features._wordSpacingMode; }

  FOG_INLINE float getLetterSpacingValue() const { return _d->features._letterSpacingValue; }
  FOG_INLINE float getWordSpacingValue() const { return _d->features._wordSpacingValue; }

  FOG_INLINE err_t setLetterSpacing(float spacing, uint32_t spacingMode)
  {
    return fog_api.font_setLetterSpacing(this, spacing, spacingMode);
  }

  FOG_INLINE err_t setWordSpacing(float spacing, uint32_t spacingMode)
  {
    return fog_api.font_setWordSpacing(this, spacing, spacingMode);
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
    return fog_api.font_setHints(this, &hints);
  }

  FOG_INLINE err_t setStyle(uint32_t style)
  {
    return fog_api.font_setStyle(this, style);
  }

  FOG_INLINE err_t setWeight(uint32_t weight)
  {
    return fog_api.font_setWeight(this, weight);
  }

  FOG_INLINE err_t setVariant(uint32_t variant)
  {
    return fog_api.font_setVariant(this, variant);
  }

  FOG_INLINE err_t setDecoration(uint32_t decoration)
  {
    return fog_api.font_setDecoration(this, decoration);
  }

  FOG_INLINE err_t setKerning(uint32_t kerning)
  {
    return fog_api.font_setKerning(this, kerning);
  }

  FOG_INLINE err_t setHinting(uint32_t hinting)
  {
    return fog_api.font_setHinting(this, hinting);
  }

  FOG_INLINE err_t setAlignMode(uint32_t alignMode)
  {
    return fog_api.font_setAlignMode(this, alignMode);
  }

  // --------------------------------------------------------------------------
  // [Accessors - Matrix]
  // --------------------------------------------------------------------------

  FOG_INLINE bool hasMatrix() const { return (_d->flags & FONT_FLAG_HAS_MATRIX) != 0; }
  FOG_INLINE const FontMatrix& getMatrix() const { return _d->matrix; }

  FOG_INLINE err_t setTransform(const FontMatrix& matrix)
  {
    return fog_api.font_setMatrix(this, &matrix);
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
    return fog_api.font_setForceCaching(this, val);
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    fog_api.font_reset(this);
  }

  // --------------------------------------------------------------------------
  // [Create]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t create(const StringW& family, float height, uint32_t unit)
  {
    return fog_api.font_create(this, &family, height, unit);
  }

  FOG_INLINE err_t create(const StringW& family, float height, uint32_t unit,
    const FontHints& hints, const TransformF& transform)
  {
    return fog_api.font_createEx(this, &family, height, unit, &hints, &transform);
  }

  FOG_INLINE err_t _fromFace(FontFace* face, float height, uint32_t unit)
  {
    return fog_api.font_fromFace(this, face, height, unit);
  }

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t getTextOutline(PathF& dst, uint32_t cntOp, const PointF& pt, const StubW& str) const
  {
    return fog_api.font_getTextOutlineFStubW(this, &dst, cntOp, &pt, &str);
  }

  FOG_INLINE err_t getTextOutline(PathF& dst, uint32_t cntOp, const PointF& pt, const StringW& str) const
  {
    return fog_api.font_getTextOutlineFStringW(this, &dst, cntOp, &pt, &str);
  }

  FOG_INLINE err_t getTextOutline(PathD& dst, uint32_t cntOp, const PointD& pt, const StubW& str) const
  {
    return fog_api.font_getTextOutlineDStubW(this, &dst, cntOp, &pt, &str);
  }

  FOG_INLINE err_t getTextOutline(PathD& dst, uint32_t cntOp, const PointD& pt, const StringW& str) const
  {
    return fog_api.font_getTextOutlineDStringW(this, &dst, cntOp, &pt, &str);
  }

  FOG_INLINE err_t getTextExtents(TextExtentsF& extents, const StubW& str) const
  {
    return fog_api.font_getTextExtentsFStubW(this, &extents, &str);
  }

  FOG_INLINE err_t getTextExtents(TextExtentsF& extents, const StringW& str) const
  {
    return fog_api.font_getTextExtentsFStringW(this, &extents, &str);
  }

  FOG_INLINE err_t getTextExtents(TextExtentsD& extents, const StubW& str) const
  {
    return fog_api.font_getTextExtentsDStubW(this, &extents, &str);
  }

  FOG_INLINE err_t getTextExtents(TextExtentsD& extents, const StringW& str) const
  {
    return fog_api.font_getTextExtentsDStringW(this, &extents, &str);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const Font& other) const
  {
    return fog_api.font_eq(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE Font& operator=(const Font& other)
  {
    fog_api.font_copy(this, &other);
    return *this;
  }

  FOG_INLINE bool operator==(const Font& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const Font& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Statics - Equality]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const Font* a, const Font* b)
  {
    return fog_api.font_eq(a, b);
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)fog_api.font_eq;
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
