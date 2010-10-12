// [Fog-Gui]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_THEMEENGINE_DEFAULTTHEMEENGINE_P_H
#define _FOG_GUI_THEMEENGINE_DEFAULTTHEMEENGINE_P_H

// [Dependencies]
#include <Fog/Core/Object.h>
#include <Fog/Gui/ThemeEngine.h>

namespace Fog {

//! @addtogroup Fog_Gui_Private
//! @{

// ============================================================================
// [Fog::DefaultTheme]
// ============================================================================

//! @internal
struct FOG_API DefaultThemeEngine : public ThemeEngine
{
  FOG_DECLARE_OBJECT(DefaultThemeEngine, ThemeEngine)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  DefaultThemeEngine();
  virtual ~DefaultThemeEngine();
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GUI_THEMEENGINE_DEFAULTTHEMEENGINE_P_H
