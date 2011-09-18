// [Fog-Gui]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_THEMING_THEMEENGINE_H
#define _FOG_GUI_THEMING_THEMEENGINE_H

// [Dependencies]
#include <Fog/Core/Kernel/Object.h>

namespace Fog {

//! @addtogroup Fog_Gui_Theming
//! @{

// ============================================================================
// [Fog::ThemeEngine]
// ============================================================================

//! @brief Theme engine.
struct FOG_API ThemeEngine : public Object
{
  FOG_DECLARE_OBJECT(ThemeEngine, Object)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  ThemeEngine();
  virtual ~ThemeEngine();

  // --------------------------------------------------------------------------
  // [Properties]
  // --------------------------------------------------------------------------

  // --------------------------------------------------------------------------
  // [Rendering]
  // --------------------------------------------------------------------------

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static ThemeEngine* _instance;

  static FOG_INLINE ThemeEngine* getInstance() { return _instance; }
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GUI_THEMING_THEMEENGINE_H
