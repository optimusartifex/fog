// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_MATH_INTERVAL_H
#define _FOG_CORE_MATH_INTERVAL_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Math/Fuzzy.h>

namespace Fog {

//! @addtogroup Fog_Core_Math
//! @{

// ============================================================================
// [Fog::IntervalF]
// ============================================================================

struct FOG_NO_EXPORT IntervalF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE IntervalF() :  _min(0.0f), _max(0.0f) {}
  FOG_INLINE IntervalF(float min, float max) : _min(min), _max(max) {}
  FOG_INLINE IntervalF(const IntervalF& other) { setInterval(other); }

  explicit FOG_INLINE IntervalF(_Uninitialized) {}
  explicit FOG_INLINE IntervalF(const IntervalD& other) { setInterval(other); }

  // --------------------------------------------------------------------------
  // [Consistency]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isValid() const { return _min <= _max; }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE float getMin() const { return _min; }
  FOG_INLINE float getMax() const { return _max; }

  FOG_INLINE void setMin(float min) { _min = min; }
  FOG_INLINE void setMax(float max) { _max = max; }

  FOG_INLINE void setInterval(float min, float max) { _min = min; _max = max; }
  FOG_INLINE void setInterval(const IntervalF& i) { _min = i._min; _max = i._max; }
  FOG_INLINE void setInterval(const IntervalD& i);

  // --------------------------------------------------------------------------
  // [HasValue]
  // --------------------------------------------------------------------------

  FOG_INLINE bool hasValue(float value) const { return (value >= _min) & (value <= _max); }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _min = 0.0f;
    _max = 0.0f;
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE IntervalF& operator=(const IntervalF& other) { setInterval(other); return *this; }
  FOG_INLINE IntervalF& operator=(const IntervalD& other) { setInterval(other); return *this; }

  FOG_INLINE bool operator==(const IntervalF& other) { return _min == other._min && _max == other._max; }
  FOG_INLINE bool operator!=(const IntervalF& other) { return _min != other._min || _max != other._max; }

  FOG_INLINE IntervalF& operator+=(const float x) { _min += x; _max += x; return *this; }
  FOG_INLINE IntervalF& operator-=(const float x) { _min -= x; _max -= x; return *this; }
  FOG_INLINE IntervalF& operator*=(const float x) { _min *= x; _max *= x; return *this; }
  FOG_INLINE IntervalF& operator/=(const float x) { _min /= x; _max /= x; return *this; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  float _min;
  float _max;
};

// ============================================================================
// [Fog::IntervalD]
// ============================================================================

struct FOG_NO_EXPORT IntervalD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE IntervalD() :  _min(0.0), _max(0.0) {}
  FOG_INLINE IntervalD(double min, double max) : _min(min), _max(max) {}
  FOG_INLINE IntervalD(const IntervalD& other) { setInterval(other); }

  explicit FOG_INLINE IntervalD(_Uninitialized) {}
  explicit FOG_INLINE IntervalD(const IntervalF& other) { setInterval(other); }

  // --------------------------------------------------------------------------
  // [Consistency]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isValid() const { return _min <= _max; }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE double getMin() const { return _min; }
  FOG_INLINE double getMax() const { return _max; }

  FOG_INLINE void setMin(double min) { _min = min; }
  FOG_INLINE void setMax(double max) { _max = max; }

  FOG_INLINE void setInterval(double min, double max) { _min = min; _max = max; }
  FOG_INLINE void setInterval(const IntervalF& i) { _min = i._min; _max = i._max; }
  FOG_INLINE void setInterval(const IntervalD& i) { _min = i._min; _max = i._max; }

  // --------------------------------------------------------------------------
  // [HasValue]
  // --------------------------------------------------------------------------

  FOG_INLINE bool hasValue(double value) const { return (value >= _min) & (value <= _max); }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _min = 0.0;
    _max = 0.0;
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE IntervalD& operator=(const IntervalF& other) { setInterval(other); return *this; }
  FOG_INLINE IntervalD& operator=(const IntervalD& other) { setInterval(other); return *this; }

  FOG_INLINE bool operator==(const IntervalD& other) { return _min == other._min && _max == other._max; }
  FOG_INLINE bool operator!=(const IntervalD& other) { return _min != other._min || _max != other._max; }

  FOG_INLINE IntervalD& operator+=(const double x) { _min += x; _max += x; return *this; }
  FOG_INLINE IntervalD& operator-=(const double x) { _min -= x; _max -= x; return *this; }
  FOG_INLINE IntervalD& operator*=(const double x) { _min *= x; _max *= x; return *this; }
  FOG_INLINE IntervalD& operator/=(const double x) { _min /= x; _max /= x; return *this; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  double _min;
  double _max;
};

// ============================================================================
// [...]
// ============================================================================

FOG_INLINE void IntervalF::setInterval(const IntervalD& i)
{
  _min = (float)i._min;
  _max = (float)i._max;
}

// ============================================================================
// [Fog::IntervalT<>]
// ============================================================================

_FOG_NUM_T(Interval)
_FOG_NUM_F(Interval)
_FOG_NUM_D(Interval)

//! @}

} // Fog namespace

// ============================================================================
// [Fog::Fuzzy<>]
// ============================================================================

FOG_FUZZY_DECLARE_F_VEC(Fog::IntervalF, 2)
FOG_FUZZY_DECLARE_D_VEC(Fog::IntervalD, 2)

// [Guard]
#endif // _FOG_CORE_MATH_INTERVAL_H
