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
#include <Fog/Core/CpuInfo.h>
#include <Fog/Core/Math.h>
#include <Fog/Graphics/Curve_p.h>
#include <Fog/Graphics/Path.h>
#include <Fog/Graphics/Transform.h>

namespace Fog {

// We can disable C implementation when hardcoding for SSE2.
#if !defined(FOG_HARDCODE_SSE2)
# define FOG_CURVE_INIT_C
#endif // FOG_HARDCODE_SSE2

// ============================================================================
// [Fog::Curve - Approximate]
// ============================================================================

#define CURVE_APPROXIMATE3_RECURSION_LIMIT 32
#define CURVE_APPROXIMATE4_RECURSION_LIMIT 32
#define CURVE_VERTEX_INITIAL_SIZE 256

#define ADD_VERTEX(_x, _y) \
  do { \
    curVertex->set(_x, _y); \
    curVertex++; \
  } while(0)

static err_t FOG_FASTCALL _G2d_QuadCurveD_approximate(
  const PointD* self,
  PathD& dst,
  uint8_t initialCommand,
  double approximationScale)
{
  double distanceToleranceSquare = Math::pow2(0.5 / approximationScale);
  double angleTolerance = 0.0;
  double x0 = self[0].x;
  double y0 = self[0].y;
  double x1 = self[1].x;
  double y1 = self[1].y;
  double x2 = self[2].x;
  double y2 = self[2].y;

  sysuint_t initialLength = dst._d->length;
  sysuint_t level = 0;

  PointD* curVertex;
  PointD* endVertex;

  PointD _stack[CURVE_APPROXIMATE3_RECURSION_LIMIT * 3];
  PointD* stack = _stack;

realloc:
  {
    sysuint_t pos = dst._add(CURVE_VERTEX_INITIAL_SIZE);
    if (pos == INVALID_INDEX)
    {
      // Purge dst length to its initial state.
      if (dst._d->length != initialLength) dst._d->length = initialLength;
      return ERR_RT_OUT_OF_MEMORY;
    }

    curVertex = dst._d->vertices + pos;
    endVertex = curVertex + CURVE_VERTEX_INITIAL_SIZE - 1;
  }

  for (;;)
  {
    // Realloc if needed, but update length if need to do.
    if (curVertex >= endVertex)
    {
      dst._d->length = (sysuint_t)(curVertex - dst._d->vertices);
      goto realloc;
    }

    // Calculate all the mid-points of the line segments.
    double x01   = (x0 + x1) * 0.5;
    double y01   = (y0 + y1) * 0.5;
    double x12   = (x1 + x2) * 0.5;
    double y12   = (y1 + y2) * 0.5;
    double x012  = (x01 + x12) * 0.5;
    double y012  = (y01 + y12) * 0.5;

    double dx = x2-x0;
    double dy = y2-y0;
    double d = Math::abs(((x1 - x2) * dy - (y1 - y2) * dx));
    double da;

    if (d > CURVE_COLLINEARITY_EPSILON_D)
    {
      // Regular case.
      if (d * d <= distanceToleranceSquare * (dx*dx + dy*dy))
      {
        // If the curvature doesn't exceed the distanceTolerance value
        // we tend to finish subdivisions.
        if (angleTolerance < CURVE_ANGLE_TOLERANCE_EPSILON_D)
        {
          ADD_VERTEX(x012, y012);
          goto ret;
        }

        // Angle & Cusp Condition.
        da = Math::abs(Math::atan2(y2 - y1, x2 - x1) - Math::atan2(y1 - y0, x1 - x0));
        if (da >= MATH_PI) da = 2.0 * MATH_PI - da;

        if (da < angleTolerance)
        {
          // Finally we can stop the recursion.
          ADD_VERTEX(x012, y012);
          goto ret;
        }
      }
    }
    else
    {
      // Collinear case.
      da = dx*dx + dy*dy;
      if (da == 0)
      {
        d = calcSqDistance(x0, y0, x1, y1);
      }
      else
      {
        d = ((x1 - x0)*dx + (y1 - y0)*dy) / da;

        if (d > 0 && d < 1)
        {
          // Simple collinear case, 1---2---3.
          // We can leave just two endpoints.
          goto ret;
        }

        if (d <= 0)
          d = calcSqDistance(x1, y1, x0, y0);
        else if (d >= 1)
          d = calcSqDistance(x1, y1, x2, y2);
        else
          d = calcSqDistance(x1, y1, x0 + d*dx, y0 + d*dy);
      }
      if (d < distanceToleranceSquare)
      {
        ADD_VERTEX(x1, y1);
        goto ret;
      }
    }

    // Continue subdivision.
    //
    // Original code from antigrain was:
    //   recursive_bezier(x0, y0, x01, y01, x012, y012, level + 1);
    //   recursive_bezier(x012, y012, x12, y12, x2, y2, level + 1);
    //
    // First recursive subdivision will be set into x0, y0, x1, y1, x2, y2,
    // second subdivision will be added into stack.

    if (level < CURVE_APPROXIMATE3_RECURSION_LIMIT)
    {
      stack[0].set(x012, y012);
      stack[1].set(x12 , y12 );
      stack[2].set(x2  , y2  );

      stack += 3;
      level++;

      x1 = x01;
      y1 = y01;
      x2 = x012;
      y2 = y012;

      continue;
    }
    else
    {
      if (Math::isNaN(x012)) goto invalidNumber;
    }

ret:
    if (level == 0) break;

    stack -= 3;
    level--;

    x0 = stack[0].x;
    y0 = stack[0].y;
    x1 = stack[1].x;
    y1 = stack[1].y;
    x2 = stack[2].x;
    y2 = stack[2].y;
  }

  // Add end point.
  ADD_VERTEX(x2, y2);

  {
    // Update dst length.
    sysuint_t length = (sysuint_t)(curVertex - dst._d->vertices);
    dst._d->length = length;

    // Make sure we are not out of bounds.
    FOG_ASSERT(dst._d->capacity >= length);

    // Fill initial and LINE_TO commands.
    uint8_t* commands = dst._d->commands + initialLength;
    sysuint_t i = length - initialLength;

    if (i)
    {
      *commands++ = initialCommand;
      i--;
    }

    while (i)
    {
      *commands++ = PATH_CMD_LINE_TO;
      i--;
    }
  }

  return ERR_OK;

invalidNumber:
  // Purge dst length to its initial state.
  if (dst._d->length != initialLength) dst._d->length = initialLength;
  return ERR_PATH_INVALID;
}

static err_t FOG_FASTCALL _G2d_CubicCurveD_approximate(
  const PointD* self,
  PathD& dst,
  uint8_t initialCommand,
  double approximationScale)
{
  double distanceToleranceSquare = Math::pow2(0.5 / approximationScale);
  double angleTolerance = 0.0;
  double cuspLimit = 0.0;
  double x0 = self[0].x;
  double y0 = self[0].y;
  double x1 = self[1].x;
  double y1 = self[1].y;
  double x2 = self[2].x;
  double y2 = self[2].y;
  double x3 = self[3].x;
  double y3 = self[3].y;

  sysuint_t initialLength = dst._d->length;
  sysuint_t level = 0;

  PointD* curVertex;
  PointD* endVertex;

  PointD _stack[CURVE_APPROXIMATE4_RECURSION_LIMIT * 4];
  PointD* stack = _stack;

realloc:
  {
    sysuint_t pos = dst._add(CURVE_VERTEX_INITIAL_SIZE);
    if (pos == INVALID_INDEX)
    {
      // Purge dst length to it's initial state.
      if (dst._d->length != initialLength) dst._d->length = initialLength;
      return ERR_RT_OUT_OF_MEMORY;
    }

    curVertex = dst._d->vertices + pos;
    endVertex = curVertex + CURVE_VERTEX_INITIAL_SIZE - 2;
  }

  for (;;)
  {
    // Realloc if needed.
    if (curVertex >= endVertex)
    {
      dst._d->length = (sysuint_t)(curVertex - dst._d->vertices);
      goto realloc;
    }

    // Calculate all the mid-points of the line segments.
    double x01   = (x0 + x1) * 0.5;
    double y01   = (y0 + y1) * 0.5;
    double x12   = (x1 + x2) * 0.5;
    double y12   = (y1 + y2) * 0.5;
    double x23   = (x2 + x3) * 0.5;
    double y23   = (y2 + y3) * 0.5;
    double x012  = (x01 + x12) * 0.5;
    double y012  = (y01 + y12) * 0.5;
    double x123  = (x12 + x23) * 0.5;
    double y123  = (y12 + y23) * 0.5;
    double x0123 = (x012 + x123) * 0.5;
    double y0123 = (y012 + y123) * 0.5;

    // Try to approximate the full cubic curve by a single straight line.
    double dx = x3 - x0;
    double dy = y3 - y0;

    double d2 = Math::abs(((x1 - x3) * dy - (y1 - y3) * dx));
    double d3 = Math::abs(((x2 - x3) * dy - (y2 - y3) * dx));
    double da1, da2, k;

    switch ((int(d2 > CURVE_COLLINEARITY_EPSILON_D) << 1) +
             int(d3 > CURVE_COLLINEARITY_EPSILON_D))
    {
      // All collinear OR p1 == p4.
      case 0:
        k = dx*dx + dy*dy;
        if (k == 0)
        {
          d2 = calcSqDistance(x0, y0, x1, y1);
          d3 = calcSqDistance(x3, y3, x2, y2);
        }
        else
        {
          k   = 1.0 / k;
          da1 = x1 - x0;
          da2 = y1 - y0;
          d2  = k * (da1 * dx + da2 * dy);
          da1 = x2 - x0;
          da2 = y2 - y0;
          d3  = k * (da1 * dx + da2 * dy);

          if (d2 > 0 && d2 < 1 && d3 > 0 && d3 < 1)
          {
            // Simple collinear case, 1---2---3---4.
            // We can leave just two endpoints.
            goto ret;
          }

          if (d2 <= 0)
            d2 = calcSqDistance(x1, y1, x0, y0);
          else if (d2 >= 1)
            d2 = calcSqDistance(x1, y1, x3, y3);
          else
            d2 = calcSqDistance(x1, y1, x0 + d2*dx, y0 + d2*dy);

          if (d3 <= 0)
            d3 = calcSqDistance(x2, y2, x0, y0);
          else if (d3 >= 1)
            d3 = calcSqDistance(x2, y2, x3, y3);
          else
            d3 = calcSqDistance(x2, y2, x0 + d3*dx, y0 + d3*dy);
        }

        if (d2 > d3)
        {
          if (d2 < distanceToleranceSquare)
          {
            ADD_VERTEX(x1, y1);
            goto ret;
          }
        }
        else
        {
          if (d3 < distanceToleranceSquare)
          {
            ADD_VERTEX(x2, y2);
            goto ret;
          }
        }
        break;

    // p1, p2, p4 are collinear, p3 is significant.
    case 1:
      if (d3 * d3 <= distanceToleranceSquare * (dx*dx + dy*dy))
      {
        if (angleTolerance < CURVE_ANGLE_TOLERANCE_EPSILON_D)
        {
          ADD_VERTEX(x12, y12);
          goto ret;
        }

        // Angle Condition.
        da1 = Math::abs(Math::atan2(y3 - y2, x3 - x2) - Math::atan2(y2 - y1, x2 - x1));
        if (da1 >= MATH_PI) da1 = 2.0 * MATH_PI - da1;

        if (da1 < angleTolerance)
        {
          ADD_VERTEX(x1, y1);
          ADD_VERTEX(x2, y2);
          goto ret;
        }

        if (cuspLimit != 0.0)
        {
          if (da1 > cuspLimit)
          {
            ADD_VERTEX(x2, y2);
            goto ret;
          }
        }
      }
      break;

    // p1, p3, p4 are collinear, p2 is significant.
    case 2:
      if (d2 * d2 <= distanceToleranceSquare * (dx*dx + dy*dy))
      {
        if (angleTolerance < CURVE_ANGLE_TOLERANCE_EPSILON_D)
        {
          ADD_VERTEX(x12, y12);
          goto ret;
        }

        // Angle Condition.
        da1 = Math::abs(Math::atan2(y2 - y1, x2 - x1) - Math::atan2(y1 - y0, x1 - x0));
        if (da1 >= MATH_PI) da1 = 2.0 * MATH_PI - da1;

        if (da1 < angleTolerance)
        {
          ADD_VERTEX(x1, y1);
          ADD_VERTEX(x2, y2);
          goto ret;
        }

        if (cuspLimit != 0.0)
        {
          if (da1 > cuspLimit)
          {
            ADD_VERTEX(x2, y2);
            goto ret;
          }
        }
      }
      break;

    // Regular case.
    case 3:
      if ((d2 + d3)*(d2 + d3) <= distanceToleranceSquare * (dx*dx + dy*dy))
      {
        // If the curvature doesn't exceed the distance_tolerance value
        // we tend to finish subdivisions.
        if (angleTolerance < CURVE_ANGLE_TOLERANCE_EPSILON_D)
        {
          ADD_VERTEX(x12, y12);
          goto ret;
        }

        // Angle & Cusp Condition.
        k   = Math::atan2(y2 - y1, x2 - x1);
        da1 = Math::abs(k - Math::atan2(y1 - y0, x1 - x0));
        da2 = Math::abs(Math::atan2(y3 - y2, x3 - x2) - k);
        if (da1 >= MATH_PI) da1 = 2.0 * MATH_PI - da1;
        if (da2 >= MATH_PI) da2 = 2.0 * MATH_PI - da2;

        if (da1 + da2 < angleTolerance)
        {
          // Finally we can stop the recursion.
          ADD_VERTEX(x12, y12);
          goto ret;
        }

        if (cuspLimit != 0.0)
        {
          if (da1 > cuspLimit)
          {
            ADD_VERTEX(x1, y1);
            goto ret;
          }

          if (da2 > cuspLimit)
          {
            ADD_VERTEX(x2, y2);
            goto ret;
          }
        }
      }
      break;
    }

    // Continue subdivision.
    //
    // Original antigrain code:
    //   recursive_bezier(x0, y0, x01, y01, x012, y012, x0123, y0123, level + 1);
    //   recursive_bezier(x0123, y0123, x123, y123, x23, y23, x3, y3, level + 1);
    //
    // First recursive subdivision will be set into x0, y0, x1, y1, x2, y2,
    // second subdivision will be added into stack.
    if (level < CURVE_APPROXIMATE4_RECURSION_LIMIT)
    {
      stack[0].set(x0123, y0123);
      stack[1].set(x123 , y123 );
      stack[2].set(x23  , y23  );
      stack[3].set(x3   , y3   );

      stack += 4;
      level++;

      x1 = x01;
      y1 = y01;
      x2 = x012;
      y2 = y012;
      x3 = x0123;
      y3 = y0123;

      continue;
    }
    else
    {
      if (Math::isNaN(x0123)) goto invalidNumber;
    }

ret:
    if (level == 0) break;

    stack -= 4;
    level--;

    x0 = stack[0].x;
    y0 = stack[0].y;
    x1 = stack[1].x;
    y1 = stack[1].y;
    x2 = stack[2].x;
    y2 = stack[2].y;
    x3 = stack[3].x;
    y3 = stack[3].y;
  }

  // Add end point.
  ADD_VERTEX(x3, y3);

  {
    // Update dst length.
    sysuint_t length = (sysuint_t)(curVertex - dst._d->vertices);
    dst._d->length = length;

    // Make sure we are not out of bounds.
    FOG_ASSERT(dst._d->capacity >= length);

    // Fill initial and MoveTo commands.
    uint8_t* commands = dst._d->commands + initialLength;
    sysuint_t i = length - initialLength;

    if (i)
    {
      *commands++ = initialCommand;
      i--;
    }

    while (i)
    {
      *commands++ = PATH_CMD_LINE_TO;
      i--;
    }
  }

  return ERR_OK;

invalidNumber:
  // Purge dst length to its initial state.
  if (dst._d->length != initialLength) dst._d->length = initialLength;
  return ERR_PATH_INVALID;
}

#undef ADD_VERTEX

} // Fog namespace

// ============================================================================
// [Library Initializers]
// ============================================================================

FOG_INIT_DECLARE err_t fog_curve_init(void)
{
  using namespace Fog;

// TODO: SSE/SSE2 version needed.
// #if defined(FOG_CURVE_INIT_C)
// #endif // FOG_CURVE_INIT_C
  _g2d.quadcurved.approximate = _G2d_QuadCurveD_approximate;

  _g2d.cubiccurved.approximate = _G2d_CubicCurveD_approximate;

  return ERR_OK;
}

FOG_INIT_DECLARE void fog_curve_shutdown(void)
{
}
