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
#include <Fog/Gui/Widget/GroupBox.h>

FOG_IMPLEMENT_OBJECT(Fog::GroupBox)

namespace Fog {

// ============================================================================
// [Fog::GroupBox]
// ============================================================================

GroupBox::GroupBox(uint32_t createFlags) :
  Widget(createFlags)
{
}

GroupBox::~GroupBox()
{
}

void GroupBox::setLabel(const String& label)
{
  _label = label;
  update(WIDGET_UPDATE_NCPAINT);
}

void GroupBox::calcWidgetSize(SizeI& size) const
{
}

void GroupBox::calcClientGeometry(RectI& geometry) const
{
}

} // Fog namespace
