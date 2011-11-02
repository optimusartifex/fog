#include "Helpers.h"

Helpers::Logger logger;

namespace Fog {

// ============================================================================
// [...]
// ============================================================================

// Get the angular direction indicated by the provided dx/dy ratio.
template<typename NumT>
FOG_STATIC_INLINE_T NumT getDirection(NumT dx, NumT dy)
{
  return Math::atan2(-dy, -dx) - NumT(MATH_PI);
}

template<typename NumT>
FOG_STATIC_INLINE_T bool notEqual(NumT x0, NumT y0, NumT x1, NumT y1)
{
  return x0 != x1 || y0 != y1;
}

// Get the angular directions for a curve's end points, giving the angles
// (start->start control) and (end control->end).
template<typename NumT>
FOG_STATIC_INLINE_T void getCurveDirections(NumT* dst, NumT xa, NumT ya, NumT xb, NumT yb, NumT xc, NumT yc, NumT xd, NumT yd)
{
  dst[0] = NumT(-1);
  dst[1] = NumT(-1);

  if      (notEqual(xa,ya,xb,yb)) { dst[0] = getDirection(xb - xa, yb - ya); }
  else if (notEqual(xa,ya,xc,yc)) { dst[0] = getDirection(xc - xa, yc - ya); }
  else if (notEqual(xa,ya,xd,yd)) { dst[0] = getDirection(xd - xa, yd - ya); }

  if      (notEqual(xd,yd,xc,yc)) { dst[1] = getDirection(xd - xc, yd - yc); }
  else if (notEqual(xd,yd,xb,yb)) { dst[1] = getDirection(xd - xb, yd - yb); }
  else if (notEqual(xd,yd,xa,ya)) { dst[1] = getDirection(xd - xa, yd - ya); }
}

template<typename NumT>
FOG_STATIC_T void QBezier_offset(NumT_(Path)* dst, const NumT_(Point)* src, NumT distance, int direction)
{
  NumT_(Point) stack[128];
  NumT_(Point)* pts = stack;

  const NumT timeThreshold = NumT(0.40);
  const NumT angleThreshold = Math::deg2rad(NumT(105));

  pts[0] = src[2];
  pts[1] = src[1];
  pts[2] = src[0];

  bool isFirst = true;
  NumT_(Point) lastOffsetPoint(NumT(0), NumT(0));

  for (;;)
  {
    NumT xa = pts[2].x;
    NumT ya = pts[2].y;
    NumT xb = pts[1].x;
    NumT yb = pts[1].y;
    NumT xc = pts[0].x;
    NumT yc = pts[0].y;

    // Offsetting the end points is always simple.
    NumT angles[2];
    NumT as, ac;

    getCurveDirections<NumT>(angles, xa, ya, xb, yb, xb, yb, xc, yc);

    Math::sincos(angles[0] + NumT(MATH_HALF_PI), &as, &ac);
    NumT nxa = xa + distance * ac;
    NumT nya = ya + distance * as;

    Math::sincos(angles[1] + NumT(MATH_HALF_PI), &as, &ac);
    NumT nxc = xc + distance * ac;
    NumT nyc = yc + distance * as;

    NumT a = Math::euclideanDistance(pts[0].x, pts[0].y, pts[1].x, pts[1].y);
    NumT b = Math::euclideanDistance(pts[2].x, pts[2].y, pts[1].x, pts[1].y);

    // t = sqrt(a) / (sqrt(a) + sqrt(b) = sqrt(ab) / (sqrt(ab) + b)
    NumT sqrt_ab = Math::sqrt(a * b);
    NumT t = sqrt_ab / (sqrt_ab + b);

    NumT_(Line) line1(UNINITIALIZED);
    NumT_(Line) line2(UNINITIALIZED);
    NumT_(Point) isect(UNINITIALIZED);

    NumT bezierAngle;

    if (t < timeThreshold || t > NumT(1.0f - timeThreshold))
      goto _Split;

    bezierAngle = Math::abs(NumI_(Line)::polyAngle(pts[0], pts[1], pts[2]));
    if (bezierAngle <= angleThreshold)
    {
_Split:
      NumI_(QBezier)::splitAt(pts, pts, pts + 2, t);
      pts += 2;
      continue;
    }

    // Offsetting the control point requires a few line intersection checks.
    // For quadratic bezier curves, we don't need to figure out where the
    // curve scaling origin is to get the correct offset point.
    line1.setLine(nxa, nya, nxa + (xb - xa), nya + (yb - ya));
    line2.setLine(nxc, nyc, nxc + (xc - xb), nyc + (yc - yb));

    if (NumI_(Line)::intersect(isect, line1, line2) == LINE_INTERSECTION_NONE)
    {
      isect.x = nxa + (nxc - nxa) * NumT(0.5);
      isect.y = nya + (nyc - nya) * NumT(0.5);
    }

    if (isFirst)
    {
      dst->moveTo(NumT_(Point)(nxa, nya));
    }

    if (!isFirst && !Fuzzy<NumT_(Point)>::eq(lastOffsetPoint, NumT_(Point)(nxa, nya)))
    {
      dst->setVertex(dst->getLength() - 1, NumT_(Point)((lastOffsetPoint.x + nxa) * NumT(0.5), (lastOffsetPoint.y + nya) * NumT(0.5)));
    }

    dst->quadTo(NumT_(Point)(isect.x, isect.y),
                NumT_(Point)(nxc, nyc));

    lastOffsetPoint.set(nxc, nyc);
    isFirst = false;

    if (pts == stack)
      break;
    pts -= 2;
  }
}

#if 0
  /**
   * Offset this cubic bezier curve, which is presumed to be safe for scaling,
   * by {distance} pixels. Note that these do not need to be full pixels
   */
  Segment simpleOffset(double distance)
  {
    double xa = points[0].getX();
    double ya = points[0].getY();
    double xb = points[1].getX();
    double yb = points[1].getY();
    double xc = points[2].getX();
    double yc = points[2].getY();
    double xd = points[3].getX();
    double yd = points[3].getY();

    // offsetting the end points is just as simple as for quadratic curves
    double[] angles = getCurveDirections(xa, ya, xb, yb, xc, yc, xd, yd);
    double nxa = xa + distance*Math.cos(angles[0] + PI/2.0);
    double nya = ya + distance*Math.sin(angles[0] + PI/2.0);
    double nxd = xd + distance*Math.cos(angles[1] + PI/2.0);
    double nyd = yd + distance*Math.sin(angles[1] + PI/2.0);


    // get the scale origin, if it's not known yet
    if(scale_origin==null) { scale_origin = getScaleOrigin(xa,ya,xb,yb,xc,yc,xd,yd); }

    // if it's still null, then we couldn't figure out what the scale origin is supposed to be. That's bad.
    if(scale_origin==null) {
//      println("ERROR: NO ORIGIN FOR "+xa+","+ya+","+xb+","+yb+","+xc+","+yc+","+xd+","+yd);
      return this; }
    double xo = scale_origin[0];
    double yo = scale_origin[1];

    // offsetting the control points, however, requires much more work now
    double[] c1line1 = {nxa, nya, nxa + (xb-xa), nya + (yb-ya)};
    double[] c1line2 = {xo,yo, xb,yb};
    double[] intersection = intersectsLineLine(c1line1, c1line2);
    if(intersection==null) {
//      println("ERROR: NO INTERSECTION ON "+nxa+","+nya+","+(nxa + (xb-xa))+","+(nya + (yb-ya))+" WITH "+xo+","+yo+","+xb+","+yb);
      return this; }
    double nxb = intersection[0];
    double nyb = intersection[1];

    double[] c2line1 = {nxd, nyd, nxd + (xc-xd), nyd + (yc-yd)};
    double[] c2line2 = {xo,yo, xc,yc};
    intersection = intersectsLineLine(c2line1, c2line2);
    if(intersection==null) {
//      println("ERROR: NO INTERSECTION ON "+nxd+","+nyd+","+(nxd + (xc-xd))+","+(nyd + (yc-yd))+" WITH "+xo+","+yo+","+xc+","+yc);
      return this; }
    double nxc = intersection[0];
    double nyc = intersection[1];

    // finally, return offset curve
    Bezier3 newcurve = new Bezier3(nxa, nya, nxb, nyb, nxc, nyc, nxd, nyd);
    newcurve.scale_origin = scale_origin;

    return newcurve;
  }

#endif

#if 0
template<typename NumT>
FOG_STATIC_T void CBezier_offset(NumT_(Path)* dst, const NumT_(Point)* src, NumT distance, int direction)
{
  NumT_(Point) stack[256];
  NumT_(Point)* pts = stack;

  const NumT timeThreshold = NumT(0.40);
  const NumT angleThreshold = Math::deg2rad(NumT(100));

  pts[0] = src[3];
  pts[1] = src[2];
  pts[2] = src[1];
  pts[3] = src[0];

  bool isFirst = true;
  NumT_(Point) lastOffsetPoint(NumT(0), NumT(0));

  // --------------------------------------------------------------------------
  // [Simplify]
  // --------------------------------------------------------------------------

  pts += (reinterpret_cast<NumT_(CBezier)*>(pts)->simplifyForProcessing(pts) - 1) * 3;
/*
  {
    NumT t[2];

    switch (reinterpret_cast<NumT_(CBezier)*>(pts)->getInflectionPoints(t))
    {
      case 2:
        NumI_(CBezier)::splitAt(pts, pts, pts + 3, t[0]);
        t[0] = (t[1] - t[0]) / (NumT(1.0) - t[0]);
        pts += 3;
        // ... Fall through ...

      case 1:
        NumI_(CBezier)::splitAt(pts, pts, pts + 3, t[0]);
        pts += 3;
        break;

      case 0:
        break;

      default:
        FOG_ASSERT_NOT_REACHED();
    }
  }
*/
  // --------------------------------------------------------------------------
  // [Offset]
  // --------------------------------------------------------------------------

  for (;;)
  {
    NumT xa = pts[3].x;
    NumT ya = pts[3].y;
    NumT xb = pts[2].x;
    NumT yb = pts[2].y;
    NumT xc = pts[1].x;
    NumT yc = pts[1].y;
    NumT xd = pts[0].x;
    NumT yd = pts[0].y;

    // Offsetting the end points is always simple.
    NumT angles[2];
    NumT as[2];
    NumT ac[2];

    getCurveDirections<NumT>(angles, xa, ya, xb, yb, xc, yc, xd, yd);

    Math::sincos(angles[0] + NumT(MATH_HALF_PI), &as[0], &ac[0]);
    Math::sincos(angles[1] + NumT(MATH_HALF_PI), &as[1], &ac[1]);

    NumT nxa = xa + distance * ac[0];
    NumT nya = ya + distance * as[0];

    NumT nxd = xd + distance * ac[1];
    NumT nyd = yd + distance * as[1];

    //NumT a = Math::euclideanDistance(pts[0].x, pts[0].y, pts[1].x, pts[1].y);
    //NumT b = Math::euclideanDistance(pts[3].x, pts[3].y, pts[2].x, pts[2].y);

    // t = sqrt(a) / (sqrt(a) + sqrt(b) = sqrt(ab) / (sqrt(ab) + b)
    //NumT sqrt_ab = Math::sqrt(a * b);
    //NumT t = sqrt_ab / (sqrt_ab + b);

    NumT_(Line) line1(UNINITIALIZED);
    NumT_(Line) line2(UNINITIALIZED);
    NumT_(Point) isect(UNINITIALIZED);
    /*
    if (t < timeThreshold || t > NumT(1.0f - timeThreshold))
      goto _Split;

    NumT bezierAngle =
      Math::abs(NumI_(Line)::polyAngle(pts[0], pts[1], pts[2])) +
      Math::abs(NumI_(Line)::polyAngle(pts[1], pts[2], pts[3])) ;

    if (bezierAngle <= angleThreshold)
    {
_Split:
      NumI_(CBezier)::splitAt(pts, pts, pts + 3, NumT(0.5));
      pts += 3;
      continue;
    }
    */
    line1.setLine(xa, ya, xa + ac[0], ya + as[0]);
    line2.setLine(xd + ac[1], yd + as[0], xd, yd);

    if (NumI_(Line)::intersect(isect, line1, line2) == LINE_INTERSECTION_NONE)
    {
      // TODO:
      return;
    }

    line1.setLine(nxa, nya, nxa + (xb - xa), nya + (yb - ya));
    line2.setLine(isect.x, isect.y, xb, yb);

    if (NumI_(Line)::intersect(isect, line1, line2) == LINE_INTERSECTION_NONE)
    {
      // TODO:
      return;
    }

    NumT nxb = isect.x;
    NumT nyb = isect.y;

    line1.setLine(nxd, nyd, nxd + (xc - xd), nyd + (yc- yd));
    line2.p[1].set(xc, yc);

    if (NumI_(Line)::intersect(isect, line1, line2) == LINE_INTERSECTION_NONE)
    {
      // TODO:
      return;
    }

    NumT nxc = isect.x;
    NumT nyc = isect.y;

    if (isFirst)
    {
      dst->moveTo(NumT_(Point)(nxa, nya));
    }

    if (!isFirst && !Fuzzy<NumT_(Point)>::eq(lastOffsetPoint, NumT_(Point)(nxa, nya)))
    {
      dst->setVertex(dst->getLength() - 1, NumT_(Point)((lastOffsetPoint.x + nxa) * NumT(0.5), (lastOffsetPoint.y + nya) * NumT(0.5)));
    }

    dst->cubicTo(NumT_(Point)(nxb, nyb),
                 NumT_(Point)(nxc, nyc),
                 NumT_(Point)(nxd, nyd));

    lastOffsetPoint.set(nxd, nyd);
    isFirst = false;

    if (pts == stack)
      break;
    pts -= 3;
  }
}
#endif

#if 1
template<typename NumT>
static uint32_t FOG_CDECL LineT_intersect(NumT_(Point)* dst,
  const NumT_(Point)* lineA,
  const NumT_(Point)* lineB)
{
  NumT_(Point) ptA = lineA[1] - lineA[0];
  NumT_(Point) ptB = lineB[1] - lineB[0];

  NumT d = ptA.y * ptB.x - ptA.x * ptB.y;
  if (!Math::isFinite(d))
    return LINE_INTERSECTION_NONE;

  if (Math::isFuzzyZero(d))
  {
  }

  NumT_(Point) off = lineA[0] - lineB[0];
  NumT t = (ptB.y * off.x - ptB.x * off.y) / d;
  dst->set(lineA[0].x + ptA.x * t, lineA[0].y + ptA.y * t);

  if (t < NumT(0.0) && t > NumT(1.0))
    return LINE_INTERSECTION_UNBOUNDED;

  t = (ptA.x * off.y - ptA.y * off.x) / d;
  if (t < NumT(0.0) && t > NumT(1.0))
    return LINE_INTERSECTION_UNBOUNDED;

  return LINE_INTERSECTION_BOUNDED;
}

template<typename NumT>
FOG_STATIC_T void CBezier_offset(NumT_(Path)* dst, const NumT_(Point)* src, NumT distance, int direction)
{
  NumT_(Point) stack[256];
  NumT_(Point)* pts = stack;

  const NumT timeThreshold = NumT(0.40);
  const NumT angleThreshold = Math::deg2rad(NumT(100));

  pts[0] = src[3];
  pts[1] = src[2];
  pts[2] = src[1];
  pts[3] = src[0];

  bool isFirst = true;
  NumT_(Point) lastOffsetPoint(NumT(0), NumT(0));

  // --------------------------------------------------------------------------
  // [Simplify]
  // --------------------------------------------------------------------------

  pts += (reinterpret_cast<NumT_(CBezier)*>(pts)->simplifyForProcessing(pts) - 1) * 3;

  // --------------------------------------------------------------------------
  // [Offset]
  // --------------------------------------------------------------------------

  for (;;)
  {
    NumT xa = pts[3].x;
    NumT ya = pts[3].y;
    NumT xb = pts[2].x;
    NumT yb = pts[2].y;
    NumT xc = pts[1].x;
    NumT yc = pts[1].y;
    NumT xd = pts[0].x;
    NumT yd = pts[0].y;

    // Offsetting the end points is always simple.
    NumT angles[4];
    NumT as[4];
    NumT ac[4];

    angles[0] = getDirection<NumT>(xb - xa, yb - ya);
    angles[1] = getDirection<NumT>(xc - xb, yc - yb);
    angles[2] = getDirection<NumT>(xc - xb, yc - yb);
    angles[3] = getDirection<NumT>(xd - xc, yd - yc);

    Math::sincos(angles[0] + NumT(MATH_HALF_PI), &as[0], &ac[0]);
    Math::sincos(angles[1] + NumT(MATH_HALF_PI), &as[1], &ac[1]);
    Math::sincos(angles[2] + NumT(MATH_HALF_PI), &as[2], &ac[2]);
    Math::sincos(angles[3] + NumT(MATH_HALF_PI), &as[3], &ac[3]);

    NumT nxa = xa + distance * ac[0];
    NumT nya = ya + distance * as[0];

    NumT nxd = xd + distance * ac[3];
    NumT nyd = yd + distance * as[3];

    NumT_(Line) line1(UNINITIALIZED);
    NumT_(Line) line2(UNINITIALIZED);
    NumT_(Point) isect(UNINITIALIZED);

    NumT bezierAngle = Math::min(
      Math::abs(NumI_(Line)::polyAngle(pts[0], pts[1], pts[2])),
      Math::abs(NumI_(Line)::polyAngle(pts[1], pts[2], pts[3])));

    if (bezierAngle <= angleThreshold)
    {
_Split:
      NumI_(CBezier)::splitAt(pts, pts, pts + 3, NumT(0.5));
      pts += 3;
      continue;
    }

    NumT d;
    //
    //NumT_(Point) ptA = lineA[1] - lineA[0];
    //NumT_(Point) ptB = lineB[1] - lineB[0];

    //NumT d = ptA.y * ptB.x - ptA.x * ptB.y;
    line1.setLine(xa + distance * ac[0], ya + distance * as[0],
                  xb + distance * ac[0], yb + distance * as[0]);
    line2.setLine(xb + distance * ac[1], yb + distance * as[1],
                  xc + distance * ac[1], yc + distance * as[1]);

    d = (line1.p[1].y - line2.p[0].y) * (line2.p[1].x - line1.p[0].x) -
        (line1.p[1].x - line2.p[0].x) * (line2.p[1].y - line1.p[0].y);

    if (Math::isFuzzyZero(d) || !Math::isFinite(d) || NumI_(Line)::intersect(isect, line1, line2) == LINE_INTERSECTION_NONE)
    {
      logger.log("NO-ISECT : [%f, %f] -> [%f %f] | [%f %f] -> [%f %f] | d=%d",
        line1.p[0].x, line1.p[0].y, line1.p[1].x, line1.p[1].y,
        line2.p[0].x, line2.p[0].y, line2.p[1].x, line2.p[1].y, d);

      isect.x = line1.p[1].x;
      isect.y = line1.p[1].y;
      //isect.x = (line1.p[1].x + line2.p[0].x) * NumT(0.5);
      //isect.y = (line1.p[1].y + line2.p[0].y) * NumT(0.5);
      // TODO:
      //return;
    }
    else
    {
      logger.log("DO-ISECT : [%f, %f] -> [%f %f] | [%f %f] -> [%f %f] | d=%d",
        line1.p[0].x, line1.p[0].y, line1.p[1].x, line1.p[1].y,
        line2.p[0].x, line2.p[0].y, line2.p[1].x, line2.p[1].y, d);
    }

    NumT nxb = isect.x;
    NumT nyb = isect.y;

    line1.setLine(xd + distance * ac[3], yd + distance * as[3],
                  xc + distance * ac[3], yc + distance * as[3]);
    line2.setLine(xc + distance * ac[2], yc + distance * as[2],
                  xb + distance * ac[2], yb + distance * as[2]);

    d = (line1.p[1].y - line2.p[0].y) * (line2.p[1].x - line1.p[0].x) -
        (line1.p[1].x - line2.p[0].x) * (line2.p[1].y - line1.p[0].y);

    if (Math::isFuzzyZero(d) || !Math::isFinite(d) || NumI_(Line)::intersect(isect, line1, line2) == LINE_INTERSECTION_NONE)
    {
      logger.log("NO-ISECT : [%f, %f] -> [%f %f] | [%f %f] -> [%f %f] | d=%d",
        line1.p[0].x, line1.p[0].y, line1.p[1].x, line1.p[1].y,
        line2.p[0].x, line2.p[0].y, line2.p[1].x, line2.p[1].y, d);

      isect.x = line1.p[1].x;
      isect.y = line1.p[1].y;
      //isect.x = (line1.p[1].x + line2.p[0].x) * NumT(0.5);
      //isect.y = (line1.p[1].y + line2.p[0].y) * NumT(0.5);
      // TODO:
      //return;
    }
    else
    {
      logger.log("DO-ISECT : [%f, %f] -> [%f %f] | [%f %f] -> [%f %f] | d=%d",
        line1.p[0].x, line1.p[0].y, line1.p[1].x, line1.p[1].y,
        line2.p[0].x, line2.p[0].y, line2.p[1].x, line2.p[1].y, d);
    }

    NumT nxc = isect.x;
    NumT nyc = isect.y;

    if (isFirst)
    {
      dst->moveTo(NumT_(Point)(nxa, nya));
    }

    if (!isFirst && !Fuzzy<NumT_(Point)>::eq(lastOffsetPoint, NumT_(Point)(nxa, nya)))
    {
      dst->setVertex(dst->getLength() - 1, NumT_(Point)((lastOffsetPoint.x + nxa) * NumT(0.5), (lastOffsetPoint.y + nya) * NumT(0.5)));
    }

    dst->cubicTo(NumT_(Point)(nxb, nyb),
                 NumT_(Point)(nxc, nyc),
                 NumT_(Point)(nxd, nyd));

    lastOffsetPoint.set(nxd, nyd);
    isFirst = false;

    if (pts == stack)
      break;
    pts -= 3;
  }
}
#endif

} // Fog namespace

// ============================================================================
// [FogTest]
// ============================================================================

using namespace Fog;

struct MyWindow : public Window
{
  // [Fog Object System]
  FOG_DECLARE_OBJECT(MyWindow, Window)

  // [Construction / Destruction]
  MyWindow(uint32_t createFlags = 0);
  virtual ~MyWindow();

  // [Event Handlers]
  virtual void onKey(KeyEvent* e);
  virtual void onMouse(MouseEvent* e);
  virtual void onPaint(PaintEvent* e);

  PathF _path;
  size_t _pathSelectedIndex;
  PointF _pathSelectedOffset;

  PointF _nearestPoint;

  int _usePathStroker;
};

FOG_IMPLEMENT_OBJECT(MyWindow)

MyWindow::MyWindow(uint32_t createFlags) :
  Window(createFlags)
{
  setWindowTitle(StringW::fromAscii8("Bezier-Offset playground."));

  _path.moveTo(PointF(100.0f, 100.0f));
  _path.cubicTo(PointF(200.0f, 200.0f),
                PointF(300.0f, 100.0f),
                PointF(360.0f, 400.0f));
  _path.buildPathInfo();

  _pathSelectedIndex = INVALID_INDEX;
  _usePathStroker = false;

  Font font = getFont();
  font.setHeight(20, UNIT_PX);
  setFont(font);
}

MyWindow::~MyWindow()
{
}

void MyWindow::onKey(KeyEvent* e)
{
  if (e->getCode() == EVENT_KEY_PRESS)
  {
    if (e->getKey() == KEY_SPACE)
    {
      _usePathStroker ^= 1;
      update(WIDGET_UPDATE_ALL);
    }
  }

  base::onKey(e);
}

void MyWindow::onMouse(MouseEvent* e)
{
  if (e->getCode() == EVENT_MOUSE_PRESS && e->getButton() == BUTTON_LEFT)
  {
    size_t ptIndex = _path.getClosestVertex(PointF(e->getPosition()), 5.0f);
    if (ptIndex != INVALID_INDEX)
    {
      _pathSelectedIndex = ptIndex;
      _pathSelectedOffset = _path.getVertices()[ptIndex] - PointF(e->getPosition());

      update(WIDGET_UPDATE_ALL);
    }
    else
    {
      reinterpret_cast<const CBezierF*>(_path.getVertices())->getClosestPoint(_nearestPoint, PointF(e->getPosition()));
      update(WIDGET_UPDATE_ALL);
    }
  }

  if (e->getCode() == EVENT_MOUSE_RELEASE && e->getButton() == BUTTON_LEFT)
  {
    if (_pathSelectedIndex != INVALID_INDEX)
    {
      _pathSelectedIndex = INVALID_INDEX;
      update(WIDGET_UPDATE_ALL);
    }
  }

  if (e->getCode() == EVENT_MOUSE_MOVE)
  {
    if (_pathSelectedIndex != INVALID_INDEX)
    {
      _path.setVertex(_pathSelectedIndex, PointF(e->getPosition()) + _pathSelectedOffset);
      update(WIDGET_UPDATE_ALL);
    }
  }

  base::onMouse(e);
}

void MyWindow::onPaint(PaintEvent* e)
{
  Time startTime = Time::now();
  Painter* p = e->getPainter();

  p->setSource(Argb32(0xFFFFFFFF));
  p->fillAll();

  {
    Helpers::drawPathEx<float>(p, _path, 4.0f, Argb32(0xFFFF0000));

    PointF pts[16];

    pts[0] = _path.getVertex(0);
    pts[1] = _path.getVertex(1);
    pts[2] = _path.getVertex(2);
    pts[3] = _path.getVertex(3);

    PathF path;
    int count = reinterpret_cast<CBezierF*>(pts)->simplifyForProcessing(pts);

    path.moveTo(pts[0]);
    for (int i = 0; i < count; i++)
    {
      path.cubicTo(pts[i*3+1], pts[i*3+2], pts[i*3+3]);
    }
    Helpers::drawPathEx<float>(p, path, 4.0f, Argb32(0xFF000000));
  }

  if (_usePathStroker)
  {
    PathStrokerF stroker;
    stroker._params->setLineWidth(30.0f);
    PathF strokedPath;

    stroker.strokePath(strokedPath, _path);

    p->setSource(Argb32(0x100000FF));
    p->fillPath(strokedPath);

    Helpers::drawPathEx<float>(p, strokedPath, 2.0f, Argb32(0xFF0000FF));
    Helpers::drawPathInfo<float>(p, PointI(5, 15), strokedPath, getFont(), Argb32(0xFF000000));
  }
  else
  {
    PathF path;
    PointF pts[4];

    pts[0] = _path.getVertex(3);
    pts[1] = _path.getVertex(2);
    pts[2] = _path.getVertex(1);
    pts[3] = _path.getVertex(0);

    CBezier_offset<float>(&path, _path.getVertices(), 15.0f, 1);
    size_t len = path.getLength();
    CBezier_offset<float>(&path, pts, 15.0f, -1);

    if (len)
    {
      path.getCommandsX()[len] = PATH_CMD_LINE_TO;
      path.close();
    }

    p->setSource(Argb32(0x100000FF));
    p->fillPath(path);

    Helpers::drawPathEx<float>(p, path, 2.0f, Argb32(0xFF0000FF));
    Helpers::drawPathInfo<float>(p, PointI(5, 15), path, getFont(), Argb32(0xFF000000));

    if (Math::isFinite(_nearestPoint.x))
    {
      p->setSource(Argb32(0xFF000000));
      p->fillCircle(CircleF(_nearestPoint, 4.0f));
    }
  }

  logger.paint(p, getFont(), PointF(5, 40), Argb32(0xFF0000FF));
  logger.clear();
}

// ============================================================================
// [MAIN]
// ============================================================================

FOG_UI_MAIN()
{
  /*
  {
    float f[4];
    int i;

    f[0] =-2.0f;
    f[1] =-1.1f;
    f[2] =-1.3f;
    f[3] = 0.5f;

    float rv[4];
    int rc = Math::solvePolynomial(rv, f, MATH_POLYNOMIAL_DEGREE_CUBIC);

    float qv[4];
    int qc = Math::solvePolynomialN(qv, f, 3, MATH_POLYNOMIAL_SOLVE_EIGEN);

    for (i = 0; i < rc; i++)
    {
      float r = rv[i];
      float e = ((f[0] * r + f[1]) * r + f[2]) * r + f[3];
      printf("R-Root: %f, evaluation: %f\n", r, e);
    }

    printf("\n");

    for (i = 0; i < qc; i++)
    {
      float r = qv[i];
      float e = ((f[0] * r + f[1]) * r + f[2]) * r + f[3];
      printf("Q-Root: %f, evaluation: %f\n", r, e);
    }
  }
  */

  Application app(StringW::fromAscii8("UI"));
  MyWindow window(WINDOW_TYPE_DEFAULT);

  window.addListener(EVENT_CLOSE, &app, &Application::quit);
  window.setSize(SizeI(400, 300));
  window.show();

  return app.run();
}
