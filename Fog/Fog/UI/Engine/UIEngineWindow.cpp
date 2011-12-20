// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/UI/Engine/UIEngine.h>
#include <Fog/UI/Engine/UIEngineWindow.h>

namespace Fog {

// ============================================================================
// [Fog::UIEngineWindowimpl]
// ============================================================================

UIEngineWindowImpl::UIEngineWindowImpl(UIEngine* engine, UIEngineWindow* window) :
  _engine(engine),
  _window(window),
  _handle(NULL),
  _isEnabled(true),
  _isEnabledToParent(false),
  _isVisible(true),
  _isVisibleToParent(false),
  _isDirty(false),
  _hasFocus(false),
  _isWindowOpaque(true),
  _isBufferCacheEnabled(true),
  _isFixed(false),
  _orientation(ORIENTATION_HORIZONTAL),
  _shouldUpdate(false),
  _shouldPaint(false),
  _shouldBlit(false),
  _resetHints(true),
  _windowDepth(0),
  _windowState(WINDOW_STATE_NORMAL),
  _windowType(WINDOW_TYPE_NORMAL),
  _windowHints(NO_FLAGS),
  _windowScreen(0),
  _windowOpacity(1.0f),
  _windowGeometry(0, 0, 0, 0),
  _clientGeometry(0, 0, 0, 0),
  _sizeGranularity(1, 1),
  _bufferCacheSize(0, 0),
  _bufferCacheCreated(),
  _bufferCacheExpire()
{
  _bufferData.reset();

  if (_window)
    _window->_d = this;
}

UIEngineWindowImpl::~UIEngineWindowImpl()
{
  if (_engine)
  {
    size_t index = _engine->_dirtyList.indexOf(this->_window);
    _engine->_dirtyList.removeAt(index);
  }

  if (_window)
    _window->_d = NULL;
}

// ============================================================================
// [Fog::UIEngineWindow - Window - Double-Buffer]
// ============================================================================

err_t UIEngineWindowImpl::resizeDoubleBuffer(const SizeI& size)
{
  int targetWidth = size.w;
  int targetHeight = size.h;

  if (size.w == 0 || size.h == 0)
  {
    return freeDoubleBuffer();
  }

  if (_isBufferCacheEnabled)
  {
    if (size.w <= _bufferCacheSize.w && size.h <= _bufferCacheSize.h)
    {
      _bufferData.size = size;
      return ERR_OK;
    }

    // Don't create smaller buffer that previous!
    targetWidth  = Math::max<int>(size.w, _bufferCacheSize.w);
    targetHeight = Math::max<int>(size.h, _bufferCacheSize.h);

    // Cache using 128x128 blocks.
    targetWidth  = (targetWidth  + 127) & ~127;
    targetHeight = (targetHeight + 127) & ~127;
  }

  FOG_RETURN_ON_ERROR(freeDoubleBuffer());
  FOG_RETURN_ON_ERROR(allocDoubleBuffer(SizeI(targetWidth, targetHeight)));

  _bufferData.size = size;

  _bufferCacheSize = SizeI(targetWidth, targetHeight);
  _bufferCacheCreated = TimeTicks::now();
  _bufferCacheExpire = _bufferCacheCreated + TimeDelta::fromSeconds(10);

  return ERR_OK;
}

// ============================================================================
// [Fog::UIEngineWindow - Construction / Destruction]
// ============================================================================

UIEngineWindow::UIEngineWindow(UIEngine* engine, uint32_t hints)
{
  if (engine != NULL)
    engine->createWindow(this, hints);
}

UIEngineWindow::~UIEngineWindow()
{
  if (_d != NULL)
    _d->_engine->destroyWindow(this);
}

// ============================================================================
// [Fog::UIEngineWindow - Accessors]
// ============================================================================

UIEngine* UIEngineWindow::getEngine() const
{
  if (FOG_IS_NULL(_d))
    return NULL;

  return _d->_engine;
}

void* UIEngineWindow::getHandle() const
{
  if (FOG_IS_NULL(_d))
    return NULL;

  return _d->_handle;
}

bool UIEngineWindow::isCreated() const
{
  if (FOG_IS_NULL(_d))
    return false;

  return _d->_handle != NULL;
}

uint32_t UIEngineWindow::getWindowDepth() const
{
  if (FOG_IS_NULL(_d))
    return 0;

  return _d->_windowDepth;
}

uint32_t UIEngineWindow::getWindowScreen() const
{
  if (FOG_IS_NULL(_d))
    return 0;

  return _d->_windowScreen;
}

// ============================================================================
// [Fog::UIEngineWindow - Dirty]
// ============================================================================

bool UIEngineWindow::isDirty() const
{
  if (FOG_IS_NULL(_d))
    return false;

  return _d->_isDirty;
}
  
// ============================================================================
// [Fog::UIEngineWindow - Enabled / Disabled]
// ============================================================================

bool UIEngineWindow::isEnabled() const
{
  if (FOG_IS_NULL(_d))
    return false;

  return _d->_isEnabled;
}

err_t UIEngineWindow::setEnabled(bool enabled)
{
  if (FOG_IS_NULL(_d))
    return ERR_RT_INVALID_STATE;

  return _d->setEnabled(enabled);
}

// ============================================================================
// [Fog::UIEngineWindow - Focus]
// ============================================================================

bool UIEngineWindow::hasFocus() const
{
  if (FOG_IS_NULL(_d))
    return false;

  return _d->_hasFocus;
}

err_t UIEngineWindow::focus()
{
  if (FOG_IS_NULL(_d))
    return ERR_RT_INVALID_STATE;

  return _d->focus();
}

// ============================================================================
// [Fog::UIEngineWindow - Window State]
// ============================================================================

bool UIEngineWindow::isVisible() const
{
  if (FOG_IS_NULL(_d))
    return false;

  return _d->_isVisible;
}

uint32_t UIEngineWindow::getState() const
{
  if (FOG_IS_NULL(_d))
    return WINDOW_STATE_NORMAL;

  return _d->_windowState;
}

err_t UIEngineWindow::setState(uint32_t state)
{
  if (FOG_IS_NULL(_d))
    return ERR_RT_INVALID_STATE;

  return _d->setState(state);
}

// ============================================================================
// [Fog::UIEngineWindow - Window Geometry]
// ============================================================================

RectI UIEngineWindow::getWindowGeometry() const
{
  if (FOG_IS_NULL(_d))
    return RectI(0, 0, 0, 0);

  return _d->_windowGeometry;
}

RectI UIEngineWindow::getClientGeometry() const
{
  if (FOG_IS_NULL(_d))
    return RectI(0, 0, 0, 0);

  return _d->_clientGeometry;
}

SizeI UIEngineWindow::getSizeGranularity() const
{
  if (FOG_IS_NULL(_d))
    return SizeI(1, 1);

  return _d->_sizeGranularity;
}

err_t UIEngineWindow::setWindowPosition(const PointI& pos)
{
  if (FOG_IS_NULL(_d))
    return ERR_RT_INVALID_STATE;

  return _d->setWindowPosition(pos);
}

err_t UIEngineWindow::setWindowSize(const SizeI& size)
{
  if (FOG_IS_NULL(_d))
    return ERR_RT_INVALID_STATE;

  return _d->setWindowSize(size);
}

// ============================================================================
// [Fog::UIEngineWindow - Window Stack]
// ============================================================================

err_t UIEngineWindow::moveToTop(UIEngineWindow* w)
{
  if (FOG_IS_NULL(_d))
    return ERR_RT_INVALID_STATE;

  void* handle = NULL;
  if (w != NULL)
    handle = w->getHandle();
  return _d->moveToTop(handle);
}

err_t UIEngineWindow::moveToBottom(UIEngineWindow* w)
{
  if (FOG_IS_NULL(_d))
    return ERR_RT_INVALID_STATE;

  void* handle = NULL;
  if (w != NULL)
    handle = w->getHandle();
  return _d->moveToBottom(handle);
}

// ============================================================================
// [Fog::UIEngineWindow - Window Coordinates]
// ============================================================================

err_t UIEngineWindow::worldToClient(PointI& pt) const
{
  if (FOG_IS_NULL(_d))
    return ERR_RT_INVALID_STATE;

  return _d->worldToClient(pt);
}

err_t UIEngineWindow::clientToWorld(PointI& pt) const
{
  if (FOG_IS_NULL(_d))
    return ERR_RT_INVALID_STATE;

  return _d->clientToWorld(pt);
}

// ============================================================================
// [Fog::UIEngineWindow - Opacity]
// ============================================================================

bool UIEngineWindow::isWindowOpaque() const
{
  if (FOG_IS_NULL(_d))
    return true;

  return _d->_isWindowOpaque;
}

float UIEngineWindow::getWindowOpacity() const
{
  if (FOG_IS_NULL(_d))
    return 1.0f;

  return _d->_windowOpacity;
}

// ============================================================================
// [Fog::UIEngineWindow - Window Title]
// ============================================================================

StringW UIEngineWindow::getWindowTitle() const
{
  if (FOG_IS_NULL(_d))
    return StringW();

  return _d->_windowTitle;
}

err_t UIEngineWindow::setWindowTitle(const StringW& title)
{
  if (FOG_IS_NULL(_d))
    return ERR_RT_INVALID_STATE;

  return _d->setWindowTitle(title);
}

// ============================================================================
// [Fog::UIEngineWindow - Window Double-Buffer]
// ============================================================================

bool UIEngineWindow::isBufferCacheEnabled() const
{
  if (FOG_IS_NULL(_d))
    return false;

  return _d->_isBufferCacheEnabled != 0;
}

// ============================================================================
// [Fog::UIEngineWindow - Events]
// ============================================================================
 
void UIEngineWindow::onEngineEvent(UIEngineEvent* ev)
{
  // Nothing to do here.
}

} // Fog namespace
