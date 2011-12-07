// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_UI_THEMEENGINE_DEFAULTTHEMEENGINE_P_H
#define _FOG_UI_THEMEENGINE_DEFAULTTHEMEENGINE_P_H

// [Dependencies]
#include <Fog/Core/Kernel/Object.h>
#include <Fog/UI/Theming/ThemeEngine.h>

namespace Fog {

//! @addtogroup Fog_Gui_Theming
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
#endif // _FOG_UI_THEMEENGINE_DEFAULTTHEMEENGINE_P_H
