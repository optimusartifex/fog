// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_FONT_WINFONTENGINE_H
#define _FOG_G2D_FONT_WINFONTENGINE_H

#include <Fog/Core/Config/Config.h>
#if defined(FOG_FONT_WINDOWS)

// [Dependencies]
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/G2d/Font/Font.h>

#include <windows.h>

//! @addtogroup Fog_G2d_Font
//! @{

namespace Fog {

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct WinFontEngine;

// ============================================================================
// [Fog::WinFontEngine]
// ============================================================================

struct FOG_API WinFontEngine : public FontEngine
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  WinFontEngine();
  virtual ~WinFontEngine();

  // --------------------------------------------------------------------------
  // [Virtuals]
  // --------------------------------------------------------------------------

  virtual List<String> getFontList();

  virtual FontFace* createDefaultFace();

  virtual FontFace* createFace(
    const String& family,
    float size, 
    const FontOptions& options,
    const TransformF& matrix);

private:
  FOG_DISABLE_COPY(WinFontEngine)
};

// ============================================================================
// [Fog::WinFontMaster]
// ============================================================================

struct FOG_API WinFontMaster
{
  WinFontMaster();
  virtual ~WinFontMaster();

  Atomic<sysuint_t> refCount;
  String family;

  LOGFONTW logFont;
  HFONT hFont;

  KerningPairF* kerningPairs;
  uint32_t kerningCount;

private:
  friend struct WinFontEngine;

  FOG_DISABLE_COPY(WinFontMaster)
};

// ============================================================================
// [Fog::WinFontFace]
// ============================================================================

struct FOG_API WinFontFace : public FontFace
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  WinFontFace();
  virtual ~WinFontFace();

  // --------------------------------------------------------------------------
  // [Virtuals]
  // --------------------------------------------------------------------------

  virtual err_t getGlyphSet(const Char* str, sysuint_t length, GlyphSet& glyphSet);
  virtual err_t getOutline(const Char* str, sysuint_t length, PathD& dst);
  virtual err_t getTextExtents(const Char* str, sysuint_t length, TextExtents& extents);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Link (reference counted) to the master font.
  //!
  //! Master font is font with the same family that contains many useful 
  //! information about the font, font options, font metrics and kerning.
  WinFontMaster* master;

  //! @brief Lock.
  mutable Lock lock;
  //! @brief Glyph cache.
  GlyphCache glyphCache;
  //! @brief Windows font handle.
  HFONT hFont;

private:
  //! @brief Renders glyph and returns it.
  //! @note You must use renderBegin() and renderEnd().
  GlyphData* renderGlyph(HDC hdc, uint32_t uc);

  friend struct WinFontEngine;

  FOG_DISABLE_COPY(WinFontFace)
};

} // Fog namespace

//! @}

// [Guard]
#endif // FOG_FONT_WINDOWS
#endif // _FOG_G2D_WINFONTENGINE_H
