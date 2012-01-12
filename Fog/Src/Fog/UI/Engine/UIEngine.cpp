// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Kernel/Application.h>
#include <Fog/Core/Kernel/EventLoop.h>
#include <Fog/Core/Kernel/Task.h>
#include <Fog/Core/Tools/Swap.h>
#include <Fog/G2d/Painting/Painter.h>
#include <Fog/UI/Engine/UIEngine.h>
#include <Fog/UI/Engine/UIEngineEvent.h>
#include <Fog/UI/Engine/UIEngineWindow.h>

FOG_IMPLEMENT_OBJECT(Fog::UIEngine)

namespace Fog {

// ============================================================================
// [Fog::UIEngineUpdateTask]
// ============================================================================

struct FOG_NO_EXPORT UIEngineUpdateTask : public CancelableTask
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  UIEngineUpdateTask(UIEngine* engine);

  // --------------------------------------------------------------------------
  // [Interface]
  // --------------------------------------------------------------------------

  virtual void run();
  virtual void cancel();
  
  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  UIEngine* _engine;
  bool _scheduled;
};

UIEngineUpdateTask::UIEngineUpdateTask(UIEngine* engine) : 
  _engine(engine),
  _scheduled(false)
{
  _destroyOnFinish = false;
}

void UIEngineUpdateTask::run()
{
  if (!_scheduled)
    return;

  // We need to clear this flag here, becuase doUpdateAll() can schedule again.
  _scheduled = false;

  // Run doUpdateAll() which is the only way to make UIEngineWindow content
  // rendered and synchronized with the screen.
  _engine->doUpdateAll();
}

void UIEngineUpdateTask::cancel()
{
  _scheduled = false;
}

// ============================================================================
// [Fog::UIEngine - Construction / Destruction]
// ============================================================================

UIEngine::UIEngine() :
  _isInitialized(false),
  _updateTask(NULL),
  _updateInProgress(false)
{
  _updateTask = fog_new UIEngineUpdateTask(this);
}

UIEngine::~UIEngine()
{
  UIEngineUpdateTask* task = reinterpret_cast<UIEngineUpdateTask*>(_updateTask);

  if (task != NULL)
  {
    if (task->_scheduled)
    {
      task->cancel();
      task->setDestroyOnFinish(true);
    }
    else
    {
      // We are safe to delete the task now, it's not scheduled so it's not
      // in the EventLoop stack.
      fog_delete(task);
    }
  }
}

// ============================================================================
// [Fog::UIEngine - Handle <-> UIEngineWindow]
// ============================================================================

err_t UIEngine::addHandle(void* handle, UIEngineWindowImpl* wImpl)
{
  return _windowMap.put(handle, wImpl, true);
}

err_t UIEngine::removeHandle(void* handle)
{
  return _windowMap.remove(handle);
}

UIEngineWindowImpl* UIEngine::getWindowByHandle(void* handle) const
{
  return _windowMap.get(handle, NULL);
}

// ============================================================================
// [Fog::UIEngine - Display / Palette]
// ============================================================================

const UIEngineDisplayInfo* UIEngine::getDisplayInfo() const { return &_displayInfo; }
const UIEnginePaletteInfo* UIEngine::getPaletteInfo() const { return &_paletteInfo; }

// ============================================================================
// [Fog::UIEngine - Keyboard / Mouse]
// ============================================================================

const UIEngineKeyboardInfo* UIEngine::getKeyboardInfo() const { return &_keyboardInfo; }
const UIEngineMouseInfo* UIEngine::getMouseInfo() const { return &_mouseInfo; }

const UIEngineKeyboardState* UIEngine::getKeyboardState(uint32_t keyboardId) const
{
  if (keyboardId >= _keyboardInfo.getDevicesCount())
    return NULL;

  return &_keyboardState[keyboardId];
}

const UIEngineMouseState* UIEngine::getMouseState(uint32_t mouseId) const
{
  if (mouseId >= _mouseInfo.getDevicesCount())
    return NULL;

  return &_mouseState[mouseId];
}

uint32_t UIEngine::getModifierFromKey(uint32_t key) const
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

void UIEngine::setMouseWheelLines(uint32_t lines)
{
  if (lines == 0)
  {
    // This method should be overridden in all cases, this is only very safe
    // value used as default in Windows, and maybe Linux and MAC. If we get
    // here then the UIEngine is probably not at production stage.
    lines = UI_ENGINE_MISC_DEFAULT_WHEEL_LINES;
  }

  _mouseInfo.setWheelLines(lines);
}

// ============================================================================
// [Fog::UIEngine - Actions]
// ============================================================================

void UIEngine::doCreateAction(UIEngineWindow* window)
{
  UIEngineCreateEvent event(UI_ENGINE_EVENT_CREATE, UI_ENGINE_EVENT_FLAG_SYSTEM);
  window->onEngineEvent(&event);
}

void UIEngine::doDestroyAction(UIEngineWindow* window)
{
  UIEngineDestroyEvent event(UI_ENGINE_EVENT_DESTROY, UI_ENGINE_EVENT_FLAG_SYSTEM);
  window->onEngineEvent(&event);
}

void UIEngine::doCloseAction(UIEngineWindow* window)
{
  UIEngineCloseEvent event(UI_ENGINE_EVENT_CLOSE, UI_ENGINE_EVENT_FLAG_SYSTEM);
  window->onEngineEvent(&event);
}

void UIEngine::doEnableAction(UIEngineWindow* window,
  uint32_t eventCode)
{
  UIEngineEvent engineEvent(eventCode, UI_ENGINE_EVENT_FLAG_SYSTEM);
  window->onEngineEvent(&engineEvent);
}

void UIEngine::doStateAction(UIEngineWindow* window,
  uint32_t eventCode,
  uint32_t windowState)
{
  UIEngineWindowImpl* d = window->_d;
  UIEngineStateEvent engineEvent(eventCode, UI_ENGINE_EVENT_FLAG_SYSTEM);

  engineEvent._oldState = d->_windowState;

  // TODO: This should been already set by X11UIEngine, what about others?
  d->_isVisible = (eventCode == UI_ENGINE_EVENT_SHOW);
  d->_windowState = windowState;

  window->onEngineEvent(&engineEvent);

  if (eventCode == UI_ENGINE_EVENT_SHOW)
    scheduleWindowUpdate(window, RectI(0, 0, d->_clientGeometry.w, d->_clientGeometry.h));
}

void UIEngine::doFocusAction(UIEngineWindow* window,
  uint32_t eventCode)
{
  UIEngineWindowImpl* d = window->_d;
  UIEngineFocusEvent engineEvent(eventCode, UI_ENGINE_EVENT_FLAG_SYSTEM);

  switch (eventCode)
  {
    case UI_ENGINE_EVENT_FOCUS_IN:
      d->_hasFocus = true;
      break;
    case UI_ENGINE_EVENT_FOCUS_OUT:
      d->_hasFocus = false;
      break;
    default:
      FOG_ASSERT_NOT_REACHED();
  }

  window->onEngineEvent(&engineEvent);
}

void UIEngine::doGeometryAction(UIEngineWindow* window,
  uint32_t eventCode,
  uint32_t orientation,
  const RectI& windowGeometry,
  const RectI& clientGeometry)
{
  UIEngineWindowImpl* d = window->_d;
  uint32_t geometryFlags = NO_FLAGS;

  const uint32_t updateFlags = 
    UI_ENGINE_CHANGED_WINDOW_SIZE |
    UI_ENGINE_CHANGED_CLIENT_SIZE |
    UI_ENGINE_CHANGED_ORIENTATION ;

  if (windowGeometry.x != d->_windowGeometry.x || windowGeometry.y != d->_windowGeometry.y)
    geometryFlags |= UI_ENGINE_CHANGED_WINDOW_POSITION;
  if (windowGeometry.w != d->_windowGeometry.w || windowGeometry.h != d->_windowGeometry.h)
    geometryFlags |= UI_ENGINE_CHANGED_WINDOW_SIZE;
  if (clientGeometry.x != d->_clientGeometry.x || clientGeometry.y != d->_clientGeometry.y)
    geometryFlags |= UI_ENGINE_CHANGED_CLIENT_POSITION;
  if (clientGeometry.w != d->_clientGeometry.w || clientGeometry.h != d->_clientGeometry.h)
    geometryFlags |= UI_ENGINE_CHANGED_CLIENT_SIZE;
  if (orientation != d->_orientation)
    geometryFlags |= UI_ENGINE_CHANGED_ORIENTATION;

  // Ignore case that the geometry is not changed.
  if (geometryFlags == NO_FLAGS)
    return;

  UIEngineGeometryEvent engineEvent(eventCode, UI_ENGINE_EVENT_FLAG_SYSTEM);

  engineEvent._oldOrientation = d->_orientation;
  engineEvent._oldWindowGeometry = d->_windowGeometry;
  engineEvent._oldClientGeometry = d->_clientGeometry;

  engineEvent._orientation = orientation;
  engineEvent._windowGeometry = windowGeometry;
  engineEvent._clientGeometry = clientGeometry;
  engineEvent._geometryFlags = geometryFlags;

  d->_orientation = orientation;
  d->_windowGeometry = windowGeometry;
  d->_clientGeometry = clientGeometry;
  window->onEngineEvent(&engineEvent);

  if ((geometryFlags & updateFlags) != 0)
    scheduleWindowUpdate(window, RectI(0, 0, clientGeometry.w, clientGeometry.h));
}

uint32_t UIEngine::doKeyAction(UIEngineWindow* window,
  uint32_t eventCode,
  uint32_t keyboardId,
  uint32_t keyCode,
  uint32_t modMask,
  uint32_t systemCode,
  uint32_t uc)
{
  UIEngineWindowImpl* d = window->_d;

  return NO_FLAGS;
}

uint32_t UIEngine::doMouseAction(UIEngineWindow* window,
  uint32_t eventCode,
  uint32_t mouseId,
  const PointI& position,
  uint32_t buttonCode,
  uint32_t buttonMask)
{
  UIEngineWindowImpl* d = window->_d;
  UIEngineMouseEvent engineEvent(eventCode, UI_ENGINE_EVENT_FLAG_SYSTEM);

  engineEvent._button = buttonCode;
  engineEvent._buttonMask = buttonMask;
  engineEvent._position = position;
  // engineEvent._offset = ;

  window->onEngineEvent(&engineEvent);
  return engineEvent._flags;
}

// ============================================================================
// [Fog::UIEngine - ScheduleUpdate / DoUpdate]
// ============================================================================

void UIEngine::scheduleUpdate()
{
  UIEngineUpdateTask* task = reinterpret_cast<UIEngineUpdateTask*>(_updateTask);

  if (!task->_scheduled)
  {
    task->_scheduled = true;
    Application::get()->getHomeThread()->getEventLoop().postTask(task);
  }
}

void UIEngine::scheduleWindowUpdate(UIEngineWindow* window, const RectI& rect)
{
  UIEngineWindowImpl* d = window->_d;
  d->_paintRegion.union_(rect);

  if (!d->_isDirty)
  {
    d->_isDirty = true;
    d->_shouldUpdate = true;
    d->_shouldPaint = true;

    _dirtyList.append(window);
    scheduleUpdate();
  }
}

void UIEngine::scheduleWindowUpdate(UIEngineWindow* window, const Region& region)
{
  UIEngineWindowImpl* d = window->_d;
  d->_paintRegion.union_(region);

  if (!d->_isDirty)
  {
    d->_isDirty = true;
    d->_shouldUpdate = true;
    d->_shouldPaint = true;

    _dirtyList.append(window);
    scheduleUpdate();
  }
}

// ============================================================================
// [Fog::UIEngine - DoUpdate]
// ============================================================================

void UIEngine::doUpdateAll()
{
  if (_dirtyList.isEmpty())
    return;

  // --------------------------------------------------------------------------
  // [Prepare]
  // --------------------------------------------------------------------------

  _updateInProgress = true;

  // Iterate over all dirty UIEngineWindow's. We use a local copy of dirty windows
  // list and clear the original list, because updating can occasionally put
  // window into dirty state.
  List<UIEngineWindow*> dirty;
  swap(dirty, _dirtyList);

  Static<Painter> painter;
  int isPainterUsed = -1;

  ListIterator<UIEngineWindow*> it(dirty);

  // --------------------------------------------------------------------------
  // [Update]
  // --------------------------------------------------------------------------

  for (it.start(); it.isValid(); it.next())
  {
    UIEngineWindow* window = it.getItem();
    if (window == NULL)
      continue;

    UIEngineWindowImpl* d = window->_d;
    d->_isDirty = false;

    if (!d->_shouldUpdate)
      continue;
    d->_shouldUpdate = false;

    if (d->_bufferData._size.w != d->_clientGeometry.w ||
        d->_bufferData._size.h != d->_clientGeometry.h)
    {
      err_t err = d->resizeDoubleBuffer(SizeI(d->_clientGeometry.w, d->_clientGeometry.h));

      if (FOG_IS_ERROR(err))
        continue;
    }

    doUpdateWindow(window);
  }

  // --------------------------------------------------------------------------
  // [Paint]
  // --------------------------------------------------------------------------

  for (it.start(); it.isValid(); it.next())
  {
    UIEngineWindow* window = it.getItem();

    if (window == NULL)
      continue;

    UIEngineWindowImpl* d = window->_d;

    if (!d->_shouldPaint)
      continue;
    d->_shouldPaint = false;

    // Don't update window that is not visible, it's waste of resources. We 
    // will update it if it gets visible later in case that the window is
    // displayed.
    if (!d->_isVisible)
    {
      d->_shouldBlit = false;
      continue;
    }

    // Ignore all UIEngineWindow instances whose contain an empty region. It must
    // be always set at least to rectangle to be able to paint into the double
    // buffer.
    if (d->_paintRegion.isEmpty())
    {
      d->_shouldBlit = false;
      continue;
    }

    RectI boundingBox(d->_paintRegion.getBoundingBox());
    err_t err;
    
    if (isPainterUsed == -1)
    {
      painter.init();
      isPainterUsed++;
    }

    // TODO: Enable after Painter::SwitchTo() is implemented.
    // if (!isPainterUsed)
    //   err = painter.begin(d->_bufferData, boundingBox);
    // else
    //   err = painter.switchTo(d->_bufferData, boundingBox);
    err = painter->begin(d->_bufferData);
    
    // TODO: Enable after Painter::clipToRegion() is implemented.

    if (err == ERR_OK)
    {
      doPaintWindow(window, &painter, boundingBox);
      isPainterUsed++;
    }

    d->_blitRegion.union_(d->_paintRegion);
    d->_paintRegion.clear();
  }

  if (isPainterUsed >= 0)
    painter.destroy();

  // --------------------------------------------------------------------------
  // [Blit]
  // --------------------------------------------------------------------------

  for (it.start(); it.isValid(); it.next())
  {
    UIEngineWindow* window = it.getItem();
    if (window == NULL)
      continue;

    UIEngineWindowImpl* d = window->_d;
    if (!d->_shouldBlit)
      continue;

    d->_shouldBlit = false;
    doBlitWindow(window);
    d->_blitRegion.clear();
  }

  // --------------------------------------------------------------------------
  // [Finished]
  // --------------------------------------------------------------------------

  _updateInProgress = false;

  // In case that there is nothing in _dirtyList we reuse the 'dirty' list we
  // acquired instead of abandoning it. We save one MemMgr::alloc() and one
  // MemMgr::free() call (each one usually needs to lock a global mutex, because
  // of thread-safety).
  if (_dirtyList.getLength() == 0)
  {
    dirty.clear();
    swap(dirty, _dirtyList);
  }
}

void UIEngine::doUpdateWindow(UIEngineWindow* window)
{
  UIEngineWindowImpl* d = window->_d;

  UIEngineUpdateEvent engineEvent(UI_ENGINE_EVENT_UPDATE, UI_ENGINE_EVENT_FLAG_SYSTEM);
  window->onEngineEvent(&engineEvent);
}

void UIEngine::doPaintWindow(UIEngineWindow* window, Painter* painter, const RectI& paintRect)
{
  UIEngineWindowImpl* d = window->_d;
  UIEnginePaintEvent engineEvent(UI_ENGINE_EVENT_PAINT, UI_ENGINE_EVENT_FLAG_SYSTEM);

  engineEvent._painter = painter;
  engineEvent._paintRect = paintRect;

  window->onEngineEvent(&engineEvent);
}

// ============================================================================
// [Fog::UIEngine - Window Management]
// ============================================================================

err_t UIEngine::cleanupWindow(UIEngineWindow* window)
{
  // --------------------------------------------------------------------------
  // [DirtyList]
  // --------------------------------------------------------------------------

  ListIterator<UIEngineWindow*> it(_dirtyList);

  while (it.isValid())
  {
    if (it.getItem() == window)
    {
      // Only one reference can be stored in _dirtyList, so if we are here then
      // we are done.
      _dirtyList.removeAt(it.getIndex());
      break;
    }

    it.next();
  }

  return ERR_OK;
}

} // Fog namespace
