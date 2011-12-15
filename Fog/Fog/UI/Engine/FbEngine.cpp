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
#include <Fog/UI/Engine/FbEngine.h>
#include <Fog/UI/Engine/FbWindow.h>

FOG_IMPLEMENT_OBJECT(Fog::FbEngine)

namespace Fog {

// ============================================================================
// [Fog::FbUpdateTask]
// ============================================================================

struct FOG_NO_EXPORT FbUpdateTask : public CancelableTask
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FbUpdateTask(FbEngine* engine);

  // --------------------------------------------------------------------------
  // [Interface]
  // --------------------------------------------------------------------------

  virtual void run();
  virtual void cancel();
  
  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  FbEngine* _engine;

  bool _scheduled;
};

FbUpdateTask::FbUpdateTask(FbEngine* engine) : 
  _engine(engine),
  _scheduled(false)
{
  _destroyOnFinish = false;
}

void FbUpdateTask::run()
{
  if (!_scheduled)
    return;
  _scheduled = false;

  // Run doUpdateAll() which is the only way to make FbWindow content rendered
  // and synchronized with the screen.
  _engine->doUpdateAll();
}

void FbUpdateTask::cancel()
{
  _scheduled = false;
}

// ============================================================================
// [Fog::FbEngine - Construction / Destruction]
// ============================================================================

FbEngine::FbEngine() :
  _isInitialized(false),
  _updateTask(fog_new FbUpdateTask(this)),
  _updateInProgress(false)
{
}

FbEngine::~FbEngine()
{
  FbUpdateTask* task = reinterpret_cast<FbUpdateTask*>(_updateTask);

  if (task != NULL)
  {
    if (task->_scheduled)
    {
      task->cancel();
      task->setDestroyOnFinish(true);
    }
    else
    {
      fog_delete(task);
    }
  }
}

// ============================================================================
// [Fog::FbEngine - Handle <-> FbWindow]
// ============================================================================
  
err_t FbEngine::addHandle(void* handle, FbWindow* window)
{
  return _windowMap.put(handle, window, true);
}

err_t FbEngine::removeHandle(void* handle)
{
  return _windowMap.remove(handle);
}

FbWindow* FbEngine::getWindowByHandle(void* handle) const
{
  return _windowMap.get(handle, NULL);
}

// ============================================================================
// [Fog::FbEngine - Display / Palette]
// ============================================================================

const FbDisplayInfo* FbEngine::getDisplayInfo() const { return &_displayInfo; }
const FbPaletteInfo* FbEngine::getPaletteInfo() const { return &_paletteInfo; }

// ============================================================================
// [Fog::FbEngine - Caret]
// ============================================================================

const FbCaretState* FbEngine::getCaretState() const { return &_caretState; }

// ============================================================================
// [Fog::FbEngine - Keyboard / Mouse]
// ============================================================================

const FbKeyboardInfo* FbEngine::getKeyboardInfo() const { return &_keyboardInfo; }
const FbMouseInfo* FbEngine::getMouseInfo() const { return &_mouseInfo; }

const FbKeyboardState* FbEngine::getKeyboardState(uint32_t id) const
{
  if (id >= _keyboardInfo.getDevicesCount())
    return NULL;

  return _keyboardState[id];
}

const FbMouseState* FbEngine::getMouseState(uint32_t id) const
{
  if (id >= _mouseInfo.getDevicesCount())
    return NULL;

  return _mouseState[id];
}

uint32_t FbEngine::getModifierFromKey(uint32_t key) const
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

void FbEngine::setMouseWheelLines(uint32_t lines)
{
  if (lines == 0)
  {
    // This method should be overridden in all cases, this is only very safe
    // value used as default in Windows, and maybe Linux and MAC. If we get
    // here then the FbEngine is probably not at production stage.
    lines = 3;
  }

  _mouseInfo.setWheelLines(lines);
}

// ============================================================================
// [Fog::FbEngine - ScheduleUpdate / DoUpdate]
// ============================================================================

void FbEngine::scheduleUpdate()
{
  FbUpdateTask* task = reinterpret_cast<FbUpdateTask*>(_updateTask);

  if (!task->_scheduled)
  {
    task->_scheduled = true;
    Application::get()->getEventLoop()->postTask(task);
  }
}

void FbEngine::doUpdateAll()
{
  if (_dirtyList.isEmpty())
    return;

  // --------------------------------------------------------------------------
  // [Prepare]
  // --------------------------------------------------------------------------

  // Iterate over all dirty GuiWindow's and clear the list.
  List<FbWindow*> dirty;
  swap(dirty, _dirtyList);

  _updateInProgress = true;

  Painter painter;
  int painterIsUsed = 0;

  // --------------------------------------------------------------------------
  // [Paint]
  // --------------------------------------------------------------------------

  ListIterator<FbWindow*> it(dirty);

  for (it.start(); it.isValid(); it.next())
  {
    FbWindow* window = it.getItem();
    if (window == NULL)
      continue;

    FbWindowData* d = window->_d;
    if (!d->shouldUpdate)
      continue;

    // Clear dirty and update flags.
    d->isDirty = false;
    d->shouldUpdate = false;

    // Don't update window that is not visible, it's waste of resources. We 
    // will update it if it gets visible later in case that the window is
    // displayed.
    if (!d->isVisible)
    {
      d->shouldBlit = false;
      continue;
    }

    // Ignore all FbWindow instances whose contain an empty region. It must
    // be always set at least to rectangle to tale effect (paint / blit).
    if (d->regionOfInterest.isEmpty())
    {
      d->shouldBlit = false;
      continue;
    }

    RectI boundingBox(d->regionOfInterest.getBoundingBox());
    err_t err = painter.begin(d->bufferData, boundingBox);
    
    // TODO: Enable after Painter::SwitchTo() is implemented.
    // if (!painterIsUsed)
    //   err = painter.begin(d->bufferData, boundingBox);
    // else
    //   err = painter.switchTo(d->bufferData, boundingBox);
    
    // TODO: Enable after Painter::clipToRegion() is implemented.

    if (err == ERR_OK)
    {
      doUpdateWindow(window, painter, boundingBox);
      painterIsUsed++;
    }
  }

  if (painterIsUsed)
    painter.end();

  // --------------------------------------------------------------------------
  // [Blit]
  // --------------------------------------------------------------------------

  for (it.start(); it.isValid(); it.next())
  {
    FbWindow* window = it.getItem();
    if (window == NULL)
      continue;

    FbWindowData* d = window->_d;
    if (!d->shouldBlit)
      continue;

    d->shouldBlit = false;
    doBlitWindow(window);
    d->regionOfInterest.clear();
  }

  // --------------------------------------------------------------------------
  // [Finished]
  // --------------------------------------------------------------------------

  // In case that there is nothing in _dirtyList we reuse the 'dirty' list we
  // acquired instead of abandoning it. We save one MemMgr::alloc() and one
  // MemMgr::free() call.
  if (_dirtyList.getLength() == 0)
  {
    dirty.clear();
    swap(dirty, _dirtyList);
  }

  _updateInProgress = false;
}

void FbEngine::doUpdateWindow(FbWindow* window, Painter& painter, const RectI& rect)
{
  FbWindowData* d = window->_d;

  // TODO:
}

} // Fog namespace


















































#if 0
void GuiEngine::dispatchEnabled(Widget* w, bool enabled)
{
  uint32_t state = w->getState();

  // Dispatch 'Enable'.
  if (enabled)
  {
    if (state == WIDGET_ENABLED) return;
    if (state == WIDGET_DISABLED_BY_PARENT && !w->hasGuiWindow()) return;

    w->_state = WIDGET_ENABLED;

    StateEvent e(EVENT_ENABLE);
    w->sendEvent(&e);

    FOG_WIDGET_TREE_ITERATOR(i1, w, true,
      // before traverse
      {
        // show only child that's hidden by parent
        if (child->getState() != WIDGET_DISABLED_BY_PARENT)
          FOG_WIDGET_TREE_ITERATOR_NEXT(i1);

        child->_state = WIDGET_ENABLED;
        child->sendEvent(&e);
      },
      // after traverse
      {
      }
    );

    w->update(WIDGET_UPDATE_ALL);
  }
  // Dispatch 'Disable'.
  else
  {
    uint32_t state = w->getState();
    if (state == WIDGET_DISABLED) return;

    w->_state = WIDGET_DISABLED;

    //StateEvent e(toState == Widget::Disabled
    //  ? EvDisable
    //  : EvDisableByParent);
    StateEvent e(EVENT_DISABLE);
    w->sendEvent(&e);

    if (state != WIDGET_ENABLED) return;

    e._code = EVENT_DISABLE_BY_PARENT;

    FOG_WIDGET_TREE_ITERATOR(i2, w, true,
      // before traverse
      {
        // Mark by 'DisableByParent' all childs that's visible.
        if (child->getState() != WIDGET_ENABLED)
        {
          FOG_WIDGET_TREE_ITERATOR_NEXT(i2);
        }
        else
        {
          child->_state = WIDGET_DISABLED_BY_PARENT;
          child->sendEvent(&e);
        }
      },
      // after traverse
      {
      }
    );

    w->update(WIDGET_UPDATE_ALL);
  }
}

void GuiEngine::dispatchVisibility(Widget* w, uint32_t visible)
{
  uint32_t visibility = w->getVisibility();
  if (visibility == visible) return;

  // Dispatch 'Show'.
  if (visible >= WIDGET_VISIBLE)
  {
    if (visibility == WIDGET_HIDDEN_BY_PARENT && !w->hasGuiWindow()) return;
    w->_visibility = visible;

    uint32_t code = EVENT_SHOW;
    if (visible == WIDGET_VISIBLE_FULLSCREEN) code = EVENT_SHOW_FULLSCREEN;
    else if (visible == WIDGET_VISIBLE_MAXIMIZED) code = EVENT_SHOW_MAXIMIZE;

    VisibilityEvent e(code);
    w->sendEvent(&e);

    if (visibility == WIDGET_HIDDEN)
    {
      if ((w->getWindowHints() & WINDOW_INLINE_POPUP) != 0 && w->getParentWidget())
      {
        Widget* parent = w->getParentWidget();
        GuiWindow* guiWindow = parent->getGuiWindow();
        if (guiWindow != NULL) guiWindow->showPopUp(w);
      }

      // Only needed if widget was hidden before this event.
      FOG_WIDGET_TREE_ITERATOR(i1, w, true,
        // before traverse
        {
        // Show only child that's hidden by parent.
        if (child->getVisibility() != WIDGET_HIDDEN_BY_PARENT) FOG_WIDGET_TREE_ITERATOR_NEXT(i1);

        child->_visibility = WIDGET_VISIBLE;
        w->sendEvent(&e);
        },
        // after traverse
        {}
      );
    }

    w->update(WIDGET_UPDATE_ALL);
  }
  // Dispatch 'Hidden'.
  else
  {
    w->_visibility = visible;
    uint32_t code = EVENT_HIDE;
    if (visible == WIDGET_VISIBLE_MINIMIZED)
    {
      code = EVENT_SHOW_MINIMIZE;
    }

    VisibilityEvent e(code);
    w->sendEvent(&e);

    if (visible == WIDGET_HIDDEN && visibility >= WIDGET_VISIBLE)
    {
      e._code = EVENT_HIDE_BY_PARENT;

      FOG_WIDGET_TREE_ITERATOR(i2, w, true,
        // before traverse
        {
          // mark 'HiddenByParent' all childs that's visible
          if (child->getVisibility() != WIDGET_VISIBLE)
          {
            FOG_WIDGET_TREE_ITERATOR_NEXT(i2);
          }
          else
          {
            child->_visibility = WIDGET_HIDDEN_BY_PARENT;
            child->sendEvent(&e);
          }
        },
        // after traverse
        {
        }
      );

      Widget* p = w->getParentWidget();
      if (p) p->update(WIDGET_UPDATE_ALL);
    }
  }

  if (visible == WIDGET_VISIBLE_RESTORE)
  {
    visible = WIDGET_VISIBLE;
  }
  w->_visibility = visible;
}

void GuiEngine::dispatchConfigure(Widget* w, const RectI& geometry, bool changedOrientation)
{
  if (w->hasGuiWindow() && w->getGuiWindow()->hasPopUp())
  {
    w->getGuiWindow()->closePopUps();
  }

  GeometryEvent e;
  e._widgetGeometry = geometry;
  e._clientGeometry.setRect(0, 0, geometry.w, geometry.h);

  w->calcClientGeometry(e._clientGeometry);
  if (!e._clientGeometry.isValid())
    e._clientGeometry.setRect(0, 0, geometry.w, geometry.h);

  uint32_t changedFlags = 0;

  if (w->_widgetGeometry.x != e._widgetGeometry.x || w->_widgetGeometry.y != e._widgetGeometry.y)
    changedFlags |= GeometryEvent::CHANGED_WIDGET_POSITION;

  if (w->_widgetGeometry.w != e._widgetGeometry.w || w->_widgetGeometry.h != e._widgetGeometry.h)
    changedFlags |= GeometryEvent::CHANGED_WIDGET_SIZE;

  if (w->_clientGeometry.x != e._clientGeometry.x || w->_clientGeometry.y != e._clientGeometry.y)
    changedFlags |= GeometryEvent::CHANGED_CLIENT_POSITION;

  if (w->_clientGeometry.w != e._clientGeometry.w || w->_clientGeometry.h != e._clientGeometry.h)
    changedFlags |= GeometryEvent::CHANGED_CLIENT_SIZE;

  if (changedOrientation)
    changedFlags |= GeometryEvent::CHANGED_ORIENTATION;

  if (changedFlags == 0) return;
  e._changedFlags = changedFlags;

  w->_widgetGeometry = e._widgetGeometry;
  w->_clientGeometry = e._clientGeometry;
  w->_updateHasNcArea();
  w->sendEvent(&e);

  Layout* layout = w->getLayout();

  // TODO: intelligent enum order so we can omit one check here!
  if (layout && layout->_activated && ((changedFlags & GeometryEvent::CHANGED_WIDGET_SIZE) ||
                                       (changedFlags & GeometryEvent::CHANGED_ORIENTATION)))
  {
    // TODO: Stefan, assertion failure, don't know why, disabled for now.
    // FOG_ASSERT(layout->_toplevel);
    layout->markAsDirty();
  }

  Widget* p = w->getParentWidget();

  if (p)
  {
    // Maximize and fullscreen should not be resized -> only visibile state.
    if (w->getVisibility() == WIDGET_VISIBLE) p->update(WIDGET_UPDATE_ALL);
  }
  else if (changedFlags & GeometryEvent::CHANGED_WIDGET_SIZE)
  {
    w->update(WIDGET_UPDATE_ALL);
  }
}
#endif
