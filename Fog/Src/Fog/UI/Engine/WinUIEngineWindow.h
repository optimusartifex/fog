// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_UI_ENGINE_WINUIENGINEWINDOW_H
#define _FOG_UI_ENGINE_WINUIENGINEWINDOW_H

// [Dependencies]
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/UI/Engine/UIEngineWindow.h>

namespace Fog {

//! @addtogroup Fog_UI_Engine
//! @{

// ============================================================================
// [Fog::WinUIEngineWindowImpl]
// ============================================================================

//! @brief Windows @ref UIEngineWindow implementation.
struct FOG_API WinUIEngineWindowImpl : public UIEngineWindowImpl
{
  typedef UIEngineWindowImpl Base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  WinUIEngineWindowImpl(UIEngine* engine, UIEngineWindow* window);
  virtual ~WinUIEngineWindowImpl();

  // --------------------------------------------------------------------------
  // [Create / Destroy]
  // --------------------------------------------------------------------------

  virtual err_t create(uint32_t hints);
  virtual err_t destroy();

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
  // [OnWinMsg]
  // --------------------------------------------------------------------------

  LRESULT onWinMsg(UINT msg, WPARAM wParam, LPARAM lParam);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Private image which is created as @c IMAGE_TYPE_WIN_DIB.
  Image _bufferImage;

private:
  _FOG_NO_COPY(WinUIEngineWindowImpl)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_UI_ENGINE_WINUIENGINEWINDOW_H
