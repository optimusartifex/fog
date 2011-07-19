// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Swap.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringTmp_p.h>
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

err_t FTFontProviderData::getFontFace(FontFace** dst, const String& fontFamily)
{
}

err_t FTFontProviderData::getFontList(List<String>& dst)
{
}

String FTFontProviderData::getDefaultFamily()
{
}

// ============================================================================
// [Fog::FTFontProviderData - Statics]
// ============================================================================

} // Fog namespace
