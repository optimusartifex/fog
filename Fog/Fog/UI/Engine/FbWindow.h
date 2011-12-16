// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_UI_ENGINE_FBWINDOW_H
#define _FOG_UI_ENGINE_FBWINDOW_H

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
#include <Fog/UI/Engine/FbSecondary.h>

namespace Fog {

//! @addtogroup Fog_UI_Engine
//! @{

// ============================================================================
// [Fog::FbWindowData]
// ============================================================================

//! @brief Frame-buffer window data (abstract)
struct FOG_API FbWindowData
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FbWindowData(FbEngine* engine, FbWindow* window);
  virtual ~FbWindowData();

  // --------------------------------------------------------------------------
  // [Window Management]
  // --------------------------------------------------------------------------

  virtual err_t create(uint32_t flags) = 0;
  virtual err_t destroy() = 0;

  virtual err_t show(uint32_t state) = 0;
  virtual err_t hide() = 0;

  virtual err_t takeFocus() = 0;

  // --------------------------------------------------------------------------
  // [Window Stack]
  // --------------------------------------------------------------------------

  //! @broef Move the frame-buffer window on top of other window given as @a
  //! handle.
  //!
  //! @param handle The top window to cover or @a NULL to make the window top-most.
  virtual void moveToTop(void* handle) = 0;

  //! @brief Move the frame-buffer window behind other window given as @a
  //! handle.
  //!
  //! @param handle The bottom window to move behind or @a NULL to move the window
  //! behind all.
  virtual void moveToBottom(void* handle) = 0;

  // --------------------------------------------------------------------------
  // [Window Coordinates]
  // --------------------------------------------------------------------------

  //! @brief Translate world (screen) pixel coordinates into the frame-bufer
  //! window client area.
  virtual err_t worldToClient(PointI& pt) const = 0;

  //! @brief Translate frame-buffer window client area coordinates into the
  //! world (screen).
  virtual err_t clientToWorld(PointI& pt) const = 0;

  // --------------------------------------------------------------------------
  // [Window Parameters]
  // --------------------------------------------------------------------------

  virtual err_t getParameter(uint32_t id, const void* val) const = 0;
  virtual err_t setParameter(uint32_t id, const void* val) = 0;

  // --------------------------------------------------------------------------
  // [Window Frame-Buffer]
  // --------------------------------------------------------------------------

  virtual void allocFrameBuffer(const SizeI& size) = 0;
  virtual void freeFrameBuffer() = 0;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------
  
  // Members here are stored in a way to minimize gaps needed to align the data
  // types. Reorder them only in case you find a better way.

  //! @brief Frame-buffer engine which owns the @c FbWindowData.
  FbEngine* engine;

  //! @brief Back-reference to the @c FbWindow instance.
  FbWindow* window;

  //! @brief Native frame-buffer window handle.
  void* handle;

  //! @brief Whether the window is enabled, disabled, or parent window/screen
  //! is disabled.
  uint8_t state;

  //! @brief Whether the window is visible, hidden, or parent window/screen
  //! is hidden.
  uint8_t visibility;

  //! @brief Window is dirty and needs update.
  uint8_t isDirty;

  //! @brief Whether the window has focus.
  uint8_t hasFocus;

  //! @brief Whether the window is fully opaque.
  //!
  //! fully opaque window is window whose @c opacity is 1 or very close to this
  //! value. Native windowing system (frame-buffer) usually contains BYTE which
  //! specifies the window opacity. If this BYTE is 256 after rounding then
  //! @c fullyOpaque is automatically set to @c true, regardless of the real 
  //! @c opacity value.
  uint8_t isWindowOpaque;

  //! @brief Whether the window compositing is enabled.
  //!
  //! If window compositing is enabled then the window frame-buffer is 
  //! premultiplied ARGB surface which is composited to the screen buffer
  //! after drawn.
  uint8_t isCompositingEnabled;

  //! @brief Whether to use cache double-buffer when resizing a window.
  //!
  //! @note This feature is enabled by default on nearly all OSes. The only
  //! exception is iOS where it's not making sense, because the window is
  //! always shown at full-screen.
  uint8_t isBufferCacheEnabled;

  //! @brief Depth of window double-buffer.
  uint8_t windowDepth;

  //! @brief Window visibility.
  uint8_t windowVisibility;

  //! @brief Window orientation.
  uint8_t orientation;

  //! @brief Whether the window should be updated.
  uint8_t shouldUpdate;

  //! @brief Whether the window should be paint.
  uint8_t shouldPaint;

  //! @brief Whether the window should be blit onto the screen.
  uint8_t shouldBlit;
  
  //! @brief Reserved
  uint8_t reserved[3];

  //! @brief Window geometry, relative to screen (default value depends on 
  //! windowing system or window manager).
  RectI windowGeometry;

  //! @brief Client geometry (default value depends on windowing system in case
  //! that native window border/title is enabled and used).
  RectI clientGeometry;

  //! @brief Window size granularity (default 1x1).
  SizeI sizeGranularity;

  //! @brief Double-buffer stored as @c ImageBits structure.
  //!
  //! These parameters are used for creating a painter, they aren't cached.
  //! Note that image stride can be larger than aligned (width * bytesPerPixel).
  ImageBits bufferData;

  //! @brief Cache size of @c buffer.
  SizeI bufferCacheSize;

  //! @brief Time when the double-buffer wes created (for caching).
  TimeTicks bufferCacheCreated;
  //! @brief Time when the double-buffer will expire.
  TimeTicks bufferCacheExpire;

  //! @brief Secondary double-buffer.
  //!
  //! @note Valid only in case that target windowing system and Fog-Framework
  //! can't share buffer for painting and blitting. It can currently happen
  //! only in case that the target pixel format is 8-bit per pixel.
  FbSecondary secondaryBuffer;

  //! @brief Window screen (the screen is where the window is displayed).
  uint32_t windowScreen;

  //! @brief Window opacity in [0, 1] range (default 1).
  float windowOpacity;

  //! @brief Window title.
  StringW windowTitle;

  //! @brief Region which will be used to paint and blit the window into the 
  //! screen.
  Region regionOfInterest;

private:
  _FOG_NO_COPY(FbWindowData)
};

// ============================================================================
// [Fog::FbWindow]
// ============================================================================

//! @brief Frame-buffer window.
struct FOG_API FbWindow
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FbWindow();
  virtual ~FbWindow();

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get frame-buffer window owner (@ref FbEngine).
  FOG_INLINE FbEngine* getEngine() const { return _d->engine; }

  //! @brief Get frame-buffer window handle.
  FOG_INLINE void* getHandle() const { return _d->handle; }

  //! @brief Get whether the frame-buffer window was created.
  FOG_INLINE bool isCreated() const { return _d->handle != NULL; }

  //! @brief Get whether the frame-buffer window is enabled.
  FOG_INLINE bool getState() const { return _d->state; }

  //! @brief Get whether the frame-buffer window is visible.
  FOG_INLINE bool getVisibility() const { return _d->visibility; }

  //! @brief Get whether the frame-buffer window is dirty (needs update or blit).
  FOG_INLINE bool isDirty() const { return _d->isDirty; }

  //! @brief Get whether the frame-buffer window has focus.
  FOG_INLINE bool hasFocus() const { return _d->hasFocus; }
  
  //! @brief Get the native depth of the frame-buffer window.
  FOG_INLINE uint32_t getWindowDepth() const { return _d->windowDepth; }

  //! @brief Get the frame-buffer window screen id.
  FOG_INLINE uint32_t getWindowScreen() const { return _d->windowScreen; }

  //! @brief Get whether the window is fully opaque.
  FOG_INLINE bool isWindowOpaque() const { return _d->isWindowOpaque; }
  //! @brief Get the opacity of the window.
  FOG_INLINE float getWindowOpacity() const { return _d->windowOpacity; }

  //! @brief Get whether the compositing is enabled.
  FOG_INLINE bool isCompositingEnabled() const { return _d->isCompositingEnabled; }

  //! @brief Get window geometry.
  FOG_INLINE const RectI& getWindowGeometry() const { return _d->windowGeometry; }
  //! @brief Get client geometry.
  FOG_INLINE const RectI& getClientGeometry() const { return _d->clientGeometry; }
  //! @brief Get Size granularity, used when resizing.
  FOG_INLINE const SizeI& getSizeGranularity() const { return _d->sizeGranularity; }

  //! @brief Get double-buffer data as @ref ImageBits.
  FOG_INLINE ImageBits* getBufferData() { return &_d->bufferData; }
  //! @overload
  FOG_INLINE const ImageBits* getBufferData() const { return &_d->bufferData; }

  //! @brief Get whether the double-buffer caching is enabled.
  //!
  //! @note Takes effect only when window is resized.
  FOG_INLINE bool isBufferCacheEnabled() const { return _d->isBufferCacheEnabled != 0; }
  //! @brief Get size of the cached double-buffer.
  FOG_INLINE const SizeI& getBufferCacheSize() const { return _d->bufferCacheSize; }

  //! @brief Get when the buffer cache was created.
  FOG_INLINE TimeTicks getBufferCacheCreated() const { return _d->bufferCacheCreated; }
  //! @brief Get when the buffer cache will expire.
  FOG_INLINE TimeTicks getBufferCacheExpire() const { return _d->bufferCacheExpire; }

  // --------------------------------------------------------------------------
  // [Window Stack]
  // --------------------------------------------------------------------------

  //! @broef Move the frame-buffer window on top of other window @a w.
  //!
  //! @param w The top window to cover or @a NULL to make the window top-most.
  virtual void moveToTop(FbWindow* w);

  //! @brief Move the frame-buffer window behind other window @a w.
  //!
  //! @param w The bottom window to move behind or @a NULL to move the window
  //! behind all.
  virtual void moveToBottom(FbWindow* w);

  // --------------------------------------------------------------------------
  // [Window Coordinates]
  // --------------------------------------------------------------------------

  //! @brief Translate world (screen) pixel coordinates into the frame-bufer
  //! window client area.
  virtual err_t worldToClient(PointI& pt) const;

  //! @brief Translate frame-buffer window client area coordinates into the
  //! world (screen).
  virtual err_t clientToWorld(PointI& pt) const;

  // --------------------------------------------------------------------------
  // [Events]
  // --------------------------------------------------------------------------
  
  virtual void onFbEvent(FbEvent* ev);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Window data.
  FbWindowData* _d;

private:
  _FOG_NO_COPY(FbWindow)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_UI_ENGINE_FBWINDOW_H
