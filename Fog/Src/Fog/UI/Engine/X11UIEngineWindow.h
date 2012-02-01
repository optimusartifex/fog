// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_UI_ENGINE_X11UIENGINEWINDOW_H
#define _FOG_UI_ENGINE_X11UIENGINEWINDOW_H

// [Dependencies]
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/UI/Engine/UIEngineWindow.h>

// [Dependencies - Fog/UI - X11]
#include <Fog/UI/Engine/X11UIEngineXlib.h>

namespace Fog {

//! @addtogroup Fog_UI_Engine
//! @{

// ============================================================================
// [Fog::X11UIEngineWindowImpl]
// ============================================================================

//! @brief X11 @ref UIEngineWindow implementation.
struct FOG_API X11UIEngineWindowImpl : public UIEngineWindowImpl
{
  typedef UIEngineWindowImpl Base;
  
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  X11UIEngineWindowImpl(UIEngine* engine, UIEngineWindow* window);
  virtual ~X11UIEngineWindowImpl();

  // --------------------------------------------------------------------------
  // [Create / Destroy]
  // --------------------------------------------------------------------------

  virtual err_t create(uint32_t hints);
  virtual err_t destroy();

  virtual void destroyed();

  // --------------------------------------------------------------------------
  // [Enabled / Disabled]
  // --------------------------------------------------------------------------

  virtual err_t setEnabled(bool enabled);

  // --------------------------------------------------------------------------
  // [Focus]
  // --------------------------------------------------------------------------

  virtual err_t focus();

  // --------------------------------------------------------------------------
  // [Window State]
  // --------------------------------------------------------------------------

  virtual err_t setState(uint32_t state);

  // --------------------------------------------------------------------------
  // [Window Geometry]
  // --------------------------------------------------------------------------

  virtual err_t setWindowPosition(const PointI& pos);
  virtual err_t setWindowSize(const SizeI& size);

  // --------------------------------------------------------------------------
  // [Window Stack]
  // --------------------------------------------------------------------------

  virtual err_t moveToTop(void* targetHandle);
  virtual err_t moveToBottom(void* targetHandle);

  // --------------------------------------------------------------------------
  // [Window Coordinates]
  // --------------------------------------------------------------------------

  virtual err_t worldToClient(PointI& pt) const;
  virtual err_t clientToWorld(PointI& pt) const;

  // --------------------------------------------------------------------------
  // [Window Opacity]
  // --------------------------------------------------------------------------

  virtual err_t setWindowOpacity(float opacity);

  // --------------------------------------------------------------------------
  // [Window Title]
  // --------------------------------------------------------------------------

  virtual err_t setWindowTitle(const StringW& title);

  // --------------------------------------------------------------------------
  // [Window Double-Buffer]
  // --------------------------------------------------------------------------

  virtual err_t allocDoubleBuffer(const SizeI& size);
  virtual err_t freeDoubleBuffer();

  // --------------------------------------------------------------------------
  // [OnXEvent]
  // --------------------------------------------------------------------------

  virtual void onXEvent(XEvent* xe);

  // --------------------------------------------------------------------------
  // [Misc]
  // --------------------------------------------------------------------------

  //! @brief Tell window manager that we want to move our window into the 
  //! position specified when creating the window (otherwise the window manager
  //! can calculate position itself - forced by using PPosition hint).
  void setMoveableHints();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief X Input Context (XIC).
  XIC _xic;
  //! @brief GC.
  GC _gc;

  //! @brief Double-buffer data pointer.
  uint8_t* _bufferPtr;
  //! @brief Double-Buffer XSHM Segment information.
  XShmSegmentInfo _shmInfo;
  //! @brief Double-Buffer XPixmap.
  Pixmap _pixmap;
  //! @brief Double-Buffer XImage.
  XImage* _ximage;
  
  //! @brief Whether the X11 window is input only (default false).
  uint32_t _isInputOnly : 1;

  //! @brief Whether the X11 window was configures (not waiting to configure
  //! event).
  uint32_t _isConfigured : 1;

  //! @brief Whether the X11 window received MapRequest and currently is hidden.
  //! (It's requested to be mapped, but parent is hidden).
  uint32_t _mapRequest : 1;

private:
  _FOG_NO_COPY(X11UIEngineWindowImpl)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_UI_ENGINE_X11UIENGINEWINDOW_H
