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
#include <Fog/Graphics/PaintUtil.h>
#include <Fog/Gui/CheckBox.h>

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
  IntSize s = getClientSize();

  IntRect bounds(0, 0, s.w, s.h);
  IntRect chrect(1, (s.h - 13) / 2, 13, 13);

  p->setSource(0xFF000000);
  p->drawRect(chrect.adjusted(0, 0, -1, -1));

  chrect.shrink(1);

  p->setSource(0xFFFFFFFF);
  p->fillRect(chrect);

  if (getChecked())
  {
    double c = (double)(s.h / 2);

    DoublePath path;
    path.moveTo(3.5, c - 1.5);
    path.lineTo(6.5, c + 3.5);
    path.lineTo(11.5, c - 4.5);

    p->setSource(0xFF000000);
    p->drawPath(path);
  }

  if (isDown())
  {
    IntRect d = chrect.adjusted(0, 0, -1, -1);
    p->setSource(0x7F8FAFFF);
    p->drawRect(d);
    d.shrink(1);
    p->setSource(0x7F6F8FFF);
    p->drawRect(d);
  }

  bounds.shrink(1);
  bounds.x += 16;
  bounds.w -= 16;

  p->setSource(0xFF000000);
  p->drawText(bounds, _text, _font, TEXT_ALIGN_LEFT | TEXT_ALIGN_VCENTER);
}

} // Fog namespace
