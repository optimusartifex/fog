// [Fog-Graphics]
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

//! @addtogroup Fog_Graphics_Text
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

#ifdef __OBJC__
@class NSFont;
#else
class NSFont;
#endif

namespace Fog {

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
    const TransformF& matrix);

private:
  FOG_DISABLE_COPY(MacFontEngine)

  AutoNSAutoreleasePool _pool;
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

  NSFont* font;

  KerningPairF* kerningPairs;
  uint32_t kerningCount;

private:
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
  virtual err_t getOutline(const Char* str, sysuint_t length, PathD& dst);
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
  //! @brief Mac font handle.
  NSFont* font;

private:
  FOG_DISABLE_COPY(MacFontFace)
};

} // Fog namespace

//! @}
#else
#warning "Fog::FontEngine::Mac support not enabled but header file included!"
#endif // FOG_FONT_MAC

// [Guard]
#endif // _FOG_GRAPHICS_MACFONTENGINE_H
