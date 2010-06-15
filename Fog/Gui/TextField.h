// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_TEXTFIELD_H
#define _FOG_GUI_TEXTFIELD_H

// [Dependencies]
#include <Fog/Gui/Frame.h>

namespace Fog {

//! @addtogroup Fog_Gui_Widget
//! @{

// ============================================================================
// [Fog::TextField]
// ============================================================================

//! @brief Scroll area.
struct FOG_API TextField : public Frame
{
  FOG_DECLARE_OBJECT(TextField, Frame)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  TextField(uint32_t createFlags = 0);
  virtual ~TextField();

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  const String& getValue() const { return _value; }
  void setValue(const String& value);

  // --------------------------------------------------------------------------
  // [Layout]
  // --------------------------------------------------------------------------

  virtual IntSize getSizeHint() const;

  // --------------------------------------------------------------------------
  // [Event Handlers]
  // --------------------------------------------------------------------------

  virtual void onPaint(PaintEvent* e);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  String _value;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GUI_TEXTFIELD_H
