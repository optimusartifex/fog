// [Fog-Gui]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

#include <Fog/Core/Build.h>
#if defined(FOG_OS_WINDOWS)

// [Dependencies]
#include <Fog/Core/Constants.h>
#include <Fog/Core/Library.h>
#include <Fog/Core/Lock.h>
#include <Fog/Graphics/Painter.h>

#include <Fog/Gui/GuiEngine/WinUxThemeLibrary.h>
#include <Fog/Gui/ThemeEngine/WinThemeEngine_p.h>

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
