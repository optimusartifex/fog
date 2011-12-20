// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_UI_ENGINE_UIENGINE_H
#define _FOG_UI_ENGINE_UIENGINE_H

// [Dependencies]
#include <Fog/Core/Kernel/Object.h>
#include <Fog/Core/Kernel/Timer.h>
#include <Fog/Core/OS/Library.h>
#include <Fog/Core/Tools/List.h>
#include <Fog/Core/Tools/Hash.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/Time.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Geometry/Size.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/ImageBits.h>
#include <Fog/G2d/Imaging/ImageConverter.h>
#include <Fog/G2d/Source/Color.h>
#include <Fog/UI/Engine/UIEngineDisplayInfo.h>
#include <Fog/UI/Engine/UIEngineKeyboardInfo.h>
#include <Fog/UI/Engine/UIEngineKeyboardState.h>
#include <Fog/UI/Engine/UIEngineMouseInfo.h>
#include <Fog/UI/Engine/UIEngineMouseState.h>
#include <Fog/UI/Engine/UIEnginePaletteInfo.h>

namespace Fog {

//! @addtogroup Fog_UI_Engine
//! @{

// ============================================================================
// [Fog::UIEngine]
// ============================================================================

//! @brief Frame-buffer engine (abstract).
struct FOG_API UIEngine : public Object
{
  FOG_DECLARE_OBJECT(UIEngine, Object)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  UIEngine();
  virtual ~UIEngine();

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE Lock& getLock() { return _lock; }

  // --------------------------------------------------------------------------
  // [Handle <-> UIEngineWindow]
  // --------------------------------------------------------------------------
  
  // UIEngine contains the default Handle<->UIEngineWindow implementation, but it's
  // to override it in case that the native engine has something better than
  // Hash<> table.

  //! @brief Add a window @a handle and UIEngineWindow @a w to the mapping.
  virtual err_t addHandle(void* handle, UIEngineWindowImpl* wImpl);
  //! @brief Remove a window @a handle from the mapping.
  virtual err_t removeHandle(void* handle);

  //! @brief Translate window @a handle to @ref UIEngineWindow.
  virtual UIEngineWindowImpl* getWindowByHandle(void* handle) const;

  // --------------------------------------------------------------------------
  // [Display / Palette]
  // --------------------------------------------------------------------------

  //! @brief Get information about display.
  virtual const UIEngineDisplayInfo* getDisplayInfo() const;
  
  //! @brief Get information about display palette (only useful for 8-bit depth).
  virtual const UIEnginePaletteInfo* getPaletteInfo() const;

  //! @brief Update display information and palette.
  //!
  //! @note Must be reimplemented by the target UIEngine.
  virtual void updateDisplayInfo() = 0;

  // --------------------------------------------------------------------------
  // [Keyboard / Mouse]
  // --------------------------------------------------------------------------

  //! @brief Get keyboard info.
  virtual const UIEngineKeyboardInfo* getKeyboardInfo() const;
  //! @brief Get mouse info.
  virtual const UIEngineMouseInfo* getMouseInfo() const;

  //! @brief Update keyboard info.
  virtual void updateKeyboardInfo() = 0;
  //! @brief Update mouse info.
  virtual void updateMouseInfo() = 0;

  //! @brief Get keyboard state.
  virtual const UIEngineKeyboardState* getKeyboardState(uint32_t keyboardId) const;
  //! @brief Get mouse state.
  virtual const UIEngineMouseState* getMouseState(uint32_t mouseId) const;

  //! @brief Get modifier from @a key.
  virtual uint32_t getModifierFromKey(uint32_t key) const;

  //! @brief Set (override) the amount of lines which should be scrolled when
  //! mouse wheel is used.
  //!
  //! @param lines Amount of lines to scroll, zero means to get the value from
  //! the OS.
  virtual void setMouseWheelLines(uint32_t lines);

  // --------------------------------------------------------------------------
  // [Actions]
  // --------------------------------------------------------------------------

  // NOTE: These virtual functions contains base implementation which is used
  // by many UIEngine`s. The idea is to make the implementation as generic as
  // possible so it's easy to add support for a different UIEngine backends.
  //
  // These methods store state and sends UIEngineEvent`s to the related 
  // UIEngineWindow instances managed by the engine.

  //! @brief Do create action.
  virtual void doCreateAction(UIEngineWindow* window);

  //! @brief Do destroy action.
  virtual void doDestroyAction(UIEngineWindow* window);

  //! @brief Do close action.
  virtual void doCloseAction(UIEngineWindow* window);

  //! @brief Do enable / disable action.
  virtual void doEnableAction(UIEngineWindow* window,
    uint32_t eventCode);

  //! @brief Do state action.
  virtual void doStateAction(UIEngineWindow* window,
    uint32_t eventCode,
    uint32_t windowState);

  //! @brief Do focus action.
  virtual void doFocusAction(UIEngineWindow* window,
    uint32_t eventCode);

  //! @brief Do geometry action.
  virtual void doGeometryAction(UIEngineWindow* window,
    uint32_t eventCode,
    uint32_t orientation,
    const RectI& windowGeometry,
    const RectI& clientGeometry);

  //! @brief Do keyboard action, including keyboard action performed by touch
  //! key event.
  virtual uint32_t doKeyAction(UIEngineWindow* window,
    uint32_t eventCode,
    uint32_t keyboardId,
    uint32_t keyCode,
    uint32_t mod,
    uint32_t systemCode,
    uint32_t uc);

  //! @brief Do mouse action to point @a position and apply new @a buttonMask.
  virtual uint32_t doMouseAction(UIEngineWindow* window,
    uint32_t eventCode,
    uint32_t mouseId,
    const PointI& position,
    uint32_t buttonCode,
    uint32_t buttonMask);

  // --------------------------------------------------------------------------
  // [ScheduleUpdate]
  // --------------------------------------------------------------------------

  //! @brief Schedule update.
  //!
  //! Updating means to iterate over all registered @c UIEngineWindow instances and
  //! to call @c UIEngineWindow::update() in case that it's needed.
  //!
  //! @note All @c UIEngineWindow instances are updated together. Only one update and
  //! blit call is executed in case that more updates were scheduled.
  virtual void scheduleUpdate();

  //! @brief Schedule update for @a window and adds @a rect to regionOfIntersect.
  virtual void scheduleWindowUpdate(UIEngineWindow* window, const RectI& rect);

  //! @brief Schedule update for @a window and adds @a region to regionOfIntersect.
  virtual void scheduleWindowUpdate(UIEngineWindow* window, const Region& region);

  // --------------------------------------------------------------------------
  // [DoUpdate]
  // --------------------------------------------------------------------------

  //! @brief Do update.
  //!
  //! @note Used internally! Do not call, always use @c scheduleUpdate() if you
  //! plan to call @c doUpdateAll().
  virtual void doUpdateAll();

  //! @brief Do update of a single @a window. 
  //!
  //! @note Called by @c doUpdate(), never call manually.
  virtual void doUpdateWindow(UIEngineWindow* window);

  //! @brief Do painting of a single @a window. Always called after @ref 
  //! doUpdateWindow()
  //!
  //! @note Called by @c doUpdate(), never call manually.
  virtual void doPaintWindow(UIEngineWindow* window, Painter* painter, const RectI& paintRect);

  //! @brief Blits window content into screen. Called usually from @c doUpdateWindow().
  virtual void doBlitWindow(UIEngineWindow* window) = 0;

  // --------------------------------------------------------------------------
  // [Window Management]
  // --------------------------------------------------------------------------

  //! @brief Create a new @ref UIEngineWindowImpl instance and assign it to the 
  //! @a window.
  virtual err_t createWindow(UIEngineWindow* window, uint32_t flags) = 0;

  //! @brief Destroy an existing @ref UIEngineWindowImpl instance assigned with.
  //! the @a window.
  //!
  //! @note You can only use @c UIEngineWindow created by @c createWindow() method.
  virtual err_t destroyWindow(UIEngineWindow* window) = 0;

  //! @brief Clean all references to the @ref UIEngineWindow and
  //! @ref UIEngineWindowImpl instances.
  //!
  //! @note You can only use @c UIEngineWindow created by @c createWindow() method.
  virtual err_t cleanupWindow(UIEngineWindow* window);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Lock.
  Lock _lock;

  //! @brief Whether the engine was correctly initialized.
  uint32_t _isInitialized;

  //! @brief Library instance, non-null in case that the @c UIEngine was opened
  //! as a plugin (see @c X11UIEngine).
  //!
  //! @note This is mainly useful/used under Linux operating system, where more
  //! engines can be build and user can specify which one to use (or it can be
  //! also detected by the runtime).
  Library _library;

  //! @brief Frame-buffer ID <-> UIEngineWindow.
  //!
  //! @note Engine can create custom implementation of ID<->UIEngineWindow translation.
  //! However, this default implementation is currently used by all UIEngine
  //! implementations used by Fog-Framework.
  Hash<void*, UIEngineWindowImpl*> _windowMap;

  //! @brief List of dirty UIEngineWindow`s which need to be updated.
  //!
  //! First call to @c scheduleUpdate() should add UIEngineWindow into this list, all
  //! other calls should be cached (UIEngineWindow is added only once per update to
  //! the list).
  List<UIEngineWindow*> _dirtyList;

  //! @brief Display information.
  UIEngineDisplayInfo _displayInfo;
  //! @brief Display palette information.
  UIEnginePaletteInfo _paletteInfo;

  //! @brief Keyboard info.
  UIEngineKeyboardInfo _keyboardInfo;
  //! @brief Mouse information.
  UIEngineMouseInfo _mouseInfo;

  //! @brief Keyboard state.
  UIEngineKeyboardState _keyboardState[16];
  //! @brief Mouse state.
  UIEngineMouseState _mouseState[16];

  //! @brief Update task (1 instance per engine).
  void* _updateTask;

  //! @brief Update is currently in progress.
  bool _updateInProgress;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_UI_ENGINE_UIENGINE_H
