// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_THEME_DEFAULT_H
#define _FOG_GUI_THEME_DEFAULT_H

// [Dependencies]
#include <Fog/Gui/Theme.h>

namespace Fog {

// ============================================================================
// [Fog::DefaultTheme]
// ============================================================================

struct FOG_API DefaultTheme : public Theme
{
  FOG_DECLARE_OBJECT(DefaultTheme, Theme)

  // [Construction / Destruction]

  DefaultTheme();
  virtual ~DefaultTheme();

  // [Properties]

  // [Rendering]
};

} // Fog namespace

// [Guard]
#endif // _FOG_GUI_THEME_DEFAULT_H
