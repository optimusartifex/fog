// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_BUTTON_H
#define _FOG_GUI_BUTTON_H

// [Dependencies]
#include <Fog/Gui/ButtonBase.h>

//! @addtogroup Fog_Gui
//! @{

namespace Fog {

// ============================================================================
// [Fog::Button]
// ============================================================================

struct FOG_API Button : public ButtonBase
{
  FOG_DECLARE_OBJECT(Button, ButtonBase)

  // [Construction / Destruction]

  Button();
  virtual ~Button();

  // [Event Handlers]

  // Fog::Widget Events.
  virtual void onPaint(PaintEvent* e);
};

} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_GUI_BUTTON_H
