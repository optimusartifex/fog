// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TOOLS_DPI_H
#define _FOG_G2D_TOOLS_DPI_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>

namespace Fog {

//! @addtogroup Fog_G2d_Tools
//! @{

// ============================================================================
// [Fog::Dpi]
// ============================================================================

//! @brief DPI value and converter.
struct FOG_NO_EXPORT Dpi
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE Dpi()
  {
    _api.dpi.reset(this);
  }

  FOG_INLINE Dpi(const Dpi& other)
  {
    _api.dpi.copy(this, &other);
  }

  FOG_INLINE explicit Dpi(float dpi)
  {
    _api.dpi.setDpi(this, dpi);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE float getDpi() const
  {
    return _data[UNIT_IN];
  }

  FOG_INLINE float getValue(uint32_t unit) const
  {
    FOG_ASSERT(unit < UNIT_COUNT);
    return _data[unit];
  }

  FOG_INLINE err_t setDpi(float dpi)
  {
    return _api.dpi.setDpi(this, dpi);
  }

  FOG_INLINE err_t setDpi(float dpi, float em, float ex)
  {
    return _api.dpi.setDpiEmEx(this, dpi, em, ex);
  }

  FOG_INLINE void setEmEx(float em, float ex)
  {
    _data[UNIT_EM] = em;
    _data[UNIT_EX] = ex;
  }

  FOG_INLINE void resetEmEx()
  {
    _data[UNIT_EM] = _data[UNIT_PC];
    _data[UNIT_EX] = _data[UNIT_PC];
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _api.dpi.reset(this);
  }

  // --------------------------------------------------------------------------
  // [Conversion]
  // --------------------------------------------------------------------------

  FOG_INLINE float toDeviceSpace(float coord, uint32_t unit) const
  {
    FOG_ASSERT(unit < UNIT_COUNT);
    return coord * _data[unit];
  }

  FOG_INLINE float fromDeviceSpace(float coord, uint32_t unit) const
  {
    FOG_ASSERT(unit < UNIT_COUNT);
    return coord / _data[unit];
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE Dpi& operator=(const Dpi& other)
  {
    _api.dpi.copy(this, &other);
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Translation data.
  float _data[UNIT_COUNT];
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TOOLS_DPI_H
