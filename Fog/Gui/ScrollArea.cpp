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
#include <Fog/Gui/ScrollArea.h>

FOG_IMPLEMENT_OBJECT(Fog::ScrollArea)

namespace Fog {

// ============================================================================
// [Fog::ScrollArea]
// ============================================================================

ScrollArea::ScrollArea(uint32_t createFlags) :
  Frame(createFlags)
{
  setFrameStyle(FRAME_TEXT_AREA);
}

ScrollArea::~ScrollArea()
{
}

void ScrollArea::calcWidgetSize(IntSize& size) const
{
  Frame::calcWidgetSize(size);
}

void ScrollArea::calcClientGeometry(IntRect& geometry) const
{
  Frame::calcClientGeometry(geometry);
}

} // Fog namespace
