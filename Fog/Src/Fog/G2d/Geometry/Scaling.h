// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_GEOMETRY_SCALING_H
#define _FOG_G2D_GEOMETRY_SCALING_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Math/Fuzzy.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/G2d/Geometry/Point.h>

namespace Fog {

//! @addtogroup Fog_G2d_Geometry
//! @{

// ============================================================================
// [Fog::ScalingF]
// ============================================================================

//! @brief Scaling (float).
struct FOG_NO_EXPORT ScalingF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE ScalingF()
  {
    reset();
  }

  FOG_INLINE ScalingF(const ScalingF& other)
  {
    setScaling(other);
  }

  explicit FOG_INLINE ScalingF(_Uninitialized) {}
  explicit FOG_INLINE ScalingF(const ScalingD& other) { setScaling(other); }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const PointF& getPoint() const { return _pt; }
  FOG_INLINE void setPoint(const PointF& pt) { _pt = pt; }

  FOG_INLINE uint32_t isSwapped() const { return _swapped; }
  FOG_INLINE void setSwapped(uint32_t swapped) { _swapped = swapped; }

  FOG_INLINE void setScaling(const ScalingF& other)
  {
    _pt = other._pt;
    _swapped = other._swapped;
  }

  // Implemented-Later.
  FOG_INLINE void setScaling(const ScalingD& other);

  FOG_INLINE void setScaling(const PointF& pt, uint32_t swapped)
  {
    _pt = pt;
    _swapped = swapped;
  }

  FOG_INLINE void setScaling(float x, float y, uint32_t swapped)
  {
    _pt.set(x, y);
    _swapped = swapped;
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _pt.set(1.0f, 1.0f);
    _swapped = 0;
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE ScalingF& operator=(const ScalingF& other) { setScaling(other); return *this; }
  FOG_INLINE ScalingF& operator=(const ScalingD& other) { setScaling(other); return *this; }

  FOG_INLINE bool operator==(const ScalingF& other) const { return  MemOps::eq_t<ScalingF>(this, &other); }
  FOG_INLINE bool operator!=(const ScalingF& other) const { return !MemOps::eq_t<ScalingF>(this, &other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  PointF _pt;
  uint32_t _swapped;
};

// ============================================================================
// [Fog::ScalingD]
// ============================================================================

//! @brief Scaling (double).
struct FOG_NO_EXPORT ScalingD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE ScalingD()
  {
    reset();
  }

  FOG_INLINE ScalingD(const ScalingD& other)
  {
    setScaling(other);
  }

  explicit FOG_INLINE ScalingD(_Uninitialized) {}
  explicit FOG_INLINE ScalingD(const ScalingF& other) { setScaling(other); }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const PointD& getPoint() const { return _pt; }
  FOG_INLINE void setPoint(const PointD& pt) { _pt = pt; }

  FOG_INLINE uint32_t isSwapped() const { return _swapped; }
  FOG_INLINE void setSwapped(uint32_t swapped) { _swapped = swapped; }

  FOG_INLINE void setScaling(const ScalingD& other)
  {
    _pt = other._pt;
    _swapped = other._swapped;
  }

  FOG_INLINE void setScaling(const ScalingF& other)
  {
    _pt = other._pt;
    _swapped = other._swapped;
  }

  FOG_INLINE void setScaling(const PointD& pt, uint32_t swapped)
  {
    _pt = pt;
    _swapped = swapped;
  }

  FOG_INLINE void setScaling(double x, double y, uint32_t swapped)
  {
    _pt.set(x, y);
    _swapped = swapped;
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _pt.set(1.0, 1.0);
    _swapped = 0;
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE ScalingD& operator=(const ScalingF& other) { setScaling(other); return *this; }
  FOG_INLINE ScalingD& operator=(const ScalingD& other) { setScaling(other); return *this; }

  FOG_INLINE bool operator==(const ScalingD& other) const { return  MemOps::eq_t<ScalingD>(this, &other); }
  FOG_INLINE bool operator!=(const ScalingD& other) const { return !MemOps::eq_t<ScalingD>(this, &other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  PointD _pt;
  uint32_t _swapped;
};

// ============================================================================
// [Implemented-Later]
// ============================================================================

FOG_INLINE void ScalingF::setScaling(const ScalingD& other)
{
  _pt = other._pt;
  _swapped = other._swapped;
}

// ============================================================================
// [Fog::ScalingT<>]
// ============================================================================

_FOG_NUM_T(Scaling)
_FOG_NUM_F(Scaling)
_FOG_NUM_D(Scaling)

//! @}

} // Fog namespace

// ============================================================================
// [Fog::Fuzzy<>]
// ============================================================================

FOG_FUZZY_DECLARE_F_VEC(Fog::ScalingF, 2);
FOG_FUZZY_DECLARE_D_VEC(Fog::ScalingD, 2);

// [Guard]
#endif // _FOG_G2D_GEOMETRY_SCALING_H
