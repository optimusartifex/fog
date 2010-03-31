// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_FONTENGINE_NULL_P_H
#define _FOG_GRAPHICS_FONTENGINE_NULL_P_H

// [Dependencies]
#include <Fog/Graphics/Font.h>

//! @addtogroup Fog_Graphics
//! @{

namespace Fog {

struct FOG_HIDDEN NullFontFace : public FontFace
{
  NullFontFace();
  virtual ~NullFontFace();

  virtual err_t getGlyphSet(const Char* str, sysuint_t length, GlyphSet& glyphSet);
  virtual err_t getOutline(const Char* str, sysuint_t length, DoublePath& dst);
  virtual err_t getTextExtents(const Char* str, sysuint_t length, TextExtents& extents);

private:
  FOG_DISABLE_COPY(NullFontFace)
};

} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_GRAPHICS_FONTENGINE_NULL_P_H
