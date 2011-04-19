// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/G2d/Painting/PainterUtil.h>

namespace Fog {
namespace PainterUtil {

void draw3dRect(Painter* p, const RectI& r, Argb32 color1, Argb32 color2, bool checked)
{
  if (!r.isValid()) return;

  float x0 = (float)r.getX0() + 0.5f;
  float y0 = (float)r.getY0() + 0.5f;
  float x1 = (float)r.getX1() - 0.5f;
  float y1 = (float)r.getY1() - 0.5f;

  p->save();

  p->setLineWidth(1.0f);
  p->setLineCaps(LINE_CAP_SQUARE);

  p->setSource(checked ? color2 : color1);
  p->drawLine(PointF(x0   , y0       ), PointF(x1   , y0       ));
  p->drawLine(PointF(x0   , y0 + 1.0f), PointF(x0   , y1 - 1.0f));

  p->setSource(checked ? color1 : color2);
  p->drawLine(PointF(x1   , y0 + 1.0f), PointF(x1   , y1 - 1.0f));
  p->drawLine(PointF(x0   , y1       ), PointF(x1   , y1       ));

  p->restore();
}

} // PainterUtil namespace
} // Fog namespace
