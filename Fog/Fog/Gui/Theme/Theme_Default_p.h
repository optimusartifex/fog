// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_THEME_DEFAULT_P_H
#define _FOG_GUI_THEME_DEFAULT_P_H

// [Dependencies]
#include <Fog/Gui/Theme.h>

namespace Fog {

//! @addtogroup Fog_Gui_Private
//! @{

// ============================================================================
// [Fog::DefaultTheme]
// ============================================================================

//! @internal
struct FOG_API DefaultTheme : public Theme
{
  FOG_DECLARE_OBJECT(DefaultTheme, Theme)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  DefaultTheme();
  virtual ~DefaultTheme();
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GUI_THEME_DEFAULT_P_H
