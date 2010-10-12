// [Fog-Gui]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Graphics/Painter.h>
#include <Fog/Gui/ThemeEngine.h>

FOG_IMPLEMENT_OBJECT(Fog::ThemeEngine)

namespace Fog {

// ============================================================================
// [Fog::ThemeEngine]
// ============================================================================

ThemeEngine* ThemeEngine::_instance;

ThemeEngine::ThemeEngine()
{
}

ThemeEngine::~ThemeEngine()
{
}

} // Fog namespace
