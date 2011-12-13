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

namespace Fog {

//! @addtogroup Fog_UI_Engine
//! @{

// ============================================================================
// [Fog::FbWindow]
// ============================================================================

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

  FOG_INLINE bool isCreated() const { return _handle != NULL; }
  FOG_INLINE void* getHandle() const { return _handle; }

  FOG_INLINE bool isEnabled() const { return _isEnabled; }
  FOG_INLINE bool isVisible() const { return _isVisible; }
  FOG_INLINE bool isDirty() const { return _isDirty; }
  FOG_INLINE bool hasFocus() const { return _hasFocus; }
  
  FOG_INLINE uint32_t getScreen() const { return _screen; }
  
  FOG_INLINE ImageBits& getBuffer() { return _buffer; }
  FOG_INLINE const ImageBits& getBuffer() const { return _buffer; }
  
  FOG_INLINE uint32_t getWindowDepth() const { return _windowDepth; }

  // --------------------------------------------------------------------------
  // [Window Management]
  // --------------------------------------------------------------------------

  virtual err_t create(uint32_t flags) = 0;
  virtual err_t destroy() = 0;

  virtual err_t show(uint32_t state) = 0;
  virtual err_t hide() = 0;

  virtual err_t takeFocus() = 0;
  
  //! @broef Move the frame-buffer window on top of other window @a w.
  //!
  //! @param w The top window to cover or @a NULL to make the window top-most.
  virtual void moveToTop(GuiWindow* w) = 0;

  //! @brief Move the frame-buffer window behind other window @a w.
  //!
  //! @param w The bottom window to move behind or @a NULL to move the window
  //! behind all.
  virtual void moveToBottom(GuiWindow* w) = 0;

  //! @brief Translate world (screen) pixel coordinates into the frame-bufer
  //! window client area.
  virtual err_t worldToClient(PointI& pt) = 0;

  //! @brief Translate frame-buffer window client area coordinates into the
  //! world (screen).
  virtual err_t clientToWorld(PointI& pt) = 0;

  //! @brief Set the frame-buffer window as dirty (schedule an update task).
  virtual void setDirty();

  // --------------------------------------------------------------------------
  // [Window Parameters]
  // --------------------------------------------------------------------------

  virtual err_t getParameter(uint32_t id, const void* val) = 0;
  virtual err_t setParameter(uint32_t id, const void* val) = 0;

  // --------------------------------------------------------------------------
  // [Double-Buffer]
  // --------------------------------------------------------------------------

  virtual void allocDoubleBuffer(const SizeI& size) = 0;
  virtual void freeDoubleBuffer() = 0;

  // --------------------------------------------------------------------------
  // [Events]
  // --------------------------------------------------------------------------
  
  virtual void onFbEvent(FbEvent* event);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Native frame-buffer window handle.
  void* _handle;

  //! @brief Whether the window is enabled.
  uint8_t _isEnabled;

  //! @brief Whether the window is visible.
  uint8_t _isVisible;

  //! @brief Window is dirty and needs update.
  uint8_t _isDirty;

  //! @brief Whether the window has focus.
  uint8_t _hasFocus;

  //! @brief Window visibility.
  uint8_t _visibility;

  //! @brief Whether the window must be blit onto the screen.
  uint8_t _needBlit;

  //! @brief Whether to use cache double-buffer when resizing a window.
  //!
  //! @note This feature is enabled by default on nearly all OSes. The only
  //! exception is iOS where it's not making sense, because the window is
  //! always shown at full-screen.
  uint8_t _useCachedResize;

  //! @brief Reserved data.
  uint8_t _reserved[1];

  //! @brief Window screen ID (the screen where the window is).
  uint32_t _screenID;

  //! @brief Window geometry, relative to screen.
  RectI _windowGeometry;
  //! @brief Window client rectangle.
  RectI _clientGeometry;

  //! @brief Window size granularity.
  PointI _sizeGranularity;

  //! @brief Double-buffer stored in @c ImageBits structure.
  //!
  //! These parameters are used for creating a painter, they aren't cached.
  //! Note that image stride can be larger than aligned (width * bytesPerPixel).
  ImageBits _bufferData;

  //! @brief Size of @c _bufferData cache.
  SizeI _bufferCacheSize;

  //! @brief Depth of window double-buffer.
  uint32_t _windowDepth;

  //! @brief Window title.
  StringW _windowTitle;
};

// ============================================================================
// [Fog::GuiBackBuffer]
// ============================================================================

//! @brief Provides cacheable backing store for system windows (@c GuiWindow).
struct FOG_API GuiBackBuffer
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getType() const { return _type; }

  FOG_INLINE const SizeI& getCachedSize() const { return _cachedSize; }

  FOG_INLINE uint8_t* getPrimaryPixels() const { return _primaryPixels; }
  FOG_INLINE ssize_t getPrimaryStride() const { return _primaryStride; }

  FOG_INLINE uint8_t* getSecondaryPixels() const { return _secondaryPixels; }
  FOG_INLINE ssize_t getSecondaryStride() const { return _secondaryStride; }

  FOG_INLINE ImageConverterBlitLineFunc getConvertFunc() const { return _convertFunc; }
  FOG_INLINE TimeTicks getCreatedTime() const { return _createdTime; }
  FOG_INLINE TimeTicks getExpireTime() const { return _expireTime; }

  // --------------------------------------------------------------------------
  // [Type]
  // --------------------------------------------------------------------------

  enum TYPE
  {
    TYPE_NONE = 0,

    TYPE_WIN_DIB = 1,

    TYPE_X11_XSHM_PIXMAP = 2,
    TYPE_X11_XIMAGE = 3,
    TYPE_X11_XIMAGE_WITH_PIXMAP = 4
  };

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Type of back buffer.
  uint32_t _type;

  //! @brief Cached 32-bit double buffer pixel data.
  uint8_t* _primaryPixels;
  //! @brief Cached 32-bit double buffer stride.
  ssize_t _primaryStride;

  //! @brief Cached 4/8/16/24/32-bit buffer pixel data (only created when needed).
  uint8_t* _secondaryPixels;
  //! @brief Cached 4/8/16/24/32-bit buffer stride (only created when needed).
  ssize_t _secondaryStride;

  //! @brief Converter used to convert pixels from secondary to primary buffer.
  ImageConverterBlitLineFunc _convertFunc;
  //! @brief Converter depth.
  int _convertDepth;

  //! @brief Time when the backing store wes created (for caching).
  TimeTicks _createdTime;
  //! @brief Time when cached backing store will be expired.
  TimeTicks _expireTime;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_UI_ENGINE_FBWINDOW_H
