// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_GRADIENT_H
#define _FOG_GRAPHICS_GRADIENT_H

// [Dependencies]
#include <Fog/Core/Static.h>
#include <Fog/Graphics/Color.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Geometry.h>

namespace Fog {

//! @addtogroup Fog_Graphics_Painting
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct Pattern;

// ============================================================================
// [Fog::Gradient]
// ============================================================================

//! @brief Gradient pattern helper class.
struct FOG_HIDDEN Gradient
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE Gradient() :
    _type(PATTERN_TYPE_LINEAR_GRADIENT),
    _spread(PATTERN_SPREAD_PAD),
    _start(0.0, 0.0),
    _end(0.0, 0.0),
    _radius(0.0)
  {
  }

  FOG_INLINE Gradient(const Gradient& other) :
    _type(other._type),
    _spread(other._spread),
    _start(other._start),
    _end(other._end),
    _radius(other._radius),
    _stops(other._stops)
  {
  }

  FOG_INLINE Gradient(uint32_t type, uint32_t spread, const DoublePoint& start, const DoublePoint& end, const List<ArgbStop>& stops) :
    _type((type < PATTERN_TYPE_LINEAR_GRADIENT) | (type > PATTERN_TYPE_CONICAL_GRADIENT) ? PATTERN_TYPE_LINEAR_GRADIENT : type),
    _spread(spread >= PATTERN_SPREAD_COUNT ? PATTERN_SPREAD_PAD : spread),
    _start(start),
    _end(end),
    _radius(0.0),
    _stops(stops)
  {
  }

  FOG_INLINE Gradient(uint32_t type, uint32_t spread, const DoublePoint& start, const DoublePoint& end, double radius, const List<ArgbStop>& stops) :
    _type((type < PATTERN_TYPE_LINEAR_GRADIENT) | (type > PATTERN_TYPE_CONICAL_GRADIENT) ? PATTERN_TYPE_LINEAR_GRADIENT : type),
    _spread(spread >= PATTERN_SPREAD_COUNT ? PATTERN_SPREAD_PAD : spread),
    _start(start),
    _end(end),
    _radius(radius),
    _stops(stops)
  {
  }

  FOG_INLINE ~Gradient()
  {
  }

  // --------------------------------------------------------------------------
  // [Type]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getType() const
  {
    return _type;
  }

  FOG_INLINE void setType(uint32_t type)
  {
    if ((type < PATTERN_TYPE_LINEAR_GRADIENT) | (type > PATTERN_TYPE_CONICAL_GRADIENT)) return;
    _type = type;
  }

  // --------------------------------------------------------------------------
  // [Spread]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getSpread() const
  {
    return _spread;
  }

  FOG_INLINE void setSpread(uint32_t spread)
  {
    if ((uint32_t)spread >= PATTERN_SPREAD_COUNT) return;
    _spread = spread;
  }

  // --------------------------------------------------------------------------
  // [Points]
  // --------------------------------------------------------------------------

  FOG_INLINE const DoublePoint& getStartPoint() const { return _start; }
  FOG_INLINE const DoublePoint& getEndPoint() const { return _end; }

  FOG_INLINE void setStartPoint(const IntPoint& pt) { _start = pt; }
  FOG_INLINE void setStartPoint(const DoublePoint& pt) { _start = pt; }

  FOG_INLINE void setEndPoint(const IntPoint& pt) { _end = pt; }
  FOG_INLINE void setEndPoint(const DoublePoint& pt) { _end = pt; }

  FOG_INLINE void setPoints(const IntPoint& startPt, const IntPoint& endPt) { _start = startPt; _end = endPt; }
  FOG_INLINE void setPoints(const DoublePoint& startPt, const DoublePoint& endPt) { _start = startPt; _end = endPt; }

  // --------------------------------------------------------------------------
  // [Radius]
  // --------------------------------------------------------------------------

  FOG_INLINE double getRadius() const { return _radius; }
  FOG_INLINE void setRadius(double r) { _radius = r; }

  // --------------------------------------------------------------------------
  // [Stops]
  // --------------------------------------------------------------------------

  FOG_INLINE const List<ArgbStop>& getStops() const { return _stops; }
  FOG_INLINE void setStops(const List<ArgbStop>& stops) { _stops = stops; }
  FOG_INLINE void resetStops() { _stops.clear(); }

  FOG_INLINE err_t addStop(const ArgbStop& stop) { return _stops.append(stop); }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE Gradient& operator=(const Gradient& other)
  {
    _type = other._type;
    _spread = other._spread;
    _start = other._start;
    _end = other._end;
    _radius = other._radius;
    _stops = other._stops;

    return *this;
  }

protected:
  uint32_t _type;
  uint32_t _spread;
  DoublePoint _start;
  DoublePoint _end;
  double _radius;
  List<ArgbStop> _stops;

  friend struct Pattern;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GRAPHICS_GRADIENT_H
