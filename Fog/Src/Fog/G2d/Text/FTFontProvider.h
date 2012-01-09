// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_FTFONTPROVIDER_H
#define _FOG_G2D_TEXT_FTFONTPROVIDER_H

// [Dependencies]
#include <Fog/G2d/Text/FontFaceCache.h>
#include <Fog/G2d/Text/FontProvider.h>

namespace Fog {

//! @addtogroup Fog_G2d_Text
//! @{

// ============================================================================
// [Fog::FTFontProviderData]
// ============================================================================

//! @brief Windows font-provider data.
struct FOG_API FTFontProviderData : public FontProviderData
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FTFontProviderData();
  virtual ~FTFontProviderData();

  // --------------------------------------------------------------------------
  // [Interface]
  // --------------------------------------------------------------------------

  virtual err_t getFontFace(FontFace** dst, const StringW& fontFamily);
  virtual err_t getFontList(List<StringW>& dst);

  virtual StringW getDefaultFamily();

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  FontFaceCache fontFaceCache;

private:
  _FOG_NO_COPY(FTFontProviderData)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TEXT_FTFONTPROVIDER_H
