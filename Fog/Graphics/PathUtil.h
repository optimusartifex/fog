// [Fog-Graphics Library - Private API]
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

// [Guard]
#ifndef _FOG_GRAPHICS_PATHUTIL_H
#define _FOG_GRAPHICS_PATHUTIL_H

// [Dependencies]
#include <Fog/Core/Math.h>
#include <Fog/Graphics/Path.h>

namespace Fog {

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct DoubleMatrix;

// ============================================================================
// [Fog::PathUtil]
// ============================================================================

namespace PathUtil {

//! @addtogroup Fog_Graphics_Geometry
//! @{

// ============================================================================
// [Fog::PathUtil::Structures]
// ============================================================================

struct ApproximateCurve3Data
{
  double x1, y1;
  double x2, y2;
  double x3, y3;
};

struct ApproximateCurve4Data
{
  double x1, y1;
  double x2, y2;
  double x3, y3;
  double x4, y4;
};

// ============================================================================
// [Fog::PathUtil::Constants]
// ============================================================================

//! @brief Coinciding points maximal distance (Epsilon).
static const double PATH_VERTEX_DIST_EPSILON = 1.0e-14;

//! @brief See calcIntersection (Epsilon)..
static const double INTERSECTION_EPSILON = 1.0e-30;

//! @brief This epsilon is used to prevent us from adding degenerate curves
//! (converging to a single point).
//!
//! The value isn't very critical. Function arc_to_bezier() has a limit
//! of the sweep_angle. If fabs(sweep_angle) exceeds pi/2 the curve
//! becomes inaccurate. But slight exceeding is quite appropriate.
static const double BEZIER_ARC_ANGLE_EPSILON = 0.01;

static const double CURVE_DISTANCE_EPSILON = 1e-30;
static const double CURVE_COLLINEARITY_EPSILON = 1e-30;
static const double CURVE_ANGLE_TOLERANCE_EPSILON = 0.01;

// ============================================================================
// [Fog::PathUtil::Function Map]
// ============================================================================

struct FunctionMap
{
  typedef void (FOG_FASTCALL *TranslatePointsDFn)(DoublePoint* dst, const DoublePoint* src, sysuint_t count, const DoublePoint* pt);
  typedef void (FOG_FASTCALL *TransformPointsDFn)(DoublePoint* dst, const DoublePoint* src, sysuint_t count, const DoubleMatrix* matrix);

  typedef err_t (FOG_FASTCALL *ApproximateCurve3Fn)(
    DoublePath& dst,
    double x1, double y1,
    double x2, double y2,
    double x3, double y3,
    uint8_t initialCommand,
    double approximationScale,
    double angleTolerance);

  typedef err_t (FOG_FASTCALL *ApproximateCurve4Fn)(
    DoublePath& dst,
    double x1, double y1,
    double x2, double y2,
    double x3, double y3,
    double x4, double y4,
    uint8_t initialCommand,
    double approximationScale,
    double angleTolerance,
    double cuspLimit);

  TranslatePointsDFn  translatePointsD;
  TransformPointsDFn  transformPointsD;

  ApproximateCurve3Fn approximateCurve3;
  ApproximateCurve4Fn approximateCurve4;
};

extern FOG_API FunctionMap functionMap;

// ============================================================================
// [Fog::PathUtil::FunctionMap wrappers]
// ============================================================================

static FOG_INLINE void translatePoints(DoublePoint* data, sysuint_t count, const DoublePoint* pt)
{ functionMap.translatePointsD(data, data, count, pt); }

static FOG_INLINE void translatePoints(DoublePoint* dst, const DoublePoint* src, sysuint_t count, const DoublePoint* pt)
{ functionMap.translatePointsD(dst, src, count, pt); }

static FOG_INLINE void transformPoints(DoublePoint* data, sysuint_t count, const DoubleMatrix* matrix)
{ functionMap.transformPointsD(data, data, count, matrix); }

static FOG_INLINE void transformPoints(DoublePoint* dst, const DoublePoint* src, sysuint_t count, const DoubleMatrix* matrix)
{ functionMap.transformPointsD(dst, src, count, matrix); }

// ============================================================================
// [Fog::PathUtil::Helpers]
// ============================================================================

static FOG_INLINE double calcDistance(
  double x1, double y1,
  double x2, double y2)
{
  double dx = x2 - x1;
  double dy = y2 - y1;
  return Math::sqrt(dx * dx + dy * dy);
}

static FOG_INLINE double calcSqDistance(
  double x1, double y1,
  double x2, double y2)
{
  double dx = x2 - x1;
  double dy = y2 - y1;
  return dx * dx + dy * dy;
}

static FOG_INLINE bool calcIntersection(
  double ax, double ay, double bx, double by,
  double cx, double cy, double dx, double dy,
  double* x, double* y)
{
  double num = (ay-cy) * (dx-cx) - (ax-cx) * (dy-cy);
  double den = (bx-ax) * (dy-cy) - (by-ay) * (dx-cx);
  if (fabs(den) < INTERSECTION_EPSILON) return false;
  double r = num / den;
  *x = ax + r * (bx-ax);
  *y = ay + r * (by-ay);
  return true;
}

static FOG_INLINE double crossProduct(
  double x1, double y1,
  double x2, double y2,
  double x,  double y)
{
  return (x - x2) * (y2 - y1) - (y - y2) * (x2 - x1);
}

//! @}

} // PathUtil namespace
} // Fog namespace

// [Guard]
#endif // _FOG_GRAPHICS_PATHUTIL_H
