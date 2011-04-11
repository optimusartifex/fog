// [Fog-Gui]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_WIDGET_WINDOW_H
#define _FOG_GUI_WIDGET_WINDOW_H

// [Dependencies]
#include <Fog/Gui/Widget/Composite.h>

namespace Fog {

//! @addtogroup Fog_Gui_Widget
//! @{

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

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GUI_WIDGET_WINDOW_H
