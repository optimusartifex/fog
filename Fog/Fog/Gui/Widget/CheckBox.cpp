// [Fog-Gui]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/G2d/Painting/PaintUtil.h>
#include <Fog/G2d/Painting/Painter.h>
#include <Fog/G2d/Text/TextRect.h>
#include <Fog/Gui/Widget/CheckBox.h>

FOG_IMPLEMENT_OBJECT(Fog::CheckBox)

namespace Fog {

// ============================================================================
// [Fog::CheckBox]
// ============================================================================

CheckBox::CheckBox() :
  ButtonBase()
{
}

CheckBox::~CheckBox()
{
}

void CheckBox::onClick(MouseEvent* e)
{
  if (e->getButton() == BUTTON_LEFT)
  {
    setChecked(_checked ^ 1);
  }

  base::onClick(e);
}

void CheckBox::onPaint(PaintEvent* e)
{
  Painter* p = e->getPainter();
  SizeI s = getClientSize();

  RectI bounds(0, 0, s.w, s.h);
  RectI chrect(1, (s.h - 13) / 2, 13, 13);

  p->setSource(Argb32(0xFF000000));
  p->drawRect(chrect.adjusted(0, 0, -1, -1));

  chrect.shrink(1);

  p->setSource(Argb32(0xFFFFFFFF));
  p->fillRect(chrect);

  if (getChecked())
  {
    float c = (float)s.h * 0.5f;

    PathF path;
    path.moveTo(PointF(3.5f, c - 1.5f));
    path.lineTo(PointF(6.5f, c + 3.5f));
    path.lineTo(PointF(11.5f, c - 4.5f));

    p->setSource(Argb32(0xFF000000));
    p->drawPath(path);
  }

  if (isDown())
  {
    RectI d = chrect.adjusted(0, 0, -1, -1);

    p->setSource(Argb32(0x7F8FAFFF));
    p->drawRect(d);

    d.shrink(1);

    p->setSource(Argb32(0x7F6F8FFF));
    p->drawRect(d);
  }

  bounds.shrink(1);
  bounds.x += 16;
  bounds.w -= 16;

  p->setSource(Argb32(0xFF000000));
  p->fillText(TextRectI(bounds, TEXT_ALIGN_LEFT | TEXT_ALIGN_VCENTER), _text, _font);
}

} // Fog namespace
