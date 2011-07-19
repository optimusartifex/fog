// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_WINFONTPROVIDER_H
#define _FOG_G2D_TEXT_WINFONTPROVIDER_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/G2d/Text/FontFaceCache.h>
#include <Fog/G2d/Text/FontProvider.h>

namespace Fog {

//! @addtogroup Fog_G2d_Text
//! @{

// ============================================================================
// [Fog::WinFontProviderData]
// ============================================================================

//! @brief Windows font-provider data.
struct FOG_API WinFontProviderData : public FontProviderData
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  WinFontProviderData();
  virtual ~WinFontProviderData();

  // --------------------------------------------------------------------------
  // [Interface]
  // --------------------------------------------------------------------------

  virtual err_t getFontFace(FontFace** dst, const String& fontFamily);
  virtual err_t getFontList(List<String>& dst);

  virtual String getDefaultFamily();

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  err_t getLogFontW(LOGFONTW* lfDst, const String& fontFamily);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  FontFaceCache fontFaceCache;

private:
  _FOG_CLASS_NO_COPY(WinFontProviderData)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TEXT_WINFONTPROVIDER_H
