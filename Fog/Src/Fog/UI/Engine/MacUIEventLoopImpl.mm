// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Tools/ManagedString.h>
#include <Fog/UI/Engine/MacUIEventLoopImpl_p.h>

// [Dependencies - Mac]
#import <Foundation/Foundation.h>

#if defined(FOG_OS_IOS)
# import <UIKit/UIKit.h>
#else
# import <AppKit/AppKit.h>
# import <Cocoa/Cocoa.h>
#endif // FOG_OS_IOS

namespace Fog {

// ============================================================================
// [Fog::MacUIEventLoopImpl - Construction / Destruction]
// ============================================================================

MacUIEventLoopImpl::MacUIEventLoopImpl()
{
  // Retype event loop to "UI.Mac".
  _type = FOG_STR_(APPLICATION_UI_Mac);
}

// ============================================================================
// [Fog::MacUIEventLoopImpl - Run]
// ============================================================================

err_t MacUIEventLoopImpl::runInternal()
{
  [NSApp run];
  return ERR_OK;
}

} // Fog namespace
