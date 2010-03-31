// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Graphics/Painter.h>
#include <Fog/Gui/Theme.h>

FOG_IMPLEMENT_OBJECT(Fog::Theme)

namespace Fog {

// ============================================================================
// [Fog::Theme]
// ============================================================================

Theme* Theme::_instance;

Theme::Theme()
{
}

Theme::~Theme()
{
}

} // Fog namespace
