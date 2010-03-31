// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_WINDOW_H
#define _FOG_GUI_WINDOW_H

// [Dependencies]
#include <Fog/Gui/Composite.h>

//! @addtogroup Fog_Gui
//! @{

namespace Fog {

// ============================================================================
// [Fog::Window]
// ============================================================================

//! @brief Top level window.
struct FOG_API Window : public Composite
{
  FOG_DECLARE_OBJECT(Window, Composite)

  Window(uint32_t createFlags = 0);
  virtual ~Window();

  virtual void onPaint(PaintEvent* e);
};

} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_GUI_WINDOW_H
