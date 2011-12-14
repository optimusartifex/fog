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

  FbWindowData();
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
  // [Window Update]
  // --------------------------------------------------------------------------

  //! @brief Set the frame-buffer window as dirty (schedule an update task).
  virtual void setDirty();

  // --------------------------------------------------------------------------
  // [Window Parameters]
  // --------------------------------------------------------------------------

  virtual err_t getParameter(uint32_t id, const void* val) = 0;
  virtual err_t setParameter(uint32_t id, const void* val) = 0;

  // --------------------------------------------------------------------------
  // [Window Frame-Buffer]
  // --------------------------------------------------------------------------

  virtual void allocFrameBuffer(const SizeI& size) = 0;
  virtual void freeFrameBuffer() = 0;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Frame-buffer engine which owns the @c FbWindowData.
  FbEngine* engine;

  //! @brief Back-reference to the @c FbWindow instance.
  FbWindow* window;

  //! @brief Native frame-buffer window handle.
  void* handle;

  //! @brief Whether the window is enabled.
  uint8_t isEnabled;

  //! @brief Whether the window is visible.
  uint8_t isVisible;

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

  //! @brief Whether the window must be blit onto the screen.
  uint8_t needBlit;

  //! @brief Reserved for future use (currently has only alignment function).
  uint8_t reserved[2];

  //! @brief Window screen (the screen is where the window is displayed).
  uint32_t windowScreen;

  //! @brief Window geometry, relative to screen (default value depends on 
  //! windowing system or window manager).
  RectI windowGeometry;

  //! @brief Client geometry (default value depends on windowing system in case
  //! that native window border/title is enabled and used).
  RectI clientGeometry;

  //! @brief Window size granularity (default 1x1).
  SizeI sizeGranularity;

  //! @brief Window opacity in [0, 1] range (default 1).
  float windowOpacity;

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

  //! @brief Window title.
  StringW windowTitle;

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

  FOG_INLINE FbEngine* getEngine() const { return _d->engine; }
  FOG_INLINE void* getHandle() const { return _d->handle; }

  FOG_INLINE bool isCreated() const { return _d->handle != NULL; }

  FOG_INLINE bool isEnabled() const { return _d->isEnabled; }
  FOG_INLINE bool isVisible() const { return _d->isVisible; }
  FOG_INLINE bool isDirty() const { return _d->isDirty; }
  FOG_INLINE bool hasFocus() const { return _d->hasFocus; }
  
  FOG_INLINE uint32_t getWindowDepth() const { return _d->windowDepth; }
  FOG_INLINE uint32_t getWindowScreen() const { return _d->windowScreen; }

  FOG_INLINE bool isWindowOpaque() const { return _d->isWindowOpaque; }
  FOG_INLINE float getWindowOpacity() const { return _d->windowOpacity; }

  FOG_INLINE bool isCompositingEnabled() const { return _d->isCompositingEnabled; }

  FOG_INLINE const RectI& getWindowGeometry() const { return _d->windowGeometry; }
  FOG_INLINE const RectI& getClientGeometry() const { return _d->clientGeometry; }
  FOG_INLINE const SizeI& getSizeGranularity() const { return _d->sizeGranularity; }

  FOG_INLINE ImageBits* getBufferData() { return &_d->bufferData; }
  FOG_INLINE const ImageBits* getBufferData() const { return &_d->bufferData; }

  FOG_INLINE bool isBufferCacheEnabled() const { return _d->isBufferCacheEnabled != 0; }
  FOG_INLINE const SizeI& getBufferCacheSize() const { return _d->bufferCacheSize; }

  FOG_INLINE TimeTicks getBufferCacheCreated() const { return _d->bufferCacheCreated; }
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

  FbWindowData* _d;

private:
  _FOG_NO_COPY(FbWindow)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_UI_ENGINE_FBWINDOW_H
