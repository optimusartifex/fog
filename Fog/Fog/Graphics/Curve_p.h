// [Fog-Graphics]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_CURVE_P_H
#define _FOG_GRAPHICS_CURVE_P_H

// [Dependencies]
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Funcs.h>

namespace Fog {

// ============================================================================
// [Fog::Curve::Structures]
// ============================================================================

struct CurveDApproximate3Data
{
  double x1, y1;
  double x2, y2;
  double x3, y3;
};

struct CurveDApproximate4Data
{
  double x1, y1;
  double x2, y2;
  double x3, y3;
  double x4, y4;
};

//! @addtogroup Fog_Graphics_Geometry
//! @{

// ============================================================================
// [Fog::Curve::Constants]
// ============================================================================

//! @brief Coinciding points maximal distance (Epsilon).
static const double PATH_VERTEX_DIST_EPSILON = 1.0e-14;

//! @brief See calcIntersection (Epsilon).
static const double INTERSECTION_EPSILON = 1.0e-30;

//! @brief This epsilon is used to prevent us from adding degenerate curves
//! (converging to a single point).
//!
//! The value isn't very critical. Function arc_to_bezier() has a limit
//! of the sweep_angle. If Math::abs(sweep_angle) exceeds pi/2 the curve
//! becomes inaccurate. But slight exceeding is quite appropriate.
static const double CURVE_ARC_ANGLE_EPSILON_D = 0.01;

static const double CURVE_DISTANCE_EPSILON = 1e-30;
static const double CURVE_COLLINEARITY_EPSILON = 1e-30;
static const double CURVE_ANGLE_TOLERANCE_EPSILON = 0.01;

// ============================================================================
// [Fog::Curve::Helpers]
// ============================================================================

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
  if (Math::abs(den) < INTERSECTION_EPSILON) return false;
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

// ============================================================================
// [Fog::CurveF]
// ============================================================================

struct FOG_HIDDEN CurveF
{
};

// ============================================================================
// [Fog::CurveD]
// ============================================================================

struct FOG_HIDDEN CurveD
{
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GRAPHICS_CURVE_P_H
