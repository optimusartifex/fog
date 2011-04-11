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
#include <Fog/Gui/Widget/Frame.h>

FOG_IMPLEMENT_OBJECT(Fog::Frame)

namespace Fog {

// ============================================================================
// [Fog::Frame]
// ============================================================================

Frame::Frame(uint32_t createFlags) :
  Widget(createFlags),
  _frameStyle(FRAME_NONE)
{
}

Frame::~Frame()
{
}

void Frame::setFrameStyle(uint32_t frameStyle)
{
  if (_frameStyle == frameStyle) return;

  _frameStyle = frameStyle;

  // Make layout / client geometry dirty only if the size changes.
  RectI newClientGeometry(0, 0, getWidth(), getHeight());
  calcClientGeometry(newClientGeometry);

  if (!newClientGeometry.isValid())
    newClientGeometry.set(0, 0, getWidth(), getHeight());

  if (_clientGeometry == newClientGeometry)
  {
    // Client geometry is not changed, everything we need is to repaint the
    // non-client area.

    // TODO
  }
  else
  {
    // Client geometry changed, we need to update everything inside and to
    // recalculate layout.

    // TODO
  }
}

void Frame::calcWidgetSize(SizeI& size) const
{
  switch (_frameStyle)
  {
    default:
    case FRAME_NONE:
      break;

    case FRAME_TEXT_AREA:
      size.adjust(2, 2);
      break;
  }
}

void Frame::calcClientGeometry(RectI& geometry) const
{
  switch (_frameStyle)
  {
    default:
    case FRAME_NONE:
      break;

    case FRAME_TEXT_AREA:
      geometry.shrink(1);
      break;
  }
}

void Frame::onNcPaint(PaintEvent* e)
{
  Painter* p = e->getPainter();

  switch (_frameStyle)
  {
    default:
    case FRAME_NONE:
      break;

    case FRAME_TEXT_AREA:
      p->setSource(Argb32(0xFF9F9F9F));
      p->drawRect(RectI(0, 0, getWidth() - 1, getHeight() - 1));
      break;
  }
}

} // Fog namespace
