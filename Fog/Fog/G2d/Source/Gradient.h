// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_SOURCE_GRADIENT_H
#define _FOG_G2D_SOURCE_GRADIENT_H

// [Dependencies]
#include <Fog/Core/Global/Class.h>
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/G2d/Global/Constants.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Source/Color.h>
#include <Fog/G2d/Source/ColorStop.h>
#include <Fog/G2d/Source/ColorStopCache.h>
#include <Fog/G2d/Source/ColorStopList.h>

namespace Fog {

//! @addtogroup Fog_G2d_Source
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct GradientF;
struct GradientD;

// ============================================================================
// [Fog::GradientF]
// ============================================================================

//! @brief Gradient (float).
struct FOG_API GradientF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  GradientF(uint32_t gradientType = GRADIENT_TYPE_INVALID);
  GradientF(const GradientF& other);
  explicit GradientF(const GradientD& other);
  ~GradientF();

  // --------------------------------------------------------------------------
  // [Type]
  // --------------------------------------------------------------------------

  //! @brief Get the gradient type.
  FOG_INLINE uint32_t getGradientType() const { return _gradientType; }

  //! @brief Set the gradient type.
  FOG_INLINE void setGradientType(uint32_t gradientType)
  {
    FOG_ASSERT(gradientType < GRADIENT_TYPE_COUNT);
    _gradientType = gradientType;
  }

  // --------------------------------------------------------------------------
  // [Spread]
  // --------------------------------------------------------------------------

  //! @brief Get the gradient spread.
  FOG_INLINE uint32_t getGradientSpread() const { return _gradientSpread; }

  //! @brief Set the gradient spread.
  FOG_INLINE void setGradientSpread(uint32_t gradientSpread)
  {
    FOG_ASSERT(gradientSpread < GRADIENT_SPREAD_COUNT);
    _gradientSpread = gradientSpread;
  }

  // --------------------------------------------------------------------------
  // [Stops]
  // --------------------------------------------------------------------------

  FOG_INLINE const ColorStopList& getStops() const { return _stops; }
  FOG_INLINE err_t setStops(const ColorStopList& list) { return _stops.setList(list); }
  FOG_INLINE err_t setStops(const List<ColorStop>& stops) { return _stops.setList(stops); }
  FOG_INLINE err_t setStops(const ColorStop* stops, size_t length) { return _stops.setList(stops, length); }
  FOG_INLINE void resetStops() { _stops.clear(); }

  FOG_INLINE err_t addStop(const ColorStop& stop) { return _stops.add(stop); }

  FOG_INLINE err_t removeStop(float offset) { return _stops.remove(offset); }
  FOG_INLINE err_t removeStop(const ColorStop& stop) { return _stops.remove(stop); }

  FOG_INLINE err_t removeStopAt(size_t index) { return _stops.removeAt(index); }
  FOG_INLINE err_t removeStopAt(const Range& range) { return _stops.removeAt(range); }
  FOG_INLINE err_t removeStopAt(const IntervalF& interval) { return _stops.removeAt(interval); }

  // --------------------------------------------------------------------------
  // [Abstract]
  // --------------------------------------------------------------------------

  FOG_INLINE const PointF* getPoints() const { return _pts; }
  FOG_INLINE PointF* getPoints() { return _pts; }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  err_t setGradient(const GradientF& other);
  err_t setGradient(const GradientD& other);

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  void reset();

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE GradientF& operator=(const GradientF& other) { setGradient(other); return *this; }
  FOG_INLINE GradientF& operator=(const GradientD& other) { setGradient(other); return *this; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief The gradient type.
  uint32_t _gradientType;
  //! @brief The gradient spread.
  uint32_t _gradientSpread;
  //! @brief Color stops.
  ColorStopList _stops;

  //! @brief Points data (meaning depends to the gradient type).
  //!
  //! Linear gradient:
  //! - pts[0] - Start point.
  //! - pts[1] - End point.
  //!
  //! Radial gradient:
  //! - pts[0] - Center point.
  //! - pts[1] - Focal point.
  //! - pts[2] - Radius (x, y).
  //!
  //! Conical gradient:
  //! - pts[0] - Center point.
  //! - pts[1] - Angle (x), sweep (y).
  //!
  //! Rectangular gradient.
  //! - pts[0] - First point.
  //! - pts[1] - Second point.
  //! - pts[2] - Focal point.
  PointF _pts[4];
};

// ============================================================================
// [Fog::GradientD]
// ============================================================================

//! @brief Gradient (double).
struct FOG_API GradientD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  GradientD(uint32_t gradientType = PATTERN_TYPE_NONE);
  explicit GradientD(const GradientF& other);
  GradientD(const GradientD& other);
  ~GradientD();

  // --------------------------------------------------------------------------
  // [Type / Spread]
  // --------------------------------------------------------------------------

  //! @brief Get the pattern type.
  FOG_INLINE uint32_t getGradientType() const { return _gradientType; }

  //! @brief Set the pattern type.
  FOG_INLINE void setGradientType(uint32_t gradientType)
  {
    FOG_ASSERT(gradientType < GRADIENT_TYPE_COUNT);
    _gradientType = gradientType;
  }

  // --------------------------------------------------------------------------
  // [Spread]
  // --------------------------------------------------------------------------

  //! @brief Get the spread type.
  FOG_INLINE uint32_t getGradientSpread() const { return _gradientSpread; }

  //! @brief Set the gradient spread.
  FOG_INLINE void setGradientSpread(uint32_t gradientSpread)
  {
    FOG_ASSERT(gradientSpread < GRADIENT_SPREAD_COUNT);
    _gradientSpread = gradientSpread;
  }

  // --------------------------------------------------------------------------
  // [Stops]
  // --------------------------------------------------------------------------

  FOG_INLINE const ColorStopList& getStops() const { return _stops; }
  FOG_INLINE err_t setStops(const ColorStopList& list) { return _stops.setList(list); }
  FOG_INLINE err_t setStops(const List<ColorStop>& stops) { return _stops.setList(stops); }
  FOG_INLINE err_t setStops(const ColorStop* stops, size_t length) { return _stops.setList(stops, length); }
  FOG_INLINE void resetStops() { _stops.clear(); }

  FOG_INLINE err_t addStop(const ColorStop& stop) { return _stops.add(stop); }

  FOG_INLINE err_t removeStop(float offset) { return _stops.remove(offset); }
  FOG_INLINE err_t removeStop(const ColorStop& stop) { return _stops.remove(stop); }

  FOG_INLINE err_t removeStopAt(size_t index) { return _stops.removeAt(index); }
  FOG_INLINE err_t removeStopAt(const Range& range) { return _stops.removeAt(range); }
  FOG_INLINE err_t removeStopAt(const IntervalF& interval) { return _stops.removeAt(interval); }

  // --------------------------------------------------------------------------
  // [Abstract]
  // --------------------------------------------------------------------------

  FOG_INLINE const PointD* getPoints() const { return _pts; }
  FOG_INLINE PointD* getPoints() { return _pts; }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  err_t setGradient(const GradientF& other);
  err_t setGradient(const GradientD& other);

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  void reset();

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE GradientD& operator=(const GradientF& other) { setGradient(other); return *this; }
  FOG_INLINE GradientD& operator=(const GradientD& other) { setGradient(other); return *this; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief The gradient type.
  uint32_t _gradientType;
  //! @brief The gradient spread.
  uint32_t _gradientSpread;
  //! @brief Color stops.
  ColorStopList _stops;

  //! @brief Points data (meaning depends to the gradient type).
  //!
  //! Linear gradient:
  //! - pts[0] - Start point.
  //! - pts[1] - End point.
  //!
  //! Radial gradient:
  //! - pts[0] - Center point.
  //! - pts[1] - Focal point.
  //! - pts[2] - Radius (x, y).
  //!
  //! Conical gradient:
  //! - pts[0] - Center point.
  //! - pts[1] - Angle (x), sweep (y).
  //!
  //! Rectangular gradient.
  //! - pts[0] - First point.
  //! - pts[1] - Second point.
  //! - pts[2] - Focal point.
  PointD _pts[4];
};

// ============================================================================
// [Fog::GradientT<>]
// ============================================================================

FOG_CLASS_PRECISION_F_D(Gradient)

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPEINFO_DECLARE(Fog::GradientF, Fog::TYPEINFO_MOVABLE)
_FOG_TYPEINFO_DECLARE(Fog::GradientD, Fog::TYPEINFO_MOVABLE)

// [Guard]
#endif // _FOG_G2D_SOURCE_GRADIENT_H
