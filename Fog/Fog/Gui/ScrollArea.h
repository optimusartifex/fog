// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_SCROLLAREA_H
#define _FOG_GUI_SCROLLAREA_H

// [Dependencies]
#include <Fog/Gui/Frame.h>

namespace Fog {

//! @addtogroup Fog_Gui_Widget
//! @{

// ============================================================================
// [Fog::ScrollArea]
// ============================================================================

//! @brief Scroll area.
struct FOG_API ScrollArea : public Frame
{
  FOG_DECLARE_OBJECT(ScrollArea, Frame)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  ScrollArea(uint32_t createFlags = 0);
  virtual ~ScrollArea();

  // --------------------------------------------------------------------------
  // [Client Geometry]
  // --------------------------------------------------------------------------

  virtual void calcWidgetSize(IntSize& size) const;
  virtual void calcClientGeometry(IntRect& geometry) const;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GUI_SCROLLAREA_H
