// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/G2d/Tools/Dpi.h>

namespace Fog {

// ============================================================================
// [Fog::Dpi - Helpers]
// ============================================================================

static inline void _Dpi_setup(float* data, float dpi, float em, float ex)
{
  data[UNIT_NONE] = 1.0f;
  data[UNIT_PX] = 1.0f;

  data[UNIT_PT] = dpi * 0.013888888889f;
  data[UNIT_PC] = dpi * 0.166666666667f;

  data[UNIT_IN] = dpi;
  data[UNIT_MM] = dpi * 0.039370077778f;
  data[UNIT_CM] = dpi * 0.393700777778f;

  data[UNIT_PERCENTAGE] = 1.0f;

  data[UNIT_EM] = em;
  data[UNIT_EX] = ex;
}

// ============================================================================
// [Fog::Dpi - Construction / Destruction]
// ============================================================================

Dpi::Dpi()
{
  reset();
}

Dpi::Dpi(float dpi)
{
  if (dpi <= 0.0f) { reset(); return; }

  _Dpi_setup(_data, dpi, 0.0f, 0.0f);
  resetEmEx();
}

Dpi::Dpi(const Dpi& other)
{
  memcpy(_data, other._data, sizeof(_data));
}

Dpi::~Dpi()
{
}

// ============================================================================
// [Fog::Dpi - Accessors]
// ============================================================================

err_t Dpi::setDpi(float dpi)
{
  if (dpi <= 0.0f) return ERR_RT_INVALID_ARGUMENT;
  _Dpi_setup(_data, dpi, 0.0f, 0.0f);

  resetEmEx();
  return ERR_OK;
}

err_t Dpi::setDpi(float dpi, float em, float ex)
{
  if (dpi <= 0.0f) return ERR_RT_INVALID_ARGUMENT;
  _Dpi_setup(_data, dpi, em, ex);

  if (em <= 0.0f || ex <= 0.0f) resetEmEx();
  return ERR_OK;
}

// ============================================================================
// [Fog::Dpi - Reset]
// ============================================================================

void Dpi::reset()
{
  _Dpi_setup(_data, 72.0f, 12.0f, 6.0f);
}

// ============================================================================
// [Fog::Dpi - Operator Overload]
// ============================================================================

Dpi& Dpi::operator=(const Dpi& other)
{
  memcpy(_data, other._data, sizeof(_data));
  return *this;
}

} // Fog namespace
