// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Tools/InternedString.h>
#include <Fog/UI/Engine/WinUIEventLoopImpl_p.h>

namespace Fog {

// ============================================================================
// [Fog::WinUIEventLoopImpl - Construction / Destruction]
// ============================================================================

WinUIEventLoopImpl::WinUIEventLoopImpl()
{
  _type = FOG_S(APPLICATION_UI_Win);
}

} // Fog namespace
