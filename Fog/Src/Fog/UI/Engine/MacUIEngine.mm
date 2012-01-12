// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/Core/Kernel/Application.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/UI/Engine/MacUIEngine.h>
#include <Fog/UI/Engine/MacUIEngineWindow.h>
#include <Fog/UI/Engine/MacUIEventLoopImpl_p.h>

// [Dependencies - Mac]
#import <Foundation/Foundation.h>

#if defined(FOG_OS_IOS)
# import <UIKit/UIKit.h>
#else
# import <AppKit/AppKit.h>
# import <Cocoa/Cocoa.h>
#endif // FOG_OS_IOS

FOG_IMPLEMENT_OBJECT(Fog::MacUIEngine)

namespace Fog {

// ============================================================================
// [Fog::MacUIEngine - Helpers]
// ============================================================================

static int MacUIEngine_getMouseWheelLines()
{
  return UI_ENGINE_MISC_DEFAULT_WHEEL_LINES;
}

// ============================================================================
// [Fog::MacUIEngine - Construction / Destruction]
// ============================================================================

MacUIEngine::MacUIEngine()
{
  _nsAutoReleasePool = [[NSAutoreleasePool alloc] init];

  // In case that NSApp exists (somewhere else created the instance) we just
  // use it. In such case we increase the reference count of NSApp so we can
  // safely call [NSApp release] in the destructor.
  if (NSApp == nil)
  {
    [NSApplication sharedApplication];
    [NSApp finishLaunching];
  }
  else
  {
    [NSApp retain];
  }

  initDisplay();
  initKeyboard();
  initMouse();

  _eventLoop.adopt(fog_new MacUIEventLoopImpl());
  _isInitialized = true;
}

MacUIEngine::~MacUIEngine()
{
  [NSApp release];
  [_nsAutoReleasePool drain];
}

// ============================================================================
// [Fog::MacUIEngine - Init]
// ============================================================================

void MacUIEngine::initDisplay()
{
  updateDisplayInfo();
}

void MacUIEngine::initKeyboard()
{
  updateKeyboardInfo();
}

void MacUIEngine::initMouse()
{
  updateMouseInfo();
}

// ============================================================================
// [Fog::MacUIEngine - Display / Palette]
// ============================================================================

void MacUIEngine::updateDisplayInfo()
{
}

// ============================================================================
// [Fog::MacUIEngine - Keyboard / Mouse]
// ============================================================================

void MacUIEngine::updateKeyboardInfo()
{
}

void MacUIEngine::updateMouseInfo()
{
}

void MacUIEngine::setMouseWheelLines(uint32_t lines)
{
  if (lines == 0)
    lines = MacUIEngine_getMouseWheelLines();

  base::setMouseWheelLines(lines);
}

// ============================================================================
// [Fog::MacUIEngine - DoUpdate]
// ============================================================================

void MacUIEngine::doBlitWindow(UIEngineWindow* window)
{
  MacUIEngineWindowImpl* d = reinterpret_cast<MacUIEngineWindowImpl*>(window->_d);
  d->blit();
}

// ============================================================================
// [Fog::MacUIEngine - Window Management]
// ============================================================================

err_t MacUIEngine::createWindow(UIEngineWindow* window, uint32_t flags)
{
  MacUIEngineWindowImpl* wImpl = fog_new MacUIEngineWindowImpl(this, window);
  if (FOG_IS_NULL(wImpl))
    return ERR_RT_OUT_OF_MEMORY;

  err_t err = wImpl->create(flags);
  if (FOG_IS_ERROR(err))
    fog_delete(wImpl);

  return err;
}

err_t MacUIEngine::destroyWindow(UIEngineWindow* window)
{
  UIEngineWindowImpl* wImpl = window->_d;
  if (FOG_IS_NULL(wImpl))
    return ERR_RT_INVALID_STATE;

  fog_delete(wImpl);
  return ERR_OK;
}

} // Fog namespace
