// [Fog-Gui]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Gui/Widget/ButtonBase.h>

FOG_IMPLEMENT_OBJECT(Fog::ButtonBase)

namespace Fog {

// ============================================================================
// [Fog::ButtonBase]
// ============================================================================

ButtonBase::ButtonBase() :
  Widget(),
  _checked(CHECKED_OFF),
  _isMouseOver(false),
  _isMouseDown(false),
  _isSpaceDown(false)
{
}

ButtonBase::~ButtonBase()
{
}

StringW ButtonBase::getText() const
{
  return _text;
}

void ButtonBase::setText(const StringW& text)
{
  _text = text;

  update(WIDGET_UPDATE_PAINT);
}

int ButtonBase::getChecked() const
{
  return _checked;
}

void ButtonBase::setChecked(int value)
{
  if (_checked == value) return;
  if (value > 2) return;

  _checked = value;

  CheckEvent e(value ? EVENT_CHECK : EVENT_UNCHECK);
  sendEvent(&e);
}

bool ButtonBase::isMouseOver() const
{
  return _isMouseOver;
}

bool ButtonBase::isMouseDown() const
{
  return _isMouseDown;
}

bool ButtonBase::isSpaceDown() const
{
  return _isSpaceDown;
}

bool ButtonBase::isDown() const
{
  return (_isMouseDown && _isMouseOver) || _isSpaceDown;
}

void ButtonBase::onFocus(FocusEvent* e)
{
  switch (e->getCode())
  {
    case EVENT_FOCUS_IN:
      break;
    case EVENT_FOCUS_OUT:
      if (_isSpaceDown)
      {
        _isSpaceDown = false;
        update(WIDGET_UPDATE_PAINT);
      }
      break;
  }

  base::onFocus(e);
}

void ButtonBase::onKey(KeyEvent* e)
{
  switch (e->getCode())
  {
    case EVENT_KEY_PRESS:
      if ((e->getKey() & KEY_MASK) == KEY_SPACE)
      {
        _isSpaceDown = true;
        update(WIDGET_UPDATE_PAINT);
        e->accept();
        return;
      }
      break;
    case EVENT_KEY_RELEASE:
      if ((e->getKey() & KEY_MASK) == KEY_SPACE)
      {
        _isSpaceDown = false;
        update(WIDGET_UPDATE_PAINT);
        e->accept();
        return;
      }
      break;
  }

  base::onKey(e);
}

void ButtonBase::onMouse(MouseEvent* e)
{
  switch (e->getCode())
  {
    case EVENT_MOUSE_IN:
      _isMouseOver = true;
      update(WIDGET_UPDATE_PAINT);
      break;
    case EVENT_MOUSE_OUT:
      _isMouseOver = false;
      update(WIDGET_UPDATE_PAINT);
      break;
    case EVENT_MOUSE_PRESS:
      if (e->getButton() == BUTTON_LEFT && !_isMouseDown)
      {
        _isMouseDown = true;
        update(WIDGET_UPDATE_PAINT);
      }
      break;
    case EVENT_MOUSE_RELEASE:
      if (e->getButton() == BUTTON_LEFT && _isMouseDown)
      {
        _isMouseDown = false;
        update(WIDGET_UPDATE_PAINT);
      }
      break;
  }

  base::onMouse(e);
}

void ButtonBase::onCheck(CheckEvent* e)
{
}

void ButtonBase::onUncheck(CheckEvent* e)
{
}

} // Fog namespace
