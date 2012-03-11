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
    fog_api.gradientf_ctor(this, gradientType);
  }

  FOG_INLINE GradientF(const GradientF& other)
  {
    fog_api.gradientf_ctorCopyF(this, &other);
  }

  explicit FOG_INLINE GradientF(const GradientD& other)
  {
    fog_api.gradientf_ctorCopyD(this, &other);
  }

  FOG_INLINE ~GradientF()
  {
    fog_api.gradientf_dtor(this);
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

  FOG_INLINE err_t addStop(float offset, const Argb32& argb32) { return _stops->add(offset, argb32); }
  FOG_INLINE err_t addStop(float offset, const Color& color) { return _stops->add(offset, color); }
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
    return fog_api.gradientf_copyF(this, &other);
  }

  FOG_INLINE err_t setGradient(const GradientD& other)
  {
    return fog_api.gradientf_copyD(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    return fog_api.gradientf_reset(this);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const GradientF& other) const
  {
    return fog_api.gradientf_eq(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE GradientF& operator=(const GradientF& other) { setGradient(other); return *this; }
  FOG_INLINE GradientF& operator=(const GradientD& other) { setGradient(other); return *this; }

  FOG_INLINE bool operator==(const GradientF& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const GradientF& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Statics - Equality]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const GradientF* a, const GradientF* b)
  {
    return fog_api.gradientf_eq(a, b);
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)fog_api.gradientf_eq;
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
    fog_api.gradientd_ctor(this, gradientType);
  }

  FOG_INLINE GradientD(const GradientD& other)
  {
    fog_api.gradientd_ctorCopyD(this, &other);
  }

  explicit FOG_INLINE GradientD(const GradientF& other)
  {
    fog_api.gradientd_ctorCopyF(this, &other);
  }

  FOG_INLINE ~GradientD()
  {
    fog_api.gradientd_dtor(this);
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

  FOG_INLINE err_t addStop(float offset, const Argb32& argb32) { return _stops->add(offset, argb32); }
  FOG_INLINE err_t addStop(float offset, const Color& color) { return _stops->add(offset, color); }
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
    return fog_api.gradientd_copyF(this, &other);
  }

  FOG_INLINE err_t setGradient(const GradientD& other)
  {
    return fog_api.gradientd_copyD(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    return fog_api.gradientd_reset(this);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const GradientD& other) const
  {
    return fog_api.gradientd_eq(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE GradientD& operator=(const GradientF& other) { setGradient(other); return *this; }
  FOG_INLINE GradientD& operator=(const GradientD& other) { setGradient(other); return *this; }

  FOG_INLINE bool operator==(const GradientD& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const GradientD& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Statics - Equality]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const GradientD* a, const GradientD* b)
  {
    return fog_api.gradientd_eq(a, b);
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)fog_api.gradientd_eq;
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

// ============================================================================
// [Fog::LinearGradientF]
// ============================================================================

//! @brief Linear gradient (float).
struct FOG_NO_EXPORT LinearGradientF : public GradientF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE LinearGradientF() : GradientF(GRADIENT_TYPE_LINEAR)
  {
  }

  FOG_INLINE LinearGradientF(const LinearGradientF& other);
  explicit FOG_INLINE LinearGradientF(const LinearGradientD& other);

  FOG_INLINE LinearGradientF(const PointF& start, const PointF& end) :
    GradientF(GRADIENT_TYPE_LINEAR)
  {
    _pts[0] = start;
    _pts[1] = end;
  }

  FOG_INLINE ~LinearGradientF()
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setGradient(const LinearGradientF& other);
  FOG_INLINE err_t setGradient(const LinearGradientD& other);

  FOG_INLINE const PointF& getStart() const { return _pts[0]; }
  FOG_INLINE const PointF& getEnd() const { return _pts[1]; }

  FOG_INLINE void setStart(const PointF& p) { _pts[0] = p; }
  FOG_INLINE void setStart(float x, float y) { _pts[0].set(x, y); }

  FOG_INLINE void setEnd(const PointF& p) { _pts[1] = p; }
  FOG_INLINE void setEnd(float x, float y) { _pts[1].set(x, y); }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE LinearGradientF& operator=(const LinearGradientF& other) { setGradient(other); return *this; }
  FOG_INLINE LinearGradientF& operator=(const LinearGradientD& other) { setGradient(other); return *this; }
};

// ============================================================================
// [Fog::LinearGradientD]
// ============================================================================

//! @brief Linear gradient (double).
struct FOG_NO_EXPORT LinearGradientD : public GradientD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE LinearGradientD() : GradientD(GRADIENT_TYPE_LINEAR)
  {
  }

  FOG_INLINE LinearGradientD(const LinearGradientF& other);
  explicit FOG_INLINE LinearGradientD(const LinearGradientD& other);

  FOG_INLINE LinearGradientD(const PointD& start, const PointD& end) :
    GradientD(GRADIENT_TYPE_LINEAR)
  {
    _pts[0] = start;
    _pts[1] = end;
  }

  FOG_INLINE ~LinearGradientD()
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setGradient(const LinearGradientF& other);
  FOG_INLINE err_t setGradient(const LinearGradientD& other);

  FOG_INLINE const PointD& getStart() const { return _pts[0]; }
  FOG_INLINE const PointD& getEnd() const { return _pts[1]; }

  FOG_INLINE void setStart(const PointD& p) { _pts[0] = p; }
  FOG_INLINE void setStart(double x, double y) { _pts[0].set(x, y); }

  FOG_INLINE void setEnd(const PointD& p) { _pts[1] = p; }
  FOG_INLINE void setEnd(double x, double y) { _pts[1].set(x, y); }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE LinearGradientD& operator=(const LinearGradientF& other) { setGradient(other); return *this; }
  FOG_INLINE LinearGradientD& operator=(const LinearGradientD& other) { setGradient(other); return *this; }
};

// ============================================================================
// [Implemented Later]
// ============================================================================

FOG_INLINE LinearGradientF::LinearGradientF(const LinearGradientF& other) : GradientF(other) {}
FOG_INLINE LinearGradientF::LinearGradientF(const LinearGradientD& other) : GradientF(other) {}

FOG_INLINE LinearGradientD::LinearGradientD(const LinearGradientF& other) : GradientD(other) {}
FOG_INLINE LinearGradientD::LinearGradientD(const LinearGradientD& other) : GradientD(other) {}

FOG_INLINE err_t LinearGradientF::setGradient(const LinearGradientF& other) { return GradientF::setGradient(other); }
FOG_INLINE err_t LinearGradientF::setGradient(const LinearGradientD& other) { return GradientF::setGradient(other); }

FOG_INLINE err_t LinearGradientD::setGradient(const LinearGradientF& other) { return GradientD::setGradient(other); }
FOG_INLINE err_t LinearGradientD::setGradient(const LinearGradientD& other) { return GradientD::setGradient(other); }

// ============================================================================
// [Fog::LinearGradientT<>]
// ============================================================================

_FOG_NUM_T(LinearGradient)
_FOG_NUM_F(LinearGradient)
_FOG_NUM_D(LinearGradient)

// ============================================================================
// [Fog::RadialGradientF]
// ============================================================================

//! @brief Radial gradient (float).
struct FOG_NO_EXPORT RadialGradientF : public GradientF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RadialGradientF() : GradientF(GRADIENT_TYPE_RADIAL)
  {
  }

  FOG_INLINE RadialGradientF(const RadialGradientF& other);
  explicit FOG_INLINE RadialGradientF(const RadialGradientD& other);

  FOG_INLINE RadialGradientF(const PointF& center, const PointF& focal, float rad) :
    GradientF(GRADIENT_TYPE_RADIAL)
  {
    _pts[0] = center;
    _pts[1] = focal;
    _pts[2].set(rad, rad);
  }

  FOG_INLINE RadialGradientF(const PointF& center, const PointF& focal, const PointF& rad) :
    GradientF(GRADIENT_TYPE_RADIAL)
  {
    _pts[0] = center;
    _pts[1] = focal;
    _pts[2] = rad;
  }

  FOG_INLINE ~RadialGradientF()
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setGradient(const RadialGradientF& other);
  FOG_INLINE err_t setGradient(const RadialGradientD& other);

  FOG_INLINE const PointF& getCenter() const { return _pts[0]; }
  FOG_INLINE const PointF& getFocal() const { return _pts[1]; }
  FOG_INLINE const PointF& getRadius() const { return _pts[2]; }

  FOG_INLINE void setCenter(const PointF& p) { _pts[0] = p; }
  FOG_INLINE void setCenter(float x, float y) { _pts[0].set(x, y); }

  FOG_INLINE void setFocal(const PointF& p) { _pts[1] = p; }
  FOG_INLINE void setFocal(float x, float y) { _pts[1].set(x, y); }

  FOG_INLINE void setRadius(const PointF& rad) { _pts[2] = rad; }
  FOG_INLINE void setRadius(float r) { _pts[2].set(r, r); }
  FOG_INLINE void setRadius(float rx, float ry) { _pts[2].set(rx, ry); }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE RadialGradientF& operator=(const RadialGradientF& other) { setGradient(other); return *this; }
  FOG_INLINE RadialGradientF& operator=(const RadialGradientD& other) { setGradient(other); return *this; }
};

// ============================================================================
// [Fog::RadialGradientD]
// ============================================================================

//! @brief Radial gradient (double).
struct FOG_NO_EXPORT RadialGradientD : public GradientD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RadialGradientD() : GradientD(GRADIENT_TYPE_RADIAL)
  {
  }

  FOG_INLINE RadialGradientD(const RadialGradientF& other);
  explicit FOG_INLINE RadialGradientD(const RadialGradientD& other);

  FOG_INLINE RadialGradientD(const PointD& center, const PointD& focal, double rad) :
    GradientD(GRADIENT_TYPE_RADIAL)
  {
    _pts[0] = center;
    _pts[1] = focal;
    _pts[2].set(rad, rad);
  }

  FOG_INLINE RadialGradientD(const PointD& center, const PointD& focal, const PointD& rad) :
    GradientD(GRADIENT_TYPE_RADIAL)
  {
    _pts[0] = center;
    _pts[1] = focal;
    _pts[2] = rad;
  }

  FOG_INLINE ~RadialGradientD()
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setGradient(const RadialGradientF& other);
  FOG_INLINE err_t setGradient(const RadialGradientD& other);

  FOG_INLINE const PointD& getCenter() const { return _pts[0]; }
  FOG_INLINE const PointD& getFocal() const { return _pts[1]; }
  FOG_INLINE const PointD& getRadius() const { return _pts[2]; }

  FOG_INLINE void setCenter(const PointD& p) { _pts[0] = p; }
  FOG_INLINE void setCenter(double x, double y) { _pts[0].set(x, y); }

  FOG_INLINE void setFocal(const PointD& p) { _pts[1] = p; }
  FOG_INLINE void setFocal(double x, double y) { _pts[1].set(x, y); }

  FOG_INLINE void setRadius(const PointD& rad) { _pts[2] = rad; }
  FOG_INLINE void setRadius(double r) { _pts[2].set(r, r); }
  FOG_INLINE void setRadius(double rx, double ry) { _pts[2].set(rx, ry); }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE RadialGradientD& operator=(const RadialGradientF& other) { setGradient(other); return *this; }
  FOG_INLINE RadialGradientD& operator=(const RadialGradientD& other) { setGradient(other); return *this; }
};

// ============================================================================
// [Implemented Later]
// ============================================================================

FOG_INLINE RadialGradientF::RadialGradientF(const RadialGradientF& other) : GradientF(other) {}
FOG_INLINE RadialGradientF::RadialGradientF(const RadialGradientD& other) : GradientF(other) {}

FOG_INLINE RadialGradientD::RadialGradientD(const RadialGradientF& other) : GradientD(other) {}
FOG_INLINE RadialGradientD::RadialGradientD(const RadialGradientD& other) : GradientD(other) {}

FOG_INLINE err_t RadialGradientF::setGradient(const RadialGradientF& other) { return GradientF::setGradient(other); }
FOG_INLINE err_t RadialGradientF::setGradient(const RadialGradientD& other) { return GradientF::setGradient(other); }

FOG_INLINE err_t RadialGradientD::setGradient(const RadialGradientF& other) { return GradientD::setGradient(other); }
FOG_INLINE err_t RadialGradientD::setGradient(const RadialGradientD& other) { return GradientD::setGradient(other); }

// ============================================================================
// [Fog::RadialGradientT<>]
// ============================================================================

_FOG_NUM_T(RadialGradient)
_FOG_NUM_F(RadialGradient)
_FOG_NUM_D(RadialGradient)

// ============================================================================
// [Fog::ConicalGradientF]
// ============================================================================

//! @brief Conical gradient (float).
struct FOG_NO_EXPORT ConicalGradientF : public GradientF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE ConicalGradientF() : GradientF(GRADIENT_TYPE_CONICAL)
  {
  }

  FOG_INLINE ConicalGradientF(const ConicalGradientF& other);
  explicit FOG_INLINE ConicalGradientF(const ConicalGradientD& other);

  FOG_INLINE ConicalGradientF(const PointF& center, float angle) :
    GradientF(GRADIENT_TYPE_CONICAL)
  {
    _pts[0] = center;
    _pts[1].x = angle;
  }

  FOG_INLINE ~ConicalGradientF()
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setGradient(const ConicalGradientF& other);
  FOG_INLINE err_t setGradient(const ConicalGradientD& other);

  FOG_INLINE const PointF& getCenter() const { return _pts[0]; }
  FOG_INLINE float getAngle() const { return _pts[1].x; }

  FOG_INLINE void setCenter(const PointF& p) { _pts[0] = p; }
  FOG_INLINE void setCenter(float x, float y) { _pts[0].set(x, y); }

  FOG_INLINE void setAngle(float angle) { _pts[1].x = angle; }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE ConicalGradientF& operator=(const ConicalGradientF& other) { setGradient(other); return *this; }
  FOG_INLINE ConicalGradientF& operator=(const ConicalGradientD& other) { setGradient(other); return *this; }
};

// ============================================================================
// [Fog::ConicalGradientD]
// ============================================================================

//! @brief Conical gradient (double).
struct FOG_NO_EXPORT ConicalGradientD : public GradientD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE ConicalGradientD() : GradientD(GRADIENT_TYPE_CONICAL)
  {
  }

  FOG_INLINE ConicalGradientD(const ConicalGradientF& other);
  explicit FOG_INLINE ConicalGradientD(const ConicalGradientD& other);

  FOG_INLINE ConicalGradientD(const PointD& center, double angle) :
    GradientD(GRADIENT_TYPE_CONICAL)
  {
    _pts[0] = center;
    _pts[1].x = angle;
  }

  FOG_INLINE ~ConicalGradientD()
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setGradient(const ConicalGradientF& other);
  FOG_INLINE err_t setGradient(const ConicalGradientD& other);

  FOG_INLINE const PointD& getCenter() const { return _pts[0]; }
  FOG_INLINE double getAngle() const { return _pts[1].x; }

  FOG_INLINE void setCenter(const PointD& p) { _pts[0] = p; }
  FOG_INLINE void setCenter(double x, double y) { _pts[0].set(x, y); }

  FOG_INLINE void setAngle(double angle) { _pts[1].x = angle; }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE ConicalGradientD& operator=(const ConicalGradientF& other) { setGradient(other); return *this; }
  FOG_INLINE ConicalGradientD& operator=(const ConicalGradientD& other) { setGradient(other); return *this; }
};

// ============================================================================
// [Implemented Later]
// ============================================================================

FOG_INLINE ConicalGradientF::ConicalGradientF(const ConicalGradientF& other) : GradientF(other) {}
FOG_INLINE ConicalGradientF::ConicalGradientF(const ConicalGradientD& other) : GradientF(other) {}

FOG_INLINE ConicalGradientD::ConicalGradientD(const ConicalGradientF& other) : GradientD(other) {}
FOG_INLINE ConicalGradientD::ConicalGradientD(const ConicalGradientD& other) : GradientD(other) {}

FOG_INLINE err_t ConicalGradientF::setGradient(const ConicalGradientF& other) { return GradientF::setGradient(other); }
FOG_INLINE err_t ConicalGradientF::setGradient(const ConicalGradientD& other) { return GradientF::setGradient(other); }

FOG_INLINE err_t ConicalGradientD::setGradient(const ConicalGradientF& other) { return GradientD::setGradient(other); }
FOG_INLINE err_t ConicalGradientD::setGradient(const ConicalGradientD& other) { return GradientD::setGradient(other); }

// ============================================================================
// [Fog::ConicalGradientT<>]
// ============================================================================

_FOG_NUM_T(ConicalGradient)
_FOG_NUM_F(ConicalGradient)
_FOG_NUM_D(ConicalGradient)

// ============================================================================
// [Fog::RectangularGradientF]
// ============================================================================

//! @brief Rectangular gradient (float).
struct FOG_NO_EXPORT RectangularGradientF : public GradientF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RectangularGradientF() : GradientF(GRADIENT_TYPE_RECTANGULAR)
  {
  }

  FOG_INLINE RectangularGradientF(const RectangularGradientF& other);
  explicit FOG_INLINE RectangularGradientF(const RectangularGradientD& other);

  FOG_INLINE RectangularGradientF(
    const PointF& first, const PointF& second, const PointF& focal) :
    GradientF(GRADIENT_TYPE_RECTANGULAR)
  {
    _pts[0] = first;
    _pts[1] = second;
    _pts[2] = focal;
  }

  FOG_INLINE ~RectangularGradientF()
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setGradient(const RectangularGradientF& other);
  FOG_INLINE err_t setGradient(const RectangularGradientD& other);

  FOG_INLINE const PointF& getFirst() const { return _pts[0]; }
  FOG_INLINE const PointF& getSecond() const { return _pts[1]; }
  FOG_INLINE const PointF& getFocal() const { return _pts[2]; }

  FOG_INLINE void setFirst(const PointF& p) { _pts[0] = p; }
  FOG_INLINE void setFirst(float x, float y) { _pts[0].set(x, y); }

  FOG_INLINE void setSecond(const PointF& p) { _pts[1] = p; }
  FOG_INLINE void setSecond(float x, float y) { _pts[1].set(x, y); }

  FOG_INLINE void setFocal(const PointF& p) { _pts[2] = p; }
  FOG_INLINE void setFocal(float x, float y) { _pts[2].set(x, y); }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE RectangularGradientF& operator=(const RectangularGradientF& other) { setGradient(other); return *this; }
  FOG_INLINE RectangularGradientF& operator=(const RectangularGradientD& other) { setGradient(other); return *this; }
};

// ============================================================================
// [Fog::RectangularGradientD]
// ============================================================================

//! @brief Rectangular gradient (double).
struct FOG_NO_EXPORT RectangularGradientD : public GradientD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RectangularGradientD() : GradientD(GRADIENT_TYPE_RECTANGULAR)
  {
  }

  FOG_INLINE RectangularGradientD(const RectangularGradientF& other);
  explicit FOG_INLINE RectangularGradientD(const RectangularGradientD& other);

  FOG_INLINE RectangularGradientD(
    const PointD& first, const PointD& second, const PointD& focal) :
    GradientD(GRADIENT_TYPE_RECTANGULAR)
  {
    _pts[0] = first;
    _pts[1] = second;
    _pts[2] = focal;
  }

  FOG_INLINE ~RectangularGradientD()
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setGradient(const RectangularGradientF& other);
  FOG_INLINE err_t setGradient(const RectangularGradientD& other);

  FOG_INLINE const PointD& getFirst() const { return _pts[0]; }
  FOG_INLINE const PointD& getSecond() const { return _pts[1]; }
  FOG_INLINE const PointD& getFocal() const { return _pts[2]; }

  FOG_INLINE void setFirst(const PointD& p) { _pts[0] = p; }
  FOG_INLINE void setFirst(double x, double y) { _pts[0].set(x, y); }

  FOG_INLINE void setSecond(const PointD& p) { _pts[1] = p; }
  FOG_INLINE void setSecond(double x, double y) { _pts[1].set(x, y); }

  FOG_INLINE void setFocal(const PointD& p) { _pts[2] = p; }
  FOG_INLINE void setFocal(double x, double y) { _pts[2].set(x, y); }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE RectangularGradientD& operator=(const RectangularGradientF& other) { setGradient(other); return *this; }
  FOG_INLINE RectangularGradientD& operator=(const RectangularGradientD& other) { setGradient(other); return *this; }
};

// ============================================================================
// [Implemented Later]
// ============================================================================

FOG_INLINE RectangularGradientF::RectangularGradientF(const RectangularGradientF& other) : GradientF(other) {}
FOG_INLINE RectangularGradientF::RectangularGradientF(const RectangularGradientD& other) : GradientF(other) {}

FOG_INLINE RectangularGradientD::RectangularGradientD(const RectangularGradientF& other) : GradientD(other) {}
FOG_INLINE RectangularGradientD::RectangularGradientD(const RectangularGradientD& other) : GradientD(other) {}

FOG_INLINE err_t RectangularGradientF::setGradient(const RectangularGradientF& other) { return GradientF::setGradient(other); }
FOG_INLINE err_t RectangularGradientF::setGradient(const RectangularGradientD& other) { return GradientF::setGradient(other); }

FOG_INLINE err_t RectangularGradientD::setGradient(const RectangularGradientF& other) { return GradientD::setGradient(other); }
FOG_INLINE err_t RectangularGradientD::setGradient(const RectangularGradientD& other) { return GradientD::setGradient(other); }

// ============================================================================
// [Fog::RectangularGradientT<>]
// ============================================================================

_FOG_NUM_T(RectangularGradient)
_FOG_NUM_F(RectangularGradient)
_FOG_NUM_D(RectangularGradient)

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_SOURCE_GRADIENT_H
