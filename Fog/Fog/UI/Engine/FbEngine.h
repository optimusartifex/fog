// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_UI_ENGINE_FBENGINE_H
#define _FOG_UI_ENGINE_FBENGINE_H

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
#include <Fog/UI/Engine/FbDisplayInfo.h>
#include <Fog/UI/Engine/FbKeyboardInfo.h>
#include <Fog/UI/Engine/FbKeyboardState.h>
#include <Fog/UI/Engine/FbMouseInfo.h>
#include <Fog/UI/Engine/FbMouseState.h>
#include <Fog/UI/Engine/FbPaletteInfo.h>

namespace Fog {

//! @addtogroup Fog_UI_Engine
//! @{

// ============================================================================
// [Fog::FbEngine]
// ============================================================================

//! @brief Frame-buffer engine (abstract).
struct FOG_API FbEngine : public Object
{
  FOG_DECLARE_OBJECT(FbEngine, Object)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FbEngine();
  virtual ~FbEngine();

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE Lock& getLock() { return _lock; }

  // --------------------------------------------------------------------------
  // [Handle <-> FbWindow]
  // --------------------------------------------------------------------------
  
  // FbEngine contains the default Handle<->FbWindow implementation, but it's
  // to override it in case that the native engine has something better than
  // Hash<> table.

  //! @brief Add a window @a handle and frame-buffer window @a w to the mapping.
  virtual err_t addHandle(void* handle, FbWindow* window);
  //! @brief Remove a window @a handle from the mapping.
  virtual err_t removeHandle(void* handle);

  //! @brief Translate window @a handle to @ref FbWindow.
  virtual FbWindow* getWindowByHandle(void* handle) const;

  // --------------------------------------------------------------------------
  // [Display / Palette]
  // --------------------------------------------------------------------------

  //! @brief Get information about display.
  virtual const FbDisplayInfo* getDisplayInfo() const;
  
  //! @brief Get information about display palette (only useful for 8-bit depth).
  virtual const FbPaletteInfo* getPaletteInfo() const;

  //! @brief Update display information and palette.
  //!
  //! @note Must be reimplemented by the target FbEngine.
  virtual void updateDisplayInfo() = 0;

  // --------------------------------------------------------------------------
  // [Keyboard / Mouse]
  // --------------------------------------------------------------------------

  //! @brief Get keyboard info.
  virtual const FbKeyboardInfo* getKeyboardInfo() const;
  //! @brief Get mouse info.
  virtual const FbMouseInfo* getMouseInfo() const;

  //! @brief Get keyboard state.
  virtual const FbKeyboardState* getKeyboardState(uint32_t keyboardId) const;
  //! @brief Get mouse state.
  virtual const FbMouseState* getMouseState(uint32_t mouseId) const;

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
  // by many FbEngine`s. The idea is to make the implementation as generic as
  // possible so it's easy to add support for a different FbEngine backends.
  //
  // These methods store state and sends FbEvent`s to the related FbWindow
  // instances managed by the engine.

  virtual void doCreateAction(FbWindow* window);
  virtual void doDestroyAction(FbWindow* window);

  //! @brief Do enable / disable action.
  virtual void doStateAction(FbWindow* window,
    uint32_t eventCode);

  //! @brief Show or hide action.
  virtual void doVisibilityAction(FbWindow* window,
    uint32_t eventCode);

  //! @brief Do focus action.
  virtual void doFocusAction(FbWindow* window,
    uint32_t eventCode);

  //! @brief Do geometry action.
  virtual void doGeometryAction(FbWindow* window,
    uint32_t eventCode,
    const RectI& windowGeometry,
    const RectI& clientGeometry,
    uint32_t orientation);

  //! @brief Do mouse action to point @a position and apply new @a buttonMask.
  virtual void doMouseAction(FbWindow* window,
    uint32_t eventCode,
    uint32_t mouseId,
    const PointI& position,
    uint32_t buttonMask);

  //! @brief Do keyboard action, including keyboard action performed by touch
  //! key event.
  virtual void doKeyAction(FbWindow* window,
    uint32_t eventCode,
    uint32_t keyboardId,
    uint32_t keyCode,
    uint32_t mod,
    uint32_t systemCode,
    uint32_t uc);

  // --------------------------------------------------------------------------
  // [ScheduleUpdate / DoUpdate]
  // --------------------------------------------------------------------------

  //! @brief Schedule update.
  //!
  //! Updating means to iterate over all registered @c FbWindow instances and
  //! to call @c FbWindow::update() in case that it's needed.
  //!
  //! @note All @c FbWindow instances are updated together. Only one update and
  //! blit call is executed in case that more updates were scheduled.
  virtual void scheduleUpdate();

  //! @brief Do update.
  //!
  //! @note Used internally! Do not call, always use @c scheduleUpdate() if you
  //! plan to call @c doUpdateAll().
  virtual void doUpdateAll();

  //! @brief Do update of a single @a window. 
  //!
  //! @note Called by @c doUpdate(), never call manually.
  virtual void doUpdateWindow(FbWindow* window);

  //! @brief Do painting of a single @a window. Always called after @ref 
  //! doUpdateWindow()
  //!
  //! @note Called by @c doUpdate(), never call manually.
  virtual void doPaintWindow(FbWindow* window, Painter& painter, const RectI& rect);

  //! @brief Blits window content into screen. Called usually from @c doUpdateWindow().
  virtual void doBlitWindow(FbWindow* window) = 0;

  // --------------------------------------------------------------------------
  // [Window Management]
  // --------------------------------------------------------------------------

  //! @brief Create a new @c FbWindow instance.
  virtual FbWindow* createWindow() = 0;

  //! @brief Destroy an existing @c FnWindow instance.
  //!
  //! @note You can only use @c FbWindow created by @c createWindow() method.
  virtual void destroyWindow(FbWindow* window) = 0;

  //! @brief Clean all references to the @a window.
  //!
  //! @note You can only use @c FbWindow created by @c createWindow() method.
  virtual void cleanupWindow(FbWindow* window);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Lock.
  Lock _lock;

  //! @brief Whether the engine was correctly initialized.
  uint32_t _isInitialized;

  //! @brief Library instance, non-null in case that the @c FbEngine was opened
  //! as a plugin (see @c X11FbEngine).
  //!
  //! @note This is mainly useful/used under Linux operating system, where more
  //! engines can be build and user can specify which one to use (or it can be
  //! also detected by the runtime).
  Library _library;

  //! @brief Frame-buffer ID <-> FbWindow.
  //!
  //! @note Engine can create custom implementation of ID<->FbWindow translation.
  //! However, this default implementation is currently used by all FbEngine
  //! implementations used by Fog-Framework.
  Hash<void*, FbWindow*> _windowMap;

  //! @brief List of dirty frame-buffer windows which need to be updated.
  //!
  //! First call to @c scheduleUpdate() should add FbWindow into this list, all
  //! other calls should be cached (FbWindow is added only once per update to
  //! the list).
  List<FbWindow*> _dirtyList;

  //! @brief Display information.
  FbDisplayInfo _displayInfo;
  //! @brief Display palette information.
  FbPaletteInfo _paletteInfo;

  //! @brief Keyboard info.
  FbKeyboardInfo _keyboardInfo;
  //! @brief Mouse information.
  FbMouseInfo _mouseInfo;

  //! @brief Keyboard state.
  FbKeyboardState _keyboardState[16];
  //! @brief Mouse state.
  FbMouseState _mouseState[16];

  //! @brief Update task (1 instance per engine).
  void* _updateTask;

  //! @brief Update is currently in progress.
  bool _updateInProgress;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_UI_ENGINE_FBENGINE_H
