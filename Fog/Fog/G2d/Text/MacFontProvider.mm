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

#if defined(FOG_OS_IOS)
# import <UIKit/UIKit.h>
#endif // FOG_OS_IOS

namespace Fog {

// ============================================================================
// [Fog::MacFontProviderData - Construction / Destruction]
// ============================================================================

MacFontProviderData::MacFontProviderData()
{
  this->name.set(Ascii8("Mac"));
  this->id = FONT_PROVIDER_MAC;
}

MacFontProviderData::~MacFontProviderData()
{
}

// ============================================================================
// [Fog::MacFontProviderData - Interface]
// ============================================================================

err_t MacFontProviderData::getFontFace(FontFace** dst, const StringW& fontFamily)
{
  CFStringRef cfFontFamily;
  FOG_RETURN_ON_ERROR(fontFamily.toCFString(&cfFontFamily));

  CTFontRef ctFont = CTFontCreateWithName(cfFontFamily, 128.0f, NULL);
  CFRelease(cfFontFamily);

  if (ctFont == NULL)
    return ERR_FONT_NOT_MATCHED;

  CFStringRef cfFontName = CTFontCopyFullName(ctFont);

  StringW fontNameW;
  err_t err = fontNameW.fromCFString(cfFontName);

  CFRelease(cfFontName);

  if (FOG_IS_ERROR(err))
  {
    CFRelease(ctFont);
    return err;
  }

  AutoLock locked(lock);
  FontFace* face = fontFaceCache.get(fontNameW);

  if (face != NULL)
  {
    *dst = face;
    return ERR_OK;
  }

  face = fog_new MacFontFace(this);
  if (FOG_IS_NULL(face))
    return ERR_RT_OUT_OF_MEMORY;

  FOG_RETURN_ON_ERROR(reinterpret_cast<MacFontFace*>(face)->_init(fontNameW, ctFont));

  err = fontFaceCache.put(face->family, face);
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
#if defined(FOG_OS_IOS)
  NSArray* nsArray = [UIFont familyNames];
  err_t err = ERR_OK;

  for (NSString* familyName in nsArray)
  {
    StringW familyNameW;
    err = familyNameW.fromNSString(familyName);
    
    if (FOG_IS_ERROR(err))
      break;

    dst.append(familyNameW);
  }

  return err;
#else
  CFArrayRef cfArray = CTFontManagerCopyAvailableFontFamilyNames();
  if (cfArray == NULL)
  {
    return ERR_RT_OUT_OF_MEMORY;
  }
  
  CFIndex i, length = CFArrayGetCount(cfArray);
  err_t err = ERR_OK;

  for (i = 0; i < length; i++)
  {
    StringW familyNameW;
    err = familyNameW.fromCFString((CFStringRef)CFArrayGetValueAtIndex(cfArray, i));
    
    if (FOG_IS_ERROR(err))
      break;

    dst.append(familyNameW);
  }

  CFRelease(cfArray);
  return err;
#endif // FOG_OS_IOS
}

StringW MacFontProviderData::getDefaultFamily()
{
  return StringW::fromAscii8("Lucida Grande");
}

} // Fog namespace
