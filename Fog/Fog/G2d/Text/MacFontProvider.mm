// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Global/Private.h>
#include <Fog/Core/OS/MacUtil.h>
#include <Fog/Core/Tools/StringTmp_p.h>
#include <Fog/G2d/Text/MacFontFace.h>
#include <Fog/G2d/Text/MacFontProvider.h>

// [Dependencies - Mac]
#import <Cocoa/Cocoa.h>

namespace Fog {

// ============================================================================
// [Fog::MacFontProviderData - Construction / Destruction]
// ============================================================================

MacFontProviderData::MacFontProviderData()
{
  this->name.set(Ascii8("Mac"));
  this->id = FONT_PROVIDER_MAC;

  mgr = [NSFontManager sharedFontManager];
}

MacFontProviderData::~MacFontProviderData()
{
}

// ============================================================================
// [Fog::MacFontProviderData - Interface]
// ============================================================================

err_t MacFontProviderData::getFontFace(FontFace** dst, const StringW& fontFamily)
{
  NSString* nsName;
  FOG_RETURN_ON_ERROR(fontFamily.toNSString(&nsName));

  NSFont* nsFont = [NSFont fontWithName: nsName size: 128.0f];
  if (nsFont == nil) return ERR_FONT_NOT_MATCHED;
  
  StringW fontName;
  FOG_RETURN_ON_ERROR(fontName.fromNSString([nsFont familyName]));

  AutoLock locked(lock);
  FontFace* face = fontFaceCache.get(fontName);

  if (face != NULL)
  {
    *dst = face;
    return ERR_OK;
  }

  face = fog_new MacFontFace(this);
  if (FOG_IS_NULL(face)) return ERR_RT_OUT_OF_MEMORY;

  FOG_RETURN_ON_ERROR(reinterpret_cast<MacFontFace*>(face)->_init(fontName, nsFont));

  err_t err = fontFaceCache.put(face->family, face);
  if (FOG_IS_ERROR(err))
  {
    face->deref();
    return err;
  }

  *dst = face;
  return ERR_OK;
}

err_t MacFontProviderData::getFontList(List<StringW>& dst)
{
  NSEnumerator* enumerator = [[mgr availableFontFamilies] objectEnumerator];
  NSString* name;

  while (name = [enumerator nextObject])
  {
    StringW tmp;
    // TODO: What about propagating an error value?
    if (tmp.fromNSString(name) == ERR_OK)
      dst.append(tmp);
  }
  
  return ERR_OK;
}

StringW MacFontProviderData::getDefaultFamily()
{
  return StringW::fromAscii8("Lucida Grande");
}

} // Fog namespace
