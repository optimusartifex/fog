// [Fog-Gui]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/G2d/Painting/Painter.h>
#include <Fog/Gui/Widget/ScrollBar.h>

FOG_IMPLEMENT_OBJECT(Fog::ScrollBar)

namespace Fog {

// ============================================================================
// [Fog::ScrollBar]
// ============================================================================

ScrollBar::ScrollBar(uint32_t createFlags) :
  Widget(createFlags)
{
}

ScrollBar::~ScrollBar()
{
}

} // Fog namespace
