// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_FONT_NULLFONTENGINE_P_H
#define _FOG_G2D_FONT_NULLFONTENGINE_P_H

// [Dependencies]
#include <Fog/G2d/Font/Font.h>

//! @addtogroup Fog_G2d_Font
//! @{

namespace Fog {

//! @internal
struct FOG_NO_EXPORT NullFontFace : public FontFace
{
  NullFontFace();
  virtual ~NullFontFace();

  virtual err_t getGlyphSet(const Char* str, sysuint_t length, GlyphSet& glyphSet);
  virtual err_t getOutline(const Char* str, sysuint_t length, PathD& dst);
  virtual err_t getTextExtents(const Char* str, sysuint_t length, TextExtents& extents);

private:
  FOG_DISABLE_COPY(NullFontFace)
};

} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_G2D_FONT_NULLFONTENGINE_P_H
