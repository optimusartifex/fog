// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_NULLFONTFACE_H
#define _FOG_G2D_TEXT_NULLFONTFACE_H

// [Dependencies]
#include <Fog/G2d/Text/FontFace.h>

namespace Fog {

//! @addtogroup Fog_G2d_Text
//! @{

// ============================================================================
// [Fog::NullFontFace]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT NullFontFace : public FontFace
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  NullFontFace();
  virtual ~NullFontFace();

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
  // [Static]
  // --------------------------------------------------------------------------

  static Static<FontFace> _dnull;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TEXT_NULLFONTFACE_H
