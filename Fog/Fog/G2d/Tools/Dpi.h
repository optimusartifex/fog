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

//! @brief DPI conversion.
struct FOG_API Dpi
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  Dpi();
  Dpi(const Dpi& other);
  ~Dpi();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  FOG_INLINE float getDpi() const
  {
    return _data[DPI_VALUE_IN];
  }

  FOG_INLINE float getValue(uint32_t valueType)
  {
    FOG_ASSERT(valueType < DPI_VALUE_COUNT);
    return _data[valueType];
  }

  err_t setDpi(float dpi, float em, float ex);
  void reset();

  float toDeviceSpace(float value, uint32_t valueType) const;
  float fromDeviceSpace(float value, uint32_t valueType) const;

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  Dpi& operator=(const Dpi& other);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief DPI translator data.
  float _data[DPI_VALUE_COUNT];
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::Dpi, Fog::TYPEINFO_MOVABLE)

// [Guard]
#endif // _FOG_G2D_TOOLS_DPI_H
