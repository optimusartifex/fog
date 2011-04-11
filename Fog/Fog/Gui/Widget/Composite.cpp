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
#include <Fog/Gui/Widget/Composite.h>

FOG_IMPLEMENT_OBJECT(Fog::Composite)

namespace Fog {

// ============================================================================
// [Fog::Composite]
// ============================================================================

Composite::Composite(uint32_t createFlags) : 
  Widget(createFlags)
{
}

Composite::~Composite()
{
}

} // Fog namespace
