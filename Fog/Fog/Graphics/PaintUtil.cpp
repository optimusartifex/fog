// [Fog-Graphics]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Graphics/PaintUtil.h>

namespace Fog {
namespace PaintUtil {

void draw3dRect(Painter* p, const RectI& r, ArgbI color1, ArgbI color2, bool checked)
{
  if (!r.isValid()) return;

  double x1 = (double)r.getX1() + 0.5;
  double y1 = (double)r.getY1() + 0.5;
  double x2 = (double)r.getX2() - 0.5;
  double y2 = (double)r.getY2() - 0.5;

  p->save();

  p->setLineWidth(1.0);
  p->setLineCaps(LINE_CAP_SQUARE);

  p->setSource(checked ? color2 : color1);
  p->drawLine(PointD(x1   , y1    ), PointD(x2   , y1    ));
  p->drawLine(PointD(x1   , y1+1.0), PointD(x1   , y2-1.0));

  p->setSource(checked ? color1 : color2);
  p->drawLine(PointD(x2   , y1+1.0), PointD(x2   , y2-1.0));
  p->drawLine(PointD(x1   , y2    ), PointD(x2   , y2    ));

  p->restore();
}

} // PaintUtil namespace
} // Fog namespace
