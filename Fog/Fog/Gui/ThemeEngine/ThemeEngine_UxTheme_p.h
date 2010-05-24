// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_THEMEENGINE_UXTHEME_P_H
#define _FOG_GUI_THEMEENGINE_UXTHEME_P_H

#include <Fog/Core/Build.h>
#if defined(FOG_OS_WINDOWS)

// [Dependencies]
#include <Fog/Core/Library.h>
#include <Fog/Core/Object.h>
#include <Fog/Gui/Constants.h>
#include <Fog/Gui/ThemeEngine.h>

#include <uxtheme.h>

namespace Fog {

//! @addtogroup Fog_Gui_Private
//! @{

// ============================================================================
// [Fog::UxThemeLibrary]
// ============================================================================

//! @internal
//!
//! MSDN: Visual Styles Reference:
//!   http://msdn.microsoft.com/en-us/library/bb773178%28v=VS.85%29.aspx
struct FOG_API UxThemeLibrary
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  UxThemeLibrary();
  ~UxThemeLibrary();

  // --------------------------------------------------------------------------
  // [Initialization]
  // --------------------------------------------------------------------------

  err_t init();
  void close();

  // --------------------------------------------------------------------------
  // [API]
  // --------------------------------------------------------------------------

  // WinXP.
  enum { NUM_XP_SYMBOLS = 8 };
  union
  {
    struct
    {
      HTHEME  (FOG_STDCALL* pOpenThemeData)(HWND hwnd, LPCWSTR pszClassList);
      HRESULT (FOG_STDCALL* pCloseThemeData)(HTHEME hTheme);
      HRESULT (FOG_STDCALL* pEnableTheming)(BOOL fEnable);

      BOOL    (FOG_STDCALL* pIsAppThemed)(void);

      HRESULT (FOG_STDCALL* pDrawThemeBackground)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, const RECT* pRect, const RECT* pClipRect);
      HRESULT (FOG_STDCALL* pDrawThemeText)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int iCharCount, DWORD dwTextFlags, DWORD dwTextFlags2, LPCRECT pRect);

      HRESULT (FOG_STDCALL* pGetThemeBackgroundRegion)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pRect, HRGN *pRegion);
      BOOL    (FOG_STDCALL* pIsThemeBackgroundPartiallyTransparent)(HTHEME hTheme, int iPartId, int iStateId);
    };
    void* xp_addr[NUM_XP_SYMBOLS];
  };

  // WinVista.
  enum { NUM_VISTA_SYMBOLS = 1 };
  union
  {
    struct 
    {
      BOOL    (FOG_STDCALL* pIsCompositionActive)(void);
    };
    void* vista_addr[NUM_VISTA_SYMBOLS];
  };

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Library dll;
  volatile err_t err;

private:
  err_t _init();

  FOG_DISABLE_COPY(UxThemeLibrary)
};

// ============================================================================
// [Fog::UxThemeEngine]
// ============================================================================

//! @internal
struct FOG_API UxThemeEngine : public ThemeEngine
{
  FOG_DECLARE_OBJECT(UxThemeEngine, ThemeEngine)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  UxThemeEngine();
  virtual ~UxThemeEngine();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------
};

//! @}

} // Fog namespace

// [Guard]
#endif // FOG_OS_WINDOWS
#endif // _FOG_GUI_THEMEENGINE_UXTHEME_P_H
