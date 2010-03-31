// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_DPI_H
#define _FOG_GRAPHICS_DPI_H

// [Dependencies]
#include <Fog/Core/Atomic.h>
#include <Fog/Graphics/Constants.h>

//! @addtogroup Fog_Graphics
//! @{

namespace Fog {

// ============================================================================
// [Fog::Dpi]
// ============================================================================

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

  FOG_INLINE double getDpi() const
  {
    return _data[DPI_VALUE_IN];
  }

  FOG_INLINE double getValue(uint32_t valueType)
  {
    FOG_ASSERT(valueType < DPI_VALUE_COUNT);
    return _data[valueType];
  }

  err_t setDpi(double dpi, double em, double ex);
  void reset();

  double toDeviceSpace(double value, uint32_t valueType) const;
  double fromDeviceSpace(double value, uint32_t valueType) const;

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  Dpi& operator=(const Dpi& other);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief DPI translator data.
  double _data[DPI_VALUE_COUNT];
};

} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_GRAPHICS_DPI_H
