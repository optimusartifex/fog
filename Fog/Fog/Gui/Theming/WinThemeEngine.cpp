// [Fog-Gui]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

#include <Fog/Core/Config/Config.h>
#if defined(FOG_OS_WINDOWS)

// [Dependencies]
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Library/Library.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Win/WinUxThemeLibrary.h>
#include <Fog/G2d/Painting/Painter.h>
#include <Fog/Gui/Theming/WinThemeEngine_p.h>

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
