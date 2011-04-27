// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TOOLS_DPI_H
#define _FOG_G2D_TOOLS_DPI_H

// [Dependencies]
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/G2d/Global/Constants.h>

namespace Fog {

//! @addtogroup Fog_G2d_Tools
//! @{

// ============================================================================
// [Fog::Dpi]
// ============================================================================

//! @brief DPI value and converter.
struct FOG_API Dpi
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  Dpi();
  Dpi(const Dpi& other);
  explicit Dpi(float dpi);
  ~Dpi();

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE float getDpi() const
  {
    return _data[COORD_UNIT_IN];
  }

  FOG_INLINE float getValue(uint32_t coordUnit)
  {
    FOG_ASSERT(valueType < COORD_UNIT_COUNT);
    return _data[coordUnit];
  }

  err_t setDpi(float dpi);
  err_t setDpi(float dpi, float em, float ex);

  FOG_INLINE void setEmEx(float em, float ex)
  {
    _data[COORD_UNIT_EM] = em;
    _data[COORD_UNIT_EX] = ex;
  }

  FOG_INLINE void resetEmEx()
  {
    _data[COORD_UNIT_EM] = _data[COORD_UNIT_PC];
    _data[COORD_UNIT_EX] = _data[COORD_UNIT_PC];
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  void reset();

  // --------------------------------------------------------------------------
  // [Conversion]
  // --------------------------------------------------------------------------

  FOG_INLINE float toDeviceSpace(float value, uint32_t coordUnit) const
  {
    FOG_ASSERT(coordUnit < COORD_UNIT_COUNT);
    return value * _data[coordUnit];
  }

  FOG_INLINE float fromDeviceSpace(float value, uint32_t coordUnit) const
  {
    FOG_ASSERT(coordUnit < COORD_UNIT_COUNT);
    return value / _data[coordUnit];
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  Dpi& operator=(const Dpi& other);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief DPI translator data.
  float _data[COORD_UNIT_COUNT];
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::Dpi, Fog::TYPEINFO_MOVABLE)

// [Guard]
#endif // _FOG_G2D_TOOLS_DPI_H
