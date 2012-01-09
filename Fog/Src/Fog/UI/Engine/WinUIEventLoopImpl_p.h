// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_UI_ENGINE_WINUIEVENTLOOPIMPL_P_H
#define _FOG_UI_ENGINE_WINUIEVENTLOOPIMPL_P_H

// [Dependencies]
#include <Fog/Core/Kernel/WinEventLoopImpl.h>

namespace Fog {

//! @addtogroup Fog_UI_Engine
//! @{

// ============================================================================
// [Fog::WinUIEventLoopImpl]
// ============================================================================

//! @brief Convenience class that sets event loop type to "UI.Win".
//!
//! This event loop is also registered by application initializer if
//! FOG_BUILD_UI is enabled.
struct FOG_NO_EXPORT WinUIEventLoopImpl : public WinEventLoopImpl
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  WinUIEventLoopImpl();
};

//! @}

} // Fog namespace

#endif // _FOG_UI_ENGINE_WINUIEVENTLOOPIMPL_P_H
