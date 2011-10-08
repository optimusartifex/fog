// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_SOURCE_GRADIENT_H
#define _FOG_G2D_SOURCE_GRADIENT_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
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
// [Fog::GradientF]
// ============================================================================

//! @brief Gradient (float).
struct FOG_NO_EXPORT GradientF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE GradientF(uint32_t gradientType = GRADIENT_TYPE_INVALID)
  {
    _api.gradientf_ctor(this, gradientType);
  }

  FOG_INLINE GradientF(const GradientF& other)
  {
    _api.gradientf_ctorCopyF(this, &other);
  }

  explicit FOG_INLINE GradientF(const GradientD& other)
  {
    _api.gradientf_ctorCopyD(this, &other);
  }

  FOG_INLINE ~GradientF()
  {
    _api.gradientf_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Type]
  // --------------------------------------------------------------------------

  //! @brief Get the gradient type.
  FOG_INLINE uint32_t getGradientType() const
  {
    return _gradientType;
  }

  //! @brief Set the gradient type.
  FOG_INLINE void setGradientType(uint32_t gradientType)
  {
    FOG_ASSERT_X(gradientType < GRADIENT_TYPE_COUNT,
      "Fog::GradientF::setGradientType() - Invalid type.");

    _gradientType = gradientType;
  }

  // --------------------------------------------------------------------------
  // [Spread]
  // --------------------------------------------------------------------------

  //! @brief Get the gradient spread.
  FOG_INLINE uint32_t getGradientSpread() const
  {
    return _gradientSpread;
  }

  //! @brief Set the gradient spread.
  FOG_INLINE void setGradientSpread(uint32_t gradientSpread)
  {
    FOG_ASSERT_X(gradientSpread < GRADIENT_SPREAD_COUNT,
      "Fog::GradientF::setGradientType() - Invalid spread.");

    _gradientSpread = gradientSpread;
  }

  // --------------------------------------------------------------------------
  // [Stops]
  // --------------------------------------------------------------------------

  FOG_INLINE const ColorStopList& getStops() const { return _stops; }
  FOG_INLINE err_t setStops(const ColorStopList& list) { return _stops->setList(list); }
  FOG_INLINE err_t setStops(const List<ColorStop>& stops) { return _stops->setList(stops); }
  FOG_INLINE err_t setStops(const ColorStop* stops, size_t length) { return _stops->setList(stops, length); }

  FOG_INLINE void clearStops() { _stops->clear(); }
  FOG_INLINE void resetStops() { _stops->reset(); }

  FOG_INLINE err_t addStop(const ColorStop& stop) { return _stops->add(stop); }

  FOG_INLINE err_t removeStop(float offset) { return _stops->remove(offset); }
  FOG_INLINE err_t removeStop(const ColorStop& stop) { return _stops->remove(stop); }

  FOG_INLINE err_t removeStopAt(size_t index) { return _stops->removeAt(index); }
  FOG_INLINE err_t removeStopAt(const Range& range) { return _stops->removeRange(range); }
  FOG_INLINE err_t removeStopAt(const IntervalF& interval) { return _stops->removeRange(interval); }

  // --------------------------------------------------------------------------
  // [Abstract]
  // --------------------------------------------------------------------------

  FOG_INLINE const PointF* getPoints() const { return _pts; }
  FOG_INLINE PointF* getPoints() { return _pts; }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setGradient(const GradientF& other)
  {
    return _api.gradientf_copyF(this, &other);
  }

  FOG_INLINE err_t setGradient(const GradientD& other)
  {
    return _api.gradientf_copyD(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    return _api.gradientf_reset(this);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const GradientF& other) const
  {
    return _api.gradientf_eq(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE GradientF& operator=(const GradientF& other) { setGradient(other); return *this; }
  FOG_INLINE GradientF& operator=(const GradientD& other) { setGradient(other); return *this; }

  FOG_INLINE bool operator==(const GradientF& other) { return  eq(other); }
  FOG_INLINE bool operator!=(const GradientF& other) { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Statics - Equality]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const GradientF* a, const GradientF* b)
  {
    return _api.gradientf_eq(a, b);
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)_api.gradientf_eq;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief The gradient type.
  uint32_t _gradientType;
  //! @brief The gradient spread.
  uint32_t _gradientSpread;

  //! @brief Color stops.
  Static<ColorStopList> _stops;

  //! @brief Points data (meaning depends on the gradient type).
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
  PointF _pts[3];
};

// ============================================================================
// [Fog::GradientD]
// ============================================================================

//! @brief Gradient (double).
struct FOG_NO_EXPORT GradientD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE GradientD(uint32_t gradientType = GRADIENT_TYPE_INVALID)
  {
    _api.gradientd_ctor(this, gradientType);
  }

  FOG_INLINE GradientD(const GradientD& other)
  {
    _api.gradientd_ctorCopyD(this, &other);
  }

  explicit FOG_INLINE GradientD(const GradientF& other)
  {
    _api.gradientd_ctorCopyF(this, &other);
  }

  FOG_INLINE ~GradientD()
  {
    _api.gradientd_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Type / Spread]
  // --------------------------------------------------------------------------

  //! @brief Get the pattern type.
  FOG_INLINE uint32_t getGradientType() const
  {
    return _gradientType;
  }

  //! @brief Set the pattern type.
  FOG_INLINE void setGradientType(uint32_t gradientType)
  {
    FOG_ASSERT_X(gradientType < GRADIENT_TYPE_COUNT,
      "Fog::GradientD::setGradientType() - Invalid type.");

    _gradientType = gradientType;
  }

  // --------------------------------------------------------------------------
  // [Spread]
  // --------------------------------------------------------------------------

  //! @brief Get the spread type.
  FOG_INLINE uint32_t getGradientSpread() const
  {
    return _gradientSpread;
  }

  //! @brief Set the gradient spread.
  FOG_INLINE void setGradientSpread(uint32_t gradientSpread)
  {
    FOG_ASSERT_X(gradientSpread < GRADIENT_SPREAD_COUNT,
      "Fog::GradientD::setGradientType() - Invalid spread.");

    _gradientSpread = gradientSpread;
  }

  // --------------------------------------------------------------------------
  // [Stops]
  // --------------------------------------------------------------------------

  FOG_INLINE const ColorStopList& getStops() const { return _stops; }
  FOG_INLINE err_t setStops(const ColorStopList& list) { return _stops->setList(list); }
  FOG_INLINE err_t setStops(const List<ColorStop>& stops) { return _stops->setList(stops); }
  FOG_INLINE err_t setStops(const ColorStop* stops, size_t length) { return _stops->setList(stops, length); }

  FOG_INLINE void clearStops() { _stops->clear(); }
  FOG_INLINE void resetStops() { _stops->reset(); }

  FOG_INLINE err_t addStop(const ColorStop& stop) { return _stops->add(stop); }

  FOG_INLINE err_t removeStop(float offset) { return _stops->remove(offset); }
  FOG_INLINE err_t removeStop(const ColorStop& stop) { return _stops->remove(stop); }

  FOG_INLINE err_t removeStopAt(size_t index) { return _stops->removeAt(index); }
  FOG_INLINE err_t removeStopAt(const Range& range) { return _stops->removeRange(range); }
  FOG_INLINE err_t removeStopAt(const IntervalF& interval) { return _stops->removeRange(interval); }

  // --------------------------------------------------------------------------
  // [Abstract]
  // --------------------------------------------------------------------------

  FOG_INLINE const PointD* getPoints() const { return _pts; }
  FOG_INLINE PointD* getPoints() { return _pts; }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setGradient(const GradientF& other)
  {
    return _api.gradientd_copyF(this, &other);
  }

  FOG_INLINE err_t setGradient(const GradientD& other)
  {
    return _api.gradientd_copyD(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    return _api.gradientd_reset(this);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const GradientD& other) const
  {
    return _api.gradientd_eq(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE GradientD& operator=(const GradientF& other) { setGradient(other); return *this; }
  FOG_INLINE GradientD& operator=(const GradientD& other) { setGradient(other); return *this; }

  FOG_INLINE bool operator==(const GradientD& other) { return  eq(other); }
  FOG_INLINE bool operator!=(const GradientD& other) { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Statics - Equality]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const GradientD* a, const GradientD* b)
  {
    return _api.gradientd_eq(a, b);
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)_api.gradientd_eq;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief The gradient type.
  uint32_t _gradientType;
  //! @brief The gradient spread.
  uint32_t _gradientSpread;

  //! @brief Color stops.
  Static<ColorStopList> _stops;

  //! @brief Points data (meaning depends on the gradient type).
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
  PointD _pts[3];
};

// ============================================================================
// [Fog::GradientT<>]
// ============================================================================

_FOG_NUM_T(Gradient)
_FOG_NUM_F(Gradient)
_FOG_NUM_D(Gradient)

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_SOURCE_GRADIENT_H
