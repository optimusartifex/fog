// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Assert.h>
#include <Fog/Core/Atomic.h>
#include <Fog/Core/Memory.h>
#include <Fog/Graphics/Dpi.h>

namespace Fog {

// ============================================================================
// [Fog::Glyph]
// ============================================================================

static inline void _setupDpi(double* data, double dpi, double em, double ex)
{
  data[DPI_VALUE_CM] = dpi * 0.3937007777777778;
  data[DPI_VALUE_EM] = em;
  data[DPI_VALUE_EX] = ex;
  data[DPI_VALUE_MM] = dpi * 0.0393700777777778;
  data[DPI_VALUE_PC] = dpi * 0.1666666666666667;
  data[DPI_VALUE_PT] = dpi * 0.0138888888888889;
  data[DPI_VALUE_PX] = 1.0;
}

Dpi::Dpi()
{
  reset();
}

Dpi::Dpi(const Dpi& other)
{
  memcpy(_data, other._data, sizeof(_data));
}

Dpi::~Dpi()
{
}

err_t Dpi::setDpi(double dpi, double em, double ex)
{
  if (dpi <= 0.0 || em <= 0.0 || ex <= 0.0) return ERR_RT_INVALID_ARGUMENT;
  _setupDpi(_data, dpi, em, ex);

  return ERR_OK;
}

void Dpi::reset()
{
  _setupDpi(_data, 72.0, 12.0, 6.0);
}

double Dpi::toDeviceSpace(double value, uint32_t valueType) const
{
  if ((uint32_t)valueType >= DPI_VALUE_COUNT) return 0.0;

  return value * _data[valueType];
}

double Dpi::fromDeviceSpace(double value, uint32_t valueType) const
{
  if ((uint32_t)valueType >= DPI_VALUE_COUNT) return 0.0;

  return value / _data[valueType];
}

Dpi& Dpi::operator=(const Dpi& other)
{
  memcpy(_data, other._data, sizeof(_data));
  return *this;
}

} // Fog namespace
