// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_WINFONT_H
#define _FOG_G2D_TEXT_WINFONT_H

// [Dependencies]
#include <Fog/G2d/Text/Font.h>
#include <Fog/G2d/Text/OpenType/OTFace.h>

namespace Fog {

//! @addtogroup Fog_G2d_Text
//! @{

// ============================================================================
// [Fog::WinFace]
// ============================================================================

struct FOG_NO_EXPORT WinFace : public Face
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE WinFace(const FaceVTable* vtable_, const StringW& family_) :
    Face(vtable_, family_)
  {
    ot.init();
  }

  FOG_INLINE ~WinFace()
  {
    ot.destroy();
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  HFONT hFont;
  Static<OT_Face> ot;

private:
  _FOG_NO_COPY(WinFace)
};

// ============================================================================
// [Fog::WinFontEngine]
// ============================================================================

struct FOG_NO_EXPORT WinFontEngine : public FontEngine
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE WinFontEngine(FontEngineVTable* vtable_) :
    FontEngine(vtable_)
  {
    lock.init();
    cache.init();
  }

  FOG_INLINE ~WinFontEngine()
  {
    cache.destroy();
    lock.destroy();
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  mutable Static<Lock> lock;
  mutable Static<FaceCache> cache;

private:
  _FOG_NO_COPY(WinFontEngine)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TEXT_WINFONT_H
