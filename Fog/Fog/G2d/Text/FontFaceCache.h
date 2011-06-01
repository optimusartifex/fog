// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_FONTFACECACHE_H
#define _FOG_G2D_TEXT_FONTFACECACHE_H

// [Dependencies]
#include <Fog/Core/Collection/Hash.h>
#include <Fog/Core/Global/Class.h>
#include <Fog/G2d/Text/FontFace.h>

namespace Fog {

//! @addtogroup Fog_G2d_Text
//! @{

// ============================================================================
// [Fog::FontFaceCache]
// ============================================================================

//! @brief Font-face cache, used by @c FontProvider`s.
struct FOG_API FontFaceCache
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FontFaceCache();
  ~FontFaceCache();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  void reset();
  FontFace* get(const String& family);

  err_t put(const String& family, FontFace* face);
  err_t del(const String& family, FontFace* face);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  UnorderedHash<String, FontFace*> cache;

private:
  _FOG_CLASS_NO_COPY(FontFaceCache)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TEXT_FONTFACECACHE_H
