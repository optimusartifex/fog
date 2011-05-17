// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_GEOMETRY_MATH2D_H
#define _FOG_G2D_GEOMETRY_MATH2D_H

// [Dependencies]
#include <Fog/Core/Math/Math.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Global/Api.h>

namespace Fog {

//! @addtogroup Fog_G2d_Geometry
//! @{

// ============================================================================
// [Fog::Math2dConst]
// ============================================================================

//! @brief Fog 2d math constants which might be used within templates.
template<typename NumT>
struct Math2dConst {};

template<>
struct Math2dConst<float>
{
  //! @brief Get math epsilon (float).
  static FOG_INLINE float getMathEpsilon() { return MATH_EPSILON_F; }

  //! @brief Get default flatness (float).
  static FOG_INLINE float getDefaultFlatness() { return 0.20f; }

  //! @brief Get intersection epsilon (float).
  static FOG_INLINE float getIntersectionEpsilon() { return 1.0e-8f; }

  static FOG_INLINE float getCollinearityEpsilon() { return 1e-8f; }

  //! @brief Get coinciding points maximal distance (float).
  static FOG_INLINE float getDistanceEpsilon() { return 1.0e-6f; }

  //! @brief Get epsilon used to prevent from adding degenerate curves (float).
  static FOG_INLINE float getAngleEpsilon() { return MATH_EPSILON_F; }
};

template<>
struct Math2dConst<double>
{
  //! @brief Get math epsilon (double).
  static FOG_INLINE double getMathEpsilon() { return MATH_EPSILON_D; }

  //! @brief Get default flatness (double).
  static FOG_INLINE double getDefaultFlatness() { return 0.20; }

  //! @brief Get intersection epsilon (double).
  static FOG_INLINE double getIntersectionEpsilon() { return 1.0e-30; }

  static FOG_INLINE double getCollinearityEpsilon() { return 1e-30; }

  //! @brief Get coinciding points maximal distance (double).
  static FOG_INLINE double getDistanceEpsilon() { return 1.0e-14; }

  //! TODO: DEPRECATED, remove getAngleEpsilon.
  //! @brief Get epsilon used to prevent from adding degenerate curves (double).
  static FOG_INLINE double getAngleEpsilon() { return MATH_EPSILON_D; }
};

// ============================================================================
// [Fog::Math2d]
// ============================================================================

namespace Math2d {

// ============================================================================
// [Fog::Math2d - DistSquare]
// ============================================================================

static FOG_INLINE float distSquare(float x0, float y0, float x1, float y1)
{
  float dx = x1 - x0;
  float dy = y1 - y0;
  return dx * dx + dy * dy;
}

static FOG_INLINE double distSquare(double x0, double y0, double x1, double y1)
{
  double dx = x1 - x0;
  double dy = y1 - y0;
  return dx * dx + dy * dy;
}

static FOG_INLINE float distSquare(const PointF& p0, const PointF& p1)
{
  PointF v(p1 - p0);
  return v.x * v.x + v.y * v.y;
}

static FOG_INLINE double distSquare(const PointD& p0, const PointD& p1)
{
  PointD v(p1 - p0);
  return v.x * v.x + v.y * v.y;
}

// ============================================================================
// [Fog::Math2d - DistEuclidean]
// ============================================================================

static FOG_INLINE float distEuclidean(float x0, float y0, float x1, float y1)
{
  return Math::sqrt(distSquare(x0, y0, x1, y1));
}

static FOG_INLINE double distEuclidean(double x0, double y0, double x1, double y1)
{
  return Math::sqrt(distSquare(x0, y0, x1, y1));
}

static FOG_INLINE float distEuclidean(const PointF& p0, const PointF& p1)
{
  return Math::sqrt(distSquare(p0, p1));
}

static FOG_INLINE double distEuclidean(const PointD& p0, const PointD& p1)
{
  return Math::sqrt(distSquare(p0, p1));
}

// ============================================================================
// [Fog::Math2d - Half]
// ============================================================================

static FOG_INLINE PointF half(const PointF& p0, const PointF& p1)
{
  PointF result(UNINITIALIZED);
  result.x = (p0.x + p1.x) * 0.5f;
  result.y = (p0.y + p1.y) * 0.5f;
  return result;
}

static FOG_INLINE PointD half(const PointD& p0, const PointD& p1)
{
  PointD result(UNINITIALIZED);
  result.x = (p0.x + p1.x) * 0.5;
  result.y = (p0.y + p1.y) * 0.5;
  return result;
}

// ============================================================================
// [Fog::Math2d - Lerp]
// ============================================================================

static FOG_INLINE PointF lerp(const PointF& p0, const PointF& p1, float t, float inv_t)
{
  PointF result(UNINITIALIZED);
  result.x = inv_t * p0.x + t * p1.x;
  result.y = inv_t * p0.y + t * p1.y;
  return result;
}

static FOG_INLINE PointD lerp(const PointD& p0, const PointD& p1, double t, double inv_t)
{
  PointD result(UNINITIALIZED);
  result.x = inv_t * p0.x + t * p1.x;
  result.y = inv_t * p0.y + t * p1.y;
  return result;
}

// ============================================================================
// [Fog::Math2d - IntersectLine]
// ============================================================================

static FOG_INLINE bool intersectLine(PointF& dst, const PointF& a0, const PointF& a1, const PointF& b0, const PointF& b1)
{
  float num = (a0.y - b0.y) * (b1.x - b0.x) - (a0.x - b0.x) * (b1.y - b0.y);
  float den = (a1.x - a0.x) * (b1.y - b0.y) - (a1.y - a0.y) * (b1.x - b0.x);
  if (Math::abs(den) < Math2dConst<float>::getIntersectionEpsilon()) return false;

  float r = num / den;
  dst.x = a0.x + (a1.x - a0.x) * r;
  dst.y = a0.y + (a1.y - a0.y) * r;
  return true;
}

static FOG_INLINE bool intersectLine(PointD& dst, const PointD& a0, const PointD& a1, const PointD& b0, const PointD& b1)
{
  double num = (a0.y - b0.y) * (b1.x - b0.x) - (a0.x - b0.x) * (b1.y - b0.y);
  double den = (a1.x - a0.x) * (b1.y - b0.y) - (a1.y - a0.y) * (b1.x - b0.x);
  if (Math::abs(den) < Math2dConst<double>::getIntersectionEpsilon()) return false;

  double r = num / den;
  dst.x = a0.x + (a1.x - a0.x) * r;
  dst.y = a0.y + (a1.y - a0.y) * r;
  return true;
}

static FOG_INLINE float crossProduct(const PointF& p0, const PointF& p1, const PointF& p2)
{
  return (p2.x - p1.x) * (p1.y - p0.y) -
         (p2.y - p1.y) * (p1.x - p0.x) ;
}

static FOG_INLINE double crossProduct(const PointD& p0, const PointD& p1, const PointD& p2)
{
  return (p2.x - p1.x) * (p1.y - p0.y) -
         (p2.y - p1.y) * (p1.x - p0.x) ;
}

} // Math2d namespace

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_GEOMETRY_MATH2D_H
