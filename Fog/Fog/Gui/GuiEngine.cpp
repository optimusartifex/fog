// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/Gui/GuiEngine.h>
#include <Fog/Core/Application.h>

FOG_IMPLEMENT_OBJECT(Fog::GuiEngine)
FOG_IMPLEMENT_OBJECT(Fog::GuiWindow)

namespace Fog {

// ============================================================================
// [Fog::GuiEngine]
// ============================================================================

GuiEngine::GuiEngine() :
  _initialized(false)
{
}

GuiEngine::~GuiEngine()
{
}

uint32_t GuiEngine::keyToModifier(uint32_t key) const
{
  switch (key)
  {
    case KEY_LEFT_SHIFT : return MODIFIER_LEFT_SHIFT ;
    case KEY_RIGHT_SHIFT: return MODIFIER_RIGHT_SHIFT;
    case KEY_LEFT_CTRL  : return MODIFIER_LEFT_CTRL  ;
    case KEY_RIGHT_CTRL : return MODIFIER_RIGHT_CTRL ;
    case KEY_LEFT_ALT   : return MODIFIER_LEFT_ALT   ;
    case KEY_RIGHT_ALT  : return MODIFIER_RIGHT_ALT  ;
  }
  return 0;
}

// ============================================================================
// [Fog::GuiWindow]
// ============================================================================

GuiWindow::GuiWindow(Widget* widget) :
  _widget(widget),
  _handle(NULL),
  _backingStore(NULL),
  _enabled(true),
  _visible(true),
  _hasFocus(false),
  _needBlit(true),
  _isDirty(false),
  _windowRect(0, 0, 1, 1),
  _clientRect(0, 0, 1, 1),
  _owner(NULL),
  _modalpolicy(MODAL_NONE),
  _modal(0),
  _lastmodal(NULL),
  _activatelist(NULL)
{
}

GuiWindow::~GuiWindow()
{
}

GuiWindow* GuiWindow::getModalWindow()
{
  if (getOwner())
  {
    // Normally this would be this. But maybe we need the 
    // owner-variable for other tasks, too!
    return getOwner()->_modal;
  }

  return NULL;
}

void GuiWindow::setOwner(GuiWindow* w)
{
  _owner = w;
  setOwner_sys();
}

void GuiWindow::releaseOwner()
{
  releaseOwner_sys();
  _owner = 0;
}

// ============================================================================
// [Fog::GuiBackBuffer]
// ============================================================================

GuiBackBuffer::GuiBackBuffer()
{
  _clear();
}

GuiBackBuffer::~GuiBackBuffer()
{
}

void GuiBackBuffer::_clear()
{
  _type = 0;
  _depth = 0;

  _buffer.data = NULL;
  _buffer.width = 0;
  _buffer.height = 0;
  _buffer.format = PIXEL_FORMAT_NULL;
  _buffer.stride = 0;

  _cachedWidth = 0;
  _cachedHeight = 0;

  _primaryPixels = NULL;
  _primaryStride = 0;

  _secondaryPixels = NULL;
  _secondaryStride = 0;

  _convertFunc = NULL;
  _convertDepth = 0;

  _createdTime = TimeTicks();
  _expireTime = TimeTicks();
}

bool GuiBackBuffer::expired(TimeTicks now) const
{
  return (_buffer.width != _cachedWidth || _buffer.height != _cachedHeight) && (now >= _expireTime);
}

} // Fog namespace
