// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_SOURCE_RADIALGRADIENT_H
#define _FOG_G2D_SOURCE_RADIALGRADIENT_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/G2d/Source/Gradient.h>

namespace Fog {

//! @addtogroup Fog_G2d_Source
//! @{

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

  FOG_INLINE void setRadius(float rad) { _pts[2].set(rad, rad); }
  FOG_INLINE void setRadius(const PointF& rad) { _pts[2] = rad; }

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

  FOG_INLINE void setRadius(double rad) { _pts[2].set(rad, rad); }
  FOG_INLINE void setRadius(const PointD& rad) { _pts[2] = rad; }

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

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_SOURCE_RADIALGRADIENT_H
