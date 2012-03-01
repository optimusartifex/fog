// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_WINFONT_H
#define _FOG_G2D_TEXT_WINFONT_H

// [Dependencies]
#include <Fog/G2d/Text/Font.h>

namespace Fog {

//! @addtogroup Fog_G2d_Text
//! @{

// ============================================================================
// [Fog::WinFace]
// ============================================================================

struct FOG_NO_EXPORT WinFace : public Face
{
  FOG_INLINE WinFace(const FaceVTable* vtable_, const StringW& family_) :
    Face(vtable_, family_)
  {
  }

  FOG_INLINE ~WinFace()
  {
  }

private:
  _FOG_NO_COPY(WinFace)
};

// ============================================================================
// [Fog::WinFontEngine]
// ============================================================================

struct FOG_NO_EXPORT WinFontEngine : public FontEngine
{
  FOG_INLINE WinFontEngine(FontEngineVTable* vtable_) :
    FontEngine(vtable_)
  {
    faceCache.init();
  }

  FOG_INLINE ~WinFontEngine()
  {
    faceCache.destroy();
  }

  Static<FaceCache> faceCache;

private:
  _FOG_NO_COPY(WinFontEngine)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TEXT_WINFONT_H
