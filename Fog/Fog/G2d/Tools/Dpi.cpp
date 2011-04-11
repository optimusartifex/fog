// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Memory/Memory.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/G2d/Tools/Dpi.h>

namespace Fog {

// ============================================================================
// [Fog::Glyph]
// ============================================================================

static inline void _setupDpi(float* data, float dpi, float em, float ex)
{
  data[DPI_VALUE_CM] = dpi * 0.393700777778f;
  data[DPI_VALUE_EM] = em;
  data[DPI_VALUE_EX] = ex;
  data[DPI_VALUE_MM] = dpi * 0.039370077778f;
  data[DPI_VALUE_PC] = dpi * 0.166666666667f;
  data[DPI_VALUE_PT] = dpi * 0.013888888889f;
  data[DPI_VALUE_PX] = 1.0f;
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

err_t Dpi::setDpi(float dpi, float em, float ex)
{
  if (dpi <= 0.0f || em <= 0.0f || ex <= 0.0f) return ERR_RT_INVALID_ARGUMENT;
  _setupDpi(_data, dpi, em, ex);

  return ERR_OK;
}

void Dpi::reset()
{
  _setupDpi(_data, 72.0f, 12.0f, 6.0f);
}

float Dpi::toDeviceSpace(float value, uint32_t valueType) const
{
  if (FOG_UNLIKELY((uint32_t)valueType >= DPI_VALUE_COUNT)) return 0.0f;

  return value * _data[valueType];
}

float Dpi::fromDeviceSpace(float value, uint32_t valueType) const
{
  if (FOG_UNLIKELY((uint32_t)valueType >= DPI_VALUE_COUNT)) return 0.0f;

  return value / _data[valueType];
}

Dpi& Dpi::operator=(const Dpi& other)
{
  memcpy(_data, other._data, sizeof(_data));
  return *this;
}

} // Fog namespace
