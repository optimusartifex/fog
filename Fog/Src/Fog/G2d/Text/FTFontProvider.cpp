// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringTmp_p.h>
#include <Fog/Core/Tools/Swap.h>
#include <Fog/G2d/Text/FTFontFace.h>
#include <Fog/G2d/Text/FTFontProvider.h>

namespace Fog {

// ============================================================================
// [Fog::FTFontProviderData - Construction / Destruction]
// ============================================================================

FTFontProviderData::FTFontProviderData()
{
}

FTFontProviderData::~FTFontProviderData()
{
}

// ============================================================================
// [Fog::FTFontProviderData - Interface]
// ============================================================================

err_t FTFontProviderData::getFontFace(FontFace** dst, const StringW& fontFamily)
{
  // TODO:
  *dst = NULL;
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FTFontProviderData::getFontList(List<StringW>& dst)
{
  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

StringW FTFontProviderData::getDefaultFamily()
{
  // TODO:
  return StringW();
}

// ============================================================================
// [Fog::FTFontProviderData - Statics]
// ============================================================================

} // Fog namespace
