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
// [Fog::GlyphItem]
// ============================================================================

struct FOG_NO_EXPORT GlyphItem
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getGlyphIndex() const { return _glyphIndex; }
  FOG_INLINE void setGlyphIndex(uint32_t index) { _glyphIndex = index; }

  FOG_INLINE uint32_t getProperties() const { return _properties; }
  FOG_INLINE void setProperties(uint32_t properties) { _properties = properties; }

  FOG_INLINE uint32_t getCluster() const { return _cluster; }
  FOG_INLINE void setCluster(uint32_t cluster) { _cluster = cluster; }

  FOG_INLINE uint16_t getComponent() const { return _component; }
  FOG_INLINE void setComponent(uint16_t component) { _component = component; }

  FOG_INLINE uint16_t getLigatureId() const { return _ligatureId; }
  FOG_INLINE void setLigatureId(uint32_t ligatureId) { _ligatureId = ligatureId; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    MemOps::zero_t<GlyphItem>(this);
  }

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

  FOG_INLINE const PointF& getPosition() const { return _position; }
  FOG_INLINE void setPosition(const PointF& pos) { _position = pos; }

  FOG_INLINE const PointF& getAdvance() const { return _advance; }
  FOG_INLINE void setAdvance(const PointF& advance) { _advance = advance; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    MemOps::zero_t<GlyphPosition>(this);
  }

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
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getLength() const { return _itemList.getLength(); }

  FOG_INLINE const List<GlyphItem>& getItemList() const { return _itemList; }
  FOG_INLINE const List<GlyphPosition>& getPositionList() const { return _positionList; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  List<GlyphItem> _itemList;
  List<GlyphPosition> _positionList;
};

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
// [Fog::FontDefs]
// ============================================================================

//! @brief Font per face definitions.
//!
//! These definitions should match single font file.
struct FOG_NO_EXPORT FontDefs
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FontDefs()
  {
    _packed = 0;
  }

  explicit FOG_INLINE FontDefs(_Uninitialized) {}

  FOG_INLINE FontDefs(uint32_t weight, uint32_t stretch, bool italic)
  {
    _packed = 0;
    _weight = weight;
    _stretch = stretch;
    _italic = italic;
  }

  FOG_INLINE FontDefs(const FontDefs& other)
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

  FOG_INLINE bool eq(const FontDefs& other) const
  {
    return _packed == other._packed;
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE FontDefs& operator=(const FontDefs& other)
  {
    _packed = other._packed;
    return *this;
  }

  FOG_INLINE bool operator==(const FontDefs& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const FontDefs& other) const { return !eq(other); }

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
// [Fog::FontInfoData]
// ============================================================================

struct FOG_NO_EXPORT FontInfoData
{
  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  FOG_INLINE FontInfoData* addRef() const
  {
    reference.inc();
    return const_cast<FontInfoData*>(this);
  }

  FOG_INLINE void release()
  {
    if (reference.deref())
      fog_api.fontinfo_dFree(this);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Reference count.
  mutable Atomic<size_t> reference;

  //! @brief Variable type and flags.
  uint32_t vType;

  //! @brief Font definitions.
  FontDefs defs;

  //! @brief Font-family name.
  Static<StringW> familyName;

  //! @brief Font file-name, including path.
  //!
  //! @note This member is only filled in case that the font can be loaded
  //! from disk and the font engine is able to get the path and file name from
  //! the native API. In case the this font is a custom font, which needs to be
  //! loaded from disk, fileName is always filled.
  Static<StringW> fileName;
};

// ============================================================================
// [Fog::FontInfo]
// ============================================================================

struct FOG_NO_EXPORT FontInfo
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FontInfo()
  {
    fog_api.fontinfo_ctor(this);
  }

  FOG_INLINE FontInfo(const FontInfo& other)
  {
    fog_api.fontinfo_ctorCopy(this, &other);
  }

#if defined(FOG_CC_HAS_RVALUE)
  FOG_INLINE FontInfo(FontInfo&& other) : _d(other._d) { other._d = NULL; }
#endif // FOG_CC_HAS_RVALUE

  explicit FOG_INLINE FontInfo(FontInfoData* d) :
    _d(d)
  {
  }

  FOG_INLINE ~FontInfo()
  {
    fog_api.fontinfo_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getReference() const { return _d->reference.get(); }
  FOG_INLINE bool isDetached() const { return _d->reference.get() == 1; }

  FOG_INLINE err_t detach() { return _d->reference.get() != 1 ? _detach() : (err_t)ERR_OK; }
  FOG_INLINE err_t _detach() { return fog_api.fontinfo_detach(this); }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setFontInfo(const FontInfo& other) { return fog_api.fontinfo_copy(this, &other); }

  FOG_INLINE bool hasFamilyName() const { return _d->familyName().getLength() != 0; }
  FOG_INLINE bool hasFileName() const { return _d->fileName().getLength() != 0; }

  FOG_INLINE FontDefs getDefs() const { return _d->defs; }
  FOG_INLINE err_t setDefs(const FontDefs& defs) { return fog_api.fontinfo_setDefs(this, &defs); }

  FOG_INLINE const StringW& getFamilyName() const { return _d->familyName; }
  FOG_INLINE err_t setFamilyName(const StringW& familyName) { return fog_api.fontinfo_setFamilyName(this, &familyName); }

  FOG_INLINE const StringW& getFileName() const { return _d->fileName; }
  FOG_INLINE err_t setFileName(const StringW& fileName) { return fog_api.fontinfo_setFileName(this, &fileName); }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    fog_api.fontinfo_reset(this);
  }

  // --------------------------------------------------------------------------
  // [Eq]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const FontInfo& other) const
  {
    return fog_api.fontinfo_eq(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Compare]
  // --------------------------------------------------------------------------

  FOG_INLINE int compare(const FontInfo& other) const
  {
    return fog_api.fontinfo_compare(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE FontInfo& operator=(const FontInfo& other)
  {
    fog_api.fontinfo_copy(this, &other);
    return *this;
  }

  FOG_INLINE bool operator==(const FontInfo& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const FontInfo& other) const { return !eq(other); }

  FOG_INLINE bool operator<=(const FontInfo& other) const { return compare(other) <= 0; }
  FOG_INLINE bool operator< (const FontInfo& other) const { return compare(other) <  0; }
  FOG_INLINE bool operator>=(const FontInfo& other) const { return compare(other) >= 0; }
  FOG_INLINE bool operator> (const FontInfo& other) const { return compare(other) >  0; }

  // --------------------------------------------------------------------------
  // [Statics - Eq]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const FontInfo* a, const FontInfo* b)
  {
    return fog_api.fontinfo_eq(a, b);
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)fog_api.fontinfo_eq;
  }

  // --------------------------------------------------------------------------
  // [Statics - Compare]
  // --------------------------------------------------------------------------

  static FOG_INLINE int compare(const FontInfo* a, const FontInfo* b)
  {
    return fog_api.fontinfo_compare(a, b);
  }

  static FOG_INLINE CompareFunc getCompareFunc()
  {
    return (CompareFunc)fog_api.fontinfo_compare;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(FontInfoData)
};

// ============================================================================
// [Fog::FontCollectionData]
// ============================================================================

struct FOG_NO_EXPORT FontCollectionData
{
  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  FOG_INLINE FontCollectionData* addRef() const
  {
    reference.inc();
    return const_cast<FontCollectionData*>(this);
  }

  FOG_INLINE void release()
  {
    if (reference.deref())
      fog_api.fontcollection_dFree(this);
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

  //! @brief List of collected FontInfo instances.
  Static< List<FontInfo> > fontList;
  //! @brief Hash, contains font families and their count in fontList.
  Static< Hash<StringW, size_t> > fontHash;
};

// ============================================================================
// [Fog::FontCollection]
// ============================================================================

struct FOG_NO_EXPORT FontCollection
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FontCollection()
  {
    fog_api.fontcollection_ctor(this);
  }

  FOG_INLINE FontCollection(const FontCollection& other)
  {
    fog_api.fontcollection_ctorCopy(this, &other);
  }

#if defined(FOG_CC_HAS_RVALUE)
  FOG_INLINE FontCollection(FontCollection&& other) : _d(other._d) { other._d = NULL; }
#endif // FOG_CC_HAS_RVALUE

  explicit FOG_INLINE FontCollection(FontCollectionData* d) :
    _d(d)
  {
  }

  FOG_INLINE ~FontCollection()
  {
    fog_api.fontcollection_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getReference() const { return _d->reference.get(); }
  FOG_INLINE bool isDetached() const { return _d->reference.get() == 1; }

  FOG_INLINE err_t detach() { return _d->reference.get() != 1 ? _detach() : (err_t)ERR_OK; }
  FOG_INLINE err_t _detach() { return fog_api.fontcollection_detach(this); }

  // --------------------------------------------------------------------------
  // [Flags]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getFlags() const { return _d->flags; }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setCollection(const FontCollection& other)
  {
    return fog_api.fontcollection_copy(this, &other);
  }

  FOG_INLINE const List<FontInfo>& getList() const
  {
    return _d->fontList;
  }

  FOG_INLINE err_t setList(const List<FontInfo>& list)
  {
    return fog_api.fontcollection_setList(this, &list);
  }

  FOG_INLINE err_t addItem(const FontInfo& item)
  {
    return fog_api.fontcollection_addItem(this, &item);
  }

  // --------------------------------------------------------------------------
  // [Clear]
  // --------------------------------------------------------------------------

  FOG_INLINE void clear()
  {
    fog_api.fontcollection_clear(this);
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    fog_api.fontcollection_reset(this);
  }

  // --------------------------------------------------------------------------
  // [Eq]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const FontCollection& other) const
  {
    return fog_api.fontcollection_eq(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE FontCollection& operator=(const FontCollection& other)
  {
    fog_api.fontcollection_copy(this, &other);
    return *this;
  }

  FOG_INLINE bool operator==(const FontCollection& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const FontCollection& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Statics - Eq]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const FontCollection* a, const FontCollection* b)
  {
    return fog_api.fontcollection_eq(a, b);
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)fog_api.fontcollection_eq;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(FontCollectionData)
};

// ============================================================================
// [Fog::FontEngineVTable]
// ============================================================================

//! @brief Font-engine virtual table.
struct FOG_NO_EXPORT FontEngineVTable
{
  void (FOG_CDECL* destroy)(FontEngine* self);

  err_t (FOG_CDECL* getAvailableFonts)(const FontEngine* self, List<FontInfo>* dst);
  err_t (FOG_CDECL* getDefaultFamily)(const FontEngine* self, StringW* dst);
  err_t (FOG_CDECL* getFontFace)(const FontEngine* self, FontFace** dst, const StringW* family, const FontDefs* defs);
};

// ============================================================================
// [Fog::FontEngine]
// ============================================================================

struct FOG_NO_EXPORT FontEngine
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FontEngine()
  {
    vtable = NULL;
    engineId = FONT_ENGINE_NULL;
    reserved = 0;
    fontCollection.init();
  }

  FOG_INLINE ~FontEngine()
  {
    fontCollection.destroy();
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

  //! @brief Font engine virtual table.
  FontEngineVTable* vtable;

  //! @brief Font engine id.
  uint32_t engineId;
  //! @brief Reserved for future use.
  uint32_t reserved;

  //! @brief Font collection.
  Static<FontCollection> fontCollection;

private:
  _FOG_NO_COPY(FontEngine)
};

// ============================================================================
// [Fog::FontEngine]
// ============================================================================

/*
struct FOG_NO_EXPORT FontEngine
{
};
*/

// ============================================================================
// [Fog::FontFaceVTable]
// ============================================================================

//! @brief Font-face virtual table.
struct FOG_NO_EXPORT FontFaceVTable
{
  void (FOG_CDECL* destroy)(FontFace* self);

  err_t (FOG_CDECL* getOutlineFromGlyphRunF)(const FontFace* self,
    PathF* dst, uint32_t cntOp,
    const uint32_t* glyphList, size_t itemAdvance,
    const PointF* positionList, size_t positionAdvance,
    size_t length);

  err_t (FOG_CDECL* getOutlineFromGlyphRunD)(const FontFace* self,
    PathD* dst, uint32_t cntOp,
    const uint32_t* glyphList, size_t glyphAdvance,
    const PointF* positionList, size_t positionAdvance,
    size_t length);
};

// ============================================================================
// [Fog::FontFace]
// ============================================================================

//! @brief Font face.
struct FOG_NO_EXPORT FontFace
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FontFace(const FontFaceVTable* vtable_, const StringW& family_)
  {
    vtable = vtable_;

    reference.init(1);
    engineId = FONT_ENGINE_NULL;
    features = NO_FLAGS;
    family.initCustom1(family_);
    designMetrics.reset();
    designEm = 0.0f;
  }

  FOG_INLINE ~FontFace()
  {
    family.destroy();
  }

  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  FOG_INLINE FontFace* addRef() const
  {
    reference.inc();
    return const_cast<FontFace*>(this);
  }

  FOG_INLINE void deref()
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

  FOG_INLINE err_t getOutlineFromGlyphRun(
    PathF& dst, uint32_t cntOp,
    const uint32_t* glyphList, size_t glyphAdvance,
    const PointF* positionList, size_t positionAdvance,
    size_t length)
  {
    return vtable->getOutlineFromGlyphRunF(this,
      &dst, cntOp, glyphList, glyphAdvance, positionList, positionAdvance, length);
  }

  FOG_INLINE err_t getOutlineFromGlyphRun(
    PathD& dst, uint32_t cntOp,
    const uint32_t* glyphList, size_t glyphAdvance,
    const PointF* positionList, size_t positionAdvance,
    size_t length) const
  {
    return vtable->getOutlineFromGlyphRunD(this,
      &dst, cntOp, glyphList, glyphAdvance, positionList, positionAdvance, length);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Font face vtable.
  const FontFaceVTable* vtable;

  //! @brief Reference count.
  mutable Atomic<size_t> reference;

  //! @brief Font-face engine-id.
  uint32_t engineId;

  //! @brief Font-face features.
  uint32_t features;

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
  _FOG_NO_COPY(FontFace)
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
  FontFace* face;
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
  FOG_INLINE FontFace* getFace() const { return _d->face; }
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
  //! @brief Low-level method to create font from existing @ref FontFace, 
  //! typically retrieved by @ref FontEngine.
  FOG_INLINE err_t _init(FontFace* face, float size,
    const FontFeatures& features, const FontMatrix& matrix)
  {
    return fog_api.font_init(this, face, size, &features, &matrix);
  }

  // --------------------------------------------------------------------------
  // [Glyphs]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t getOutlineFromGlyphRun(PathF& dst, uint32_t cntOp,
    const GlyphRun& glyphRun) const
  {
    FOG_ASSERT(glyphRun._itemList.getLength() == glyphRun._positionList.getLength());

    const GlyphItem* glyphs = glyphRun._itemList.getData();
    const GlyphPosition* positions = glyphRun._positionList.getData();
    size_t length = glyphRun.getLength();

    return fog_api.font_getOutlineFromGlyphRunF(this, &dst, cntOp,
      &glyphs->_glyphIndex, sizeof(GlyphItem), &positions->_position, sizeof(GlyphPosition), length);
  }

  FOG_INLINE err_t getOutlineFromGlyphRun(PathD& dst, uint32_t cntOp,
    const GlyphRun& glyphRun) const
  {
    FOG_ASSERT(glyphRun._itemList.getLength() == glyphRun._positionList.getLength());

    const GlyphItem* glyphs = glyphRun._itemList.getData();
    const GlyphPosition* positions = glyphRun._positionList.getData();
    size_t length = glyphRun.getLength();

    return fog_api.font_getOutlineFromGlyphRunD(this, &dst, cntOp,
      &glyphs->_glyphIndex, sizeof(GlyphItem), &positions->_position, sizeof(GlyphPosition), length);
  }

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

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TEXT_FONT_H
