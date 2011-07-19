// [Fog-Gui]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Painting/PaintUtil.h>
#include <Fog/G2d/Painting/Painter.h>
#include <Fog/G2d/Source/LinearGradient.h>
#include <Fog/G2d/Text/TextRect.h>
#include <Fog/Gui/Widget/Button.h>

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
  RoundF round(
    (float)bounds.x + 0.5f, (float)bounds.y + 0.5f,
    (float)bounds.w - 1.0f, (float)bounds.h - 1.0f, 3.0f);

  PointF pts[2];
  pts[0].set(0.0f, 0.0f);
  pts[1].set(0.0f, float(s.h));
  if (isDown()) swap(pts[0], pts[1]);

  LinearGradientF gf(pts[0], pts[1]);
  LinearGradientF gs(gf);

  gf.setGradientSpread(GRADIENT_SPREAD_PAD);
  gf.addStop(ColorStop(0.0f, Argb32(0xFFFFFFFF)));
  gf.addStop(ColorStop(1.0f, Argb32(isMouseOver() ? 0xFFCADAFF : 0xFFB8C8FF)));

  gs.setGradientSpread(GRADIENT_SPREAD_PAD);
  gs.addStop(ColorStop(0.0f, Argb32(0xFF5070D0)));
  gs.addStop(ColorStop(1.0f, Argb32(0xFF4F4F7F)));

  p->setSource(gf);
  p->fillRound(round);
  p->setSource(gs);
  p->drawRound(round);

  p->setSource(Argb32(0xFF000000));
  if (isDown()) bounds.translate(1, 1);
  p->fillText(TextRectI(bounds, TEXT_ALIGN_CENTER), _text, _font);
}

} // Fog namespace
