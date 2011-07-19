// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_SOURCE_RECTANGULARGRADIENT_H
#define _FOG_G2D_SOURCE_RECTANGULARGRADIENT_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/G2d/Source/Gradient.h>

namespace Fog {

//! @addtogroup Fog_G2d_Source
//! @{

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
  FOG_INLINE void setSecond(const PointF& p) { _pts[1] = p; }
  FOG_INLINE void setFocal(const PointF& p) { _pts[2] = p; }

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
  FOG_INLINE void setSecond(const PointD& p) { _pts[1] = p; }
  FOG_INLINE void setFocal(const PointD& p) { _pts[2] = p; }

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

FOG_CLASS_PRECISION_F_D(RectangularGradient)

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPEINFO_DECLARE(Fog::RectangularGradientF, Fog::TYPEINFO_MOVABLE)
_FOG_TYPEINFO_DECLARE(Fog::RectangularGradientD, Fog::TYPEINFO_MOVABLE)

// [Guard]
#endif // _FOG_G2D_SOURCE_RECTANGULARGRADIENT_H
