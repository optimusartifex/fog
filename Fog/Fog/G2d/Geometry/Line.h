// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_GEOMETRY_LINE_H
#define _FOG_G2D_GEOMETRY_LINE_H

// [Dependencies]
#include <Fog/Core/Global/Swap.h>
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Global/TypeVariant.h>
#include <Fog/Core/Global/Uninitialized.h>
#include <Fog/Core/Math/Fuzzy.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/Memory.h>
#include <Fog/G2d/Geometry/Box.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Global/Api.h>

namespace Fog {

//! @addtogroup Fog_G2d_Geometry
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct LineF;
struct LineD;

// ============================================================================
// [Fog::LineF]
// ============================================================================

//! @brief Line (float).
struct FOG_NO_EXPORT LineF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE LineF() { reset(); }
  FOG_INLINE LineF(_Uninitialized) {}

  FOG_INLINE LineF(const LineF& other) { p[0] = other.p[0]; p[1] = other.p[1]; }
  FOG_INLINE LineF(const PointF& pt0, const PointF& pt1) { p[0] = pt0; p[1] = pt1; }
  FOG_INLINE LineF(float x0, float y0, float x1, float y1) { p[0].set(x0, y0); p[1].set(x1, y1); }
  FOG_INLINE LineF(const PointF* pts) { p[0] = pts[0]; p[1] = pts[1]; }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const PointF* getPoints() const { return p; }
  FOG_INLINE const PointF& getFirst() const { return p[0]; }
  FOG_INLINE const PointF& getLast() const { return p[1]; }

  FOG_INLINE const PointF getVector() const { return PointF(p[1].x - p[0].x, p[1].y - p[0].y); }
  FOG_INLINE float getLength() const { return Math::hypot(p[1].x - p[0].x, p[1].y - p[0].y); }
  FOG_INLINE float getAngle() const { return Math::atan2(p[0].y - p[1].y, p[1].x - p[0].x); }

  FOG_INLINE void setPoints(const PointF* pts) { p[0] = pts[0]; p[1] = pts[1]; }
  FOG_INLINE void setFirst(const PointF& pt0) { p[0] = pt0; }
  FOG_INLINE void setLast(const PointF& pt1) { p[1] = pt1; }

  FOG_INLINE void setAngle(float rad)
  {
    float len = getLength();
    float aSin, aCos;

    Math::sincos(rad, &aSin, &aCos);
    p[1].set(p[0].x + aCos * len, p[0].y - aSin * len);
  }

  FOG_INLINE void setLine(const LineF& line) { p[0] = line.p[0]; p[1] = line.p[1]; }
  FOG_INLINE void setLine(const PointF& pt0, const PointF& pt1) { p[0] = pt0; p[1] = pt1; }
  FOG_INLINE void setLine(float x0, float y0, float x1, float y1) { p[0].set(x0, y0); p[1].set(x1, y1); }
  
  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset() { p[0].reset(); p[1].reset(); }

  // --------------------------------------------------------------------------
  // [BoundingRect]
  // --------------------------------------------------------------------------

  FOG_INLINE RectF getBoundingRect() const
  {
    float x0 = p[0].x;
    float y0 = p[0].y;
    float x1 = p[1].x;
    float y1 = p[1].y;

    if (x0 > x1) swap(x0, x1);
    if (y0 > y1) swap(y0, y1);

    return RectF(x0, y0, x1 - x0, y1 - y0);
  }

  // --------------------------------------------------------------------------
  // [Transform]
  // --------------------------------------------------------------------------

  FOG_INLINE void translate(const PointF& pt)
  {
    p[0] += pt;
    p[1] += pt;
  }

  // --------------------------------------------------------------------------
  // [Convert]
  // --------------------------------------------------------------------------

  FOG_INLINE LineD toLineD() const;

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE LineF& operator=(const LineF& other)
  {
    setLine(other);
    return *this;
  }

  FOG_INLINE bool operator==(const LineF& other)
  {
    return p[0] == other.p[0] && p[1] == other.p[1];
  }

  FOG_INLINE bool operator!=(const LineF& other)
  {
    return !operator==(other);
  }

  // --------------------------------------------------------------------------
  // [Algebra]
  // --------------------------------------------------------------------------

  static FOG_INLINE uint32_t intersect(PointF& dst, const LineF& a, const LineF& b)
  {
    _g2d.linef.intersect(&dst, a.p, b.p);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  PointF p[2];
};

// ============================================================================
// [Fog::LineD]
// ============================================================================

//! @brief Line (double).
struct FOG_NO_EXPORT LineD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE LineD() { reset(); }
  FOG_INLINE LineD(_Uninitialized) {}

  FOG_INLINE LineD(const LineD& other) { p[0] = other.p[0]; p[1] = other.p[1]; }
  FOG_INLINE LineD(const PointD& pt0, const PointD& pt1) { p[0] = pt0; p[1] = pt1; }
  FOG_INLINE LineD(double x0, double y0, double x1, double y1) { p[0].set(x0, y0); p[1].set(x1, y1); }
  FOG_INLINE LineD(const PointD* pts) { p[0] = pts[0]; p[1] = pts[1]; }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const PointD* getPoints() const { return p; }
  FOG_INLINE const PointD& getFirst() const { return p[0]; }
  FOG_INLINE const PointD& getLast() const { return p[1]; }

  FOG_INLINE const PointD getVector() const { return PointD(p[1].x - p[0].x, p[1].y - p[0].y); }
  FOG_INLINE double getLength() const { return Math::hypot(p[1].x - p[0].x, p[1].y - p[0].y); }
  FOG_INLINE double getAngle() const { return Math::atan2(p[0].y - p[1].y, p[1].x - p[0].x); }

  FOG_INLINE void setPoints(const PointD* pts) { p[0] = pts[0]; p[1] = pts[1]; }
  FOG_INLINE void setFirst(const PointD& pt0) { p[0] = pt0; }
  FOG_INLINE void setLast(const PointD& pt1) { p[1] = pt1; }

  FOG_INLINE void setAngle(double rad)
  {
    double len = getLength();
    double aSin, aCos;

    Math::sincos(rad, &aSin, &aCos);
    p[1].set(p[0].x + aCos * len, p[0].y - aSin * len);
  }

  FOG_INLINE void setLine(const LineD& line) { p[0] = line.p[0]; p[1] = line.p[1]; }
  FOG_INLINE void setLine(const PointD& pt0, const PointD& pt1) { p[0] = pt0; p[1] = pt1; }
  FOG_INLINE void setLine(double x0, double y0, double x1, double y1) { p[0].set(x0, y0); p[1].set(x1, y1); }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset() { p[0].reset(); p[1].reset(); }

  // --------------------------------------------------------------------------
  // [BoundingRect]
  // --------------------------------------------------------------------------

  FOG_INLINE RectD getBoundingRect() const
  {
    double x0 = p[0].x;
    double y0 = p[0].y;
    double x1 = p[1].x;
    double y1 = p[1].y;

    if (x0 > x1) swap(x0, x1);
    if (y0 > y1) swap(y0, y1);

    return RectD(x0, y0, x1 - x0, y1 - y0);
  }

  // --------------------------------------------------------------------------
  // [Transform]
  // --------------------------------------------------------------------------

  FOG_INLINE void translate(const PointD& pt)
  {
    p[0] += pt;
    p[1] += pt;
  }

  // --------------------------------------------------------------------------
  // [Convert]
  // --------------------------------------------------------------------------

  FOG_INLINE LineF toLineF() const;

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE LineD& operator=(const LineD& other)
  {
    setLine(other);
    return *this;
  }

  FOG_INLINE bool operator==(const LineD& other)
  {
    return p[0] == other.p[0] && p[1] == other.p[1];
  }

  FOG_INLINE bool operator!=(const LineD& other)
  {
    return !operator==(other);
  }

  // --------------------------------------------------------------------------
  // [Algebra]
  // --------------------------------------------------------------------------

  static FOG_INLINE uint32_t intersect(PointD& dst, const LineD& a, const LineD& b)
  {
    _g2d.lined.intersect(&dst, a.p, b.p);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  PointD p[2];
};

// ============================================================================
// [Implemented-Later]
// ============================================================================

FOG_INLINE LineD LineF::toLineD() const { return LineD(p[0].toPointD(), p[1].toPointD()); }
FOG_INLINE LineF LineD::toLineF() const { return LineF(p[0].toPointF(), p[1].toPointF()); }

// ============================================================================
// [Fog::LineT<>]
// ============================================================================

FOG_TYPEVARIANT_DECLARE_F_D(Line)

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::LineF, Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::LineD, Fog::TYPEINFO_PRIMITIVE)

// ============================================================================
// [Fog::Fuzzy<>]
// ============================================================================

FOG_DECLARE_FUZZY(Fog::LineF, Math::feqv((const float *)&a, (const float *)&b, 4))
FOG_DECLARE_FUZZY(Fog::LineD, Math::feqv((const double*)&a, (const double*)&b, 4))

// [Guard]
#endif // _FOG_G2D_GEOMETRY_LINE_H
