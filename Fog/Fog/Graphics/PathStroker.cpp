// [Fog-Graphics]
//
// [License]
// MIT, See COPYING file in package

//----------------------------------------------------------------------------
// Anti-Grain Geometry - Version 2.4
// Copyright (C) 2002-2005 Maxim Shemanarev (http://www.antigrain.com)
//
// Permission to copy, use, modify, sell and distribute this software
// is granted provided this copyright notice appears in all copies.
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//----------------------------------------------------------------------------
// Contact: mcseem@antigrain.com
//          mcseemagg@yahoo.com
//          http://www.antigrain.com
//----------------------------------------------------------------------------

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Assert.h>
#include <Fog/Core/Constants.h>
#include <Fog/Core/Math.h>
#include <Fog/Core/Memory.h>
#include <Fog/Core/Misc.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Curve_p.h>
#include <Fog/Graphics/Path.h>
#include <Fog/Graphics/PathStroker.h>
#include <Fog/Graphics/Transform.h>

namespace Fog {

// ============================================================================
// [Fog::StrokerPrivate]
// ============================================================================

struct FOG_HIDDEN PathStrokerPrivate
{
  FOG_INLINE PathStrokerPrivate(const PathStroker& stroker, PathD& dst) :
    stroker(stroker),
    dst(dst),
    distances(NULL),
    distancesAlloc(0)
  {
  }

  FOG_INLINE ~PathStrokerPrivate()
  {
  }

  err_t _begin();
  err_t _grow();

  err_t calcCap(
    const PointD& v0,
    const PointD& v1,
    double len,
    uint32_t cap);

  err_t calcJoin(
    const PointD& v0,
    const PointD& v1,
    const PointD& v2,
    double len1,
    double len2);

  err_t calcArc(
    double x, double y,
    double dx1, double dy1,
    double dx2, double dy2);

  err_t calcMiter(
    const PointD& v0,
    const PointD& v1,
    const PointD& v2,
    double dx1, double dy1,
    double dx2, double dy2,
    int lineJoin,
    double mlimit,
    double dbevel);

  err_t stroke(const PointD* src, sysuint_t count, bool outline);

  const PathStroker& stroker;

  PathD& dst;
  PointD* dstCur;
  PointD* dstEnd;

  //! @brief Memory buffer used to store distances.
  LocalBuffer<1024> buffer;
  double* distances;
  sysuint_t distancesAlloc;
};

// ============================================================================
// [Fog::Stroker - Construction / Destruction]
// ============================================================================

PathStroker::PathStroker() :
  _approximationScale(1.0)
{
  _update();
}

PathStroker::PathStroker(const PathStrokeParams& params, double approximationScale) :
  _params(params),
  _approximationScale(approximationScale)
{
  _update();
}

PathStroker::~PathStroker()
{
}

void PathStroker::setParams(const PathStrokeParams& params)
{
  _params = params;
  _update();
}

void PathStroker::setApproximationScale(double approximationScale)
{
  _approximationScale = approximationScale;
  _update();
}

void PathStroker::_update()
{
  _w = _params._lineWidth * 0.5;

  if (_w < 0.0)
  {
    _wAbs  = -_w;
    _wSign = -1;
  }
  else
  {
    _wAbs  = _w;
    _wSign = 1;
  }

  _wEps = _w / 1024.0;
  _da = Math::acos(_wAbs / (_wAbs + 0.125 / _approximationScale)) * 2.0;
}

err_t PathStroker::stroke(PathD& dst) const
{
  return stroke(dst, dst);
}

err_t PathStroker::stroke(PathD& dst, const PathD& src) const
{
  // We need:
  // - the Path instances have to be different.
  // - source path must be flat.
  if (&dst == &src || !src.isFlat())
  {
    PathD tmp;
    FOG_RETURN_ON_ERROR(
      src.flattenTo(tmp, NULL, _approximationScale)
    );
    return stroke(dst, tmp);
  }

  dst.clear();
  PathStrokerPrivate p(*this, dst);

  const uint8_t* commands = src.getCommands();
  const PointD* vertices = src.getVertices();

  // Traverse path, find moveTo / lineTo segments and stroke them.
  const uint8_t* subCommand = NULL;
  const uint8_t* curCommand = commands;
  sysuint_t remain = src.getLength();

  while (remain)
  {
    uint8_t cmd = curCommand[0];

    // LineTo is the most used command here, so it's first.
    if (FOG_LIKELY(PathCmd::isLineTo(cmd)))
    {
      // Nothing here...
    }
    else if (PathCmd::isMoveTo(cmd))
    {
      // Send path to stroker if there is something.
      if (FOG_LIKELY(subCommand != NULL && subCommand != curCommand))
      {
        sysuint_t subStart = (sysuint_t)(subCommand - commands);
        sysuint_t subLength = (sysuint_t)(curCommand - subCommand);

        FOG_RETURN_ON_ERROR(
          p.stroke(vertices + subStart, subLength, false)
        );
      }

      // Advance to new subpath.
      subCommand = curCommand;
    }
    else if (PathCmd::isClose(cmd) || PathCmd::isStop(cmd))
    {
      // Send path to stroker if there is something.
      if (FOG_LIKELY(subCommand != NULL))
      {
        sysuint_t subStart = (sysuint_t)(subCommand - commands);
        sysuint_t subLength = (sysuint_t)(curCommand - subCommand);

        FOG_RETURN_ON_ERROR(
          p.stroke(vertices + subStart, subLength, PathCmd::isClose(cmd))
        );
      }

      // We clear beginning mark, because we expect PATH_MOVE_TO command from now.
      subCommand = NULL;
    }

    curCommand++;
    remain--;
  }

  // Send path to stroker if there is something (this is last path not 
  // stopped by command).
  if (subCommand != NULL && subCommand != curCommand)
  {
    sysuint_t subStart = (sysuint_t)(subCommand - commands);
    sysuint_t subLength = (sysuint_t)(curCommand - subCommand);

    FOG_RETURN_ON_ERROR(
      p.stroke(vertices + subStart, subLength, false)
    );
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::StrokerPrivate]
// ============================================================================

#define ADD_VERTEX(x, y) \
  do { \
    if (FOG_UNLIKELY(dstCur == dstEnd)) FOG_RETURN_ON_ERROR(_grow()); \
    dstCur->set(x, y); \
    dstCur++; \
  } while(0)

#define CUR_INDEX() \
  ( (sysuint_t)(dstCur - dst._d->vertices) )

err_t PathStrokerPrivate::_begin()
{
  sysuint_t cap = dst.getCapacity();
  sysuint_t remain = cap - dst.getLength();
  if (remain < 64 || !dst.isDetached())
  {
    if (cap < 256)
      cap = 256;
    else
      cap *= 2;
    FOG_RETURN_ON_ERROR(dst.reserve(cap));
  }

  dstCur = const_cast<PointD*>(dst.getVertices());
  dstEnd = dstCur;

  dstCur += dst.getLength();
  dstEnd += dst.getCapacity();

  return ERR_OK;
}

err_t PathStrokerPrivate::_grow()
{
  sysuint_t len = dst._d->length;
  sysuint_t cap = dst._d->capacity;

  dst._d->length = cap;
  if (cap < 256)
    cap = 512;
  else
    cap *= 2;

  err_t err = dst.reserve(cap);
  if (err)
  {
    dst._d->length = len;
    return err;
  }

  dstCur = const_cast<PointD*>(dst.getVertices());
  dstEnd = dstCur;

  dstCur += dst.getLength();
  dstEnd += dst.getCapacity();

  return ERR_OK;
};

#if 0
err_t PathStrokerPrivate::calcArc(
  double x,   double y,
  double dx1, double dy1,
  double dx2, double dy2)
{
  double a1 = Math::atan2(dy1 * stroker._wSign, dx1 * stroker._wSign);
  double a2 = Math::atan2(dy2 * stroker._wSign, dx2 * stroker._wSign);
  double da = stroker._da;
  int i, n;

  ADD_VERTEX(x + dx1, y + dy1);
  if (stroker._wSign > 0)
  {
    if (a1 > a2) a2 += 2.0 * MATH_PI;
    n = int((a2 - a1) / da);
    da = (a2 - a1) / (n + 1);
    a1 += da;

    for (i = 0; i < n; i++)
    {
      double a1Sin;
      double a1Cos;
      Math::sincos(a1, &a1Sin, &a1Cos);

      ADD_VERTEX(x + a1Cos * stroker._w, y + a1Sin * stroker._w);
      a1 += da;
    }
  }
  else
  {
    if (a1 < a2) a2 -= 2.0 * MATH_PI;
    n = int((a1 - a2) / da);
    da = (a1 - a2) / (n + 1);
    a1 -= da;

    for (i = 0; i < n; i++)
    {
      double a1Sin;
      double a1Cos;
      Math::sincos(a1, &a1Sin, &a1Cos);

      ADD_VERTEX(x + a1Cos * stroker._w, y + a1Sin * stroker._w);
      a1 -= da;
    }
  }

  ADD_VERTEX(x + dx2, y + dy2);
  return ERR_OK;
}
#endif

err_t PathStrokerPrivate::calcArc(
  double x,   double y,
  double dx1, double dy1,
  double dx2, double dy2)
{
  double a1, a2;
  double da = stroker._da;
  int i, n;

  ADD_VERTEX(x + dx1, y + dy1);
  if (stroker._wSign > 0)
  {
    a1 = Math::atan2(dy1, dx1);
    a2 = Math::atan2(dy2, dx2);
    if (a1 > a2) a2 += 2.0 * MATH_PI;
    n = int((a2 - a1) / da);
  }
  else
  {
    a1 = Math::atan2(-dy1, -dx1);
    a2 = Math::atan2(-dy2, -dx2);
    if (a1 < a2) a2 -= 2.0 * MATH_PI;
    n = int((a1 - a2) / da);
  }

  da = (a2 - a1) / (n + 1);
  a1 += da;

  for (i = 0; i < n; i++)
  {
    double a1Sin;
    double a1Cos;
    Math::sincos(a1, &a1Sin, &a1Cos);

    ADD_VERTEX(x + a1Cos * stroker._w, y + a1Sin * stroker._w);
    a1 += da;
  }

  ADD_VERTEX(x + dx2, y + dy2);
  return ERR_OK;
}

err_t PathStrokerPrivate::calcMiter(
  const PointD& v0,
  const PointD& v1,
  const PointD& v2,
  double dx1, double dy1,
  double dx2, double dy2,
  int lineJoin,
  double mlimit,
  double dbevel)
{
  double xi = v1.x;
  double yi = v1.y;
  double di = 1;
  double lim = stroker._wAbs * mlimit;
  bool intersectionFailed  = true; // Assume the worst

  if (calcIntersection(
    v0.x + dx1, v0.y - dy1,
    v1.x + dx1, v1.y - dy1,
    v1.x + dx2, v1.y - dy2,
    v2.x + dx2, v2.y - dy2,
    &xi, &yi))
  {
    // Calculation of the intersection succeeded.
    di = Math::dist(v1.x, v1.y, xi, yi);
    if (di <= lim)
    {
      // Inside the miter limit.
      ADD_VERTEX(xi, yi);
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
    double x2 = v1.x + dx1;
    double y2 = v1.y - dy1;
    if ((crossProduct(v0.x, v0.y, v1.x, v1.y, x2, y2) < 0.0) ==
        (crossProduct(v1.x, v1.y, v2.x, v2.y, x2, y2) < 0.0))
    {
      // This case means that the next segment continues the previous one
      // (straight line).
      ADD_VERTEX(v1.x + dx1, v1.y - dy1);
      return ERR_OK;
    }
  }

  // Miter limit exceeded.
  switch(lineJoin)
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
        mlimit *= stroker._wSign;
        ADD_VERTEX(v1.x + dx1 + dy1 * mlimit, v1.y - dy1 + dx1 * mlimit);
        ADD_VERTEX(v1.x + dx2 - dy2 * mlimit, v1.y - dy2 - dx2 * mlimit);
      }
      else
      {
        double x1 = v1.x + dx1;
        double y1 = v1.y - dy1;
        double x2 = v1.x + dx2;
        double y2 = v1.y - dy2;
        di = (lim - dbevel) / (di - dbevel);
        ADD_VERTEX(x1 + (xi - x1) * di, y1 + (yi - y1) * di);
        ADD_VERTEX(x2 + (xi - x2) * di, y2 + (yi - y2) * di);
      }
      break;
  }

  return ERR_OK;
}

err_t PathStrokerPrivate::calcCap(
  const PointD& v0,
  const PointD& v1,
  double len,
  uint32_t cap)
{
  double ilen = 1.0 / len;

  double dx1 = (v1.y - v0.y) * ilen;
  double dy1 = (v1.x - v0.x) * ilen;

  dx1 *= stroker._w;
  dy1 *= stroker._w;

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
      double dx2 = dy1 * stroker._wSign;
      double dy2 = dx1 * stroker._wSign;

      ADD_VERTEX(v0.x - dx1 - dx2, v0.y + dy1 - dy2);
      ADD_VERTEX(v0.x + dx1 - dx2, v0.y - dy1 - dy2);
      break;
    }

    case LINE_CAP_ROUND:
    {
      int i;
      int n = int(MATH_PI / stroker._da);
      double da = MATH_PI / (n + 1);
      double a1;

      ADD_VERTEX(v0.x - dx1, v0.y + dy1);

      if (stroker._wSign > 0)
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
        double a1_sin;
        double a1_cos;
        Math::sincos(a1, &a1_sin, &a1_cos);

        ADD_VERTEX(v0.x + a1_cos * stroker._w, v0.y + a1_sin * stroker._w);
        a1 += da;
      }

      ADD_VERTEX(v0.x + dx1, v0.y - dy1);
      break;
    }

    case LINE_CAP_ROUND_REVERT:
    {
      int i;
      int n = int(MATH_PI / stroker._da);
      double da = MATH_PI / (n + 1);
      double a1;

      double dx2 = dy1 * stroker._wSign;
      double dy2 = dx1 * stroker._wSign;

      double vx = v0.x - dx2;
      double vy = v0.y - dy2;

      ADD_VERTEX(vx - dx1, vy + dy1);

      if (stroker._wSign > 0)
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
        double a1_sin;
        double a1_cos;
        Math::sincos(a1, &a1_sin, &a1_cos);

        ADD_VERTEX(vx + a1_cos * stroker._w, vy + a1_sin * stroker._w);
        a1 += da;
      }

      ADD_VERTEX(vx + dx1, vy - dy1);
      break;
    }

    case LINE_CAP_TRIANGLE:
    {
      double dx2 = dy1 * stroker._wSign;
      double dy2 = dx1 * stroker._wSign;

      ADD_VERTEX(v0.x - dx1, v0.y + dy1);
      ADD_VERTEX(v0.x - dx2, v0.y - dy2);
      ADD_VERTEX(v0.x + dx1, v0.y - dy1);
      break;
    }

    case LINE_CAP_TRIANGLE_REVERT:
    {
      double dx2 = dy1 * stroker._wSign;
      double dy2 = dx1 * stroker._wSign;

      ADD_VERTEX(v0.x - dx1 - dx2, v0.y + dy1 - dy2);
      ADD_VERTEX(v0.x, v0.y);
      ADD_VERTEX(v0.x + dx1 - dx2, v0.y - dy1 - dy2);
    }
  }

  return ERR_OK;
}

err_t PathStrokerPrivate::calcJoin(
  const PointD& v0,
  const PointD& v1,
  const PointD& v2,
  double len1,
  double len2)
{
  double wilen1 = (stroker._w / len1);
  double wilen2 = (stroker._w / len2);

  double dx1 = (v1.y - v0.y) * wilen1;
  double dy1 = (v1.x - v0.x) * wilen1;
  double dx2 = (v2.y - v1.y) * wilen2;
  double dy2 = (v2.x - v1.x) * wilen2;

  double cp = crossProduct(v0.x, v0.y, v1.x, v1.y, v2.x, v2.y);

  if (cp != 0 && (cp > 0) == (stroker._w > 0))
  {
    // Inner join.
    double limit = ((len1 < len2) ? len1 : len2) / stroker._wAbs;
    if (limit < stroker._params._innerLimit) limit = stroker._params._innerLimit;

    switch (stroker._params._innerJoin)
    {
      case INNER_JOIN_BEVEL:
        ADD_VERTEX(v1.x + dx1, v1.y - dy1);
        ADD_VERTEX(v1.x + dx2, v1.y - dy2);
        break;

      case INNER_JOIN_MITER:
        FOG_RETURN_ON_ERROR( calcMiter(v0, v1, v2, dx1, dy1, dx2, dy2, LINE_JOIN_MITER_REVERT, limit, 0) );
        break;

      case INNER_JOIN_JAG:
      case INNER_JOIN_ROUND:
        cp = (dx1-dx2) * (dx1-dx2) + (dy1-dy2) * (dy1-dy2);
        if (cp < len1 * len1 && cp < len2 * len2)
        {
          FOG_RETURN_ON_ERROR( calcMiter(v0, v1, v2, dx1, dy1, dx2, dy2, LINE_JOIN_MITER_REVERT, limit, 0) );
        }
        else
        {
          if (stroker._params._innerJoin == INNER_JOIN_JAG)
          {
            ADD_VERTEX(v1.x + dx1, v1.y - dy1);
            ADD_VERTEX(v1.x,       v1.y      );
            ADD_VERTEX(v1.x + dx2, v1.y - dy2);
          }
          else
          {
            ADD_VERTEX(v1.x + dx1, v1.y - dy1);
            ADD_VERTEX(v1.x,       v1.y      );
            FOG_RETURN_ON_ERROR( calcArc(v1.x, v1.y, dx2, -dy2, dx1, -dy1) );
            ADD_VERTEX(v1.x,       v1.y      );
            ADD_VERTEX(v1.x + dx2, v1.y - dy2);
          }
        }
        break;

      default:
        FOG_ASSERT_NOT_REACHED();
    }
  }
  else
  {
    // Outer join

    // Calculate the distance between v1 and the central point of the bevel
    // line segment.
    double dx = (dx1 + dx2) / 2;
    double dy = (dy1 + dy2) / 2;
    double dbevel = Math::sqrt(dx * dx + dy * dy);

    if (stroker._params._lineJoin == LINE_JOIN_ROUND ||
        stroker._params._lineJoin == LINE_JOIN_BEVEL)
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
      if (stroker._approximationScale * (stroker._wAbs - dbevel) < stroker._wEps)
      {
        if (calcIntersection(
          v0.x + dx1, v0.y - dy1,
          v1.x + dx1, v1.y - dy1,
          v1.x + dx2, v1.y - dy2,
          v2.x + dx2, v2.y - dy2,
          &dx, &dy))
        {
          ADD_VERTEX(dx, dy);
        }
        else
        {
          ADD_VERTEX(v1.x + dx1, v1.y - dy1);
        }
        return ERR_OK;
      }
    }

    switch (stroker._params._lineJoin)
    {
      case LINE_JOIN_MITER:
      case LINE_JOIN_MITER_REVERT:
      case LINE_JOIN_MITER_ROUND:
        FOG_RETURN_ON_ERROR(
          calcMiter(v0, v1, v2, dx1, dy1, dx2, dy2,
            stroker._params._lineJoin,
            stroker._params._miterLimit, dbevel)
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

err_t PathStrokerPrivate::stroke(const PointD* src, sysuint_t count, bool outline)
{
  // Can't stroke one-vertex array.
  if (count <= 1) return ERR_PATH_CANT_STROKE;
  // To do outline we need at least three vertices.
  if (outline && count <= 2) return ERR_PATH_CANT_STROKE;

  const PointD* cur;
  sysuint_t i;
  sysuint_t moveToPosition0 = dst.getLength();
  sysuint_t moveToPosition1 = INVALID_INDEX;

  FOG_RETURN_ON_ERROR(
    _begin()
  );

  // Alloc or realloc array for our distances (distance between individual
  // vertices [0]->[1], [1]->[2], ...). Distance at index[0] means distance
  // between src[0] and src[1], etc. Last distance is special and it 0.0 if
  // path is not closed, otherwise src[count-1]->src[0].
  if (distancesAlloc < count)
  {
    // Need to realloc, we align count to 128 vertices.
    if (distances) buffer.free();

    distancesAlloc = (count + 127) & ~127;
    distances = reinterpret_cast<double*>(buffer.alloc(count * sizeof(double)));

    if (distances == NULL)
    {
      distancesAlloc = 0;
      return ERR_RT_OUT_OF_MEMORY;
    }
  }

  double *dist;

  for (i = count - 1, cur = src, dist = distances; i; i--, cur++, dist++)
  {
    double d = Math::dist(cur[0].x, cur[0].y, cur[1].x, cur[1].y);
    if (d <= PATH_VERTEX_DIST_EPSILON_D) d = 0.0;

    dist[0] = d;
  }

  const PointD* srcEnd = src + count;

  PointD cp[3]; // Current points.
  double cd[3]; // Current distances.

  // If something went wrong.
  // for (i = 0; i < count; i++)
  // {
  //  fog_debug("Vertex [%g, %g]", src[i].x, src[i].y);
  // }

  // --------------------------------------------------------------------------
  // [Outline]
  // --------------------------------------------------------------------------

#define IS_DEGENERATED_DIST(__dist__) ((__dist__) <= PATH_VERTEX_DIST_EPSILON_D)

  if (outline)
  {
    // We need also to calc distance between first and last point.
    {
      double d = Math::dist(src[0].x, src[0].y, src[count-1].x, src[count-1].y);
      if (d <= PATH_VERTEX_DIST_EPSILON_D) d = 0.0;

      distances[count - 1] = d;
    }

    PointD fp[2]; // First points.
    double fd[2]; // First distances.

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
    sysuint_t firstI = i;

    cp[i] = src[count - 1];
    cd[i] = dist[count - 1];
    if (FOG_LIKELY(!IS_DEGENERATED_DIST(cd[i]))) { i++; firstI++; }

    do {
      if (FOG_UNLIKELY(cur == srcEnd)) return ERR_PATH_CANT_STROKE;

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

      if (cur == srcEnd) goto outline1Done;
      while (FOG_UNLIKELY(IS_DEGENERATED_DIST(dist[0])))
      {
        dist++;
        if (++cur == srcEnd) goto outline1Done;
      }

      cp[0] = cp[1];
      cd[0] = cd[1];

      cp[1] = cp[2];
      cd[1] = cd[2];

      cp[2] = *cur++;
      cd[2] = *dist++;
    }

    // End joins.
outline1Done:
    FOG_RETURN_ON_ERROR( calcJoin(cp[1], cp[2], fp[0], cd[1], cd[2]) );
    FOG_RETURN_ON_ERROR( calcJoin(cp[2], fp[0], fp[1], cd[2], fd[0]) );

    // Close path (CW).
    ADD_VERTEX(NAN, NAN);

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
    cd[i] = dist[0];
    if (cd[i] != 0.0) { i++; firstI++; }

    do {
      cp[i] = *--cur;
      cd[i] = *--dist;
      if (FOG_LIKELY(cd[i] != 0.0)) i++;
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
        if (cur == src) goto outline2Done;
        cur--;
        dist--;
      } while (FOG_UNLIKELY(dist[0] == 0.0));

      cp[0] = cp[1];
      cd[0] = cd[1];

      cp[1] = cp[2];
      cd[1] = cd[2];

      cp[2] = *cur;
      cd[2] = *dist;
    }

    // End joins.
outline2Done:
    FOG_RETURN_ON_ERROR( calcJoin(cp[1], cp[2], fp[0], cd[2], fd[0]) );
    FOG_RETURN_ON_ERROR( calcJoin(cp[2], fp[0], fp[1], fd[0], fd[1]) );

    // Close path (CCW).
    ADD_VERTEX(NAN, NAN);
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
      if (FOG_UNLIKELY(cur == srcEnd)) return ERR_PATH_CANT_STROKE;

      cp[i] = *cur++;
      cd[i] = *dist++;
      if (FOG_LIKELY(cd[i] != 0.0)) i++;
    } while (i < 2);

    // Start cap.
    FOG_RETURN_ON_ERROR( calcCap(cp[0], cp[1], cd[0], stroker._params._startCap) );

    // Make the outline.
    if (cur == srcEnd) goto pen1Done;
    while (FOG_UNLIKELY(dist[0] == 0.0))
    {
      dist++;
      if (++cur == srcEnd) goto pen1Done;
    }

    goto pen1Loop;

    for (;;)
    {
      cp[0] = cp[1];
      cd[0] = cd[1];

      cp[1] = cp[2];
      cd[1] = cd[2];

pen1Loop:
      cp[2] = *cur++;
      cd[2] = *dist++;

      FOG_RETURN_ON_ERROR( calcJoin(cp[0], cp[1], cp[2], cd[0], cd[1]) );
      if (cur == srcEnd) goto pen1Done;

      while (FOG_UNLIKELY(dist[0] == 0.0))
      {
        dist++;
        if (++cur == srcEnd) goto pen1Done;
      }
    }

    // End joins.
pen1Done:

    // ------------------------------------------------------------------------
    // [Outline 2]
    // ------------------------------------------------------------------------

    // Fill the current points / distances and skip degenerate cases.
    i = 0;

    do {
      cp[i] = *--cur;
      cd[i] = *--dist;
      if (FOG_LIKELY(cd[i] != 0.0)) i++;
    } while (i < 2);

    // End cap.
    FOG_RETURN_ON_ERROR( calcCap(cp[0], cp[1], cd[1], stroker._params._endCap) );

    // Make the outline.
    if (cur == src) goto pen2Done;

    cur--;
    dist--;

    while (FOG_UNLIKELY(dist[0] == 0.0))
    {
      if (cur == src) goto pen2Done;

      dist--;
      cur--;
    }

    goto pen2Loop;

    for (;;)
    {
      do {
        if (cur == src) goto pen2Done;

        cur--;
        dist--;
      } while (FOG_UNLIKELY(dist[0] == 0.0));

      cp[0] = cp[1];
      cd[0] = cd[1];

      cp[1] = cp[2];
      cd[1] = cd[2];

pen2Loop:
      cp[2] = *cur;
      cd[2] = *dist;

      FOG_RETURN_ON_ERROR( calcJoin(cp[0], cp[1], cp[2], cd[1], cd[2]) );
    }
pen2Done:
    // Close path (CCW).
    ADD_VERTEX(NAN, NAN);
  }

  // Fix length of path.
  {
    sysuint_t finalLength = CUR_INDEX();
    dst._d->length = finalLength;
    FOG_ASSERT(finalLength <= dst._d->capacity);

    // Fix path adding PATH_CMD_MOVE_TO/CLOSE commands at begin of each 
    // outline and filling rest by PATH_CMD_LINE_TO. This allowed us to 
    // simplify ADD_VERTEX() macro.
    uint8_t* dstCommands = const_cast<uint8_t*>(dst.getCommands());

    if (moveToPosition0 < finalLength)
    {
      memset(dstCommands + moveToPosition0, (unsigned int)(PATH_CMD_LINE_TO), finalLength - moveToPosition0);
      dstCommands[moveToPosition0] = PATH_CMD_MOVE_TO;
      dstCommands[finalLength - 1] = PATH_CMD_END | PATH_CMD_FLAG_CLOSE | PATH_CMD_FLAG_CW;
    }
    if (moveToPosition1 < finalLength)
    {
      dstCommands[moveToPosition1 - 1] = PATH_CMD_END | PATH_CMD_FLAG_CLOSE | PATH_CMD_FLAG_CCW;
      dstCommands[moveToPosition1] = PATH_CMD_MOVE_TO;
    }
  }

  return ERR_OK;
}

#if 0

// ============================================================================
// [Fog::Path - Dash]
// ============================================================================

err_t PathD::dash(const List<double>& dashes, double startOffset, double approximationScale)
{
  return dashTo(*this, dashes, startOffset, approximationScale);
}

err_t PathD::dashTo(PathD& dst, const List<double>& dashes, double startOffset, double approximationScale)
{
  if (getType() != LineType)
  {
    PathD tmp;
    flattenTo(tmp, NULL, approximationScale);
    return tmp.dashTo(dst, dashes, startOffset, approximationScale);
  }
  else
  {
    AggPath src(*this);

    agg::conv_dash<AggPath, agg::vcgen_dash> dasher(src);

    List<double>::ConstIterator it(dashes);
    for (;;)
    {
      double d1 = it.value(); it.toNext();
      if (!it.isValid()) break;
      double d2 = it.value(); it.toNext();
      dasher.add_dash(d1, d2);
      if (!it.isValid()) break;
    }
    dasher.dash_start(startOffset);

    if (this == &dst)
    {
      PathD tmp;
      err_t err = concatToPath(tmp, dasher);
      if (err) return err;
      return dst.set(tmp);
    }
    else
    {
      dst.clear();
      return concatToPath(dst, dasher);
    }
  }
}

// ============================================================================
// [Fog::Path - Stroke]
// ============================================================================

err_t PathD::stroke(const StrokerParams& strokeParams, double approximationScale)
{
  return strokeTo(*this, strokeParams);
}

#if 1
err_t PathD::strokeTo(PathD& dst, const StrokerParams& strokeParams, double approximationScale) const
{
  if (getType() != LineType)
  {
    PathD tmp;
    flattenTo(tmp, NULL, approximationScale);
    return tmp.strokeTo(dst, strokeParams, approximationScale);
  }
  else
  {
    AggPath src(*this);

    agg::conv_stroke<AggPath> stroker(src);
    stroker.width(strokeParams.lineWidth);
    stroker.miter_limit(strokeParams.miterLimit);
    stroker.line_join(static_cast<agg::line_join_e>(strokeParams.lineJoin));
    stroker.line_cap(static_cast<agg::line_cap_e>(strokeParams.lineCap));
    stroker.approximation_scale(approximationScale);

    if (this == &dst)
    {
      PathD tmp;
      err_t err = concatToPath(tmp, stroker);
      if (err) return err;
      return dst.set(tmp);
    }
    else
    {
      dst.clear();
      return concatToPath(dst, stroker);
    }
  }
}
#endif


  // [Dash]

  err_t dash(const List<double>& dashes, double startOffset, double approximationScale = 1.0);

  //! @brief Similar method to @c dash(), but with specified destination path.
  err_t dashTo(PathD& dst, const List<double>& dashes, double startOffset, double approximationScale = 1.0);

  // [Stroke]

  //! @brief Stroke the path.
  //!
  //! @note Stroking path will also flatten it.
  err_t stroke(const StrokerParams& strokeParams, double approximationScale = 1.0);

  //! @brief Similar method to @c stroke(), but with specified destination path.
  err_t strokeTo(PathD& dst, const StrokerParams& strokeParams, double approximationScale = 1.0) const;

#endif

} // Fog namespace
