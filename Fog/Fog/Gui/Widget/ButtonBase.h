// [Fog-Gui]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_WIDGET_BUTTONBASE_H
#define _FOG_GUI_WIDGET_BUTTONBASE_H

// [Dependencies]
#include <Fog/Gui/Widget/Widget.h>

namespace Fog {

//! @addtogroup Fog_Gui_Widget
//! @{

// ============================================================================
// [Fog::ButtonBase]
// ============================================================================

//! @brief Base class for button-like widgets.
struct FOG_API ButtonBase : public Widget
{
  FOG_DECLARE_OBJECT(ButtonBase, Widget)

  // [Construction / Destruction]

  ButtonBase();
  virtual ~ButtonBase();

  // [Text]

  StringW getText() const;
  void setText(const StringW& text);

  // [Value]

  //! @brief Get whether widget is checked (see @c CheckedState).
  int getChecked() const;
  //! @brief Set checked state of the widget (see @c CheckedState).
  void setChecked(int value);

  //! @brief Set widget state to checked.
  FOG_INLINE void check() { setChecked(true); }
  //! @brief Set widget state to unchecked.
  FOG_INLINE void uncheck() { setChecked(false); }

  // [Interaction]

  bool isMouseOver() const;
  bool isMouseDown() const;
  bool isSpaceDown() const;
  bool isDown() const;

  // [Event Handlers]

  // Fog::Widget Events.
  virtual void onFocus(FocusEvent* e);
  virtual void onKey(KeyEvent* e);
  virtual void onMouse(MouseEvent* e);

  // Fog::ButtonBase Events.
  virtual void onCheck(CheckEvent* e);
  virtual void onUncheck(CheckEvent* e);

  // [Event Map]

  FOG_EVENT_BEGIN()
    FOG_EVENT_DEF(EVENT_CHECK            , onCheck           , CheckEvent     , OVERRIDE)
    FOG_EVENT_DEF(EVENT_UNCHECK          , onUncheck         , CheckEvent     , OVERRIDE)
  FOG_EVENT_END()

protected:
  StringW _text;

  int _checked;
  bool _isMouseOver;
  bool _isMouseDown;
  bool _isSpaceDown;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GUI_WIDGET_BUTTONBASE_H
