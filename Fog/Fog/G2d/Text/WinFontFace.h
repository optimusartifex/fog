// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_WINFONTFACE_H
#define _FOG_G2D_TEXT_WINFONTFACE_H

// [Dependencies]
#include <Fog/G2d/Text/FontFace.h>
#include <Fog/G2d/Text/GlyphOutlineCache.h>

namespace Fog {

//! @addtogroup Fog_G2d_Text
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct WinFontProviderData;

// ============================================================================
// [Fog::WinFontHandle]
// ============================================================================

//! @internal
//!
//! @brief Windows font-handle, including basic information fetched from it.
struct FOG_API WinFontHandle
{
  //! @brief HFONT handle.
  HFONT hFont;

  //! @brief Kerning table (NULL if font-face doesn't contain kerning).
  FontKerningTableF* kerningTable;
};

// ============================================================================
// [Fog::WinFontFace]
// ============================================================================

//! @brief Windows font-face.
struct FOG_API WinFontFace : public FontFace
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  WinFontFace(WinFontProviderData* pd);
  virtual ~WinFontFace();

  // --------------------------------------------------------------------------
  // [Interface]
  // --------------------------------------------------------------------------

  virtual err_t getTextOutline(PathF& dst, const FontData* d, const PointF& pt, const StubW& str);
  virtual err_t getTextOutline(PathD& dst, const FontData* d, const PointD& pt, const StubW& str);
  virtual err_t getTextExtents(TextExtents& extents, const FontData* d, const StubW& str);

  virtual FontKerningTableF* getKerningTable(const FontData* d);

  // --------------------------------------------------------------------------
  // [Private]
  // --------------------------------------------------------------------------

  virtual err_t _renderGlyphOutline(PathF& dst, GlyphMetricsF& metrics, const FontData* d, uint32_t uc, void* ctx);
  virtual err_t _renderGlyphOutline(PathD& dst, GlyphMetricsF& metrics, const FontData* d, uint32_t uc, void* ctx);

  // --------------------------------------------------------------------------
  // [Specific]
  // --------------------------------------------------------------------------

  err_t _init(const LOGFONTW* logFont);
  void _reset();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  enum { HFONT_CACHE_SIZE = 4 };

  //! @brief Link to Windows font-provider.
  WinFontProviderData* pd;

  //! @brief Glyph outline cache.
  GlyphOutlineCache outlineCache;

  //! @brief Master handle.
  WinFontHandle hMaster;
  //! @brief Cached handles.
  WinFontHandle hCache[HFONT_CACHE_SIZE];

private:
  _FOG_NO_COPY(WinFontFace)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TEXT_WINFONTFACE_H
