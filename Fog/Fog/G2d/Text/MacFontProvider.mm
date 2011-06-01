// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Internal_Core_p.h>
#include <Fog/Core/Mac/MacUtil_Core.h>
#include <Fog/Core/Tools/StringTmp_p.h>
#include <Fog/G2d/Text/MacFontFace.h>
#include <Fog/G2d/Text/MacFontProvider.h>

// [Cocoa]
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

err_t MacFontProviderData::getFontFace(FontFace** dst, const String& fontFamily)
{
  NSString* nsName = MacUtil::NSFromString(fontFamily);

  NSFont* nsFont = [NSFont fontWithName: nsName size: 128.0f];
  if (nsFont == nil) return ERR_FONT_NOT_MATCHED;
  
  err_t err;
  String fontName;

  err = MacUtil::StringFromNS(fontName, [nsFont familyName]);
  if (FOG_IS_ERROR(err)) return err;

  AutoLock locked(lock);
  
  FontFace* face = fontFaceCache.get(fontName);
  if (face != NULL)
  {
    *dst = face;
    return ERR_OK;
  }

  face = fog_new MacFontFace(this);
  if (FOG_IS_NULL(face)) return ERR_RT_OUT_OF_MEMORY;

  err = reinterpret_cast<MacFontFace*>(face)->_init(fontName, nsFont);
  if (FOG_IS_ERROR(err)) return err;

  err = fontFaceCache.put(face->family, face);
  if (FOG_IS_ERROR(err))
  {
    face->deref();
    return err;
  }

  *dst = face;
  return ERR_OK;
}

err_t MacFontProviderData::getFontList(List<String>& dst)
{
  NSEnumerator* enumerator = [[mgr availableFontFamilies] objectEnumerator];
  NSString* name;

  while (name = [enumerator nextObject])
  {
    String tmp;
    if (MacUtil::StringFromNS(tmp, name) == ERR_OK)
      dst.append(tmp);
  }
  
  return ERR_OK;
}

String MacFontProviderData::getDefaultFamily()
{
  return Ascii8("Lucida Grande");
}

} // Fog namespace
