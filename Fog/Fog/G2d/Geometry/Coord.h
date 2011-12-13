// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_GEOMETRY_COORD_H
#define _FOG_G2D_GEOMETRY_COORD_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Math/Fuzzy.h>
#include <Fog/Core/Math/Math.h>

namespace Fog {

//! @addtogroup Fog_G2d_Geometry
//! @{

// ============================================================================
// [Fog::CoordF]
// ============================================================================

//! @brief Coord (float).
#include <Fog/Core/C++/PackDWord.h>
struct FOG_NO_EXPORT CoordF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE CoordF() {}
  explicit FOG_INLINE CoordF(_Uninitialized) {}

  FOG_INLINE CoordF(const CoordF& other) :
    value(other.value),
    unit(other.unit)
  {
  }

  FOG_INLINE CoordF(float value, uint32_t unit) :
    value(value),
    unit(unit)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE void setValue(float value) { this->value = value; }
  FOG_INLINE void setUnit(uint32_t unit) { this->unit = unit; }

  FOG_INLINE float getValue() const { return value; }
  FOG_INLINE uint32_t getUnit() const { return unit; }

  FOG_INLINE void setCoord(float value, uint32_t unit)
  {
    this->value = value;
    this->unit = unit;
  }

  FOG_INLINE void setCoord(const CoordF& other)
  {
    this->value = other.value;
    this->unit = other.unit;
  }
  
  // Implemented-Later.
  FOG_INLINE void setCoord(const CoordD& other);

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const CoordF& other) const
  {
    return value == other.value && unit == other.unit;
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE CoordF& operator=(const CoordF& other) { setCoord(other); return *this; }
  FOG_INLINE CoordF& operator=(const CoordD& other) { setCoord(other); return *this; }

  FOG_INLINE bool operator==(const CoordF& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const CoordF& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  float value;
  uint32_t unit;
};
#include <Fog/Core/C++/PackRestore.h>

// ============================================================================
// [Fog::CoordD]
// ============================================================================

//! @brief Arc base (double).
struct FOG_NO_EXPORT CoordD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE CoordD() {}
  explicit FOG_INLINE CoordD(_Uninitialized) {}

  FOG_INLINE CoordD(const CoordD& other) :
    value(other.value),
    unit(other.unit)
  {
  }

  FOG_INLINE CoordD(float value, uint32_t unit) :
    value(value),
    unit(unit)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE void setValue(double value) { this->value = value; }
  FOG_INLINE void setUnit(uint32_t unit) { this->unit = unit; }

  FOG_INLINE double getValue() const { return value; }
  FOG_INLINE uint32_t getUnit() const { return unit; }

  FOG_INLINE void setCoord(double value, uint32_t unit)
  {
    this->value = value;
    this->unit = unit;
  }

  FOG_INLINE void setCoord(const CoordF& other)
  {
    this->value = double(other.value);
    this->unit = other.unit;
  }

  FOG_INLINE void setCoord(const CoordD& other)
  {
    this->value = other.value;
    this->unit = other.unit;
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const CoordD& other) const
  {
    return value == other.value && unit == other.unit;
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE CoordD& operator=(const CoordF& other) { setCoord(other); return *this; }
  FOG_INLINE CoordD& operator=(const CoordD& other) { setCoord(other); return *this; }

  FOG_INLINE bool operator==(const CoordD& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const CoordD& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  double value;
  uint32_t unit;
};

// ============================================================================
// [Implemented-Later]
// ============================================================================

FOG_INLINE void CoordF::setCoord(const CoordD& other)
{
  this->value = (float)other.value;
  this->unit = other.unit;
}

// ============================================================================
// [Fog::CoordT<>]
// ============================================================================

_FOG_NUM_T(Coord)
_FOG_NUM_F(Coord)
_FOG_NUM_D(Coord)

//! @}

} // Fog namespace

// ============================================================================
// [Fog::Fuzzy<>]
// ============================================================================

FOG_FUZZY_DECLARE_F(Fog::CoordF, Math::isFuzzyEq(a.value, b.value, epsilon) && a.unit == b.unit)
FOG_FUZZY_DECLARE_D(Fog::CoordD, Math::isFuzzyEq(a.value, b.value, epsilon) && a.unit == b.unit)

// [Guard]
#endif // _FOG_G2D_GEOMETRY_COORD_H
