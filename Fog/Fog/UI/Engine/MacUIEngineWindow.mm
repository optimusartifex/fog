// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/UI/Engine/MacUIEngine.h>
#include <Fog/UI/Engine/MacUIEngineWindow.h>

// [Dependencies - Mac]
#if defined(FOG_OS_IOS)
# import <UIKit/UIKit.h>
#else
# import <AppKit/AppKit.h>
# import <Cocoa/Cocoa.h>
#endif // FOG_OS_IOS

namespace Fog {

// ============================================================================
// [Fog::MacUIEngineWindowData - Construction / Destruction]
// ============================================================================

MacUIEngineWindowData::MacUIEngineWindowData(MacUIEngine* engine, UIEngineWindow* window) :
  UIEngineWindowData(engine, window)
{
}

MacUIEngineWindowData::~MacUIEngineWindowData()
{
}

// ============================================================================
// [Fog::MacUIEngineWindowData - Window Management]
// ============================================================================

err_t MacUIEngineWindowData::create(uint32_t flags)
{
}

err_t MacUIEngineWindowData::destroy()
{
}

// ============================================================================
// [Fog::MacUIEngineWindowData - Window Stack]
// ============================================================================

void MacUIEngineWindowData::moveToTop(void* handle)
{
}

void MacUIEngineWindowData::moveToBottom(void* handle)
{
}

// ============================================================================
// [Fog::MacUIEngineWindowData - Window Coordinates]
// ============================================================================

err_t MacUIEngineWindowData::worldToClient(PointI& pt) const
{
}

err_t MacUIEngineWindowData::clientToWorld(PointI& pt) const
{
}

// ============================================================================
// [Fog::MacUIEngineWindowData - Window Parameters]
// ============================================================================

err_t MacUIEngineWindowData::getParameter(uint32_t id, const void* val) const
{
}

err_t MacUIEngineWindowData::setParameter(uint32_t id, const void* val)
{
}

// ============================================================================
// [Fog::MacUIEngineWindowData - Window Frame-Buffer]
// ============================================================================

err_t MacUIEngineWindowData::allocFrameBuffer(const SizeI& size)
{
}

err_t MacUIEngineWindowData::freeFrameBuffer()
{
}

} // Fog namespace
