// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

#include <Fog/Core/C++/Base.h>
#if defined(FOG_OS_WINDOWS)

// [Dependencies]
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/OS/Library.h>
#include <Fog/G2d/Painting/Painter.h>
#include <Fog/UI/Theming/WinThemeEngine_p.h>
#include <Fog/G2d/Win/UxThemeLibrary.h>

FOG_IMPLEMENT_OBJECT(Fog::WinThemeEngine)

namespace Fog {

// ============================================================================
// [Fog::WinThemeEngine]
// ============================================================================

WinThemeEngine::WinThemeEngine()
{
}

WinThemeEngine::~WinThemeEngine()
{
}

} // Fog namespace

#endif // FOG_OS_WINDOWS
