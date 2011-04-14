// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_FONT_FONT_H
#define _FOG_G2D_FONT_FONT_H

// [Dependencies]
#include <Fog/Core/Collection/Hash.h>
#include <Fog/Core/Collection/HashUtil.h>
#include <Fog/Core/Global/Static.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/G2d/Font/Glyph.h>
#include <Fog/G2d/Font/GlyphCache.h>
#include <Fog/G2d/Font/GlyphSet.h>
#include <Fog/G2d/Font/KerningPair.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Global/Constants.h>

namespace Fog {

//! @addtogroup Fog_G2d_Font
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct Font;
struct FontEngine;
struct FontFace;
struct FontManager;
struct FontMetrics;
struct FontOptions;
struct GlyphSet;
struct PathF;
struct PathD;
struct TextExtents;

// ============================================================================
// [Fog::FontOptions]
// ============================================================================

//! @brief Font options.
struct FOG_NO_EXPORT FontOptions
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FontOptions()
  {
    _created();
    reset();
  }
  
  FOG_INLINE FontOptions(
    uint8_t weight,
    uint8_t style,
    uint8_t decoration,
    uint8_t kerning = FONT_KERNING_DEFAULT,
    uint8_t hinting = FONT_HINTING_DEFAULT)
  {
    _created();

    _weight = weight;
    _style = style;
    _decoration = decoration;
    _kerning = kerning;
    _hinting = hinting;
  }
  
  FOG_INLINE FontOptions(const FontOptions& other)
  {
    _created();

    _weight = other._weight;
    _style = other._style;
    _decoration = other._decoration;
    _kerning = other._kerning;
    _hinting = other._hinting;
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getWeight() const { return _weight; } 
  FOG_INLINE uint32_t getStyle() const { return _style; } 
  FOG_INLINE uint32_t getDecoration() const { return _decoration; } 
  FOG_INLINE uint32_t getKerning() const { return _kerning; } 
  FOG_INLINE uint32_t getHinting() const { return _hinting; } 

  FOG_INLINE void setWeight(uint32_t weight) { _weight = (uint8_t)weight; }
  FOG_INLINE void setStyle(uint32_t style) { _style = (uint8_t)style; }
  FOG_INLINE void setDecoration(uint32_t decoration) { _decoration = (uint8_t)decoration; }
  FOG_INLINE void setKerning(uint32_t kerning) { _kerning = (uint8_t)kerning; }
  FOG_INLINE void setHinting(uint32_t hinting) { _hinting = (uint8_t)hinting; }

  FOG_INLINE void reset()
  {
    _weight = FONT_WEIGHT_NORMAL;
    _style = FONT_STYLE_NORMAL;
    _decoration = FONT_DECORATION_NONE;
    _kerning = FONT_KERNING_DEFAULT;
    _hinting = FONT_HINTING_DEFAULT;
  }

  // --------------------------------------------------------------------------
  // [HashCode]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getHashCode() const
  {
    return HashUtil::combineHash(
      _data[0],
      _data[1]);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE FontOptions& operator=(const FontOptions& other)
  {
    _data[0] = other._data[0];
    _data[1] = other._data[1];

    return *this;
  }

  FOG_INLINE bool operator==(const FontOptions& other) const
  {
    return (_data[0] == other._data[0]) &
           (_data[1] == other._data[1]) ;
  }

  FOG_INLINE bool operator!=(const FontOptions& other) const
  {
    return (_data[0] != other._data[0]) |
           (_data[1] != other._data[1]) ;
  }

  // --------------------------------------------------------------------------
  // [Internal]
  // --------------------------------------------------------------------------

  //! @brief Called by constructor, clears data where are unused fields.
  FOG_INLINE void _created()
  {
    _data[1] = 0;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  union
  {
    struct
    {
      //! @brief Font weight.
      uint8_t _weight;
      //! @brief Font style.
      uint8_t _style;
      //! @brief Font decoration.
      uint8_t _decoration;
      //! @brief Font kerning.
      uint8_t _kerning;
      //! @brief Font hinting.
      uint8_t _hinting;
      //! @brief Unused data, set to zeros in constructor.
      uint8_t _unused[3];
    };
    //! @brief Data as 4-BYTE (DWORD) array (for fast compare/copy/set).
    uint32_t _data[2];
  };
};

// ============================================================================
// [Fog::FontMetrics]
// ============================================================================

//! @brief Font metrics.
struct FOG_NO_EXPORT FontMetrics
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE float getSize() const { return _size; }
  FOG_INLINE float getAscent() const { return _ascent; }
  FOG_INLINE float getDescent() const { return _descent; }
  FOG_INLINE float getHeight() const { return _height; }
  FOG_INLINE float getAverageWidth() const { return _averageWidth; }
  FOG_INLINE float getMaximumWidth() const { return _maximumWidth; }

  FOG_INLINE void setSize(float size) { _size = size; }
  FOG_INLINE void setAscent(float ascent) { _ascent = ascent; }
  FOG_INLINE void setDescent(float descent) { _descent = descent; }
  FOG_INLINE void setHeight(float height) { _height = height; }
  FOG_INLINE void setAverageWidth(float averageWidth) { _averageWidth = averageWidth; }
  FOG_INLINE void setMaximumWidth(float maximumWidth) { _maximumWidth = maximumWidth; }

  FOG_INLINE void reset()
  {
    _size = 0.0f;
    _ascent = 0.0f;
    _descent = 0.0f;
    _height = 0.0f;
    _averageWidth = 0.0f;
    _maximumWidth = 0.0f;
  }

  // --------------------------------------------------------------------------
  // [HashCode]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getHashCode() const
  {
    return HashUtil::combineHash(
      HashUtil::getHashCode(_size),
      HashUtil::getHashCode(_ascent),
      HashUtil::getHashCode(_descent),
      HashUtil::getHashCode(_height),
      HashUtil::getHashCode(_averageWidth),
      HashUtil::getHashCode(_maximumWidth));
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Font size.
  float _size;
  //! @brief Font ascent.
  float _ascent;
  //! @brief Font descent.
  float _descent;
  //! @brief Height.
  float _height;
  //! @brief Average width.
  float _averageWidth;
  //! @brief Maximum width of largest character.
  float _maximumWidth;

private:
  friend struct Font;
};

// ============================================================================
// [Fog::FontFace]
// ============================================================================

//! @brief Font face.
//!
//! Font face is reference counted class used inside of @c Font.
struct FOG_API FontFace
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FontFace();
  virtual ~FontFace();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual FontFace* ref() const;
  virtual void deref();

  virtual err_t getGlyphSet(const Char* str, sysuint_t length, GlyphSet& glyphSet) = 0;
  virtual err_t getOutline(const Char* str, sysuint_t length, PathD& dst) = 0;
  virtual err_t getTextExtents(const Char* str, sysuint_t length, TextExtents& extents) = 0;

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual uint32_t getHashCode() const;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Reference count.
  mutable Atomic<sysuint_t> refCount;

  //! @brief Font face type.
  uint32_t type;
  //! @brief Font face flags.
  uint32_t flags;

  //! @brief Font family.
  String family;
  //! @brief Font options.
  FontOptions options;
  //! @brief Font metrics.
  FontMetrics metrics;
  //! @brief Transformation matrix.
  TransformF transform;
};

// ============================================================================
// [Fog::FontEngine]
// ============================================================================

//! @brief Font engine.
struct FOG_API FontEngine
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FontEngine(uint32_t type);
  virtual ~FontEngine();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual List<String> getFontList() = 0;

  virtual FontFace* createDefaultFace() = 0;

  virtual FontFace* createFace(
    const String& family,
    float size,
    const FontOptions& options,
    const TransformF& matrix) = 0;

  virtual List<String> getDefaultFontDirectories();

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getType() const { return _type; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

protected:
  uint32_t _type;

private:
  FOG_DISABLE_COPY(FontEngine)
};

// ============================================================================
// [Fog::Font]
// ============================================================================

//! @brief Font.
struct FOG_API Font
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  Font();
  Font(const Font& other);
  explicit FOG_INLINE Font(FontFace* d) : _d(d) {}
  ~Font();

  // --------------------------------------------------------------------------
  // [Implicit Sharing]
  // --------------------------------------------------------------------------

  //! @copydoc Doxygen::Implicit::getRefCount().
  FOG_INLINE sysuint_t getRefCount() const { return _d->refCount.get(); }
  //! @copydoc Doxygen::Implicit::reset().
  void reset();

  // --------------------------------------------------------------------------
  // [Face Type / Flags]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getType() const { return _d->type; }
  FOG_INLINE uint32_t getFlags() const { return _d->flags; }

  FOG_INLINE bool isCached() const { return (_d->flags & FONT_FACE_CACHED) != 0; }
  FOG_INLINE bool hasGlyphs() const { return (_d->flags & FONT_FACE_GLYPHS) != 0; }
  FOG_INLINE bool hasOutlines() const { return (_d->flags & FONT_FACE_OUTLINES) != 0; }
  FOG_INLINE bool hasKerning() const { return (_d->flags & FONT_FACE_KERNING) != 0; }
  FOG_INLINE bool hasQuantizedMetrics() const { return (_d->flags & FONT_FACE_QUANTIZED_METRICS) != 0; }

  // --------------------------------------------------------------------------
  // [Face Family / Options / Metrics / Matrix]
  // --------------------------------------------------------------------------

  FOG_INLINE const String& getFamily() const { return _d->family; }

  FOG_INLINE const FontOptions& getOptions() { return _d->options; }
  FOG_INLINE uint32_t getWeight() const { return _d->options.getWeight(); }
  FOG_INLINE uint32_t getStyle() const { return _d->options.getStyle(); }
  FOG_INLINE uint32_t getDecoration() const { return _d->options.getDecoration(); }
  FOG_INLINE uint32_t getKerning() const { return _d->options.getKerning(); }
  FOG_INLINE uint32_t getHinting() const { return _d->options.getHinting(); }

  FOG_INLINE const FontMetrics& getMetrics() const { return _d->metrics; }
  FOG_INLINE float getSize() const { return _d->metrics._size; }
  FOG_INLINE float getAscent() const { return _d->metrics._ascent; }
  FOG_INLINE float getDescent() const { return _d->metrics._descent; }
  FOG_INLINE float getHeight() const { return _d->metrics._height; }
  FOG_INLINE float getAverageWidth() const { return _d->metrics._averageWidth; }
  FOG_INLINE float getMaximumWidth() const { return _d->metrics._maximumWidth; }

  FOG_INLINE const TransformF& getTransform() const { return _d->transform; }

  err_t setFamily(const String& family);

  err_t setOptions(const FontOptions& options);
  err_t setWeight(uint32_t weight);
  err_t setStyle(uint32_t style);
  err_t setDecoration(uint32_t decoration);
  err_t setKerning(uint32_t kerning);
  err_t setHinting(uint32_t hinting);

  err_t setSize(float size);
  err_t setTransform(const TransformF& transform);

  err_t setFont(const Font& other);
  err_t setFont(const String& family, float size);
  err_t setFont(const String& family, float size, const FontOptions& options);
  err_t setFont(const String& family, float size, const FontOptions& options, const TransformF& transform);

  // --------------------------------------------------------------------------
  // [Face Methods]
  // --------------------------------------------------------------------------

  err_t getGlyphSet(const String& str, GlyphSet& glyphSet) const;
  err_t getGlyphSet(const Char* str, sysuint_t length, GlyphSet& glyphSet) const;

  err_t getOutline(const String& str, PathD& dst) const;
  err_t getOutline(const Char* str, sysuint_t length, PathD& dst) const;

  err_t getTextExtents(const String& str, TextExtents& extents) const;
  err_t getTextExtents(const Char* str, sysuint_t length, TextExtents& extents) const;

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  Font& operator=(const Font& other);

  // --------------------------------------------------------------------------
  // [Font Path Management]
  // --------------------------------------------------------------------------

  static bool addFontPath(const String& path);
  static void addFontPaths(const List<String>& paths);
  static bool removeFontPath(const String& path);
  static bool hasFontPath(const String& path);
  static bool findFontFile(const String& fileName, String& dest);
  static List<String> getPathList();

  // --------------------------------------------------------------------------
  // [Font List Management]
  // --------------------------------------------------------------------------

  static List<String> getFontList();

  // --------------------------------------------------------------------------
  // [Font Face Management]
  // --------------------------------------------------------------------------

  static err_t setFontFace(
    Font& font,
    const String& family,
    float size,
    const FontOptions& options,
    const TransformF& matrix);

  static err_t putFace(FontFace* face);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  FOG_DECLARE_D(FontFace)

private:
  friend struct FontManager;
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::Font, Fog::TYPEINFO_MOVABLE)

// [Guard]
#endif // _FOG_G2D_FONT_FONT_H
