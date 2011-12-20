// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_UI_ENGINE_MACFBWINDOW_H
#define _FOG_UI_ENGINE_MACFBWINDOW_H

// [Dependencies]
#include <Fog/UI/Engine/UIEngineWindow.h>

namespace Fog {

//! @addtogroup Fog_UI_Engine
//! @{

// ============================================================================
// [Fog::MacUIEngineWindowData]
// ============================================================================

//! @brief Mac specific frame-buffer window data.
struct FOG_API MacUIEngineWindowData : public UIEngineWindowData
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  MacUIEngineWindowData(MacUIEngine* engine, UIEngineWindow* window);
  virtual ~MacUIEngineWindowData();

  // --------------------------------------------------------------------------
  // [Window Management]
  // --------------------------------------------------------------------------

  virtual err_t create(uint32_t flags);
  virtual err_t destroy();

  // --------------------------------------------------------------------------
  // [Window Stack]
  // --------------------------------------------------------------------------

  virtual void moveToTop(void* handle);
  virtual void moveToBottom(void* handle);

  // --------------------------------------------------------------------------
  // [Window Coordinates]
  // --------------------------------------------------------------------------

  virtual err_t worldToClient(PointI& pt) const;
  virtual err_t clientToWorld(PointI& pt) const;

  // --------------------------------------------------------------------------
  // [Window Parameters]
  // --------------------------------------------------------------------------

  virtual err_t getParameter(uint32_t id, const void* val) const;
  virtual err_t setParameter(uint32_t id, const void* val);

  // --------------------------------------------------------------------------
  // [Window Frame-Buffer]
  // --------------------------------------------------------------------------

  virtual err_t allocFrameBuffer(const SizeI& size);
  virtual err_t freeFrameBuffer();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

private:
  _FOG_NO_COPY(MacUIEngineWindowData)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_UI_ENGINE_MACFBWINDOW_H
