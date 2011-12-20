// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_UI_ENGINE_UIENGINEWINDOW_H
#define _FOG_UI_ENGINE_UIENGINEWINDOW_H

// [Dependencies]
#include <Fog/Core/Kernel/Object.h>
#include <Fog/Core/Kernel/Timer.h>
#include <Fog/Core/Math/Constants.h>
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
#include <Fog/G2d/Source/Color.h>
#include <Fog/G2d/Tools/Region.h>
#include <Fog/UI/Engine/UIEngineSecondaryFB.h>

namespace Fog {

//! @addtogroup Fog_UI_Engine
//! @{

// ============================================================================
// [Fog::UIEngineWindowImpl]
// ============================================================================

//! @brief Frame-buffer window data (abstract)
struct FOG_API UIEngineWindowImpl
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  UIEngineWindowImpl(UIEngine* engine, UIEngineWindow* window);
  virtual ~UIEngineWindowImpl();

  // --------------------------------------------------------------------------
  // [Create / Destroy]
  // --------------------------------------------------------------------------

  virtual err_t create(uint32_t hints) = 0;
  virtual err_t destroy() = 0;

  // --------------------------------------------------------------------------
  // [Enabled / Disabled]
  // --------------------------------------------------------------------------

  virtual err_t setEnabled(bool enabled) = 0;

  // --------------------------------------------------------------------------
  // [Focus]
  // --------------------------------------------------------------------------

  virtual err_t focus() = 0;

  // --------------------------------------------------------------------------
  // [Window State]
  // --------------------------------------------------------------------------

  virtual err_t setState(uint32_t state) = 0;

  // --------------------------------------------------------------------------
  // [Window Geometry]
  // --------------------------------------------------------------------------

  virtual err_t setWindowPosition(const PointI& pos) = 0;
  virtual err_t setWindowSize(const SizeI& size) = 0;

  // --------------------------------------------------------------------------
  // [Window Stack]
  // --------------------------------------------------------------------------

  //! @broef Move the UIEngineWindow on top of other window given as @a handle.
  //!
  //! @param handle The top window to cover or @a NULL to make the window top-most.
  virtual err_t moveToTop(void* targetHandle) = 0;

  //! @brief Move the UIEngineWindow behind other window given as @a handle.
  //!
  //! @param handle The bottom window to move behind or @a NULL to move the window
  //! behind all.
  virtual err_t moveToBottom(void* targetHandle) = 0;

  // --------------------------------------------------------------------------
  // [Window Coordinates]
  // --------------------------------------------------------------------------

  //! @brief Translate world pixel coordinates into the UIEngineWindow client.
  virtual err_t worldToClient(PointI& pt) const = 0;

  //! @brief Translate UIEngineWindow client coordinates into the world.
  virtual err_t clientToWorld(PointI& pt) const = 0;

  // --------------------------------------------------------------------------
  // [Window Opacity]
  // --------------------------------------------------------------------------

  virtual err_t setOpacity(float opacity) = 0;

  // --------------------------------------------------------------------------
  // [Window Title]
  // --------------------------------------------------------------------------

  virtual err_t setWindowTitle(const StringW& title) = 0;

  // --------------------------------------------------------------------------
  // [Window - Double-Buffer]
  // --------------------------------------------------------------------------

  virtual err_t allocDoubleBuffer(const SizeI& size) = 0;
  virtual err_t freeDoubleBuffer() = 0;

  virtual err_t resizeDoubleBuffer(const SizeI& size);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------
  
  // Members here are stored in a way to minimize gaps needed to align the data
  // types. Reorder them only in case you find a better way.

  //! @brief Frame-buffer engine which owns the @c UIEngineWindowImpl.
  UIEngine* _engine;

  //! @brief Back-reference to the @c UIEngineWindow instance.
  UIEngineWindow* _window;

  //! @brief Native window handle.
  void* _handle;

  //! @brief Whether the window is enabled.
  uint32_t _isEnabled : 1;
  //! @brief Whether the window is enabled to the parent (only if isEnabled == 0).
  uint32_t _isEnabledToParent : 1;

  //! @brief Whether the window is visible.
  uint32_t _isVisible : 1;
  //! @brief Whether the window is visible to the parent (only if isVisible == 0).
  uint32_t _isVisibleToParent : 1;

  //! @brief Whether the window is dirty and needs update.
  uint32_t _isDirty : 1;
  //! @brief Whether the window has focus.
  uint32_t _hasFocus : 1;

  //! @brief Whether the window is fully opaque.
  //!
  //! fully opaque window is window whose @c opacity is 1 or very close to this
  //! value. Native windowing system (frame-buffer) usually contains BYTE which
  //! specifies the window opacity. If this BYTE is 256 after rounding then
  //! @c fullyOpaque is automatically set to @c true, regardless of the real 
  //! @c opacity value.
  uint32_t _isWindowOpaque : 1;

  //! @brief Whether to use cache double-buffer when resizing a window.
  //!
  //! @note This feature is enabled by default on nearly all OSes. The only
  //! exception is iOS where it's not making sense, because the window is
  //! always shown at full-screen.
  uint32_t _isBufferCacheEnabled : 1;

  //! @brief Whether the window is not fixed (movable and resizable).
  //!
  //! This is always true for iOS.
  uint32_t _isFixed : 1;

  //! @brief Window orientation.
  uint32_t _orientation : 1;

  //! @brief Whether the window should be updated.
  uint32_t _shouldUpdate : 1;

  //! @brief Whether the window should be paint.
  uint32_t _shouldPaint : 1;

  //! @brief Whether the window should be blit onto the screen.
  uint32_t _shouldBlit : 1;

  //! @brief Whether to reset window hints in createWindow().
  uint32_t _resetHints : 1;

  //! @brief Depth of window double-buffer.
  uint32_t _windowDepth : 10;

  //! @brief Window state.
  uint32_t _windowState : 4;

  //! @brief Window style.
  uint32_t _windowType : 4;

  //! @brief Window hints.
  uint32_t _windowHints;

  //! @brief Window screen (the screen is where the window is displayed).
  uint32_t _windowScreen;

  //! @brief Window opacity in [0, 1] range (default 1).
  float _windowOpacity;

  //! @brief Window geometry, relative to screen (default value depends on 
  //! windowing system or window manager).
  RectI _windowGeometry;

  //! @brief Client geometry (default value depends on windowing system in case
  //! that native window border/title is enabled and used).
  RectI _clientGeometry;

  //! @brief Window size granularity (default 1x1).
  SizeI _sizeGranularity;

  //! @brief Double-buffer stored as @c ImageBits structure.
  //!
  //! These parameters are used for creating a painter, they aren't cached.
  //! Note that image stride can be larger than aligned (width * bytesPerPixel).
  ImageBits _bufferData;

  //! @brief Double-buffer type.
  uint32_t _bufferType;

  //! @brief Cache size of @c buffer.
  SizeI _bufferCacheSize;

  //! @brief Time when the double-buffer wes created (for caching).
  TimeTicks _bufferCacheCreated;
  //! @brief Time when the double-buffer will expire.
  TimeTicks _bufferCacheExpire;

  //! @brief Secondary double-buffer.
  //!
  //! @note Valid only in case that target windowing system and Fog-Framework
  //! can't share buffer for painting and blitting. It can currently happen
  //! only in case that the target pixel format is 8-bit per pixel.
  UIEngineSecondaryFB _secondaryFB;

  //! @brief Window title.
  StringW _windowTitle;

  //! @brief Region which will be used to paint and blit the window into the 
  //! screen.
  Region _regionOfInterest;

private:
  _FOG_NO_COPY(UIEngineWindowImpl)
};

// ============================================================================
// [Fog::UIEngineWindow]
// ============================================================================

//! @brief Frame-buffer window.
struct FOG_API UIEngineWindow
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  UIEngineWindow(UIEngine* engine, uint32_t hints = 0);
  virtual ~UIEngineWindow();

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get UIEngineWindow owner (@ref UIEngine).
  UIEngine* getEngine() const;

  //! @brief Get UIEngineWindow handle.
  void* getHandle() const;

  //! @brief Get whether the UIEngineWindow was created.
  bool isCreated() const;

  //! @brief Get the native depth of the UIEngineWindow.
  uint32_t getWindowDepth() const;

  //! @brief Get the UIEngineWindow screen id.
  uint32_t getWindowScreen() const;

  // --------------------------------------------------------------------------
  // [Dirty]
  // --------------------------------------------------------------------------

  //! @brief Get whether the UIEngineWindowis dirty (needs update or blit).
  bool isDirty() const;
  
  // --------------------------------------------------------------------------
  // [Enabled / Disabled]
  // --------------------------------------------------------------------------

  //! @brief Get whether the UIEngineWindow is enabled.
  bool isEnabled() const;

  //! @brief Enable or disable UIEngineWindow.
  err_t setEnabled(bool enabled);

  FOG_INLINE err_t enable() { return setEnabled(true); }
  FOG_INLINE err_t disable() { return setEnabled(false); }

  // --------------------------------------------------------------------------
  // [Focus]
  // --------------------------------------------------------------------------

  //! @brief Get whether the UIEngineWindow has focus.
  bool hasFocus() const;

  //! @brief Try to focus on the UIEngineWindow.
  err_t focus();

  // --------------------------------------------------------------------------
  // [Window State]
  // --------------------------------------------------------------------------

  //! @brief Get whether the UIEngineWindow is visible.
  bool isVisible() const;

  //! @brief Get UIEngineWindow state.
  uint32_t getState() const;

  //! @brief Set UIEngineWindow state.
  virtual err_t setState(uint32_t state);

  //! @brief Set UIEngineWindow state to @ref WINDOW_STATE_NORMAL, 
  //! @ref WINDOW_STATE_MAXIMIZED, or @ref WINDOW_STATE_FULLSCREEN.
  FOG_INLINE err_t show() { return setState(WINDOW_STATE_NORMAL); }

  //! @brief Set UIEngineWindow state to @ref WINDOW_STATE_HIDDEN.
  FOG_INLINE err_t hide() { return setState(WINDOW_STATE_HIDDEN); }

  // --------------------------------------------------------------------------
  // [Window Geometry]
  // --------------------------------------------------------------------------

  //! @brief Get window geometry.
  RectI getWindowGeometry() const;
  //! @brief Get client geometry.
  RectI getClientGeometry() const;
  //! @brief Get Size granularity, used when resizing.
  SizeI getSizeGranularity() const;

  //! @brief Set window position to @a pos.
  err_t setWindowPosition(const PointI& pos);
  //! @brief Set window size to @a size.
  err_t setWindowSize(const SizeI& size);

  // --------------------------------------------------------------------------
  // [Window Stack]
  // --------------------------------------------------------------------------

  //! @broef Move the UIEngineWindow on top of other window @a w.
  //!
  //! @param w The top window to cover or @a NULL to make the window top-most.
  err_t moveToTop(UIEngineWindow* w);

  //! @brief Move the UIEngineWindow behind other window @a w.
  //!
  //! @param w The bottom window to move behind or @a NULL to move the window
  //! behind all.
  err_t moveToBottom(UIEngineWindow* w);

  // --------------------------------------------------------------------------
  // [Window Coordinates]
  // --------------------------------------------------------------------------

  //! @brief Translate world (screen) pixel coordinates into the frame-bufer
  //! window client area.
  err_t worldToClient(PointI& pt) const;

  //! @brief Translate frame-buffer window client area coordinates into the
  //! world (screen).
  err_t clientToWorld(PointI& pt) const;

  // --------------------------------------------------------------------------
  // [Window Opacity]
  // --------------------------------------------------------------------------

  //! @brief Get whether the UIEngineWindow is fully opaque.
  bool isWindowOpaque() const;
  //! @brief Get the opacity of the UIEngineWindow.
  float getWindowOpacity() const;

  // --------------------------------------------------------------------------
  // [Window Title]
  // --------------------------------------------------------------------------

  StringW getWindowTitle() const;
  err_t setWindowTitle(const StringW& title);

  // --------------------------------------------------------------------------
  // [Window Double-Buffer]
  // --------------------------------------------------------------------------

  //! @brief Get whether the double-buffer caching is enabled.
  //!
  //! @note Takes effect only when window is resized.
  bool isBufferCacheEnabled() const;

  // --------------------------------------------------------------------------
  // [Events]
  // --------------------------------------------------------------------------
  
  virtual void onEngineEvent(UIEngineEvent* ev);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Window data.
  UIEngineWindowImpl* _d;

private:
  _FOG_NO_COPY(UIEngineWindow)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_UI_ENGINE_UIENGINEWINDOW_H
