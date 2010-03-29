// [Fog-Graphics Library - Public API]
//
// [Licence]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_FONTENGINE_GDI_H
#define _FOG_GRAPHICS_FONTENGINE_GDI_H

// [Dependencies]
#include <Fog/Build/Build.h>

#if defined(FOG_FONT_WINDOWS)

#include <Fog/Core/Lock.h>
#include <Fog/Core/String.h>
#include <Fog/Graphics/Font.h>
#include <Fog/Graphics/FontEngine.h>

#include <windows.h>

//! @addtogroup Fog_Graphics
//! @{

namespace Fog {

struct WinFontEngine;

// ============================================================================
// [Fog::WinFontFace]
// ============================================================================

struct FOG_API WinFontFace : public FontFace
{
  // Lock
  mutable Lock lock;
  // Glyph cache:
  GlyphCache glyphCache;
  // Windows font handle
  HFONT hFont;

  WinFontFace();
  virtual ~WinFontFace();

  virtual err_t getGlyphSet(const Char* str, sysuint_t length, GlyphSet& glyphSet);
  virtual err_t getOutline(const Char* str, sysuint_t length, Path& dst);
  virtual err_t getTextExtents(const Char* str, sysuint_t length, TextExtents& extents);

private:
  //! @brief Renders glyph and returns it.
  //! @note You must use renderBegin() and renderEnd().
  GlyphData* renderGlyph(HDC hdc, uint32_t uc);

  friend struct WinFontEngine;

  FOG_DISABLE_COPY(WinFontFace)
};

// ============================================================================
// [Fog::WinFontEngine]
// ============================================================================

struct FOG_API WinFontEngine : public FontEngine
{
  WinFontEngine();
  virtual ~WinFontEngine();

  virtual List<String> getFontList();

  virtual FontFace* createDefaultFace();

  virtual FontFace* createFace(
    const String& family, uint32_t size, 
    const FontCaps& caps);

private:
  FOG_DISABLE_COPY(WinFontEngine)
};

} // Fog namespace

//! @}

#else
#warning "Fog::FontEngine::Gdi support not enabled but header file included!"
#endif // FOG_FONT_WINDOWS

// [Guard]
#endif // _FOG_GRAPHICS_FONTENGINE_GDI_H
