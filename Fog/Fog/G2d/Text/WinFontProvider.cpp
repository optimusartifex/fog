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
#include <Fog/G2d/Text/WinFontFace.h>
#include <Fog/G2d/Text/WinFontProvider.h>
#include <Fog/G2d/Text/WinFontUtil_p.h>

#include <wchar.h>

namespace Fog {

// ============================================================================
// [Fog::WinFontProviderData - Construction / Destruction]
// ============================================================================

WinFontProviderData::WinFontProviderData()
{
}

WinFontProviderData::~WinFontProviderData()
{
}

// ============================================================================
// [Fog::WinFontProviderData - Interface]
// ============================================================================

err_t WinFontProviderData::getFontFace(FontFace** dst, const StringW& fontFamily)
{
  LOGFONTW lf;
  FOG_RETURN_ON_ERROR(getLogFontW(&lf, fontFamily));

  StringTmpW<64> lfName;
  lfName.setWChar(lf.lfFaceName);

  AutoLock locked(lock);

  FontFace* face = fontFaceCache.get(lfName);
  if (face != NULL)
  {
    *dst = face;
    return ERR_OK;
  }

  face = fog_new WinFontFace(this);
  if (FOG_IS_NULL(face)) return ERR_RT_OUT_OF_MEMORY;

  err_t err = reinterpret_cast<WinFontFace*>(face)->_init(&lf);
  if (FOG_IS_ERROR(err))
  {
    fog_delete(face);
    return err;
  }

  err = fontFaceCache.put(face->family, face);

  // Shouldn't happen, because the whole access is locked
  if (FOG_IS_ERROR(err))
  {
    if (err == ERR_RT_OBJECT_ALREADY_EXISTS)
    {
      WinFontFace* cached = reinterpret_cast<WinFontFace*>(fontFaceCache.get(face->family));
      FOG_ASSERT(cached != NULL);

      face->deref();
      face = cached;
    }
    else
    {
      face->deref();
      return err;
    }
  }

  *dst = face;
  return ERR_OK;
}

err_t WinFontProviderData::getFontList(List<StringW>& dst)
{
  WinFontEnumContext ctx(dst);

  LOGFONTW lf;
  ZeroMemory(&lf, sizeof(lf));
  lf.lfCharSet = DEFAULT_CHARSET;

  ctx.enumFontFamilies(&lf);
  return ctx.err;
}

StringW WinFontProviderData::getDefaultFamily()
{
  return StringW::fromAscii8("Times New Roman");
}

// ============================================================================
// [Fog::WinFontProviderData - Statics]
// ============================================================================

err_t WinFontProviderData::getLogFontW(LOGFONTW* lfDst, const StringW& fontFamily)
{
  FOG_ASSERT(lfDst != NULL);
  ZeroMemory(lfDst, sizeof(LOGFONTW));

  // --------------------------------------------------------------------------
  // [Context]
  // --------------------------------------------------------------------------

  WinFontInfoContext ctx;
  if (!ctx.isValid())
    return ERR_RT_OUT_OF_MEMORY;

  // --------------------------------------------------------------------------
  // [LogFont]
  // --------------------------------------------------------------------------

  if (fontFamily.getLength() >= FOG_ARRAY_SIZE(lfDst->lfFaceName))
    return ERR_FONT_INVALID_FACE;

  if (fontFamily.getLength() == 0)
  {
    StringW defaultFamily = getDefaultFamily();
    CopyMemory(lfDst->lfFaceName, defaultFamily.getData(), (defaultFamily.getLength() + 1) * sizeof(WCHAR));
  }
  else
  {
    CopyMemory(lfDst->lfFaceName, fontFamily.getData(), (fontFamily.getLength() + 1) * sizeof(WCHAR));
  }

  lfDst->lfHeight = 0;
  lfDst->lfWeight = 0; //FONT_WEIGHT_NORMAL * 100;
  lfDst->lfItalic = 0;
  lfDst->lfCharSet = DEFAULT_CHARSET;

  // --------------------------------------------------------------------------
  // [Enum]
  // --------------------------------------------------------------------------

  ctx.enumFontFamilies(lfDst);
  if (!ctx.acceptable)
    return ERR_FONT_NOT_MATCHED;

  MemOps::copy(lfDst->lfFaceName, ctx.logFont.lfFaceName, FOG_ARRAY_SIZE(ctx.logFont.lfFaceName) * sizeof(WCHAR));
  return ERR_OK;
}

} // Fog namespace
