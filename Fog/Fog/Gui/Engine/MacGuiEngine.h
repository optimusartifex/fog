// [Fog-Gui]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_GUIENGINE_MACGUIENGINE_H
#define _FOG_GUI_GUIENGINE_MACGUIENGINE_H

// [Dependencies]
#include <Fog/Core/System/Object.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/Gui/Global/Constants.h>
#include <Fog/Gui/Engine/GuiEngine.h>
#include <Fog/Core/Mac/MacUtil.h>

#include <CoreFoundation/CoreFoundation.h>

#ifdef __OBJC__
@class NSEvent;
@class NSRunLoop;
@class FogView;          		// see MacGuiEngine.mm
@class FogWindow;        		// see MacGuiEngine.mm
#else
class NSEvent;
class NSRunLoop;
class FogView;
class FogWindow;
#endif

namespace Fog {

//! @addtogroup Fog_Gui_Engine
//! @{

// ============================================================================
// [Fog::MacGuiEngine]
// ============================================================================

struct FOG_API MacGuiEngine : public GuiEngine
{
  FOG_DECLARE_OBJECT(MacGuiEngine, GuiEngine)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  MacGuiEngine();
  virtual ~MacGuiEngine();

  // --------------------------------------------------------------------------
  // [Display]
  // --------------------------------------------------------------------------

  virtual void updateDisplayInfo();

  // --------------------------------------------------------------------------
  // [Update]
  // --------------------------------------------------------------------------

  virtual void doBlitWindow(GuiWindow* window, const BoxI* rects, sysuint_t count);

  // --------------------------------------------------------------------------
  // [GuiWindow]
  // --------------------------------------------------------------------------

  virtual GuiWindow* createGuiWindow(Widget* widget);
  virtual void destroyGuiWindow(GuiWindow* native);

  virtual void minimize(GuiWindow*);
  virtual void maximize(GuiWindow*);

  ScopedAutoreleasePool pool;
};

// ============================================================================
// [Fog::MacGuiWindow]
// ============================================================================

struct FOG_API MacGuiWindow : public GuiWindow
{
  FOG_DECLARE_OBJECT(MacGuiWindow, GuiWindow)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  MacGuiWindow(Widget* widget);
  virtual ~MacGuiWindow();

  // --------------------------------------------------------------------------
  // [Window Manipulation]
  // --------------------------------------------------------------------------

  virtual err_t create(uint32_t flags);
  virtual err_t destroy();

  virtual err_t enable();
  virtual err_t disable();

  virtual err_t show(uint32_t state);
  virtual err_t hide();

  virtual err_t setPosition(const PointI& pos);
  virtual err_t setSize(const SizeI& size);
  virtual err_t setGeometry(const RectI& geometry);

  virtual err_t takeFocus();

  virtual err_t setTitle(const String& title);
  virtual err_t getTitle(String& title);

  virtual err_t setIcon(const Image& icon);
  virtual err_t getIcon(Image& icon);

  virtual err_t setSizeGranularity(const PointI& pt);
  virtual err_t getSizeGranularity(PointI& pt);
  
  virtual err_t worldToClient(PointI* coords);
  virtual err_t clientToWorld(PointI* coords);
  
  virtual void setOwner(GuiWindow* owner);
  virtual void releaseOwner();

  // --------------------------------------------------------------------------
  // [Z-Order]
  // --------------------------------------------------------------------------

  //Move Window on Top of other Window! (If w == 0 Move on top of all Windows)
  virtual void moveToTop(GuiWindow* w=0);
  //Move Window behind other Window! (If w == 0 Move behind all Windows of screen)
  virtual void moveToBottom(GuiWindow* w=0);

  virtual void setTransparency(float val);
  
  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------
  FogWindow* window;
};

// ============================================================================
// [Fog::MacGuiBackBuffer]
// ============================================================================

//! @brief Provides cacheable buffer for widgets.
struct FOG_API MacGuiBackBuffer : public GuiBackBuffer
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  MacGuiBackBuffer();
  virtual ~MacGuiBackBuffer();

  // --------------------------------------------------------------------------
  // [Interface]
  // --------------------------------------------------------------------------

  virtual bool resize(int width, int height, bool cache);
  virtual void destroy();
  virtual void updateRects(const BoxI* rects, sysuint_t count);
  virtual void updateRects(FogView* view, const BoxI* rects, sysuint_t count);
};

// ============================================================================
// [Fog::MacEventLoop]
// ============================================================================

// @brief Using CFRunLoop
struct MacEventLoopBase : public EventLoop
{
  MacEventLoopBase();
  virtual ~MacEventLoopBase();

  virtual void quit() = 0;

protected:
  virtual void _runInternal() = 0;
  virtual void _scheduleWork();
  virtual void _scheduleDelayedWork(const Time& delayedWorkTime);

private:
  // Timer callback scheduled by _scheduleDelayedWork.  This does not do any
  // work, but it signals _delayedWorkSource so that delayed work can be
  // performed within the appropriate priority constraints.
  CFRunLoopTimerRef _delayedWorkTimer;
  static void runDelayedWorkTimer(CFRunLoopTimerRef timer, void* info);

  CFRunLoopSourceRef _workSource;
  FOG_INLINE static void runWorkSource(void* info) { static_cast<MacEventLoopBase*>(info)->runWork(); }
  bool runWork();

  CFRunLoopSourceRef _delayedWorkSource;
  FOG_INLINE static void runDelayedWorkSource(void* info) { static_cast<MacEventLoopBase*>(info)->runDelayedWork(); }
  bool runDelayedWork();

  CFRunLoopSourceRef _idleWorkSource;
  FOG_INLINE static void runIdleWorkSource(void* info) { static_cast<MacEventLoopBase*>(info)->runIdleWork(); }
  bool runIdleWork();

  // The thread's run loop.
  CFRunLoopRef _runLoop;

  // The time that _delayedWorkTimer is scheduled to fire.  This is tracked
  // independently of CFRunLoopTimerGetNextFireDate(_delayedWorkTimer)
  // to be able to reset the timer properly after waking from system sleep.
  // See PowerStateNotification.
  CFAbsoluteTime _delayedWorkFireTime;

  Time _delayedWorkTime;
};

// @brief Using NSRunLoop
struct FOG_API MacNonMainEventLoop : public MacEventLoopBase
{
  virtual void quit();
  virtual void _runInternal();
};

// @brief Using NSApplication
struct FOG_API MacMainEventLoop : public MacEventLoopBase
{
  virtual void quit();
  virtual void _runInternal();
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GUI_GUIENGINE_MACGUIENGINE_H
