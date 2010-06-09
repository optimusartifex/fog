// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_FRAME_H
#define _FOG_GUI_FRAME_H

// [Dependencies]
#include <Fog/Gui/Widget.h>

namespace Fog {

//! @addtogroup Fog_Gui_Widget
//! @{

// ============================================================================
// [Fog::Frame]
// ============================================================================

//! @brief Frame widget.
//!
//! Frame is widget that contains frame type and can paint frame to its
//! non-client area.
struct FOG_API Frame : public Widget
{
  FOG_DECLARE_OBJECT(Frame, Widget)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  Frame(uint32_t createFlags = 0);
  virtual ~Frame();

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getFrameStyle() const { return _frameStyle; }
  void setFrameStyle(uint32_t frameStyle);

  // --------------------------------------------------------------------------
  // [Client Geometry]
  // --------------------------------------------------------------------------

  virtual void calcWidgetSize(IntSize& size) const;
  virtual void calcClientGeometry(IntRect& geometry) const;

  // --------------------------------------------------------------------------
  // [Event Handlers]
  // --------------------------------------------------------------------------

  virtual void onNcPaint(PaintEvent* e);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint32_t _frameStyle;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GUI_FRAME_H
