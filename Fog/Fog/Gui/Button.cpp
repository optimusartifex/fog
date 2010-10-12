// [Fog-Gui]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Swap.h>
#include <Fog/Graphics/Painter.h>
#include <Fog/Graphics/PaintUtil.h>
#include <Fog/Gui/Button.h>

FOG_IMPLEMENT_OBJECT(Fog::Button)

namespace Fog {

// ============================================================================
// [Fog::Button]
// ============================================================================

Button::Button() : 
  ButtonBase()
{
}

Button::~Button()
{
}

SizeI Button::getSizeHint() const
{
  return SizeI(50, 24);
}

void Button::onPaint(PaintEvent* e)
{
  Painter* p = e->getPainter();
  SizeI s = getClientSize();
  RectI bounds(0, 0, s.w, s.h);

  Pattern pat;
  pat.setType(PATTERN_LINEAR_GRADIENT);
  pat.setSpread(PATTERN_SPREAD_PAD);

  PointI pts[2];
  pts[0].set(0, 0);
  pts[1].set(0, s.h);
  if (isDown()) swap(pts[0], pts[1]);
  pat.setPoints(pts[0], pts[1]);

  pat.addStop(ArgbStop(0.00f, 0xFFFFFFFF));
  pat.addStop(ArgbStop(1.00f, isMouseOver() ? 0xFFCADAFF : 0xFFB8C8FF));
  
  p->setSource(pat);
  p->fillRound(bounds, PointI(3, 3));
  p->resetSource();

  pat.resetStops();
  pat.addStop(ArgbStop(0.00f, 0xFF5070D0));
  pat.addStop(ArgbStop(1.00f, 0xFF4F4F7F));

  // p->setSource(0xFF70709F);
  p->setSource(pat);
  p->drawRound(
    RectD(
      (double)bounds.x + 0.5,
      (double)bounds.y + 0.5,
      (double)bounds.w - 1.0,
      (double)bounds.h - 1.0), 
    PointD(3.0, 3.0));

  p->setSource(0xFF000000);
  if (isDown()) bounds.translate(1, 1);
  p->drawText(bounds, _text, _font, TEXT_ALIGN_CENTER);
}

} // Fog namespace
