// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_SOURCE_CONICALGRADIENT_H
#define _FOG_G2D_SOURCE_CONICALGRADIENT_H

// [Dependencies]
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Global/TypeVariant.h>
#include <Fog/G2d/Source/Gradient.h>

namespace Fog {

//! @addtogroup Fog_G2d_Source
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct ConicalGradientF;
struct ConicalGradientD;

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

FOG_TYPEVARIANT_DECLARE_F_D(ConicalGradient)

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::ConicalGradientF, Fog::TYPEINFO_MOVABLE)
FOG_DECLARE_TYPEINFO(Fog::ConicalGradientD, Fog::TYPEINFO_MOVABLE)

// [Guard]
#endif // _FOG_G2D_SOURCE_CONICALGRADIENT_H
