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

FOG_IMPLEMENT_OBJECT(Fog::MacUIEngine)

namespace Fog {

// ============================================================================
// [Fog::MacUIEngine - Construction / Destruction]
// ============================================================================

MacUIEngine::MacUIEngine()
{
}

MacUIEngine::~MacUIEngine()
{
}

// ============================================================================
// [Fog::MacUIEngine - Display / Palette]
// ============================================================================

void MacUIEngine::updateDisplayInfo()
{
}

// ============================================================================
// [Fog::MacUIEngine - Keyboard / Mouse]
// ============================================================================

void MacUIEngine::setMouseWheelLines(uint32_t lines)
{
}

// ============================================================================
// [Fog::MacUIEngine - ScheduleUpdate / DoUpdate]
// ============================================================================

void MacUIEngine::doBlitWindow(UIEngineWindow* Window)
{
}

// ============================================================================
// [Fog::MacUIEngine - Window Management]
// ============================================================================

UIEngineWindow* MacUIEngine::createWindow()
{
}

void MacUIEngine::destroyWindow(UIEngineWindow* Window)
{
}

void MacUIEngine::cleanupWindow(UIEngineWindow* Window)
{
}

} // Fog namespace
