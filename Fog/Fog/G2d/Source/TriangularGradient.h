// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_SOURCE_TRIANGULARGRADIENT_H
#define _FOG_G2D_SOURCE_TRIANGULARGRADIENT_H

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

struct TriangularGradientF;
struct TriangularGradientD;

// ============================================================================
// [Fog::TriangularGradientF]
// ============================================================================

//! @brief Triangular gradient (float).
struct FOG_NO_EXPORT TriangularGradientF : public GradientF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE TriangularGradientF() : GradientF(GRADIENT_TYPE_TRIANGULAR)
  {
  }

  FOG_INLINE TriangularGradientF(const TriangularGradientF& other);
  explicit FOG_INLINE TriangularGradientF(const TriangularGradientD& other);

  FOG_INLINE TriangularGradientF(
    const PointF& first, const PointF& second,
    const PointF& third, const PointF& focal) : 
    GradientF(GRADIENT_TYPE_TRIANGULAR)
  {
    _pts[0] = first;
    _pts[1] = second;
    _pts[2] = third;
    _pts[3] = focal;
  }

  FOG_INLINE ~TriangularGradientF()
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setGradient(const TriangularGradientF& other);
  FOG_INLINE err_t setGradient(const TriangularGradientD& other);

  FOG_INLINE const PointF& getFirst() const { return _pts[0]; }
  FOG_INLINE const PointF& getSecond() const { return _pts[1]; }
  FOG_INLINE const PointF& getThird() const { return _pts[2]; }
  FOG_INLINE const PointF& getFocal() const { return _pts[3]; }

  FOG_INLINE void setFirst(const PointF& p) { _pts[0] = p; }
  FOG_INLINE void setSecond(const PointF& p) { _pts[1] = p; }
  FOG_INLINE void setThird(const PointF& p) { _pts[2] = p; }
  FOG_INLINE void setFocal(const PointF& p) { _pts[3] = p; }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE TriangularGradientF& operator=(const TriangularGradientF& other) { setGradient(other); return *this; }
  FOG_INLINE TriangularGradientF& operator=(const TriangularGradientD& other) { setGradient(other); return *this; }
};

// ============================================================================
// [Fog::TriangularGradientD]
// ============================================================================

//! @brief Triangular gradient (double).
struct FOG_NO_EXPORT TriangularGradientD : public GradientD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE TriangularGradientD() : GradientD(GRADIENT_TYPE_TRIANGULAR)
  {
  }

  FOG_INLINE TriangularGradientD(const TriangularGradientF& other);
  explicit FOG_INLINE TriangularGradientD(const TriangularGradientD& other);

  FOG_INLINE TriangularGradientD(
    const PointD& first, const PointD& second, 
    const PointD& third, const PointD& focal) : 
    GradientD(GRADIENT_TYPE_TRIANGULAR)
  {
    _pts[0] = first;
    _pts[1] = second;
    _pts[2] = third;
    _pts[3] = focal;
  }

  FOG_INLINE ~TriangularGradientD()
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setGradient(const TriangularGradientF& other);
  FOG_INLINE err_t setGradient(const TriangularGradientD& other);

  FOG_INLINE const PointD& getFirst() const { return _pts[0]; }
  FOG_INLINE const PointD& getSecond() const { return _pts[1]; }
  FOG_INLINE const PointD& getThird() const { return _pts[2]; }
  FOG_INLINE const PointD& getFocal() const { return _pts[3]; }

  FOG_INLINE void setFirst(const PointD& p) { _pts[0] = p; }
  FOG_INLINE void setSecond(const PointD& p) { _pts[1] = p; }
  FOG_INLINE void setThird(const PointD& p) { _pts[2] = p; }
  FOG_INLINE void setFocal(const PointD& p) { _pts[3] = p; }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE TriangularGradientD& operator=(const TriangularGradientF& other) { setGradient(other); return *this; }
  FOG_INLINE TriangularGradientD& operator=(const TriangularGradientD& other) { setGradient(other); return *this; }
};

// ============================================================================
// [Implemented Later]
// ============================================================================

FOG_INLINE TriangularGradientF::TriangularGradientF(const TriangularGradientF& other) : GradientF(other) {}
FOG_INLINE TriangularGradientF::TriangularGradientF(const TriangularGradientD& other) : GradientF(other) {}

FOG_INLINE TriangularGradientD::TriangularGradientD(const TriangularGradientF& other) : GradientD(other) {}
FOG_INLINE TriangularGradientD::TriangularGradientD(const TriangularGradientD& other) : GradientD(other) {}

FOG_INLINE err_t TriangularGradientF::setGradient(const TriangularGradientF& other) { return GradientF::setGradient(other); }
FOG_INLINE err_t TriangularGradientF::setGradient(const TriangularGradientD& other) { return GradientF::setGradient(other); }

FOG_INLINE err_t TriangularGradientD::setGradient(const TriangularGradientF& other) { return GradientD::setGradient(other); }
FOG_INLINE err_t TriangularGradientD::setGradient(const TriangularGradientD& other) { return GradientD::setGradient(other); }

//! @}

// ============================================================================
// [Fog::TriangularGradientT<>]
// ============================================================================

FOG_TYPEVARIANT_DECLARE_F_D(TriangularGradient)

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::TriangularGradientF, Fog::TYPEINFO_MOVABLE)
FOG_DECLARE_TYPEINFO(Fog::TriangularGradientD, Fog::TYPEINFO_MOVABLE)

// [Guard]
#endif // _FOG_G2D_SOURCE_TRIANGULARGRADIENT_H
