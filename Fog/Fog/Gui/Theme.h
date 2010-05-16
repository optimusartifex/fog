// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_THEME_H
#define _FOG_GUI_THEME_H

// [Dependencies]
#include <Fog/Core/Object.h>

namespace Fog {

//! @addtogroup Fog_Gui_Theming
//! @{

// ============================================================================
// [Fog::Theme]
// ============================================================================

//! @brief Theme.
struct FOG_API Theme : public Object
{
  FOG_DECLARE_OBJECT(Theme, Object)

  // [Construction / Destruction]

  Theme();
  virtual ~Theme();

  // [Properties]

  // [Rendering]

  // [Statics]

  static Theme* _instance;

  static FOG_INLINE Theme* getInstance() { return _instance; }
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GUI_THEME_H
