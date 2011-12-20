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
#include <Fog/UI/Engine/WinUIEventLoopImpl_p.h>

namespace Fog {

// ============================================================================
// [Fog::WinUIEventLoopImpl]
// ============================================================================

WinUIEventLoopImpl::WinUIEventLoopImpl()
{
  type = FOG_STR_(APPLICATION_UI_Win);
}

} // Fog namespace
