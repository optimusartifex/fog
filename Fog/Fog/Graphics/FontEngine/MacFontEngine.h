// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_MACFONTENGINE_H
#define _FOG_GRAPHICS_MACFONTENGINE_H

// [Dependencies]
#include <Fog/Core/Build.h>

#if defined(FOG_FONT_MAC)

#include <Fog/Core/MacUtil.h>
#include <Fog/Graphics/Font.h>

#include <Carbon/Carbon.h>

//! @addtogroup Fog_Graphics_Text
//! @{

namespace Fog {

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct MacFontEngine;

// ============================================================================
// [Fog::MacFontEngine]
// ============================================================================

struct FOG_API MacFontEngine : public FontEngine
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  MacFontEngine();
  virtual ~MacFontEngine();

  // --------------------------------------------------------------------------
  // [Virtuals]
  // --------------------------------------------------------------------------

  virtual List<String> getFontList();

  virtual FontFace* createDefaultFace();

  virtual FontFace* createFace(
    const String& family,
    float size, 
    const FontOptions& options,
    const FloatMatrix& matrix);

private:
  FOG_DISABLE_COPY(MacFontEngine)
};

// ============================================================================
// [Fog::MacFontMaster]
// ============================================================================

struct FOG_API MacFontMaster
{
  MacFontMaster();
  virtual ~MacFontMaster();

  Atomic<sysuint_t> refCount;
  String family;

  CFType<CTFontRef> font;

  FloatKerningPair* kerningPairs;
  uint32_t kerningCount;

private:
  friend struct MacFontEngine;

  FOG_DISABLE_COPY(MacFontMaster)
};

// ============================================================================
// [Fog::MacFontFace]
// ============================================================================

struct FOG_API MacFontFace : public FontFace
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  MacFontFace();
  virtual ~MacFontFace();

  // --------------------------------------------------------------------------
  // [Virtuals]
  // --------------------------------------------------------------------------

  virtual err_t getGlyphSet(const Char* str, sysuint_t length, GlyphSet& glyphSet);
  virtual err_t getOutline(const Char* str, sysuint_t length, DoublePath& dst);
  virtual err_t getTextExtents(const Char* str, sysuint_t length, TextExtents& extents);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Link (reference counted) to the master font.
  //!
  //! Master font is font with the same family that contains many useful 
  //! information about the font, font options, font metrics and kerning.
  MacFontMaster* master;

  //! @brief Lock.
  mutable Lock lock;
  //! @brief Glyph cache.
  GlyphCache glyphCache;
  //! @brief Mac font handle.
  CFType<CTFontRef> font;

private:
  DoublePath renderGlyph(uint32_t uc, DoublePoint& offset);

  friend struct MacFontEngine;

  FOG_DISABLE_COPY(MacFontFace)
};

} // Fog namespace

//! @}

#else
#warning "Fog::FontEngine::Mac support not enabled but header file included!"
#endif // FOG_FONT_MAC

// [Guard]
#endif // _FOG_GRAPHICS_MACFONTENGINE_H
