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

void Button::onPaint(PaintEvent* e)
{
  Painter* p = e->getPainter();
  IntSize s = getClientSize();
  IntRect bounds(0, 0, s.w, s.h);

/*
  IntRect bounds(0, 0, s.w, s.h);
  PaintUtil::draw3dRect(p, bounds, 0xFFFFFFFF, 0xFF000000, isDown());

  bounds.shrink(1);

  Pattern pat;
  pat.setType(PATTERN_TYPE_LINEAR_GRADIENT);
  pat.setSpread(PATTERN_SPREAD_PAD);

  if (!isDown())
  {
    pat.setStartPoint(IntPoint(0, 0));
    pat.setEndPoint(IntPoint(0, s.h));
  }
  else
  {
    pat.setStartPoint(IntPoint(0, s.h));
    pat.setEndPoint(IntPoint(0, 0));
  }

  pat.addStop(ArgbStop(0.00f, 0xFFFFFFFF));
  pat.addStop(ArgbStop(0.48f, 0xFF8CAAFF));
  pat.addStop(ArgbStop(0.52f, 0xFF6E8CFF));
  pat.addStop(ArgbStop(1.00f, 0xFF4664FF));
  p->setSource(pat);
  p->fillRect(bounds);

  if (isDown()) bounds.translate(1, 1);
  p->setSource(0xFF000000);
  p->drawText(bounds, _text, _font, TEXT_ALIGN_CENTER);
*/


  Pattern pat;
  pat.setType(PATTERN_TYPE_LINEAR_GRADIENT);
  pat.setSpread(PATTERN_SPREAD_PAD);

  if (!isDown())
  {
    pat.setStartPoint(IntPoint(0, 0));
    pat.setEndPoint(IntPoint(0, s.h));
  }
  else
  {
    pat.setStartPoint(IntPoint(0, s.h));
    pat.setEndPoint(IntPoint(0, 0));
  }

  pat.addStop(ArgbStop(0.00f, 0xFFFFFFFF));
  pat.addStop(ArgbStop(0.48f, 0xFF8CAAFF));
  pat.addStop(ArgbStop(0.52f, 0xFF6E8CFF));
  pat.addStop(ArgbStop(1.00f, 0xFF4664FF));
  p->setSource(pat);
  p->fillRound(bounds, IntPoint(3, 3));

  p->setSource(0xFF00006F);
  p->drawRound(
    DoubleRect(
      (double)bounds.x + 0.5,
      (double)bounds.y + 0.5,
      (double)bounds.w - 1.0,
      (double)bounds.h - 1.0), 
    DoublePoint(3.0, 3.0));

  p->setSource(0xFF000000);
  p->drawText(bounds, _text, _font, TEXT_ALIGN_CENTER);
}

} // Fog namespace
