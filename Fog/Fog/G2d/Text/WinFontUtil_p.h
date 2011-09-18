// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_WINFONTUTIL_P_H
#define _FOG_G2D_TEXT_WINFONTUTIL_P_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/Core/Tools/List.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/Swap.h>

namespace Fog {

//! @addtogroup Fog_G2d_Text
//! @{

// ============================================================================
// [Fog::WinAbstractEnumContext]
// ============================================================================

template<typename T>
struct WinAbstractEnumContext
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE WinAbstractEnumContext()
  {
    hdc = CreateCompatibleDC(NULL);
    if (hdc != NULL) SetGraphicsMode(hdc, GM_ADVANCED);
  }

  FOG_INLINE ~WinAbstractEnumContext()
  {
    if (hdc != NULL) DeleteDC(hdc);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isValid() const
  {
    return hdc != NULL;
  }

  FOG_INLINE int enumFontFamilies(LOGFONTW* lf)
  {
    return EnumFontFamiliesExW(hdc, lf, (FONTENUMPROCW)_OnEnumProc, (LPARAM)this, 0);
  }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static int CALLBACK _OnEnumProc(const LOGFONTW* plf, const TEXTMETRICW* ptm, DWORD fontType, LPARAM lParam)
  {
    T* ctx = (T*)lParam;
    return ctx->onEntry(plf, ptm, fontType);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  HDC hdc;

private:
  _FOG_NO_COPY(WinAbstractEnumContext)
};

// ============================================================================
// [Fog::WinFontEnumContext]
// ============================================================================

struct FOG_NO_EXPORT WinFontEnumContext : public WinAbstractEnumContext<WinFontEnumContext>
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE WinFontEnumContext(List<StringW>& fonts) :
    fonts(fonts),
    err(ERR_OK)
  {
  }

  FOG_INLINE ~WinFontEnumContext()
  {
  }

  // --------------------------------------------------------------------------
  // [Events]
  // --------------------------------------------------------------------------

  FOG_INLINE int onEntry(const LOGFONTW* plf, const TEXTMETRICW* ptm, DWORD fontType)
  {
    const WCHAR* faceName = plf->lfFaceName;

    // Accept only true-type fonts.
    if ((fontType & TRUETYPE_FONTTYPE) == 0) return 1;

    // Reject '@'.
    if (faceName[0] == L'@') return 1;

    // Windows sends more fonts that are needed, but equal fonts
    // are usually sent together, so we will simply copy this
    // font to buffer and compare it with previous. If this will
    // match - reject it immediately to save CPU cycles.
    curFaceName.setWChar(faceName);

    if (curFaceName != lastFaceName)
    {
      if (fonts.contains(curFaceName))
      {
        swap(lastFaceName, curFaceName);
      }
      else
      {
        lastFaceName = curFaceName;
        lastFaceName.squeeze();
        fonts.append(lastFaceName);
      }
    }

    // Return 1 to continue listing.
    return 1;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  err_t err;

  List<StringW>& fonts;
  StringW curFaceName;
  StringW lastFaceName;

private:
  _FOG_NO_COPY(WinFontEnumContext)
};

// ============================================================================
// [Fog::WinFontInfoContext]
// ============================================================================

struct FOG_NO_EXPORT WinFontInfoContext : public WinAbstractEnumContext<WinFontInfoContext>
{
  FOG_INLINE WinFontInfoContext() :
    acceptable(0),
    designEmSquare(0)
  {
  }

  // --------------------------------------------------------------------------
  // [Events]
  // --------------------------------------------------------------------------

  FOG_INLINE int onEntry(const LOGFONTW* plf, const TEXTMETRICW* ptm, DWORD fontType)
  {
    // Accept only true-type fonts.
    if ((fontType & TRUETYPE_FONTTYPE) == 0) return 1;

    const NEWTEXTMETRICEXW* pntm = reinterpret_cast<const NEWTEXTMETRICEXW*>(ptm);
    acceptable = true;
    designEmSquare = pntm->ntmTm.ntmCellHeight;
    MemOps::copy(&logFont, plf, sizeof(LOGFONTW));

    return 0;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint32_t acceptable;
  uint32_t designEmSquare;
  LOGFONTW logFont;
};
//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TEXT_WINFONTUTIL_P_H
