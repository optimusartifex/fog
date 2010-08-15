// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_THEMEENGINE_WINTHEMEENGINE_P_H
#define _FOG_GUI_THEMEENGINE_WINTHEMEENGINE_P_H

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
// [Fog::WinThemeEngine]
// ============================================================================

//! @internal
struct FOG_API WinThemeEngine : public ThemeEngine
{
  FOG_DECLARE_OBJECT(WinThemeEngine, ThemeEngine)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  WinThemeEngine();
  virtual ~WinThemeEngine();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------
};

//! @}

} // Fog namespace

// [Guard]
#endif // FOG_OS_WINDOWS
#endif // _FOG_GUI_THEMEENGINE_WINTHEMEENGINE_P_H
