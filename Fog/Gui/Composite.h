// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_COMPOSITE_H
#define _FOG_GUI_COMPOSITE_H

// [Dependencies]
#include <Fog/Gui/Widget.h>

namespace Fog {

//! @addtogroup Fog_Gui_Widget
//! @{

// ============================================================================
// [Fog::Composite]
// ============================================================================

//! @brief Composite widget.
struct FOG_API Composite : public Widget
{
  FOG_DECLARE_OBJECT(Composite, Widget)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  Composite(uint32_t createFlags = 0);
  virtual ~Composite();
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GUI_COMPOSITE_H
