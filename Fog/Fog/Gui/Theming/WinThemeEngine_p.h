// [Fog-Gui]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_THEMING_WINTHEMEENGINE_P_H
#define _FOG_GUI_THEMING_WINTHEMEENGINE_P_H

#include <Fog/Core/Config/Config.h>
#if defined(FOG_OS_WINDOWS)

// [Dependencies]
#include <Fog/Core/Library/Library.h>
#include <Fog/Core/System/Object.h>
#include <Fog/Gui/Global/Constants.h>
#include <Fog/Gui/Theming/ThemeEngine.h>

#include <uxtheme.h>

namespace Fog {

//! @addtogroup Fog_Gui_Theming
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
#endif // _FOG_GUI_THEMING_WINTHEMEENGINE_P_H
