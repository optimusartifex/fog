// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_MACFONTFACE_H
#define _FOG_G2D_TEXT_MACFONTFACE_H

// [Dependencies]
#include <Fog/Core/Mac/MacDefs.h>
#include <Fog/G2d/Text/FontFace.h>
#include <Fog/G2d/Text/FontKerning.h>
#include <Fog/G2d/Text/GlyphOutlineCache.h>

namespace Fog {

//! @addtogroup Fog_G2d_Text
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct MacFontProviderData;

// ============================================================================
// [Fog::MacFontFace]
// ============================================================================

//! @brief MAC font-face.
struct FOG_API MacFontFace : public FontFace
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  MacFontFace(MacFontProviderData* pd);
  virtual ~MacFontFace();

  // --------------------------------------------------------------------------
  // [Interface]
  // --------------------------------------------------------------------------

  virtual err_t getTextOutline(PathF& dst, const FontData* d, const PointF& pt, const Utf16& str);
  virtual err_t getTextOutline(PathD& dst, const FontData* d, const PointD& pt, const Utf16& str);
  virtual err_t getTextExtents(TextExtents& extents, const FontData* d, const Utf16& str);

  virtual FontKerningTableF* getKerningTable(const FontData* d);

  // --------------------------------------------------------------------------
  // [Private]
  // --------------------------------------------------------------------------

  virtual err_t _renderGlyphOutline(PathF& dst, GlyphMetricsF& metrics, const FontData* d, uint32_t uc, void* ctx);
  virtual err_t _renderGlyphOutline(PathD& dst, GlyphMetricsF& metrics, const FontData* d, uint32_t uc, void* ctx);

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  err_t _init(const String& family, NSFont* src);
  void _reset();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Link to MAC font-provider.
  MacFontProviderData* pd;
  //! @brief Glyph outline cache.
  GlyphOutlineCache outlineCache;

  //! @brief MAC font-face.
  NSFont* nsFont;
  //! @brief Kerning.
  FontKerningTableF* kerningTable;

private:
  _FOG_CLASS_NO_COPY(MacFontFace)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TEXT_MACFONTFACE_H
