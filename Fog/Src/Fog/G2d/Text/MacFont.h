// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_MACFONT_H
#define _FOG_G2D_TEXT_MACFONT_H

// [Dependencies]
#include <Fog/G2d/Text/Font.h>
#include <Fog/G2d/Text/OpenType/OTFace.h>

namespace Fog {

//! @addtogroup Fog_G2d_Text
//! @{

// ============================================================================
// [Fog::MacFace]
// ============================================================================

struct FOG_NO_EXPORT MacFace : public Face
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE MacFace(const FaceVTable* vtable_, const StringW& family_) :
    Face(vtable_, family_)
  {
    ctFont = NULL;
    ot.init();
    ot->_face = this;
  }

  FOG_INLINE ~MacFace()
  {
    ot.destroy();
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  CTFontRef ctFont;
  Static<OTFace> ot;

private:
  FOG_NO_COPY(MacFace)
};

// ============================================================================
// [Fog::MacFontEngine]
// ============================================================================

struct FOG_NO_EXPORT MacFontEngine : public FontEngine
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE MacFontEngine(FontEngineVTable* vtable_) :
    FontEngine(vtable_)
  {
    lock.init();
    cache.init();
    defaultFaceName.init();
  }

  FOG_INLINE ~MacFontEngine()
  {
    defaultFaceName.destroy();
    cache.destroy();
    lock.destroy();
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  mutable Static<Lock> lock;
  mutable Static<FaceCache> cache;

  Static<StringW> defaultFaceName;

private:
  FOG_NO_COPY(MacFontEngine)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TEXT_MACFONT_H
