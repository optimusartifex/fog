// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Global/Private.h>
#include <Fog/Core/Math/Constants.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/MemBufferTmp_p.h>
#include <Fog/G2d/Geometry/Math2d.h>
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/G2d/Geometry/PathClipper.h>
#include <Fog/G2d/Geometry/PathStroker.h>
#include <Fog/G2d/Geometry/PathTmp_p.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Transform.h>

namespace Fog {

// ============================================================================
// [Fog::PathStrokerContextT<> - Declaration]
// ============================================================================

template<typename NumT>
struct PathStrokerContextT
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE PathStrokerContextT(const NumT_(PathStroker)* stroker, NumT_(Path)* dst) :
    stroker(stroker),
    dst(dst),
    distances(NULL),
    distancesAlloc(0)
  {
    dstInitial = dst->getLength();
  }

  FOG_INLINE ~PathStrokerContextT()
  {
  }

  // --------------------------------------------------------------------------
  // [Stroke]
  // --------------------------------------------------------------------------

  err_t strokeShape(uint32_t shapeType, const void* shapeData);
  err_t strokePath(const NumT_(Path)* src);
  err_t strokePathPrivate(const NumT_(Path)* src);

  // --------------------------------------------------------------------------
  // [Prepare / Finalize]
  // --------------------------------------------------------------------------

  err_t _begin();
  err_t _grow();

  err_t calcArc(
    NumT x,   NumT y,
    NumT dx1, NumT dy1,
    NumT dx2, NumT dy2);

  err_t calcMiter(
    const NumT_(Point)& v0,
    const NumT_(Point)& v1,
    const NumT_(Point)& v2,
    NumT dx1, NumT dy1,
    NumT dx2, NumT dy2,
    int lineJoin,
    NumT mlimit,
    NumT dbevel);

  err_t calcCap(
    const NumT_(Point)& v0,
    const NumT_(Point)& v1,
    NumT len,
    uint32_t cap);

  err_t calcJoin(
    const NumT_(Point)& v0,
    const NumT_(Point)& v1,
    const NumT_(Point)& v2,
    NumT len1,
    NumT len2);

  err_t strokePathFigure(const NumT_(Point)* src, size_t count, bool outline);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  const NumT_(PathStroker)* stroker;

  NumT_(Path)* dst;
  size_t dstInitial;

  NumT_(Point)* dstCur;
  NumT_(Point)* dstEnd;
  // NumT_(Point)* pts;
  // uint8_t *cmd;
  // size_t remain;

  //! @brief Memory buffer used to store distances.
  MemBufferTmp<1024> buffer;

  NumT* distances;
  size_t distancesAlloc;
};

// ============================================================================
// [Fog::PathStrokerContextT<> - Stroke]
// ============================================================================

template<typename NumT>
err_t PathStrokerContextT<NumT>::strokeShape(uint32_t shapeType, const void* shapeData)
{
  NumT_T1(PathTmp, 32) tmp;
  tmp._shape(shapeType, shapeData, PATH_DIRECTION_CW, NULL);
  return strokePath(&tmp);
}

template<typename NumT>
err_t PathStrokerContextT<NumT>::strokePath(const NumT_(Path)* src)
{
  // We need:
  // - the Path instances have to be different.
  // - source path must be flat.
  if (dst == src || src->hasBeziers() || !stroker->_transform->isIdentity())
  {
    NumT_T1(PathTmp, 200) tmp;
    FOG_RETURN_ON_ERROR(NumI_(Path)::flatten(tmp, *src, stroker->_flatness));
    return strokePathPrivate(&tmp);
  }
  else
  {
    return strokePathPrivate(src);
  }
}

template<typename NumT>
err_t PathStrokerContextT<NumT>::strokePathPrivate(const NumT_(Path)* src)
{
  const uint8_t* commands = src->getCommands();
  const NumT_(Point)* vertices = src->getVertices();

  // Traverse path, find moveTo / lineTo segments and stroke them.
  const uint8_t* subCommand = NULL;
  const uint8_t* curCommand = commands;
  size_t remain = src->getLength();

  while (remain)
  {
    uint8_t cmd = curCommand[0];

    // LineTo is the most used command, so it's first.
    if (PathCmd::isLineTo(cmd))
    {
      // Nothing here...
    }
    else if (PathCmd::isMoveTo(cmd))
    {
      // Send path to stroker if there is something.
      if (FOG_LIKELY(subCommand != NULL && subCommand != curCommand))
      {
        size_t subStart = (size_t)(subCommand - commands);
        size_t subLength = (size_t)(curCommand - subCommand);

        FOG_RETURN_ON_ERROR(
          strokePathFigure(vertices + subStart, subLength, false)
        );
      }

      // Advance to new subpath.
      subCommand = curCommand;
    }
    else if (PathCmd::isClose(cmd))
    {
      // Send path to stroker if there is something.
      if (FOG_LIKELY(subCommand != NULL))
      {
        size_t subStart = (size_t)(subCommand - commands);
        size_t subLength = (size_t)(curCommand - subCommand);

        FOG_RETURN_ON_ERROR(
          strokePathFigure(vertices + subStart, subLength, subLength > 2)
        );
      }

      // We clear beginning mark, because we expect PATH_MOVE_TO command from now.
      subCommand = NULL;
    }

    curCommand++;
    remain--;
  }

  // Send path to stroker if there is something (this is the last, unclosed, figure)
  if (subCommand != NULL && subCommand != curCommand)
  {
    size_t subStart = (size_t)(subCommand - commands);
    size_t subLength = (size_t)(curCommand - subCommand);

    FOG_RETURN_ON_ERROR(
      strokePathFigure(vertices + subStart, subLength, false)
    );
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::PathStrokerContextT<> - Calc]
// ============================================================================

#define ADD_VERTEX(x, y) \
  FOG_MACRO_BEGIN \
    if (FOG_UNLIKELY(dstCur == dstEnd)) FOG_RETURN_ON_ERROR(_grow()); \
    \
    dstCur->set(x, y); \
    dstCur++; \
  FOG_MACRO_END

#define CUR_INDEX() \
  ( (size_t)(dstCur - dst->_d->vertices) )

template<typename NumT>
err_t PathStrokerContextT<NumT>::_begin()
{
  size_t cap = dst->getCapacity();
  size_t remain = cap - dst->getLength();

  if (remain < 64 || !dst->isDetached())
  {
    if (cap < 256)
      cap = 256;
    else
      cap *= 2;
    FOG_RETURN_ON_ERROR(dst->reserve(cap));
  }

  dstCur = const_cast<NumT_(Point)*>(dst->getVertices());
  dstEnd = dstCur;

  dstCur += dst->getLength();
  dstEnd += dst->getCapacity();

  return ERR_OK;
}

template<typename NumT>
err_t PathStrokerContextT<NumT>::_grow()
{
  size_t len = dst->_d->length;
  size_t cap = dst->_d->capacity;

  dst->_d->length = cap;
  if (cap < 256)
    cap = 512;
  else
    cap *= 2;

  err_t err = dst->reserve(cap);
  if (FOG_IS_ERROR(err))
    return err;

  dstCur = const_cast<NumT_(Point)*>(dst->getVertices());
  dstEnd = dstCur;

  dstCur += dst->getLength();
  dstEnd += dst->getCapacity();

  return ERR_OK;
};

#if 0
err_t PathStrokerContextT<NumT>::calcArc(
  NumT x,   NumT y,
  NumT dx1, NumT dy1,
  NumT dx2, NumT dy2)
{
  NumT a1 = Math::atan2(dy1 * stroker->_wSign, dx1 * stroker->_wSign);
  NumT a2 = Math::atan2(dy2 * stroker->_wSign, dx2 * stroker->_wSign);
  NumT da = stroker->_da;
  int i, n;

  ADD_VERTEX(x + dx1, y + dy1);
  if (stroker->_wSign > 0)
  {
    if (a1 > a2) a2 += MATH_TWO_PI;
    n = int((a2 - a1) / da);
    da = (a2 - a1) / (n + 1);
    a1 += da;

    for (i = 0; i < n; i++)
    {
      NumT a1Sin, a1Cos;
      Math::sincos(a1, &a1Sin, &a1Cos);

      ADD_VERTEX(x + a1Cos * stroker->_w, y + a1Sin * stroker->_w);
      a1 += da;
    }
  }
  else
  {
    if (a1 < a2) a2 -= MATH_TWO_PI;
    n = int((a1 - a2) / da);
    da = (a1 - a2) / (n + 1);
    a1 -= da;

    for (i = 0; i < n; i++)
    {
      NumT a1Sin, a1Cos;
      Math::sincos(a1, &a1Sin, &a1Cos);

      ADD_VERTEX(x + a1Cos * stroker->_w, y + a1Sin * stroker->_w);
      a1 -= da;
    }
  }

  ADD_VERTEX(x + dx2, y + dy2);
  return ERR_OK;
}
#endif

template<typename NumT>
err_t PathStrokerContextT<NumT>::calcArc(
  NumT x,   NumT y,
  NumT dx1, NumT dy1,
  NumT dx2, NumT dy2)
{
  NumT a1, a2;
  NumT da = stroker->_da;
  int i, n;

  ADD_VERTEX(x + dx1, y + dy1);
  if (stroker->_wSign > 0)
  {
    a1 = Math::atan2(dy1, dx1);
    a2 = Math::atan2(dy2, dx2);
    if (a1 > a2) a2 += NumT(MATH_TWO_PI);
    n = int((a2 - a1) / da);
  }
  else
  {
    a1 = Math::atan2(-dy1, -dx1);
    a2 = Math::atan2(-dy2, -dx2);
    if (a1 < a2) a2 -= NumT(MATH_TWO_PI);
    n = int((a1 - a2) / da);
  }

  da = (a2 - a1) / (n + 1);
  a1 += da;

  for (i = 0; i < n; i++)
  {
    NumT a1Sin, a1Cos;
    Math::sincos(a1, &a1Sin, &a1Cos);

    ADD_VERTEX(x + a1Cos * stroker->_w, y + a1Sin * stroker->_w);
    a1 += da;
  }

  ADD_VERTEX(x + dx2, y + dy2);
  return ERR_OK;
}

template<typename NumT>
err_t PathStrokerContextT<NumT>::calcMiter(
  const NumT_(Point)& v0,
  const NumT_(Point)& v1,
  const NumT_(Point)& v2,
  NumT dx1, NumT dy1,
  NumT dx2, NumT dy2,
  int lineJoin,
  NumT mlimit,
  NumT dbevel)
{
  NumT_(Point) pi(v1.x, v1.y);
  NumT di = NumT(1);
  NumT lim = stroker->_wAbs * mlimit;
  bool intersectionFailed  = true; // Assume the worst

  if (Math2d::intersectLine(pi,
    NumT_(Point)(v0.x + dx1, v0.y - dy1),
    NumT_(Point)(v1.x + dx1, v1.y - dy1),
    NumT_(Point)(v1.x + dx2, v1.y - dy2),
    NumT_(Point)(v2.x + dx2, v2.y - dy2)))
  {
    // Calculation of the intersection succeeded.
    di = Math::dist(v1.x, v1.y, pi.x, pi.y);
    if (di <= lim)
    {
      // Inside the miter limit.
      ADD_VERTEX(pi.x, pi.y);
      return ERR_OK;
    }
    intersectionFailed = false;
  }
  else
  {
    // Calculation of the intersection failed, most probably the three points
    // lie one straight line. First check if v0 and v2 lie on the opposite
    // sides of vector: (v1.x, v1.y) -> (v1.x+dx1, v1.y-dy1), that is, the
    // perpendicular to the line determined by vertices v0 and v1.
    //
    // This condition determines whether the next line segments continues
    // the previous one or goes back.
    NumT_(Point) vt(v1.x + dx1, v1.y - dy1);

    if ((Math2d::crossProduct(v0, v1, vt) < 0.0) ==
        (Math2d::crossProduct(v1, v2, vt) < 0.0))
    {
      // This case means that the next segment continues the previous one
      // (straight line).
      ADD_VERTEX(v1.x + dx1, v1.y - dy1);
      return ERR_OK;
    }
  }

  // Miter limit exceeded.
  switch (lineJoin)
  {
    case LINE_JOIN_MITER_REVERT:
      // For the compatibility with SVG, PDF, etc, we use a simple bevel
      // join instead of "smart" bevel.
      ADD_VERTEX(v1.x + dx1, v1.y - dy1);
      ADD_VERTEX(v1.x + dx2, v1.y - dy2);
      break;

    case LINE_JOIN_MITER_ROUND:
      FOG_RETURN_ON_ERROR( calcArc(v1.x, v1.y, dx1, -dy1, dx2, -dy2) );
      break;

    default:
      // If no miter-revert, calculate new dx1, dy1, dx2, dy2.
      if (intersectionFailed)
      {
        mlimit *= stroker->_wSign;
        ADD_VERTEX(v1.x + dx1 + dy1 * mlimit, v1.y - dy1 + dx1 * mlimit);
        ADD_VERTEX(v1.x + dx2 - dy2 * mlimit, v1.y - dy2 - dx2 * mlimit);
      }
      else
      {
        NumT x1 = v1.x + dx1;
        NumT y1 = v1.y - dy1;
        NumT x2 = v1.x + dx2;
        NumT y2 = v1.y - dy2;
        di = (lim - dbevel) / (di - dbevel);
        ADD_VERTEX(x1 + (pi.x - x1) * di, y1 + (pi.y - y1) * di);
        ADD_VERTEX(x2 + (pi.x - x2) * di, y2 + (pi.y - y2) * di);
      }
      break;
  }

  return ERR_OK;
}

template<typename NumT>
err_t PathStrokerContextT<NumT>::calcCap(
  const NumT_(Point)& v0,
  const NumT_(Point)& v1,
  NumT len,
  uint32_t cap)
{
  NumT ilen = NumT(1.0) / len;

  NumT dx1 = (v1.y - v0.y) * ilen;
  NumT dy1 = (v1.x - v0.x) * ilen;

  dx1 *= stroker->_w;
  dy1 *= stroker->_w;

  switch (cap)
  {
    case LINE_CAP_BUTT:
    {
      ADD_VERTEX(v0.x - dx1, v0.y + dy1);
      ADD_VERTEX(v0.x + dx1, v0.y - dy1);
      break;
    }

    case LINE_CAP_SQUARE:
    {
      NumT dx2 = dy1 * stroker->_wSign;
      NumT dy2 = dx1 * stroker->_wSign;

      ADD_VERTEX(v0.x - dx1 - dx2, v0.y + dy1 - dy2);
      ADD_VERTEX(v0.x + dx1 - dx2, v0.y - dy1 - dy2);
      break;
    }

    case LINE_CAP_ROUND:
    {
      int i;
      int n = int(MATH_PI / stroker->_da);
      NumT da = NumT(MATH_PI) / NumT(n + 1);
      NumT a1;

      ADD_VERTEX(v0.x - dx1, v0.y + dy1);

      if (stroker->_wSign > 0)
      {
        a1 = Math::atan2(dy1, -dx1) + da;
      }
      else
      {
        da = -da;
        a1 = Math::atan2(-dy1, dx1) + da;
      }

      for (i = 0; i < n; i++)
      {
        NumT a1_sin;
        NumT a1_cos;
        Math::sincos(a1, &a1_sin, &a1_cos);

        ADD_VERTEX(v0.x + a1_cos * stroker->_w, v0.y + a1_sin * stroker->_w);
        a1 += da;
      }

      ADD_VERTEX(v0.x + dx1, v0.y - dy1);
      break;
    }

    case LINE_CAP_ROUND_REVERSE:
    {
      int i;
      int n = int(MATH_PI / stroker->_da);
      NumT da = NumT(MATH_PI) / NumT(n + 1);
      NumT a1;

      NumT dx2 = dy1 * stroker->_wSign;
      NumT dy2 = dx1 * stroker->_wSign;

      NumT vx = v0.x - dx2;
      NumT vy = v0.y - dy2;

      ADD_VERTEX(vx - dx1, vy + dy1);

      if (stroker->_wSign > 0)
      {
        da = -da;
        a1 = Math::atan2(dy1, -dx1) + da;
      }
      else
      {
        a1 = Math::atan2(-dy1, dx1) + da;
      }

      for (i = 0; i < n; i++)
      {
        NumT a1_sin;
        NumT a1_cos;
        Math::sincos(a1, &a1_sin, &a1_cos);

        ADD_VERTEX(vx + a1_cos * stroker->_w, vy + a1_sin * stroker->_w);
        a1 += da;
      }

      ADD_VERTEX(vx + dx1, vy - dy1);
      break;
    }

    case LINE_CAP_TRIANGLE:
    {
      NumT dx2 = dy1 * stroker->_wSign;
      NumT dy2 = dx1 * stroker->_wSign;

      ADD_VERTEX(v0.x - dx1, v0.y + dy1);
      ADD_VERTEX(v0.x - dx2, v0.y - dy2);
      ADD_VERTEX(v0.x + dx1, v0.y - dy1);
      break;
    }

    case LINE_CAP_TRIANGLE_REVERSE:
    {
      NumT dx2 = dy1 * stroker->_wSign;
      NumT dy2 = dx1 * stroker->_wSign;

      ADD_VERTEX(v0.x - dx1 - dx2, v0.y + dy1 - dy2);
      ADD_VERTEX(v0.x, v0.y);
      ADD_VERTEX(v0.x + dx1 - dx2, v0.y - dy1 - dy2);
    }
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::INNER_JOIN]
// ============================================================================

template<typename NumT>
err_t PathStrokerContextT<NumT>::calcJoin(
  const NumT_(Point)& v0,
  const NumT_(Point)& v1,
  const NumT_(Point)& v2,
  NumT len1,
  NumT len2)
{
  NumT wilen1 = (stroker->_w / len1);
  NumT wilen2 = (stroker->_w / len2);

  NumT dx1 = (v1.y - v0.y) * wilen1;
  NumT dy1 = (v1.x - v0.x) * wilen1;
  NumT dx2 = (v2.y - v1.y) * wilen2;
  NumT dy2 = (v2.x - v1.x) * wilen2;

  NumT cp = Math2d::crossProduct(v0, v1, v2);

  if (cp != 0 && (cp > 0) == (stroker->_w > 0))
  {
    ADD_VERTEX(v1.x + dx1, v1.y - dy1);
    ADD_VERTEX(v1.x + dx2, v1.y - dy2);
  }
  else
  {
    // Outer join

    // Calculate the distance between v1 and the central point of the bevel
    // line segment.
    NumT dx = (dx1 + dx2) / 2;
    NumT dy = (dy1 + dy2) / 2;
    NumT dbevel = Math::sqrt(dx * dx + dy * dy);
    /*
    if (stroker->_params->getLineJoin() == LINE_JOIN_ROUND ||
        stroker->_params->getLineJoin() == LINE_JOIN_BEVEL)
    {
      // This is an optimization that reduces the number of points
      // in cases of almost collinear segments. If there's no
      // visible difference between bevel and miter joins we'd rather
      // use miter join because it adds only one point instead of two.
      //
      // Here we calculate the middle point between the bevel points
      // and then, the distance between v1 and this middle point.
      // At outer joins this distance always less than stroke width,
      // because it's actually the height of an isosceles triangle of
      // v1 and its two bevel points. If the difference between this
      // width and this value is small (no visible bevel) we can
      // add just one point.
      //
      // The constant in the expression makes the result approximately
      // the same as in round joins and caps. You can safely comment
      // out this entire "if".
      // TODO: ApproxScale used
      if (stroker->_flatness * (stroker->_wAbs - dbevel) < stroker->_wEps)
      {
        NumT_(Point) pi;
        if (Math2d::intersectLine(pi,
          NumT_(Point)(v0.x + dx1, v0.y - dy1),
          NumT_(Point)(v1.x + dx1, v1.y - dy1),
          NumT_(Point)(v1.x + dx2, v1.y - dy2),
          NumT_(Point)(v2.x + dx2, v2.y - dy2)))
        {
          ADD_VERTEX(pi.x, pi.y);
        }
        else
        {
          ADD_VERTEX(v1.x + dx1, v1.y - dy1);
        }
        return ERR_OK;
      }
    }
    */

    switch (stroker->_params->getLineJoin())
    {
      case LINE_JOIN_MITER:
      case LINE_JOIN_MITER_REVERT:
      case LINE_JOIN_MITER_ROUND:
        FOG_RETURN_ON_ERROR(
          calcMiter(v0, v1, v2, dx1, dy1, dx2, dy2,
            stroker->_params->getLineJoin(),
            stroker->_params->getMiterLimit(), dbevel)
        );
        break;

      case LINE_JOIN_ROUND:
        FOG_RETURN_ON_ERROR(
          calcArc(v1.x, v1.y, dx1, -dy1, dx2, -dy2)
        );
        break;

      case LINE_JOIN_BEVEL:
        ADD_VERTEX(v1.x + dx1, v1.y - dy1);
        ADD_VERTEX(v1.x + dx2, v1.y - dy2);
        break;

      default:
        FOG_ASSERT_NOT_REACHED();
    }
  }

  return ERR_OK;
}

template<typename NumT>
err_t PathStrokerContextT<NumT>::strokePathFigure(const NumT_(Point)* src, size_t count, bool outline)
{
  // Can't stroke one-vertex array.
  if (count <= 1)
    return ERR_OK;
    //return ERR_GEOMETRY_CANT_STROKE;

  // To do outline we need at least three vertices.
  if (outline && count <= 2)
    return ERR_OK;
    // return ERR_GEOMETRY_CANT_STROKE;

  const NumT_(Point)* cur;
  size_t i;
  size_t moveToPosition0 = dst->getLength();
  size_t moveToPosition1 = INVALID_INDEX;

  FOG_RETURN_ON_ERROR(
    _begin()
  );

  // Alloc or realloc array for our distances (distance between individual
  // vertices [0]->[1], [1]->[2], ...). Distance at index[0] means distance
  // between src[0] and src[1], etc. Last distance is special and it is 0.0
  // if path is not closed, otherwise src[count-1]->src[0].
  if (distancesAlloc < count)
  {
    // Need to realloc, we align count to 128 vertices.
    if (distances) buffer.reset();

    distancesAlloc = (count + 127) & ~127;
    distances = reinterpret_cast<NumT*>(buffer.alloc(distancesAlloc * sizeof(NumT)));

    if (distances == NULL)
    {
      distancesAlloc = 0;
      return ERR_RT_OUT_OF_MEMORY;
    }
  }

  NumT *dist;

  for (i = 0; i < count - 1; i++)
  {
    NumT d = Math::dist(src[i].x, src[i].y, src[i + 1].x, src[i + 1].y);
    if (d <= MathConstant<NumT>::getDistanceEpsilon()) d = NumT(0.0);

    distances[i] = d;
  }

/*
  for (i = count - 1, cur = src, dist = distances; i; i--, cur++, dist++)
  {
    NumT d = Math::dist(cur[0].x, cur[0].y, cur[1].x, cur[1].y);
    if (d <= MathConstant<NumT>::getDistanceEpsilon()) d = 0.0;

    dist[0] = d;
  }
*/
  const NumT_(Point)* srcEnd = src + count;

  NumT_(Point) cp[3]; // Current points.
  NumT cd[3];         // Current distances.

  // If something went wrong.
  // for (i = 0; i < count; i++)
  // {
  //  Debug::dbgFormat("Vertex [%g, %g]", src[i].x, src[i].y);
  // }

  // --------------------------------------------------------------------------
  // [Outline]
  // --------------------------------------------------------------------------

#define IS_DEGENERATED_DIST(__dist__) ((__dist__) <= MathConstant<NumT>::getDistanceEpsilon())

  if (outline)
  {
    // We need also to calc distance between first and last point.
    {
      NumT d = Math::dist(src[count - 1].x, src[count - 1].y, src[0].x, src[0].y);
      if (d <= MathConstant<NumT>::getDistanceEpsilon()) d = NumT(0.0);

      distances[count - 1] = d;
    }

    NumT_(Point) fp[2]; // First points.
    NumT fd[2]; // First distances.

    // ------------------------------------------------------------------------
    // [Outline 1]
    // ------------------------------------------------------------------------

    cur = src;
    dist = distances;

    // Fill the current points / distances and skip degenerate cases.
    i = 0;

    // FirstI is first point. When first and last points are equal, we need to
    // mask the second one as first.
    //
    // Examine two paths:
    //
    // - [100, 100] -> [300, 300] -> [100, 300] -> CLOSE
    //
    //   The firstI will be zero in that case. Path is self-closing, but the
    //   close end-point is not shared with first vertex.
    //
    // - [100, 100] -> [300, 300] -> [100, 300] -> [100, 100] -> CLOSE
    //
    //   The firstI will be one. Path is self-closing, but unfortunatelly the
    //   closing point vertex is already there (fourth command).
    //
    size_t firstI = i;

    cp[i] = src[count - 1];
    cd[i] = dist[count - 1];
    if (FOG_LIKELY(!IS_DEGENERATED_DIST(cd[i]))) { i++; firstI++; }

    do {
      if (FOG_UNLIKELY(cur == srcEnd))
        return ERR_OK;
        //return ERR_GEOMETRY_CANT_STROKE;

      cp[i] = *cur++;
      cd[i] = *dist++;
      if (FOG_LIKELY(!IS_DEGENERATED_DIST(cd[i]))) i++;
    } while (i < 3);

    // Save two first points and distances (we need them to finish the outline).
    fp[0] = cp[firstI];
    fd[0] = cd[firstI];

    fp[1] = cp[firstI + 1];
    fd[1] = cd[firstI + 1];

    // Make the outline.
    for (;;)
    {
      calcJoin(cp[0], cp[1], cp[2], cd[0], cd[1]);

      if (cur == srcEnd) goto _Outline1Done;
      while (FOG_UNLIKELY(IS_DEGENERATED_DIST(dist[0])))
      {
        dist++;
        if (++cur == srcEnd) goto _Outline1Done;
      }

      cp[0] = cp[1];
      cd[0] = cd[1];

      cp[1] = cp[2];
      cd[1] = cd[2];

      cp[2] = *cur++;
      cd[2] = *dist++;
    }

    // End joins.
_Outline1Done:
    FOG_RETURN_ON_ERROR( calcJoin(cp[1], cp[2], fp[0], cd[1], cd[2]) );
    FOG_RETURN_ON_ERROR( calcJoin(cp[2], fp[0], fp[1], cd[2], fd[0]) );

    // Close path (CW).
    ADD_VERTEX(Math::getQNanT<NumT>(), Math::getQNanT<NumT>());

    // ------------------------------------------------------------------------
    // [Outline 2]
    // ------------------------------------------------------------------------

    moveToPosition1 = CUR_INDEX();

    cur = src + count;
    dist = distances + count;

    // Fill the current points / distances and skip degenerate cases.
    i = 0;
    firstI = 0;
    cp[i] = src[0];
    cd[i] = distances[0];
    if (cd[i] != NumT(0.0)) { i++; firstI++; }

    do {
      cp[i] = *--cur;
      cd[i] = *--dist;
      if (FOG_LIKELY(cd[i] != NumT(0.0))) i++;
    } while (i < 3);

    // Save two first points and distances (we need them to finish the outline).
    fp[0] = cp[firstI];
    fd[0] = cd[firstI];

    fp[1] = cp[firstI + 1];
    fd[1] = cd[firstI + 1];

    // Make the outline.
    for (;;)
    {
      calcJoin(cp[0], cp[1], cp[2], cd[1], cd[2]);

      do {
        if (cur == src) goto _Outline2Done;
        cur--;
        dist--;
      } while (FOG_UNLIKELY(dist[0] == NumT(0.0)));

      cp[0] = cp[1];
      cd[0] = cd[1];

      cp[1] = cp[2];
      cd[1] = cd[2];

      cp[2] = *cur;
      cd[2] = *dist;
    }

    // End joins.
_Outline2Done:
    FOG_RETURN_ON_ERROR( calcJoin(cp[1], cp[2], fp[0], cd[2], fd[0]) );
    FOG_RETURN_ON_ERROR( calcJoin(cp[2], fp[0], fp[1], fd[0], fd[1]) );

    // Close path (CCW).
    ADD_VERTEX(Math::getQNanT<NumT>(), Math::getQNanT<NumT>());
  }

  // --------------------------------------------------------------------------
  // [Pen]
  // --------------------------------------------------------------------------

  else
  {
    distances[count - 1] = distances[count - 2];

    // ------------------------------------------------------------------------
    // [Outline 1]
    // ------------------------------------------------------------------------

    cur = src;
    dist = distances;

    // Fill the current points / distances and skip degenerate cases.
    i = 0;

    do {
      if (FOG_UNLIKELY(cur == srcEnd))
        return ERR_OK;
        //return ERR_GEOMETRY_CANT_STROKE;

      cp[i] = *cur++;
      cd[i] = *dist++;
      if (FOG_LIKELY(cd[i] != NumT(0.0))) i++;
    } while (i < 2);

    // Start cap.
    FOG_RETURN_ON_ERROR( calcCap(cp[0], cp[1], cd[0], stroker->_params->getStartCap()) );

    // Make the outline.
    if (cur == srcEnd) goto _Pen1Done;
    while (FOG_UNLIKELY(dist[0] == NumT(0.0)))
    {
      dist++;
      if (++cur == srcEnd) goto _Pen1Done;
    }

    goto _Pen1Loop;

    for (;;)
    {
      cp[0] = cp[1];
      cd[0] = cd[1];

      cp[1] = cp[2];
      cd[1] = cd[2];

_Pen1Loop:
      cp[2] = *cur++;
      cd[2] = *dist++;

      FOG_RETURN_ON_ERROR( calcJoin(cp[0], cp[1], cp[2], cd[0], cd[1]) );
      if (cur == srcEnd) goto _Pen1Done;

      while (FOG_UNLIKELY(dist[0] == NumT(0.0)))
      {
        dist++;
        if (++cur == srcEnd) goto _Pen1Done;
      }
    }

    // End joins.
_Pen1Done:

    // ------------------------------------------------------------------------
    // [Outline 2]
    // ------------------------------------------------------------------------

    // Fill the current points / distances and skip degenerate cases.
    i = 0;

    do {
      cp[i] = *--cur;
      cd[i] = *--dist;
      if (FOG_LIKELY(cd[i] != NumT(0.0))) i++;
    } while (i < 2);

    // End cap.
    FOG_RETURN_ON_ERROR( calcCap(cp[0], cp[1], cd[1], stroker->_params->getEndCap()) );

    // Make the outline.
    if (cur == src) goto _Pen2Done;

    cur--;
    dist--;

    while (FOG_UNLIKELY(dist[0] == NumT(0.0)))
    {
      if (cur == src) goto _Pen2Done;

      dist--;
      cur--;
    }

    goto _Pen2Loop;

    for (;;)
    {
      do {
        if (cur == src) goto _Pen2Done;

        cur--;
        dist--;
      } while (FOG_UNLIKELY(dist[0] == NumT(0.0)));

      cp[0] = cp[1];
      cd[0] = cd[1];

      cp[1] = cp[2];
      cd[1] = cd[2];

_Pen2Loop:
      cp[2] = *cur;
      cd[2] = *dist;

      FOG_RETURN_ON_ERROR( calcJoin(cp[0], cp[1], cp[2], cd[1], cd[2]) );
    }
_Pen2Done:
    // Close path (CCW).
    ADD_VERTEX(Math::getQNanT<NumT>(), Math::getQNanT<NumT>());
  }

  {
    // Fix the length of the path.
    size_t finalLength = CUR_INDEX();
    dst->_d->length = finalLength;
    FOG_ASSERT(finalLength <= dst->_d->capacity);

    // Fix path adding PATH_CMD_MOVE_TO/CLOSE commands at begin of each
    // outline and filling rest by PATH_CMD_LINE_TO. This allowed us to
    // simplify ADD_VERTEX() macro.
    uint8_t* dstCommands = const_cast<uint8_t*>(dst->getCommands());

    // Close clockwise path.
    if (moveToPosition0 < finalLength)
    {
      memset(dstCommands + moveToPosition0, (unsigned int)(PATH_CMD_LINE_TO), finalLength - moveToPosition0);
      dstCommands[moveToPosition0] = PATH_CMD_MOVE_TO;
      dstCommands[finalLength - 1] = PATH_CMD_CLOSE;
    }

    // Close counter-clockwise path.
    if (moveToPosition1 < finalLength)
    {
      dstCommands[moveToPosition1 - 1] = PATH_CMD_CLOSE;
      dstCommands[moveToPosition1    ] = PATH_CMD_MOVE_TO;
    }
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::PathStroker - Construction / Destruction]
// ============================================================================

template<typename NumT>
static void FOG_CDECL PathStrokerT_ctor(NumT_(PathStroker)* self)
{
  self->_params.init();
  self->_transform.init();
  self->_clipBox.reset();
  self->_transformedClipBox.reset();
  self->_w = NumT(0);
  self->_wAbs = NumT(0);
  self->_wEps = NumT(0);
  self->_da = NumT(0);
  self->_flatness = MathConstant<NumT>::getDefaultFlatness();
  self->_wSign = 1;
  self->_isDirty = true;
  self->_isClippingEnabled = false;
  self->_isTransformSimple = true;
  self->_flattenType = PATH_FLATTEN_DISABLED;
}

template<typename NumT>
static void FOG_CDECL PathStrokerT_ctorCopy(NumT_(PathStroker)* self, const NumT_(PathStroker)* other)
{
  self->_params.initCustom1(other->_params);
  self->_transform.initCustom1(other->_transform);
  self->_clipBox = other->_clipBox;
  self->_transformedClipBox = other->_transformedClipBox;
  self->_w = other->_w;
  self->_wAbs = other->_wAbs;
  self->_wEps = other->_wEps;
  self->_da = other->_da;
  self->_flatness = other->_flatness;
  self->_wSign = other->_wSign;
  self->_isDirty = other->_isDirty;
  self->_isClippingEnabled = other->_isClippingEnabled;
  self->_isTransformSimple = other->_isTransformSimple;
  self->_flattenType = other->_flattenType;
}

template<typename NumT>
static void FOG_CDECL PathStrokerT_ctorParams(NumT_(PathStroker)* self,
  const NumT_(PathStrokerParams)* params, const NumT_(Transform)* tr, const NumT_(Box)* clipBox)
{
  if (params != NULL)
    self->_params.initCustom1(*params);
  else
    self->_params.init();

  if (tr != NULL)
    self->_transform.initCustom1(*tr);
  else
    self->_transform.init();

  if (clipBox != NULL)
    self->_clipBox = *clipBox;
  else
    self->_clipBox.reset();

  self->_transformedClipBox.reset();
  self->_w = NumT(0);
  self->_wAbs = NumT(0);
  self->_wEps = NumT(0);
  self->_da = NumT(0);
  self->_flatness = MathConstant<NumT>::getDefaultFlatness();
  self->_wSign = 1;
  self->_isDirty = true;
  self->_isClippingEnabled = false;
  self->_isTransformSimple = true;
  self->_flattenType = PATH_FLATTEN_DISABLED;
}

template<typename NumT>
static void FOG_CDECL PathStrokerT_dtor(NumT_(PathStroker)* self)
{
  self->_params.destroy();
}

// ============================================================================
// [Fog::PathStroker - Params]
// ============================================================================

template<typename NumT>
static void FOG_CDECL PathStrokerT_setParams(NumT_(PathStroker)* self, const NumT_(PathStrokerParams)* params)
{
  self->_params() = *params;
  self->_isDirty = true;
}

template<typename NumT>
static void FOG_CDECL PathStrokerT_setOther(NumT_(PathStroker)* self, const NumT_(PathStroker)* other)
{
  self->_params() = other->_params();
  self->_transform() = other->_transform();
  self->_clipBox = other->_clipBox;
  self->_transformedClipBox = other->_transformedClipBox;
  self->_w = other->_w;
  self->_wAbs = other->_wAbs;
  self->_wEps = other->_wEps;
  self->_da = other->_da;
  self->_flatness = other->_flatness;
  self->_wSign = other->_wSign;
  self->_isDirty = other->_isDirty;
  self->_isClippingEnabled = other->_isClippingEnabled;
  self->_isTransformSimple = other->_isTransformSimple;
  self->_flattenType = other->_flattenType;
}

// ============================================================================
// [Fog::PathStroker - Stroker]
// ============================================================================

template<typename NumT>
static err_t PathStrokerT_postProcess(const NumT_(PathStroker)* self, NumT_(Path)* dst)
{
  const NumT_(Transform)& tr = self->getTransform();

  if (self->isClippingEnabled())
  {
    NumT_(PathClipper) clipper(self->_clipBox);

    if (tr._getType() != TRANSFORM_TYPE_IDENTITY)
    {
      NumT_T1(PathTmp, 128) tmp;

      FOG_RETURN_ON_ERROR(tr.mapPath(tmp, *dst));
      dst->clear();
      FOG_RETURN_ON_ERROR(clipper.clipPath(*dst, tmp));

      return ERR_OK;
    }
    else
    {
      switch (clipper.measurePath(*dst))
      {
        case PATH_CLIPPER_MEASURE_BOUNDED:
        {
          return ERR_OK;
        }

        case PATH_CLIPPER_MEASURE_UNBOUNDED:
        {
          NumT_(Path) tmp;
          FOG_RETURN_ON_ERROR(clipper.continuePath(tmp, *dst));
          *dst = tmp;
          return ERR_OK;
        }

        default:
        {
          dst->clear();
          return ERR_GEOMETRY_INVALID;
        }
      }
    }
  }
  else
  {
    if (tr._getType() != TRANSFORM_TYPE_IDENTITY)
      return tr.mapPath(*dst, *dst);
    else
      return ERR_OK;
  }
}

template<typename NumT>
static err_t FOG_CDECL PathStrokerT_strokeShape(const NumT_(PathStroker)* self, NumT_(Path)* dst, uint32_t shapeType, const void* shapeData)
{
  self->update();

  {
    PathStrokerContextT<NumT> ctx(self, dst);
    FOG_RETURN_ON_ERROR(ctx.strokeShape(shapeType, shapeData));
  }

  return PathStrokerT_postProcess<NumT>(self, dst);
}

template<typename NumT>
static err_t FOG_CDECL PathStrokerT_strokePath(const NumT_(PathStroker)* self, NumT_(Path)* dst, const NumT_(Path)* src)
{
  self->update();

  {
    PathStrokerContextT<NumT> ctx(self, dst);
    FOG_RETURN_ON_ERROR(ctx.strokePath(src));
  }

  return PathStrokerT_postProcess<NumT>(self, dst);
}

// ============================================================================
// [Fog::PathStroker - Update]
// ============================================================================

template<typename NumT>
static void FOG_CDECL PathStrokerT_update(NumT_(PathStroker)* self)
{
  self->_w = self->_params->_lineWidth * NumT(0.5);
  self->_wAbs = self->_w;
  self->_wSign = 1;

  if (self->_w < NumT(0.0))
  {
    self->_wAbs  = -self->_w;
    self->_wSign = -1;
  }

  self->_wEps = self->_w / NumT(1024.0);
  self->_da = Math::acos(self->_wAbs / (self->_wAbs + NumT(0.125) * self->_flatness)) * NumT(2.0);

  self->_isDirty = false;
  self->_isTransformSimple = self->_transform->getType() == TRANSFORM_TYPE_IDENTITY;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void PathStroker_init(void)
{
  _api.pathstrokerf_ctor = PathStrokerT_ctor<float>;
  _api.pathstrokerf_ctorCopy = PathStrokerT_ctorCopy<float>;
  _api.pathstrokerf_ctorParams = PathStrokerT_ctorParams<float>;
  _api.pathstrokerf_dtor = PathStrokerT_dtor<float>;
  _api.pathstrokerf_setParams = PathStrokerT_setParams<float>;
  _api.pathstrokerf_setOther = PathStrokerT_setOther<float>;
  _api.pathstrokerf_strokeShape = PathStrokerT_strokeShape<float>;
  _api.pathstrokerf_strokePath = PathStrokerT_strokePath<float>;
  _api.pathstrokerf_update = PathStrokerT_update<float>;

  _api.pathstrokerd_ctor = PathStrokerT_ctor<double>;
  _api.pathstrokerd_ctorCopy = PathStrokerT_ctorCopy<double>;
  _api.pathstrokerd_ctorParams = PathStrokerT_ctorParams<double>;
  _api.pathstrokerd_dtor = PathStrokerT_dtor<double>;
  _api.pathstrokerd_setParams = PathStrokerT_setParams<double>;
  _api.pathstrokerd_setOther = PathStrokerT_setOther<double>;
  _api.pathstrokerd_strokeShape = PathStrokerT_strokeShape<double>;
  _api.pathstrokerd_strokePath = PathStrokerT_strokePath<double>;
  _api.pathstrokerd_update = PathStrokerT_update<double>;
}

} // Fog namespace
