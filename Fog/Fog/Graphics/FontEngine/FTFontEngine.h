// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_FTFONTENGINE_H
#define _FOG_GRAPHICS_FTFONTENGINE_H

#include <Fog/Core/Build.h>
#if defined(FOG_FONT_FREETYPE)

// [Dependencies]
#include <Fog/Core/Lock.h>
#include <Fog/Core/String.h>
#include <Fog/Graphics/Font.h>

namespace Fog {

//! @addtogroup Fog_Graphics_Text
//! @{

struct FTFontEngine;
struct FTFontFace;
struct FTFontFile;

// ============================================================================
// [Fog::FTFontFace]
// ============================================================================

//! @brief Font face used by FreeType font engine.
//!
//! @note This class is platform specific.
//!
//! @sa @c FTFontEngine.
struct FOG_API FTFontFace : public FontFace
{
  FTFontFace();
  virtual ~FTFontFace();

  virtual err_t getGlyphSet(const Char* str, sysuint_t length, GlyphSet& glyphSet);
  virtual err_t getOutline(const Char* str, sysuint_t length, DoublePath& dst);
  virtual err_t getTextExtents(const Char* str, sysuint_t length, TextExtents& extents);

  //! @brief Glyph cache lock.
  mutable Lock lock;
  //! @brief Glyph cache.
  GlyphCache glyphCache;
  //! @brief FreeType file reference.
  FTFontFile* file;

private:
  GlyphData* renderGlyph(uint32_t uc);

  FOG_DISABLE_COPY(FTFontFace)
};

// ============================================================================
// [Fog::FTFontEngine]
// ============================================================================

//! @brief FreeType font engine.
//!
//! @note This class is platform specific.
//!
//! @sa @c FTFontFace.
struct FOG_API FTFontEngine : public FontEngine
{
  FTFontEngine();
  virtual ~FTFontEngine();

  virtual List<String> getFontList();

  virtual FontFace* createDefaultFace();

  virtual FontFace* createFace(
    const String& family, float size, 
    const FontOptions& options, 
    const FloatMatrix& matrix);

  virtual List<String> getDefaultFontDirectories();

protected:
  void close();

  UnorderedHash<String, FTFontFile*> _ftCache;

private:
  FOG_DISABLE_COPY(FTFontEngine)
};

//! @}

} // Fog namespace

// [Guard]
#endif // FOG_FONT_FREETYPE
#endif // _FOG_GRAPHICS_FTFONTENGINE_H
