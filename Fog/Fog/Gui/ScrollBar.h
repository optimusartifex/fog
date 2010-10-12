// [Fog-Gui]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_SCROLLBAR_H
#define _FOG_GUI_SCROLLBAR_H

// [Dependencies]
#include <Fog/Gui/Widget.h>

namespace Fog {

//! @addtogroup Fog_Gui_Widget
//! @{

// ============================================================================
// [Fog::ScrollBar]
// ============================================================================

//! @brief Frame widget.
//!
//! Frame is widget that contains frame type and can paint frame to its
//! non-client area.
struct FOG_API ScrollBar : public Widget
{
  FOG_DECLARE_OBJECT(ScrollBar, Widget)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  ScrollBar(uint32_t createFlags = 0);
  virtual ~ScrollBar();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GUI_SCROLLBAR_H
