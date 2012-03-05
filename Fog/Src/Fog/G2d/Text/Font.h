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
#include <Fog/Core/Tools/Hash.h>
#include <Fog/Core/Tools/List.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Geometry/Size.h>
#include <Fog/G2d/Geometry/Transform.h>

namespace Fog {

//! @addtogroup Fog_G2d_Text
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct OT_Table;

// ============================================================================
// [Fog::FontSpacing]
// ============================================================================

//! @brief Font spacing mode and value pair.
struct FOG_NO_EXPORT FontSpacing
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FontSpacing()
  {
    MemOps::zero_t<FontSpacing>(this);
  }

  explicit FOG_INLINE FontSpacing(_Uninitialized) {}

  FOG_INLINE FontSpacing(const FontSpacing& other)
  {
    MemOps::copy_t<FontSpacing>(this, &other);
  }

  FOG_INLINE FontSpacing(uint32_t mode, float value) :
    _mode(mode),
    _value(value)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getMode() const { return _mode; }
  FOG_INLINE void setMode(uint32_t mode) { _mode = mode; }

  FOG_INLINE float getValue() const { return _value; }
  FOG_INLINE void setValue(float value) { _value = value; }

  FOG_INLINE void setSpacing(uint32_t mode, float value)
  {
    _mode = mode;
    _value = value;
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    MemOps::zero_t<FontSpacing>(this);
  }  

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE FontSpacing& operator=(FontSpacing& other)
  {
    MemOps::copy_t<FontSpacing>(this, &other);
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint32_t _mode;
  float _value;
};

// ============================================================================
// [Fog::FontMetrics]
// ============================================================================

//! @brief Font metrics.
//!
//! Represents design or scaled font metrics.
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

  FOG_INLINE float getLineGap() const { return _lineGap; }
  FOG_INLINE void setLineGap(float lineGap) { _lineGap = lineGap; }

  FOG_INLINE float getLineSpacing() const { return _lineSpacing; }
  FOG_INLINE void setLineSpacing(float lineSpacing) { _lineSpacing = lineSpacing; }

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

  //! @brief Font size.
  float _size;
  //! @brief Font ascent (positive).
  float _ascent;
  //! @brief Font descent (positive).
  float _descent;
  //! @brief Line-gap, sometimes called external-leading (positive).
  float _lineGap;
  //! @brief Line-spacing combines ascent, descent, and lineGap (positive).
  float _lineSpacing;
  //! @brief Capital letter height (positive).
  float _capHeight;
  //! @brief Small 'x' letter height (positive).
  float _xHeight;
};

// ============================================================================
// [Fog::FontFeatures]
// ============================================================================

//! @brief Font features.
//!
//! Represents nearly all features which can be used together with @ref Font.
//! The structure is designed for easy manipulation so you can get features
//! from @ref Font, manipulate it, and set it back.
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
  FOG_INLINE void setNumericFigure(uint32_t numericFigure) { _numericFigure = numericFigure; }

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

  FOG_INLINE FontSpacing getLetterSpacing() const { return FontSpacing(_letterSpacingMode, _letterSpacingValue); }
  FOG_INLINE FontSpacing getWordSpacing() const { return FontSpacing(_wordSpacingMode, _wordSpacingValue); }

  FOG_INLINE uint32_t getLetterSpacingMode() const { return _letterSpacingMode; }
  FOG_INLINE void setLetterSpacingMode(uint32_t letterSpacingMode) { _letterSpacingMode = letterSpacingMode; }

  FOG_INLINE uint32_t getWordSpacingMode() const { return _wordSpacingMode; }
  FOG_INLINE void setWordSpacingMode(uint32_t wordSpacingMode) { _wordSpacingMode = wordSpacingMode; }

  FOG_INLINE float getLetterSpacingValue() const { return _letterSpacingValue; }
  FOG_INLINE void setLetterSpacingValue(float val) { _letterSpacingValue = val; }

  FOG_INLINE float getWordSpacingValue() const { return _wordSpacingValue; }
  FOG_INLINE void setWordSpacingValue(float val) { _wordSpacingValue = val; }

  FOG_INLINE uint32_t getWeight() const { return _weight; }
  FOG_INLINE void setWeight(uint32_t val) { _weight = val; }

  FOG_INLINE uint32_t getStretch() const { return _stretch; }
  FOG_INLINE void setStretch(uint32_t val) { _stretch = val; }

  FOG_INLINE uint32_t getDecoration() const { return _decoration; }
  FOG_INLINE void setDecoration(uint32_t val) { _decoration = val; }

  FOG_INLINE uint32_t getStyle() const { return _style; }
  FOG_INLINE void setStyle(uint32_t val) { _style = val; }

  FOG_INLINE float getSizeAdjust() const { return _sizeAdjust; }
  FOG_INLINE void setSizeAdjust(float val) { _sizeAdjust = val; }

  FOG_INLINE bool hasLetterSpacing() const
  {
    return (_letterSpacingMode == FONT_SPACING_ABSOLUTE   && _letterSpacingValue == 0.0f) ||
           (_letterSpacingMode == FONT_SPACING_PERCENTAGE && _letterSpacingValue == 1.0f) ;
  }

  FOG_INLINE bool hasWordSpacing() const
  {
    return (_wordSpacingMode == FONT_SPACING_ABSOLUTE   && _wordSpacingValue == 0.0f) ||
           (_wordSpacingMode == FONT_SPACING_PERCENTAGE && _wordSpacingValue == 1.0f) ;
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _packed[0] = 0;
    _packed[1] = 0;

    _kerning = FONT_KERNING_ENABLED;
    _weight = FONT_WEIGHT_NORMAL;
    _stretch = FONT_STRETCH_NORMAL;

    _letterSpacingValue = 0.0f;
    _wordSpacingValue = 0.0f;
    _sizeAdjust = 0.0f;
  }

  // --------------------------------------------------------------------------
  // [Eq]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const FontFeatures& other) const
  {
    return MemOps::eq_t<FontFeatures>(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE FontFeatures& operator=(const FontFeatures& other)
  {
    MemOps::copy_t<FontFeatures>(this, &other);
    return *this;
  }

  FOG_INLINE bool operator==(const FontFeatures& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const FontFeatures& other) const { return !eq(other); }

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

//! @brief Font matrix used by @ref Font.
//!
//! Font matrix is much simpler than @ref TransformF or @ref TransformD. The
//! purpose is to be compatible to the native APIs, whose do not allow to 
//! assign perspective transformation matrix to fonts.
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
  // [Flags]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isIdentity() const
  {
    return _xx == 1.0f && _xy == 0.0f &&
           _yx == 0.0f && _yy == 1.0f ;
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _xx = 1.0f; _xy = 0.0f;
    _yx = 0.0f; _yy = 1.0f;
  }

  // --------------------------------------------------------------------------
  // [Eq]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const FontMatrix& other) const
  {
    return MemOps::eq_t<FontMatrix>(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE FontMatrix& operator=(const FontMatrix& other)
  {
    MemOps::copy_t<FontMatrix>(this, &other);
    return *this;
  }

  FOG_INLINE bool operator==(const FontMatrix& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const FontMatrix& other) const { return !eq(other); }

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
// [Fog::FaceFeatures]
// ============================================================================

//! @brief Face features.
//!
//! These definitions should match single font file.
struct FOG_NO_EXPORT FaceFeatures
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FaceFeatures()
  {
    _packed = 0;
  }

  explicit FOG_INLINE FaceFeatures(_Uninitialized) {}

  FOG_INLINE FaceFeatures(uint32_t weight, uint32_t stretch, bool italic)
  {
    _packed = 0;
    _weight = weight;
    _stretch = stretch;
    _italic = italic;
  }

  FOG_INLINE FaceFeatures(const FaceFeatures& other)
  {
    _packed = other._packed;
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getWeight() const { return _weight; }
  FOG_INLINE void setWeight(uint32_t weight) { _weight = weight; }

  FOG_INLINE uint32_t getStretch() const { return _stretch; }
  FOG_INLINE void setStretch(uint32_t stretch) { _stretch = stretch; }

  FOG_INLINE bool getItalic() const { return (bool)_italic; }
  FOG_INLINE void setItalic(bool italic) { _italic = italic; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _packed = 0;
  }

  // --------------------------------------------------------------------------
  // [Eq]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const FaceFeatures& other) const
  {
    return _packed == other._packed;
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE FaceFeatures& operator=(const FaceFeatures& other)
  {
    _packed = other._packed;
    return *this;
  }

  FOG_INLINE bool operator==(const FaceFeatures& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const FaceFeatures& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  union
  {
    struct
    {
      uint32_t _weight : 8;
      uint32_t _stretch : 8;
      uint32_t _italic : 1;
      uint32_t _reserved : 15;
    };

    uint32_t _packed;
  };
};

// ============================================================================
// [Fog::FaceInfoMetrics]
// ============================================================================

struct FOG_NO_EXPORT FaceInfoMetrics
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FaceInfoMetrics()
  {
    reset();
  }

  explicit FOG_INLINE FaceInfoMetrics(_Uninitialized) {}

  FOG_INLINE FaceInfoMetrics(uint32_t emSize)
  {
    reset();

    _emSize = static_cast<uint16_t>(emSize);
  }

  FOG_INLINE FaceInfoMetrics(const FaceInfoMetrics& other)
  {
    MemOps::copy_t<FaceInfoMetrics>(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getEmSize() const { return _emSize; }
  FOG_INLINE void setEmSize(uint32_t emSize) { _emSize = emSize; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    MemOps::zero_t<FaceInfoMetrics>(this);
  }

  // --------------------------------------------------------------------------
  // [Eq]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const FaceInfoMetrics& other) const
  {
    return MemOps::eq_t<FaceInfoMetrics>(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE FaceInfoMetrics& operator=(const FaceInfoMetrics& other)
  {
    MemOps::copy_t<FaceInfoMetrics>(this, &other);
    return *this;
  }

  FOG_INLINE bool operator==(const FaceInfoMetrics& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const FaceInfoMetrics& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Design em-size.
  uint16_t _emSize;
  //! @brief Design em-height.
  uint16_t _reserved_0;
  //! @brief Reserved for future use.
  uint32_t _reserved_1;
};

// ============================================================================
// [Fog::FaceInfoData]
// ============================================================================

struct FOG_NO_EXPORT FaceInfoData
{
  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  FOG_INLINE FaceInfoData* addRef() const
  {
    reference.inc();
    return const_cast<FaceInfoData*>(this);
  }

  FOG_INLINE void release()
  {
    if (reference.deref())
      fog_api.faceinfo_dFree(this);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Reference count.
  mutable Atomic<size_t> reference;

  //! @brief Variable type and flags.
  uint32_t vType;

  //! @brief Face features.
  FaceFeatures features;

  //! @brief Family name, extracted from full family name in form "Family [Brand]".
  Static<StringW> familyName;

  //! @brief Brand name, extracted from full family name in form "Family [Brand]".
  Static<StringW> brandName;

  //! @brief File name in absolute path form.
  //!
  //! @note This member is only filled in case that the font can be loaded
  //! from disk and the font engine is able to get the path and file name from
  //! the native API. In case the this font is a custom font, which needs to be
  //! loaded from disk, fileName is always filled.
  Static<StringW> fileName;

  //! @brief Face metrics.
  FaceInfoMetrics metrics;
};

// ============================================================================
// [Fog::FaceInfo]
// ============================================================================

struct FOG_NO_EXPORT FaceInfo
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FaceInfo()
  {
    fog_api.faceinfo_ctor(this);
  }

  FOG_INLINE FaceInfo(const FaceInfo& other)
  {
    fog_api.faceinfo_ctorCopy(this, &other);
  }

#if defined(FOG_CC_HAS_RVALUE)
  FOG_INLINE FaceInfo(FaceInfo&& other) : _d(other._d) { other._d = NULL; }
#endif // FOG_CC_HAS_RVALUE

  explicit FOG_INLINE FaceInfo(FaceInfoData* d) :
    _d(d)
  {
  }

  FOG_INLINE ~FaceInfo()
  {
    fog_api.faceinfo_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getReference() const { return _d->reference.get(); }
  FOG_INLINE bool isDetached() const { return _d->reference.get() == 1; }

  FOG_INLINE err_t detach() { return _d->reference.get() != 1 ? _detach() : (err_t)ERR_OK; }
  FOG_INLINE err_t _detach() { return fog_api.faceinfo_detach(this); }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setFaceInfo(const FaceInfo& other) { return fog_api.faceinfo_copy(this, &other); }

  FOG_INLINE bool hasFamilyName() const { return _d->familyName().getLength() != 0; }
  FOG_INLINE bool hasFileName() const { return _d->fileName().getLength() != 0; }

  FOG_INLINE FaceFeatures getFeatures() const { return _d->features; }
  FOG_INLINE err_t setFeatures(const FaceFeatures& features) { return fog_api.faceinfo_setFeatures(this, &features); }

  FOG_INLINE const StringW& getFamilyName() const { return _d->familyName; }
  FOG_INLINE err_t setFamilyName(const StringW& familyName) { return fog_api.faceinfo_setFamilyName(this, &familyName); }

  FOG_INLINE const StringW& getFileName() const { return _d->fileName; }
  FOG_INLINE err_t setFileName(const StringW& fileName) { return fog_api.faceinfo_setFileName(this, &fileName); }

  FOG_INLINE const FaceInfoMetrics& getMetrics() const { return _d->metrics; }
  FOG_INLINE err_t setMetrics(const FaceInfoMetrics& metrics) { return fog_api.faceinfo_setMetrics(this, &metrics); }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    fog_api.faceinfo_reset(this);
  }

  // --------------------------------------------------------------------------
  // [Eq]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const FaceInfo& other) const
  {
    return fog_api.faceinfo_eq(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Compare]
  // --------------------------------------------------------------------------

  FOG_INLINE int compare(const FaceInfo& other) const
  {
    return fog_api.faceinfo_compare(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE FaceInfo& operator=(const FaceInfo& other)
  {
    fog_api.faceinfo_copy(this, &other);
    return *this;
  }

  FOG_INLINE bool operator==(const FaceInfo& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const FaceInfo& other) const { return !eq(other); }

  FOG_INLINE bool operator<=(const FaceInfo& other) const { return compare(other) <= 0; }
  FOG_INLINE bool operator< (const FaceInfo& other) const { return compare(other) <  0; }
  FOG_INLINE bool operator>=(const FaceInfo& other) const { return compare(other) >= 0; }
  FOG_INLINE bool operator> (const FaceInfo& other) const { return compare(other) >  0; }

  // --------------------------------------------------------------------------
  // [Statics - Eq]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const FaceInfo* a, const FaceInfo* b)
  {
    return fog_api.faceinfo_eq(a, b);
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)fog_api.faceinfo_eq;
  }

  // --------------------------------------------------------------------------
  // [Statics - Compare]
  // --------------------------------------------------------------------------

  static FOG_INLINE int compare(const FaceInfo* a, const FaceInfo* b)
  {
    return fog_api.faceinfo_compare(a, b);
  }

  static FOG_INLINE CompareFunc getCompareFunc()
  {
    return (CompareFunc)fog_api.faceinfo_compare;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(FaceInfoData)
};

// ============================================================================
// [Fog::FaceCollectionData]
// ============================================================================

struct FOG_NO_EXPORT FaceCollectionData
{
  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  FOG_INLINE FaceCollectionData* addRef() const
  {
    reference.inc();
    return const_cast<FaceCollectionData*>(this);
  }

  FOG_INLINE void release()
  {
    if (reference.deref())
      fog_api.facecollection_dFree(this);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Reference count.
  mutable Atomic<size_t> reference;

  //! @brief Variable type and flags.
  uint32_t vType;
  //! @brief Collection flags.
  uint32_t flags;

  //! @brief List of collected FaceInfo instances.
  Static< List<FaceInfo> > faceList;
  //! @brief Hash, contains font families and their count in faceList.
  Static< Hash<StringW, size_t> > faceHash;
};

// ============================================================================
// [Fog::FaceCollection]
// ============================================================================

struct FOG_NO_EXPORT FaceCollection
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FaceCollection()
  {
    fog_api.facecollection_ctor(this);
  }

  FOG_INLINE FaceCollection(const FaceCollection& other)
  {
    fog_api.facecollection_ctorCopy(this, &other);
  }

#if defined(FOG_CC_HAS_RVALUE)
  FOG_INLINE FaceCollection(FaceCollection&& other) : _d(other._d) { other._d = NULL; }
#endif // FOG_CC_HAS_RVALUE

  explicit FOG_INLINE FaceCollection(FaceCollectionData* d) :
    _d(d)
  {
  }

  FOG_INLINE ~FaceCollection()
  {
    fog_api.facecollection_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getReference() const { return _d->reference.get(); }
  FOG_INLINE bool isDetached() const { return _d->reference.get() == 1; }

  FOG_INLINE err_t detach() { return _d->reference.get() != 1 ? _detach() : (err_t)ERR_OK; }
  FOG_INLINE err_t _detach() { return fog_api.facecollection_detach(this); }

  // --------------------------------------------------------------------------
  // [Flags]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getFlags() const { return _d->flags; }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setCollection(const FaceCollection& other)
  {
    return fog_api.facecollection_copy(this, &other);
  }

  FOG_INLINE const List<FaceInfo>& getList() const
  {
    return _d->faceList;
  }

  FOG_INLINE err_t setList(const List<FaceInfo>& list)
  {
    return fog_api.facecollection_setList(this, &list);
  }

  FOG_INLINE Range getFamilyRange(const StringW& family) const
  {
    Range result(UNINITIALIZED);
    fog_api.facecollection_getFamilyRangeStringW(this, &family, &result);
    return result;
  }

  FOG_INLINE Range getFamilyRange(const StubW& family) const
  {
    Range result(UNINITIALIZED);
    fog_api.facecollection_getFamilyRangeStubW(this, &family, &result);
    return result;
  }

  FOG_INLINE size_t indexOf(const StringW& family, const FaceFeatures& features) const
  {
    return fog_api.facecollection_indexOfStringW(this, &family, &features);
  }

  FOG_INLINE size_t indexOf(const StubW& family, const FaceFeatures& features) const
  {
    return fog_api.facecollection_indexOfStubW(this, &family, &features);
  }

  // --------------------------------------------------------------------------
  // [Manipulation]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t addItem(const FaceInfo& item, size_t* dstIndex = NULL)
  {
    return fog_api.facecollection_addItem(this, &item, dstIndex);
  }

  // --------------------------------------------------------------------------
  // [Clear]
  // --------------------------------------------------------------------------

  FOG_INLINE void clear()
  {
    fog_api.facecollection_clear(this);
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    fog_api.facecollection_reset(this);
  }

  // --------------------------------------------------------------------------
  // [Eq]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const FaceCollection& other) const
  {
    return fog_api.facecollection_eq(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE FaceCollection& operator=(const FaceCollection& other)
  {
    fog_api.facecollection_copy(this, &other);
    return *this;
  }

  FOG_INLINE bool operator==(const FaceCollection& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const FaceCollection& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Statics - Eq]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const FaceCollection* a, const FaceCollection* b)
  {
    return fog_api.facecollection_eq(a, b);
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)fog_api.facecollection_eq;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(FaceCollectionData)
};

// ============================================================================
// [Fog::FaceVTable]
// ============================================================================

//! @brief Font-face virtual table.
struct FOG_NO_EXPORT FaceVTable
{
  void (FOG_CDECL* destroy)(Face* self);

  OT_Table* (FOG_CDECL* getTable)(const Face* self, uint32_t tag);
  void (FOG_CDECL* releaseTable)(const Face* self, OT_Table* table);

  err_t (FOG_CDECL* getOutlineFromGlyphRunF)(FontData* d,
    PathF* dst, uint32_t cntOp,
    const uint32_t* glyphList, size_t itemAdvance,
    const PointF* positionList, size_t positionAdvance,
    size_t length);

  err_t (FOG_CDECL* getOutlineFromGlyphRunD)(FontData* d,
    PathD* dst, uint32_t cntOp,
    const uint32_t* glyphList, size_t glyphAdvance,
    const PointF* positionList, size_t positionAdvance,
    size_t length);
};

// ============================================================================
// [Fog::Face]
// ============================================================================

//! @brief Font face.
struct FOG_NO_EXPORT Face
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE Face(const FaceVTable* vtable_, const StringW& family_)
  {
    vtable = vtable_;

    reference.init(1);
    engineId = FONT_ENGINE_NULL;
    features.reset();
    family.initCustom1(family_);
    designMetrics.reset();
    designEm = 0.0f;
  }

  FOG_INLINE ~Face()
  {
    family.destroy();
  }

  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  FOG_INLINE Face* addRef() const
  {
    reference.inc();
    return const_cast<Face*>(this);
  }

  FOG_INLINE void release()
  {
    if (reference.deref())
      destroy();
  }

  // --------------------------------------------------------------------------
  // [Interface]
  // --------------------------------------------------------------------------

  FOG_INLINE void destroy()
  {
    vtable->destroy(this);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Font face vtable.
  const FaceVTable* vtable;

  //! @brief Reference count.
  mutable Atomic<size_t> reference;

  //! @brief Font-face engine-id.
  uint32_t engineId;

  //! @brief Font-face features.
  FaceFeatures features;

  //! @brief Font-face family.
  Static<StringW> family;

  //! @brief Design metrics.
  //!
  //! Ideally in integers, but can be scaled if the exact information can't be
  //! fetched (happens under Windows).
  FontMetrics designMetrics;

  //! @brief Design EM square
  //!
  //! Ideally in integers, but can be scaled if the exact information can't be
  //! fetched (Windows).
  float designEm;

private:
  FOG_NO_COPY(Face)
};

// ============================================================================
// [Fog::FaceCache]
// ============================================================================

//! @brief Face cache.
struct FOG_NO_EXPORT FaceCache
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FaceCache()
  {
    fog_api.facecache_ctor(this);
  }

  FOG_INLINE ~FaceCache()
  {
    fog_api.facecache_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    fog_api.facecache_reset(this);
  }

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  FOG_INLINE Face* getExactFace(const StringW& name, const FaceFeatures& features)
  {
    return fog_api.facecache_getExactFace(this, &name, &features);
  }

  FOG_INLINE err_t getAllFaces(const StringW& name, List<Face*>& dst)
  {
    return fog_api.facecache_getAllFaces(this, &name, &dst);
  }

  FOG_INLINE err_t put(const StringW& name, const FaceFeatures& features, Face* face)
  {
    return fog_api.facecache_put(this, &name, &features, face);
  }

  FOG_INLINE err_t remove(const StringW& name, const FaceFeatures& features, Face* face)
  {
    return fog_api.facecache_remove(this, &name, &features, face);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Static< Hash< StringW, List<Face*> > > data;

private:
  FOG_NO_COPY(FaceCache)
};

// ============================================================================
// [Fog::FontData]
// ============================================================================

//! @brief Font data.
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

  //! @brief Font face.
  Face* face;
  //! @brief Scaled font metrics.
  FontMetrics metrics;
  //! @brief Font features.
  FontFeatures features;
  //! @brief Custom transformation matrix to apply to glyphs.
  FontMatrix matrix;
  //! @brief Scale constant to get the scaled metrics from the design-metrics.
  float scale;
};

// ============================================================================
// [Fog::Font]
// ============================================================================

//! @brief Font.
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
  // [Set]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setFont(const Font& other)
  {
    return fog_api.font_copy(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Accessors - Face]
  // --------------------------------------------------------------------------

  //! @brief Get Font face.
  FOG_INLINE Face* getFace() const { return _d->face; }
  //! @brief Get font family.
  FOG_INLINE const StringW& getFamily() const { return _d->face->family; }
  //! @brief Get font scale relative to the font design metrics.
  FOG_INLINE float getScale() const { return _d->scale; }

  // --------------------------------------------------------------------------
  // [Accessors - Param]
  // --------------------------------------------------------------------------

  //! @brief Get font parameter, see @ref FONT_PARAM.
  FOG_INLINE err_t getParam(uint32_t id, void* dst) const { return fog_api.font_getParam(this, id, dst); }
  //! @brief Set font parameter, see @ref FONT_PARAM.
  FOG_INLINE err_t setParam(uint32_t id, const void* src) { return fog_api.font_setParam(this, id, src); }

  // --------------------------------------------------------------------------
  // [Accessors - Features]
  // --------------------------------------------------------------------------

  //! @brief Get font kerning.
  FOG_INLINE uint32_t getKerning() const { return _d->features._kerning; }
  //! @brief Set font kerning to @a val.
  FOG_INLINE err_t setKerning(uint32_t val) { return setParam(FONT_PARAM_KERNING, &val); }

  //! @brief Get whether to use common ligatures.
  FOG_INLINE uint32_t getCommonLigatures() const { return _d->features._commonLigatures; }
  //! @brief Set whether to use common ligatures.
  FOG_INLINE err_t setCommonLigatures(uint32_t val) { return setParam(FONT_PARAM_COMMON_LIGATURES, &val); }

  //! @brief Get whether to use discretionary ligatures.
  FOG_INLINE uint32_t getDiscretionaryLigatures() const { return _d->features._discretionaryLigatures; }
  //! @brief Set whether to use discretionary ligatures.
  FOG_INLINE err_t setDiscretionaryLigatures(uint32_t val) { return setParam(FONT_PARAM_DISCRETIONARY_LIGATURES, &val); }

  //! @brief Get whether to use historical ligatures.
  FOG_INLINE uint32_t getHistoricalLigatures() const { return _d->features._historicalLigatures; }
  //! @brief set whether to use historical ligatures.
  FOG_INLINE err_t setHistoricalLigatures(uint32_t val) { return setParam(FONT_PARAM_HISTORICAL_LIGATURES, &val); }

  //! @brief Get caps variants, see @ref FONT_CAPS.
  FOG_INLINE uint32_t getCaps() const { return _d->features._caps; }
  //! @brief Set caps variants to @a val, see @ref FONT_CAPS
  FOG_INLINE err_t setCaps(uint32_t val) { return setParam(FONT_PARAM_CAPS, &val); }

  //! @brief Get numeric figure variant, see @ref FONT_NUMERIC_FIGURE.
  FOG_INLINE uint32_t getNumericFigure() const { return _d->features._numericFigure; }
  //! @brief Set numeric figure variant, see @ref FONT_NUMERIC_FIGURE.
  FOG_INLINE err_t setNumericFigure(uint32_t val) { return setParam(FONT_PARAM_NUMERIC_FIGURE, &val); }

  //! @brief Get numeric spacing variant, see @ref FONT_NUMERIC_SPACING.
  FOG_INLINE uint32_t getNumericSpacing() const { return _d->features._numericSpacing; }
  //! @brief Set numeric spacing variant, see @ref FONT_NUMERIC_SPACING.
  FOG_INLINE err_t setNumericSpacing(uint32_t val) { return setParam(FONT_PARAM_NUMERIC_SPACING, &val); }

  //! @brief Get numeric fraction variant, see @ref FONT_NUMERIC_FRACTION.
  FOG_INLINE uint32_t getNumericFraction() const { return _d->features._numericFraction; }
  //! @brief Set numeric fraction variant, see @ref FONT_NUMERIC_FRACTION.
  FOG_INLINE err_t setNumericFraction(uint32_t val) { return setParam(FONT_PARAM_NUMERIC_FRACTION, &val); }

  //! @brief Get whether to slash numeric zero.
  FOG_INLINE uint32_t getNumericSlashedZero() const { return _d->features._numericSlashedZero; }
  //! @brief Set whether to slash numeric zero.
  FOG_INLINE err_t setNumericSlashedZero(uint32_t val) { return setParam(FONT_PARAM_NUMERIC_SLASHED_ZERO, &val); }

  //! @brief Get east-asian variant, see @ref FONT_EAST_ASIAN_VARIANT.
  FOG_INLINE uint32_t getEastAsianVariant() const { return _d->features._eastAsianVariant; }
  //! @brief Set east-asian variant, see @ref FONT_EAST_ASIAN_VARIANT.
  FOG_INLINE err_t setEastAsianVariant(uint32_t val) { return setParam(FONT_PARAM_EAST_ASIAN_VARIANT, &val); }

  //! @brief Get east-asian width, see @ref FONT_EAST_ASIAN_WIDTH.
  FOG_INLINE uint32_t getEastAsianWidth() const { return _d->features._eastAsianWidth; }
  //! @brief Get east-asian width, see @ref FONT_EAST_ASIAN_WIDTH.
  FOG_INLINE err_t setEastAsianWidth(uint32_t val) { return setParam(FONT_PARAM_EAST_ASIAN_WIDTH, &val); }

  //! @brief Get letter spacing mode.
  FOG_INLINE uint32_t getLetterSpacingMode() const { return _d->features._letterSpacingMode; }
  //! @brief Get letter spacing value.
  FOG_INLINE float getLetterSpacingValue() const { return _d->features._letterSpacingValue; }

  //! @brief Get letter spacing mode/value.
  FOG_INLINE FontSpacing getLetterSpacing() const { return FontSpacing(_d->features._letterSpacingMode, _d->features._letterSpacingValue); }
  //! @brief Set letter spacing mode/value.
  FOG_INLINE err_t setLetterSpacing(const FontSpacing& val) { return setParam(FONT_PARAM_LETTER_SPACING, &val); }

  //! @brief Get word spacing mode.
  FOG_INLINE uint32_t getWordSpacingMode() const { return _d->features._wordSpacingMode; }
  //! @brief Get word spacing value.
  FOG_INLINE float getWordSpacingValue() const { return _d->features._wordSpacingValue; }

  //! @brief Get word spacing mode/value.
  FOG_INLINE FontSpacing getWordSpacing() const { return FontSpacing(_d->features._wordSpacingMode, _d->features._wordSpacingValue); }
  //! @brief Set word spacing mode/value.
  FOG_INLINE err_t setWordSpacing(const FontSpacing& val) { return setParam(FONT_PARAM_WORD_SPACING, &val); }

  //! @brief Get size adjust.
  FOG_INLINE float getSizeAdjust() const { return _d->features._sizeAdjust; }
  //! @brief Set size adjust.
  FOG_INLINE err_t setSizeAdjust(float val) { return setParam(FONT_PARAM_SIZE_ADJUST, &val); }
  
  // --------------------------------------------------------------------------
  // [Accessors - Matrix]
  // --------------------------------------------------------------------------

  //! @brief Get font matrix.
  FOG_INLINE const FontMatrix& getMatrix() const { return _d->matrix; }
  //! @brief Set font matrix to @a matrix.
  FOG_INLINE err_t setMatrix(const FontMatrix& matrix) { return setParam(FONT_PARAM_MATRIX, &matrix); }

  // --------------------------------------------------------------------------
  // [Accessors - Metrics]
  // --------------------------------------------------------------------------

  //! @brief Get font metrics.
  FOG_INLINE const FontMetrics& getMetrics() const { return _d->metrics; }
  //! @brief Get font size.
  FOG_INLINE float getSize() const { return _d->metrics._size; }
  //! @brief Get font ascent (positive).
  FOG_INLINE float getAscent() const { return _d->metrics._ascent; }
  //! @brief Get font descent (positive).
  FOG_INLINE float getDescent() const { return _d->metrics._descent; }
  //! @brief Get font capital letter height (positive).
  FOG_INLINE float getCapHeight() const { return _d->metrics._capHeight; }
  //! @brief Get font 'x' letter height (positive).
  FOG_INLINE float getXHeight() const { return _d->metrics._xHeight; }

  //! @brief Set font size to @a size.
  FOG_INLINE err_t setSize(float size) { return setParam(FONT_PARAM_SIZE, &size); }

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

  //! @brief Query for a requested font family and size using the default font 
  //! features and identity matrix.
  FOG_INLINE err_t create(const StringW& family, float size)
  {
    return fog_api.font_create(this, &family, size, NULL, NULL);
  }

  //! @brief Query for a requested font family and size using specified font 
  //! features and identity matrix.
  FOG_INLINE err_t create(const StringW& family, float size,
    const FontFeatures& features)
  {
    return fog_api.font_create(this, &family, size, &features, NULL);
  }

  //! @brief Query for a requested font family and size using specified font 
  //! features and matrix.
  FOG_INLINE err_t create(const StringW& family, float size,
    const FontFeatures& features, const FontMatrix& matrix)
  {
    return fog_api.font_create(this, &family, size, &features, &matrix);
  }

  //! @internal.
  //!
  //! @brief Low-level method to create font from existing @ref Face, 
  //! typically retrieved by @ref FontEngine.
  FOG_INLINE err_t _init(Face* face, float size,
    const FontFeatures& features, const FontMatrix& matrix)
  {
    return fog_api.font_init(this, face, size, &features, &matrix);
  }

  // --------------------------------------------------------------------------
  // [Glyphs]
  // --------------------------------------------------------------------------

  // Implemented-Later.
  FOG_INLINE err_t getOutlineFromGlyphRun(PathF& dst, uint32_t cntOp,
    const GlyphRun& glyphRun) const;

  // Implemented-Later.
  FOG_INLINE err_t getOutlineFromGlyphRun(PathD& dst, uint32_t cntOp,
    const GlyphRun& glyphRun) const;

  FOG_INLINE err_t getOutlineFromGlyphRun(PathF& dst, uint32_t cntOp,
    const uint32_t* glyphs, const PointF* positions, size_t length) const
  {
    return fog_api.font_getOutlineFromGlyphRunF(this, &dst, cntOp,
      glyphs, sizeof(uint32_t), positions, sizeof(PointF), length);
  }

  FOG_INLINE err_t getOutlineFromGlyphRun(PathD& dst, uint32_t cntOp,
    const uint32_t* glyphs, const PointF* positions, size_t length) const
  {
    return fog_api.font_getOutlineFromGlyphRunD(this, &dst, cntOp,
      glyphs, sizeof(uint32_t), positions, sizeof(PointF), length);
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

// ============================================================================
// [Fog::FontEngineVTable]
// ============================================================================

//! @brief Font-engine virtual table.
struct FOG_NO_EXPORT FontEngineVTable
{
  void (FOG_CDECL* destroy)(FontEngine* self);

  err_t (FOG_CDECL* queryFace)(const FontEngine* self, Face** dst, const StringW* family, const FaceFeatures* features);
  err_t (FOG_CDECL* getAvailableFaces)(const FontEngine* self, FaceCollection* dst);
  err_t (FOG_CDECL* getDefaultFace)(const FontEngine* self, FaceInfo* dst);
};

// ============================================================================
// [Fog::FontEngine]
// ============================================================================

struct FOG_NO_EXPORT FontEngine
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FontEngine(FontEngineVTable* vtable_)
  {
    vtable = vtable_;
    faceCollection.init();

    // Initialized to NULL, FontEngine have to select the default font-face
    // and initialize it after connected with the platform native font-system.
    defaultFont->_d = NULL;
  }

  FOG_INLINE ~FontEngine()
  {
    faceCollection.destroy();

    // Here it's too late to destroy the default font. Although the FontEngine
    // can destroy it and leave garbage in '_d' member, we require it cleared
    // to prevent memory leaks or other issues.
    FOG_ASSERT(defaultFont->_d == NULL);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getEngineId() const { return engineId; }
  FOG_INLINE uint32_t getFeatures() const { return features; }

  // --------------------------------------------------------------------------
  // [Interface]
  // --------------------------------------------------------------------------

  FOG_INLINE void destroy()
  {
    vtable->destroy(this);
  }

  FOG_INLINE err_t queryFace(Face** dst, const StringW& family, const FaceFeatures& features) const
  {
    return vtable->queryFace(this, dst, &family, &features);
  }

  FOG_INLINE err_t getAvailableFaces(FaceCollection& dst) const
  {
    return vtable->getAvailableFaces(this, &dst);
  }

  FOG_INLINE err_t getDefaultFace(FaceInfo& dst) const
  {
    return vtable->getDefaultFace(this, &dst);
  }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE FontEngine* getGlobal()
  {
    return fog_api.fontengine_oGlobal;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Font engine virtual table.
  FontEngineVTable* vtable;

  uint32_t engineId;
  uint32_t features;

  //! @brief Font collection.
  Static<FaceCollection> faceCollection;

  //! @brief Default font created by the engine.
  Static<Font> defaultFont;

private:
  FOG_NO_COPY(FontEngine)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TEXT_FONT_H
