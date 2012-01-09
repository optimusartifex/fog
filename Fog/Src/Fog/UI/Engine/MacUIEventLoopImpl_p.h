// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_UI_ENGINE_MACUIEVENTLOOPIMPL_P_H
#define _FOG_UI_ENGINE_MACUIEVENTLOOPIMPL_P_H

// [Dependencies]
#include <Fog/Core/Kernel/MacEventLoopImpl.h>

namespace Fog {

//! @addtogroup Fog_UI_Engine
//! @{

// ============================================================================
// [Fog::MacUIEventLoopImpl]
// ============================================================================

//! @brief Convenience class that sets event loop type to "UI.Mac".
//!
//! This event loop is also registered by application initializer if
//! FOG_BUILD_UI is enabled.
struct FOG_NO_EXPORT MacUIEventLoopImpl : public MacEventLoopImpl
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  MacUIEventLoopImpl();

  // --------------------------------------------------------------------------
  // [Run]
  // --------------------------------------------------------------------------

  virtual err_t runInternal();
};

//! @}

} // Fog namespace

#endif // _FOG_UI_ENGINE_MACUIEVENTLOOPIMPL_P_H
