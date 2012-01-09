// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_SOURCE_LINEARGRADIENT_H
#define _FOG_G2D_SOURCE_LINEARGRADIENT_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/G2d/Source/Gradient.h>

namespace Fog {

//! @addtogroup Fog_G2d_Source
//! @{

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

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_SOURCE_LINEARGRADIENT_H
